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

TEST_FIXTURE( TestVariablesFixture, InitVariableSetsTypeToUndefined )
{
	CHECK( v.m_Type == E_VART_UNDEFINED );
}

TEST_FIXTURE( TestVariablesFixture, InitVariableInitsTheID )
{
	CHECK( v.m_Id.m_Text == 0x0 );
	CHECK( v.m_Id.m_Hash == INVALID_ID );
	CHECK( v.m_Id.m_Line == 0 );
}

TEST_FIXTURE( TestVariablesFixture, InitVariableInitsDataToZero )
{
	CHECK( v.m_Data.m_Integer == 0x0 );
	CHECK( v.m_Data.m_Float == 0.0f );
	CHECK( v.m_Data.m_String == 0x0 );
	CHECK( v.m_Data.m_Bool == false );
}

TEST_FIXTURE( TestVariablesFixture, InitVariableSetsNextToNull )
{
	CHECK( v.m_Next == 0x0 );
}
