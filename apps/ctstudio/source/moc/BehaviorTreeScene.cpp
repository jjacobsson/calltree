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
#include "../NodeToNodeArrow.h"
#include "../btree_callbacks.h"
#include <btree/btree.h>
#include <btree/btree_parse.h>
#include <other/lookup3.h>
#include <malloc.h>

#include <QtGui/QtGui>

const float g_NodeWidth = 256.0f;
const float g_NodeHeight = 256.0f;
const float g_HoriSpace = 64.0f;
const float g_VertSpace = 128.0f;

BehaviorTreeScene::BehaviorTreeScene( QMainWindow* mw )
  : m_TreeContext( 0x0 )
  , m_MainWindow( mw )
{
  BehaviorTreeContextSetup btcs;
  btcs.m_Alloc = &allocate_memory;
  btcs.m_Free = &free_memory;
  m_TreeContext = BehaviorTreeContextCreate( &btcs );
}

BehaviorTreeScene::~BehaviorTreeScene()
{
  BehaviorTreeContextDestroy( m_TreeContext );
  m_TreeContext = (BehaviorTreeContext)0xdeadbeef;
}

void BehaviorTreeScene::dragEnterEvent( QDragEnterEvent *event )
{
  if( event->mimeData()->hasFormat("ctstudio/x-node") )
    event->accept();
  else
    event->ignore();
}

void BehaviorTreeScene::dragLeaveEvent( QDragLeaveEvent *event )
{
   event->accept();
}

void BehaviorTreeScene::dragMoveEvent( QDragMoveEvent *event )
{
  if( event->mimeData()->hasFormat("ctstudio/x-node") )
    event->accept();
  else
    event->ignore();
}

void BehaviorTreeScene::dropEvent( QDropEvent* event )
{
  if( event->mimeData()->hasFormat("ctstudio/x-node") )
  {
    event->accept();

    hash_t h = hashlittle( "main" );
    NamedSymbol* ns = BehaviorTreeContextFindSymbol( m_TreeContext, h );
    BehaviorTree* bt = 0x0;
    BehaviorTreeSceneItem* tree = 0x0;
    if( !ns )
    {
      bt = (BehaviorTree*)BehaviorTreeContextAllocateObject( m_TreeContext );

      tree = new BehaviorTreeTree( bt );
      addItem( tree );

      InitBehaviorTree( bt );
      bt->m_Id.m_Hash = h;
      bt->m_Id.m_Text = BehaviorTreeContextRegisterString( m_TreeContext, "main" );
      bt->m_Declared = true;
      bt->m_UserData = tree;

      NamedSymbol reg;
      reg.m_Type = E_ST_TREE;
      reg.m_Symbol.m_Tree = bt;
      BehaviorTreeContextRegisterSymbol( m_TreeContext, reg );
    }
    else if( ns->m_Type != E_ST_TREE )
    {
      // FAIL!
    }
    else if( !ns->m_Symbol.m_Tree->m_Declared )
    {
      bt = ns->m_Symbol.m_Tree;
      bt->m_Declared = true;
    }
    else
    {
      bt = ns->m_Symbol.m_Tree;
    }

    if( !bt )
      return;

    tree = (BehaviorTreeSceneItem*)bt->m_UserData;

    QByteArray pieceData = event->mimeData()->data("ctstudio/x-node");
    QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
    int grist_type, act_dec_id;
    dataStream >> grist_type >> act_dec_id;

    Node* n = (Node*)BehaviorTreeContextAllocateObject( m_TreeContext );
    InitNode( n );
    switch( grist_type )
    {
    case E_GRIST_SEQUENCE: n->m_Grist.m_Type = E_GRIST_SEQUENCE; break;
    case E_GRIST_SELECTOR: n->m_Grist.m_Type = E_GRIST_SELECTOR; break;
    case E_GRIST_PARALLEL: n->m_Grist.m_Type = E_GRIST_PARALLEL; break;
    case E_GRIST_DYN_SELECTOR: n->m_Grist.m_Type = E_GRIST_DYN_SELECTOR; break;
    default:
      // FAIL!
      break;
    }

    createGraphics( n, tree );

    n->m_Pare.m_Type = E_ST_TREE;
    n->m_Pare.m_Tree = bt;

    if( bt->m_Root )
      AppendToEndOfList( bt->m_Root, n );
    else
      bt->m_Root = n;

    layout();
  }
  else
    event->ignore();
}

