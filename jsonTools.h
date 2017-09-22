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
# if !defined( __jsonTools_h__ )
# define __jsonTools_h__
# include "charstream.h"
# include "zarray.h"
# include <string.h>
# include <stdexcept>

namespace mtc
{
  struct ParseJsonError: public std::runtime_error
    {
      ParseJsonError( const char* msg ): std::runtime_error( msg )  {}
    };

  struct json_use_exceptions_t
    {
      void* operator ()( const char* msg, ... ) const
        {
          _auto_<char>  msgstr;
          va_list       vaargs;

          va_start( vaargs, msg );
            msgstr = vstrduprintf( msg, vaargs );
          va_end  ( vaargs );

          throw ParseJsonError( msgstr );
            return nullptr;
        }
    };
  struct json_no_exceptions_t
    {
      void* operator ()( const char*, ... ) const
        {
          return nullptr;
        }
    };

  constexpr json_use_exceptions_t json_use_exceptions{};
  constexpr json_no_exceptions_t  json_no_exceptions{};

  /*
    revive - декларации типов для вычитывания, подсказки алгоритму для строгой типизации;
    оно работает и без них, но использует тогда ограниченный набор типов
  */
  struct jsonRevive
  {
    unsigned    k_type;
    unsigned    v_type;
    jsonRevive* listed;
    jsonRevive* nested;

  protected:
    unsigned    cchkey;
    char        thekey[1];

  public:     // construction
    jsonRevive( unsigned    kt, unsigned    vt,
                const void* pk, unsigned    lk,
                jsonRevive* pn, jsonRevive* ps ): k_type( kt ), v_type( vt ), listed( pn ), nested( ps ), cchkey( lk )
    {
      if ( pk != nullptr )
        memcpy( thekey, pk, lk );
    }

  public:     // closed destructor
   ~jsonRevive()
    {
      if ( listed != nullptr )
        delete listed;
      if ( nested != nullptr )
        delete nested;
    }

  public:     // creation
    template <class M = def_alloc>
    static  jsonRevive* Create( unsigned k, unsigned v, const void* p, unsigned l, jsonRevive* n, jsonRevive* s )
      {
        jsonRevive* palloc;

        if ( (palloc = (jsonRevive*)M().alloc( sizeof(*palloc) + l - sizeof(palloc->thekey) )) != nullptr )
          new( palloc ) jsonRevive( k, v, p, l, n, s );
        return palloc;
      }
    const  jsonRevive*  Search( unsigned  k ) const
      {
        byte_t  thekey[4];                                                                                \
        return search( thekey, zarray_int_to_key( thekey, k ), z_word32 );
      }
    const  jsonRevive*  Search( const char* k ) const
      {  return search( k, (unsigned)w_strlen( k ), z_charstr );  }
    const  jsonRevive*  Search( const widechar* k ) const
      {  return search( k, (unsigned)(sizeof(widechar) * w_strlen( k )), z_widestr );  }

