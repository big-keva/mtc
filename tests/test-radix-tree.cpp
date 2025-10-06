# include "../test-it-easy.hpp"
# include "../arena.hpp"
# include "../radix-tree.hpp"
# include <cstdio>
# include <vector>

#include "../uuid.h"

using namespace mtc;

int  FetchInt( const char* s )
{
  int value;

  return ::FetchFrom( s, value ) != nullptr ? value : -1;
}

TestItEasy::RegisterFunc  testRadixTree( []()
{
  TEST_CASE( "mtc/radix::tree" )
  {
    SECTION( "radix::tree may be created:" )
    {
      Arena arena;

      SECTION( "- as empty object" )
      {
        SECTION( "* with default allocator" )
        {
          REQUIRE( radix::tree<std::string>().empty() );
        }
        SECTION( "* with custom allocator" )
        {
          REQUIRE( radix::tree<std::string, Arena::allocator<int>>( arena.get_allocator<int>() ).empty() );
        }
      }
      SECTION( "- by initializer list" )
      {
        SECTION( "* with default allocator" )
        {
          radix::tree<std::string>  tree{
            { "aaa", "bbb" },
            { "bbb", "ccc" },
            { "ccc", "ddd" } };

          REQUIRE( tree.size() == 3 );
        }
        SECTION( "* with custom allocator" )
        {
          radix::tree<std::string, Arena::allocator<int>>  tree( {
            { "aaa", "bbb" },
            { "bbb", "ccc" },
            { "ccc", "ddd" } }, arena.get_allocator<int>() );

          REQUIRE( tree.size() == 3 );
        }
      }
      SECTION( "- with iterator convertable to value_type" )
      {
        auto  data = std::vector<std::pair<const std::string, int>>{
          { "the", 1 },
          { "key", 2 },
          { "sequence", 3 } };
        auto  tree = radix::tree<int>{ data.begin(), data.end() };

        REQUIRE( tree.size() == 3 );
        if ( REQUIRE( tree.Search( "the" ) != nullptr ) )
          REQUIRE( *tree.Search( "the" ) == 1 );
        if ( REQUIRE( tree.Search( "key" ) != nullptr ) )
          REQUIRE( *tree.Search( "key" ) == 2 );
        if ( REQUIRE( tree.Search( "sequence" ) != nullptr ) )
          REQUIRE( *tree.Search( "sequence" ) == 3 );

        REQUIRE( tree.Search( "sequencer" ) == nullptr );
        REQUIRE( tree.Search( "" ) == nullptr );
      }
      SECTION( "- with copy constructor" )
      {
        auto  src = radix::tree<int>{
          { "the", 1 },
          { "key", 2 },
          { "sequence", 3 } };
        auto  res = radix::tree<int>( src );

        REQUIRE( src.size() == 3 );
        REQUIRE( res.size() == 3 );
      }
      SECTION( "- with copy constructor and another allocator" )
      {
        auto  src = radix::tree<int>{
          { "the", 1 },
          { "key", 2 },
          { "sequence", 3 } };
        auto  res = radix::tree<int, Arena::allocator<int>>( src, arena.get_allocator<int>() );

        REQUIRE( src.size() == 3 );
        REQUIRE( res.size() == 3 );
      }
      SECTION( "- with movement constructor" )
      {
        auto  src = radix::tree<int>{
          { "the", 1 },
          { "key", 2 },
          { "sequence", 3 } };

        REQUIRE(  src.size() == 3 );

        auto  res = radix::tree<int>( std::move( src ) );

        REQUIRE( src.size() == 0 );
        REQUIRE( res.size() == 3 );
      }
      SECTION( "- as an Arena object" )
      {
        SECTION( "* without arguments" )
        {
          auto  obj = arena.Create<radix::tree<int, Arena::allocator<int>>>();

          REQUIRE( obj != nullptr );
        }
        SECTION( "* with initializer list" )
        {
          auto  obj = arena.Create<radix::tree<int, Arena::allocator<int>>>( std::initializer_list<radix::tree<int>::value_type>{
            { "aaa", 1 },
            { "bbb", 2 },
            { "ccc", 3 } } );

          REQUIRE( obj != nullptr );
          REQUIRE( obj->size() == 3 );
          if ( REQUIRE( obj->Search( "aaa" ) != nullptr ) )
            REQUIRE( *obj->Search( "aaa" ) == 1 );
        }
        SECTION( "- with iterator convertable to value_type" )
        {
          auto  data = std::vector<std::pair<const std::string, int>>{
            { "the", 1 },
            { "key", 2 },
            { "sequence", 3 } };
          auto  obj = arena.Create<radix::tree<int, Arena::allocator<int>>>( data.begin(), data.end() );

          REQUIRE( obj->size() == 3 );
          if ( REQUIRE( obj->Search( "the" ) != nullptr ) )
            REQUIRE( *obj->Search( "the" ) == 1 );
          if ( REQUIRE( obj->Search( "key" ) != nullptr ) )
            REQUIRE( *obj->Search( "key" ) == 2 );
          if ( REQUIRE( obj->Search( "sequence" ) != nullptr ) )
            REQUIRE( *obj->Search( "sequence" ) == 3 );

          REQUIRE( obj->Search( "sequencer" ) == nullptr );
          REQUIRE( obj->Search( "" ) == nullptr );
        }
      }
    }
    SECTION( "radix::tree may be assigned another value" )
    {
      auto  src = radix::tree<int>{
        { "the", 1 },
        { "key", 2 },
        { "sequence", 3 } };
      auto  res = radix::tree<int>();

      SECTION( "- with copy operator" )
      {
        res = src;

        REQUIRE( src.size() == 3 );
        REQUIRE( res.size() == 3 );
      }
      SECTION( "- with move operator" )
      {
        res = std::move( src );
        REQUIRE( src.size() == 0 );
        REQUIRE( res.size() == 3 );
      }
      SECTION( "- with initializer list" )
      {
        res = {
          { "qqq", 96 },
          { "www", 97 },
          { "eee", 98 },
          { "rrr", 99 } };
        REQUIRE( res.size() == 4 );
      }
    }
    SECTION( "radix::tree can operate key-value pairs" )
    {
      auto  tree = radix::tree<std::string>();

      SECTION( "- it may be filled dynamically" )
      {
        REQUIRE_NOTHROW( tree.Insert( { "aaa", 3 }, "bbb" ) );
          REQUIRE( tree.size() == 1 );
        REQUIRE_NOTHROW( tree.Insert( "bbb", "ccc" ) );
          REQUIRE( tree.size() == 2 );
        REQUIRE_NOTHROW( tree.Insert( "ccc", "ddd" ) );
          REQUIRE( tree.size() == 3 );
        REQUIRE_NOTHROW( tree.Insert( "ccc", "eee" ) );
          REQUIRE( tree.size() == 3 );
        REQUIRE_NOTHROW( tree.Insert( "ddd", "eee" ) );
          REQUIRE( tree.size() == 4 );
        REQUIRE_NOTHROW( tree.Insert( "bbbb", "ffff" ) );
          REQUIRE( tree.size() == 5 );
      }
      SECTION( "- it may be searched for keys" )
      {
        REQUIRE( tree.Search( { "aaa", 2 } ) == nullptr );
        if ( REQUIRE( tree.Search( "aaa" ) != nullptr ) )
          REQUIRE( *tree.Search( "aaa" ) == "bbb" );

        if ( REQUIRE( tree.Search( "bbb" ) != nullptr ) )
          REQUIRE( *tree.Search( "bbb" ) == "ccc" );
      }
      SECTION( "- key may be deleted" )
      {
        REQUIRE_NOTHROW( tree.Delete( "qqq" ) );
          REQUIRE( tree.size() == 5 );
        REQUIRE_NOTHROW( tree.Delete( "ddd" ) );
          REQUIRE( tree.size() == 4 );
        SECTION( "* after deletion the key is lost" )
        {
          REQUIRE( tree.find( "ddd" ) == tree.end() );
          REQUIRE( tree.Search( "ddd" ) == nullptr );
        }
        SECTION( "* keys with lost branches are merged" )
        {
          REQUIRE_NOTHROW( tree.Delete( "bbb" ) );
            REQUIRE( tree.size() == 3 );
        }
      }
      SECTION( "- at() either returns reference or throws std::out_of_range" )
      {
        if ( REQUIRE_NOTHROW( tree.at( "aaa" ) ) )
          REQUIRE( tree.at( "aaa" ) == "bbb" );
        REQUIRE_EXCEPTION( tree.at( "none" ), std::out_of_range );
      }
      SECTION( "- [] refers existing objects or creates a new uninitialized one" )
      {
        REQUIRE( tree["aaa"] == "bbb" );
        REQUIRE_NOTHROW( tree["fff"] = "ggg" );
        if ( REQUIRE( tree.Search( "fff" ) != nullptr ) )
          REQUIRE( *tree.Search( "fff" ) == "ggg" );
      }
      SECTION( "- clear() resets all the data" )
      {
        if ( REQUIRE_NOTHROW( tree.clear() ) )
        {
          REQUIRE( tree.size() == 0 );
          REQUIRE( tree.empty() );
        }
      }
    }
    SECTION( "radix::tree has iterator and const_iterator" )
    {
      auto  tree = radix::tree<int>{
        { "the", 1 },
        { "key", 2 },
        { "sequence", 3 } };

      SECTION( "- both iterator and const_iterator may be created" )
      {
        REQUIRE_NOTHROW( ((const radix::tree<std::string>&)tree).begin() );
        REQUIRE_NOTHROW( ((const radix::tree<std::string>&)tree).end() );
        REQUIRE_NOTHROW( tree.cbegin() );
        REQUIRE_NOTHROW( tree.cend() );
      }
      SECTION( "- iterator and const_iterator are comparable" )
      {
//        REQUIRE( tree.cbegin() == tree.begin() );
//        REQUIRE( tree.cend() == tree.end() );
      }
      SECTION( "- iterator may be iterated along the map" )
      {
        auto  beg = tree.cbegin();
        auto  end = tree.cend();

        if ( REQUIRE( beg != end ) )
        {
          SECTION( "* keys follow in lexicographical order" )
          {
            if ( REQUIRE( beg->first == "key" ) )
              REQUIRE( beg->second == 2 );

            if ( REQUIRE( ++beg != end ) )
            {
              if ( REQUIRE( beg->key == "sequence" ) )
                REQUIRE( beg->second == 3 );

              if ( REQUIRE( ++beg != end ) )
              {
                if ( REQUIRE( beg->key == "the" ) )
                  REQUIRE( beg->value == 1 );

                REQUIRE( ++beg == end );
              }
            }
          }
        }
      }
      SECTION( "- iterator can modify values" )
      {
        auto  it = tree.begin();

        if ( REQUIRE( it != tree.end() ) && REQUIRE_NOTHROW( it->value = 4 ) )
          REQUIRE( it->value == 4 );
      }
      SECTION( "- iterator fields have aliases" )
      {
        auto  it = tree.begin();

        if ( REQUIRE( it != tree.end() ) && REQUIRE_NOTHROW( it->second = 1 ) )
          REQUIRE( it->value == 1 );
      }
    }
    SECTION( "search result may be std::compatible iterator" )
    {
      radix::tree<std::string>  tree{
        { "aaa", "bbb" },
        { "bbb", "ccc" },
        { "ccc", "ddd" } };

      SECTION( "- it may be const_iterator..." )
      {
        auto& test = (const radix::tree<std::string>&)tree;
        auto  pos = test.end();

        REQUIRE_NOTHROW( pos = test.find( "aaa" ) );
          REQUIRE( pos != test.end() );
          REQUIRE( pos->first == "aaa" );
          REQUIRE( pos->second == "bbb" );

        REQUIRE_NOTHROW( pos = test.find( "bbb" ) );
          REQUIRE( pos != test.end() );
          REQUIRE( pos->first == "bbb" );
          REQUIRE( pos->second == "ccc" );

        REQUIRE_NOTHROW( pos = test.find( "ddd" ) );
          REQUIRE( pos == test.end() );
      }
      SECTION( "- and iterator allowing modification of data" )
      {
        auto  pos = tree.end();

        REQUIRE_NOTHROW( pos = tree.find( "aaa" ) );
          REQUIRE( pos != tree.end() );
          REQUIRE( pos->first == "aaa" );
          REQUIRE( pos->second == "bbb" );

        REQUIRE_NOTHROW( pos = tree.find( "bbb" ) );
          REQUIRE( pos != tree.end() );
          REQUIRE( pos->first == "bbb" );
          REQUIRE( pos->second == "ccc" );

        REQUIRE_NOTHROW( pos->second = "eee" );
          REQUIRE( pos->second == "eee" );

        REQUIRE_NOTHROW( pos = tree.find( "ddd" ) );
          REQUIRE( pos == tree.end() );
      }
    }
    SECTION( "lower_bound and upper_bound are also supported" )
    {
      radix::tree<std::string>  tree{
        { "aaa",  "bbb" },
        { "bbb",  "ccc" },
        { "bbbd", "cccd" },
        { "bbbde", "cccde" },
        { "ccc",  "ddd" },
        { "ddd",  "eee" },
        { "eee",  "fff" } };

      SECTION( "- lower_bound searches first not less element" )
      {
        if ( REQUIRE_NOTHROW( tree.lower_bound( "b" ) )
          && REQUIRE( tree.lower_bound( "b" ) != tree.end() ) )
             REQUIRE( tree.lower_bound( "b" )->key.to_string() == "bbb" );
        if ( REQUIRE_NOTHROW( tree.lower_bound( "bbb" ) )
          && REQUIRE( tree.lower_bound( "bbb" ) != tree.end() ) )
             REQUIRE( tree.lower_bound( "bbb" )->key.to_string() == "bbb" );
        if ( REQUIRE_NOTHROW( tree.lower_bound( "bbbc" ) )
          && REQUIRE( tree.lower_bound( "bbbc" ) != tree.end() ) )
             REQUIRE( tree.lower_bound( "bbbc" )->key.to_string() == "bbbd" );
        if ( REQUIRE_NOTHROW( tree.lower_bound( "bbbda" ) )
          && REQUIRE( tree.lower_bound( "bbbda" ) != tree.end() ) )
             REQUIRE( tree.lower_bound( "bbbda" )->key.to_string() == "bbbde" );
        if ( REQUIRE_NOTHROW( tree.lower_bound( "bbbdf" ) )
          && REQUIRE( tree.lower_bound( "bbbdf" ) != tree.end() ) )
             REQUIRE( tree.lower_bound( "bbbdf" )->key.to_string() == "ccc" );
        if ( REQUIRE_NOTHROW( tree.lower_bound( "eeea" ) ) )
          REQUIRE( tree.lower_bound( "eeea" ) == tree.end() );
      }
    }
    SECTION( "std::compatible insert(...) also present" )
    {
      auto  tree = radix::tree<int>();
      auto  ipos = radix::tree<int>::iterator<>();
      bool  done;

      SECTION( "- value_type&&" )
      {
        SECTION( "* if key not exists, it is added" )
        {
          if ( REQUIRE_NOTHROW( std::tie( ipos, done ) = tree.insert( { "aaa", 1 } ) ) )
          {
            REQUIRE( done == true );

            if ( REQUIRE( tree.size() == 1 ) )
              REQUIRE( ipos->second == 1 );

            if ( REQUIRE( tree.Search( "aaa" ) != nullptr ) )
              REQUIRE( *tree.Search( "aaa" ) == 1 );
          }
        }
        SECTION( "* if key exists, no modification performed" )
        {
          if ( REQUIRE_NOTHROW( std::tie( ipos, done ) = tree.insert( { "aaa", 2 } ) ) )
          {
              REQUIRE( done == false );

            if ( REQUIRE( tree.size() == 1 ) )
              REQUIRE( ipos->second == 1 );

            if ( REQUIRE_NOTHROW( ipos->second = 2 ) && REQUIRE( tree.Search( "aaa" ) != nullptr ) )
              REQUIRE( *tree.Search( "aaa" ) == 2 );
          }
        }
      }
      SECTION( "- const value_type&" )
      {
        const auto value = radix::tree<int>::value_type{ "bbb", 3 };

        SECTION( "* if key not exists, it is added" )
        {
          if ( REQUIRE_NOTHROW( std::tie( ipos, done ) = tree.insert( value ) ) )
          {
            REQUIRE( done == true );

            if ( REQUIRE( tree.size() == 2 ) )
              REQUIRE( ipos->second == 3 );
          }
        }
      }
      SECTION( "- intializer list" )
      {
        if ( REQUIRE_NOTHROW( tree.insert( {
          { "ccc", 5 },
          { "ddd", 9 } } ) ) )
        {
          if ( REQUIRE( tree.Search( "ddd" ) != nullptr ) )
            REQUIRE( *tree.Search( "ddd" ) == 9 );
        }
      }
      SECTION( "- values iterator" )
      {
        auto  data = std::vector<radix::tree<int>::value_type>{
          { "eee", 11 },
          { " f ", 12 } };

        if ( REQUIRE_NOTHROW( tree.insert( data.begin(), data.end() ) ) )
        {
          if ( REQUIRE( tree.Search( "eee" ) != nullptr ) )
            REQUIRE( *tree.Search( "eee" ) == 11 );
        }
      }
    }
    SECTION( "std::compatible remove(...) is implemented" )
    {
      radix::tree<int>  tree{
        { "aaa",  1 },
        { "bbb",  2 },
        { "bbbd", 3 },
        { "bbbde", 4 },
        { "ccc",  5 },
        { "ddd",  6 },
        { "eee",  7 } };

      SECTION( "- erase() returns the cound of removed elements" )
      {
        REQUIRE( tree.erase( "000" ) == 0 );
        REQUIRE( tree.erase( "aaa" ) == 1 );
      }
      SECTION( "- erase( pos ) of last key returns end()" )
      {
        auto  pos = tree.lower_bound( "eee" );

        if ( REQUIRE( pos != tree.end() ) && REQUIRE_NOTHROW( pos = tree.erase( pos ) ) )
          REQUIRE( pos == tree.end() );
      }
      SECTION( "- erase( pos ) of existing key returns next one" )
      {
        auto  pos = tree.begin();
        auto  del = std::initializer_list<const char*>{
          "bbb",
          "bbbd",
          "bbbde",
          "ccc",
          "ddd" };

        for ( auto it = del.begin(); it != del.end(); ++it )
        {
          auto  next = it + 1;

          if ( !REQUIRE_NOTHROW( pos != tree.end() )
            || !REQUIRE( pos->key == *it ) )  break;

          if ( !REQUIRE_NOTHROW( pos = tree.erase( pos ) ) )
            break;

          if ( next != del.end() )
          {
            if ( !REQUIRE( pos != tree.end() )
              || !REQUIRE( pos->key == *next ) ) break;
          }
            else
          REQUIRE( pos == tree.end() );
        }

        REQUIRE( tree.empty() );
      }
      SECTION( "- erase( beg, end ) allows sequental delete" )
      {
        tree = {
          { "aaa", 1 },
          { "bbb", 2 },
          { "ccc", 3 },
          { "ddd", 4 } };

        auto  beg = tree.lower_bound( "bbb" );
        auto  end = tree.lower_bound( "ddd" );
        auto  pos = tree.end();

        if ( REQUIRE_NOTHROW( pos = tree.erase( beg, end ) ) )
          REQUIRE( pos->key == "ddd" );
        REQUIRE( tree.size() == 2 );
      }
    }
  }
  TEST_CASE( "mtc/radix::dump" )
  {
    std::vector<char>             buff;
    mtc::radix::dump<const char>  dump;

    SECTION( "radix::tree may be serialized as radix::dump" )
    {
      auto  tree = mtc::radix::tree<int>{
        { "andrey",   55 },
        { "anna",     48 },
        { "anton",    41 },
        { "igor",     63 },
        { "maria",    18 },
        { "natasha",  59 },
        { "veselina", 15 } };

      REQUIRE_NOTHROW( buff.resize( ::GetBufLen( tree ) ) );
      REQUIRE_NOTHROW( tree.Serialize( buff.data() ) == buff.size() + buff.data() );
      REQUIRE_NOTHROW( dump = buff.data() );
    }
    SECTION( "dump is searchable" )
    {
      REQUIRE( mtc::radix::dump<const char>( buff.data() ).Search( "andrey" ) != nullptr );
      REQUIRE( mtc::radix::dump<const char>( buff.data() ).Search( "igor" ) != nullptr );
      REQUIRE( mtc::radix::dump<const char>( buff.data() ).Search( "michael" ) == nullptr );
    }
    SECTION( "radix::dump has const_iterator" )
    {
      SECTION( "- const_iterator may be created" )
      {
        REQUIRE_NOTHROW( dump.begin() );
        REQUIRE_NOTHROW( dump.end() );
        REQUIRE_NOTHROW( dump.cbegin() );
        REQUIRE_NOTHROW( dump.cend() );
      }
      SECTION( "- const_iterator may be iterated along the map" )
      {
        auto  beg = dump.cbegin();
        auto  end = dump.cend();

        if ( REQUIRE( beg != end ) )
        {
          SECTION( "* keys follow in lexicographical order" )
          {
            if ( REQUIRE( beg->first == "andrey" ) )
              REQUIRE( FetchInt( beg->second ) == 55 );

            if ( REQUIRE_NOTHROW( ++beg ) && REQUIRE( beg != end ) )
              if ( REQUIRE( beg->key == "anna" ) )
                REQUIRE( FetchInt( beg->second ) == 48 );

            if ( REQUIRE_NOTHROW( ++beg ) && REQUIRE( beg != end ) )
              if ( REQUIRE( beg->key == "anton" ) )
                REQUIRE( FetchInt( beg->second ) == 41 );

            if ( REQUIRE_NOTHROW( ++beg ) && REQUIRE( beg != end ) )
              if ( REQUIRE( beg->key == "igor" ) )
                REQUIRE( FetchInt( beg->second ) == 63 );

            if ( REQUIRE_NOTHROW( ++beg ) && REQUIRE( beg != end ) )
              if ( REQUIRE( beg->key == "maria" ) )
                REQUIRE( FetchInt( beg->second ) == 18 );

            if ( REQUIRE_NOTHROW( ++beg ) && REQUIRE( beg != end ) )
              if ( REQUIRE( beg->key == "natasha" ) )
                REQUIRE( FetchInt( beg->second ) == 59 );

            if ( REQUIRE_NOTHROW( ++beg ) && REQUIRE( beg != end ) )
              if ( REQUIRE( beg->key == "veselina" ) )
                REQUIRE( FetchInt( beg->second ) == 15 );

            if ( REQUIRE_NOTHROW( ++beg ) )
              REQUIRE( beg == end );
          }
        }
      }
      SECTION( "- iterator fields have aliases" )
      {
        auto  it = dump.begin();

        if ( REQUIRE( it != dump.end() ) && REQUIRE_NOTHROW( FetchInt( it->second ) == 55 ) )
          REQUIRE( FetchInt( it->value ) == 55 );
      }
    }
    SECTION( "search result may be std::compatible iterator" )
    {
      auto  pos = dump.end();

      if ( REQUIRE_NOTHROW( pos = dump.find( "natasha" ) ) && REQUIRE( pos != dump.end() ) )
      {
        if ( REQUIRE( pos->first == "natasha" ) && REQUIRE( FetchInt( pos->second ) == 59 ) )
        {
          SECTION( "iterator returned by find() is iterable" )
          {
            if ( REQUIRE_NOTHROW( ++pos ) && REQUIRE( pos != dump.end() ) )
            {
              REQUIRE( pos->key == "veselina" );
              REQUIRE( FetchInt( pos->second ) == 15 );
            }
          }
        }
      }

      if ( REQUIRE_NOTHROW( pos = dump.find( "zorro" ) ) )
        REQUIRE( pos == dump.end() );
    }
    SECTION( "lower_bound and upper_bound are also supported" )
    {
      SECTION( "- lower_bound searches first not less element" )
      {
        if ( REQUIRE_NOTHROW( dump.lower_bound( "b" ) )
          && REQUIRE( dump.lower_bound( "b" ) != dump.end() ) )
             REQUIRE( dump.lower_bound( "b" )->key == "igor" );
        if ( REQUIRE_NOTHROW( dump.lower_bound( "v" ) )
          && REQUIRE( dump.lower_bound( "v" ) != dump.end() ) )
             REQUIRE( dump.lower_bound( "v" )->key == "veselina" );
        if ( REQUIRE_NOTHROW( dump.lower_bound( "zorro" ) ) )
          REQUIRE( dump.lower_bound( "zorro" ) == dump.end() );
      }
    }
  }

} );