bool BehaviorTreeScene::readFile( const QString& qt_filename )
{
  BehaviorTreeContextDestroy( m_TreeContext );
  m_TreeContext = (BehaviorTreeContext)0xdeadbeef;

  BehaviorTreeContextSetup btcs;
  btcs.m_Alloc = &allocate_memory;
  btcs.m_Free = &free_memory;
  m_TreeContext = BehaviorTreeContextCreate( &btcs );

  QFile f( qt_filename );

  if( !f.open( QFile::ReadOnly ) )
    return false;

  std::string filename( qt_filename.toStdString() );

  ParsingInfo pi;
  pi.m_Name = BehaviorTreeContextRegisterString( m_TreeContext, filename.c_str() );
  pi.m_File = &f;

  ParserContextFunctions pcf;
  pcf.m_Read = &read_file;
  pcf.m_Error = &parser_error;
  pcf.m_Warning = &parser_warning;
  pcf.m_Translate = &parser_translate_include;

  ParserContext pc = ParserContextCreate( m_TreeContext );
  ParserContextSetExtra( pc, &pi );
  ParserContextSetCurrent( pc, pi.m_Name );
  int returnCode = Parse( pc, &pcf );
  ParserContextDestroy( pc );

  if( returnCode != 0 )
    return false;

  clear();

  createGraphics();
  layout();

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
  setSceneRect( itemsBoundingRect() );
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
    createGraphics( s[i].m_Symbol.m_Tree->m_Root, tree );
  }
}

void BehaviorTreeScene::createGraphics( Node* n, BehaviorTreeSceneItem* parent )
{
  while( n )
  {
    BehaviorTreeNode* svg_item = new BehaviorTreeNode( n, parent );

    connect( svg_item, SIGNAL( itemDragged() ), this, SLOT( layout() ) );

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
    n->moveBy( slide, 0 );
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

  n->setPos( 0.0f, 0.0f );

  if( n->parentItem() )
    n->moveBy( 0.0f, g_NodeHeight + g_VertSpace );

  while( it )
  {
    ExtentsList t;
    depthFirstPlace( it, t );
    double slide = minimumRootDistance( el, t );
    it->moveBy( slide, 0 );
    lx = slide;
    moveExtents( t, slide );
    mergeExtents( el, el, t );
    it = it->nextSibling();
  }

  it = n->firstChild();
  if( it )
  {
    lx /= 2.0;
    while( it )
    {
      it->moveBy( -lx, 0 );
      it = it->nextSibling();
    }
    moveExtents( el, -lx );
  }

  Extents e;
  e.l = 0;
  e.r = g_NodeWidth + g_HoriSpace;
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

void BehaviorTreeScene::drawItems( QPainter* painter, int numItems,
  QGraphicsItem* items[], const QStyleOptionGraphicsItem options[],
  QWidget* widget )
{
  int c = 0;
  QGraphicsItem** tItems =
      (QGraphicsItem**)alloca( sizeof(QGraphicsItem*) * numItems );
  QStyleOptionGraphicsItem* tOptions = new QStyleOptionGraphicsItem[numItems];

  for( int i = 0; i < numItems; ++i )
  {
    if( items[i]->type() == NodeToNodeArrow::Type )
    {
      tItems[c] = items[i];
      tOptions[c] = options[i];
      ++c;
    }
  }
  for( int i = 0; i < numItems; ++i )
  {
    if( items[i]->type() != NodeToNodeArrow::Type )
    {
      tItems[c] = items[i];
      tOptions[c] = options[i];
      ++c;
    }
  }

  QGraphicsScene::drawItems( painter, numItems, tItems, tOptions, widget );

  delete[] tOptions;
}

