/* 
 * 
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com ) 
 * All rights reserved.
 * 
 * See LICENSE file for details
 * 
 */

#include <btree/btree_data.h>

int NodeGrist::s_GristCount;

NodeGrist::NodeGrist()
	: m_Chaff( 0x0 )
{
	++s_GristCount;
}

NodeGrist::~NodeGrist()
{
	--s_GristCount;
}


