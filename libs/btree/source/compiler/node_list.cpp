/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <btree/node_list.h>
#include <btree/node.h>

void NodeList::SetParent( Node* n )
{
    iterator it, it_e( end() );
    for( it = begin(); it != it_e; ++it )
        (*it)->m_Parent = n;
}

bool NodeList::Append( Node* n )
{
    if( m_Count >= s_MaxNodeCount )
        return false;

    n->m_IsChild        = true;
    m_Nodes[m_Count]    = n;
    ++m_Count;

    return true;
}

int NodeList::Size()
{
    return (int)m_Count;
}

