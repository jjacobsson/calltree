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
	CHECK( !SafeToConvert( v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToInteger )
{
	CHECK( !SafeToConvert( v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToFloat )
{
	CHECK( !SafeToConvert( v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToString )
{
	CHECK( !SafeToConvert( v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToBool )
{
	CHECK( !SafeToConvert( v, E_VART_BOOL ) );
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
	CHECK( !SafeToConvert( v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToInteger )
{
	CHECK( SafeToConvert( v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToFloat )
{
	CHECK( SafeToConvert( v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CantConvertToString )
{
	CHECK( !SafeToConvert( v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToBool )
{
	CHECK( SafeToConvert( v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToIntegerConversionReturnsCorrectValue )
{
	v.m_Data.m_Integer = 357;
	CHECK( 357 == ValueAsInteger( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToIntegerConversionDoesNotReturnIncorrectValue )
{
	v.m_Data.m_Integer = 357;
	CHECK( 4723 != ValueAsInteger( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToFloatConversionReturnsCorrectValue )
{
	v.m_Data.m_Integer = 17;
	CHECK( 17.0f == ValueAsFloat( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToFloatConversionDoesNotReturnIncorrectValue )
{
	v.m_Data.m_Integer = 17;
	CHECK( 17.0002f != ValueAsFloat( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToStringConversionReturnsNull )
{
	v.m_Data.m_Integer = 242754;
	CHECK( 0x0 == ValueAsString( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, PositiveIntegerToBoolConversionReturnsTrue )
{
	v.m_Data.m_Integer = 1;
	CHECK( true == ValueAsBool( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, NegativeIntegerToBoolConversionReturnsTrue )
{
	v.m_Data.m_Integer = -3746;
	CHECK( true == ValueAsBool( v ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, ZeroIntegerToBoolConversionReturnsFalse )
{
	v.m_Data.m_Integer = 0;
	CHECK( false == ValueAsBool( v ) );
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
	CHECK( !SafeToConvert( v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToInteger )
{
	CHECK( SafeToConvert( v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToFloat )
{
	CHECK( SafeToConvert( v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCantConvertToString )
{
	CHECK( !SafeToConvert( v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToBool )
{
	CHECK( SafeToConvert( v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatConvertToIntegerReturnsCorrectValue )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( 17 == ValueAsInteger( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatConvertToStringReturnsNull )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( 0x0 == ValueAsString( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, PositiveFloatConvertsToTrueBool )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( true == ValueAsBool( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, NegativeFloatConvertsToTrueBool )
{
	v.m_Data.m_Float = -317.002f;
	CHECK( true == ValueAsBool( v ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, ZeroFloatConvertsToFalseBool )
{
	v.m_Data.m_Float = -317.002f;
	CHECK( true == ValueAsBool( v ) );
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
	CHECK( !SafeToConvert( v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToInteger )
{
	CHECK( !SafeToConvert( v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToFloat )
{
	CHECK( !SafeToConvert( v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCanConvertToString )
{
	CHECK( SafeToConvert( v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToBool )
{
	CHECK( !SafeToConvert( v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsToZeroInteger )
{
	CHECK( 0 == ValueAsInteger( v ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsToZeroFloat )
{
	CHECK( 0.0f == ValueAsFloat( v ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsCorrectlyToString )
{
	CHECK( g_TestString == ValueAsString( v )->m_Parsed );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsCorrectlyToBool )
{
	CHECK( false == ValueAsBool( v ) );
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
	CHECK( !SafeToConvert( v, E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToInteger )
{
	CHECK( SafeToConvert( v, E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToFloat )
{
	CHECK( SafeToConvert( v, E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCantConvertToString )
{
	CHECK( !SafeToConvert( v, E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToBool )
{
	CHECK( SafeToConvert( v, E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToInteger )
{
	CHECK( 1 == ValueAsInteger( v ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToFloat )
{
	CHECK( 1.0f == ValueAsFloat( v ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsPredictablyToString )
{
	CHECK( false == ValueAsString( v ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToBool )
{
	CHECK( true == ValueAsBool( v ) );
}

