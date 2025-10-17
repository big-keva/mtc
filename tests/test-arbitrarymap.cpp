# include "../test-it-easy.hpp"
# include "../arbitrarymap.h"
# include <cstdio>

using namespace mtc;

TestItEasy::RegisterFunc  testArbitraryMap( []()
{

  TEST_CASE( "mtc/ArbitraryMap" )
  {
    SECTION( "arbitrarymap may be created:" )
    {
      SECTION( "- as empty object;" )
      {
        arbitrarymap<std::string> map;

        REQUIRE( map.size() == size_t(0) );
        REQUIRE( map.empty() );
      }
      SECTION( "- with initializer list" )
      {
        auto map = arbitrarymap<std::string>( {
          { "aaa", "bbb" },
          { "bbb", "ccc" },
          { "ccc", "ddd" } } );

        REQUIRE( map.size() == size_t(3) );
        REQUIRE( !map.empty() );
      }
    }
    SECTION( "arbitrarymap may be filled dynamically" )
    {
      arbitrarymap<std::string> map;

      REQUIRE_NOTHROW( map.Insert( "aaa", 3, "bbb" ) );
        REQUIRE( map.size() == size_t(1) );
      REQUIRE_NOTHROW( map.Insert( "bbb", "ccc" ) );
        REQUIRE( map.size() == size_t(2) );
      REQUIRE_NOTHROW( map.Insert( "ccc", "ddd" ) );
        REQUIRE( map.size() == size_t(3) );
    }
    SECTION( "arbitrarymap may be searched for keys" )
    {
      arbitrarymap<std::string> map;

      REQUIRE_NOTHROW( map.Insert( "aaa", 3, "bbb" ) );
        REQUIRE( map.Search( "aaa", 3 ) != nullptr );
        REQUIRE( map.Search( "aaa" ) == nullptr );
      REQUIRE_NOTHROW( map.Insert( "bbb", "ccc" ) );
        REQUIRE( map.Search( "bbb" ) != nullptr );
    }
    SECTION( "arbitrarymap has const_iterator" )
    {
      auto  am = arbitrarymap<std::string>( {
        { "aaa", "bbb" },
        { "bbb", "ccc" },
        { "ccc", "ddd" } } );

      REQUIRE( am.size() == size_t(3) );
/*
      SECTION( "iterator may be created" )
      {
        REQUIRE_NOTHROW( ((const arbitrarymap<std::string>&)am).begin() );
        REQUIRE_NOTHROW( ((const arbitrarymap<std::string>&)am).end() );
      }
      SECTION( "iterator may be iterated along the map" )
      {
        auto  beg = am.cbegin();
        auto  end = am.cend();

        REQUIRE( beg != end );
        REQUIRE( beg->first.size() == size_t(4) );
          ++beg;
        REQUIRE( beg != end );
        REQUIRE( beg->first.size() == size_t(4) );
          ++beg;
        REQUIRE( beg != end );
        REQUIRE( beg->first.size() == size_t(4) );
          ++beg;
        REQUIRE( beg == end );
      }
      SECTION( "search result may be std:: compatible iterator" )
      {
        auto  pos = am.end();

        REQUIRE_NOTHROW( pos = am.find( "aaa" ) );
          REQUIRE( pos != am.end() );
          REQUIRE( pos->first == "aaa" );
          REQUIRE( pos->second == "bbb" );

        REQUIRE_NOTHROW( pos = am.find( "bbb" ) );
          REQUIRE( pos != am.end() );
          REQUIRE( pos->first == "bbb" );
          REQUIRE( pos->second == "ccc" );

        REQUIRE_NOTHROW( pos->second = "eee" );
          REQUIRE( pos->second == "eee" );

        REQUIRE_NOTHROW( pos = am.find( "ddd" ) );
          REQUIRE( pos == am.end() );
      }
      SECTION( "at() either returns reference or throws std::out_of_range" )
      {
        REQUIRE_NOTHROW( am.at( "aaa" ) );
          REQUIRE( am.at( "aaa" ) == "bbb" );
        REQUIRE_EXCEPTION( am.at( "none" ), std::out_of_range );
      }
      SECTION( "[] refers existing objects or creates a new uninitialized one" )
      {
        REQUIRE( am["aaa"] == "bbb" );
        REQUIRE_NOTHROW( am["fff"] = "ggg" );
      }
      SECTION( "after Delete( ... ) key is lost" )
      {
        REQUIRE_NOTHROW( am.Delete( "aaa" ) );
          REQUIRE( am.size() == size_t(3) );
          REQUIRE( am.find( "aaa" ) == am.end() );
          REQUIRE( am.Search( "aaa" ) == nullptr );
      }
*/
    }
  }

} );
