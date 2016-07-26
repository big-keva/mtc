# include <mtc/zarray.h>
# include <mtc/jsonTools.h>
# include <cstdarg>

using namespace mtc;

template <class testtype>
void  TestParseVal( const char* strval, mtc::int32_t  vvalue,
                    const char* vtempl, const char*   sztype )
{
  jsonstream<const char>  stream( strval );
  testtype                avalue;

  printf( "test \"%s\" as %s: ", strval, sztype );

  if ( ParseJson( stream, avalue ) != nullptr )
  {
    printf( vtempl, avalue );
    printf( ", %s\n", avalue == vvalue ? "OK" : "FAULT" );
  }
    else
  printf( "FAULT\n" );
}

# define  test_parse_val( _t_, _v_, _f_ ) TestParseVal<_t_>( #_v_, _v_, _f_, #_t_ )

template <class testtype>
void  TestParseArr( const char* sztype, const char* strval, int nvalue, ... )
{
  jsonstream<const char>  stm( strval );
  array<testtype>         arr;

  printf( "test array<%s> on %s: ", sztype, strval );

  if ( ParseJson( stm, arr ) != nullptr )
  {
    if ( nvalue == arr.size() )
    {
      va_list vaargs;
      int     nindex = 0;

      va_start( vaargs, nvalue );

      while ( nvalue > 0 && va_arg( vaargs, testtype ) == arr[nindex++] )
        --nvalue;
          
      va_end( vaargs );

      printf( "%s\n", nvalue == 0 ? "OK" : "elements differ, FAULT" );
    } else printf( "element count mismatch, FAULT\n" );
  }
    else
  printf( "FAULT\n" );
}

int main()
{
  const char*             sz;
  jsonstream<const char>  in;
  jsonRevive*             rv;
  zarray<>                zv;

// create revive struct
  rv =  add_charstr       ( "query", 
        add_word32        ( "from",
        add_word32        ( "to",
        add_array_charstr ( "users",
        add_array_zarray  ( "fields",
            add_charstr       ( "name",
            add_charstr       ( "surname",
            add_double        ( "age", nullptr ))),
                            nullptr ) ) ) ) );

  test_parse_val( char, 0, "%d" );
  test_parse_val( char, -0, "%d" );
  test_parse_val( char, 11, "%d" );
  test_parse_val( char, -11, "%d" );
  test_parse_val( char, 311, "%d" );  // must fail
  test_parse_val( char, -311, "%d" ); // must fail

  test_parse_val( mtc::int16_t, 0, "%d" );
  test_parse_val( mtc::int16_t, -0, "%d" );
  test_parse_val( mtc::int16_t, 11, "%d" );
  test_parse_val( mtc::int16_t, -11, "%d" );
  test_parse_val( mtc::int16_t, 311, "%d" );
  test_parse_val( mtc::int16_t, -311, "%d" );
  test_parse_val( mtc::word16_t, -311, "%d" );    // must fail

  TestParseArr<char>          ( "char",     "[1, 2, 3, 4, 5]", 5, 1, 2, 3, 4, 5 );
  TestParseArr<mtc::int32_t>  ( "int32_t",  "[-0, -1, -2, -3, -4]", 5, 0, -1, -2, -3, -4 );
  TestParseArr<mtc::word32_t> ( "word32_t", "[-0, -1, -2, -3, -4]", 5, 0, 1, 2, 3, 4 ); // must fail

// test string array
  array<_auto_<char>>     chsarr;
  array<_auto_<widechar>> wcsarr;

  sz = "[\"aaa\", \"bbb\", \"слово\" ]";
  printf( "tesing string array %s: %s\n", sz, ParseJson( in = sz, chsarr ) != nullptr ?
    "OK" : "FAULT" );

  sz = "[\"\\u0412\\u0467\\u0492\", \"aaa\", \"bbb\", \"слово\" ]";
  printf( "tesing string array %s: %s\n", sz, ParseJson( in = sz, wcsarr ) != nullptr ?
    "OK" : "FAULT" );

  sz = "{ \"query\": \"search documents\", "
         "\"from\": 1, "
         "\"to\": 10, "
         "\"users\": [\"fiva\", \"danila\", \"kaa\"], "
         "\"settings\": { \"weight\": 0.01 }, "
         "\"fields\": [{\"name\": \"Yuri\", \"surname\": \"Nazarov\", \"age\": 42}] }";

  printf( "testing zarray %s: %s\n", sz, ParseJson( in = sz, zv ) != nullptr ? "OK" : "FAULT" );
    PrintJson( stdout, zv );

  return 0;
}
