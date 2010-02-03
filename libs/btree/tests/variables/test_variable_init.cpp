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
	CHECK( v.m_Data.m_String.m_Parsed == 0x0 );
	CHECK( v.m_Data.m_String.m_Raw == 0x0 );
	CHECK( v.m_Data.m_Bool == false );
}

TEST_FIXTURE( TestVariablesFixture, InitVariableSetsNextToNull )
{
	CHECK( v.m_Next == 0x0 );
}
