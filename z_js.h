/*

The MIT License (MIT)

Copyright (c) 2000-2016 Андрей Коваленко aka Keva
  keva@meta.ua
  keva@rambler.ru
  skype: big_keva
  phone: +7(495)648-4058, +7(916)015-5592

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

=============================================================================

Данная лицензия разрешает лицам, получившим копию данного программного обеспечения
и сопутствующей документации (в дальнейшем именуемыми «Программное Обеспечение»),
безвозмездно использовать Программное Обеспечение без ограничений, включая неограниченное
право на использование, копирование, изменение, слияние, публикацию, распространение,
сублицензирование и/или продажу копий Программного Обеспечения, а также лицам, которым
предоставляется данное Программное Обеспечение, при соблюдении следующих условий:

Указанное выше уведомление об авторском праве и данные условия должны быть включены во
все копии или значимые части данного Программного Обеспечения.

ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ,
ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ТОВАРНОЙ ПРИГОДНОСТИ,
СООТВЕТСТВИЯ ПО ЕГО КОНКРЕТНОМУ НАЗНАЧЕНИЮ И ОТСУТСТВИЯ НАРУШЕНИЙ, НО НЕ ОГРАНИЧИВАЯСЬ
ИМИ.

НИ В КАКОМ СЛУЧАЕ АВТОРЫ ИЛИ ПРАВООБЛАДАТЕЛИ НЕ НЕСУТ ОТВЕТСТВЕННОСТИ ПО КАКИМ-ЛИБО ИСКАМ,
ЗА УЩЕРБ ИЛИ ПО ИНЫМ ТРЕБОВАНИЯМ, В ТОМ ЧИСЛЕ, ПРИ ДЕЙСТВИИ КОНТРАКТА, ДЕЛИКТЕ ИЛИ ИНОЙ
СИТУАЦИИ, ВОЗНИКШИМ ИЗ-ЗА ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ ИЛИ ИНЫХ ДЕЙСТВИЙ
С ПРОГРАММНЫМ ОБЕСПЕЧЕНИЕМ.

*/
# if !defined( __mtc_z_js_h__ )
# define __mtc_z_js_h__
# include "zmap.h"
# include "utf.hpp"
# include <inttypes.h>

namespace mtc {
namespace json {

  namespace print {

    struct compact
    {
      template <class O>  O*  Shift( O* o ) const {  return o;  }
      template <class O>  O*  Space( O* o ) const {  return o;  }
      template <class O>  O*  Break( O* o ) const {  return o;  }
    };

    class decorated
    {
      int   nlevel;

    public:     // construction
      decorated(): nlevel( 0 ) {  }
      decorated( const decorated& js ): nlevel( js.nlevel + 1 ) {  }

    public:     // shifts
      template <class O>  O*  Shift( O* o ) const
        {
          for ( auto n = 0; o != nullptr && n < nlevel; ++n )
            o = ::Serialize( o, "  ", 2 );
          return o;
        }
      template <class O>  O*  Space( O* o ) const
        {
          return ::Serialize( o, ' ' );
        }
      template <class O>  O*  Break( O* o ) const
        {
          return ::Serialize( o, '\n' );
        }
    };

    template <class O>
    O*  charstr( O* o, const char* s, size_t l = (size_t)-1 )
      {
        static char         repsrc[] = "\b\t\n\f\r\"/\\\0";
        static const char*  repval[] = { "\\b", "\\t", "\\n", "\\f", "\\r", "\\\"", "\\/", "\\\\", "\\u0000" };
        char*               reppos;
        const char*         endptr;
        char                chbuff[0x10];
        size_t              nstore;

        if ( s == nullptr )
          return ::Serialize( o, "null", 4 );

        if ( l == (size_t)-1 )
          l = w_strlen( s );

        for ( endptr = s + l, o = ::Serialize( o, '\"' ); s < endptr; )
        {
          unsigned char chnext;

          if ( (reppos = strchr( repsrc, chnext = (unsigned char)*s++ )) != nullptr )
            o = ::Serialize( o, repval[reppos - repsrc], nstore = strlen( repval[reppos - repsrc] ) );
          else if ( chnext < 0x20 )
            o = ::Serialize( o, chbuff, nstore = sprintf( chbuff, "\\u%04x", chnext ) );
          else o = ::Serialize( o, chnext );
        }

        return ::Serialize( o, '\"' );
      }

