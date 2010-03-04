/*******************************************************************************
 * Copyright (c) 2009-04-24 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include "BehaviorTreeScene.h"
#include "BehaviorTreeNode.h"
#include "BehaviorTreeTree.h"
#include "BehaviorTreeInclude.h"
#include "../NodeToNodeArrow.h"
#include "../btree_callbacks.h"
#include "../x-node.h"
#include <btree/btree.h>
#include <other/lookup3.h>
#include <malloc.h>

#include <QtGui/QtGui>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

const float g_NodeWidth = 256.0f;
const float g_NodeHeight = 256.0f;
const float g_HoriSpace = 64.0f;
const float g_VertSpace = 128.0f;

BehaviorTreeScene::BehaviorTreeScene( QMainWindow* mw )
  : m_TreeContext( 0x0 )
  , m_FullContext( 0x0 )
  , m_MainWindow( mw )
  , m_DragItem( 0x0 )
{
  Allocator a;
  a.m_Alloc     = &allocate_memory;
  a.m_Free      = &free_memory;
  m_TreeContext = BehaviorTreeContextCreate( a );
}

BehaviorTreeScene::~BehaviorTreeScene()
{
  BehaviorTreeContextDestroy( m_TreeContext );
  m_TreeContext = (BehaviorTreeContext)0xdeadbeef;
  BehaviorTreeContextDestroy( m_FullContext );
  m_FullContext = (BehaviorTreeContext)0xdeadbeef;
}

void BehaviorTreeScene::dragEnterEvent( QDragEnterEvent *event )
{
  bool accept = false;
  if( event->mimeData()->hasFormat("ctstudio/x-node") && !m_DragItem )
  {
    QByteArray pieceData = event->mimeData()->data("ctstudio/x-node");
    XNodeData node_data;
    memcpy( &node_data, pieceData.constData(), sizeof( XNodeData ) );
    setupDrag( node_data );
    m_DragItem->dragBegin();
    accept = true;
  }
  else if( event->mimeData()->hasFormat( "text/uri-list" ) && !m_DragItem )
  {
    QList<QUrl> urls = event->mimeData()->urls();
    if( !urls.isEmpty() )
    {
      QString fileName = urls.first().toLocalFile();
      if( !fileName.isEmpty() )
      {
        Include* i = BehaviorTreeContextCreateInclude( m_TreeContext, fileName.toAscii().constData() );
        if( i )
        {
          m_DragItem = new BehaviorTreeInclude( i );
          i->m_UserData = m_DragItem;
          addItem( m_DragItem );
          accept = true;
        }
      }
    }
  }

  if( accept )
    event->accept();
  else
    event->ignore();
}

void BehaviorTreeScene::dragLeaveEvent( QDragLeaveEvent *event )
{
  if( m_DragItem )
  {
    m_DragItem->destroyResources( m_TreeContext );
    delete m_DragItem;
    m_DragItem = 0x0;
    event->accept();
  }
  else
    event->ignore();
}

void BehaviorTreeScene::dragMoveEvent( QDragMoveEvent *event, const QPointF& mapped_pos )
{
  if( m_DragItem )
  {
    m_DragItem->setPos( mapped_pos );
    m_DragItem->dragMove();
    event->accept();
  }
  else
    event->ignore();
}

void BehaviorTreeScene::dropEvent( QDropEvent* event )
{
  if( m_DragItem )
  {
    if( m_DragItem->validForDrop() )
    {
      m_DragItem->dragEnd();
      m_DragItem->setVisible( true );
      layout();
      emit modified();
      if( m_DragItem->isType( BehaviorTreeIncludeType ) )
        updateClone();
    }
    else
    {
      m_DragItem->destroyResources( m_TreeContext );
      delete m_DragItem;
    }

    m_DragItem = 0x0;
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

bool BehaviorTreeScene::readFile( const QString& qt_filename )
{
  BehaviorTreeContextDestroy( m_TreeContext );
  m_TreeContext = 0x0;

  Allocator a;
  a.m_Alloc = &allocate_memory;
  a.m_Free = &free_memory;
  m_TreeContext = BehaviorTreeContextCreate( a );

  QFileInfo fi( qt_filename );
  QFile f( qt_filename );

  if( !f.open( QFile::ReadOnly ) )
    return false;

  ParsingInfo pi;
  pi.m_File = &f;
  pi.m_FileName = &fi;

  ParserContextFunctions pcf;
  pcf.m_Read = &read_file;
  pcf.m_Error = &parser_error;
  pcf.m_Warning = &parser_warning;
  pcf.m_Translate = &parser_translate_include;

  ParserContext pc = ParserContextCreate( m_TreeContext );
  ParserContextSetExtra( pc, &pi );
  int returnCode = Parse( pc, &pcf );
  ParserContextDestroy( pc );

  updateClone();

  if( returnCode != 0 )
    return false;

  clear();

  createGraphics();
  layout();

  return true;
}

bool BehaviorTreeScene::writeFile( const QString& qt_filename )
{
  QFileInfo fi( qt_filename );
  QFile f( qt_filename );

  if( !f.open( QFile::WriteOnly ) )
    return false;

  SavingInfo si;
  si.m_File = &f;
  si.m_FileName = &fi;

  SaverContextFunctions scf;
  scf.m_Write = &write_file;
  scf.m_Translate = &saver_translate_include;

  SaverContext sc = SaverContextCreate( m_TreeContext );
  SaverContextSetExtra( sc, &si );
  Save( sc, &scf );
  SaverContextDestroy( sc );

  return true;
}

void BehaviorTreeScene::layout()
{
  if( !m_TreeContext )
    return;

  ExtentsList el;
  int i, c;
  NamedSymbol* s = BehaviorTreeContextAccessSymbols( m_TreeContext, &c );
  for( i = 0; i < c; ++i )
  {
    if( s[i].m_Type != E_ST_TREE || !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    layoutRoot( (BehaviorTreeSceneItem*)(s[i].m_Symbol.m_Tree->m_UserData), el );
  }
  Include* inc = BehaviorTreeContextGetFirstInclude( m_TreeContext );
  float increment_pos = 0.0f;
  while( inc )
  {
    BehaviorTreeSceneItem* inc_si = (BehaviorTreeSceneItem*)inc->m_UserData;
    inc_si->setPos( increment_pos, 0 );
    inc = inc->m_Next;

    QRectF bounds = inc_si->boundingRect();
    increment_pos += (float)bounds.width() + g_HoriSpace;
  }

  setSceneRect( itemsBoundingRect() );
}

void BehaviorTreeScene::itemModified()
{
  emit modified();
}

void BehaviorTreeScene::createGraphics()
{
  int i, c;
  NamedSymbol* s = BehaviorTreeContextAccessSymbols( m_TreeContext, &c );
  for( i = 0; i < c; ++i )
  {
    if( s[i].m_Type != E_ST_TREE || !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    BehaviorTreeSceneItem* tree = new BehaviorTreeTree( s[i].m_Symbol.m_Tree );
    s[i].m_Symbol.m_Tree->m_UserData = tree;
    addItem( tree );
    connect( tree, SIGNAL(modified()), this, SLOT( itemModified() ) );
    createGraphics( s[i].m_Symbol.m_Tree->m_Root, tree );
  }

  Include* inc = BehaviorTreeContextGetFirstInclude( m_TreeContext );
  float increment_pos = 0;
  while( inc )
  {
    BehaviorTreeSceneItem* inc_si = new BehaviorTreeInclude( inc );
    addItem( inc_si );
    inc_si->setPos( increment_pos, 0 );
    inc->m_UserData = inc_si;
    inc = inc->m_Next;

    QRectF bounds = inc_si->boundingRect();
    increment_pos += (float)bounds.width() + g_HoriSpace;
  }

}

void BehaviorTreeScene::createGraphics( Node* n, BehaviorTreeSceneItem* parent )
{
  while( n )
  {
    BehaviorTreeNode* svg_item = new BehaviorTreeNode( n, parent );

    connect( svg_item, SIGNAL( itemDragged() ), this, SLOT( layout() ) );
    connect( svg_item, SIGNAL( modified() ), this, SLOT( itemModified() ) );

    connect(
      svg_item,
      SIGNAL( relinkTargetMessage( QString, int ) ),
      m_MainWindow->statusBar(),
      SLOT( showMessage( QString, int ) )
    );

    if( !parent )
      addItem( svg_item );

    if( parent )
    {
      NodeToNodeArrow* a = new NodeToNodeArrow( parent, svg_item, this );
      parent->addArrow( a );
      svg_item->addArrow( a );
    }

    n->m_UserData = (void*)svg_item;
    createGraphics( GetFirstChild( n ), svg_item );
    n = n->m_Next;
  }
}

void BehaviorTreeScene::layoutRoot( BehaviorTreeSceneItem* n, ExtentsList& el )
{
  while( n )
  {
    ExtentsList t;
    depthFirstPlace( n, t );
    padExtents( el, t );
    double slide = minimumRootDistance( el, t );
    n->moveBy( slide, g_NodeHeight + g_VertSpace );
    moveExtents( t, slide );
    mergeExtents( el, el, t );
    n = n->nextSibling();
  }
}

void BehaviorTreeScene::depthFirstPlace( BehaviorTreeSceneItem* n, ExtentsList& pel )
{
  ExtentsList el;
  BehaviorTreeSceneItem* it = n->firstChild();
  double lx = 0.0;
  double fr = 0.0f;
  n->setPos( 0.0f, 0.0f );

  if( n->parentItem() )
    n->moveBy( 0.0f, g_NodeHeight + g_VertSpace );

  while( it )
  {
    ExtentsList t;
    depthFirstPlace( it, t );
    double slide = minimumRootDistance( el, t );
    it->moveBy( slide, 0 );
    fr = it->pos().rx() + it->boundingRect().width();
    lx = slide;
    moveExtents( t, slide );
    mergeExtents( el, el, t );
    it = it->nextSibling();
  }

  QRectF bounds = n->boundingRect();

  it = n->firstChild();
  if( it )
  {
    lx /= 2.0;

    if( lx == 0.0 )
    {
      float nw = bounds.width();
      float cw = it->boundingRect().width();
      lx = (cw - nw) / 2.0;
    }

    while( it )
    {
      it->moveBy( -lx, 0 );
      it = it->nextSibling();
    }
    moveExtents( el, -lx );
  }

  Extents e;
  e.l = 0;
  e.r = bounds.width() + g_HoriSpace;
  pel.push_back( e );
  if( !el.empty() )
    pel.insert( pel.end(), el.begin(), el.end() );

}

double BehaviorTreeScene::minimumRootDistance( const ExtentsList& lel,
  const ExtentsList& rel )
{
  double ret = 0.0;
  size_t s = lel.size();
  if( s > rel.size() )
    s = rel.size();

  for( size_t i = 0; i < s; ++i )
  {
    const Extents& l = lel[i];
    const Extents& r = rel[i];
    double d = l.r - r.l;
    if( d > ret )
      ret = d;
  }

  return ret;
}

void BehaviorTreeScene::moveExtents( ExtentsList& el, double dist )
{
  size_t s = el.size();
  for( size_t i = 0; i < s; ++i )
  {
    el[i].l += dist;
    el[i].r += dist;
  }
}

void BehaviorTreeScene::mergeExtents( ExtentsList& r, const ExtentsList& lel,
  const ExtentsList& rel )
{
  size_t ls = lel.size();
  size_t rs = rel.size();

  if( ls < rs )
    r.resize( rs );
  else
    r.resize( ls );

  size_t s = r.size();

  for( size_t i = 0; i < s; ++i )
  {
    if( i < ls && i < rs )
    {
      const Extents& le = lel[i];
      const Extents& re = rel[i];
      r[i].l = le.l;
      r[i].r = re.r;
    }
    else if( i < ls )
      r[i] = lel[i];
    else if( i < rs )
      r[i] = rel[i];
  }
}

void BehaviorTreeScene::padExtents( ExtentsList& l, const ExtentsList& r )
{
  int ls = (int)l.size();
  int rs = (int)r.size();

  if( ls > 0 && rs > ls )
  {
    l.resize( rs );
    for( int i = ls; i < rs; ++i )
      l[i] = l[0];
  }
}

void BehaviorTreeScene::setupDrag( const XNodeData& data )
{
  switch( data.m_Type )
  {
  case E_XNDT_TREE:
    setupTreeDrag( data );
    break;
  case E_XNDT_NODE:
    setupNodeDrag( data );
    break;
  }
}

void BehaviorTreeScene::setupTreeDrag( const XNodeData& data )
{
  //First, find a name for the new tree, we try "main" first
  //but if that is taken we go with "tree_X" where X is an incrementing number.
  char tree_name_buff[128];
  sprintf( tree_name_buff, "%s", "main" );
  hash_t name_hash = hashlittle( tree_name_buff );
  NamedSymbol* ns = BehaviorTreeContextFindSymbol( m_TreeContext, name_hash );
  unsigned int c = 0;
  while( ns )
  {
    sprintf( tree_name_buff, "tree_%08u", ++c );
    name_hash = hashlittle( tree_name_buff );
    ns = BehaviorTreeContextFindSymbol( m_TreeContext, name_hash );
  }

  //When we have a name we create and setup a BehaviorTree structure for it
  //that we add to the symbol register, and we create a "drag item" in the graphics scene.

  BehaviorTree* tree = (BehaviorTree*)BehaviorTreeContextAllocateObject( m_TreeContext );
  InitBehaviorTree( tree );
  tree->m_Id.m_Text = BehaviorTreeContextRegisterString( m_TreeContext, tree_name_buff );
  tree->m_Id.m_Hash = name_hash;

  m_DragItem = new BehaviorTreeTree( tree );
  addItem( m_DragItem );

  connect( m_DragItem, SIGNAL(modified()), this, SLOT( itemModified() ) );

  tree->m_UserData = m_DragItem;
  tree->m_Declared = true;

  NamedSymbol new_symbol;
  new_symbol.m_Type = E_ST_TREE;
  new_symbol.m_Symbol.m_Tree = tree;
  BehaviorTreeContextRegisterSymbol( m_TreeContext, new_symbol );
}

void BehaviorTreeScene::setupNodeDrag( const XNodeData& data )
{
  //First, allocate a node from the tree and set it up.
  Node* node = (Node*)BehaviorTreeContextAllocateObject( m_TreeContext );
  InitNode( node );
  //Setup the node's grist type
  node->m_Grist.m_Type = (NodeGristType)data.m_NodeGrist;

  //Do grist dependent setup.
  switch( node->m_Grist.m_Type )
  {
  case E_GRIST_DECORATOR:
    setupDecoratorNode( node, data );
    break;
  case E_GRIST_ACTION:
    setupActionNode( node, data );
    break;
  case E_GRIST_UNKOWN:
  case E_GRIST_SEQUENCE:
  case E_GRIST_SELECTOR:
  case E_GRIST_PARALLEL:
  case E_GRIST_DYN_SELECTOR:
  case E_GRIST_SUCCEED:
  case E_GRIST_FAIL:
  case E_GRIST_WORK:
  case E_MAX_GRIST_TYPES:
    /* Warning killers */
    break;
  }
  // Create the graphics item.
  m_DragItem = new BehaviorTreeNode( node );
  addItem( m_DragItem );

  connect( m_DragItem, SIGNAL( itemDragged() ), this, SLOT( layout() ) );
  connect( m_DragItem, SIGNAL( modified() ), this, SLOT( itemModified() ) );

  connect(
    m_DragItem,
    SIGNAL( relinkTargetMessage( QString, int ) ),
    m_MainWindow->statusBar(),
    SLOT( showMessage( QString, int ) )
  );

  node->m_UserData = m_DragItem;
}

