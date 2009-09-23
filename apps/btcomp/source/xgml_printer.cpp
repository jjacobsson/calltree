/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "xgml_printer.h"
#include <btree/node.h>
#include <btree/node_grist.h>
#include <btree/identifier.h>
#include <btree/action.h>
#include <btree/decorator.h>
#include <btree/action_node_grist.h>
#include <btree/decorator_node_grist.h>

const double XGMLPrinter::s_node_width = 120.0;

XGMLPrinter::XGMLPrinter()
	: m_Debug( false )
{
}

XGMLPrinter::~XGMLPrinter()
{
    NodeList::iterator it, it_e( m_Nodes.end() );
    for( it = m_Nodes.begin(); it != it_e; ++it )
        delete *it;
}

void XGMLPrinter::Visit( Node* n )
{
    NodeInfo* gn = new NodeInfo;
    gn->m_Node   = n;

    gn->m_Pos.x  = gn->m_Pos.y      = 0.0;
    gn->m_Depth  = gn->m_ChildCount = gn->m_DepthIndex = gn->m_ParentIndex = 0;
    gn->m_Parent = gn->m_PrevDepth  = gn->m_NextDepth  = 0x0;
    gn->m_FirstChild = gn->m_LastChild = 0x0;

    if( n->m_Parent )
    {
        int pgn = FindParentIndex( n->m_Parent );
        Spring s;
        s.m_From = pgn;
        s.m_To   = m_Nodes.size();
        m_Springs.push_back( s );

        gn->m_Parent      = m_Nodes[pgn];
        gn->m_Depth       = gn->m_Parent->m_Depth + 1;
        gn->m_Pos.y       = gn->m_Parent->m_Pos.y + 140.0;
        gn->m_ParentIndex = gn->m_Parent->m_ChildCount++;
        if( gn->m_Parent->m_FirstChild == 0x0 )
            gn->m_Parent->m_FirstChild = gn;

        gn->m_Parent->m_LastChild = gn;
    }

    while( gn->m_Depth >= (int)m_Depth.size() )
    {
        Depth depth;
        depth.m_Last  = 0x0;
        depth.m_First = 0x0;
        depth.m_Count = 0;
        m_Depth.push_back( depth );
    }

    Depth& depth = m_Depth[ gn->m_Depth ];

    gn->m_DepthIndex = depth.m_Count++;

    if( !depth.m_First )
    {
        gn->m_PrevDepth = 0x0;
        gn->m_NextDepth = 0x0;
        depth.m_First   = gn;
        depth.m_Last    = gn;
    }
    else
    {
        gn->m_PrevDepth = depth.m_Last;
        gn->m_NextDepth = 0x0;
        depth.m_Last->m_NextDepth = gn;
        depth.m_Last = gn;
    }

    m_Nodes.push_back( gn );
}

void XGMLPrinter::Layout()
{
    ExtentsList el;
    NodeList::iterator it, it_e( m_Nodes.end() );
    for( it = m_Nodes.begin(); it != it_e; ++it )
    {
        if( (*it)->m_Parent == 0x0 )
        {
            ExtentsList t;
            DepthFirstPlace( (*it), t );
            double slide = MinimumRootDistance( el, t );
            (*it)->m_Pos.x = slide;
            MoveExtents( t, slide );
            MergeExtents( el, el, t );
            TransformToWorld( (*it) );
        }
    }
}

