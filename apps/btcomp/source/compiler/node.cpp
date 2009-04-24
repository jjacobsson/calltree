/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#include "node.h"
#include "visitor.h"
#include "../btree/btree.h"
#include "node_grist.h"

Node::~Node()
{
	if( m_Tree )
	{
		m_Tree->FreeId( m_Id );
	}
}

void Node::Visit( INodeVisitor* nv )
{
	if( m_Grist )
		nv->Visit( this );

	m_Grist->Visit( nv );
}
