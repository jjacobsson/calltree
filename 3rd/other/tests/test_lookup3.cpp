#include <UnitTest++.h>
#include <other/lookup3.h>

#include <stdio.h>

TEST( hashlittleProducesPredictableValues )
{
  unsigned int hash = hashlittle( "A test string" );
  CHECK( hash == 0xe0ecf1a7 );
}
