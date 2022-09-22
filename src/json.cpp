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
# include "../json.h"
# include <type_traits>

namespace mtc {
namespace json {
namespace parse {

  auto  Parse( reader& s, zmap& z, const zval* revive ) -> zmap&;

  // reader implementation

  auto  reader::getnext() -> char
    {
      char  chnext = buflen != 0 ? chbuff[--buflen] : source.get();

      return lineId += (chnext == '\n' ? 1 : 0), chnext;
    }

  auto  reader::putback( char ch ) -> reader&
    {
      if ( buflen > 1 )
        throw error( "stream buffer overflow" ).set_code_lineid( __LINE__ );

      return lineId -= ((chbuff[buflen++] = ch) == '\n' ? 1 : 0), *this;
    }

  bool  reader::isspace( char ch ) const
    {
      return ch != '\0' && (unsigned char)ch <= 0x20;
    }

  auto  reader::nospace() -> char
    {
      for ( ; ; )
      {
        char  getchr;
        char  chnext;

        if ( (getchr = getnext()) == '\0' )
          return getchr;

        if ( isspace( getchr ) )
          continue;

        if ( getchr != '/' )
          return getchr;

        switch ( chnext = this->getnext() )
        {
          case '*':
            {
              for ( getchr = '/'; ; getchr = chnext )
              {
                if ( (chnext = this->getnext()) == '\0')
                  return chnext;
                if ( chnext == '/' && getchr == '*' )
                  break;
              }
              break;
            }
          case '/':
            {
              while ( (getchr = this->getnext()) != '\0' && getchr != '\n' )
                (void)NULL;
              if ( getchr == '\0' ) return getchr;
                else break;
            }

          case '\0':  return getchr;
          default:    putback( chnext );
                      return getchr;
        }
      }
    }

  bool  reader::getfour( char* four )
    {
      return (four[0] = nospace()) != '\0'
          && (four[1] = getnext()) != '\0'
          && (four[2] = getnext()) != '\0'
          && (four[3] = getnext()) != '\0';
    }

  // parse helpers

  struct noheader {  void operator ()( reader& ) const {}  };

  /*
    helper functions for parsing json
  */
  inline  bool  wstrtodbl( double& o, const widechar* s )
  {
    widechar* endptr;
      o = w_strtod( s, &endptr );
    return *endptr == 0;
  }

  inline  bool  wstrtoint( unsigned& o, const widestr& s )
  {
    widechar* endptr;
      o = w_strtol( s.c_str(), &endptr, 10 );
    return *endptr == 0 && endptr > s;
  }

  inline  bool  wstrtostr( charstr& o, widestr& s )
  {
    for ( auto c: s )
      if ( (c & ~0x7f) != 0 )
        return false;

    o.reserve( s.length() );

    for ( auto c: s )
      o.push_back( (char)c );
        
    return true;
  }

  inline  bool  is_num_char( char ch ) {  return ch >= '0' && ch <= '9';  }
  inline  bool  is_int_char( char ch ) {  return is_num_char( ch ) || ch == '-';  }
  inline  bool  is_flo_char( char ch ) {  return is_int_char( ch ) || ch == '.' || ch == 'e' || ch == 'E';  }

  /*
    Parsing:
      - ordinal values;
  */

