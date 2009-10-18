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
#include <stdio.h>

const float g_NodeWidth  = 256.0f;
const float g_NodeHeight = 256.0f;
const float g_HoriSpace  = 64.0f;
const float g_VertSpace  = 128.0f;

BehaviorTreeScene::BehaviorTreeScene()
	: m_Tree( 0x0 )
{
}

BehaviorTreeScene::~BehaviorTreeScene()
{
	delete m_Tree;
}

bool BehaviorTreeScene::readFile( const QString& filename )
{
	if( m_Tree )
		delete m_Tree;
	m_Tree = new BehaviorTree;

	int returnCode = m_Tree->Parse( filename.toAscii() );
	if( returnCode != 0 )
		return false;

	clear();

	createGraphics( m_Tree->m_Root, 0x0 );
	layoutNode( m_Tree->m_Root );

	return true;
}

void BehaviorTreeScene::layoutNodes()
{
	if( m_Tree )
	{
		layoutNode( m_Tree->m_Root );
		setSceneRect( itemsBoundingRect() );
	}
}

void BehaviorTreeScene::createGraphics( Node* n, BehaviorTreeNode* parent )
{
	while( n )
	{
		BehaviorTreeNode* svg_item = new BehaviorTreeNode( n, parent );

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
