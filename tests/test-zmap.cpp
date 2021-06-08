# include "../zmap.h"
# include "../z_js.h"
# include "../utf.hpp"
# include "../serialize.h"

struct byte_counter
{
  size_t length = 0;
};

template <>
byte_counter* Serialize( byte_counter* bc, const void*, size_t l )
{
  if ( bc != nullptr )
    bc->length += l;
  return bc;
}

auto  CreateDump( const mtc::zmap& z ) -> std::vector<char>
{
  auto  stored = std::vector<char>( z.GetBufLen() );
  return z.Serialize( stored.data() ), std::move( stored );
}

int main()
{
  auto  zmap = mtc::zmap{
    { "char", 'c' },
    { "charstr", "string" },
    { "array_charstr", mtc::array_charstr{ "s1", "t2", "u3" } },
    { "array_int32", mtc::array_int32{ 1, 2, 3 } },
    { "array_zval", mtc::array_zval{ 1, "aaa", 3.7, mtc::zmap{ { "key", "value" } } } },
    { "array_zmap", mtc::array_zmap{ { { "key", "value" }, { "int", 9 } } } },
    { "zmap", mtc::zmap{
        { "int", 9 },
        { "float", (float)9.0 } } } };

  auto  buff = CreateDump( zmap );

  auto  dump = mtc::zmap::dump( buff.data() );

  fprintf( mtc::json::Print( stdout, dump, mtc::json::print::decorated() ),
    "\n" );

    assert( dump == zmap );

 /*
  * test pointer access to values
  */
  {
    assert( dump.get_char( "char" ) != nullptr );
      assert( *dump.get_char( "char" ) == 'c' );
    assert( dump.get_charstr( "charstr" ) != nullptr );
      assert( *dump.get_charstr( "charstr" ) == "string" );
    assert( dump.get_array_charstr( "array_charstr" ) != nullptr );
      assert( (*dump.get_array_charstr( "array_charstr" ) == mtc::array_charstr{ "s1", "t2", "u3" }) );
    assert( dump.get_array_int32( "array_int32" ) != nullptr );
      assert( (*dump.get_array_int32( "array_int32" ) == mtc::array_int32{ 1, 2, 3 }) );
    assert( dump.get_array_zval( "array_zval" ) != nullptr );
      assert( (*dump.get_array_zval( "array_zval" ) == mtc::array_zval{ 1, "aaa", 3.7, mtc::zmap{ { "key", "value" } } }) );
    assert( dump.get_array_zmap( "array_zmap" ) != nullptr );
      assert( (*dump.get_array_zmap( "array_zmap" ) == mtc::array_zmap{ { { "key", "value" }, { "int", 9 } } }) );
    assert( dump.get_zmap( "zmap" ) != nullptr );
      assert( (*dump.get_zmap( "zmap" ) == mtc::zmap{ { "int", 9 }, { "float", (float)9.0 } }) );
  }

 /*
  * test value access
  */
  {
    assert( dump.get_char( "char", 'a' ) == 'c' );
      assert( dump.get_char( "?char", 'a' ) == 'a' );
    assert( dump.get_charstr( "charstr", "non-string" ) == "string" );
      assert( dump.get_charstr( "?charstr", "non-string" ) == "non-string" );
    assert( (dump.get_zmap( "zmap", { { "int", 5 } } ) == mtc::zmap{
      { "int", 9 },
      { "float", (float)9.0 } }) );
    assert( (dump.get_zmap( "?zmap", { { "int", 5 } } ) == mtc::zmap{
      { "int", 5 } }) );
  }

  {
    auto  zval = mtc::zval( 1 );
      auto  vbuf = std::vector<char>( zval.GetBufLen() );
      zval.Serialize( vbuf.data() );
    auto  zv_1 = mtc::zval::dump( vbuf.data() );
    auto  zv_2 = zval;

    assert( zv_1.get_int32() != nullptr );
    assert( zv_2.get_int32() != nullptr );
    assert( *zv_1.get_int32() == *zv_2.get_int32() );
  }

  // test zmap::dump::get()
  // должна возвращать zval::dump
  {
    auto  pchr = dump.get( "char" );
    auto  pstr = dump.get( "charstr" );
    auto  parr = dump.get( "array_int32" );
    auto  pnul = dump.get( "charstr-xxx" );
      assert( pchr != nullptr );
      assert( pstr != nullptr );
      assert( parr != nullptr );
      assert( pnul == nullptr );

      assert( pchr->get_type() == mtc::zval::z_char );
      assert( pchr->get_char() != nullptr );
      assert( *pchr->get_char() == 'c' );
      assert( *pchr == 'c' );

      assert( parr->get_type() == mtc::zval::z_array_int32 );
      assert( parr->get_array_int32() != nullptr );
      assert( (*parr->get_array_int32() == mtc::array_int32{ 1, 2, 3 }) );
      assert( (*parr == mtc::array_int32{ 1, 2, 3 }) );

      assert( pstr->get_type() == mtc::zval::z_charstr );
      assert( pstr->get_charstr() != nullptr );
      assert( *pstr->get_charstr() == "string" );
      assert( *pstr == "string" );
  }

  // test zmap::view::get()
  // должна возвращать zval::view
  {
    auto  view = dump.get_zmap( "zmap", { { "int", 8 }, { "float", (float)8.0 } } );
      auto  pint = view.get( "int" );
        assert( pint != nullptr );
        assert( *pint->get_int32() == 9 );
      // assert( *pint == 9 );
      auto  pflo = view.get( "float" );
        assert( pflo != nullptr );
        assert( *pflo->get_float() == 9.0 );
      auto  pnul = view.get( "none" );
        assert( pnul == nullptr );
  }
  {
    auto  view = dump.get_zmap( "zxxx", { { "int", 8 }, { "float", (float)8.0 } } );
      auto  pint = view.get( "int" );
        assert( pint != nullptr );
        assert( *pint->get_int32() == 8 );
      // assert( *pint == 9 );
      auto  pflo = view.get( "float" );
        assert( pflo != nullptr );
        assert( *pflo->get_float() == 8.0 );
      auto  pnul = view.get( "none" );
        assert( pnul == nullptr );
  }

  // setting new values to zval::view
  {
    auto  view = dump.get_zmap( "zmap", {} );
      auto  pnul = view.get( "int_" );
      auto  zval = mtc::zval( 7 );
      auto  pint = view.get_int32( "int" );
      auto  i_32 = (int32_t)1;

      if ( pint == nullptr )
        pint = &i_32;

      if ( pnul == nullptr )
        pnul = &zval;
  }

  {
    auto  v = mtc::zval( 1 );
      fprintf( stdout, "zval as int32 -> " );
      fprintf( mtc::json::Print( stdout, v ), "\n" );
  }
  {
    auto  v = mtc::array_int32{ 1, 2, 3 };
      fprintf( stdout, "zval as array_int32 -> " );
      fprintf( mtc::json::Print( stdout, v ), "\n" );
  }
  {
    fprintf( stdout, "===== access to fields =====\n" );

    auto  pchar = dump.get_char( "char" );
      fprintf( stdout, "'char' -> '%c'\n", *pchar );

//    auto  pbyte = dump.get_byte( "char" );  fprintf( stdout, "%c\n", *pbyte );
    auto  pstrs = dump.get_array_charstr( "array_charstr" );
      fprintf( stdout, "'array_charstr' -> " );
      fprintf( mtc::json::Print( stdout, *pstrs ), "\n" );

    auto  pints = dump.get_array_int32( "array_int32" );
      fprintf( stdout, "'array_int32' -> " );
      fprintf( mtc::json::Print( stdout, *pints ), "\n" );

    auto  pzmap = dump.get_zmap( "zmap" );
      fprintf( stdout, "'zmap' -> " );
      fprintf( mtc::json::Print( stdout, *pzmap ), "\n" );

    auto  azmap = dump.get_zmap( "dump", {
      { "int", 91 },
      { "zmap", mtc::zmap{
        { "key", "str" } } },
      { "float", 9.97 } } );
    fprintf( stdout, "default value of 'zmap' -> " );
    fprintf( mtc::json::Print( stdout, azmap ), "\n" );

    auto  pzarr = dump.get_array_zmap( "array_zmap" );
      fprintf( stdout, "'array_zmap' -> " );
      fprintf( mtc::json::Print( stdout, *pzarr ), "\n" );

    auto  pvarr = dump.get_array_zval( "array_zval" );
      fprintf( stdout, "'array_zval' -> " );
      fprintf( mtc::json::Print( stdout, *pvarr ), "\n" );
    assert( (*pvarr == mtc::array_zval{ 1, "aaa", 3.7, mtc::zmap{ { "key", "value" } } }) );

/*
    pmap = dump.get_zmap( "dump", {
      { "int", 9 } } );*/
  }

// zmap - произвольная индексируемая структура полей, массивов и структур с доступом по строковым,
// численным и wcs-ключам.
// Может быть отображён в json.
  {
    mtc::zmap zm;

    zm.set_char( "char", 'a' );
    zm.set_byte( "byte (unsigned char)", 0xb );

    zm.set_int16( "16-bit integer", 0xc );
    zm.set_int32( "32-bit integer", -14 );
    zm.set_int64( "64-bit integer", 971 );

    zm.set_int16( 16, 0xb );
    zm.set_int32( 32, -15 );
    zm.set_int64( 64, 971 );

    zm.set_word16( "16-bit unsigned integer", 0xc );
    zm.set_word32( "32-bit unsigned integer", 0xd );
    zm.set_word64( "64-bit unsigned integer", 0xe );

    zm.set_float( "float", 1.0 );
    zm.set_double( "double", -1.0 );

    zm.set_charstr( "string", "simple string" );
//    zm.set_widestr( "string", L"simple string" ); - widestr is utf-16 string

    zm.set_array_double( "double array", { 1.0, 2.0, 3.0 } );
    zm.set_array_charstr( "string array", { "string", "value" } );

    mtc::json::Print( stdout, zm, mtc::json::print::decorated() );
  }

// Возможна инициализация предварительно размещённых полей zmap.
  {
    mtc::zmap zm;

    *zm.set_int16( "16-bit integer" ) = 16;
    *zm.set_int32( "32-bit integer" ) = -14;
    *zm.set_charstr( "string placement" ) = "971";

    *zm.set_array_charstr( "string array placement" ) = { "string", "array", "placement" };

    mtc::json::Print( stdout, zm, mtc::json::print::decorated() );
  }

// Доступна сокращённая запись при инициализации.
  {
    mtc::zmap zm;

    zm[16] = 16;
    zm[32] = -32;
    zm[64] = (uint64_t)64;
    zm["key"] = "value";
    zm["float"] = (float)1.0;
    zm["double"] = (double)1.0;

    zm["string_array"] = mtc::array_charstr{ "string", "value", "array" };

    mtc::json::Print( stdout, zm, mtc::json::print::decorated() );
  }

// Значения доступны как с контролем наличия, так и со значениями по умолчанию
  {
    mtc::zmap zm;

    zm.set_int32( "int32", 5 );

  // получаем значение по ключу 'int32'
    assert( zm.get_int32( "int32" ) != nullptr );
      assert( *zm.get_int32( "int32" ) == 5 );

  // получаем значение по отсутствующему ключу
    assert( zm.get_int32( "other int" ) == nullptr );
      assert( zm.get_int32( "other int", 3 ) == 3 );
  }

// Контроль типов строгий...
// ... но справедливый.
  {
    mtc::zmap zm;

    zm.set_int32( "int32", 5 );

  // пробуем получить double по ключу 'int32' - не получится
    assert( zm.get_double( "int32" ) == nullptr );
    assert( zm.get_word32( "int32" ) == nullptr );

    assert( zm.get_word32( "int32", 3 ) == 3 );

  // но проверить сам факт наличия ключа и тип значения возможно:
    assert( zm.get( "int32" ) != nullptr );
      assert( zm.get( "int32" )->get_type() == mtc::zval::z_int32 );
  }

// Возможна инициализация списками
  {
    mtc::zmap zm{
      { "key 1", "value-1" },
      { "key 2", (int32_t)10 }
    };

    mtc::json::Print( stdout, zm, mtc::json::print::decorated() );
  }

// Элементами могут быть аналогичные структуры и их массивы; массивы совместимы с std::vector<>
  {
    mtc::zmap zm;

    zm.set_zmap( "zmap 1", {
        { "key", "value" } } );

    zm["zmap 2"] = mtc::zmap{
        { "key", "value" },
        { 2, 2 } };

    zm.set_array_zmap( "array 1", {
      {
        { "member 1", 1 }
      },
      {
        { "member 2", "some string value" }
      } } );

    zm["array 2"] = mtc::array_zmap{
      {
        { "member 3", 10 }
      },
      {
        { "member 4", "other string value" }
      } };

    zm["array 3"] = std::vector<double>{ 1.1, 2.2, 3.3 };

    mtc::json::Print( stdout, zm, mtc::json::print::decorated() );
  }

// Любой zmap можно сериализовать в массив заведомо известного размера, а потом извлечь
  {
    mtc::zmap zm{
      { "key 1", "value 1" },
      { "key 2", "value 2" } };
    mtc::zmap zl;

    std::vector<char> zb( zm.GetBufLen() );
      zm.Serialize( zb.data() );

    zl.FetchFrom( (const char*)zb.data() );

    assert( zm == zl );
  }

// Способ сериализации можно переопределить:
// см. структуру byte_counter
  {
    mtc::zmap zm{
      { "test", 1 },
      { "string", "s" },
      { "array", mtc::zmap{
        { 3, 4 },
        { 1, 2 },
        { 5, 6 } } } };
    byte_counter  bc;

    zm.Serialize( &bc );

    assert( bc.length == zm.GetBufLen() );
  }

  return 0;
}
