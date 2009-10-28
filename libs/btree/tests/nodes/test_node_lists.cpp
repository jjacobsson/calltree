/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <UnitTest++.h>
#include <btree/btree_data.h>
#include <btree/btree_func.h>

TEST( TestUnlinkParentAndSiblingsDoesNotBreakList )
{
	Node n1,n2,n3;

	InitNode( &n1 );
	InitNode( &n2 );
	InitNode( &n3 );

	n1.m_Id.m_Hash = 1;
	n2.m_Id.m_Hash = 2;
	n3.m_Id.m_Hash = 3;

	AppendToEndOfList( &n1, &n2 );
	AppendToEndOfList( &n1, &n3 );

	CHECK( n1.m_Next == &n2 );
	CHECK( n1.m_Prev == 0x0 );
	CHECK( n2.m_Next == &n3 );
	CHECK( n2.m_Prev == &n1 );
	CHECK( n3.m_Next == 0x0 );
	CHECK( n3.m_Prev == &n2 );

	UnlinkNodeFromParentAndSiblings( &n2 );


	CHECK( n1.m_Next == &n3 );
	CHECK( n1.m_Prev == 0x0 );
	CHECK( n2.m_Next == 0x0 );
	CHECK( n2.m_Prev == 0x0 );
	CHECK( n3.m_Next == 0x0 );
	CHECK( n3.m_Prev == &n1 );
}

TEST( GetFirstChildReturnsNullOnNull )
{
	CHECK( GetFirstChild( 0x0 ) == 0x0 );
}
