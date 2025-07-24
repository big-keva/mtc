# include "../test-it-easy.hpp"
# include "../bitset.h"
# include <cstdio>

using namespace mtc;

TestItEasy::RegisterFunc  testBitset( []()
{
  TEST_CASE( "mtc/bitset" )
  {
    SECTION( "it is implemented on integers," )
    {
      uint64_t  u = 0;

      SECTION( "*_set" )
      {
        REQUIRE_NOTHROW( bitset_set( u, { 0, 1 } ) );
        REQUIRE_NOTHROW( bitset_set( u, { 31, 32 } ) );
        REQUIRE_NOTHROW( bitset_set( u, 63 ) );
          REQUIRE( u == 0x8000000180000003 );
        REQUIRE_EXCEPTION( bitset_set( u, { 62, 65 } ), std::invalid_argument );
      }
      SECTION( "*_get" )
      {
        REQUIRE( bitset_get( u, { 0, 1 } ) );
        REQUIRE( bitset_get( u, { 31, 32 } ) );
        REQUIRE( bitset_get( u, 63 ) );
        REQUIRE( bitset_get( u, { 62, 65 } ) );
      }
      SECTION( "*_del" )
      {
        REQUIRE_NOTHROW( bitset_del( u, 1 ) );
          REQUIRE( bitset_get( u, 1 ) == false );
        REQUIRE_NOTHROW( bitset_del( u, 31 ) );
          REQUIRE( bitset_get( u, 31 ) == false );
        REQUIRE_NOTHROW( bitset_del( u, { 62, 63 } ) );
          REQUIRE( bitset_get( u, { 62, 65 } ) == false );
      }
    }
    SECTION( "on vectors" )
    {
      std::vector<uint64_t> bs_1;

      SECTION( "*_set" )
      {
        REQUIRE_NOTHROW( bitset_set( bs_1, { 62, 64 } ) );
        REQUIRE_NOTHROW( bitset_set( bs_1, { 127, 192 } ) );
          REQUIRE_NOTHROW( bs_1[0] == 0xc000000000000000ULL );
          REQUIRE_NOTHROW( bs_1[1] == 0x8000000000000001ULL );
          REQUIRE_NOTHROW( bs_1[2] == 0xffffffffffffffffULL );
          REQUIRE_NOTHROW( bs_1[3] == 0x0000000000000001ULL );
      }
      SECTION( "*_get" )
      {
        REQUIRE( bitset_get( bs_1, { 61, 62 } ) );
        REQUIRE( bitset_get( bs_1, { 62, 65 } ) );
        REQUIRE( bitset_get( bs_1, { 65, 126 } ) == false );
        REQUIRE( bitset_get( bs_1, 129 ) );
        REQUIRE( bitset_get( bs_1, { 193, 1000 } ) == false );
      }
      SECTION( "*_del" )
      {
        REQUIRE_NOTHROW( bitset_del( bs_1, { 60, 62 } ) );
          REQUIRE( bitset_get( bs_1, { 61, 62 } ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, { 62, 65 } ) );
          REQUIRE( bitset_get( bs_1, { 63, 65 } ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, { 65, 126 } ) );
          REQUIRE( bitset_get( bs_1, { 63, 126 } ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, 129 ) );
          REQUIRE( bitset_get( bs_1, 127 ) == true );
          REQUIRE( bitset_get( bs_1, 128 ) == true );
          REQUIRE( bitset_get( bs_1, 129 ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, { 193, 1000 } ) );
          REQUIRE_NOTHROW( bitset_del( bs_1, { 193, 1000 } ) );
      }
    }
    SECTION( "on arrays" )
    {
      uint64_t  bs_1[4] = { 0, 0, 0, 0 };

      SECTION( "*_set" )
      {
        REQUIRE_NOTHROW( bitset_set( bs_1, { 62, 64 } ) );
        REQUIRE_NOTHROW( bitset_set( bs_1, { 127, 192 } ) );
          REQUIRE_NOTHROW( bs_1[0] == 0xc000000000000000ULL );
          REQUIRE_NOTHROW( bs_1[1] == 0x8000000000000001ULL );
          REQUIRE_NOTHROW( bs_1[2] == 0xffffffffffffffffULL );
          REQUIRE_NOTHROW( bs_1[3] == 0x0000000000000001ULL );
        REQUIRE_EXCEPTION( bitset_set( bs_1, 1024 ), std::invalid_argument );
      }
      SECTION( "*_get" )
      {
        REQUIRE( bitset_get( bs_1, { 61, 62 } ) );
        REQUIRE( bitset_get( bs_1, { 62, 65 } ) );
        REQUIRE( bitset_get( bs_1, { 65, 126 } ) == false );
        REQUIRE( bitset_get( bs_1, 129 ) );
        REQUIRE( bitset_get( bs_1, { 193, 1000 } ) == false );
      }
      SECTION( "*_del" )
      {
        REQUIRE_NOTHROW( bitset_del( bs_1, { 60, 62 } ) );
          REQUIRE( bitset_get( bs_1, { 61, 62 } ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, { 62, 65 } ) );
          REQUIRE( bitset_get( bs_1, { 63, 65 } ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, { 65, 126 } ) );
          REQUIRE( bitset_get( bs_1, { 63, 126 } ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, 129 ) );
          REQUIRE( bitset_get( bs_1, 127 ) == true );
          REQUIRE( bitset_get( bs_1, 128 ) == true );
          REQUIRE( bitset_get( bs_1, 129 ) == false );
        REQUIRE_NOTHROW( bitset_del( bs_1, { 193, 1000 } ) );
          REQUIRE_NOTHROW( bitset_del( bs_1, { 193, 1000 } ) );
      }
    }
    SECTION( "bitset bit count may be calculated" )
    {
      uint64_t              bs_1 = 3;
      uint32_t              bs_2[3] = { 1, 2, 3 };
      std::vector<uint16_t> bs_3{ 1, 2, 3, 4 };

      REQUIRE( bitset_count( bs_1 ) == 2 );
      REQUIRE( bitset_count( bs_2 ) == 4 );
    }
  }

} );
