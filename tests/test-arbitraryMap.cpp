# include "../arbitrary_map.h"
# include "../test-it-easy.hpp"
# include <map>

using namespace mtc;

TestItEasy::RegisterFunc  indexer( []()
  {
    TEST_CASE( "mtc/arbitrary_map" )
    {
      auto  map = arbitrary_map<int>();

      SECTION( "map may be created as initializer list" )
      {
        auto  bbb = arbitrary_map<int>::value_type{ { "bbb", 3 }, 2 };
        auto  ccc = arbitrary_map<int>::value_type{ { "ccc", 3 }, 3 };

        REQUIRE_NOTHROW( map = arbitrary_map<int>( {
          { { "aaa", 3 }, 1 } } ) );
        map.insert( bbb );
        map.insert( std::move( ccc ) );
        REQUIRE_NOTHROW( map.at( { "aaa", 3 } ) );
        REQUIRE_NOTHROW( map.at( { "bbb", 3 } ) );
        REQUIRE_NOTHROW( map.at( { "ccc", 3 } ) );
        REQUIRE_EXCEPTION( map.at( { "ddd", 3 } ), std::out_of_range );
      }
      SECTION( "keys may be added dynamically by operator []" )
      {
        REQUIRE_NOTHROW( (map[{ "ddd", 3 }] = 4) );
        REQUIRE_NOTHROW( map.at( { "ddd", 3 } ) );
      }
      SECTION( "map supports iterators" )
      {
        decltype(map)::iterator beg;
        decltype(map)::iterator end;

        REQUIRE_NOTHROW( beg = map.begin() );
        REQUIRE_NOTHROW( end = map.end() );
        REQUIRE( beg != end );

        REQUIRE( *beg == arbitrary_map<int>::value_type { { "aaa", 3 }, 1 } );
        REQUIRE( ++beg != end );
        REQUIRE( ++beg != end );
        REQUIRE( ++beg != end );
        REQUIRE( ++beg == end );
      }
    }
  } );