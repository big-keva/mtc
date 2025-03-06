# include "../zmap.h"
# include "../json.h"
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

auto  CreateDump( const mtc::zval& z ) -> std::vector<char>
{
  auto  stored = std::vector<char>( z.GetBufLen() );
  return z.Serialize( stored.data() ), std::move( stored );
}

void  TestIterators()
{
  auto  zmap = mtc::zmap{
    { "key1", "value1" },
    { "key2", "value2" },
    { 0U, 3 } };

  for ( auto& it: zmap )
  {
    std::string keystr(
      it.first.is_charstr() ? it.first.to_charstr() :
      it.first.is_widestr() ? mtc::utf8::encode( it.first.to_widestr() ) : std::to_string( it.first.operator unsigned int() ) + 'U' );
    std::string valstr = it.second.to_string();

    fprintf( stdout, "%s\t->\t%s\n", keystr.c_str(), valstr.c_str() );
  }
}

namespace mtc
{
  struct node_t
  {
    struct buffer;

    enum: uint32_t
    {
      O_FRAGLEN = 0x0000ffff,   // character count in a block
      O_TREELEN = 0x01ff0000,   // 9 bit, count of subnodes (0-256)
      O_TREELIM = 0x3e000000,   // 5 bit, 2^treelim is allocated limit, 0 = 0, 16 = 256
      O_KEYTYPE = 0xc0000000    // 2 bit, 0 = none, 1 = unsigned, 2 = charstr and 3 = widestr
    };
    union buf_or_ptr
    {
      uint8_t*  psz;
      uint8_t   buf[1];
    };
    using zvalue_ptr = std::unique_ptr<zval>;

    node_t*     pnodes = nullptr;
    zvalue_ptr  pvalue;
    uint32_t    ustate = 0;
    buf_or_ptr  keyptr;

  public:
    node_t() = default;
    node_t( node_t&& );
    node_t( const node_t& ) = delete;
   ~node_t() {  clear();  }

    node_t( const uint8_t*  keytop, size_t length );
  protected:

    static  auto  get_tree_limit( uint32_t ) -> size_t;
    inline  auto  get_string_len() const -> size_t  {  return (ustate & O_FRAGLEN);  }
    inline  auto  get_string_ptr() const -> const uint8_t*;
    inline  auto  get_tree_count() const -> size_t  {  return (ustate & O_TREELEN) >> 16;  }
    inline  auto  get_tree_limit() const -> size_t  {  return (ustate & O_TREELIM) >> 25;  }
    inline  auto  get_type_value() const -> size_t  {  return (ustate & O_KEYTYPE) >> 30;  }
    inline  auto  set_string_len( size_t len ) -> node_t& {  ustate = (ustate & ~O_FRAGLEN) | len;  return *this;  }
    inline  auto  set_tree_count( size_t cnt ) -> node_t& {  ustate = (ustate & ~O_TREELEN) | (cnt << 16);  return *this;  }
    inline  auto  set_tree_limit( size_t lim ) -> node_t& {  ustate = (ustate & ~O_TREELIM) | (lim << 25);  return *this;  }
    inline  auto  set_type_value( unsigned typ ) -> node_t&  {  ustate = (ustate & ~O_KEYTYPE) | (typ << 30);  return *this;  }

    static  auto  allocate_nodes( size_t ) -> node_t*;

  protected:
    auto  GetBufLen() const -> size_t;
    template <class O>
    auto  Serialize( O* ) const -> O*;
    template <class S>
    auto  FetchFrom( S* ) -> S*;

  public:
    void  clear();
    void  print( FILE*, const std::string& topstr = "", const std::string& othstr = "" );
    int   lines() const;

    auto  insert( const uint8_t* key, size_t len ) -> node_t*;
    auto  insert( node_t* pos, node_t&& ) -> node_t*;

  };

  struct node_t::buffer
  {
    enum: int
    {  offset = sizeof(node_t) - offsetof(node_t, keyptr)  };
  };

  // node_t implementation

  node_t::node_t( node_t&& node ):
    pnodes( node.pnodes ),
    pvalue( std::move( node.pvalue ) ),
    ustate( node.ustate )
  {
    auto  cchstr = get_string_len();

    if ( cchstr <= sizeof(node_t) - offsetof(node_t, keyptr) ) memcpy( keyptr.buf, node.keyptr.buf, cchstr );
      else keyptr.psz = node.keyptr.psz;

    node.pnodes = nullptr;
    node.ustate = 0;
  }

