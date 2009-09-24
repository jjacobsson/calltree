/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#include <btree/btree.h>

void Node::Visit( INodeVisitor* nv )
{
	if( m_Grist )
		nv->Visit( this );

	m_Grist->Visit( nv );
}
