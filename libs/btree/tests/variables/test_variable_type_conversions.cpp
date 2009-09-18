#include <UnitTest++.h>
#include <btree/variable.h>

/*
 * Undefined test (lol)
 */

TEST( UndefinedCantConvertToUndefined )
{
	Variable v;
	v.m_Type = Variable::E_VART_UNDEFINED;
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST( UndefinedCantConvertToInteger )
{
	Variable v;
	v.m_Type = Variable::E_VART_UNDEFINED;
	CHECK( !v.SafeToConvert( Variable::E_VART_INTEGER ) );
}

TEST( UndefinedCantConvertToFloat )
{
	Variable v;
	v.m_Type = Variable::E_VART_UNDEFINED;
	CHECK( !v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST( UndefinedCantConvertToString )
{
	Variable v;
	v.m_Type = Variable::E_VART_UNDEFINED;
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST( UndefinedCantConvertToBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_UNDEFINED;
	CHECK( !v.SafeToConvert( Variable::E_VART_BOOL ) );
}

/*
 * Integer tests
 */

TEST( IntegerCantConvertToUndefined )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	CHECK( !v.SafeToConvert( Variable::E_VART_UNDEFINED ) );
}

TEST( IntegerCanConvertToInteger )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	CHECK( v.SafeToConvert( Variable::E_VART_INTEGER ) );
}


TEST( IntegerCanConvertToFloat )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	CHECK( v.SafeToConvert( Variable::E_VART_FLOAT ) );
}

TEST( IntegerCantConvertToString )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	CHECK( !v.SafeToConvert( Variable::E_VART_STRING ) );
}

TEST( IntegerCanConvertToBool )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	CHECK( v.SafeToConvert( Variable::E_VART_BOOL ) );
}

TEST( IntegerToIntegerConversionReturnsCorrectValue )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	v.m_Data.m_Integer = 357;
	CHECK( 357 == v.ValueAsInteger() );
}

TEST( IntegerToIntegerConversionDoesNotReturnIncorrectValue )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	v.m_Data.m_Integer = 357;
	CHECK( 4723 != v.ValueAsInteger() );
}

TEST( IntegerToFloatConversionReturnsCorrectValue )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	v.m_Data.m_Integer = 17;
	CHECK( 17.0f == v.ValueAsFloat() );
}

TEST( IntegerToFloatConversionDoesNotReturnIncorrectValue )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	v.m_Data.m_Integer = 17;
	CHECK( 17.0002f != v.ValueAsFloat() );
}

TEST( IntegerToStringConversionReturnsNull )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	v.m_Data.m_Integer = 242754;
	CHECK( 0x0 == v.ValueAsString() );
}

TEST( PositiveIntegerToBoolConversionReturnsTrue )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	v.m_Data.m_Integer = 1;
	CHECK( true == v.ValueAsBool() );
}

TEST( NegativeIntegerToBoolConversionReturnsTrue )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
	v.m_Data.m_Integer = -3746;
	CHECK( true == v.ValueAsBool() );
}

TEST( ZeroIntegerToBoolConversionReturnsFalse )
{
	Variable v;
	v.m_Type = Variable::E_VART_INTEGER;
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

