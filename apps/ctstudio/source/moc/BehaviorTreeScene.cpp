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
  m_TreeContext = create_bt_context( a );
  updateClone();
}

BehaviorTreeScene::~BehaviorTreeScene()
{
  destroy( m_TreeContext );
  m_TreeContext = (BehaviorTreeContext)0xdeadbeef;
  destroy( m_FullContext );
  m_FullContext = (BehaviorTreeContext)0xdeadbeef;
}

void BehaviorTreeScene::newTree()
{
  clear();

  destroy( m_TreeContext );
  m_TreeContext = 0x0;

  Allocator a;
  a.m_Alloc     = &allocate_memory;
  a.m_Free      = &free_memory;
  m_TreeContext = create_bt_context( a );
  updateClone();
}

bool BehaviorTreeScene::readFile( const QString& qt_filename )
{
  clear();

  destroy( m_TreeContext );
  m_TreeContext = 0x0;

  Allocator a;
  a.m_Alloc = &allocate_memory;
  a.m_Free = &free_memory;
  m_TreeContext = create_bt_context( a );

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

  ParserContext pc = create_parser_context( m_TreeContext );
  set_extra( pc, &pi );
  int returnCode = parse( pc, &pcf );
  destroy( pc );

  updateClone();

  if( returnCode != 0 )
    return false;

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

  SaverContext sc = create_saver_context( m_TreeContext );
  set_extra( sc, &si );
  save( sc, &scf );
  destroy( sc );

  return true;
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
        Include* i = create_include( m_TreeContext, fileName.toAscii().constData() );
        if( i )
        {
          m_DragItem = new BehaviorTreeInclude( m_TreeContext, i );
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
    m_DragItem->dragFail();
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
      layout();
      emit modified();
      if( m_DragItem->isType( BehaviorTreeIncludeType ) )
        updateClone();
    }
    else
    {
      m_DragItem->dragFail();
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

void BehaviorTreeScene::layout()
{
  if( !m_TreeContext )
    return;

  ExtentsList el;
  int i, c;
  NamedSymbol* s = access_symbols( m_TreeContext, &c );
  for( i = 0; i < c; ++i )
  {
    if( s[i].m_Type != E_ST_TREE || !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    layoutRoot( (BehaviorTreeSceneItem*)(s[i].m_Symbol.m_Tree->m_UserData), el );
  }
  Include* inc = get_first_include( m_TreeContext );
  float increment_pos = 0.0f;
  while( inc )
  {
    BehaviorTreeSceneItem* inc_si = (BehaviorTreeSceneItem*)inc->m_UserData;
    inc_si->setPos( increment_pos, 0 );
    inc = inc->m_Next;

    QRectF bounds = inc_si->boundingRect();
    increment_pos += (float)bounds.width() + g_HoriSpace;
  }

  QRectF sr( itemsBoundingRect() );
  sr.adjust( -256.0, -256.0, 256.0, 256.0 );
  setSceneRect( sr );
}

void BehaviorTreeScene::itemModified( bool geometry_changed )
{
  emit modified();

  if( geometry_changed )
    layout();
}

void BehaviorTreeScene::itemSymbolChanged( unsigned int hash_id )
{
  typedef QList<QGraphicsItem *> ItemList;
  ItemList children( items() );
  ItemList::iterator it, it_e = children.end();
  for( it = children.begin() ; it != it_e; ++it )
  {
    BehaviorTreeSceneItem* btsi = qgraphicsitem_cast<BehaviorTreeSceneItem*>( *it );
    if( !btsi )
      continue;
    btsi->symbolChanged( hash_id );
  }
  emit modified();
}

void BehaviorTreeScene::deleteSelected()
{
  typedef QList<QGraphicsItem *> ItemList;
  ItemList items( selectedItems() );
  ItemList::iterator it, it_e = items.end();

  bool itemDestroyed = false;
  bool includeDestroyed = false;

  for( it = items.begin(); it != it_e; ++it )
  {
    BehaviorTreeSceneItem* btsi = qgraphicsitem_cast<BehaviorTreeSceneItem*>( *it );
    if( !btsi )
      continue;

    if( btsi->isType( BehaviorTreeIncludeType ) )
      includeDestroyed = true;

    delete btsi;
    itemDestroyed = true;
    break;
  }

  if( itemDestroyed )
  {
    emit modified();
    layout();
  }

  if( includeDestroyed )
    updateClone();
}

void BehaviorTreeScene::nodeSelected( QWidget* property_widget )
{
  emit itemSelected( property_widget );
}

void BehaviorTreeScene::updateClone()
{
  destroy( m_FullContext );
  m_FullContext = clone( m_TreeContext );

  ParsingInfo pi;

  ParserContextFunctions pcf;
  pcf.m_Read = &read_file;
  pcf.m_Error = &parser_error;
  pcf.m_Warning = &parser_warning;
  pcf.m_Translate = &parser_translate_include;

  Include* include = get_first_include( m_FullContext );
  int returnCode = 0;
  while( include )
  {
    QFileInfo fi( include->m_Name );
    QFile f( include->m_Name );

    pi.m_File = &f;
    pi.m_FileName = &fi;

    if( !pi.m_File->open( QFile::ReadOnly ) )
      break;

    ParserContext pc = create_parser_context( m_FullContext );
    set_extra( pc, &pi );
    set_current( pc, include->m_Name );
    returnCode = parse( pc, &pcf );
    destroy( pc );

    if( returnCode != 0 )
      break;

    include = include->m_Next;
  }

  if( returnCode != 0 )
  {
    destroy( m_FullContext );
    m_FullContext = clone( m_TreeContext );
  }

  int count = 0;
  NamedSymbol* ns_start = access_symbols( m_FullContext, &count );
  for( int i = 0; i < count; ++i )
  {
    NamedSymbol* ns = ns_start + i;

    switch( ns->m_Type )
    {
    case E_ST_UNKOWN:
      break;
    case E_ST_TREE:
      break;
    case E_ST_ACTION:
      if( ns->m_Symbol.m_Action->m_Declared )
      {
        NamedSymbol* ons = find_symbol( m_TreeContext , ns->m_Symbol.m_Action->m_Id.m_Hash );
        if( !ons )
        {
          clone( m_TreeContext, ns->m_Symbol.m_Action );
          ons = find_symbol( m_TreeContext , ns->m_Symbol.m_Action->m_Id.m_Hash );
          ons->m_Symbol.m_Action->m_Declared = false;
        }
        else if( ons->m_Symbol.m_Action->m_Declared == false && ons->m_Type == ns->m_Type )
        {
          clone( m_TreeContext, &ons->m_Symbol.m_Action->m_Locator, &ns->m_Symbol.m_Action->m_Locator );
          free_list( m_TreeContext, ons->m_Symbol.m_Action->m_Options );
          free_list( m_TreeContext, ons->m_Symbol.m_Action->m_Declarations );
          ons->m_Symbol.m_Action->m_Options = clone_list( m_TreeContext, ns->m_Symbol.m_Action->m_Options );
          ons->m_Symbol.m_Action->m_Declarations = clone_list( m_TreeContext, ns->m_Symbol.m_Action->m_Declarations );
        }
      }
      break;
    case E_ST_DECORATOR:
      if( ns->m_Symbol.m_Decorator->m_Declared )
      {
        NamedSymbol* ons = find_symbol( m_TreeContext, ns->m_Symbol.m_Decorator->m_Id.m_Hash );
        if( !ons )
        {
          clone( m_TreeContext, ns->m_Symbol.m_Decorator );
          ons = find_symbol( m_TreeContext , ns->m_Symbol.m_Decorator->m_Id.m_Hash );
          ons->m_Symbol.m_Action->m_Declared = false;
        }
        else if( ons->m_Symbol.m_Decorator->m_Declared == false && ons->m_Type == ns->m_Type )
        {
          clone( m_TreeContext, &ons->m_Symbol.m_Decorator->m_Locator, &ns->m_Symbol.m_Decorator->m_Locator );
          free_list( m_TreeContext, ons->m_Symbol.m_Decorator->m_Options );
          free_list( m_TreeContext, ons->m_Symbol.m_Decorator->m_Declarations );
          ons->m_Symbol.m_Decorator->m_Options = clone_list( m_TreeContext, ns->m_Symbol.m_Decorator->m_Options );
          ons->m_Symbol.m_Decorator->m_Declarations = clone_list( m_TreeContext, ns->m_Symbol.m_Decorator->m_Declarations );
        }
      }
      break;
    case E_MAX_SYMBOL_TYPES:
      break;
    }
  }

  emit updatedSymbols( m_FullContext );
}

void BehaviorTreeScene::createGraphics()
{
  int i, c;
  NamedSymbol* s = access_symbols( m_TreeContext, &c );
  for( i = 0; i < c; ++i )
  {
    if( s[i].m_Type != E_ST_TREE || !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    BehaviorTreeSceneItem* tree = new BehaviorTreeTree( m_TreeContext, s[i].m_Symbol.m_Tree );
    s[i].m_Symbol.m_Tree->m_UserData = tree;
    addItem( tree );
    connect( tree, SIGNAL(modified(bool)), this, SLOT(itemModified(bool)) );
    connect( tree, SIGNAL(itemSelected(QWidget*)), this, SLOT(nodeSelected(QWidget*)) );
    connect( tree, SIGNAL(symbolsChanged()), this, SLOT(updateClone()));
    connect( tree, SIGNAL(itemSymbolChanged(unsigned int)), this, SLOT(itemSymbolChanged(unsigned int)));
    createGraphics( s[i].m_Symbol.m_Tree->m_Root, tree );
  }

  Include* inc = get_first_include( m_TreeContext );
  float increment_pos = 0;
  while( inc )
  {
    BehaviorTreeSceneItem* inc_si = new BehaviorTreeInclude( m_TreeContext, inc );
    connect( inc_si, SIGNAL( itemSelected(QWidget*) ), this, SLOT( nodeSelected(QWidget*) ) );
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
    BehaviorTreeNode* svg_item = new BehaviorTreeNode( m_TreeContext, n, parent );

    connect( svg_item, SIGNAL( itemDragged() ), this, SLOT( layout() ) );
    connect( svg_item, SIGNAL( modified(bool) ), this, SLOT( itemModified(bool) ) );
    connect( svg_item, SIGNAL( itemSelected(QWidget*) ), this, SLOT( nodeSelected(QWidget*) ) );

    connect(
      svg_item,
      SIGNAL( relinkTargetMessage( QString, int ) ),
      m_MainWindow->statusBar(),
      SLOT( showMessage( QString, int ) )
    );

    if( !parent )
      addItem( svg_item );

    svg_item->setupPropertyEditor();

    if( parent )
    {
      NodeToNodeArrow* a = new NodeToNodeArrow( parent, svg_item, this );
      parent->addArrow( a );
      svg_item->addArrow( a );
    }

    n->m_UserData = (void*)svg_item;
    createGraphics( get_first_child( n ), svg_item );
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

    int els = el.size();
    if( els > 0 )
    {
      double br = 0.0;
      for( int i = 0; i < els; ++i )
      {
        if( el[i].r > br )
          br = el[i].r;
      }
      for( int i = 0; i < els; ++i )
        el[i].r = br;
    }
  }
}

void BehaviorTreeScene::depthFirstPlace( BehaviorTreeSceneItem* n, ExtentsList& pel )
{
  ExtentsList el;
  BehaviorTreeSceneItem* it = n->firstChild();
  double lx = 0.0;
  n->setPos( 0.0f, 0.0f );

  if( n->parentItem() )
    n->moveBy( 0.0f, g_NodeHeight + g_VertSpace );

  QRectF child_bounds;
  while( it )
  {
    ExtentsList t;
    depthFirstPlace( it, t );
    double slide = minimumRootDistance( el, t );
    it->moveBy( slide, 0 );
    child_bounds |= it->layoutBoundingRect().translated( it->pos() );
    lx = slide;
    moveExtents( t, slide );
    mergeExtents( el, el, t );
    it = it->nextSibling();
  }

  QRectF bounds = n->boundingRect();

  it = n->firstChild();
  if( it )
  {
    lx = (child_bounds.width() / 2.0) - (bounds.width() / 2.0);
    lx += it->layoutOffset();
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

  if( rs > ls )
    l.resize( rs );

  if( ls > 0 )
  {
    double br = 0.0;
    for( int i = 0; i < rs; ++i )
    {
      if( l[i].r > br )
        br = l[i].r;
    }
    for( int i = 0; i < rs; ++i )
      l[i].r = br;
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

  if( m_DragItem )
  {
    connect( m_DragItem, SIGNAL( modified(bool) ), this, SLOT( itemModified(bool) ) );
    connect( m_DragItem, SIGNAL( itemSelected( QWidget* ) ), this, SLOT( nodeSelected( QWidget* ) ) );
  }
}

void BehaviorTreeScene::setupTreeDrag( const XNodeData& data )
{
  //First, find a name for the new tree, we try "main" first
  //but if that is taken we go with "tree_X" where X is an incrementing number.
  char tree_name_buff[128];
  sprintf( tree_name_buff, "%s", "main" );
  hash_t name_hash = hashlittle( tree_name_buff );
  NamedSymbol* ns = find_symbol( m_TreeContext, name_hash );
  unsigned int c = 0;
  while( ns )
  {
    sprintf( tree_name_buff, "tree_%08u", ++c );
    name_hash = hashlittle( tree_name_buff );
    ns = find_symbol( m_TreeContext, name_hash );
  }

  //When we have a name we create and setup a BehaviorTree structure for it
  //that we add to the symbol register, and we create a "drag item" in the graphics scene.

  BehaviorTree* tree = (BehaviorTree*)allocate_object( m_TreeContext );
  init( tree );
  tree->m_Id.m_Text = register_string( m_TreeContext, tree_name_buff );
  tree->m_Id.m_Hash = name_hash;

  m_DragItem = new BehaviorTreeTree( m_TreeContext, tree );
  addItem( m_DragItem );

  connect( m_DragItem, SIGNAL(symbolsChanged()), this, SLOT(updateClone()));

  tree->m_UserData = m_DragItem;
  tree->m_Declared = true;

  NamedSymbol new_symbol;
  new_symbol.m_Type = E_ST_TREE;
  new_symbol.m_Symbol.m_Tree = tree;
  register_symbol( m_TreeContext, new_symbol );
}

void BehaviorTreeScene::setupNodeDrag( const XNodeData& data )
{
  //First, allocate a node from the tree and set it up.
  Node* node = (Node*)allocate_object( m_TreeContext );
  init( node );
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
  case E_GRIST_TREE:
    setupTreeNode( node, data );
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
  {
    BehaviorTreeNode* btn = new BehaviorTreeNode( m_TreeContext, node );
    addItem( btn );
    btn->setupPropertyEditor();
    m_DragItem = btn;
  }

  connect( m_DragItem, SIGNAL( itemDragged() ), this, SLOT( layout() ) );
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

  ns = find_symbol( m_FullContext, xnd.m_FuncId );
  Decorator* declared = ns->m_Symbol.m_Decorator;
  n->m_Grist.m_Decorator.m_Parameters = clone_list( m_TreeContext, declared->m_Declarations );
  Parameter* v = n->m_Grist.m_Decorator.m_Parameters;
  while( v )
  {
    v->m_ValueSet = true;
    v = v->m_Next;
  }
  n->m_Grist.m_Decorator.m_Decorator = look_up_decorator( m_TreeContext, &declared->m_Id );
}

void BehaviorTreeScene::setupActionNode( Node* n, const XNodeData& xnd )
{
  NamedSymbol* ns;

  ns = find_symbol( m_FullContext, xnd.m_FuncId );
  Action* declared = ns->m_Symbol.m_Action;
  n->m_Grist.m_Action.m_Parameters = clone_list( m_TreeContext, declared->m_Declarations );

  Parameter* v = n->m_Grist.m_Action.m_Parameters;
  while( v )
  {
    v->m_ValueSet = true;
    v = v->m_Next;
  }
  n->m_Grist.m_Action.m_Action = look_up_action( m_TreeContext, &declared->m_Id );
}

void BehaviorTreeScene::setupTreeNode( Node* n, const XNodeData& xnd )
{
  NamedSymbol* ns;
  ns = find_symbol( m_FullContext, xnd.m_FuncId );
  BehaviorTree* declared = ns->m_Symbol.m_Tree;
  n->m_Grist.m_Tree.m_Parameters = clone_list( m_TreeContext, declared->m_Declarations );

  Parameter* v = n->m_Grist.m_Action.m_Parameters;
  while( v )
  {
    v->m_ValueSet = true;
    v = v->m_Next;
  }

  n->m_Grist.m_Tree.m_Tree = look_up_behavior_tree( m_TreeContext, &declared->m_Id );
}