void XGMLPrinter::DepthFirstPlace( NodeInfo* n, ExtentsList& pel )
{
    ExtentsList el;
    NodeInfo* it = n->m_FirstChild;
    while( it )
    {
        ExtentsList t;
        DepthFirstPlace( it, t );
        double slide = MinimumRootDistance( el, t );
        
        it->m_Pos.x = slide;
        MoveExtents( t, slide );
        MergeExtents( el, el, t );

        if( it == n->m_LastChild )
            it = 0x0;
        else
            it = it->m_NextDepth;
    }

    it = n->m_FirstChild;
    if( it )
    {

        double fx = n->m_FirstChild->m_Pos.x;
        double lx = n->m_LastChild->m_Pos.x;
        double slide = (lx - fx) / 2.0;

        while( it )
        {
            it->m_Pos.x -= slide;
            if( it == n->m_LastChild )
                it = 0x0;
            else
                it = it->m_NextDepth;
        }

        MoveExtents( el, -slide );
    }

    Extents e;
    e.l = 0;
    e.r = s_node_width;
    pel.push_back( e );
    if( !el.empty() )
        pel.insert( pel.end(), el.begin(), el.end() );

}

double XGMLPrinter::MinimumRootDistance( const ExtentsList& lel, const ExtentsList& rel )
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

void XGMLPrinter::MoveExtents( ExtentsList& el, double dist )
{
    size_t s = el.size();
    for( size_t i = 0; i < s; ++i )
    {
        el[i].l += dist;
        el[i].r += dist;
        if( m_Debug )
            printf( "i: %d w: %.2f\n", i, el[i].r - el[i].l );
    }
}

void XGMLPrinter::MergeExtents( ExtentsList& r, const ExtentsList& lel, const ExtentsList& rel )
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

void XGMLPrinter::TransformToWorld( NodeInfo* n )
{
    if( n->m_Parent )
        n->m_Pos.x += n->m_Parent->m_Pos.x;
    NodeInfo* it = n->m_FirstChild;
    while( it )
    {
        TransformToWorld( it );
        if( it == n->m_LastChild )
            it = 0x0;
        else
            it = it->m_NextDepth;
    }
}

void XGMLPrinter::Print( FILE* file ) const
{
    fprintf( file, "<?xml version=\"1.0\" encoding=\"Cp1252\"?>\n" );
    fprintf( file, "<section name=\"xgml\">\n" );
    fprintf( file, "<attribute key=\"Creator\" type=\"String\">btcomp</attribute>\n" );
    fprintf( file, "<attribute key=\"Version\" type=\"String\">1.0b</attribute>\n" );
    fprintf( file, "<section name=\"graph\">\n" );
    fprintf( file, "<attribute key=\"hierarchic\" type=\"int\">1</attribute>\n" );
    fprintf( file, "<attribute key=\"label\" type=\"String\"></attribute>\n" );
    fprintf( file, "<attribute key=\"directed\" type=\"int\">1</attribute>\n" );
    int i = 0;
    {
        NodeList::const_iterator it, it_e( m_Nodes.end() );
        for( it = m_Nodes.begin(); it != it_e; ++it, ++i )
        {
            fprintf( file, "<section name=\"node\">\n" );
            fprintf( file, "<attribute key=\"id\" type=\"int\">%d</attribute>\n", i );
            switch( (*it)->m_Node->m_Grist->GetType() )
            {
            case E_GRIST_SEQUENCE:
                PrintSequence( file, (*it) );
                break;
            case E_GRIST_SELECTOR:
                PrintSelector( file, (*it) );
                break;
            case E_GRIST_PARALLEL:
                PrintParallel( file, (*it) );
                break;
            case E_GRIST_DYN_SELECTOR:
                PrintDynSelector( file, (*it) );
                break;
            case E_GRIST_DECORATOR:
                PrintDecorator( file, (*it) );
                break;
            case E_GRIST_ACTION:
                PrintAction( file, (*it) );
                break;
            }
            fprintf( file, "</section>\n" );
        }
    }
    {
        SpringList::const_iterator it, it_e( m_Springs.end() );
        for( it = m_Springs.begin(); it != it_e; ++it )
        {
            fprintf( file, "<section name=\"edge\">\n" );
            fprintf( file, "<attribute key=\"source\" type=\"int\">%d</attribute>\n", (*it).m_From );
            fprintf( file, "<attribute key=\"target\" type=\"int\">%d</attribute>\n", (*it).m_To );
            fprintf( file, "<attribute key=\"label\" type=\"String\"></attribute>\n" );
            fprintf( file, "<section name=\"graphics\">\n" );
            fprintf( file, "<attribute key=\"fill\" type=\"String\">#000000</attribute>\n" );
            fprintf( file, "<attribute key=\"targetArrow\" type=\"String\">standard</attribute>\n" );
            fprintf( file, "</section>\n" );
            fprintf( file, "<section name=\"LabelGraphics\">\n" );
            fprintf( file, "</section>\n" );
            fprintf( file, "</section>\n" );
        }
    }


    fprintf( file, "</section>\n" );
    fprintf( file, "</section>\n" );

}

