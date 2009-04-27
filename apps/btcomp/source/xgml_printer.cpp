/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include "xgml_printer.h"
#include "compiler/node.h"
#include "compiler/node_grist.h"
#include "compiler/identifier.h"
#include "compiler/action.h"
#include "compiler/decorator.h"
#include "compiler/grist/action_node_grist.h"
#include "compiler/grist/decorator_node_grist.h"

XGMLPrinter::XGMLPrinter()
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
    SNode* gn       = new SNode;
    gn->m_Node      = n;

    gn->m_Pos.x     = gn->m_Pos.y       = 0.0;
    gn->m_Depth     = gn->m_ChildCount  = gn->m_DepthIndex  = gn->m_ParentIndex = 0;
    gn->m_Parent    = gn->m_PrevDepth   = gn->m_NextDepth   = 0x0;


    if( n->m_Parent )
    {
        int pgn = FindParentIndex( n->m_Parent );
        SSpring s;
        s.m_From        = pgn;
        s.m_To          = m_Nodes.size();
        m_Springs.push_back( s );

        gn->m_Parent        = m_Nodes[pgn];
        gn->m_Depth         = gn->m_Parent->m_Depth + 1;
        gn->m_Pos.y         = gn->m_Parent->m_Pos.y + 140.0;
        gn->m_ParentIndex   = gn->m_Parent->m_ChildCount++;
    }

    while( gn->m_Depth >= (int)m_Depth.size() )
    {
        SDepth depth;
        depth.m_Last    = 0x0;
        depth.m_Count   = 0;
        m_Depth.push_back( depth );
    }

    SDepth& depth   = m_Depth[ gn->m_Depth ];

    gn->m_DepthIndex    = depth.m_Count++;
    gn->m_PrevDepth     = depth.m_Last;
    depth.m_Last        = gn;

    m_Nodes.push_back( gn );
}

void XGMLPrinter::Layout()
{
    /* combined width calculation */
    /* slight edit, an attempt to force github to get with the program. */
    const double node_width     = 120.0f;

    NodeList::iterator it, it_e( m_Nodes.end() );
    for( it = m_Nodes.begin(); it != it_e; ++it )
    {
        SNode* n = (*it);
        if( n->m_Parent )
        {
            const double width  = n->m_Parent->m_ChildCount * node_width;
            const double start  = n->m_Parent->m_Pos.x + (node_width / 2.0) - (width / 2.0);
            n->m_Pos.x = start + (node_width * n->m_ParentIndex);
        }
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
        CSpringList::const_iterator it, it_e( m_Springs.end() );
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


void XGMLPrinter::PrintSequence( FILE* f, const SNode* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Sequence</attribute>\n" );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">ellipse</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#FFFF00</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "<attribute key=\"outlineWidth\" type=\"int\">2</attribute>\n" );
    fprintf( f, "<attribute key=\"outlineStyle\" type=\"String\">dashed</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Sequence</attribute>\n" );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintSelector( FILE* f, const SNode* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Selector</attribute>\n" );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">ellipse</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#00FF00</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Selector</attribute>\n" );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintParallel( FILE* f, const SNode* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Parallel</attribute>\n" );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">roundrectangle</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#800080</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Parallel</attribute>\n" );
    fprintf( f, "<attribute key=\"color\" type=\"String\">#FFFFFF</attribute>\n" );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintDynSelector( FILE* f, const SNode* n ) const
{
    fprintf( f, "<attribute key=\"label\" type=\"String\">Dyn Selector</attribute>\n" );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">octagon</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#339966</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">Dyn Selector</attribute>\n" );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintDecorator( FILE* f, const SNode* n ) const
{
    DecoratorNodeGrist* dng = (DecoratorNodeGrist*)n->m_Node->m_Grist;
    const char* const text      = dng->GetDecorator()->m_Id->m_Text;

    fprintf( f, "<attribute key=\"label\" type=\"String\">%s</attribute>\n", text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">diamond</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#FF0000</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">%s</attribute>\n", text );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintAction( FILE* f, const SNode* n ) const
{
    ActionNodeGrist* ang    = (ActionNodeGrist*)n->m_Node->m_Grist;
    const char* const text  = ang->GetAction()->m_Id->m_Text;
    fprintf( f, "<attribute key=\"label\" type=\"String\">%s</attribute>\n", text );
    PrintCommonGraphics( f, n );
    fprintf( f, "<attribute key=\"type\" type=\"String\">triangle</attribute>\n" );
    fprintf( f, "<attribute key=\"fill\" type=\"String\">#6666FF</attribute>\n" );
    fprintf( f, "<attribute key=\"outline\" type=\"String\">#000000</attribute>\n" );
    fprintf( f, "</section>\n" );
    PrintCommonLabel( f );
    fprintf( f, "<attribute key=\"text\" type=\"String\">%s</attribute>\n", text );
    fprintf( f, "</section>\n" );
}

void XGMLPrinter::PrintCommonGraphics( FILE* f, const SNode* n ) const
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

