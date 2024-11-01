# include "../test-it-easy.hpp"
# include "../arena.hpp"
# include <cstdio>

TestItEasy::RegisterFunc  testArena( []()
{

  TEST_CASE( "mtc/Arena" )
  {
    mtc::Arena  arena;

    SECTION( "it is created with zero allocations and size" )
    {
      REQUIRE( arena.memcount() == 0U );
      REQUIRE( arena.memusage() == 0U );
    }
    SECTION( "it allocates blocks with alignment" )
    {
      auto  p1 = arena.allocate( 1023, 8 );

      REQUIRE( arena.memcount() == 1U );
      REQUIRE( arena.memusage() == 1024U );
      REQUIRE( ((uintptr_t)( p1 ) & 0x03) == 0U );

      auto  p2 = arena.allocate( 17, 2 );

      REQUIRE( arena.memcount() == 2U );
      REQUIRE( arena.memusage() == 1042U );
      REQUIRE( ((uintptr_t)( p1 ) & 0x01) == 0U );
    }
  }

} );