/*
  Все кириллические тексты в этом файле представлены кодировкой utf-8. В том числе и этот заголовок,
  обеспечивающий достаточное количество текста, чтобы никто не ошибся с кодировкой.
*/
# define CATCH_CONFIG_MAIN
# include <catch2/catch.hpp>
# include <mtc/wcsstr.h>

class fake_allocator
{
  size_t  allocated_bytes = 0;

public:     // allocator functionality
  void*   alloc( size_t n )
    {
      allocated_bytes += n;
      return mtc::def_alloc::alloc( n );
    }

public:     // call wrapper
  template <class function_allocator>
  size_t  call_function( function_allocator fn )
    {
      auto p = fn( *this );

      mtc::def_alloc::free( p );

      return allocated_bytes;
    }
};

class hexstr_to_widechar
{
  public: widechar  operator ()( const char*& s ) const
    {
      widechar  wc;
      char*     ep;

      if ( *s != '\\' )
        return *s++;
      if ( *++s == 'x' )  ++s;
        else return 0;
      wc = (widechar)strtoul( s, &ep, 16 );
        s = (const char*)ep;
      return wc;
    }
  public: widechar  operator ()( const widechar*& s ) const
    {
      return *s++;
    }
};

widechar  w_zero = 0;
widechar  w_word[] = { (widechar)'w', (widechar)'o', (widechar)'r', (widechar)'d', (widechar)0 };

