/*
 *
 * Copyright (C) 2009, Joacim Jacobsson ( j dot jacobsson at gmail dot com )
 * All rights reserved.
 *
 * See LICENSE file for details
 *
 */

#include <UnitTest++.h>
#include "test_variables.h"
#include <btree/identifier.h>

TEST( FindLastVariableReturnsNullOnNull )
{
	CHECK( FindLastVariable( 0x0 ) == 0x0 );
}

TEST_FIXTURE( TestVariablesFixture, FindLastVariableReturnsCorrectWithSingleElementList )
{
	CHECK( FindLastVariable( &v ) == &v );
}

TEST_FIXTURE( TestVariablesFixture, FindLastVariableReturnsCorrectWithMultiElementList )
{
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	CHECK( FindLastVariable( &v1 ) == &v4 );
}

TEST( CountElementsReturnsCorrectNumberWithEmptyList )
{
	CHECK( CountElementsInList( 0x0 ) == 0 );
}

TEST_FIXTURE( TestVariablesFixture, CountElementsReturnsCorrectNumberWithSingleElementList )
{
	CHECK( CountElementsInList( &v ) == 1 );
}

TEST_FIXTURE( TestVariablesFixture, CountElementsReturnsCorrectNumberWithMultiElementList )
{
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	CHECK( CountElementsInList( &v1 ) == 4 );
}

TEST_FIXTURE( TestVariablesFixture, AppendVariableToEndOfList )
{
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = 0x0;
	v4.m_Next = 0x0;

	AppendToEndOfList( &v1, &v4 );

	CHECK( &v4 == FindLastVariable( &v1 ) );
}

TEST_FIXTURE( TestVariablesFixture, AppendListToEndOfList )
{
	v1.m_Next = &v2;
	v2.m_Next = 0x0;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	AppendToEndOfList( &v1, &v3 );
	CHECK( &v4 == FindLastVariable( &v1 ) );
}

TEST_FIXTURE( TestVariablesFixture, FindVariableWithIdReturnsCorrectWithEmptyList )
{
	CHECK( FindVariableWithIdHash( 0x0, 0 ) == 0x0 );
}

TEST_FIXTURE( TestVariablesFixture, FindVariableWithIdReturnsCorrectSingleElementListWithCorrectId )
{
	Identifier id;

	id.m_Hash	= 0xdeadbeef;
	v.m_Id		= &id;

	CHECK( FindVariableWithIdHash( &v, 0xdeadbeef ) == &v );
}

TEST_FIXTURE( TestVariablesFixture, FindVarialbeWithIdReturnsCorrectMultiElementListWithCorrectId )
{
	Identifier id;
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	id.m_Hash = 0xdeadbeef;
	v3.m_Id = &id;
	CHECK( FindVariableWithIdHash( &v1, 0xdeadbeef ) == &v3 );
}

TEST_FIXTURE( TestVariablesFixture, FindVarialbeWithIdReturnsCorrectMultiElementListWithWrongId )
{
	Identifier id;
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	id.m_Hash = 0xcafebabe;
	v3.m_Id = &id;
	CHECK( FindVariableWithIdHash( &v1, 0xdeadbeef ) == 0x0 );
}


