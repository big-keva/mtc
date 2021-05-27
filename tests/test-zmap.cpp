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
/*
class val_t
{
    int   t;
public:
  val_t( int v ): t( v )  {}

  operator const int& () const {  return t;  }
  auto  operator * () const -> const int& {  return t;  }
};

template <class T>
class cls_t
{
  T t;
public:
  cls_t( const T& s ): t( s ) {}

  operator const T& () const {  return t;  }

  auto  operator * () const -> const T& {  return t;  }

  auto  operator -> () const -> const T*  {  return &t;  }
};

template <class T>
class class_ptr
{
  struct value
  {
    T               t;
    std::atomic_int c;
  public:
    template <class ... Args>
    value( Args ... args ): t( std::move( args... ) ) {  c = 1;  }
  };

  value*  p = nullptr;

public:
  class_ptr() = default;
  class_ptr( const class_ptr& s ) {  if ( (p = s.p) != nullptr )  ++p->c;  }
  template <class ... Args>
  class_ptr( Args... args ): p( new value( args ... ) ) {}

public:
  auto  operator * () const -> const T& {  return p->t.operator *();  }
  auto  operator -> () const -> const T&  {  return p->t;  }
};

template <class T, class ... Args>
class_ptr<T>  make_ptr( Args ... args ) {  return class_ptr<T>( args... );  }

template <class T>
using cls_p = class_ptr<cls_t<T>>;
using val_p = class_ptr<val_t>;
*/
/*
template <class T, class V>
bool  operator == ( const cls_t<T>& s, V v ) {  return ((const T&)s) == v;  }
*/

auto  CreateDump( const mtc::zmap& z ) -> std::vector<char>
{
  auto  stored = std::vector<char>( z.GetBufLen() );
  return z.Serialize( stored.data() ), std::move( stored );
}

int main()
{
  auto  buff = CreateDump( {
    { "char", 'c' },
    { "charstr", "string" },
    { "array_charstr", mtc::array_charstr{ "s1", "t2", "u3" } },
    { "array_int32", mtc::array_int32{ 1, 2, 3 } },
    { "array_zval", mtc::array_zval{ 1, "aaa", 3.7, mtc::zmap{ { "key", "value" } } } },
    { "array_zmap", mtc::array_zmap{ { { "key", "value" }, { "int", 9 } } } },
    { "zmap", mtc::zmap{
        { "int", 9 }
      } } } );

  auto  dump = mtc::zmap::dump( buff.data() );
  {
    auto  pchar = dump.get_char( "char" );  fprintf( stdout, "%c\n", *pchar );
//    auto  pbyte = dump.get_byte( "char" );  fprintf( stdout, "%c\n", *pbyte );
    auto  pstrs = dump.get_array_charstr( "array_charstr" );
      fprintf( stdout, "%u\n", (unsigned)pstrs->size() );

      for ( auto it: *pstrs )
        fprintf( stdout, "\'%s\'\n", it.c_str() );
      for ( auto it = pstrs->begin(); it != pstrs->end(); ++it )
        fprintf( stdout, "\'%s\'\n", it->c_str() );

    auto  pints = dump.get_array_int32( "array_int32" );
      fprintf( stdout, "%u\n", (unsigned)pints->size() );

      for ( auto it: *pints )
        fprintf( stdout, "\'%d\'\n", it );
      for ( auto it = pints->begin(); it != pints->end(); ++it )
        fprintf( stdout, "\'%d\'\n", *it );

    auto  pmap = dump.get_zmap( "zmap" );
      fprintf( stdout, "%d\n", pmap->get_int32( "int", 0 ) );

    auto  zget = dump.get_zmap( "dump", {
      { "int", 91 },
      { "zmap", mtc::zmap{
        { "key", "str" } } },
      { "float", 9.97 } } );

    fprintf( stdout, "%d\n", zget.get_int32( "int", 0 ) );

    {
      auto  test = zget.get_zmap( "zmap" );
        fprintf( stdout, "%s\n", test->get_charstr( "key", "" ).c_str() );

      auto  view = zget.get_zmap( "zmap", {} );
        fprintf( stdout, "%s\n", view.get_charstr( "key", "" ).c_str() );
    }

    auto  varr = dump.get_array_zval( "array_zval" );
      for ( auto it: *varr )
      {
        fprintf(
          mtc::json::Print( stdout, it.to_zval() ), "\n" );
      }

    assert( (*varr == mtc::array_zval{ 1, "aaa", 3.7, mtc::zmap{ { "key", "value" } } }) );

    auto  zarr = dump.get_array_zmap( "array_zmap" );
      for ( auto it: *zarr )
      {
        fprintf(
          mtc::json::Print( stdout, it.to_zmap() ), "\n" );
      }

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
