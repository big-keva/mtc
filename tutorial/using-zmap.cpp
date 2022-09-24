/*
 * zmap - класс поддержки jsonb с некоторыми удобными расширениями, обеспечивающий быстрый доступ
 * к полям с возможностью строгой типизации или без неё, а также сериализации/десериализации
 * структуры в потоки разных типов и из них.
 *
 * Есть преобразование в текстовый json и загрузка из такого формата, в том числе со вспомогательными
 * указаниями желаемых типов элементов.
 */
# include "../zmap.h"
# include "../json.h"

//
// Заполнение объектов zmap возможно с явным указанием типов помещаемых объектов.
//
void  FillZmapWithTypedAPI()
{
  mtc::zmap m;

  m.set_int16( "key1", 7 );           // "key1" -> 7
 *m.set_int32( "key2" ) = 11;         // "key2" -> 11
 *m.set_int64( "key3", 2 ) += 15;     // "key3" -> (int64)17

  m.set_charstr( "str1", "some string" );                 // "str1" -> "some string"
  m.set_array_float( "arr_float", { 1.7, 2.4, 5.5 } );    // "arr_float" -> [1.7, 2.4, 5.5]
 *m.set_array_double( "arr_double" ) = std::move(
    std::vector<double>{ 2.8, 3.5, 6.6 } );

  m.set_array_charstr( "arr_str" );
  m.get_array_charstr( "arr_str" )->push_back( "other" );
  m.get_array_charstr( "arr_str" )->push_back( "string" );
  m.get_array_charstr( "arr_str" )->push_back( "array" );

  m.set_zmap( "zmap" )->set_byte( "byte", 255 );
  m.get_zmap( "zmap" )->set_char( "char", -1 );

  m.set_array_zval( "arr_zval", { "string value", 2, -1.7,
    mtc::zmap{ { "key1", std::vector<mtc::charstr>{ "a", "b", "c" } } },
    mtc::array_zval{ 7, 6, 1.0 } } );

  fprintf( mtc::json::Print( stderr, m, mtc::json::print::decorated() ), "\n" );
}

/*
 * В этом примере мы создаём простой zmap по шагам, ключ за ключом, сериализуем в массив, после чего
 * загружаем и убеждаемся, что получили точно такой же zmap.
 *
 * Для этого запрашиваем значения полей и сравниваем с оригинальными
 */
void  CreateSimpleZmapAndSerializeItToBuffer()
{
  mtc::zmap         zmap;
  mtc::zmap         znew;
  std::vector<char> buff;

  zmap.set_int32( "key as string", 32 );    // ключ - строка
  zmap.set_charstr( 32U, "string value" );  // ключ - unsigned
  zmap.set_float( "float value", 1.71 );
 *zmap.set_float( "other float" ) = 2.82;

  zmap.set_array_charstr( "string array", { "s1", "s2", "s3" } );

  buff.resize( zmap.GetBufLen() );    // резервируем место для сериализации
  zmap.Serialize( buff.data() );

  znew.FetchFrom( (const char*)buff.data() );

  assert( znew == zmap );
}

/*
 * zmap может создаваться сразу со списком полей, как initializer_list<>, а также можно использовать
 * оператор-помощник [] с ключами в ряде ситуаций.
 *
 * В финале построенный zmap печатается как json.
 */
void  CreateZmapAsInitializerListAndShowHowToUseZmapsAsFields()
{
  auto  zmap = mtc::zmap{
    { "integer field", 32 },
    { "long integer feeld", (int64_t)64 },
    { "string field", "field value" },
    { "zmap field", mtc::zmap{
      { "zmap field 1", 1 },
      { "zmap field 2", "2" } } },
    { "array of integer values", mtc::array_int32{ 1, 2, 3 } },
    { "array of float values", mtc::array_float{ 1, 2, 3 } },
    { "array of double values", mtc::array_double{ 1, 2, 3 } },
    { "array of string values", mtc::array_charstr{ "1", "2", "3" } },
    { "array of any value", mtc::array_zval{ 1, 2.0, "3" } },
    { "array of zmap", mtc::array_zmap{
      { { 1U, 1 } },
      { { 2U, 2.0 } },
      { { "3", "3.0" } } } }
  };

  zmap["add field"] = 19U;
  zmap["add array"] = mtc::array_charstr{ "s1", "s2", "s3" };
  zmap["add vector"] = std::vector<uint32_t>{ 81, 82, 83 };

  assert(  zmap.get_word32( "add field" ) != nullptr );
  assert( *zmap.get_word32( "add field" ) == 19U );
  assert(  zmap.get_word32( "add field", 0 ) == 19U );
}

