/*
  Все кириллические тексты в этом файле представлены кодировкой utf-8. В том числе и этот заголовок,
  обеспечивающий достаточное количество текста, чтобы никто не ошибся с кодировкой.
*/
# include "../test-it-easy.hpp"
# include "../wcsstr.h"

using namespace mtc;

template <class C>
class counting_allocator: public std::allocator<C>
{
  size_t  allocated_bytes = 0;

public:     // allocator functionality
  C*   allocate( size_t n )
    {
      allocated_bytes += n;
      return std::allocator<C>::allocate( n );
    }

public:     // call wrapper
  template <class function_allocator>
  size_t  call_function( function_allocator fn )
    {
      auto p = fn( *this );

      std::allocator<C>::deallocate( p, allocated_bytes );

      return allocated_bytes * sizeof(C);
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

template <class T>
class printing_allocator
{
public:
  typedef T         value_type;
  typedef T*        pointer;
  typedef const T*  const_pointer;
  typedef size_t    size_type;
  typedef int       difference_type;

public:
  printing_allocator() {}
  printing_allocator( const printing_allocator& p ) {}
  printing_allocator( printing_allocator&& p ) {}

  T* allocate( size_t n, const void * hint = 0 )
  {
    fprintf( stdout, "allocate %u bytes\n", n * sizeof(T) );
    return (T*)malloc( n * sizeof(T) );
  }
  void  deallocate( T* p, size_t n )
  {
    free( p );
  }
};

widechar  w_zero = 0;
widechar  w_word[] = { (widechar)'w', (widechar)'o', (widechar)'r', (widechar)'d', (widechar)0 };
widechar  w_WORD[] = { (widechar)'W', (widechar)'O', (widechar)'R', (widechar)'D', (widechar)0 };

TestItEasy::RegisterFunc  testWcsstr( []
  {
    TEST_CASE( "mtc/wcsstr" )
    {
      SECTION( "strlen() family counts bytes" )
      {
        REQUIRE( w_strlen( "" ) == 0 );
        REQUIRE( w_strlen( &w_zero ) == 0 );

        REQUIRE( w_strlen( "a" ) == 1 );
        REQUIRE( w_strlen( "word" ) == 4 );

        REQUIRE( w_strlen( w_word ) == 4 );

        REQUIRE( w_strlen( "слово" ) == 10 );    // utf-8 encoded
      }
      SECTION( "strdup() family" )
      {
        REQUIRE( w_strdup( (const char*)nullptr ) == nullptr );
        REQUIRE( w_strdup( (const char*)nullptr, 100 ) == nullptr );
      }
      SECTION( "strcpy() family" )
      {
        char      c_buff[0x100];
        widechar  w_buff[0x100];

        REQUIRE( w_strcmp( w_strcpy( c_buff, "aaa" ), "aaa" ) == 0 );
        REQUIRE( w_strcmp( w_strcpy( w_buff, w_word ), "word" ) == 0 );

        REQUIRE( w_strcmp( w_strcpy( w_buff, "word" ), w_word ) == 0 );

        REQUIRE( w_strcmp( w_strncpy( c_buff, "aaaa", 10 ), "aaaa" ) == 0 );
        REQUIRE(           w_strncpy( c_buff, "aaa", 3 )[3] == 'a' );

        REQUIRE( w_strcmp( w_strncpy( w_buff, w_word, 10 ), w_word ) == 0 );
        REQUIRE(           w_strncpy( w_buff, w_word, 3 )[3] == (widechar)'d' );

        REQUIRE( w_strncpy( w_strcpy( w_buff, "aaaaaaa" ), "bbb", 3 )[4] == (widechar)'a' );

        REQUIRE( w_strcmp( w_strcpy( w_buff, "aaaaaa",
          []( const char*& s ){  return *s++ != 0 ? 'b' : 0;  } ), "bbbbbb" ) == 0 );

        REQUIRE( w_strcmp( w_strcpy( w_buff, "aaaaaa",
          []( const char*& s ){  return *s++ != 0 ? 'b' : 0;  } ), "bbbbbb" ) == 0 );

        REQUIRE( w_strcmp( w_strcpy( w_buff, "\\x77\\x006f\\x072\\x64", hexstr_to_widechar() ), w_word ) == 0 );
      }
      SECTION( "strcat() family" )
      {
        char      c_buff[0x100];
        widechar  w_buff[0x100];

        REQUIRE( w_strcmp( mtc::w_strcat( w_strcpy( c_buff, "red " ), "scar" ),
          "red scar" )  == 0 );
        REQUIRE( w_strcmp( mtc::w_strcat( w_strcpy( w_buff, "red " ), "scar" ),
          "red scar" )  == 0 );
        REQUIRE( w_strcmp( mtc::w_strcat( w_strcpy( w_buff, "bad " ), "\\x77\\x006f\\x072\\x64", hexstr_to_widechar() ),
          "bad word" ) == 0 );
      }
      SECTION( "strcmp() family" )
      {
        widechar  w_buf1[0x100];
        widechar  w_buf2[0x100];

        REQUIRE( w_strcmp( "abc",                       "abc" ) == 0 );
        REQUIRE( w_strcmp( w_strcpy( w_buf1, "abc" ),   "abc" ) == 0 );
        REQUIRE( w_strcmp( w_strcpy( w_buf1, "abc" ),   w_strcpy( w_buf2, "abc" ) ) == 0 );
        REQUIRE( w_strcmp( "abc",                       w_strcpy( w_buf2, "abc" ) ) == 0 );

        REQUIRE( w_strcmp( w_word,                      "\\x77\\x06f\\x072\\x64", hexstr_to_widechar() ) == 0 );
        REQUIRE( w_strcmp( "\\x0077\\x06f\\x72\\x064",  w_word,                   hexstr_to_widechar() ) == 0 );
      }
      SECTION( "strncmp() family()" )
      {
        widechar  w_buf1[0x100];
        widechar  w_buf2[0x100];

        REQUIRE( w_strncmp( "abcdef",                  "abcfed", 3 ) == 0 );
        REQUIRE( w_strncmp( w_strcpy( w_buf1, "abc" ), "abd",    2 ) == 0 );
        REQUIRE( w_strncmp( w_strcpy( w_buf1, "abc" ),  w_strcpy( w_buf2, "acb" ), 2 ) != 0 );
        REQUIRE( w_strncmp( "abc",                      w_strcpy( w_buf2, "acb" ), 1 ) == 0 );

        REQUIRE( w_strncmp( w_word,                     "\\x77\\x006f\\x072\\x64", 4, hexstr_to_widechar() ) == 0 );
        REQUIRE( w_strncmp( "\\x0077\\x06f\\x72\\x064", w_word,                    5, hexstr_to_widechar() ) == 0 );
      }
      SECTION( "strcasecmp() family()" )
      {
        REQUIRE( w_strcasecmp( "ABCd", "abcd" ) == 0 );
        REQUIRE( w_strcasecmp( w_word, "WORD" ) == 0 );
      }
      SECTION( "strncasecmp() family()" )
      {
        REQUIRE( w_strncasecmp( "ABCq", "abcd", 3 ) == 0 );
        REQUIRE( w_strncasecmp( "ABCq", "abcd", 4 ) != 0 );
        REQUIRE( w_strncasecmp( w_word, "WORD", 3 ) == 0 );
        REQUIRE( w_strncasecmp( w_WORD, w_word, 4 ) == 0 );
        REQUIRE( w_strncasecmp( "word", w_WORD, 4 ) == 0 );
      }
      SECTION( "strchr() family()" )
      {
        char*     s;
        widechar* w;

        REQUIRE( ((s = mtc::w_strchr( "bing", 'i' )) != nullptr && *s == 'i') );
        REQUIRE( ((w = mtc::w_strchr( w_word, 'o' )) != nullptr && *w == 'o') );
      }
      SECTION( "strstr() family()" )
      {
        const char*     s;
        const widechar* w;

        REQUIRE( ((s = mtc::w_strstr( "bing", "in" )) != nullptr && *s == 'i') );
        REQUIRE( ((w = mtc::w_strstr( w_word, w_word + 1 )) != nullptr && *w == w_word[1]) );

        REQUIRE( ((s = mtc::w_strstr( "word", w_word + 1 )) != nullptr && *s == 'o') );
        REQUIRE( ((w = mtc::w_strstr( w_word, "or" )) != nullptr && *w == w_word[1]) );
      }
      SECTION( "strrintf() family" )
      {
        REQUIRE( strprintf( "%d", 7 ) == "7" );
        REQUIRE( strprintf( /*std::allocator*/printing_allocator<char>() )( "start string @%d", 77 ).length() == 16 );

        auto  p = strduprintf( "aaa-%d", 7 );
          REQUIRE( p != nullptr );
          REQUIRE( w_strcmp( p, "aaa-7" ) == 0 );
        delete p;
      }
      SECTION( "strtox() family" )
      {
        char* e;

        REQUIRE( w_strtol( "12345", &e, 0 ) == 12345 );
        REQUIRE( w_strtol( "12345", nullptr, 0 ) == 12345 );
        REQUIRE( w_strtol( "12345", nullptr, 10 ) == 12345 );
        REQUIRE( w_strtol( "12345", nullptr, 16 ) == 0x12345 );

        REQUIRE( w_strtol( "12345", &e, 11 ) == 0 );
        REQUIRE( w_strtol( "12345", nullptr, 11 ) == 0 );

        REQUIRE( w_strtol( "12345", &e, 16 ) == 0x12345 );
        REQUIRE( w_strtol( "12345", nullptr, 16 ) == 0x12345 );
        REQUIRE( w_strtol( "0x12345", &e, 16 ) == 0x12345 );

        REQUIRE( w_strtol( "0x12345", &e, 10 ) == 0 );

        REQUIRE( w_strtol( "-12345", &e, 0 ) == -12345 );
        REQUIRE( w_strtol( "-12345", &e, 10 ) == -12345 );
        REQUIRE( w_strtol( "-12345", &e, 16 ) == -0x12345 );

        REQUIRE( w_strtol( "-0x12345", &e, 0 ) == -0x12345 );
        REQUIRE( w_strtol( "-0x12345", &e, 10 ) == 0 );
        REQUIRE( w_strtol( "-0x12345", &e, 16 ) == -0x12345 );

      /*
        auto  u = mtc::w_strtoul( "12345", &e, 0 );
        */
      }
    }
  } );