  node_t::node_t(
    const uint8_t*  keytop,
    size_t          length ): pnodes( nullptr ), ustate( length )
  {
    assert( length <= 0xffff );

    if ( length <= sizeof(node_t) - offsetof(node_t, keyptr) ) memcpy( keyptr.buf, keytop, length );
      else keyptr.psz = (uint8_t*)memcpy( new uint8_t[length], keytop, length );
  }

  void  node_t::clear()
  {
    if ( pnodes != nullptr )
    {
      for ( auto ptr = pnodes, end = ptr + get_tree_count(); ptr != end; ++ptr )
        ptr->~node_t();
      delete[] (char*)pnodes;
    }
    pnodes = nullptr;
    pvalue.reset();
    if ( get_string_len() > sizeof(node_t) - offsetof(node_t, keyptr) && keyptr.psz != nullptr )
      delete [] keyptr.psz;
    ustate = 0;
  }

  auto  node_t::get_string_ptr() const -> const uint8_t*
  {
    return get_string_len() <= sizeof(node_t) - offsetof(node_t, keyptr) ? keyptr.buf : keyptr.psz;
  }

  auto  node_t::get_tree_limit( uint32_t count ) -> size_t
  {
    return count == 0   ? 0 :
           count <= 2   ? 1 :
           count <= 4   ? 2 :
           count <= 8   ? 3 :
           count <= 16  ? 4 :
           count <= 32  ? 5 :
           count <= 64  ? 6 :
           count <= 128 ? 7 : 8;
  }

  auto  node_t::insert( const uint8_t* keystr, size_t keylen ) -> node_t*
  {
    auto  expand = this;

    while ( keylen != 0 )
    {
      auto  keytop = expand->get_string_ptr();
      auto  keyend = expand->get_string_len() + keytop;

      // compare matching part of key
      while ( keytop != keyend && keylen != 0 && *keytop == *keystr )
        ++keystr, --keylen, ++keytop;

      // проверить совпадение фрагментов; если фрагмент соответствует ключу полностью,
      // * проверить, полностью ли исчерпан сам ключ; если и ключ сам тоже закончился,
      //   узел найден и можно его вернуть;
      // * иначе пройти по списку вложенных узлов и найти подходящий или точку вставки;
      if ( keytop == keyend )
      {
        if ( keylen != 0 )
        {
          auto  ptrtop = expand->pnodes;
          auto  ptrend = ptrtop + expand->get_tree_count();
          auto  chnext = *keystr;

          // Выбрать вложенный элемент, который либо начинается на нужный символ, либо
          // перед которым надо вставлять новый элемент.
          while ( ptrtop != ptrend && *ptrtop->get_string_ptr() < chnext )
            ++ptrtop;

          // если элемент не найден, создать новый, вставить на нужное место и вернуть
          // последний в цепочке созданных сниз элементов
          if ( ptrtop != ptrend && *ptrtop->get_string_ptr() == chnext )
          {
            expand = ptrtop;
            continue;
          }

          expand = expand->insert( ptrtop, { keystr, keylen } );
        }
        return expand;
      }
        else
        // иначе совпадение частичное, хотя бы один символ; поделить узел на два, до совпадения,
        // c возможным привешенным значением, и после совпадения - со списком вложенных элементов
      {
        auto  ccrest = keytop - expand->get_string_ptr();

        if ( keylen == 0 )
        {
          auto  ulimit = get_tree_limit( 1 );
          auto  uitems = 1 << (1 + ulimit);
          auto  subset = allocate_nodes( uitems );    // остаток существующего ключа

          new ( subset + 0 )
            node_t( keytop, keyend - keytop );
          subset->pnodes = expand->pnodes;
            expand->pnodes = nullptr;
          subset->pvalue = std::move(
            expand->pvalue );
          subset->
            set_tree_limit( expand->get_tree_limit() )
           .set_tree_count( expand->get_tree_count() )
           .set_type_value( expand->get_type_value() );

          if ( expand->get_string_len() > buffer::offset && ccrest <= buffer::offset )
          {
            auto  delptr = expand->keyptr.psz;
              memcpy( expand->keyptr.buf, delptr, ccrest );
            delete [] delptr;
          }
          expand->pnodes = subset;
          expand->
            set_string_len( ccrest )
           .set_tree_count( 1 )
           .set_tree_limit( ulimit );

          return expand;
        }
          else
        {
          auto    ulimit = get_tree_limit( 2 );
          auto    uitems = 1 << ulimit;
          auto    subset = allocate_nodes( uitems );      // список вложенных элементов для этого узла
          node_t* p_rest;
          node_t* p_push;

          if ( *keytop < *keystr )  {  p_rest = subset + 0;  p_push = subset + 1;  }
            else  {  p_rest = subset + 1;  p_push = subset + 0;  }

          new( p_rest )
            node_t( keytop, keyend - keytop );
          p_rest->pnodes = expand->pnodes;
            expand->pnodes = nullptr;
          p_rest->pvalue = std::move(
            expand->pvalue );
          p_rest->
            set_tree_limit( expand->get_tree_limit() )
           .set_type_value( expand->get_type_value() )
           .set_tree_count( expand->get_tree_count() );

          new( p_push )
            node_t( keystr, keylen );

          expand->pnodes = subset;

          if ( expand->get_string_len() > buffer::offset && ccrest <= buffer::offset )
          {
            auto  delptr = expand->keyptr.psz;
            memcpy( expand->keyptr.buf, delptr, ccrest );
            delete [] delptr;
          }

          expand->
            set_string_len( ccrest )
           .set_tree_count( 2 )
           .set_tree_limit( ulimit );

          return p_push;
        }
      }
    }

    return expand;
  }

