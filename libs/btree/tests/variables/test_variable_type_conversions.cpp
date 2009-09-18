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