void BehaviorTreeScene::setupDecoratorNode( Node* n, const XNodeData& xnd )
{
  NamedSymbol* ns;

  ns = BehaviorTreeContextFindSymbol( m_FullContext, xnd.m_FuncId );
  Decorator* declared = ns->m_Symbol.m_Decorator;
  n->m_Grist.m_Decorator.m_Arguments = CloneVariableList( m_TreeContext, declared->m_Args );
  Variable* v = n->m_Grist.m_Decorator.m_Arguments;
  while( v )
  {
    v->m_ValueSet = true;
    v = v->m_Next;
  }
  ns = BehaviorTreeContextFindSymbol( m_TreeContext, xnd.m_FuncId );
  Decorator* partial = 0x0;
  if( !ns )
  {
    NamedSymbol new_sym;
    new_sym.m_Type = E_ST_DECORATOR;
    new_sym.m_Symbol.m_Decorator = (Decorator*)BehaviorTreeContextAllocateObject( m_TreeContext );
    partial = new_sym.m_Symbol.m_Decorator;
    InitDecorator( partial );
    CloneId( m_TreeContext, &partial->m_Id, &declared->m_Id );
    BehaviorTreeContextRegisterSymbol( m_TreeContext, new_sym );
  }
  else
  {
    partial = ns->m_Symbol.m_Decorator;
  }

  n->m_Grist.m_Decorator.m_Decorator = partial;

}

