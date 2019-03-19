# define CATCH_CONFIG_MAIN
# include <catch2/catch.hpp>
# include "../patricia.h"
# include <stdio.h>

TEST_CASE( "patricia keys are deleteable" )
{
  SECTION( "one-range keys may be deleted" )
  {
    mtc::patricia::tree<int>  patree( {
      { "aaa", 111 },
      { "bbb", 222 },
      { "ccc", 333 }
    } );

    patree.Delete( std::string( "bbb" ) );
    patree.Delete( std::string( "ccc" ) );
    patree.Delete( std::string( "aaa" ) );
  }

  SECTION( "partial keys may be deleted upper-to-lower" )
  {
    mtc::patricia::tree<int>  patree;

    patree.Insert( std::string( "a" ), 1 );
    patree.Insert( std::string( "aa" ), 11 );

    patree.Delete( std::string( "aa" ) );
    patree.Delete( std::string( "a" ) );
  }

  SECTION( "partial keys may be deleted with merging" )
  {
    mtc::patricia::tree<int>  patree;

    patree.insert( { { "a", 1 },
                     { "aa", 11 },
                     { "aaa", 111 } } );

    patree.Delete( std::string( "aa" ) );
    REQUIRE( patree.Search( std::string( "a" ) ) != nullptr );
    REQUIRE( patree.Search( std::string( "aa" ) ) == nullptr );
    REQUIRE( patree.Search( std::string( "aaa" ) ) != nullptr );

    patree.Delete( std::string( "a" ) );
    REQUIRE( patree.Search( std::string( "aaa" ) ) != nullptr );

    patree.Delete( std::string( "aaa" ) );
  }

}

TEST_CASE( "patricia::tree is dynamic" )
{
  mtc::patricia::tree<int>  patree;

  SECTION( "adding new keys return access to their values" )
  {
    REQUIRE(  patree.Insert( "aaa", 12345 ) != nullptr );
    REQUIRE(  patree.Search( "aaa" ) != nullptr );
    REQUIRE( *patree.Search( "aaa" ) == 12345 );
  }

  SECTION( "adding new keys without values add default values" )
  {
    REQUIRE(  patree.Insert( mtc::patricia::key( "bbb", 3 ) ) != nullptr );
    REQUIRE(  patree.Search( mtc::patricia::key( "bbb", 3 ) ) != nullptr );
    REQUIRE( *patree.Search( mtc::patricia::key( "bbb", 3 ) ) == 0 );
  }

  SECTION( "adding existing keys return overriden values with same pointers" )
  {
    auto  oldptr = patree.Insert( mtc::patricia::key( "aaa", 3 ), 12345 );

    REQUIRE( patree.Search( mtc::patricia::key( "aaa", 3 ) ) == oldptr );
    REQUIRE( patree.Insert( mtc::patricia::key( "aaa", 3 ), 0 ) == oldptr );
  }

  SECTION( "deleted keys are deleted physically" )
  {
    patree.Insert( "ccc", 7 );
    patree.Insert( "eee", 9 );

    REQUIRE( patree.Search( "ccc" ) != nullptr );
    REQUIRE( patree.Search( "eee" ) != nullptr );

    patree.Delete( "ccc" );

    REQUIRE( patree.Search( mtc::patricia::key( "ccc", 3 ) ) == nullptr );
    REQUIRE( patree.Search( mtc::patricia::key( "eee", 3 ) ) != nullptr );

    patree.Delete( mtc::patricia::key( "eee", 3 ) );

    REQUIRE( patree.Search( mtc::patricia::key( "eee", 3 ) ) == nullptr );
  }

  SECTION( "constant access is also available" )
  {
    const auto& tree = patree;

    patree.Insert( "aaa", 7 );
    patree.Insert( "bbb", 9 );

    REQUIRE( tree.Search( "aaa" ) != nullptr );
    REQUIRE( tree.Search( "eee" ) == nullptr );
  }
}

