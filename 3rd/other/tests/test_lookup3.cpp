#include <UnitTest++.h>
#include <other/lookup3.h>

TEST( IsItInTheArchive )
{
    int bla = hashlittle( "A test string" );
    CHECK( bla == hashlittle( "A test string" ) );
}
