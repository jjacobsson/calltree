/*******************************************************************************
 * Copyright (c) 2009-04-24 Joacim Jacobsson.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Joacim Jacobsson - first implementation
 *******************************************************************************/

#include <UnitTest++.h>
#include "test_variables.h"

TEST( FindLastVariableReturnsNullOnNull )
{
	CHECK( find_last( 0x0 ) == 0x0 );
}

TEST_FIXTURE( TestVariablesFixture, FindLastVariableReturnsCorrectWithSingleElementList )
{
	CHECK( find_last( &v ) == &v );
}

TEST_FIXTURE( TestVariablesFixture, FindLastVariableReturnsCorrectWithMultiElementList )
{
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	CHECK( find_last( &v1 ) == &v4 );
}

TEST( CountElementsReturnsCorrectNumberWithEmptyList )
{
	CHECK( count_elements( 0x0 ) == 0 );
}

TEST_FIXTURE( TestVariablesFixture, CountElementsReturnsCorrectNumberWithSingleElementList )
{
	CHECK( count_elements( &v ) == 1 );
}

TEST_FIXTURE( TestVariablesFixture, CountElementsReturnsCorrectNumberWithMultiElementList )
{
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	CHECK( count_elements( &v1 ) == 4 );
}

TEST_FIXTURE( TestVariablesFixture, AppendVariableToEndOfList )
{
	v1.m_Next = &v2;
	v2.m_Next = &v3;
	v3.m_Next = 0x0;
	v4.m_Next = 0x0;

	append_to_end( &v1, &v4 );

	CHECK( &v4 == find_last( &v1 ) );
}

TEST_FIXTURE( TestVariablesFixture, AppendListToEndOfList )
{
	v1.m_Next = &v2;
	v2.m_Next = 0x0;
	v3.m_Next = &v4;
	v4.m_Next = 0x0;
	append_to_end( &v1, &v3 );
	CHECK( &v4 == find_last( &v1 ) );
}

TEST_FIXTURE( TestVariablesFixture, FindVariableWithIdReturnsCorrectWithEmptyList )
{
	CHECK( find_by_hash( 0x0, 0 ) == 0x0 );
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
	CHECK( find_by_hash( &v, 0xfefefefe ) == &v );
}

TEST_FIXTURE( VariableListsWithIDs, FindVarialbeWithIdReturnsCorrectMultiElementListWithCorrectId )
{
	CHECK( find_by_hash( &v, 0xdeadc0de ) == &v3 );
}

TEST_FIXTURE( VariableListsWithIDs, FindVarialbeWithIdReturnsCorrectMultiElementListWithWrongId )
{
	CHECK( find_by_hash( &v1, 0xcdcdcdcd ) == 0x0 );
}

TEST( CountOccourancesOfIdHashInListReturnsZeroOnEmptyList )
{
	CHECK( count_occourances_of_hash_in_list( 0x0, 0 ) == 0 );
}

TEST_FIXTURE( VariableListsWithIDs, CountOccourancesOfIdHashInListCountsCorrectly )
{
	CHECK( count_occourances_of_hash_in_list( &v, 0xdeadbeef ) == 1 );
	CHECK( count_occourances_of_hash_in_list( &v, 0xcafebabe ) == 2 );
	CHECK( count_occourances_of_hash_in_list( &v, 0xcdcdcdcd ) == 0 );
}

TEST( TestIfAllIDsAreUniqueInVariableListReturnsTrueOnEmptyList )
{
	CHECK( id_hashes_are_unique_in_list( 0x0 ) == true );
}

TEST_FIXTURE( TestVariablesFixture, TestIfAllIDsAreUniqueInVariableListSingleElementReturnsTrue )
{
	v.m_Id.m_Hash = 0xcafebabe;
	CHECK( id_hashes_are_unique_in_list( &v ) == true );
}

TEST_FIXTURE( VariableListsWithIDs, TestIfAllIDsAreUniqueInVariableListMultiElementReturnsFalse )
{
	CHECK( id_hashes_are_unique_in_list( &v ) == false );
}

TEST_FIXTURE( VariableListsWithIDs, estIfAllIDsAreUniqueInVariableListMultiElementReturnsTrue )
{
	v4.m_Id.m_Hash = 0xcdcdcdcd;
	CHECK( id_hashes_are_unique_in_list( &v ) == true );
}


