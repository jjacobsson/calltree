/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#ifndef XGML_PRINTER_H_
#define XGML_PRINTER_H_

#include "compiler/visitor.h"

#include <vector>
#include <stdlib.h>

class XGMLPrinter : public INodeVisitor
{
public:
    XGMLPrinter();
    ~XGMLPrinter();

    void Visit( Node* n );

    void Layout();

    void Print( FILE* file ) const;

private:

    static const double s_node_width;

    struct NodeInfo;

    struct Extents
    {
        double l;
        double r;
    };

    typedef std::vector<Extents> ExtentsList;

    void DepthFirstPlace( NodeInfo* n, ExtentsList& el );
    double MinimumRootDistance( const ExtentsList& l, const ExtentsList& r );
    void MoveExtents( ExtentsList& el, double dist );
    void MergeExtents( ExtentsList& r, const ExtentsList& l, const ExtentsList& r );
    void TransformToWorld( NodeInfo* n );
    

    int FindParentIndex( const Node* n ) const;

    struct Spring
    {
        int m_From;
        int m_To;
    };
    struct Vector
    {
        double x;
        double y;
    };

    struct NodeInfo
    {
        Vector      m_Pos;
        Node*       m_Node;
        NodeInfo*   m_Parent;
        NodeInfo*   m_PrevDepth;
        NodeInfo*   m_NextDepth;
        NodeInfo*   m_FirstChild;
        NodeInfo*   m_LastChild;
        int         m_Depth;
        int         m_ChildCount;
        int         m_DepthIndex;  /* index on this depth */
        int         m_ParentIndex; /* index under parent */
        float       m_Width;
    };

    struct Depth
    {
        NodeInfo* m_Last;
        NodeInfo* m_First;
        int       m_Count;
    };

    void PrintSequence( FILE* f, const NodeInfo* n ) const;
    void PrintSelector( FILE* f, const NodeInfo* n ) const;
    void PrintParallel( FILE* f, const NodeInfo* n ) const;
    void PrintDynSelector( FILE* f, const NodeInfo* n ) const;
    void PrintDecorator( FILE* f, const NodeInfo* n ) const;
    void PrintAction( FILE* f, const NodeInfo* n ) const;

    void PrintCommonGraphics( FILE* f, const NodeInfo* n ) const;
    void PrintCommonLabel( FILE* f ) const;

    typedef std::vector<Spring>    SpringList;
    typedef std::vector<Depth>     DepthList;
    typedef std::vector<NodeInfo*> NodeList;

    SpringList m_Springs;
    NodeList   m_Nodes;
    DepthList  m_Depth;
    bool       m_Debug;
};



#endif /* XGML_PRINTER_H_ */