TEST_CASE( "mtc::wcsstr.h tests", "[QueryParser]" )
{
  SECTION( "mtc::impl internal tests" )
  {
    REQUIRE( mtc::impl::w_in_lim( 'a', 'a', 'a' ) );
    REQUIRE_FALSE( mtc::impl::w_in_lim( 'a', 'b', 'b' ) );

    REQUIRE( mtc::impl::w_in_lim( (widechar)'a', (widechar)'a', (widechar)'z' ) );
    REQUIRE( mtc::impl::w_in_lim( 0x407, 0x400, 0x420 ) );

    REQUIRE( mtc::impl::w_is_chr( 'a', (widechar)'a' ) );
    REQUIRE( mtc::impl::w_is_chr( 'a', (widechar)'a' ) );
    REQUIRE( mtc::impl::w_is_chr( (widechar)'a', (widechar)'a' ) );

    REQUIRE( mtc::impl::w_is_chr( 'a', 'c', 'b', 'a' ) );
    REQUIRE_FALSE( mtc::impl::w_is_chr( 'a', 'c', 'b' ) );

    REQUIRE( mtc::impl::w_is_num( '0' ) );
    REQUIRE( mtc::impl::w_is_num( '9' ) );
    REQUIRE_FALSE( mtc::impl::w_is_num( 'a' ) );

  }

  SECTION( "w_strlen family" )
  {
    REQUIRE( mtc::w_strlen( "" ) == 0 );
    REQUIRE( mtc::w_strlen( &w_zero ) == 0 );

    REQUIRE( mtc::w_strlen( "a" ) == 1 );
    REQUIRE( mtc::w_strlen( "word" ) == 4 );

    REQUIRE( mtc::w_strlen( w_word ) == 4 );

    REQUIRE( mtc::w_strlen( "слово" ) == 10 );    // utf-8 encoded
  }

  SECTION( "w_strdup family" )
  {
    REQUIRE( mtc::w_strdup( (const char*)nullptr ) == nullptr );
    REQUIRE( mtc::w_strdup( (const char*)nullptr, 100 ) == nullptr );

    REQUIRE( fake_allocator().call_function( []( fake_allocator& fa )
      {  return mtc::w_strdup( fa, "aaa" );  } ) == 4 );
    REQUIRE( fake_allocator().call_function( []( fake_allocator& fa )
      {  return mtc::w_strdup( fa, "aaa", 2 );  } ) == 3 );

    REQUIRE( fake_allocator().call_function( [&]( fake_allocator& fa )
      {  return mtc::w_strdup( fa, w_word );  } ) == 10 );
    REQUIRE( fake_allocator().call_function( [&]( fake_allocator& fa )
      {  return mtc::w_strdup( fa, w_word, 2 );  } ) == 6 );
  }

  SECTION( "w_strcpy family" )
  {
    char      c_buff[0x100];
    widechar  w_buff[0x100];

    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( c_buff, "aaa" ), "aaa" ) == 0 );
    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( w_buff, w_word ), "word" ) == 0 );

    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( w_buff, "word" ), w_word ) == 0 );

    REQUIRE( mtc::w_strcmp( mtc::w_strncpy( c_buff, "aaaa", 10 ), "aaaa" ) == 0 );
    REQUIRE(                mtc::w_strncpy( c_buff, "aaa", 3 )[3] == 'a' );

    REQUIRE( mtc::w_strcmp( mtc::w_strncpy( w_buff, w_word, 10 ), w_word ) == 0 );
    REQUIRE(                mtc::w_strncpy( w_buff, w_word, 3 )[3] == (widechar)'d' );

    REQUIRE( mtc::w_strncpy( mtc::w_strcpy( w_buff, "aaaaaaa" ), "bbb", 3 )[4] == (widechar)'a' );

    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( w_buff, "aaaaaa",
      []( const char*& s ){  return *s++ != 0 ? 'b' : 0;  } ), "bbbbbb" ) == 0 );

    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( w_buff, "aaaaaa",
      []( const char*& s ){  return *s++ != 0 ? 'b' : 0;  } ), "bbbbbb" ) == 0 );

    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( w_buff, "\\x77\\x006f\\x072\\x64", hexstr_to_widechar() ), w_word ) == 0 );
  }

  SECTION( "w_strcat family" )
  {
    char      c_buff[0x100];
    widechar  w_buff[0x100];

    REQUIRE( mtc::w_strcmp( mtc::w_strcat( mtc::w_strcpy( c_buff, "red " ), "scar" ),
      "red scar" )  == 0 );
    REQUIRE( mtc::w_strcmp( mtc::w_strcat( mtc::w_strcpy( w_buff, "red " ), "scar" ),
      "red scar" )  == 0 );
    REQUIRE( mtc::w_strcmp( mtc::w_strcat( mtc::w_strcpy( w_buff, "bad " ), "\\x77\\x006f\\x072\\x64", hexstr_to_widechar() ),
      "bad word" ) == 0 );
  }
    
  SECTION( "w_strcmp family" )
  {
    widechar  w_buf1[0x100];
    widechar  w_buf2[0x100];

    REQUIRE( mtc::w_strcmp( "abc",                          "abc" ) == 0 );
    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( w_buf1, "abc" ), "abc" ) == 0 );
    REQUIRE( mtc::w_strcmp( mtc::w_strcpy( w_buf1, "abc" ), mtc::w_strcpy( w_buf2, "abc" ) ) == 0 );
    REQUIRE( mtc::w_strcmp( "abc",                          mtc::w_strcpy( w_buf2, "abc" ) ) == 0 );

    REQUIRE( mtc::w_strcmp( w_word,                         "\\x77\\x06f\\x072\\x64", hexstr_to_widechar() ) == 0 );
    REQUIRE( mtc::w_strcmp( "\\x0077\\x06f\\x72\\x064",     w_word,                   hexstr_to_widechar() ) == 0 );
  }

  SECTION( "w_strncmp family" )
  {
    widechar  w_buf1[0x100];
    widechar  w_buf2[0x100];

    REQUIRE( mtc::w_strncmp( "abcdef",                       "abcfed", 3 ) == 0 );
    REQUIRE( mtc::w_strncmp( mtc::w_strcpy( w_buf1, "abc" ), "abd",    2 ) == 0 );
    REQUIRE( mtc::w_strncmp( mtc::w_strcpy( w_buf1, "abc" ), mtc::w_strcpy( w_buf2, "acb" ), 2 ) != 0 );
    REQUIRE( mtc::w_strncmp( "abc",                          mtc::w_strcpy( w_buf2, "acb" ), 1 ) == 0 );

    REQUIRE( mtc::w_strncmp( w_word,                         "\\x77\\x006f\\x072\\x64", 4, hexstr_to_widechar() ) == 0 );
    REQUIRE( mtc::w_strncmp( "\\x0077\\x06f\\x72\\x064",     w_word,                    5, hexstr_to_widechar() ) == 0 );
  }

  SECTION( "w_strcasecmp family" )
  {
    REQUIRE( mtc::w_strcasecmp( "ABCd", "abcd" ) == 0 );
    REQUIRE( mtc::w_strcasecmp( w_word, "WORD" ) == 0 );
  }

  SECTION( "w_strcnasecmp family" )
  {
    REQUIRE( mtc::w_strncasecmp( "ABCq", "abcd", 3 ) == 0 );
    REQUIRE( mtc::w_strncasecmp( "ABCq", "abcd", 4 ) != 0 );
    REQUIRE( mtc::w_strncasecmp( w_word, "WORD", 2 ) == 0 );
  }

  SECTION( "w_strchr family" )
  {
    char*     s;
    widechar* w;

    REQUIRE( ((s = mtc::w_strchr( "bing", 'i' )) != nullptr && *s == 'i') );
    REQUIRE( ((w = mtc::w_strchr( w_word, 'o' )) != nullptr && *w == 'o') );
  }

  SECTION( "w_strstr family" )
  {
    const char*     s;
    const widechar* w;

    REQUIRE( ((s = mtc::w_strstr( "bing", "in" )) != nullptr && *s == 'i') );
    REQUIRE( ((w = mtc::w_strstr( w_word, w_word + 1 )) != nullptr && *w == w_word[1]) );

    REQUIRE( ((s = mtc::w_strstr( "word", w_word + 1 )) != nullptr && *s == 'o') );
    REQUIRE( ((w = mtc::w_strstr( w_word, "or" )) != nullptr && *w == w_word[1]) );
  }

}
