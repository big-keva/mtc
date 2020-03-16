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
# pragma once
# if !defined( __mtc_uuid_h__ )
# define __mtc_uuid_h__
# include "serialize.decl.h"
# include "wcsstr.h"
# include <stdexcept>
# include <cstdint>

namespace mtc {

  class uuid
  {
    friend std::string  to_string( const uuid& );

  public:
    enum: size_t {  length = 16 };

  public:
    class parse_error: public std::runtime_error  {  using std::runtime_error::runtime_error;  };

  public:
    uuid()
      {  std::fill( std::begin( ubytes ), std::end( ubytes ), 0 );  }
    uuid( const uint8_t (&val)[16] )
      {  std::copy( std::begin( val ), std::end( val ), std::begin( ubytes ) );  }
    uuid( const uuid& rt )
      {  std::copy( std::begin( rt.ubytes ), std::end( rt.ubytes ), std::begin( ubytes ) );  }
    uuid& operator = ( const uuid& rt )
      {  return std::copy( std::begin( rt.ubytes ), std::end( rt.ubytes ), std::begin( ubytes ) ), *this;  }

  public:
    auto  data() const -> const uint8_t*  {  return ubytes;  }
    auto  size() const -> size_t          {  return 16;  }

  public:
    static  uuid  get_uuid( const char* s, const char* e = nullptr )  {  return get_uuid<char>( s, e );  }
    static  uuid  get_uuid( const widechar* s, const widechar* e = nullptr )  {  return get_uuid<widechar>( s, e );  }
    static  uuid  get_uuid( const charstr& s )   {  return get_uuid( s.c_str() );  }
    static  uuid  get_uuid( const widestr& s )   {  return get_uuid( s.c_str() );  }

    static  auto  parse_uuid( uuid& o, const char* s, const char* e = nullptr )  -> const char*  {  return parse_uuid<char>( o, s, e );  }
    static  auto  parse_uuid( uuid& o, const widechar* s, const widechar* e = nullptr )  -> const widechar*  {  return parse_uuid<widechar>( o, s, e );  }

  protected:
    template <class C>
    static  uuid      get_uuid( const C*, const C* );
    template <class C>
    static  const C*  parse_uuid( uuid&, const C*, const C* );

  public:
    bool  operator == ( const uuid& rt ) const  {  return compare( rt ) == 0;  }
    bool  operator != ( const uuid& rt ) const  {  return !(*this == rt);  }
    bool  operator <  ( const uuid& rt ) const  {  return compare( rt ) <  0;  }
    bool  operator <= ( const uuid& rt ) const  {  return compare( rt ) <= 0;  }
    bool  operator >  ( const uuid& rt ) const  {  return compare( rt ) >  0;  }
    bool  operator >= ( const uuid& rt ) const  {  return compare( rt ) >= 0;  }
    auto  compare( const uuid& rt ) const -> int
      {  return memcmp( ubytes, rt.ubytes, sizeof(ubytes) );  }

  protected:
    uint8_t ubytes[length];

  };

  using uuid_t = uuid;

  inline  auto  to_string( const uuid_t& uuid ) -> std::string
    {
      return strprintf( "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid.ubytes[0x0], uuid.ubytes[0x1], uuid.ubytes[0x2], uuid.ubytes[0x3],
        uuid.ubytes[0x4], uuid.ubytes[0x5], uuid.ubytes[0x6], uuid.ubytes[0x7],
        uuid.ubytes[0x8], uuid.ubytes[0x9], uuid.ubytes[0xa], uuid.ubytes[0xb],
        uuid.ubytes[0xc], uuid.ubytes[0xd], uuid.ubytes[0xe], uuid.ubytes[0xf] );
    }

  template <class C>
  uuid  uuid::get_uuid( const C* s, const C* e )
    {
      uuid  uvalue;

      return parse_uuid( uvalue, s, e ), uvalue;
    }

  template <class C>
  const C*  uuid::parse_uuid( uuid& o, const C* s, const C* e )
    {
      auto  getchr = []( uint8_t& o, const C* s ) -> const C*
        {
          auto  chnext = *s++;

          if ( chnext >= '0' && chnext <= '9' ) o = (chnext - '0' + 0x0) << 4;  else
          if ( chnext >= 'a' && chnext <= 'f' ) o = (chnext - 'a' + 0xa) << 4;  else
          if ( chnext >= 'A' && chnext <= 'F' ) o = (chnext - 'A' + 0xa) << 4;  else
          return nullptr;

          chnext = *s++;

          if ( chnext >= '0' && chnext <= '9' ) o |= (chnext - '0' + 0x0);  else
          if ( chnext >= 'a' && chnext <= 'f' ) o |= (chnext - 'a' + 0xa);  else
          if ( chnext >= 'A' && chnext <= 'F' ) o |= (chnext - 'A' + 0xa);  else
          return nullptr;

          return *s == '-' ? s + 1 : s;
        };
      auto* output = std::begin( o.ubytes );

      if ( e == nullptr )
        for ( auto e = s; *e != '\0'; ++e ) (void)NULL;

      for ( auto i = 0; i != 16 && s < e - 1; ++i )
        if ( (s = getchr( *output++, s )) == nullptr )  throw parse_error( "hex character expected" );

      if ( output != std::end( o.ubytes ) )
        throw parse_error( "not enough data to initialize uuid" );

      return s;
    }

}

inline  size_t  GetBufLen( const mtc::uuid_t& uuid )
  {  (void)uuid;  return 16;  }
template <class O>
inline  O*      Serialize( O* o, const mtc::uuid_t& uuid )
  {
    return ::Serialize( o, uuid.data(), uuid.size() );
  }
template <class S>
inline  S*      FetchFrom( S* s, mtc::uuid_t& uuid )
  {
    return ::FetchFrom( s, (uint8_t*)uuid.data(), uuid.size() );
  }

# endif   // __mtc_uuid_h__
