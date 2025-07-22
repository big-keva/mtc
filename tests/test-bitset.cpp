# include "../test-it-easy.hpp"
# include "../bitset.h"
# include <cstdio>

using namespace mtc;

TestItEasy::RegisterFunc  testBitset( []()
{
  TEST_CASE( "mtc/bitset" )
  {
    SECTION( "it is implemented on different collectors" )
    {
      std::vector<char> bs_1;
      std::vector<char> bs_2;

      SECTION( "bitset may be set" )
      {
        REQUIRE_NOTHROW( bitset_set( bs_1, 3 ) );
        REQUIRE_NOTHROW( bitset_set( bs_2, 3 ) );
        REQUIRE_NOTHROW( bitset_set( bs_2, 3621 ) );

        SECTION( "values set may be got" )
        {
          REQUIRE( bitset_get( bs_1, 3 ) );
          REQUIRE( bitset_get( bs_1, 2 ) == false );
          REQUIRE( bitset_get( bs_2, 3 ) );
          REQUIRE( bitset_get( bs_2, 3621 ) );
        }
      }

      SECTION( "bitsets may be intersected" )
      {
        REQUIRE_NOTHROW( bitset_and( bs_1, bs_2 ) );
        REQUIRE( bitset_get( bs_1, 3 ) );
      }
    }
    SECTION( "bitset bit count may be calculated" )
    {
      uint64_t              bs_1 = 3;
      uint32_t              bs_2[3] = { 1, 2, 3 };
      std::vector<uint16_t> bs_3{ 1, 2, 3, 4 };

      REQUIRE( popcount( bs_1 ) == 2 );
      REQUIRE( popcount( bs_2 ) == 4 );
    }
  }

} );
