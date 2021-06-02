/*
  Все кириллические тексты в этом файле представлены кодировкой utf-8. В том числе и этот заголовок,
  обеспечивающий достаточное количество текста, чтобы никто не ошибся с кодировкой.
*/
# include "../wcsstr.h"

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

widechar  w_zero = 0;
widechar  w_word[] = { (widechar)'w', (widechar)'o', (widechar)'r', (widechar)'d', (widechar)0 };
widechar  w_WORD[] = { (widechar)'W', (widechar)'O', (widechar)'R', (widechar)'D', (widechar)0 };

void  TestStrlenFamily()
{
  assert( mtc::w_strlen( "" ) == 0 );
  assert( mtc::w_strlen( &w_zero ) == 0 );

  assert( mtc::w_strlen( "a" ) == 1 );
  assert( mtc::w_strlen( "word" ) == 4 );

  assert( mtc::w_strlen( w_word ) == 4 );

  assert( mtc::w_strlen( "слово" ) == 10 );    // utf-8 encoded
}

void  TestStrdupFamily()
{
  assert( mtc::w_strdup( (const char*)nullptr ) == nullptr );
  assert( mtc::w_strdup( (const char*)nullptr, 100 ) == nullptr );
}

void  TestStrcpyFamily()
{
  char      c_buff[0x100];
  widechar  w_buff[0x100];

  assert( mtc::w_strcmp( mtc::w_strcpy( c_buff, "aaa" ), "aaa" ) == 0 );
  assert( mtc::w_strcmp( mtc::w_strcpy( w_buff, w_word ), "word" ) == 0 );

  assert( mtc::w_strcmp( mtc::w_strcpy( w_buff, "word" ), w_word ) == 0 );

  assert( mtc::w_strcmp( mtc::w_strncpy( c_buff, "aaaa", 10 ), "aaaa" ) == 0 );
  assert(                mtc::w_strncpy( c_buff, "aaa", 3 )[3] == 'a' );

  assert( mtc::w_strcmp( mtc::w_strncpy( w_buff, w_word, 10 ), w_word ) == 0 );
  assert(                mtc::w_strncpy( w_buff, w_word, 3 )[3] == (widechar)'d' );

  assert( mtc::w_strncpy( mtc::w_strcpy( w_buff, "aaaaaaa" ), "bbb", 3 )[4] == (widechar)'a' );

  assert( mtc::w_strcmp( mtc::w_strcpy( w_buff, "aaaaaa",
    []( const char*& s ){  return *s++ != 0 ? 'b' : 0;  } ), "bbbbbb" ) == 0 );

  assert( mtc::w_strcmp( mtc::w_strcpy( w_buff, "aaaaaa",
    []( const char*& s ){  return *s++ != 0 ? 'b' : 0;  } ), "bbbbbb" ) == 0 );

  assert( mtc::w_strcmp( mtc::w_strcpy( w_buff, "\\x77\\x006f\\x072\\x64", hexstr_to_widechar() ), w_word ) == 0 );
}

void  TestStrcatFamily()
{
  char      c_buff[0x100];
  widechar  w_buff[0x100];

  assert( mtc::w_strcmp( mtc::w_strcat( mtc::w_strcpy( c_buff, "red " ), "scar" ),
    "red scar" )  == 0 );
  assert( mtc::w_strcmp( mtc::w_strcat( mtc::w_strcpy( w_buff, "red " ), "scar" ),
    "red scar" )  == 0 );
  assert( mtc::w_strcmp( mtc::w_strcat( mtc::w_strcpy( w_buff, "bad " ), "\\x77\\x006f\\x072\\x64", hexstr_to_widechar() ),
    "bad word" ) == 0 );
}

void  TestStrcmpFamily()
{
  widechar  w_buf1[0x100];
  widechar  w_buf2[0x100];

  assert( mtc::w_strcmp( "abc",                          "abc" ) == 0 );
  assert( mtc::w_strcmp( mtc::w_strcpy( w_buf1, "abc" ), "abc" ) == 0 );
  assert( mtc::w_strcmp( mtc::w_strcpy( w_buf1, "abc" ), mtc::w_strcpy( w_buf2, "abc" ) ) == 0 );
  assert( mtc::w_strcmp( "abc",                          mtc::w_strcpy( w_buf2, "abc" ) ) == 0 );

  assert( mtc::w_strcmp( w_word,                         "\\x77\\x06f\\x072\\x64", hexstr_to_widechar() ) == 0 );
  assert( mtc::w_strcmp( "\\x0077\\x06f\\x72\\x064",     w_word,                   hexstr_to_widechar() ) == 0 );
}