  auto  node_t::insert( node_t* pos, node_t&& put ) -> node_t*
  {
    auto  count = get_tree_count();
    auto  limit = get_tree_limit();
    auto  nodes = limit == 0 ? 0 : (1U << limit);
    auto  index = pos - pnodes;

    if ( count < nodes )
    {
      for ( auto o = pnodes + count, s = o, e = pnodes + index; s != e; s->~node_t() )
        new( o-- ) node_t( std::move( *--s ) );
    }
      else
    {
      auto  ulimit = get_tree_limit( count + 1 );
      auto  uitems = 1 << ulimit;
      auto  newset = allocate_nodes( uitems );

      if ( pnodes != nullptr )
      {
        auto  output = newset;
        auto  srcptr = pnodes;

        for ( auto e = pnodes + index; srcptr != e; (srcptr++)->~node_t() )
          new( output++ ) node_t( std::move( *srcptr ) );

        ++output;

        for ( auto e = pnodes + count; srcptr != e; (srcptr++)->~node_t() )
          new( output++ ) node_t( std::move( *srcptr ) );

        delete[] (char*)pnodes;
      }

      pnodes = newset;
      set_tree_limit( ulimit );
    }
    set_tree_count( count + 1 );
    return new( pnodes + index ) node_t( std::move( put ) );
  }

  auto  node_t::allocate_nodes( size_t count ) -> node_t*
  {
    return (node_t*)new char[sizeof(node_t) * count];
  }

}
# if 0
int main()
{
  mtc::node_t n( (const uint8_t*)"aaaaaaaaaaaaaaaaaaaaaaaaaaa", 27 );

  n.insert( (const uint8_t*)"aaabbb", 6 );
  n.insert( (const uint8_t*)"bbb", 3 );
  n.insert( (const uint8_t*)"aaaccc", 6 );
  n.insert( (const uint8_t*)"aaabbbccc", 9 );
  return 0;

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
  fwrite( buff.data(), 1, buff.size(), stdout );
  return 0;
  auto  dump = mtc::zmap::dump( buff.data() );

  fprintf( mtc::json::Print( stdout, dump, mtc::json::print::decorated() ),
    "\n" );

  assert( dump == zmap );

  {
    auto  zv = mtc::zval( 1 );
    auto  zb = CreateDump( zv );
    auto  dp = mtc::zval::dump( zb.data() );

    auto  zc = zv.CompTo( zv );
    auto  dc = dp.CompTo( dp );

    assert( zc == dc );
  }

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
    mtc::zmap z2;

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

    for ( int i = 0; i != 1000; ++i )
    {
      union
      {
        uint64_t  u64value;
        uint32_t  u32value[2];
        char      strvalue[8];
      } thekey;

      thekey.u32value[0] = rand();
      thekey.u32value[1] = rand();

      zm.set_word32( mtc::zmap::key( thekey.strvalue, sizeof(thekey.strvalue) ), thekey.u32value[0] );
    }

    mtc::json::Print( stdout, zm, mtc::json::print::decorated() );

    std::vector<char> serial( zm.GetBufLen() );
      zm.Serialize( serial.data() );
      z2.FetchFrom( (const char*)serial.data() );

    assert( zm == z2 );
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

    fprintf( mtc::json::Print( stdout, zm, mtc::json::print::decorated() ), "\n" );
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
      { std::string( "1\x00\x03\x71", 4 ), std::string( "1\x00\x31\x71", 4 ) },
      { "array", mtc::zmap{
        { 3, 4 },
        { 1, 2 },
        { 5, 6 } } } };
    byte_counter  bc;

    zm.Serialize( &bc );

    assert( bc.length == zm.GetBufLen() );

    fprintf( mtc::json::Print( stdout, zm, mtc::json::print::decorated() ), "\n" );
  }

  TestIterators();

  return 0;
}
# endif