int XGMLPrinter::FindParentIndex( const Node* n ) const
{
    int i = 0;
    NodeList::const_iterator it, it_e( m_Nodes.end() );
    for( it = m_Nodes.begin(); it != it_e; ++it, ++i )
    {
        if( (*it)->m_Node == n )
            return i;
    }
    return -1;
}

void XGMLPrinter::PrintSequence( FILE* f, const NodeInfo* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Sequence\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">ellipse</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#FFFF00</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "<attribute key=\"outlineWidth\" type=\"int\">2</attribute>\n" );
    fprintf( f, "<attribute key=\"outlineStyle\" type=\"String\">dashed</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Sequence\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintSelector( FILE* f, const NodeInfo* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Selector\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">ellipse</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#00FF00</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Selector\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintParallel( FILE* f, const NodeInfo* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Parallel\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">roundrectangle</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#800080</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Parallel\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    fprintf( f, "<attribute key=\"color\" type=\"String\">#FFFFFF</attribute>\n" );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintDynSelector( FILE* f, const NodeInfo* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Dyn Selector\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">octagon</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#339966</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Dyn Selector\n%s</attribute>\n", n->m_Node->m_Id.m_Text );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintDecorator( FILE* f, const NodeInfo* n ) const
{
    DecoratorNodeGrist* dng = (DecoratorNodeGrist*)n->m_Node->m_Grist;
    const char* const text      = dng->GetDecorator()->m_Id.m_Text;

    fprintf( f, "<attribute key=\"label\" type=\"String\">%s\n%s</attribute>\n", text, n->m_Node->m_Id.m_Text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">diamond</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#FF0000</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">%s\n%s</attribute>\n", text, n->m_Node->m_Id.m_Text );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintAction( FILE* f, const NodeInfo* n ) const
{
    ActionNodeGrist* ang    = (ActionNodeGrist*)n->m_Node->m_Grist;
    const char* const text  = ang->GetAction()->m_Id.m_Text;
    fprintf( f, "<attribute key=\"label\" type=\"String\">%s\n%s</attribute>\n", text, n->m_Node->m_Id.m_Text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">triangle</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#6666FF</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">%s\n%s</attribute>\n", text, n->m_Node->m_Id.m_Text );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintCommonGraphics( FILE* f, const NodeInfo* n ) const
{
    fprintf( f, "<section name=\"graphics\">\n" );
    fprintf( f, "<attribute key=\"x\" type=\"double\">%.2f</attribute>\n", n->m_Pos.x );
    fprintf( f, "<attribute key=\"y\" type=\"double\">%.2f</attribute>\n", n->m_Pos.y );
    fprintf( f, "<attribute key=\"w\" type=\"double\">100.0</attribute>\n" );
    fprintf( f, "<attribute key=\"h\" type=\"double\">100.0</attribute>\n" );
}

void XGMLPrinter::PrintCommonLabel( FILE* f ) const
{
    fprintf( f, "<section name=\"LabelGraphics\">\n" );
    fprintf( f, "<attribute key=\"fontSize\" type=\"int\">13</attribute>\n" );
    fprintf( f, "<attribute key=\"fontName\" type=\"String\">Dialog</attribute>\n" );
    fprintf( f, "<attribute key=\"anchor\" type=\"String\">c</attribute>\n" );
}

