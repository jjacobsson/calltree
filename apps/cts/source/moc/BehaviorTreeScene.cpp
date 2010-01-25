/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "BehaviorTreeScene.h"
#include "BehaviorTreeNode.h"
#include "../NodeToNodeArrow.h"
#include <btree/btree.h>
#include <btree/btree_parse.h>
#include <stdio.h>
#include <malloc.h>

#include <QtGui/QtGui>

const float g_NodeWidth  = 256.0f;
const float g_NodeHeight = 256.0f;
const float g_HoriSpace  = 64.0f;
const float g_VertSpace  = 128.0f;

struct ParsingInfo
{
  FILE*         m_File;
  const char*   m_Name;
};

int read_file( ParserContext pc, char* buffer, int maxsize )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( !pi )
    return 0;
  if( feof( pi->m_File ) )
    return 0;
  return (int)fread( buffer, 1, maxsize, pi->m_File );
}

void parser_error( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( pi )
  {
     fprintf( stdout, "%s(%d) : error : %s\n", pi->m_Name, ParserContextGetLineNo( pc ), msg );
  }
  else
  {
     fprintf( stdout, "<no file>(%d) : error : %s\n", ParserContextGetLineNo( pc ), msg );
  }
}

void parser_warning( ParserContext pc, const char* msg )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  if( pi )
  {
     fprintf( stdout, "%s(%d) : warning : %s\n", pi->m_Name, ParserContextGetLineNo( pc ), msg );
  }
  else
  {
     fprintf( stdout, "<no file>(%d) : warning : %s\n", ParserContextGetLineNo( pc ), msg );
  }
}

const char* parser_translate_include( ParserContext pc, const char* include )
{
  ParsingInfo* pi = (ParsingInfo*)ParserContextGetExtra( pc );
  BehaviorTreeContext btc = ParserContextGetBehaviorTreeContext( pc );

  StringBuffer sb;
  StringBufferInit( pc, &sb );

  if( pi->m_Name )
  {
    char backslash  = '\\';
    char frontslash = '/';

    int s = 0, last = -1;
    const char* p = pi->m_Name;
    while( p && *p )
    {
      if( *p == backslash || *p == frontslash )
        last = s;
      ++p;
      ++s;
    }
    if( last != -1 )
      StringBufferAppend( pc, &sb, pi->m_Name, last + 1 );
  }

  StringBufferAppend( pc, &sb, include );
  const char* ret = BehaviorTreeContextRegisterString( btc, sb.m_Str );
  StringBufferDestroy( pc, &sb );

  return ret;
}

void* allocate_memory( mem_size_t size )
{
  return malloc( size );
}

void free_memory( void* ptr )
{
  if( ptr )
    free( ptr );
}

BehaviorTreeScene::BehaviorTreeScene()
	: m_TreeContext( 0x0 )
{

}

BehaviorTreeScene::~BehaviorTreeScene()
{
	BehaviorTreeContextDestroy( m_TreeContext );
	m_TreeContext = (BehaviorTreeContext)0xdeadbeef;
}

