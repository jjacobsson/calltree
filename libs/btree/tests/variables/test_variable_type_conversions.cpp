#include <UnitTest++.h>
#include <btree/variable.h>

const char* g_TestString = "A test string.";

class VariableTypeFixture
{
public:
	Variable v;
};

/*
 * Undefined test (lol)
 */

class VariableTypeUndefinedFixture : public VariableTypeFixture
{
public:
	VariableTypeUndefinedFixture()
	{
		v.m_Type = Variable::E_VART_UNDEFINED;
	}
};

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToUndefined )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToInteger )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToFloat )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToString )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeUndefinedFixture, CantConvertToBool )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_BOOL ) );
}

/*
 * Integer tests
 */

class VariableTypeIntegerFixture : public VariableTypeFixture
{
public:
	VariableTypeIntegerFixture()
	{
		v.m_Type = Variable::E_VART_INTEGER;
		v.m_Data.m_Integer = 17;
	}
};


TEST_FIXTURE( VariableTypeIntegerFixture, CantConvertToUndefined )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToInteger )
{
	CHECK( v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToFloat )
{
	CHECK( v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CantConvertToString )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, CanConvertToBool )
{
	CHECK( v.SafeToConvert( Variable::E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToIntegerConversionReturnsCorrectValue )
{
	v.m_Data.m_Integer = 357;
	CHECK( 357 == v.ValueAsInteger() );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToIntegerConversionDoesNotReturnIncorrectValue )
{
	v.m_Data.m_Integer = 357;
	CHECK( 4723 != v.ValueAsInteger() );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToFloatConversionReturnsCorrectValue )
{
	v.m_Data.m_Integer = 17;
	CHECK( 17.0f == v.ValueAsFloat() );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToFloatConversionDoesNotReturnIncorrectValue )
{
	v.m_Data.m_Integer = 17;
	CHECK( 17.0002f != v.ValueAsFloat() );
}

TEST_FIXTURE( VariableTypeIntegerFixture, IntegerToStringConversionReturnsNull )
{
	v.m_Data.m_Integer = 242754;
	CHECK( 0x0 == v.ValueAsString() );
}

TEST_FIXTURE( VariableTypeIntegerFixture, PositiveIntegerToBoolConversionReturnsTrue )
{
	v.m_Data.m_Integer = 1;
	CHECK( true == v.ValueAsBool() );
}

TEST_FIXTURE( VariableTypeIntegerFixture, NegativeIntegerToBoolConversionReturnsTrue )
{
	v.m_Data.m_Integer = -3746;
	CHECK( true == v.ValueAsBool() );
}

TEST_FIXTURE( VariableTypeIntegerFixture, ZeroIntegerToBoolConversionReturnsFalse )
{
	v.m_Data.m_Integer = 0;
	CHECK( false == v.ValueAsBool() );
}


/*
 * Float tests
 */


class VariableTypeFloatFixture : public VariableTypeFixture
{
public:
	VariableTypeFloatFixture()
	{
		v.m_Type = Variable::E_VART_FLOAT;
		v.m_Data.m_Float = 17.003f;
	}
};

TEST_FIXTURE( VariableTypeFloatFixture, FloatCantConvertToUndefined )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToInteger )
{
	CHECK( v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToFloat )
{
	CHECK( v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCantConvertToString )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatCanConvertToBool )
{
	CHECK( v.SafeToConvert( Variable::E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatConvertToIntegerReturnsCorrectValue )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( 17 == v.ValueAsInteger() );
}

TEST_FIXTURE( VariableTypeFloatFixture, FloatConvertToStringReturnsNull )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( 0x0 == v.ValueAsString() );
}

TEST_FIXTURE( VariableTypeFloatFixture, PositiveFloatConvertsToTrueBool )
{
	v.m_Data.m_Float = 17.002f;
	CHECK( true == v.ValueAsBool() );
}

TEST_FIXTURE( VariableTypeFloatFixture, NegativeFloatConvertsToTrueBool )
{
	v.m_Data.m_Float = -317.002f;
	CHECK( true == v.ValueAsBool() );
}

TEST_FIXTURE( VariableTypeFloatFixture, ZeroFloatConvertsToFalseBool )
{
	v.m_Data.m_Float = -317.002f;
	CHECK( true == v.ValueAsBool() );
}

/*
 * String Tests
 */

class VariableTypeStringFixture : public VariableTypeFixture
{
public:
	VariableTypeStringFixture()
	{
		v.m_Type = Variable::E_VART_STRING;
		v.m_Data.m_String = g_TestString;
	}
};

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToUndefined )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToInteger )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToFloat )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCanConvertToString )
{
	CHECK( v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringCantConvertToBool )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsToZeroInteger )
{
	CHECK( 0 == v.ValueAsInteger() );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsToZeroFloat )
{
	CHECK( 0.0f == v.ValueAsFloat() );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsCorrectlyToString )
{
	CHECK( g_TestString == v.ValueAsString() );
}

TEST_FIXTURE( VariableTypeStringFixture, StringConvertsCorrectlyToBool )
{
	CHECK( false == v.ValueAsBool() );
}


/*
 * Bool Tests
 */

class VariableTypeBoolFixture : public VariableTypeFixture
{
public:
	VariableTypeBoolFixture()
	{
		v.m_Type = Variable::E_VART_BOOL;
		v.m_Data.m_Bool = true;
	}
};

TEST_FIXTURE( VariableTypeBoolFixture, BoolCantConvertToUndefined )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToInteger )
{
	CHECK( v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToFloat )
{
	CHECK( v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCantConvertToString )
{
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolCanConvertToBool )
{
	CHECK( v.SafeToConvert( Variable::E_VART_BOOL ) );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToInteger )
{
	CHECK( 1 == v.ValueAsInteger() );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToFloat )
{
	CHECK( 1.0f == v.ValueAsFloat() );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsPredictablyToString )
{
	CHECK( false == v.ValueAsString() );
}

TEST_FIXTURE( VariableTypeBoolFixture, BoolConvertsCorrectlyToBool )
{
	CHECK( true == v.ValueAsBool() );
}