  // unsigneds
  template <class U>
  auto  uParse( reader& stm, U& u ) -> U&
    {
      char  chnext;

      if ( !is_num_char( chnext = stm.nospace() ) )
      {
        throw error( "0..9 expected" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( stm.getline() );
      }

      for ( u = ((uint8_t)chnext - (uint8_t)'0'); is_num_char( chnext = stm.getnext() ); )
        u = u * 10 + (uint8_t)chnext - '0';

      return stm.putback( chnext ), u;
    }

  // signed integers

  template <class I>
  auto  iParse( reader& stm, I& i ) -> I&
    {
      typename std::make_unsigned<I>::type  uvalue;
      char                                  chnext;
      bool                                  is_neg;

      if ( (is_neg = ((chnext = stm.nospace()) == '-')) == false )
        stm.putback( chnext );

      uParse( stm, uvalue );

      if ( is_neg )
        return (i = 0 - uvalue);
      else
        return (i = uvalue);
    }

  // floats

  template <class F>
  auto  fParse( reader& stm, F& flo ) -> F&
    {
      char  flobuf[0x40];
      char* floptr;
      char* endptr;

      for ( *(floptr = flobuf) = stm.nospace();
        floptr < flobuf + 0x40 && is_flo_char( *floptr ); *++floptr = stm.getnext() )  (void)0;

      flo = (F)strtod( flobuf, &endptr );

      if ( floptr >= std::end(flobuf) )
      {
        throw error( "sequence too long to be float" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( stm.getline() );
      }

      if ( endptr != floptr )
      {
        throw error( "invalid floating point value" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( stm.getline() );
      }

      return stm.putback( *floptr ), flo;
    }

  // strings
  inline
  auto  xFetch( reader& src ) -> widechar
    {
      char      hexchr[5] = "    ";
      widechar  uvalue;
      char*     endptr;

    // convert sequence to utf-16 code
      if ( !src.getfour( hexchr ) )
      {
        throw error( "4-digit hexadecimal character code expected" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( src.getline() );
      }

      uvalue = (widechar)strtoul( hexchr, &endptr, 0x10 );

      if ( endptr - hexchr != 4 )
      {
        throw error( "4-digit hexadecimal character code expected" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( src.getline() );
      }

      return uvalue;
    }

 /*
  * wFetch( src, zval&[, hdr] )
  * Дозагружает строку до конца, полагая, что она уже widechar-строка, но не полагаясь на кодировку строки.
  */
  template <class header = noheader>
  auto  wFetch( reader& src, zval& val, const header& hdr = header() ) -> zval&
    {
      auto  wc_str = val.get_widestr();  assert( wc_str != nullptr );
      char  chnext;
      char  chprev;

      hdr( src );

      for ( chprev = '\0'; (chnext = src.getnext()) != '\0'; chprev = chnext )
      {
      // check for end of identifier
        if ( chnext == '\"' && chprev != '\\' )
          return val;

      // check for '\\'
        if ( chnext == '\\' && chprev != '\\' )
          continue;

      // check regular char
        if ( chprev != '\\' )
        {
          wc_str->push_back( (widechar)(unsigned char)chnext );
          continue;
        }

        switch ( chnext )
        {
          case 'u':   wc_str->push_back( xFetch( src ) );  break;
          case 'b':   wc_str->push_back( '\b' ); break;
          case 't':   wc_str->push_back( '\t' ); break;
          case 'n':   wc_str->push_back( '\n' ); break;
          case 'f':   wc_str->push_back( '\f' ); break;
          case 'r':   wc_str->push_back( '\r' ); break;
          case '\"':  wc_str->push_back( '\"' ); break;
          case '/':   wc_str->push_back( '/'  ); break;
          case '\\':  wc_str->push_back( '\\' ); 
                      chnext = '\0';            break;
          default:    throw error( "invalid escape sequence" )
                        .set_code_lineid( __LINE__ )
                        .set_json_lineid( src.getline() );
        }
        chprev = '\0';
      }
      throw error( "unexpected end of stream" )
        .set_code_lineid( __LINE__ )
        .set_json_lineid( src.getline() );
    }
    
 /*
  * sFetch( src, zval& )
  * Загружает символьную строку до момента, когда встретится символ unicode \u или конец строки;
  * При встрече такого символа декодирует его, преобразует всю строку в utf-16 расширением символов,
  * после чего продолжает загрузку из потока.
  * Результатом является либо zval{charstr}, либо zval{widestr}, без каких-либо намёков на исходную
  * кодировку строки.
  */
  template <class header = noheader>
  auto  sFetch( reader& src, zval& val, const header& hdr = header() ) -> zval&
    {
      auto  mb_str = val.set_charstr();  assert( mb_str != nullptr );
      char  chnext;
      char  chprev;

      hdr( src );

      for ( chprev = '\0'; (chnext = src.getnext()) != '\0'; chprev = chnext )
      {
      // check for end of identifier
        if ( chnext == '\"' && chprev != '\\' )
          return val;

      // check for '\\'
        if ( chnext == '\\' && chprev != '\\' )
          continue;

      // check regular char
        if ( chprev != '\\' )
        {
          mb_str->push_back( chnext );
          continue;
        }

      // check long code: 4 symbols
        if ( chnext == 'u' )
        {
          auto  uvalue = xFetch( src );

        // check if encoded character has lower code; continue in this case
          if ( (uvalue & ~0x00ff) == 0 )
          {
            mb_str->push_back( (char)uvalue );
            continue;
          }
            else
        // transform string to pseudo-unicode sequence by expanding previous characters;
        // finish loading as widestr
          {
            auto  wc_str = utf16::expand( *mb_str );
            
            wc_str.push_back( uvalue );
            return val.set_widestr( std::move( wc_str ) ), wFetch( src, val );
          }
        }
          else
        switch ( chnext )
        {
          case 'b':   mb_str->push_back( '\b' ); break;
          case 't':   mb_str->push_back( '\t' ); break;
          case 'n':   mb_str->push_back( '\n' ); break;
          case 'f':   mb_str->push_back( '\f' ); break;
          case 'r':   mb_str->push_back( '\r' ); break;
          case '\"':  mb_str->push_back( '\"' ); break;
          case '/':   mb_str->push_back( '/'  ); break;
          case '\\':  mb_str->push_back( '\\' ); 
                      chnext = '\0';            break;
          default:    throw error( "invalid escape sequence" )
            .set_code_lineid( __LINE__ )
            .set_json_lineid( src.getline() );
        }
        chprev = '\0';
      }
      throw error( "unexpected end of stream" )
        .set_code_lineid( __LINE__ )
        .set_json_lineid( src.getline() );
    }

  auto  sParse( reader& src, zval& val ) -> zval&
    {
      return val.set_charstr(), sFetch( src, val, []( reader& src )
        {
          if ( src.nospace() != '"' )
          {
            throw error( "'\"' expected" )
              .set_code_lineid( __LINE__ )
              .set_json_lineid( src.getline() );
          }
        } );
    }

  auto  wParse( reader& src, zval& val ) -> zval&
    {
      return val.set_widestr(), wFetch( src, val, []( reader& src )
        {
          if ( src.nospace() != '"' )
          {
            throw error( "'\"' expected" )
              .set_code_lineid( __LINE__ )
              .set_json_lineid( src.getline() );
          }
        } );
    }

 /*
  * zsLoad( ... )
  * Загружает строку и при необходимости конвертирует её в widestr, если она в utf8.
  * Иначе оставляет её символьной строкой
  */
  auto  zsLoad( reader& src, zval& val ) -> zval&
    {
      auto  is_simple_codepage = []( const widestr& s ) -> bool
        {
          for ( auto& c: s )
            if ( c > 0xff ) return false;
          return true;
        };

      sParse( src, val );

      if ( val.get_charstr() != nullptr )
      {
        if ( utf8::detect( *val.get_charstr() ) )
          val.set_widestr( std::move( utf16::encode( *val.get_charstr() ) ) );
      }
        else
      if ( val.get_widestr() != nullptr )
      {
        if ( utf8::detect( *val.get_widestr() ) )
        {
          val.get_widestr()->resize( utf::encode(
            utf16::out( (widechar*)val.get_widestr()->c_str(), val.get_widestr()->length() ),
            utf16::in( val.get_widestr()->c_str(), val.get_widestr()->length() ) ) );
        }
          else
        if ( is_simple_codepage( *val.get_widestr() ) )
        {
          auto  newstr = charstr();

          for ( auto& c: *val.get_widestr() )
            newstr.push_back( (char)(unsigned char)c );

          val.set_charstr( std::move( newstr ) );
        }
      }
      return val;
    }

  auto  Parse( reader& s, byte_t&   u, const zval* ) -> byte_t&   {  return uParse( s, u );  }
  auto  Parse( reader& s, uint16_t& u, const zval* ) -> uint16_t& {  return uParse( s, u );  }
  auto  Parse( reader& s, uint32_t& u, const zval* ) -> uint32_t& {  return uParse( s, u );  }
  auto  Parse( reader& s, uint64_t& u, const zval* ) -> uint64_t& {  return uParse( s, u );  }

  auto  Parse( reader& s, char_t&   i, const zval* ) -> char_t&   {  return iParse( s, i );  }
  auto  Parse( reader& s, int8_t&   i, const zval* ) -> int8_t&   {  return iParse( s, i );  }
  auto  Parse( reader& s, int16_t&  i, const zval* ) -> int16_t&  {  return iParse( s, i );  }
  auto  Parse( reader& s, int32_t&  i, const zval* ) -> int32_t&  {  return iParse( s, i );  }
  auto  Parse( reader& s, int64_t&  i, const zval* ) -> int64_t&  {  return iParse( s, i );  }

  auto  Parse( reader& s, float&    f, const zval* ) -> float&    {  return fParse( s, f );  }
  auto  Parse( reader& s, double&   f, const zval* ) -> double&   {  return fParse( s, f );  }

  auto  Parse( reader& s, charstr& r, const zval* ) -> charstr&
    {
      zval  z;

      if ( sParse( s, z ).get_charstr() == nullptr )
      {
        throw error( "string cannot be parsed as simple character string" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( s.getline() );
      }

      return r = std::move( *z.get_charstr() );
    }

  auto  Parse( reader& s, widestr& r, const zval* ) -> widestr&
    {
      zval      zv;
      charstr*  ps;
      widestr*  pw;

      if ( (ps = sParse( s, zv ).get_charstr()) == nullptr && (pw = zv.get_widestr()) == nullptr )
      {
        throw error( "string cannot be parsed (not a string)" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( s.getline() );
      }

    // if a value is character string, check if it is an utf8 string; convert utf8
    // to widechar string directly
      if ( ps != nullptr )
      {
        return utf8::detect( *ps )
          ? r = std::move( utf16::encode( *ps ) )
          : r = std::move( utf16::expand( *ps ) );
      }
      if ( pw != nullptr )
      {
        return utf8::detect( *pw )
          ? r = std::move( utf::encode( utf16::out(), utf8::in( *pw ) ) )
          : r = std::move( *pw );
      }
      throw std::logic_error( "must not get control" );
    }

  template <class V>
  auto  Parse( reader& stm, std::vector<V>& out, const zval* revive = nullptr ) -> std::vector<V>&
    {
      char  chnext;

      if ( (chnext = stm.nospace()) != '[' )
      {
        throw error( "'[' expected" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( stm.getline() );
      }

      while ( (chnext = stm.nospace()) != '\0' && chnext != ']' )
        {
          V avalue;

          out.push_back( std::move( Parse( stm.putback( chnext ), avalue, revive ) ) );

          if ( (chnext = stm.nospace()) == ',' )
            continue;
          if ( chnext == ']' )  stm.putback( chnext );
            else
          {
            throw error( "',' or ']' expected" )
              .set_code_lineid( __LINE__ )
              .set_json_lineid( stm.getline() );
          }
        }

      if ( chnext != ']' )
      {
        throw error( "']' expected" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( stm.getline() );
      }

      return out;
    }

  static zmap type_set(
  {
    { "char",     (uint32_t)zval::z_char },
    { "int8",     (uint32_t)zval::z_char },
    { "byte",     (uint32_t)zval::z_byte },
    { "uint8",    (uint32_t)zval::z_byte },
    { "int16",    (uint32_t)zval::z_int16 },
    { "int32",    (uint32_t)zval::z_int32 },
    { "int64",    (uint32_t)zval::z_int64 },
    { "word16",   (uint32_t)zval::z_word16 },
    { "word32",   (uint32_t)zval::z_word32 },
    { "word64",   (uint32_t)zval::z_word64 },
    { "float",    (uint32_t)zval::z_float },
    { "double",   (uint32_t)zval::z_double },

    { "charstr",  (uint32_t)zval::z_charstr },
    { "widestr",  (uint32_t)zval::z_widestr },

    { "array_char"  ,   (uint32_t)zval::z_array_char    },
    { "array_byte"  ,   (uint32_t)zval::z_array_byte    },
    { "array_int16" ,   (uint32_t)zval::z_array_int16   },
    { "array_word16",   (uint32_t)zval::z_array_word16  },
    { "array_int32" ,   (uint32_t)zval::z_array_int32   },
    { "array_word32",   (uint32_t)zval::z_array_word32  },
    { "array_int64" ,   (uint32_t)zval::z_array_int64   },
    { "array_word64",   (uint32_t)zval::z_array_word64  },
    { "array_float" ,   (uint32_t)zval::z_array_float   },
    { "array_double",   (uint32_t)zval::z_array_double  },

    { "array_charstr",  (uint32_t)zval::z_array_charstr },
    { "array_widestr",  (uint32_t)zval::z_array_widestr }
  } );

  auto  map_type( const std::string& s_type ) -> unsigned
    {
      auto  pval = type_set.get_word32( s_type );

      return pval != nullptr ? *pval : (unsigned)zval::z_untyped;
    }

  auto  Parse( reader& s, zmap& z, const zval* revive ) -> zmap&
    {
      return Parse( s, z, revive != nullptr ? revive->get_zmap() : nullptr );
    }

  auto  Parse( reader& s, zval& z, const zval* revive ) -> zval&
    {
      char        chnext;
      unsigned    v_type = zval::z_untyped;
      const zmap* z_data = nullptr;

      if ( revive != nullptr )
      {
        switch ( revive->get_type() )
        {
          case zval::z_char:        v_type = *revive->get_char();  break;
          case zval::z_byte:        v_type = *revive->get_byte();  break;
          case zval::z_int16:       v_type = *revive->get_int16();  break;
          case zval::z_word16:      v_type = *revive->get_word16();  break;
          case zval::z_int32:       v_type = *revive->get_int32();  break;
          case zval::z_word32:      v_type = *revive->get_word32();  break;
          case zval::z_charstr:     v_type = map_type( *revive->get_charstr() );  break;
          case zval::z_zmap:
          case zval::z_array_zmap:  z_data = revive->get_zmap();  break;
        }
      }

      switch ( v_type )
      {
        case zval::z_char:    return Parse( s, *z.set_char() ), z;
        case zval::z_int16:   return Parse( s, *z.set_int16()  ), z;
        case zval::z_int32:   return Parse( s, *z.set_int32()  ), z;
        case zval::z_int64:   return Parse( s, *z.set_int64()  ), z;
        case zval::z_byte:    return Parse( s, *z.set_byte()   ), z;
        case zval::z_word16:  return Parse( s, *z.set_word16() ), z;
        case zval::z_word32:  return Parse( s, *z.set_word32() ), z;
        case zval::z_word64:  return Parse( s, *z.set_word64() ), z;
        case zval::z_float:   return Parse( s, *z.set_float()  ), z;
        case zval::z_double:  return Parse( s, *z.set_double() ), z;
        case zval::z_charstr: return Parse( s, *z.set_charstr() ), z;
        case zval::z_widestr: return Parse( s, *z.set_widestr() ), z;
        case zval::z_zmap:    return Parse( s, *z.set_zmap(), z_data ), z;

        case zval::z_array_char:    return Parse( s, *z.set_array_char() ), z;
        case zval::z_array_byte:    return Parse( s, *z.set_array_byte() ), z;
        case zval::z_array_int16:   return Parse( s, *z.set_array_int16() ), z;
        case zval::z_array_int32:   return Parse( s, *z.set_array_int32() ), z;
        case zval::z_array_int64:   return Parse( s, *z.set_array_int64() ), z;
        case zval::z_array_word16:  return Parse( s, *z.set_array_word16() ), z;
        case zval::z_array_word32:  return Parse( s, *z.set_array_word32() ), z;
        case zval::z_array_word64:  return Parse( s, *z.set_array_word64() ), z;
        case zval::z_array_float:   return Parse( s, *z.set_array_float() ), z;
        case zval::z_array_double:  return Parse( s, *z.set_array_double() ), z;
        case zval::z_array_charstr: return Parse( s, *z.set_array_charstr() ), z;
        case zval::z_array_widestr: return Parse( s, *z.set_array_widestr() ), z;
        default:  break;
      }

    // untyped value load: object zmap{...}, untyped array [...], string "...", integer -?[0-9]+ or float -?[0-9]+\.?[0-9]*([Ee]-?[0-9]+)?
      switch ( chnext = s.nospace() )
      {
        case '{':
          return Parse( s.putback( chnext ), *z.set_zmap(), z_data ), z;

        case '[':
          {
            auto        arrval = z.set_array_zval();
            zval        newval;
            zval        _zvrev;
            const zval* pzvrev = nullptr;

            if ( z_data != nullptr )
              {
                _zvrev.set_zmap( *z_data );  pzvrev = &_zvrev;
              }

            Parse( s.putback( chnext ), *arrval, pzvrev );

          // check if std::vector<zval> is convertible to simple types; if is, try convert to simple types
            if ( arrval->empty() )
              return z;

            for ( auto prev = arrval->begin(), next = prev + 1; next != arrval->end(); prev = next++ )
              if ( next->get_type() != prev->get_type() )
                return z;

            switch ( arrval->front().get_type() )
            {
            # define  derive_transform( _type_ )                                \
              case zval::z_##_type_:                                            \
                {                                                               \
                  mtc::array_##_type_&  newarr = *newval.set_array_##_type_();  \
                                                                                \
                  for ( auto it: *arrval )                                      \
                    newarr.push_back( std::move( *it.get_##_type_() ) );        \
                  break;                                                        \
                }
              derive_transform( char )
              derive_transform( byte )
              derive_transform( int16 )
              derive_transform( int32 )
              derive_transform( int64 )
              derive_transform( float )
              derive_transform( word16 )
              derive_transform( word32 )
              derive_transform( word64 )
              derive_transform( double )
              derive_transform( charstr )
              derive_transform( widestr )
              derive_transform( zmap )
            # undef  derive_transform
              default:  return z;
            }
            z = std::move( newval );
          }
          return z;

        case '\"':
          return zsLoad( s.putback( chnext ), z );

        default:  break;
      }
      if ( is_int_char( chnext ) )
      {
        charstr cvalue( 1, chnext );
        bool    bpoint;
        bool    bexpon;
        char    chprev;

        for ( chprev = '\0', bexpon = bpoint = false; ; )
        {
          if ( is_num_char( chnext = s.getnext() ) )
          {
            cvalue.push_back( chprev = chnext );
          }
            else
          if ( chnext == '.' )
          {
            if ( bpoint || bexpon )
            {
              throw error( "unexpected '.' in numeric format" )
                .set_code_lineid( __LINE__ )
                .set_json_lineid( s.getline() );
            }
            cvalue.push_back( chprev = chnext );
              bpoint = true;
          }
            else
          if ( chnext == 'e' || chnext == 'E' )
          {
            if ( bexpon )
            {
              throw error( "unexpected 'e' in numeric format" )
                .set_code_lineid( __LINE__ )
                .set_json_lineid( s.getline() );
            }
            cvalue.push_back( chprev = chnext );
              bexpon = true;
          }
            else
          if ( chnext == '-' )
          {
            if ( chprev != 'e' && chprev != 'E' )
            {
              throw error( "unexpected '-' in numeric format" )
                .set_code_lineid( __LINE__ )
                .set_json_lineid( s.getline() );
            }
            cvalue.push_back( chprev = chnext );
          }
            else
          {
            s.putback( chnext );
            break;
          }
        }
        if ( bpoint || bexpon )
        {
          char*   endptr;
          double  dvalue = strtod( cvalue.c_str(), &endptr );

          if ( *endptr != '\0' )
          {
            throw error( "invalid numeric format" )
              .set_code_lineid( __LINE__ )
              .set_json_lineid( s.getline() );
          }
          z.set_double( dvalue );
        }
          else
        {
          char*   endptr;
          int32_t nvalue = strtol( cvalue.c_str(), &endptr, 10 );

          if ( *endptr != '\0' )
          {
            throw error( "invalid numeric format" )
              .set_code_lineid( __LINE__ )
              .set_json_lineid( s.getline() );
          }
          z.set_int32( nvalue );
        }
        return z;
      }

    // check for boolean represented as byte
      if ( chnext == 't' )
      {
        if ( (chnext = s.getnext()) != 'r'
          || (chnext = s.getnext()) != 'u'
          || (chnext = s.getnext()) != 'e' )
        {
          throw error( strprintf( "unexpected character '%c'", chnext ) )
            .set_code_lineid( __LINE__ )
            .set_json_lineid( s.getline() );
        }
        return z.set_byte( 1 ), z;
      }
        else
      if ( chnext == 'f' )
      {
        if ( (chnext = s.getnext()) != 'a'
          || (chnext = s.getnext()) != 'l'
          || (chnext = s.getnext()) != 's'
          || (chnext = s.getnext()) != 'e' )
        {
          throw error( strprintf( "unexpected character '%c'", chnext ) )
            .set_code_lineid( __LINE__ )
            .set_json_lineid( s.getline() );
        }
        return z.set_byte( 0 ), z;
      }
        else
      if ( chnext == 'n' )
      {
        if ( (chnext = s.getnext()) != 'u'
          || (chnext = s.getnext()) != 'l'
          || (chnext = s.getnext()) != 'l' )
        {
          throw error( strprintf( "unexpected character '%c'", chnext ) )
            .set_code_lineid( __LINE__ )
            .set_json_lineid( s.getline() );
        }
        return z.set_charstr(), z;
      }
        else
      {
        throw error( strprintf( "unexpected character '%c'", chnext ) )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( s.getline() );
      }
    }

  auto  Parse( reader& s, zmap& z, const zmap* revive ) -> zmap&
  {
    char  chnext;

    z.clear();

  // open object
    if ( (chnext = s.nospace()) == '\0' )
      return z;

    if ( chnext != '{' )
    {
      throw error( "'{' expected" )
        .set_code_lineid( __LINE__ )
        .set_json_lineid( s.getline() );
    }

  // char by char until end or '}'
    while ( (chnext = s.nospace()) != '\0' && chnext != '}' )
    {
      zval        zv_key;
      zval*       newval = nullptr;
      const zval* revval = nullptr;

    // get variable name as widestring
      try
      {  Parse( s.putback( chnext ), zv_key );  }
      catch ( const error& jx )
      {
        throw error( strprintf( "%s while parsing variable name", jx.what() ) )
          .set_code_lineid( jx.get_code_lineid() )
          .set_json_lineid( jx.get_json_lineid() );
      }

      if ( zv_key.get_charstr() != nullptr )
      {
        newval = z.put( *zv_key.get_charstr() );
        revval = revive != nullptr ? revive->get( *zv_key.get_charstr() ) : nullptr;
      }
        else
      if ( zv_key.get_widestr() != nullptr )
      {
        newval = z.put( *zv_key.get_widestr() );
        revval = revive != nullptr ? revive->get( *zv_key.get_widestr() ) : nullptr;
      }
        else
      if ( zv_key.get_int32() != nullptr )
      {
        newval = z.put( *zv_key.get_int32() );
        revval = revive != nullptr ? revive->get( *zv_key.get_int32() ) : nullptr;
      }
        else
      {
        throw error( "invalid key type" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( s.getline() );
      }

    // check for colon
      if ( (chnext = s.nospace()) != ':' )
      {
        throw error( "':' expected" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( s.getline() );
      }

    // get the value
      try
      {  Parse( s, *newval, revval );  }
      catch ( const error& jx )
      {
        throw error( strprintf( "%s -> %s", zv_key.to_string().c_str(), jx.what() ) )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( jx.get_json_lineid() );
      }

    // check for comma
      if ( (chnext = s.nospace()) == ',' )
        continue;
      if ( chnext == '}' )  s.putback( chnext );
        else
      {
        throw error( "'}' or ',' expected" )
          .set_code_lineid( __LINE__ )
          .set_json_lineid( s.getline() );
      }
    }

  // check valid script
    if ( chnext != '}' )
    {
      throw error( "'}' expected" )
        .set_code_lineid( __LINE__ )
        .set_json_lineid( s.getline() );
    }

    return z;
  }

  auto  Parse( reader& src, array_char& out, const zval* revive ) -> array_char&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_byte& out, const zval* revive ) -> array_byte&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_int16& out, const zval* revive ) -> array_int16&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_word16& out, const zval* revive ) -> array_word16&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_int32& out,  const zval* revive ) -> array_int32&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_word32& out, const zval* revive ) -> array_word32&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_int64& out,  const zval* revive ) -> array_int64&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_word64& out, const zval* revive ) -> array_word64&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_float& out,  const zval* revive ) -> array_float&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_double& out, const zval* revive ) -> array_double&
    {  return Parse<>( src, out, revive );  }

  auto  Parse( reader& src, array_charstr& out, const zval* revive ) -> array_charstr&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_widestr& out, const zval* revive ) -> array_widestr&
    {  return Parse<>( src, out, revive );  }

  auto  Parse( reader& src, array_zmap& out, const zval* revive ) -> array_zmap&
    {  return Parse<>( src, out, revive );  }
  auto  Parse( reader& src, array_zval& out, const zval* revive ) -> array_zval&
    {  return Parse<>( src, out, revive );  }

}}}