void BehaviorTreeScene::setupActionNode( Node* n, const XNodeData& xnd )
{
  NamedSymbol* ns;

  ns = BehaviorTreeContextFindSymbol( m_FullContext, xnd.m_FuncId );
  Action* declared = ns->m_Symbol.m_Action;
  n->m_Grist.m_Action.m_Arguments = CloneVariableList( m_TreeContext, declared->m_Args );

  Variable* v = n->m_Grist.m_Action.m_Arguments;
  while( v )
  {
    v->m_ValueSet = true;
    v = v->m_Next;
  }
  ns = BehaviorTreeContextFindSymbol( m_TreeContext, xnd.m_FuncId );
  Action* partial = 0x0;
  if( !ns )
  {
    NamedSymbol new_sym;
    new_sym.m_Type = E_ST_ACTION;
    new_sym.m_Symbol.m_Action = (Action*)BehaviorTreeContextAllocateObject( m_TreeContext );
    partial = new_sym.m_Symbol.m_Action;
    InitAction( partial );
    CloneId( m_TreeContext, &partial->m_Id, &declared->m_Id );
    BehaviorTreeContextRegisterSymbol( m_TreeContext, new_sym );
  }
  else
  {
    partial = ns->m_Symbol.m_Action;
  }
  n->m_Grist.m_Action.m_Action = partial;
}

void BehaviorTreeScene::updateClone()
{
  BehaviorTreeContextDestroy( m_FullContext );
  m_FullContext = BehaviorTreeContextClone( m_TreeContext );

  ParsingInfo pi;

  ParserContextFunctions pcf;
  pcf.m_Read = &read_file;
  pcf.m_Error = &parser_error;
  pcf.m_Warning = &parser_warning;
  pcf.m_Translate = &parser_translate_include;

  Include* include = BehaviorTreeContextGetFirstInclude( m_FullContext );
  while( include )
  {
    QFileInfo fi( include->m_Name );
    QFile f( include->m_Name );

    pi.m_File = &f;
    pi.m_FileName = &fi;

    if( !pi.m_File->open( QFile::ReadOnly ) )
      break;

    ParserContext pc = ParserContextCreate( m_FullContext );
    ParserContextSetExtra( pc, &pi );
    ParserContextSetCurrent( pc, include->m_Name );
    int returnCode = Parse( pc, &pcf );
    ParserContextDestroy( pc );

    if( returnCode != 0 )
      break;

    include = include->m_Next;
  }

  emit updatedSymbols( m_FullContext );
}
