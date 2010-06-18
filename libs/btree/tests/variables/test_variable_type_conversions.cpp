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

const char* g_TestString = "A test string.";

/*
 * Undefined test (lol)
 */

class VariableTypeUndefinedFixture : public TestVariablesFixture
{
public:
	VariableTypeUndefinedFixture()
	{
		v.m_Type = E_VART_UNDEFINED;
	}
};

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToUndefined )
{
	CHECK( !safe_to_convert( &v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToInteger )
{
	CHECK( !safe_to_convert( &v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToFloat )
{
	CHECK( !safe_to_convert( &v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToString )
{
	CHECK( !safe_to_convert( &v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToBool )
{
	CHECK( !safe_to_convert( &v, E_VART_BOOL ) );
}

/*
 * Integer tests
 */

class VariableTypeIntegerFixture : public TestVariablesFixture
{
public:
	VariableTypeIntegerFixture()
	{
		v.m_Type = E_VART_INTEGER;
		v.m_Data.m_Integer = 17;
	}
};


TEST_FIXTURE( VariableTypeIntegerFixture, CantConvertToUndefined )
{
	CHECK( !safe_to_convert( &v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToInteger )
{
	CHECK( safe_to_convert( &v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToFloat )
{
	CHECK( safe_to_convert( &v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CantConvertToString )
{
	CHECK( !safe_to_convert( &v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToBool )
{
	CHECK( safe_to_convert( &v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToIntegerConversionReturnsCorrectValue )
{
	v.m_Data.m_Integer = 357;
	CHECK( 357 == as_integer( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToIntegerConversionDoesNotReturnIncorrectValue )
{
	v.m_Data.m_Integer = 357;
	CHECK( 4723 != as_integer( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToFloatConversionReturnsCorrectValue )
{
	v.m_Data.m_Integer = 17;
	CHECK( 17.0f == as_float( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToFloatConversionDoesNotReturnIncorrectValue )
{
	v.m_Data.m_Integer = 17;
	CHECK( 17.0002f != as_float( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToStringConversionReturnsNull )
{
	v.m_Data.m_Integer = 242754;
	CHECK( 0x0 == as_string( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, PositiveIntegerToBoolConversionReturnsTrue )
{
	v.m_Data.m_Integer = 1;
	CHECK( true == as_bool( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, NegativeIntegerToBoolConversionReturnsTrue )
{
	v.m_Data.m_Integer = -3746;
	CHECK( true == as_bool( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, ZeroIntegerToBoolConversionReturnsFalse )
{
	v.m_Data.m_Integer = 0;
	CHECK( false == as_bool( v ) );
}


/*
 * Float tests
 */


class VariableTypeFloatFixture : public TestVariablesFixture
{
public:
	VariableTypeFloatFixture()
	{
		v.m_Type = E_VART_FLOAT;
		v.m_Data.m_Float = 17.003f;
	}
};

TEST_FIXTURE( VariableTypeFloatFixture, FloatCantConvertToUndefined )
{
	CHECK( !safe_to_convert( &v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToInteger )
{
	CHECK( safe_to_convert( &v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToFloat )
{
	CHECK( safe_to_convert( &v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCantConvertToString )
{
	CHECK( !safe_to_convert( &v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToBool )
{
	CHECK( safe_to_convert( &v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatConvertToIntegerReturnsCorrectValue )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( 17 == as_integer( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatConvertToStringReturnsNull )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( 0x0 == as_string( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, PositiveFloatConvertsToTrueBool )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( true == as_bool( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, NegativeFloatConvertsToTrueBool )
{
	v.m_Data.m_Float = -317.002f;
	CHECK( true == as_bool( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, ZeroFloatConvertsToFalseBool )
{
	v.m_Data.m_Float = -317.002f;
	CHECK( true == as_bool( v ) );
}

/*
 * String Tests
 */

class VariableTypeStringFixture : public TestVariablesFixture
{
public:
	VariableTypeStringFixture()
	{
		v.m_Type = E_VART_STRING;
		v.m_Data.m_String.m_Parsed = g_TestString;
	}
};

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToUndefined )
{
	CHECK( !safe_to_convert( &v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToInteger )
{
	CHECK( !safe_to_convert( &v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToFloat )
{
	CHECK( !safe_to_convert( &v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCanConvertToString )
{
	CHECK( safe_to_convert( &v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToBool )
{
	CHECK( !safe_to_convert( &v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsToZeroInteger )
{
	CHECK( 0 == as_integer( v ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsToZeroFloat )
{
	CHECK( 0.0f == as_float( v ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsCorrectlyToString )
{
	CHECK( g_TestString == as_string( v )->m_Parsed );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsCorrectlyToBool )
{
	CHECK( true == as_bool( v ) );
}


/*
 * Bool Tests
 */

class VariableTypeBoolFixture : public TestVariablesFixture
{
public:
	VariableTypeBoolFixture()
	{
		v.m_Type = E_VART_BOOL;
		v.m_Data.m_Bool = true;
	}
};

TEST_FIXTURE( VariableTypeBoolFixture, BoolCantConvertToUndefined )
{
	CHECK( !safe_to_convert( &v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToInteger )
{
	CHECK( safe_to_convert( &v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToFloat )
{
	CHECK( safe_to_convert( &v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCantConvertToString )
{
	CHECK( !safe_to_convert( &v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToBool )
{
	CHECK( safe_to_convert( &v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToInteger )
{
	CHECK( 0xffffffff == as_integer( v ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToFloat )
{
	CHECK( 1.0f == as_float( v ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsPredictablyToString )
{
	CHECK( false == as_string( v ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToBool )
{
	CHECK( true == as_bool( v ) );
}

