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

TEST( FloatCantConvertToUndefined )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST( FloatCanConvertToInteger )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	CHECK( v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST( FloatCanConvertToFloat )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	CHECK( v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST( FloatCantConvertToString )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST( FloatCanConvertToBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	CHECK( v.SafeToConvert( Variable::E_VART_BOOL ) );
}

TEST( FloatConvertToIntegerReturnsCorrectValue )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	v.m_Data.m_Float = 17.002f;
	CHECK( 17 == v.ValueAsInteger() );
}

TEST( FloatConvertToStringReturnsNull )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	v.m_Data.m_Float = 17.002f;
	CHECK( 0x0 == v.ValueAsString() );
}

TEST( PositiveFloatConvertsToTrueBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	v.m_Data.m_Float = 17.002f;
	CHECK( true == v.ValueAsBool() );
}

TEST( NegativeFloatConvertsToTrueBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	v.m_Data.m_Float = -317.002f;
	CHECK( true == v.ValueAsBool() );
}

TEST( ZeroFloatConvertsToFalseBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_FLOAT;
	v.m_Data.m_Float = -317.002f;
	CHECK( true == v.ValueAsBool() );
}

/*
 * String Tests
 */

TEST( StringCantConvertToUndefined )
{
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST( StringCantConvertToInteger )
{
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	CHECK( !v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST( StringCantConvertToFloat )
{
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	CHECK( !v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST( StringCanConvertToString )
{
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	CHECK( v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST( StringCantConvertToBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	CHECK( !v.SafeToConvert( Variable::E_VART_BOOL ) );
}

TEST( StringConvertsToZeroInteger )
{
	const char* test_str = "A test string.";
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	v.m_Data.m_String = test_str;
	CHECK( 0 == v.ValueAsInteger() );
}

TEST( StringConvertsToZeroFloat )
{
	const char* test_str = "A test string.";
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	v.m_Data.m_String = test_str;
	CHECK( 0.0f == v.ValueAsFloat() );
}


TEST( StringConvertsCorrectlyToString )
{
	const char* test_str = "A test string.";
	Variable v;
	v.m_Type = Variable::E_VART_STRING;
	v.m_Data.m_String = test_str;
	CHECK( test_str == v.ValueAsString() );
}


/*
 * Bool Tests
 */

TEST( BoolCantConvertToUndefined )
{
	Variable v;
	v.m_Type = Variable::E_VART_BOOL;
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST( BoolCanConvertToInteger )
{
	Variable v;
	v.m_Type = Variable::E_VART_BOOL;
	CHECK( v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST( BoolCanConvertToFloat )
{
	Variable v;
	v.m_Type = Variable::E_VART_BOOL;
	CHECK( v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST( BoolCantConvertToString )
{
	Variable v;
	v.m_Type = Variable::E_VART_BOOL;
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST( BoolCanConvertToBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_BOOL;
	CHECK( v.SafeToConvert( Variable::E_VART_BOOL ) );
}