/*
 * Доступ к элементам zmap тоже может быть по []
 */
void  TestZmapFieldsAccess()
{
  auto  zmap = new mtc::zmap{
    { "integer field", 32 },
    { "long integer field", (int64_t)64 },
    { "string field", "field value" },
    { "zmap field", mtc::zmap{
      { "zmap field 1", 1 },
      { "zmap field 2", "2" } } },
    { "array of integer values", mtc::array_int32{ 1, 2, 3 } },
    { "array of double values", mtc::array_double{ 1, 2, 3 } },
    { "array of string values", mtc::array_charstr{ "1", "2", "3" } },
    { "array of any value", mtc::array_zval{ 1, 2.0, "3" } },
    { "array of zmap", mtc::array_zmap{
      { { 1U, 1 } },
      { { 2U, 2.0 } },
      { { "3", "3.0" } } } }
  };

  if ( is_set( (*zmap)["long integer field"] ) && (*zmap)["long integer field"] == (int64_t)64 )
  {
    auto p = (*zmap)["int"];

    delete zmap;
    if ( p == 64 )
    {

    int i = 0;
    //auto  ft = get_type( zmap["integer_field"] );
    }
  }
}

class Schema
{
  mtc::zmap   typesMap;
  mtc::zmap   fieldMap;

public:
  Schema( const mtc::zmap& z = {} );

public:
  auto  operator []( const mtc::zmap::key& ) -> Schema;
  auto  get_type() -> uint8_t;
};

Schema::Schema( const mtc::zmap& z ): typesMap{
  { "char",     mtc::zval::z_char },
  { "byte",     mtc::zval::z_byte },
  { "int16",    mtc::zval::z_int16 },
  { "int32",    mtc::zval::z_int32 },
  { "int64",    mtc::zval::z_int64 },
  { "uint16",   mtc::zval::z_word16 },
  { "uint32",   mtc::zval::z_word32 },
  { "uint64",   mtc::zval::z_word64 },
  { "float",    mtc::zval::z_float },
  { "double",   mtc::zval::z_double },
  { "string",   mtc::zval::z_widestr },
  { "widestr",  mtc::zval::z_widestr },
  { "charstr",  mtc::zval::z_charstr } }
{
  if ( z.get_charstr( "type" ) != nullptr )
    typesMap[*z.get_charstr( "type" )] = typesMap;

  if ( (fieldMap = z.get_zmap( "properties", {} )).empty() )
    throw std::invalid_argument( "no 'properties' provided in schema" );
}

void  TestParseJson()
{
  /*
  mtc::zmap zmap;

  mtc::json::Parse(
    "{"
      "\"i\": 7,\n"
      "\"b\": 3\n"
    "}", zmap,
    mtc::zmap{
      { "type", "object" },
      { "properties", mtc::zmap{
        { "l", mtc::zmap{ { "type", "object" } } },
        { "r", mtc::zmap{ { "type", "object" } } },
        { "o", mtc::zval::z_byte } } },
      { "required", mtc::array_charstr{ "l", "o", "r" } }
    } );

  mtc::json::Print( stdout, zmap );
  */
  Schema  schema( {
    { "type", "query" },
    { "args", mtc::zmap{
      { "l", "query" },
      { "r", mtc::zmap{
        { "type", "value" },
        { "args", mtc::zmap{
          { "l", mtc::zval::z_double } } } } },
      { "o", mtc::zval::z_int16 } } },
    { "required", mtc::array_zval{ "o" } } } );
}

int   main()
{
  FillZmapWithTypedAPI();
  return 0;
}