bool BehaviorTreeScene::readFile( const QString& filename )
{
  BehaviorTreeContextDestroy( m_TreeContext );
  m_TreeContext = (BehaviorTreeContext)0xdeadbeef;

  BehaviorTreeContextSetup btcs;
  btcs.m_Alloc = &allocate_memory;
  btcs.m_Free = &free_memory;
  m_TreeContext = BehaviorTreeContextCreate( &btcs );

  ParsingInfo pi;
  pi.m_Name = filename.toAscii();
  pi.m_File = fopen( pi.m_Name, "r" );
  if( !pi.m_File )
    return false;

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


  int i, c;
  NamedSymbol* s = BehaviorTreeContextAccessSymbols( m_TreeContext, &c );
  for( i = 0; i < c; ++i )
  {
    if( s[i].m_Type != E_ST_TREE || !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    createGraphics( s[i].m_Symbol.m_Tree->m_Root, 0x0 );
    layoutNode( s[i].m_Symbol.m_Tree->m_Root );
  }

/*
	int returnCode = m_Tree->Parse( filename.toAscii() );
	if( returnCode != 0 )
		return false;

	clear();

	createGraphics( m_Tree->m_Root, 0x0 );
	layoutNode( m_Tree->m_Root );
*/
	return true;
}

void BehaviorTreeScene::layoutNodes()
{
  if( !m_TreeContext )
    return;

  int i, c;
  NamedSymbol* s = BehaviorTreeContextAccessSymbols( m_TreeContext, &c );
  for( i = 0; i < c; ++i )
  {
    if( s[i].m_Type != E_ST_TREE || !s[i].m_Symbol.m_Tree->m_Declared )
      continue;
    createGraphics( s[i].m_Symbol.m_Tree->m_Root, 0x0 );
    layoutNode( s[i].m_Symbol.m_Tree->m_Root );
  }
  setSceneRect( itemsBoundingRect() );
}

void BehaviorTreeScene::createGraphics( Node* n, BehaviorTreeNode* parent )
{
	while( n )
	{
		BehaviorTreeNode* svg_item = new BehaviorTreeNode( n, parent );

		connect( svg_item, SIGNAL( nodeDragged() ), this, SLOT( layoutNodes() ) );

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

void BehaviorTreeScene::layoutNode( Node* n )
{
	ExtentsList el;
	while( n )
	{
        ExtentsList t;
        depthFirstPlace( n, t );
        double slide = minimumRootDistance( el, t );
        BehaviorTreeNode* svg_item = (BehaviorTreeNode*)(n->m_UserData);
        svg_item->moveBy( slide, 0 );
        moveExtents( t, slide );
        mergeExtents( el, el, t );
		//transformToWorld( n, 0x0 );
        n = n->m_Next;
	}
 }

void BehaviorTreeScene::depthFirstPlace( Node* n, ExtentsList& pel )
{
    ExtentsList el;
    Node* it = GetFirstChild( n );
    double lx = 0.0f;

    BehaviorTreeNode* svg_item = (BehaviorTreeNode*)(n->m_UserData);
    svg_item->setPos( 0.0f, 0.0f );

    if( svg_item->parentItem() )
    	svg_item->moveBy( 0.0f, g_NodeHeight + g_VertSpace );

    while( it )
    {
        ExtentsList t;
        depthFirstPlace( it, t );
        double slide = minimumRootDistance( el, t );

        svg_item = (BehaviorTreeNode*)(it->m_UserData);
        QPointF pos( svg_item->pos() );
        pos.rx() = slide;
        svg_item->setPos( pos );

        lx = slide;
        moveExtents( t, slide );
        mergeExtents( el, el, t );

        it = it->m_Next;
    }

    it = GetFirstChild( n );
    if( it )
    {
    	svg_item = (BehaviorTreeNode*)(it->m_UserData);
        QPointF pos( svg_item->pos() );
        double fx = pos.x();
        double slide = (lx - fx) / 2.0;

        while( it )
        {
        	svg_item = (BehaviorTreeNode*)(it->m_UserData);
			svg_item->moveBy( -slide, 0.0 );
            it = it->m_Next;
        }

        moveExtents( el, -slide );
    }

    Extents e;
    e.l = 0;
    e.r = g_NodeWidth + g_HoriSpace ;
    pel.push_back( e );
    if( !el.empty() )
        pel.insert( pel.end(), el.begin(), el.end() );

}


double BehaviorTreeScene::minimumRootDistance( const ExtentsList& lel, const ExtentsList& rel )
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

void BehaviorTreeScene::mergeExtents( ExtentsList& r, const ExtentsList& lel, const ExtentsList& rel )
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

void BehaviorTreeScene::transformToWorld( Node* n, Node* p )
{
	QGraphicsSvgItem* p_svg_item = 0x0;
	if( p )
		p_svg_item = (BehaviorTreeNode*)(p->m_UserData);
	while( n )
	{
		if( p_svg_item )
		{
			BehaviorTreeNode* n_svg_item = (BehaviorTreeNode*)(n->m_UserData);
			n_svg_item->moveBy( p_svg_item->x(), p_svg_item->y() + g_NodeHeight + g_VertSpace );
		}
		Node* c = GetFirstChild( n );

		if( c )
			transformToWorld( c, n );

		n = n->m_Next;
	}
}

void BehaviorTreeScene::drawItems(
		QPainter* painter,
		int numItems,
		QGraphicsItem* items[],
		const QStyleOptionGraphicsItem options[],
		QWidget* widget
	)
{
	int c = 0;
	QGraphicsItem** tItems = (QGraphicsItem**)alloca( sizeof(QGraphicsItem*) * numItems );
	QStyleOptionGraphicsItem* tOptions = new QStyleOptionGraphicsItem[numItems];


	for( int i = 0; i < numItems; ++i )
	{
		if( items[i]->type() == NodeToNodeArrow::Type )
		{
			tItems[c]	= items[i];
			tOptions[c]	= options[i];
			++c;
		}
	}
	for( int i = 0; i < numItems; ++i )
	{
		if( items[i]->type() != NodeToNodeArrow::Type )
		{
			tItems[c]	= items[i];
			tOptions[c]	= options[i];
			++c;
		}
	}

	QGraphicsScene::drawItems( painter, numItems, tItems, tOptions, widget );

	delete [] tOptions;
}