TEST_CASE( "patricia::tree is iterable" )
{
  mtc::patricia::tree<int>  patree;

  SECTION( "empty patricia tree has no elements" )
  {
    REQUIRE( patree.begin() == patree.end() );
  }

  SECTION( "one-element patricia tree is iterable" )
  {
    patree.Insert( "aaa", 1 );

    auto  it = patree.begin();

    REQUIRE( it != patree.end() );
    REQUIRE( it.key() == std::string( "aaa" ) );
    REQUIRE( it.val() == 1 );
    REQUIRE( ++it == patree.end() );
  }

  SECTION( "multi-elements patricia tree is iterable" )
  {
    patree.Insert( std::string( "a" ), 1 );
    patree.Insert( std::string( "aa" ), 11 );
    patree.Insert( std::string( "aaa" ), 111 );
    patree.Insert( std::string( "bbb" ), 222 );
    patree.Insert( std::string( "ccc" ), 333 );

    auto  it = patree.begin();

    REQUIRE( it != patree.end() );
    REQUIRE( it.key() == std::string( "a" ) );
    REQUIRE( it.val() == 1 );
      ++it;
    REQUIRE( it.key() == std::string( "aa" ) );
    REQUIRE( it.val() == 11 );
      ++it;
    REQUIRE( it.key() == std::string( "aaa" ) );
    REQUIRE( it.val() == 111 );
      ++it;
    REQUIRE( it.key() == std::string( "bbb" ) );
    REQUIRE( it.val() == 222 );
      ++it;
    REQUIRE( it.key() == std::string( "ccc" ) );
    REQUIRE( it.val() == 333 );
    REQUIRE( ++it == patree.end() );
  }

  SECTION( "deleted elements do not affect the iterator" )
  {
    patree.Insert( std::string( "a" ), 1 );
    patree.Insert( std::string( "aa" ), 11 );
    patree.Insert( std::string( "aaa" ), 111 );
    patree.Insert( std::string( "bbb" ), 222 );

    patree.Delete( std::string( "aa" ) );
    patree.Delete( std::string( "bbb" ) );

    auto  it = patree.begin();

    REQUIRE( it != patree.end() );
    REQUIRE( it.key() == std::string( "a" ) );
    REQUIRE( it.val() == 1 );
      ++it;
    REQUIRE( it.key() == std::string( "aaa" ) );
    REQUIRE( it.val() == 111 );

    REQUIRE( ++it == patree.end() );
  }

  SECTION( "patricia::tree iterator is searchable" )
  {
    patree.Insert( std::string( "a" ), 1 );
    patree.Insert( std::string( "aa" ), 11 );
    patree.Insert( std::string( "aaa" ), 111 );
    patree.Insert( std::string( "bbb" ), 222 );

    const mtc::patricia::tree<int>& rt = patree;
    auto                            it = rt.end();

    it = rt.find( "bbb" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "bbb" ) );

      it = rt.lower_bound( "" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "a" ) );

    it = rt.lower_bound( "0000" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "a" ) );

    it = rt.lower_bound( "a" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "a" ) );

    it = rt.lower_bound( "a0" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "aa" ) );

    it = rt.lower_bound( "aa" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "aa" ) );

    it = rt.lower_bound( "aa0" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "aaa" ) );

    it = rt.lower_bound( "aab" );
      REQUIRE( it != rt.end() );
      REQUIRE( it.key() == std::string( "bbb" ) );

    it = rt.lower_bound( "bbbb" );
      REQUIRE( it == rt.end() );
  }
}

TEST_CASE( "patricia::tree is serializable" )
{
  mtc::patricia::tree<int>  patree;
  std::vector<char>         serial;

  patree.Insert( std::string( "a" ), 1 );
  patree.Insert( std::string( "aa" ), 11 );
  patree.Insert( std::string( "aaa" ), 111 );
  patree.Insert( std::string( "bbb" ), 222 );

  serial.resize( patree.GetBufLen() );

  REQUIRE( patree.Serialize( serial.data() ) - serial.data() == serial.size() );

  SECTION( "patricia::dump provives untyped access to data" )
  {
    mtc::patricia::dump   dumped( serial.data() );

    REQUIRE( dumped.Search( mtc::patricia::key( "z" ) ) == nullptr );
    REQUIRE( dumped.Search( mtc::patricia::key( "a" ) ) != nullptr );
    REQUIRE( dumped.Search( mtc::patricia::key( "aa" ) ) != nullptr );
    REQUIRE( dumped.Search( mtc::patricia::key( "aaa" ) ) != nullptr );
    REQUIRE( dumped.Search( mtc::patricia::key( "bbb" ) ) != nullptr );
  }

  SECTION( "patricia::dump provives untyped iterator" )
  {
    mtc::patricia::dump   dumped( serial.data() );

    auto  it = dumped.begin();

    REQUIRE( it != dumped.end() );
    REQUIRE( it.key() == std::string( "a" ) );
    REQUIRE( ++it != dumped.end() );
    REQUIRE( it.key() == std::string( "aa" ) );
    REQUIRE( ++it != dumped.end() );
    REQUIRE( it.key() == std::string( "aaa" ) );
    REQUIRE( ++it != dumped.end() );
    REQUIRE( it.key() == std::string( "bbb" ) );
    REQUIRE( ++it == dumped.end() );
  }

}