    template <class O>
    O*  widestr( O* o, const widechar* s, size_t l = (size_t)-1 )
      {
        static char         repsrc[] = "\b\t\n\f\r\"/\\\0";
        static const char*  repval[] = { "\\b", "\\t", "\\n", "\\f", "\\r", "\\\"", "\\/", "\\\\", "\\u0000" };
        char*               reppos;
        auto                endptr = s;
        char                chnext[0x10];

        if ( s == nullptr )
          return ::Serialize( o, "null", 4 );

        if ( l == (size_t)-1 )
          l = w_strlen( s );

        for ( o = ::Serialize( o, '\"' ), endptr = s + l; s != endptr; ++s )
        {
          if ( (*s & ~0x7f) == 0 && (reppos = strchr( repsrc, *s )) != nullptr )
            o = ::Serialize( o, repval[reppos - repsrc], (unsigned)strlen( repval[reppos - repsrc] ) );
          else if ( *s < 0x20 )
            o = ::Serialize( o, chnext, sprintf( chnext, "\\u%04x", *s ) );
          else if ( *s < 0x80 )
            o = ::Serialize( o, (char)*s );
          else
            {
              auto  cnt = utf8::charsize( s, endptr - s );
              auto  nch = utf8::encode( chnext, sizeof(chnext), s, cnt );

              o = ::Serialize( o, chnext, nch );  s += cnt > 1 ? 1 : 0;
            }
        }

        return ::Serialize( o, '\"' );
      }
  }

  template <class O, class D = print::compact>  O*  Print( O*, const zval::dump&, const D& decorate = D() );
  template <class O, class D = print::compact>  O*  Print( O*, const zmap::dump&, const D& decorate = D() );

  template <class O, class D = print::compact>  O*  Print( O* o, const zval& z, const D& decorate = D() )
    {  return Print( o, zval::dump( &z ), decorate );  }
  template <class O, class D = print::compact>  O*  Print( O* o, const zmap& z, const D& decorate = D() )
    {  return Print( o, zmap::dump( &z ), decorate );  }

  template <class O, class D = print::compact>  O*  Print( O* o, const charstr& s, const D& deco = D() )
    {  (void)deco;  return print::charstr( o, s.c_str(), s.length() );  }
  template <class O, class D = print::compact>  O*  Print( O* o, const widestr& s, const D& deco = D() )
    {  (void)deco;  return print::widestr( o, s.c_str(), s.length() );  }

  /*
    Набор примитивов для печати базовых типов zarray<>

    template <class O, class T, class D = print_compact>
    inline O* Print( O* o, T t, const D& deco = D() ) { ... }
  */
  # define  derive_printjson_dec( _type_, _tmpl_ )                                      \
  template <class O, class D = print::compact>                                          \
  inline  O*  Print( O* o, _type_ t, const D& decorate = D() )                          \
    {                                                                                   \
      char  decval[0x40];                                                               \
      return ::Serialize( decorate.Shift( o ), decval, sprintf( decval, _tmpl_, t ) );  \
    }
    derive_printjson_dec( int8_t,   "%d" )
    derive_printjson_dec( uint8_t,  "%u" )
    derive_printjson_dec( int16_t,  "%d" )
    derive_printjson_dec( uint16_t, "%u" )
    derive_printjson_dec( int32_t,  "%d" )
    derive_printjson_dec( uint32_t, "%u" )
  # if defined( _MSC_VER )
    derive_printjson_dec( int64_t,  "%I64d" )
    derive_printjson_dec( uint64_t, "%I64u" )
  # else
    derive_printjson_dec( int64_t,  "%" PRId64 )
    derive_printjson_dec( uint64_t, "%" PRIu64 )
  # endif  // _MSC_VER
  # undef derive_printjson_dec

