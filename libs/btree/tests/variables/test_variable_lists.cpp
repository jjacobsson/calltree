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

class VariableListsWithIDs : public TestVariablesFixture
{
public:
	VariableListsWithIDs()
	{
		v.m_Next  = &v1;
		v1.m_Next = &v2;
		v2.m_Next = &v3;
		v3.m_Next = &v4;
		v4.m_Next = 0x0;

		v.m_Id.m_Hash  = 0xfefefefe;
		v1.m_Id.m_Hash = 0xcafebabe;
		v2.m_Id.m_Hash = 0xdeadbeef;
		v3.m_Id.m_Hash = 0xdeadc0de;
		v4.m_Id.m_Hash = 0xcafebabe;
	}

};


TEST_FIXTURE( VariableListsWithIDs, FindVariableWithIdReturnsCorrectSingleElementListWithCorrectId )
{
	CHECK( FindVariableWithIdHash( &v, 0xfefefefe ) == &v );
}

TEST_FIXTURE( VariableListsWithIDs, FindVarialbeWithIdReturnsCorrectMultiElementListWithCorrectId )
{
	CHECK( FindVariableWithIdHash( &v, 0xdeadc0de ) == &v3 );
}

TEST_FIXTURE( VariableListsWithIDs, FindVarialbeWithIdReturnsCorrectMultiElementListWithWrongId )
{
	CHECK( FindVariableWithIdHash( &v1, 0xcdcdcdcd ) == 0x0 );
}

TEST( CountOccourancesOfIdHashInListReturnsZeroOnEmptyList )
{
	CHECK( CountOccourancesOfIdHashInList( 0x0, 0 ) == 0 );
}

TEST_FIXTURE( VariableListsWithIDs, CountOccourancesOfIdHashInListCountsCorrectly )
{
	CHECK( CountOccourancesOfIdHashInList( &v, 0xdeadbeef ) == 1 );
	CHECK( CountOccourancesOfIdHashInList( &v, 0xcafebabe ) == 2 );
	CHECK( CountOccourancesOfIdHashInList( &v, 0xcdcdcdcd ) == 0 );
}

TEST( TestIfAllIDsAreUniqueInVariableListReturnsTrueOnEmptyList )
{
	CHECK( VariableIdsAreUniqueInList( 0x0 ) == true );
}

TEST_FIXTURE( TestVariablesFixture, TestIfAllIDsAreUniqueInVariableListSingleElementReturnsTrue )
{
	v.m_Id.m_Hash = 0xcafebabe;
	CHECK( VariableIdsAreUniqueInList( &v ) == true );
}

TEST_FIXTURE( VariableListsWithIDs, TestIfAllIDsAreUniqueInVariableListMultiElementReturnsFalse )
{
	CHECK( VariableIdsAreUniqueInList( &v ) == false );
}

TEST_FIXTURE( VariableListsWithIDs, estIfAllIDsAreUniqueInVariableListMultiElementReturnsTrue )
{
	v4.m_Id.m_Hash = 0xcdcdcdcd;
	CHECK( VariableIdsAreUniqueInList( &v ) == true );
}