void  TestStrncmpFamily()
{
  widechar  w_buf1[0x100];
  widechar  w_buf2[0x100];

  assert( mtc::w_strncmp( "abcdef",                       "abcfed", 3 ) == 0 );
  assert( mtc::w_strncmp( mtc::w_strcpy( w_buf1, "abc" ), "abd",    2 ) == 0 );
  assert( mtc::w_strncmp( mtc::w_strcpy( w_buf1, "abc" ), mtc::w_strcpy( w_buf2, "acb" ), 2 ) != 0 );
  assert( mtc::w_strncmp( "abc",                          mtc::w_strcpy( w_buf2, "acb" ), 1 ) == 0 );

  assert( mtc::w_strncmp( w_word,                         "\\x77\\x006f\\x072\\x64", 4, hexstr_to_widechar() ) == 0 );
  assert( mtc::w_strncmp( "\\x0077\\x06f\\x72\\x064",     w_word,                    5, hexstr_to_widechar() ) == 0 );
}

void  TestStrcasecmpFamily()
{
  assert( mtc::w_strcasecmp( "ABCd", "abcd" ) == 0 );
  assert( mtc::w_strcasecmp( w_word, "WORD" ) == 0 );
}

void  TestStrncasecmpFamily()
{
  assert( mtc::w_strncasecmp( "ABCq", "abcd", 3 ) == 0 );
  assert( mtc::w_strncasecmp( "ABCq", "abcd", 4 ) != 0 );
  assert( mtc::w_strncasecmp( w_word, "WORD", 3 ) == 0 );
  assert( mtc::w_strncasecmp( w_WORD, w_word, 4 ) == 0 );
  assert( mtc::w_strncasecmp( "word", w_WORD, 4 ) == 0 );
}

void  TestStrchrFamily()
{
  char*     s;
  widechar* w;

  assert( ((s = mtc::w_strchr( "bing", 'i' )) != nullptr && *s == 'i') );
  assert( ((w = mtc::w_strchr( w_word, 'o' )) != nullptr && *w == 'o') );
}

void  TestStrstrFamily()
{
  const char*     s;
  const widechar* w;

  assert( ((s = mtc::w_strstr( "bing", "in" )) != nullptr && *s == 'i') );
  assert( ((w = mtc::w_strstr( w_word, w_word + 1 )) != nullptr && *w == w_word[1]) );

  assert( ((s = mtc::w_strstr( "word", w_word + 1 )) != nullptr && *s == 'o') );
  assert( ((w = mtc::w_strstr( w_word, "or" )) != nullptr && *w == w_word[1]) );
}

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

void  TestAllocatedStrings()
{
  assert( mtc::strprintf( "%d", 7 ) == "7" );
  assert( mtc::strprintf( /*std::allocator*/printing_allocator<char>() )( "start string @%d", 77 ).length() == 16 );

  auto  p = mtc::strduprintf( "aaa-%d", 7 );
    assert( p != nullptr );
    assert( mtc::w_strcmp( p, "aaa-7" ) == 0 );
  delete p;
}

void  TestStrtoxFamily()
{
  char* e;

  assert( mtc::w_strtol( "12345", &e, 0 ) == 12345 );
  assert( mtc::w_strtol( "12345", nullptr, 0 ) == 12345 );
  assert( mtc::w_strtol( "12345", nullptr, 10 ) == 12345 );
  assert( mtc::w_strtol( "12345", nullptr, 16 ) == 0x12345 );

  assert( mtc::w_strtol( "12345", &e, 11 ) == 0 );
  assert( mtc::w_strtol( "12345", nullptr, 11 ) == 0 );

  assert( mtc::w_strtol( "12345", &e, 16 ) == 0x12345 );
  assert( mtc::w_strtol( "12345", nullptr, 16 ) == 0x12345 );
  assert( mtc::w_strtol( "0x12345", &e, 16 ) == 0x12345 );

  assert( mtc::w_strtol( "0x12345", &e, 10 ) == 0 );

  assert( mtc::w_strtol( "-12345", &e, 0 ) == -12345 );
  assert( mtc::w_strtol( "-12345", &e, 10 ) == -12345 );
  assert( mtc::w_strtol( "-12345", &e, 16 ) == -0x12345 );

  assert( mtc::w_strtol( "-0x12345", &e, 0 ) == -0x12345 );
  assert( mtc::w_strtol( "-0x12345", &e, 10 ) == 0 );
  assert( mtc::w_strtol( "-0x12345", &e, 16 ) == -0x12345 );

/*
  auto  u = mtc::w_strtoul( "12345", &e, 0 );
  */
}

int   main( int argc, char* argv[] )
{
  TestStrlenFamily();
  TestStrdupFamily();
  TestStrcpyFamily();
  TestStrcatFamily();
  TestStrcmpFamily();
  TestStrncmpFamily();
  TestStrcasecmpFamily();
  TestStrncasecmpFamily();
  TestStrchrFamily();
  TestStrstrFamily();
  TestAllocatedStrings();
  TestStrtoxFamily();

  return 0;
}