  # define  derive_printjson_flo( _type_ )                                            \
    template <class O, class D = print::compact>                                      \
    inline  O*  Print( O* o, _type_ t, const D& decorate = D() )                      \
    {                                                                                 \
      char  floval[0x10];                                                             \
      return ::Serialize( decorate.Shift( o ), floval, sprintf( floval, "%f", t ) );  \
    }
    derive_printjson_flo( float )
    derive_printjson_flo( double )
  # undef derive_printjson_flo

  template <class O, class D = print::compact>  O*  Print( O* o, const uuid_t& uuid, const D& deco = D() )
    {
      auto  s = mtc::to_string( uuid );
      return ::Serialize( deco.Shift( o ), s.c_str(), s.length() );
    }

// vectors
  template <class O, class T1, class T2, class D = print::compact>
  O*  Print( O* o, const zval::dump::array_t<T1, T2>& a, const D& decorate = D() )
  {
    auto  ptop = a.begin();
    auto  pend = a.end();

    for ( o = ::Serialize( o, '[' ); o != nullptr && ptop != pend; ++ptop )
    {
      if ( ptop != a.begin() )
        o = ::Serialize( o, ',' );
      o = Print( decorate.Break( o ), *ptop, D( decorate ) );
    }
    return ::Serialize( decorate.Shift( decorate.Break( o ) ), ']' );
  }

  template <class O, class T, class D = print::compact>
  O*  Print( O* o, const std::vector<T>& a, const D& decorate = D() )
    {  return Print( o, zval::dump::array_t<T>( &a ), decorate );  }

  template <class O, class D>
  O*  Print( O* o, const zval::dump& v, const D& decorate )
  {
    switch ( v.get_type() )
    {
      case zval::z_char:    return Print( decorate.Space( o ), *v.get_char() );
      case zval::z_byte:    return Print( decorate.Space( o ), *v.get_byte() );
      case zval::z_int16:   return Print( decorate.Space( o ), *v.get_int16() );
      case zval::z_word16:  return Print( decorate.Space( o ), *v.get_word16() );
      case zval::z_int32:   return Print( decorate.Space( o ), *v.get_int32() );
      case zval::z_word32:  return Print( decorate.Space( o ), *v.get_word32() );
      case zval::z_int64:   return Print( decorate.Space( o ), *v.get_int64() );
      case zval::z_word64:  return Print( decorate.Space( o ), *v.get_word64() );
      case zval::z_float:   return Print( decorate.Space( o ), *v.get_float() );
      case zval::z_double:  return Print( decorate.Space( o ), *v.get_double() );
      case zval::z_uuid:    return Print( decorate.Space( o ), *v.get_uuid() );

      case zval::z_charstr: return Print( decorate.Space( o ), *v.get_charstr() );
      case zval::z_widestr: return Print( decorate.Space( o ), *v.get_widestr() );
      case zval::z_zmap:    return Print( decorate.Break( o ), *v.get_zmap(), D( decorate ) );

      case zval::z_array_char:    return Print( decorate.Space( o ), *v.get_array_char(),   D( decorate ) );
      case zval::z_array_byte:    return Print( decorate.Space( o ), *v.get_array_byte(),   D( decorate ) );
      case zval::z_array_int16:   return Print( decorate.Space( o ), *v.get_array_int16(),  D( decorate ) );
      case zval::z_array_word16:  return Print( decorate.Space( o ), *v.get_array_word16(), D( decorate ) );
      case zval::z_array_int32:   return Print( decorate.Space( o ), *v.get_array_int32(),  D( decorate ) );
      case zval::z_array_word32:  return Print( decorate.Space( o ), *v.get_array_word32(), D( decorate ) );
      case zval::z_array_int64:   return Print( decorate.Space( o ), *v.get_array_int64(),  D( decorate ) );
      case zval::z_array_word64:  return Print( decorate.Space( o ), *v.get_array_word64(), D( decorate ) );
      case zval::z_array_float:   return Print( decorate.Space( o ), *v.get_array_float(),  D( decorate ) );
      case zval::z_array_double:  return Print( decorate.Space( o ), *v.get_array_double(), D( decorate ) );

      case zval::z_array_charstr: return Print( decorate.Space( o ), *v.get_array_charstr(), D( decorate ) );
      case zval::z_array_widestr: return Print( decorate.Space( o ), *v.get_array_widestr(), D( decorate ) );
      case zval::z_array_zval:    return Print( decorate.Space( o ), *v.get_array_zval(), D( decorate ) );
      case zval::z_array_zmap:    return Print( decorate.Space( o ), *v.get_array_zmap(), D( decorate ) );
      case zval::z_array_uuid:    return Print( decorate.Space( o ), *v.get_array_uuid(), D( decorate ) );

      default:  assert( false );  abort();  return o;
    }
  }

