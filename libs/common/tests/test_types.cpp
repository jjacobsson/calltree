#include <UnitTest++.h>
#include <common/types.h>

TEST( uint8_is_one_byte )
{
    CHECK( sizeof( uint8 ) == 1 );
}

TEST( uint16_is_two_bytes )
{
    CHECK( sizeof( uint16 ) == 2 );
}

TEST( uint32_is_four_bytes )
{
    CHECK( sizeof( uint32 ) == 4 );
}

TEST( uint64_is_eight_bytes )
{
    CHECK( sizeof( uint64 ) == 8 );
}

TEST( pointer_is_same_as_uint32 )
{
    CHECK( sizeof( void* ) == sizeof( uint32 ) );
}
