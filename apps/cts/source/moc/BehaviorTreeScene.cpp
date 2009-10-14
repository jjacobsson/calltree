/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "BehaviorTreeScene.h"
#include <btree/btree.h>

#include <QtGui/QGraphicsScene>
#include <QtSvg/QGraphicsSvgItem>

const char* const g_NodeResourcePaths[_E_MAX_GRIST_TYPES_] = {
	":/nodes/unknown.svg",
	":/nodes/sequence.svg",
	":/nodes/selector.svg",
	":/nodes/parallel.svg",
	":/nodes/dyn_selector.svg",
	":/nodes/decorator.svg",
	":/nodes/action.svg"
};

const float g_NodeWidth  = 256.0f;
const float g_NodeHeight = 256.0f;
const float g_HoriSpace  = 32.0f;
const float g_VertSpace  = 128.0f;

Node* GetFirstChild( Node* n )
{
	Node* r = 0x0;
	switch( n->m_Grist.m_Type )
	{
	case E_GRIST_SEQUENCE:
		r = n->m_Grist.m_Sequence.m_FirstChild;
		break;
	case E_GRIST_SELECTOR:
		r = n->m_Grist.m_Selector.m_FirstChild;
		break;
	case E_GRIST_PARALLEL:
		r = n->m_Grist.m_Parallel.m_FirstChild;
		break;
	case E_GRIST_DYN_SELECTOR:
		r = n->m_Grist.m_DynSelector.m_FirstChild;
		break;
	case E_GRIST_DECORATOR:
		r = n->m_Grist.m_Decorator.m_Child;
		break;
	case E_GRIST_ACTION:
		break;
	}
	return r;
}

BehaviorTreeScene::BehaviorTreeScene()
{
}

bool BehaviorTreeScene::readFile( const QString& filename )
{
	BehaviorTree bt;
	int returnCode = bt.Parse( filename.toAscii() );
	if( returnCode != 0 )
		return false;

	clear();
	parseNode( bt.m_Root );

	return true;
}

void BehaviorTreeScene::parseNode( Node* n )
{
	ExtentsList el;
	while( n )
	{
        ExtentsList t;
        depthFirstPlace( n, t );
        double slide = minimumRootDistance( el, t );
        moveExtents( t, slide );
        mergeExtents( el, el, t );
        transformToWorld( n, 0x0 );

		n = n->m_Sibling;
	}
}

void BehaviorTreeScene::depthFirstPlace( Node* n, ExtentsList& pel )
{
    ExtentsList el;

    QGraphicsSvgItem* svg_item = new QGraphicsSvgItem( g_NodeResourcePaths[n->m_Grist.m_Type] );
	addItem( svg_item );
	n->m_UserData = (void*)svg_item;

    Node* it = GetFirstChild( n );

    double lx = 0.0f;

    while( it )
    {
        ExtentsList t;
        depthFirstPlace( it, t );
        double slide = minimumRootDistance( el, t );

        QGraphicsSvgItem* svg_item = (QGraphicsSvgItem*)(it->m_UserData);
        QPointF pos( svg_item->pos() );
        pos.rx() = slide;
        svg_item->setPos( pos );

        lx = slide;
        moveExtents( t, slide );
        mergeExtents( el, el, t );

        it = it->m_Sibling;
    }

    it = GetFirstChild( n );
    if( it )
    {
        QGraphicsSvgItem* svg_item = (QGraphicsSvgItem*)(it->m_UserData);
        QPointF pos( svg_item->pos() );
        double fx = pos.x();
        double slide = (lx - fx) / 2.0;

        while( it )
        {
        	svg_item = (QGraphicsSvgItem*)(it->m_UserData);
            svg_item->moveBy( -slide, 0.0f );
            it = it->m_Sibling;
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
		p_svg_item = (QGraphicsSvgItem*)(p->m_UserData);
	while( n )
	{
		if( p_svg_item )
		{
			QGraphicsSvgItem* n_svg_item = (QGraphicsSvgItem*)(n->m_UserData);
			n_svg_item->moveBy( p_svg_item->x(), p_svg_item->y() + g_NodeHeight + g_VertSpace );
		}
		Node* c = GetFirstChild( n );

		if( c )
			transformToWorld( c, n );

		n = n->m_Sibling;
	}
}