  template <class O, class D>
  O*  Print( O* o, const zmap::dump& z, const D& decorate )
  {
    bool  bcomma = false;

    o = decorate.Break( ::Serialize( decorate.Shift( o ), '{' ) );

    for ( auto beg = z.begin(), end = z.end(); beg != end; ++beg )
      if ( beg->second.get_type() != zval::z_untyped )
      {
        D   subdec( decorate );

      // possible comma
        if ( bcomma )
          o = subdec.Break( ::Serialize( o, ',' ) );

      // key
        switch ( beg->first.type() )
        {
          case zmap::key::uint:
            o = ::Serialize( Print( ::Serialize( subdec.Shift( o ), '"' ), (unsigned)beg->first ), '"' );
            break;
          case zmap::key::cstr:
            o = print::charstr( subdec.Shift( o ), (const char*)beg->first );
            break;
          case zmap::key::wstr:
            o = print::widestr( subdec.Shift( o ), (const widechar*)beg->first );
            break;
        }

      // value
        o = Print( ::Serialize( o, ':' ), beg->second, subdec );
        bcomma = true;
      }

    return ::Serialize( decorate.Shift( bcomma ? decorate.Break( o ) : o ), '}' );
  }

  /*
    json parser
  */
  namespace parse
  {
    class error: public std::runtime_error
    {
      using std::runtime_error::runtime_error;

      int         codeLineId = -1;
      int         jsonLineId = -1;
      std::string sourcePath;

    public:
      auto  set_code_lineid( int line ) -> error& {  return codeLineId = line, *this;  }
      auto  set_json_lineid( int line ) -> error& {  return jsonLineId = line, *this;  }
      auto  set_source_path( const std::string& s ) -> error& {  return sourcePath = s, *this;  }

    public:
      int   get_code_lineid() const {  return codeLineId;  }
      int   get_json_lineid() const {  return jsonLineId;  }
      auto  get_source_path() const -> const std::string& {  return sourcePath;  }

    };

    struct stream
    {
      virtual char  get() = 0;
    };

    template <class S>
    class source: public stream
    {
      S*  s;
    public:     // construction
      source( S* on ): s( on ) {}
      operator S* () {  return s;  }

    public:     // overridable
      virtual char  get() override final
        {
          char  ch;

          return s != nullptr && (s = ::FetchFrom( s, ch )) != nullptr ? ch : '\0';
        }
    };

    template <class S>
    auto  make_source( S* on ) -> source<S> {  return source<S>( on );  };

    /*
      json::intl::reader - ориентированный на json вычитыватель символов последовательно, с некоторыми
      удобными доработками для тетрад и пробелов
    */
    class reader
    {
      stream& source;
      char    chbuff[3];
      size_t  buflen;
      int     lineId = 1;