  protected:  // search helper
    const   jsonRevive* search( const void* k, unsigned l, unsigned t ) const
      {
        return cchkey == l && memcmp( thekey, k, l ) == 0 && k_type == t ? this
          : listed != nullptr ? listed->search( k, l, t ) : nullptr;
      }

  };

# define  derive_revive( _type_ )                                                                                           \
  template <class M = def_alloc>  jsonRevive* add_##_type_( unsigned thekey, jsonRevive* fsnext = nullptr )                 \
  {                                                                                                                         \
    byte_t  strkey[4];                                                                                                      \
    return jsonRevive::Create<M>( z_word32, z_##_type_, strkey, zarray_int_to_key( strkey, thekey ), fsnext, nullptr );     \
  }                                                                                                                         \
  template <class M = def_alloc>  jsonRevive* add_##_type_( const char* thekey, jsonRevive* fsnext = nullptr )              \
  {                                                                                                                         \
    return jsonRevive::Create<M>( z_charstr, z_##_type_, thekey, (unsigned)w_strlen( thekey ), fsnext, nullptr );           \
  }                                                                                                                         \
  template <class M = def_alloc>  jsonRevive* add_##_type_( const widechar* thekey, jsonRevive* fsnext = nullptr )          \
  {                                                                                                                         \
    return jsonRevive::Create<M>( z_widestr, z_##_type_, thekey, (unsigned)(sizeof(widechar) * w_strlen( thekey )), fsnext, nullptr );  \
  }
  derive_revive( char )
  derive_revive( byte )
  derive_revive( int16 )
  derive_revive( word16 )
  derive_revive( int32 )
  derive_revive( word32 )
  derive_revive( int64 )
  derive_revive( word64 )
  derive_revive( float )
  derive_revive( double )
  derive_revive( charstr )
  derive_revive( widestr )
  derive_revive( buffer )
  derive_revive( array_char )
  derive_revive( array_byte )
  derive_revive( array_int16 )
  derive_revive( array_word16 )
  derive_revive( array_int32 )
  derive_revive( array_word32 )
  derive_revive( array_int64 )
  derive_revive( array_word64 )
  derive_revive( array_float )
  derive_revive( array_double )
  derive_revive( array_charstr )
  derive_revive( array_widestr )
  derive_revive( array_buffer )
# undef derive_revive

# define  derive_revive( _type_ )                                                                                                     \
  template <class M = def_alloc>  jsonRevive* add_##_type_( unsigned  thekey, jsonRevive* nested, jsonRevive* fsnext = nullptr )      \
  {                                                                                                                                   \
    byte_t  strkey[4];                                                                                                                \
    return jsonRevive::Create<M>( z_word32, z_##_type_, strkey, zarray_int_to_key( strkey, thekey ), fsnext, nested );                \
  }                                                                                                                                   \
  template <class M = def_alloc>  jsonRevive*  add_##_type_( const char* szname, jsonRevive* nested, jsonRevive* fsnext = nullptr )   \
  {                                                                                                                                   \
    return jsonRevive::Create<M>( z_charstr, z_##_type_, szname, (unsigned)w_strlen( szname ), fsnext, nested );                      \
  }                                                                                                                                   \
  template <class M = def_alloc>  jsonRevive*  add_##_type_( const widechar* szname, void* nested, void* fsnext = nullptr )           \
  {                                                                                                                                   \
    return jsonRevive::Create<M>( z_widestr, z_##_type_, szname, (unsigned)(sizeof(widechar) * w_strlen( szname )), fsnext, nested ); \
  } 
  derive_revive( zarray )
  derive_revive( array_zarray )
  
# undef derive_revive

  /*
    charstream - класс для вычитывания данных из потока с промежуточной буферизацией,
    позволяющий делать getnext() и putback(). Глубина стека тождественно равна 2 :)
  */
  template <class S>
  struct jsonstream: public charstream<S>
  {
    jsonstream( S* s ): charstream<S>( s )  {}
    jsonstream& operator = ( S* s ) {  return (jsonstream&)charstream<S>::operator = ( s );  }

  public:     // override
    jsonstream& putback( char c ) {  return (jsonstream&)charstream<S>::putback( c );  }

  public:     // gets
    /*
      get first non-space character
    */
    char  nospace()
      {
        for ( ; ; )
        {
          char  getchr;
          char  chnext;

          if ( (getchr = this->getnext()) == '\0' )
            return getchr;

          if ( (unsigned char)getchr <= 0x20 )
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
            default:    this->putback( chnext );
                        return getchr;
          }
        }
      }
    bool  getfour( char* p )
      {
        return (*p++ = nospace()) != '\0' && (*p++ = this->getnext()) != '\0'
            && (*p++ = this->getnext()) != '\0' && (*p++ = this->getnext()) != '\0';
      }
    template <class C, class M, class X = json_use_exceptions_t>
    int   getstring( _auto_<C, M>&  refstr, const X& except = X() )
      {
        char  chnext;
        char  chprev;
        int   cchstr = 0;
        int   climit = 0;

        if ( (chnext = nospace()) != '\"' )
        {
          except( "'\"' expected" );
          return EINVAL;
        }

        for ( chprev = '\0'; (chnext = this->getnext()) != '\0'; chprev = chnext )
        {
        // check for end of identifier
          if ( chnext == '\"' && chprev != '\\' )
            return refstr.ptr() == nullptr ? append( refstr, (C)'\0', cchstr, climit ) : 0;

        // check for '\\'
          if ( chnext == '\\' && chprev != '\\' )
            continue;

        // check regular char
          if ( chprev != '\\' )
          {
            if ( append( refstr, (C)(byte_t)chnext, cchstr, climit ) != 0 ) return ENOMEM;
              else continue;
          }

        // check long code: 4 symbols
          if ( chnext == 'u' )
          {
            char      hexchr[5] = "    ";
            widechar  uvalue;
            char*     endptr;

            if ( getfour( hexchr ) )
            {
              uvalue = (widechar)strtoul( hexchr, &endptr, 0x10 );
            }
              else
            {
              except( "4-digit hexadecimal character code expected" );
              return EINVAL;
            }
            if ( endptr - hexchr != 4 )
            {
              except( "4-digit hexadecimal character code expected" );
              return EINVAL;
            }
              
            if ( sizeof(C) == sizeof(char) && uvalue > 127 )
            {
              if ( (uvalue & ~0x07ff) == 0 )
              {
                if ( append( refstr, (C)(0xC0 | (byte_t)(uvalue >> 6)),   cchstr, climit ) != 0
                  || append( refstr, (C)(0x80 | (byte_t)(uvalue & 0x3f)), cchstr, climit ) != 0 )
                return ENOMEM;
              }
                else
              {
                if ( append( refstr, (C)(0xE0 | (byte_t)(uvalue >> 12)),          cchstr, climit ) != 0
                  || append( refstr, (C)(0x80 | (byte_t)((uvalue >> 6) & 0x3F)),  cchstr, climit ) != 0
                  || append( refstr, (C)(0x80 | (byte_t)(uvalue & 0x3f)),         cchstr, climit ) != 0 )
                return ENOMEM;
              }
            }
              else
            if ( append( refstr, (C)uvalue, cchstr, climit ) != 0 )
              return ENOMEM;
          }
            else
          switch ( chnext )
          {
            case 'b':   if ( append( refstr, (C)'\b', cchstr, climit ) == 0 )  break;  else return ENOMEM;
            case 't':   if ( append( refstr, (C)'\t', cchstr, climit ) == 0 )  break;  else return ENOMEM;
            case 'n':   if ( append( refstr, (C)'\n', cchstr, climit ) == 0 )  break;  else return ENOMEM;
            case 'f':   if ( append( refstr, (C)'\f', cchstr, climit ) == 0 )  break;  else return ENOMEM;
            case 'r':   if ( append( refstr, (C)'\r', cchstr, climit ) == 0 )  break;  else return ENOMEM;
            case '\"':  if ( append( refstr, (C)'\"', cchstr, climit ) == 0 )  break;  else return ENOMEM;
            case '/':   if ( append( refstr, (C)'/' , cchstr, climit ) == 0 )  break;  else return ENOMEM;
            case '\\':  if ( append( refstr, (C)'\\', cchstr, climit ) == 0 )
              {  chnext = '\0';  break;  }  else return ENOMEM;
            default:    except( "invalid escape sequence" );
                        return EINVAL;
          }
          chprev = '\0';
        }
        except( "unexpected end of stream" );
        return EINVAL;
      }

  protected:  // helpers
    template <class C, class M>
    int   append( _auto_<C, M>& refstr, C c, int& cchstr, int& climit )
      {
        if ( cchstr >= climit - 1 )
        {
          _auto_<C, M>  newstr;
          int           newlim;

          if ( (newstr = (C*)M().alloc( (newlim = climit + 0x100) * sizeof(C) )) == nullptr )
            return ENOMEM;
          if ( refstr != nullptr )
            memcpy( (C*)newstr, (C*)refstr, sizeof(C) * (cchstr + 1) );
          climit = newlim;
          refstr = newstr;
        }

        refstr[cchstr++] = c;
        refstr[cchstr+0] = (C)0;
        return 0;
      }
  };

  /*
    helper functions
  */
  inline  bool  wstrtodbl( double& o, const widechar* s )
  {
    widechar* endptr;
      o = w_strtod( s, &endptr );
    return *endptr == 0;
  }

  inline  bool  wstrtoint( unsigned& o, const widechar* s )
  {
    widechar* endptr;
      o = w_strtol( s, &endptr, 10 );
    return *endptr == 0 && endptr > s;
  }

  template <class M>
  inline  bool  wstrtostr( _auto_<char, M>& o, const widechar* s )
  {
    _auto_<char, M> a;
    char*           p;

    if ( (p = a = (char*)M().alloc( sizeof(char) * (1 + w_strlen( s )))) == nullptr )
      return false;

    while ( *s != 0 && *s < 0x100 )
      *p++ = (char)*s++;

    if ( *s == 0 )  {  *p = '\0';  o = a;  return true;  }
      else return false;
  }

// JSON decoration modes

  struct json_compact
  {
    template <class O>  O*  Shift( O* o ) const {  return o;  }
    template <class O>  O*  Space( O* o ) const {  return o;  }
    template <class O>  O*  Break( O* o ) const {  return o;  }
  };

  class json_decorated
  {
    int   nlevel;

  public:     // construction
    json_decorated(): nlevel( 0 ) {  }
    json_decorated( const json_decorated& js ): nlevel( js.nlevel + 1 ) {  }

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

// JSON serialization

  # if !defined( _MSC_VER )
  #   if !defined( PRId64 )
  #     if __WORDSIZE == 64
  #       define PRId64 "ld"
  #     elif __WORDSIZE == 32
  #       define PRId64 "lld"
  #     else
  #       error Strange int64_t type!
  #     endif
  #   endif

  #   if !defined( PRIu64 )
  #     if __WORDSIZE == 64
  #       define PRIu64 "lu"
  #     elif __WORDSIZE == 32
  #       define PRIu64 "llu"
  #     else
  #       error Strange int64_t type!
  #     endif
  #   endif
  # endif

  # define  derive_printjson_dec( _type_, _tmpl_ )                                      \
  template <class O, class D = json_compact>                                            \
  inline  O*  PrintJson( O* o, _type_ t, const D& decorate = D() )                      \
    {                                                                                   \
      char  decval[0x40];                                                               \
      return ::Serialize( decorate.Shift( o ), decval, sprintf( decval, _tmpl_, t ) );  \
    }
    derive_printjson_dec( char,   "%d" )
    derive_printjson_dec( byte_t, "%u" )
    derive_printjson_dec( int16_t,  "%d" )
    derive_printjson_dec( word16_t, "%u" )
    derive_printjson_dec( int32_t,  "%d" )
    derive_printjson_dec( word32_t, "%u" )
  # if defined( _MSC_VER )
    derive_printjson_dec( int64_t,  "%I64d" )
    derive_printjson_dec( word64_t, "%I64u" )
  # else
    derive_printjson_dec( int64_t,  "%" PRId64 )
    derive_printjson_dec( word64_t, "%" PRIu64 )
  # endif  // _MSC_VER
  # undef derive_printjson_dec

  # define  derive_printjson_flo( _type_ )                                            \
    template <class O, class D = json_compact>                                        \
    inline  O*  PrintJson( O* o, _type_ t, const D& decorate = D() )                  \
    {                                                                                 \
      char  floval[0x10];                                                             \
      return ::Serialize( decorate.Shift( o ), floval, sprintf( floval, "%f", t ) );  \
    }
    derive_printjson_flo( float )
    derive_printjson_flo( double )
  # undef derive_printjson_flo

  /*
    Строка - либо utf-8, либо "мы не знаем, что за строка". В обоих случаях делаются замены экранируемых символов,
    а всё остальное сериализуется либо как есть, либо в виде шестнадцатеричного кода, если не укладывается в utf8
  */
  template <class O>
  inline  O*  PrintText( O* o, const char* s, size_t l )
  {
    static char         repsrc[] = "\b\t\n\f\r\"/\\";
    static const char*  repval[] = { "\\b", "\\t", "\\n", "\\f", "\\r", "\\\"", "\\/", "\\\\" };
    char*               reppos;
    const char*         endptr;
    char                chbuff[0x10];
    size_t              nstore;

    if ( s == nullptr )
      return ::Serialize( o, "null", 4 );

    for ( endptr = s + l, o = ::Serialize( o, '\"' ); s < endptr; )
    {
      unsigned char chnext;

      if ( (reppos = strchr( repsrc, chnext = (unsigned char)*s++ )) != nullptr )
        o = ::Serialize( o, repval[reppos - repsrc], nstore = strlen( repval[reppos - repsrc] ) );
      else if ( chnext < 0x20 )
        o = ::Serialize( o, chbuff, nstore = sprintf( chbuff, "\\u%04x", chnext ) );
      else if ( chnext < 0x80 )
        o = ::Serialize( o, chnext );
      else
        {
          int   cbchar;
          int   nleast;

        /* detect the utf-8 unicode char byte count */
          if ( (chnext & 0xe0) == 0xc0 )  {  nleast = 1;  }  else
          if ( (chnext & 0xf0) == 0xe0 )  {  nleast = 2;  }  else
          if ( (chnext & 0xf8) == 0xf0 )  {  nleast = 3;  }  else
          if ( (chnext & 0xfc) == 0xf8 )  {  nleast = 4;  }  else
          if ( (chnext & 0xfe) == 0xfc )  {  nleast = 5;  }  else nleast = 0;

          for ( cbchar = 0; cbchar < nleast && s + cbchar < endptr && (s[cbchar] & 0xc0) == 0x80; ++cbchar )
            (void)NULL;

          if ( nleast == 0 || cbchar < nleast )
            {
              o = ::Serialize( o, chbuff, nstore = sprintf( chbuff, "\\u%04x", chnext ) );
            }
          else
            {
              o = ::Serialize( o, s - 1, cbchar + 1 );
              s += cbchar;
            }
        }
    }

    return ::Serialize( o, '\"' );
  }

  template <class O>
  inline  O*  PrintText( O* o, const widechar* s, size_t l )
  {
    static char         repsrc[] = "\b\t\n\f\r\"/\\";
    static const char*  repval[] = { "\\b", "\\t", "\\n", "\\f", "\\r", "\\\"", "\\/", "\\\\" };
    char*               reppos;
    char                chnext[0x10];

    if ( s == nullptr )
      return ::Serialize( o, "null", 4 );

    for ( o = ::Serialize( o, '\"' ); l-- > 0; ++s )
    {
      if ( (*s & ~0xff) == 0 && (reppos = strchr( repsrc, *s )) != nullptr )
        o = ::Serialize( o, repval[reppos - repsrc], (unsigned)strlen( repval[reppos - repsrc] ) );
      else if ( *s >= 0x80 || *s < 0x20 )
        o = ::Serialize( o, chnext, sprintf( chnext, "\\u%04x", *s ) );
      else
        o = ::Serialize( o, (char)*s );
    }

    return ::Serialize( o, '\"' );
  }

  template <class O, class D = json_compact>
  inline  O*  PrintJson( O* o, const char* s, const D& decorate = D() )
    {
      (void)decorate;
      return PrintText( o, s, w_strlen( s ) );
    }
  template <class O, class D = json_compact>
  inline  O*  PrintJson( O* o, const widechar* s, const D& decorate = D() )
    {
      (void)decorate;
      return PrintText( o, s, w_strlen( s ) );
    }
  template <class O, class D = json_compact>
  inline  O*  PrintJson( O* o, const _auto_<char>& s, const D& decorate = D() )
    {
      (void)decorate;
      return PrintText( o, (const char*)s, w_strlen( s ) );
    }
  template <class O, class D = json_compact>
  inline  O*  PrintJson( O* o, const _auto_<widechar>& s, const D& decorate = D() )
    {
      (void)decorate;
      return PrintText( o, (const widechar*)s, w_strlen( s ) );
    }

  template <class O, class M, class D = json_compact>  O*  PrintJson( O*, const xvalue<M>&, const D& decorate = D() );
  template <class O, class M, class D = json_compact>  O*  PrintJson( O*, const zarray<M>&, const D& decorate = D() );

// arrays
  template <class O, class T, class M, class D = json_compact>
  inline  O*  PrintJson( O* o, const array<T, M>& a, const D& decorate = D() )
  {
    auto  ptop = a.begin();
    auto  pend = a.end();

    for ( o = decorate.Break( ::Serialize( decorate.Shift( o ), '[' ) ); o != nullptr && ptop < pend; ++ptop )
    {
      o = decorate.Break( ::Serialize( PrintJson( o, *ptop, D( decorate ) ), ",", ptop < pend - 1 ? 1 : 0 ) );
    }
    return ::Serialize( decorate.Shift( o ), ']' );
  }

  template <class O, class M, class D>
  inline  O*  PrintJson( O* o, const xvalue<M>& v, const D& decorate )
  {
    switch ( v.gettype() )
    {
      case z_char:    return PrintJson( decorate.Shift( o ), *v.get_char() );
      case z_byte:    return PrintJson( decorate.Shift( o ), *v.get_byte() );
      case z_int16:   return PrintJson( decorate.Shift( o ), *v.get_int16() );
      case z_word16:  return PrintJson( decorate.Shift( o ), *v.get_word16() );
      case z_int32:   return PrintJson( decorate.Shift( o ), *v.get_int32() );
      case z_word32:  return PrintJson( decorate.Shift( o ), *v.get_word32() );
      case z_int64:   return PrintJson( decorate.Shift( o ), *v.get_int64() );
      case z_word64:  return PrintJson( decorate.Shift( o ), *v.get_word64() );
      case z_float:   return PrintJson( decorate.Shift( o ), *v.get_float() );
      case z_double:  return PrintJson( decorate.Shift( o ), *v.get_double() );

      case z_charstr: return PrintJson( decorate.Shift( o ), v.get_charstr() );
      case z_widestr: return PrintJson( decorate.Shift( o ), v.get_widestr() );
      case z_zarray:  return PrintJson(                 o,  *v.get_zarray(), decorate );

      case z_array_char:    return PrintJson( decorate.Break( o ), *v.get_array_char(),   D( decorate ) );
      case z_array_byte:    return PrintJson( decorate.Break( o ), *v.get_array_byte(),   D( decorate ) );
      case z_array_int16:   return PrintJson( decorate.Break( o ), *v.get_array_int16(),  D( decorate ) );
      case z_array_word16:  return PrintJson( decorate.Break( o ), *v.get_array_word16(), D( decorate ) );
      case z_array_int32:   return PrintJson( decorate.Break( o ), *v.get_array_int32(),  D( decorate ) );
      case z_array_word32:  return PrintJson( decorate.Break( o ), *v.get_array_word32(), D( decorate ) );
      case z_array_int64:   return PrintJson( decorate.Break( o ), *v.get_array_int64(),  D( decorate ) );
      case z_array_word64:  return PrintJson( decorate.Break( o ), *v.get_array_word64(), D( decorate ) );
      case z_array_float:   return PrintJson( decorate.Break( o ), *v.get_array_float(),  D( decorate ) );
      case z_array_double:  return PrintJson( decorate.Break( o ), *v.get_array_double(), D( decorate ) );

      case z_array_charstr: return PrintJson( decorate.Break( o ), *v.get_array_charstr(), D( decorate ) );
      case z_array_widestr: return PrintJson( decorate.Break( o ), *v.get_array_widestr(), D( decorate ) );
      case z_array_zarray:  return PrintJson( decorate.Break( o ), *v.get_array_zarray(), D( decorate ) );
      case z_array_xvalue:  return PrintJson( decorate.Break( o ), *v.get_array_xvalue(), D( decorate ) );

      default:  assert( false );  abort();  return o;
    }
  }

  template <class O, class M, class D>
  inline  O*  PrintJson( O* o, const zarray<M>& v, const D& decorate )
  {
    bool  bcomma = false;

    o = decorate.Break( ::Serialize( decorate.Shift( o ), '{' ) );

    v.for_each( [&]( const typename zarray<M>::zkey& k, const xvalue<M>& v )
      {
        D   subdecor( decorate );

      // possible comma
        if ( bcomma )
          o = subdecor.Break( ::Serialize( o, ',' ) );

      // key
        if ( (const char*)k != nullptr )      o = PrintJson( subdecor.Shift( o ), (const char*)k );  else
        if ( (const widechar*)k != nullptr )  o = PrintJson( subdecor.Shift( o ), (const widechar*)k );  else
                                            o = ::Serialize( PrintJson( ::Serialize( subdecor.Shift( o ), '"' ), (unsigned)k ), '"' );

      // value
        o = PrintJson( ::Serialize( o, ':' ), v, subdecor );
        bcomma = true;
        return 0;
      } );

    return ::Serialize( decorate.Shift( bcomma ? decorate.Break( o ) : o ), '}' );
  }

  /*
    Parsing:
      - ordinal values;
  */

  inline  bool  json_is_num_char( char ch ) {  return ch >= '0' && ch <= '9';  }
  inline  bool  json_is_int_char( char ch ) {  return json_is_num_char( ch ) || ch == '-';  }
  inline  bool  json_is_flo_char( char ch ) {  return json_is_int_char( ch ) || ch == '.' || ch == 'e' || ch == 'E';  }

  // unsigned integers
  # define  derive_parse( _type_ )                                                      \
    template <class S, class X = json_use_exceptions_t>                                 \
    S*  ParseJson( jsonstream<S>& s, _type_& v, const X& x = X() )                      \
    {                                                                                   \
      char  chnext;                                                                     \
                                                                                        \
      if ( !json_is_num_char( chnext = s.nospace() ) )                                  \
        return (S*)x( "0..9 expected" );                                                \
      for ( v = (byte_t)chnext - '0'; json_is_num_char( chnext = s.getnext() ); )       \
        v = v * 10 + (byte_t)chnext - '0';                                              \
      return s.putback( chnext );                                                       \
    }
    derive_parse( byte_t )
    derive_parse( word16_t )
    derive_parse( word32_t )
    derive_parse( word64_t )
  # undef derive_parse

  // signed integers
  # define  derive_parse( _type_ )                                                      \
  template <class S, class X = json_use_exceptions_t>                                   \
  S*  ParseJson( jsonstream<S>& s, _type_& v, const X& x = X() )                        \
  {                                                                                     \
    char  chnext;                                                                       \
    bool  is_neg;                                                                       \
                                                                                        \
    if ( (is_neg = ((chnext = s.nospace()) == '-')) != false )                          \
      chnext = s.getnext();                                                             \
    if ( !json_is_num_char( chnext ) )                                                  \
      return (S*)x( "0..9 expected" );                                                  \
    for ( v = (byte_t)chnext - '0'; json_is_num_char( chnext = s.getnext() ); )         \
      v = v * 10 + (byte_t)chnext - '0';                                                \
    if ( is_neg ) v = -v;                                                               \
      return s.putback( chnext );                                                       \
  }
    derive_parse( char )
    derive_parse( int16_t )
    derive_parse( int32_t )
    derive_parse( int64_t )
  # undef derive_parse

  // floating
  # define  derive_parse( _type_ )                                                                \
    template <class S, class X = json_use_exceptions_t>                                           \
    S*  ParseJson( jsonstream<S>& s, _type_&  v, const X& x = X() )                               \
    {                                                                                             \
      char  flobuf[0x40];                                                                         \
      char* floptr;                                                                               \
      char* endptr;                                                                               \
                                                                                                  \
      for ( *(floptr = flobuf) = s.nospace();                                                     \
        floptr < flobuf + 0x40 && json_is_flo_char( *floptr ); *++floptr = s.getnext() )  (void)0;\
                                                                                                  \
      v = (_type_)strtod( flobuf, &endptr );                                                      \
                                                                                                  \
      if ( floptr >= array_end(flobuf) )                                                          \
        return (S*)x( "sequence too long to be float" );                                          \
                                                                                                  \
      if ( endptr != floptr )                                                                     \
        return (S*)x( "invalid floating point value" );                                           \
                                                                                                  \
      return s.putback( *floptr );                                                                \
    }
    derive_parse( float )
    derive_parse( double )
  # undef derive_parse

  // arrays
  # define  derive_fetch_array( _type_ )                                              \
    template <class S, class M, class X = json_use_exceptions_t>                      \
    S*  ParseJson( jsonstream<S>& s, array<_type_, M>& a, const X& x = X() )          \
    {                                                                                 \
      char  chnext;                                                                   \
                                                                                      \
      if ( (chnext = s.nospace()) != '[' )                                            \
        return (S*)x( "'[' expected" );                                               \
                                                                                      \
      while ( (chnext = s.nospace()) != '\0' && chnext != ']' )                       \
      {                                                                               \
        _type_  avalue;                                                               \
                                                                                      \
        if ( ParseJson( s.putback( chnext ), avalue, x ) == nullptr )                 \
          return nullptr;                                                             \
        if ( a.Append( avalue ) != 0 )                                                \
          return nullptr;                                                             \
        if ( (chnext = s.nospace()) == ',' )                                          \
          continue;                                                                   \
        if ( chnext == ']' )  s.putback( chnext );                                    \
          else return (S*)x( "',' or ']' expected" );                                 \
      }                                                                               \
      return chnext == ']' ? s : (S*)x( "']' expected" );                             \
    }
    derive_fetch_array( char )
    derive_fetch_array( byte_t )
    derive_fetch_array( int16_t )
    derive_fetch_array( int32_t )
    derive_fetch_array( int64_t )
    derive_fetch_array( word16_t )
    derive_fetch_array( word32_t )
    derive_fetch_array( word64_t )
    derive_fetch_array( float )
    derive_fetch_array( double )
    derive_fetch_array( zarray<M> )
  # undef derive_fetch_array

  template <class S, class C, class M, class X = json_use_exceptions_t>
  S*  ParseJson( jsonstream<S>& s, array<_auto_<C, M>, M>& a, const X& x = X() )
  {
    char  chnext;

    if ( (chnext = s.nospace()) != '[' )
      return nullptr;

    while ( (chnext = s.nospace()) != '\0' && chnext != ']' )
    {
      _auto_<C, M>  sznext;

      if ( chnext != '\"' )
        return (S*)x( "string value must start with '\"'" );

      if ( s.putback( chnext ).getstring( sznext, x ) != 0 || a.Append( sznext ) != 0 )
        return nullptr;

      if ( (chnext = s.nospace()) == ',' )
        continue;
      if ( chnext == ']' )  s.putback( chnext );
        else return (S*)x( "']' expected" );
    }
    return chnext == ']' ? s : (S*)x( "']' expected" );
  }

  template <class S, class T, class M, class X = json_use_exceptions_t>
  S*  ParseJson( jsonstream<S>& s, array<T, M>& a, const jsonRevive* p = nullptr, const X& x = X() )
  {
    char  chnext;

    if ( (chnext = s.nospace()) != '[' )
      return (S*)x( "'[' expected" );

    while ( (chnext = s.nospace()) != '\0' && chnext != ']' )
    {
      T t;

      if ( (S*)(s = ParseJson( s.putback( chnext ), t, p, x )) == nullptr )
        return nullptr;
      if ( a.Append( t ) != 0 )
        return nullptr;

      if ( (chnext = s.nospace()) == ',' )
        continue;
      if ( chnext == ']' )  s.putback( chnext );
        else return (S*)x( "']' expected" );
    }
    return chnext == ']' ? s : (S*)x( "']' expected" );
  }

  template <class S, class M, class X = json_use_exceptions_t>
  S*  ParseJson( jsonstream<S>& s, xvalue<M>& x, const jsonRevive* p, const X& e = X()  )
  {
    const byte_t  vatype = p != nullptr ? p->v_type : 0xff;
    char          chnext;

    switch ( vatype )
    {
      case z_char:     return ParseJson( s, *x.set_char(), e );
      case z_int16:    return ParseJson( s, *x.set_int16(), e );
      case z_int32:    return ParseJson( s, *x.set_int32(), e );
      case z_int64:    return ParseJson( s, *x.set_int64(), e );
      case z_byte:     return ParseJson( s, *x.set_byte(), e );
      case z_word16:   return ParseJson( s, *x.set_word16(), e );
      case z_word32:   return ParseJson( s, *x.set_word32(), e );
      case z_word64:   return ParseJson( s, *x.set_word64(), e );
      case z_float:    return ParseJson( s, *x.set_float(), e );
      case z_double:   return ParseJson( s, *x.set_double(), e );
      case z_charstr:  return s.getstring( *(_auto_<char, M>*)&x.set_charstr(), e ) == 0 ? s : nullptr;
      case z_widestr:  return s.getstring( *(_auto_<widechar, M>*)&x.set_widestr(), e ) == 0 ? s : nullptr;
//      case z_buffer:   return ParseJsonStore( o, *(_freebuffer_*)&chdata );
      case z_zarray:   return ParseJson( s, *x.set_zarray(), p, e );

      case z_array_char:   return ParseJson( s, *x.set_array_char(), e );
      case z_array_byte:   return ParseJson( s, *x.set_array_byte(), e );
      case z_array_int16:  return ParseJson( s, *x.set_array_int16(), e );
      case z_array_int32:  return ParseJson( s, *x.set_array_int32(), e );
      case z_array_int64:  return ParseJson( s, *x.set_array_int64(), e );
      case z_array_word16: return ParseJson( s, *x.set_array_word16(), e );
      case z_array_word32: return ParseJson( s, *x.set_array_word32(), e );
      case z_array_word64: return ParseJson( s, *x.set_array_word64(), e );
      case z_array_float:  return ParseJson( s, *x.set_array_float(), e );
      case z_array_double: return ParseJson( s, *x.set_array_double(), e );
      case z_array_zarray: return ParseJson( s, *x.set_array_zarray(), p != nullptr ? p->nested : nullptr, e );
      case z_array_charstr: return ParseJson( s, *x.set_array_charstr(), e );
      case z_array_widestr: return ParseJson( s, *x.set_array_widestr(), e );
/*      case z_array_buffer: return ::JsonStore( o, *(array_buffer*)&chdata );
      case z_array_xvalue: return ::JsonStore( o, *(array_xvalue*)&chdata );
  */
      default:  break;
    }

  // untyped xvalue load: object zarray {...}, untyped array [...], string "...", integer -?[0-9]+ or float -?[0-9]+\.?[0-9]*([Ee]-?[0-9]+)?
    switch ( chnext = s.nospace() )
    {
      case '{':   return ParseJson( s.putback( chnext ), *x.set_zarray(), p, e );
      case '[':
        {
          if ( (s = ParseJson( s.putback( chnext ), *x.set_array_xvalue(), p, e )) != nullptr && p == nullptr && x.get_array_xvalue()->size() > 0 )
          {
            array<xvalue<M>, M>*  parray = x.get_array_xvalue();
            xvalue<M>             newval;

            for ( auto p = parray->begin() + 1; p < parray->end(); ++p )
              if ( p->gettype() != p[-1].gettype() )
                return (S*)s;

            switch ( parray->first().gettype() )
            {
            # define  derive_transform( _type_ )                                                        \
              case z_##_type_:                                                                          \
                {                                                                                       \
                  array<_type_##_t, M>& a = *newval.set_array_##_type_();                               \
                                                                                                        \
                  if ( parray->for_each( [&a]( const xvalue<M>& s ){  return a.Append( *s.get_##_type_() );  } ) != 0 ) \
                    return (S*)s;                                                                       \
                  break;                                                                                \
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
            # undef  derive_transform
              case z_charstr:
                {
                  array<_auto_<char, M>, M>&  a = *newval.set_array_charstr();

                  if ( a.SetLen( parray->GetLen() ) != 0 )
                    return (S*)s;
                  for ( auto i = 0; i < parray->size(); ++i )
                    inplace_swap( *(char**)(a + i), *(char**)(*parray)[i].get_holder() );
                  break;
                }
              case z_widestr:
                {
                  array<_auto_<widechar, M>, M>&  a = *newval.set_array_widestr();

                  if ( a.SetLen( parray->GetLen() ) != 0 )
                    return (S*)s;
                  for ( auto i = 0; i < parray->size(); ++i )
                    inplace_swap( *(widechar**)(a + i), *(widechar**)(*parray)[i].get_holder() );
                  break;
                }
              case z_zarray:
                {
                  array<zarray<M>, M>&  a = *newval.set_array_zarray();

                  if ( a.SetLen( parray->GetLen() ) != 0 )
                    return (S*)s;
                  for ( auto i = 0; i < parray->size(); ++i )
                    a[i] = *(*parray)[i].get_zarray();
                  break;
                }
              default:
                newval = x;
                break;
            }
            x = newval;
          }
          return (S*)s;
        }
      case '\"':
        {
          _auto_<widechar, M> wcsstr;
          _auto_<char, M>     chrstr;
          double              dblval;
          unsigned            intval;

        // get next string
          if ( s.putback( chnext ).getstring( wcsstr, e ) != 0 )
            return nullptr;

        // check if charstr or widestr
          if ( wstrtodbl( dblval, wcsstr ) )  x.set_double( dblval );   else
          if ( wstrtoint( intval, wcsstr ) )  x.set_word32( intval );   else
          if ( wstrtostr( chrstr, wcsstr ) )  x.set_charstr( chrstr );  else                              
                                              x.set_widestr( wcsstr );
          return s;
        }
      default:  break;
    }
    if ( json_is_int_char( chnext ) )
    {
      array<char, M>  cvalue;
      bool            bpoint;
      bool            bexpon;
      char            chprev;

      if ( cvalue.Append( chnext ) != 0 )
        return nullptr;
      for ( chprev = '\0', bexpon = bpoint = false; ; )
      {
        if ( json_is_num_char( chnext = s.getnext() ) )
        {
          if ( cvalue.Append( chprev = chnext ) != 0 )
            return nullptr;
        }
          else
        if ( chnext == '.' )
        {
          if ( bpoint || bexpon || cvalue.Append( chprev = chnext ) != 0 ) return nullptr;
            else  bpoint = true;
        }
          else
        if ( chnext == 'e' || chnext == 'E' )
        {
          if ( bexpon || cvalue.Append( chprev = chnext ) != 0 ) return nullptr;
            else  bexpon = true;
        }
          else
        if ( chnext == '-' )
        {
          if ( (chprev != 'e' && chprev != 'E') || cvalue.Append( chprev = chnext ) != 0 )
            return nullptr;
        }
          else
        {
          s.putback( chnext );
          break;
        }
      }
      if ( cvalue.Append( '\0' ) != 0 )
        return nullptr;
      if ( bpoint || bexpon )
      {
        char*   endptr;
        double  dvalue = strtod( cvalue, &endptr );

        if ( *endptr != '\0' )  return nullptr;
          else  x.set_double( dvalue );
      }
        else
      {
        char*   endptr;
        int32_t nvalue = strtol( cvalue, &endptr, 10 );

        if ( *endptr != '\0' )  return nullptr;
          else  x.set_int32( nvalue );
      }
      return s;
    }

  // check for boolean represented as byte
    if ( chnext == 't' )
    {
      if ( (chnext = s.getnext()) != 'r'
        || (chnext = s.getnext()) != 'u'
        || (chnext = s.getnext()) != 'e' )  return (S*)e( "unexpected character '%c'", chnext );
      x.set_byte( 1 );
        return s;
    }
      else
    if ( chnext == 'f' )
    {
      if ( (chnext = s.getnext()) != 'a'
        || (chnext = s.getnext()) != 'l'
        || (chnext = s.getnext()) != 's'
        || (chnext = s.getnext()) != 'e' )  return (S*)e( "unexpected character '%c'", chnext );
      x.set_byte( 0 );
        return s;
    }
      else
    return (S*)e( "unexpected character '%c'", chnext );
  }

  template <class S, class M, class X = json_use_exceptions_t>
  S*  ParseJson( jsonstream<S>& s, zarray<M>& z, const jsonRevive* p = nullptr, const X& x = X() )
  {
    char  chnext;

  // reset
    z.DelAllData();

  // open object
    if ( (chnext = s.nospace()) != '{' )
      return (S*)x( "'{' expected" );

  // char by char until end or '}'
    while ( (chnext = s.nospace()) != '\0' && chnext != '}' )
    {
      unsigned            intkey = 0;
      _auto_<char, M>     strkey;
      _auto_<widechar, M> wcskey;
      xvalue<M>*          newval = nullptr;
      const jsonRevive*   ptypes = nullptr;

    // get variable name as widestring
      try
      {
        if ( s.putback( chnext ).getstring( wcskey, x ) != 0 )
          return nullptr;
      }
      catch ( const ParseJsonError& jx )
      {
        x( "%s while parsing variable name", jx.what() );
      }

    // check variable name type (if defined); create key type and value type for the key
      if ( p != nullptr )
      {
        if (                                (ptypes = p->Search( wcskey )) != nullptr )  newval = z.put_xvalue( wcskey );
          else
        if ( wstrtostr( strkey, wcskey ) && (ptypes = p->Search( strkey )) != nullptr )  newval = z.put_xvalue( strkey );
          else
        if ( wstrtoint( intkey, wcskey ) && (ptypes = p->Search( intkey )) != nullptr )  newval = z.put_xvalue( intkey );
      }

    // default key create
      if ( newval == nullptr )
      {
        if (                      wstrtoint( intkey, wcskey ) )  newval = z.put_xvalue( intkey );  else
        if ( strkey != nullptr || wstrtostr( strkey, wcskey ) )  newval = z.put_xvalue( strkey );  else
                                                                 newval = z.put_xvalue( wcskey );
      }

    // check for colon
      if ( (chnext = s.nospace()) != ':' )
        return (S*)x( "':' expected" );

    // get the value
      try
      {
        if ( (S*)(s = ParseJson( s, *newval, ptypes, x )) == nullptr )
          return nullptr;
      }
      catch ( const ParseJsonError& jx )
      {
        x( "%s while parsing variable value", jx.what() );
      }

    // check for comma
      if ( (chnext = s.nospace()) == ',' )
        continue;
      if ( chnext == '}' )  s.putback( chnext );
        else return (S*)x( "'}' or ',' expected" );
    }

  // check valid script
    return chnext == '}' ? s : (S*)x( "'}' expected" );
  }

  template <class S, class M, class X = json_use_exceptions_t>
  S*  ParseJson( S* s, xvalue<M>& x, const jsonRevive* p = nullptr, const X& e = X() )
  {
    jsonstream<S> source( s );

    return ParseJson( source, x, p, e );
  }

  template <class S, class M, class X = json_use_exceptions_t>
  S*  ParseJson( S* s, zarray<M>& z, const jsonRevive* p = nullptr, const X& x = X() )
  {
    jsonstream<S> source( s );

    return ParseJson( source, z, p, x );
  }

}

# endif  // __jsonTools_h__
