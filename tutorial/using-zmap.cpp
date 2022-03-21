/*
 * zmap - класс поддержки jsonb с некоторыми удобными расширениями, обеспечивающий быстрый доступ
 * к полям с возможностью строгой типизации или без неё, а также сериализации/десериализации
 * структуры в потоки разных типов и из них.
 *
 * Есть преобразование в текстовый json и загрузка из такого формата, в том числе со вспомогательными
 * указаниями желаемых типов элементов.
 */
# include "../zmap.h"
# include "../z_js.h"

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

  assert( *znew.get_int32( "key as string" ) == *zmap.get_int32( "key as string" ) );
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
  zmap["add array"] = mtc::array_charstr{ "s1", "s2", "s3" };         // mtc::array_charstr и
  zmap["add vector"] = std::vector<std::string>{ "s1", "s2", "s3" };  // std::vector<std::string> - это синонимы

  mtc::json::Print( stdout, zmap, mtc::json::print::decorated() );
}

int   main()
{
  CreateSimpleZmapAndSerializeItToBuffer();
  CreateZmapAsInitializerListAndShowHowToUseZmapsAsFields();
  return 0;
}