    public:     // construction
      reader( stream& s ): source( s ), chbuff{ 0 }, buflen( 0 )  {}

    public:     // helpers
      char    getnext();
      reader& putback( char );
      bool    isspace( char ) const;
      char    nospace();
      bool    getfour( char* four );
      int     getline() const {  return lineId;  }
    };

    auto  Parse( reader&, byte_t&,   const zval* revive = nullptr ) -> byte_t&;
    auto  Parse( reader&, uint16_t&, const zval* revive = nullptr ) -> uint16_t&;
    auto  Parse( reader&, uint32_t&, const zval* revive = nullptr ) -> uint32_t&;
    auto  Parse( reader&, uint64_t&, const zval* revive = nullptr ) -> uint64_t&;

    auto  Parse( reader&, char_t& , const zval* revive = nullptr ) -> char_t&;
    auto  Parse( reader&, int16_t&, const zval* revive = nullptr ) -> int16_t&;
    auto  Parse( reader&, int32_t&, const zval* revive = nullptr ) -> int32_t&;
    auto  Parse( reader&, int64_t&, const zval* revive = nullptr ) -> int64_t&;

    auto  Parse( reader&, float&  , const zval* revive = nullptr ) -> float&;
    auto  Parse( reader&, double& , const zval* revive = nullptr ) -> double&;

    auto  Parse( reader&, charstr&, const zval* revive = nullptr ) -> charstr&;
    auto  Parse( reader&, widestr&, const zval* revive = nullptr ) -> widestr&;

    auto  Parse( reader&, zval&, const zval* revive = nullptr ) -> zval&;
    auto  Parse( reader&, zmap&, const zmap* revive = nullptr ) -> zmap&;

    auto  Parse( reader&, array_char&,   const zval* revive = nullptr ) -> array_char&;
    auto  Parse( reader&, array_byte&,   const zval* revive = nullptr ) -> array_byte&;
    auto  Parse( reader&, array_int16&,  const zval* revive = nullptr ) -> array_int16&;
    auto  Parse( reader&, array_word16&, const zval* revive = nullptr ) -> array_word16&;
    auto  Parse( reader&, array_int32&,  const zval* revive = nullptr ) -> array_int32&;
    auto  Parse( reader&, array_word32&, const zval* revive = nullptr ) -> array_word32&;
    auto  Parse( reader&, array_int64&,  const zval* revive = nullptr ) -> array_int64&;
    auto  Parse( reader&, array_word64&, const zval* revive = nullptr ) -> array_word64&;
    auto  Parse( reader&, array_float&,  const zval* revive = nullptr ) -> array_float&;
    auto  Parse( reader&, array_double&, const zval* revive = nullptr ) -> array_double&;

    auto  Parse( reader&, array_charstr&, const zval* revive = nullptr ) -> array_charstr&;
    auto  Parse( reader&, array_widestr&, const zval* revive = nullptr ) -> array_widestr&;

    auto  Parse( reader&, array_zmap&, const zval* revive = nullptr ) -> array_zmap&;
    auto  Parse( reader&, array_zval&, const zval* revive = nullptr ) -> array_zval&;

  }

  template <class S, class T>
  S*  Parse( S* s, T& t, const zval& revive = zmap() )
  {
    parse::source<S>  stream( s );
    parse::reader     reader( stream );

    return parse::Parse( reader, t, &revive ), stream;
  }

  template <class S>
  S*  Parse( S* s, zval& x, const zval& revive = zmap() )
  {
    parse::source<S>  stream( s );
    parse::reader     reader( stream );

    return parse::Parse( reader, x, &revive ), stream;
  }

  template <class S>
  S*  Parse( S* s, zmap& z, const zmap& revive = zmap() )
  {
    parse::source<S>  stream( s );
    parse::reader     reader( stream );

    return parse::Parse( reader, z, &revive ), stream;
  }

}}    // json namespace

# endif  // __mtc_z_js_h__
