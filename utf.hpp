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
# if !defined( __mtc_utf_hpp__ )
# define __mtc_utf_hpp__
# include <cstdint>
# include <cstddef>
# include "wcsstr.h"
# include <stdexcept>

# if !defined( __widechar_defined__ )
# define  __widechar_defined__
#   if defined(WCHAR_MAX) && (WCHAR_MAX >> 16) == 0
    typedef wchar_t         widechar;
#   else
    typedef unsigned short  widechar;
#   endif  // size
# endif  // __widechar_defined__

namespace mtc {

  class utf;
  class utf8;
  class utf16;
  class utf32;

  class utf
  {
  public:
    template <class target, class in>
    static  auto  strlen( const target&, in src ) -> size_t;
    template <class target>
    static  auto  encode( target out, uint32_t chr ) -> typename target::res_type;
    template <class target, class in>
    static  auto  encode( target out, in src ) -> typename target::res_type;

  public:     // strlen
    static  auto  strlen( const utf8& out, const char* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf8& out, const widechar* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf8& out, const uint32_t* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf16& out, const char* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf16& out, const widechar* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf16& out, const uint32_t* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf32& out, const char* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf32& out, const widechar* str, size_t len = (size_t)-1 ) -> size_t;
    static  auto  strlen( const utf32& out, const uint32_t* str, size_t len = (size_t)-1 ) -> size_t;

  public:     // old-style helpers for utf16/utf8 conversions
  /*
   *  encode( pszout, cchout, pwssrc, cchsrc )
   *  Кодирует символ в utf8
   */  
    static  auto  encode( char* out, size_t len, uint32_t wch ) -> size_t;
  /*
   *  encode( pszout, cchout, pwssrc, cchsrc )
   *  Кодирует строку в utf8
   */  
    static  auto  encode( char* out, size_t len, const widechar* src, size_t cch = (size_t)-1 ) -> size_t;
    static  auto  encode( const widechar*, size_t = (size_t)-1 ) -> charstr;
    static  auto  encode( const widestr& ) -> charstr;

  /*
   *  decode( pwsout, cchout, pszsrc, cchsrc )
   *  Декодирует строку из utf8 в utf16
   */
    inline  auto  decode( widechar* out, size_t len, const char* src, size_t cch = (size_t)-1 ) -> size_t;
    static  auto  decode( const char*, size_t = (size_t)-1 ) -> widestr;
    static  auto  decode( const charstr& ) -> widestr;

  /*
   *  cbchar( uint32_t uc )
   *  Возвращает количество символов, которыми кодируется этот символ.
   */
    static  auto  cbchar( uint32_t ) -> size_t;
  /*
   *  cbchar( utfstr )
   *  Возвращает количество байт, кодирующих первый символ строки.
   */
    static  auto  cbchar( const char* str, size_t len = (size_t)-1 ) -> size_t;
  /*
   * detect( utfstr )
   * Возвращает признак того, что строка является utf8-строкой.
   */
    static  bool  detect( const char* str, size_t len = (size_t)-1 );
  /*
   * verify( utfstr )
   * Возвращает признак того, что строка может быть корректной utf8-строкой.
   */
    static  bool  verify( const char* str, size_t len = (size_t)-1 );
  /*
   * strlen( utfstr )
   * Возвращает количество символов, закодированных utf8-строкой.
   */
    static  auto  strlen( const char* str, size_t len = (size_t)-1 ) -> size_t;

  };

  class utf8
  {
    static  bool  put_char( charstr&, uint32_t );

  public:
    typedef char  chartype;
    typedef std::basic_string<chartype> string_t;
    
  protected:
    template <class _char>
    class input;
    class outptr;
    class outstr;
    class string;

    template <class _char>
    static  bool  isutf8( const _char*, size_t );
    template <class _char>
    static  auto  chsize( const _char*, size_t ) -> size_t;

  public:
    static  auto  in( const char* str, size_t len = (size_t)-1 ) -> input<char>;
    static  auto  in( const charstr& str ) -> input<char>;
    static  auto  in( const widechar* str, size_t len = (size_t)-1 ) -> input<widechar>;
    static  auto  in( const widestr& str ) -> input<widechar>;
    static  auto  in( const uint32_t* str, size_t len = (size_t)-1 ) -> input<uint32_t>;
    static  auto  in( const std::basic_string<uint32_t>& str ) -> input<uint32_t>;

    static  auto  out( chartype* str, size_t len ) -> outptr;
    static  auto  out( string_t& str ) -> outstr;
    static  auto  out() -> string;

  public:     // detectors
    static  bool  detect( const char*     pch, size_t cch )  {  return isutf8( pch, cch );  }
    static  bool  detect( const widechar* pch, size_t cch )  {  return isutf8( pch, cch );  }
    static  bool  detect( const uint32_t* pch, size_t cch )  {  return isutf8( pch, cch );  }

    static  bool  detect( const charstr&  src ) {  return detect( src.c_str(), src.length() );  }
    static  bool  detect( const widestr&  src ) {  return detect( src.c_str(), src.length() );  }
    static  bool  detect( const std::basic_string<uint32_t>&  src ) {  return detect( src.c_str(), src.length() );  }

  public:     // encoders
    static  auto  encode( char* out, size_t len, uint32_t chr ) -> size_t;
    static  auto  encode( charstr&, uint32_t chr ) -> charstr&;
    static  auto  encode( uint32_t chr ) -> charstr;

    static  auto  encode( char* out, size_t len, const widechar* str, size_t cch = (size_t)-1 ) -> size_t;
    static  auto  encode( char* out, size_t len, const uint32_t* str, size_t cch = (size_t)-1 ) -> size_t;

    static  auto  encode( charstr&, const widechar* str, size_t cch = (size_t)-1 ) -> charstr&;
    static  auto  encode( charstr&, const uint32_t* str, size_t cch = (size_t)-1 ) -> charstr&;

    static  auto  encode( charstr&, const widestr& ) -> charstr&;
    static  auto  encode( charstr&, const std::basic_string<uint32_t>& ) -> charstr&;

    static  auto  encode( const widechar* str, size_t cch = (size_t)-1 ) -> charstr;
    static  auto  encode( const uint32_t* str, size_t cch = (size_t)-1 ) -> charstr;

    static  auto  encode( const widestr& ) -> charstr;
    static  auto  encode( const std::basic_string<uint32_t>& ) -> charstr;

  public:
    static  auto  charsize( uint32_t uch ) -> size_t;
    static  auto  charsize( const char* str, size_t len = (size_t)-1 ) -> size_t  {  return chsize( str, len );  }
    static  auto  charsize( const widechar* str, size_t len = (size_t)-1 ) -> size_t  {  return chsize( str, len );  }
    static  auto  charsize( const uint32_t* str, size_t len = (size_t)-1 ) -> size_t  {  return chsize( str, len );  }

  };

  class utf16
  {
    static  widechar  upper_16( uint32_t u )  {  return 0xd800 | ((u - 0x10000) >> 10);  }
    static  widechar  lower_16( uint32_t u )  {  return 0xdc00 | ((u & 0x03ff));  }

    static  bool  put_char( widestr&, uint32_t );

  public:
    typedef widechar  chartype;
    typedef std::basic_string<chartype> string_t;
    
  protected:
    class input;
    class outptr;
    class outstr;
    class string;

  public:
    static  auto  in( const chartype* str, size_t len = (size_t)-1 ) -> input;
    static  auto  in( const string_t& str ) -> input;

    static  auto  out( chartype* str, size_t len ) -> outptr;
    static  auto  out( string_t& str ) -> outstr;
    static  auto  out() -> string;

  public:     // encoders
    static  auto  encode( chartype* out, size_t len, uint32_t chr ) -> size_t;
    static  auto  encode( string_t&, uint32_t chr ) -> string_t&;
    static  auto  encode( uint32_t chr ) -> string_t;

    static  auto  encode( chartype* out, size_t len, const char* str, size_t cch = (size_t)-1 ) -> size_t;
    static  auto  encode( chartype* out, size_t len, const uint32_t* str, size_t cch = (size_t)-1 ) -> size_t;

    static  auto  encode( string_t&, const char* str, size_t cch = (size_t)-1 ) -> string_t&;
    static  auto  encode( string_t&, const uint32_t* str, size_t cch = (size_t)-1 ) -> string_t&;

    static  auto  encode( string_t&, const utf8::string_t& ) -> string_t&;
    static  auto  encode( string_t&, const std::basic_string<uint32_t>& ) -> string_t&;

    static  auto  encode( const char* str, size_t cch = (size_t)-1 ) -> string_t;
    static  auto  encode( const uint32_t* str, size_t cch = (size_t)-1 ) -> string_t;

    static  auto  encode( const utf8::string_t& ) -> string_t;
    static  auto  encode( const std::basic_string<uint32_t>& ) -> string_t;

  public:     // expand from utf8/utf32
    static  auto  expand( const char*, size_t = (size_t)-1 ) -> string_t;
    static  auto  expand( const charstr& mb_str ) -> string_t;

  public:
    static  auto  charsize( uint32_t uch ) -> size_t  {  return uch >= 0x01ffff ? 2 : 1;  }

  };

  class utf32
  {
  public:
    typedef uint32_t  chartype;
    typedef std::basic_string<chartype> string_t;
    
  protected:
    class input;
    class outptr;
    class outstr;
    class string;

  public:
    static  auto  in( const chartype* str, size_t len = (size_t)-1 ) -> input;
    static  auto  in( const string_t& str ) -> input;

    static  auto  out( chartype* str, size_t len ) -> outptr;
    static  auto  out( string_t& str ) -> outstr;
    static  auto  out() -> string;

  public:     // encode family
    static  auto  encode( chartype* out, size_t len, uint32_t chr ) -> size_t;
    static  auto  encode( string_t&, uint32_t chr ) -> string_t&;
    static  auto  encode( uint32_t chr ) -> string_t;

    static  auto  encode( chartype* out, size_t len, const char* str, size_t cch = (size_t)-1 ) -> size_t;
    static  auto  encode( chartype* out, size_t len, const widechar* str, size_t cch = (size_t)-1 ) -> size_t;

    static  auto  encode( string_t&, const char* str, size_t cch = (size_t)-1 ) -> string_t&;
    static  auto  encode( string_t&, const widechar* str, size_t cch = (size_t)-1 ) -> string_t&;

    static  auto  encode( string_t&, const utf8::string_t& ) -> string_t&;
    static  auto  encode( string_t&, const utf16::string_t& ) -> string_t&;

    static  auto  encode( const char* str, size_t cch = (size_t)-1 ) -> string_t;
    static  auto  encode( const widechar* str, size_t cch = (size_t)-1 ) -> string_t;

    static  auto  encode( const utf8::string_t& ) -> string_t;
    static  auto  encode( const utf16::string_t& ) -> string_t;

  public:
    static  auto  charsize( uint32_t ) -> size_t  {  return 1;  }

  };

  template <class _char>
  class utf8::input
  {
    using uchar = typename std::make_unsigned<_char>::type;

    const _char* src;
    const _char* end;

  public:
    input( const _char*, size_t );
    input( const std::basic_string<_char>& str ): src( str.c_str() ), end( str.c_str() + str.length() ) {}
    input( const input& in ): src( in.src ), end( in.end ) {}

  public:
    bool  get_char( uint32_t& uch );
    auto  get_size() const -> size_t  {  return end - src;  }

  };

  class utf8::outptr
  {
    chartype* org;
    chartype* out;
    chartype* end;

  public:
    typedef size_t  res_type;

  public:
    outptr( chartype* s, size_t l ): org( s ), out( s ), end( s + l ) {}
    outptr( const outptr& o ): org( o.org ), out( o.out ), end( o.end ) {}

  public:
    bool      put_char( uint32_t chr );
    res_type  finalize() const  {  return end != nullptr ? out - org : (size_t)-1;  }

  };

  class utf8::outstr
  {
    string_t&  str;

  public:
    typedef string_t& res_type;

  public:
    outstr( string_t& s ): str( s ) {}
    outstr( const outstr& o ): str( o.str ) {}

  public:
    bool      put_char( uint32_t chr )  {  return utf8::put_char( str, chr );  }
    res_type  finalize() {  return str;  }

  };

  class utf8::string: protected string_t
  {
  public:
    typedef string_t  res_type;

  public:
    string() = default;
    string( string&& s ): string_t( std::move( s ) ) {}

  public:
    bool      put_char( uint32_t chr )  {  return utf8::put_char( *this, chr );  }
    res_type  finalize() {  return std::move( *this );  }

  };

  class utf16::input
  {
    const chartype* src;
    const chartype* end;

  public:
    input( const chartype*, size_t );
    input( const string_t& str ): src( str.c_str() ), end( str.c_str() + str.length() ) {}
    input( const input& in ): src( in.src ), end( in.end ) {}

  public:
    bool  get_char( uint32_t& uch );
    auto  get_size() const -> size_t  {  return end - src;  }

  };

  class utf16::outptr
  {
    chartype* org;
    chartype* out;
    chartype* end;

  public:
    typedef size_t  res_type;

  public:
    outptr( chartype* s, size_t l ): org( s ), out( s ), end( s + l ) {}
    outptr( const outptr& o ): org( o.org ), out( o.out ), end( o.end ) {}

  public:
    bool      put_char( uint32_t chr );
    res_type  finalize() const  {  return end != nullptr ? out - org : (size_t)-1;  }

  };

  class utf16::outstr
  {
    string_t&  str;

  public:
    typedef string_t& res_type;

  public:
    outstr( string_t& s ): str( s ) {}
    outstr( const outstr& o ): str( o.str ) {}

  public:
    bool      put_char( uint32_t chr )  {  return utf16::put_char( str, chr );  }
    res_type  finalize() {  return str;  }

  };

  class utf16::string: protected string_t
  {
  public:
    typedef string_t  res_type;

  public:
    string() = default;
    string( string&& s ): string_t( std::move( s ) ) {}

  public:
    bool      put_char( uint32_t chr )  {  return utf16::put_char( *this, chr );  }
    res_type  finalize() {  return std::move( *this );  }

  };

  class utf32::input
  {
    const chartype* src;
    const chartype* end;

  public:
    input( const chartype*, size_t );
    input( const string_t& s ): src( s.c_str() ), end( s.c_str() + s.length() ) {}
    input( const input& in ): src( in.src ), end( in.end ) {}

  public:
    bool  get_char( uint32_t& uch ) {  return src != end ? (uch = *src++, true) : false;  }
    auto  get_size() const -> size_t  {  return end - src;  }

  };

  class utf32::outptr
  {
    chartype* org;
    chartype* out;
    chartype* end;

  public:
    typedef size_t  res_type;

  public:
    outptr( chartype* s, size_t l ): org( s ), out( s ), end( s + l ) {}
    outptr( const outptr& o ): org( o.org ), out( o.out ), end( o.end ) {}

  public:
    bool      put_char( uint32_t chr );
    res_type  finalize() const  {  return end != nullptr ? out - org : (size_t)-1;  }

  };

  class utf32::outstr
  {
    string_t&  str;

  public:
    typedef string_t& res_type;

  public:
    outstr( string_t& s ): str( s ) {}
    outstr( const outstr& o ): str( o.str ) {}

  public:
    bool      put_char( uint32_t chr )  {  return str.push_back( chr ), true;  }
    res_type  finalize() {  return str;  }

  };

  class utf32::string: protected string_t
  {
  public:
    typedef string_t  res_type;

  public:
    string() = default;
    string( string&& s ): string_t( std::move( s ) ) {}

  public:
    bool      put_char( uint32_t chr )  {  return push_back( chr ), true;  }
    res_type  finalize() {  return std::move( *this );  }

  };

  // utf8::input inline implementation

  template <class _char>
  utf8::input<_char>::input( const _char* str, size_t len ): src( str )
    {
      if ( len == (size_t)-1 )
        for ( len = 0; src[len] != 0; ++len ) (void)NULL;
      end = str + len;
    }

  template <class _char>
  bool  utf8::input<_char>::get_char( uint32_t& uch )
    {
      if ( src >= end )
        return false;

    // on non-utf strings, throw logic_error as impossible call
      if ( ((uchar)*this->src & ~0x0ff) != 0 )
        throw std::logic_error( "invalid source string for utf-8 conversion" );

      if ( ((uch = (uchar)*this->src++) & 0x80) != 0 )
      {
        if ( (uch & 0xe0) == 0xc0 )
        {
          if ( this->src >= this->end )
            throw std::logic_error( "invalid source for utf conversion" );
          uch = ((uch & 0x1f) << 6) | (((uchar)*this->src++) & 0x3f);
        }
          else
        if ( (uch & 0xf0) == 0xe0 )
        {
          if ( this->src + 1 >= this->end )
            throw std::logic_error( "invalid source for utf conversion" );
          uch = ((uch & 0x0f) << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
        }
          else
        if ( (uch & 0xf8) == 0xf0 )
        {
          if ( this->src + 2 >= this->end )
            throw std::logic_error( "invalid source for utf conversion" );
          uch = ((uch & 0x07) << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
        }
          else
        if ( (uch & 0xfc) == 0xf8 )
        {
          if ( this->src + 3 >= this->end )
            throw std::logic_error( "invalid source for utf conversion" );
          uch = ((uch & 0x03) << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
        }
          else
        if ( (uch & 0xfe) == 0xfc )
        {
          if ( this->src + 3 >= this->end )
            throw std::logic_error( "invalid source for utf conversion" );
          uch = ((uch & 0x01) << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
          uch = (uch << 6) | (((uchar)*this->src++) & 0x3f);
        }
      }

      return true;
    }

  // utf8::outptr inline implementation

  inline  bool  utf8::outptr::put_char( uint32_t uch )
    {
      if ( (uch & ~0x007f) == 0 )
      {
        if ( out != end ) *out++ = (char)uch;
          else return false;
      }
        else
      if ( (uch & ~0x07ff) == 0 )
      {
        if ( out >= end - 1 )
          return false;
        *out++ = (char)(0xC0 | (unsigned char)((uch >> 0x06) & 0x3f));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3f));
      }
        else
      if ( (uch & ~0x0ffff) == 0 )
      {
        if ( out >= end - 2 )
          return false;
        *out++ = (char)(0xE0 | (unsigned char)((uch >> 0x0c) & 0x0f));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x06) & 0x3F));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3F));
      }
        else
      if ( uch <= 0x10ffff )
      {
        if ( out >= end - 3 )
          return false;
        *out++ = (char)(0xf0 | (unsigned char)((uch >> 0x12) & 0x07));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x0c) & 0x3F));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x06) & 0x3F));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3F));
      }
        else
      return false;

      if ( out != end )
        *out = 0;

      return true;
    }

  // utf8::outstr inline implementation

  // utf8 inline impl

  inline  bool  utf8::put_char( string_t& str, uint32_t uch )
    {
      if ( (uch & ~0x007f) == 0 )
        return  str.push_back( (char)uch ), true;

      if ( (uch & ~0x07ff) == 0 )
        return  str.push_back( (char)(0xC0 | (unsigned char)((uch >> 0x06) & 0x3f)) ),
                str.push_back( (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3f)) ), true;

      if ( (uch & ~0x0ffff) == 0 )
        return  str.push_back( (char)(0xE0 | (unsigned char)((uch >> 0x0c) & 0x0f)) ),
                str.push_back( (char)(0x80 | (unsigned char)((uch >> 0x06) & 0x3F)) ),
                str.push_back( (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3F)) ), true;

      if ( uch <= 0x10ffff )
      {
        return  str.push_back( (char)(0xf0 | (unsigned char)((uch >> 0x12) & 0x07)) ),
                str.push_back( (char)(0x80 | (unsigned char)((uch >> 0x0c) & 0x3F)) ),
                str.push_back( (char)(0x80 | (unsigned char)((uch >> 0x06) & 0x3F)) ),
                str.push_back( (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3F)) ), true;
      }

      return false;
    }

  /*
  * detect( pch, cch )
  * возвращает true, если строка однозначно является utf8-кодом без ошибочного кодирования.
  */
  template <class _char>
  bool  utf8::isutf8( const _char* pch, size_t cch )
    {
      using uchar = typename std::make_unsigned<_char>::type;

      auto  end = pch;
      auto  upp = false;

      if ( cch == (size_t)-1 )  for ( ; *end != 0; ++end )  (void)NULL;
        else end += cch;

      while ( pch != end )
      {
        auto  chnext = (uchar)*pch++;

        if ( (chnext & ~0x0ff) != 0 )   // upper characters are not parts of utf conversion
          return false;
        if ( (chnext & ~0x07f) != 0 )   // check for utf character sequences
        {
          auto  nadded = (chnext & 0xe0) == 0xc0 ? 1 :
                         (chnext & 0xf0) == 0xe0 ? 2 :
                         (chnext & 0xf8) == 0xf0 ? 3 :
                         (chnext & 0xfc) == 0xf8 ? 4 :
                         (chnext & 0xfe) == 0xfc ? 5 :
                         (chnext & 0xff) == 0xfe ? 6 : 0;

          while ( nadded > 0 && pch != end )
            if ( (*pch++ & 0xc0) != 0x80 )  return false;
              else --nadded;

          if ( nadded == 0 )  upp = true;
            else return false;
        }
      }
      return upp;
    }

  template <class _char>
  auto  utf8::chsize( const _char* pch, size_t cch ) -> size_t
    {
      using uchar = typename std::make_unsigned<_char>::type;

      auto    utforg = pch;
      uchar   chnext;
      size_t  nleast;

    // check for length
      if ( pch == nullptr )
        return 0;
      if ( cch == (size_t)-1 )
        for ( cch = 0; pch[cch] != 0; ++cch ) (void)NULL;
      if ( cch == 0 )
        return 0;

    // check if 7-bit character
      if ( ((chnext = (uchar)*pch++) & 0x80) == 0 )
        return 1;

    // check the number of bytes in the byte sequence if pch symbol
      if ( (chnext & 0xe0) == 0xc0 )  nleast = 1;
        else
      if ( (chnext & 0xf0) == 0xe0 )  nleast = 2;
        else
      if ( (chnext & 0xf8) == 0xf0 )  nleast = 3;
        else
      if ( (chnext & 0xfc) == 0xf8 )  nleast = 4;
        else
      if ( (chnext & 0xfe) == 0xfc )  nleast = 5;
        else
      return 1;

    // check the length of a least sequence
      if ( cch < nleast + 1 )
        return 0;

    // check least bits
      while ( nleast-- > 0 )
        if ( (*pch & 0xC0) != 0x80 ) return 0;
          else ++pch;

      return pch - utforg;
    }

  inline  auto  utf8::in( const char* str, size_t len ) -> input<char> {  return input<char>( str, len );  }
  inline  auto  utf8::in( const charstr& str ) -> input<char> {  return input<char>( str );  }
  inline  auto  utf8::in( const widechar* str, size_t len ) -> input<widechar> {  return input<widechar>( str, len );  }
  inline  auto  utf8::in( const widestr& str ) -> input<widechar> {  return input<widechar>( str );  }
  inline  auto  utf8::in( const uint32_t* str, size_t len ) -> input<uint32_t> {  return input<uint32_t>( str, len );  }
  inline  auto  utf8::in( const std::basic_string<uint32_t>& str ) -> input<uint32_t> {  return input<uint32_t>( str );  }

  inline  auto  utf8::out( char* str, size_t len ) -> outptr  {  return outptr( str, len );  }
  inline  auto  utf8::out( charstr& str ) -> outstr {  return outstr( str );  }
  inline  auto  utf8::out() -> string {  return string();  }

  inline  auto  utf8::encode( char* psz, size_t len, uint32_t chr ) -> size_t
    {  return utf::encode( out( psz, len ), chr );  }
  inline  auto  utf8::encode( charstr& str, uint32_t chr ) -> charstr&
    {  return utf::encode( out( str ), chr );  }
  inline  auto  utf8::encode( uint32_t chr ) -> charstr
    {
      auto  str = charstr();
      return std::move( utf::encode( out( str ), chr ) );
    }

  inline  auto  utf8::encode( char* out, size_t len, const widechar* str, size_t cch ) -> size_t
    {  return utf::encode( utf8::out( out, len ), utf16::in( str, cch ) );  }
  inline  auto  utf8::encode( char* out, size_t len, const uint32_t* str, size_t cch ) -> size_t
    {  return utf::encode( utf8::out( out, len ), utf32::in( str, cch ) );  }

  inline  auto  utf8::encode( charstr& out, const widechar* str, size_t cch ) -> charstr&
    {  return utf::encode( utf8::out( out ), utf16::in( str, cch ) );  }
  inline  auto  utf8::encode( charstr& out, const uint32_t* str, size_t cch ) -> charstr&
    {  return utf::encode( utf8::out( out ), utf32::in( str, cch ) );  }

  inline  auto  utf8::encode( charstr& out, const widestr& str ) -> charstr&
    {  return utf::encode( utf8::out( out ), utf16::in( str ) );  }
  inline  auto  utf8::encode( charstr& out, const std::basic_string<uint32_t>& str ) -> charstr&
    {  return utf::encode( utf8::out( out ), utf32::in( str ) );  }

  inline  auto  utf8::encode( const widechar* str, size_t cch ) -> charstr
    {
      auto  out = charstr();
      return std::move( utf::encode( utf8::out( out ), utf16::in( str, cch ) ) );
    }
  inline  auto  utf8::encode( const uint32_t* str, size_t cch ) -> charstr
    {
      auto  out = charstr();
      return std::move( utf::encode( utf8::out( out ), utf32::in( str, cch ) ) );
    }

  inline  auto  utf8::encode( const widestr& str ) -> charstr
    {  return encode( str.c_str(), str.length() );  }
  inline  auto  utf8::encode( const std::basic_string<uint32_t>& str ) -> charstr
    {  return encode( str.c_str(), str.length() );  }

  inline  auto  utf8::charsize( uint32_t uch ) -> size_t
    {
      return  uch <= 0x0000007f ? 1 :
              uch <= 0x000007ff ? 2 :
              uch <= 0x0000ffff ? 3 :
              uch <= 0x001fffff ? 4 :
              uch <= 0x03ffffff ? 5 :
              uch <= 0x7fffffff ? 6 : 7;
    }

  // utf16::input inline implementation

  inline  utf16::input::input( const widechar* str, size_t len ): src( str )
    {
      if ( len == (size_t)-1 )
        for ( len = 0; src[len] != 0; ++len ) (void)NULL;
      end = str + len;
    }

  inline  bool  utf16::input::get_char( uint32_t& uch )
    {
      if ( src != end ) uch = *src++;
        else return false;
      if ( uch >= 0xd800 && uch <= 0xdfff && src != end )
        uch = 0x10000 + ((uch & 0x07ff) << 10) + (*src++ & 0x3ff);
      return true;
    }

  // utf16::outptr inline implementation

  inline  bool  utf16::outptr::put_char( uint32_t chr )
    {
      if ( chr <= 0x0ffff )
        return out < end ? (*out++ = chr, true) : (end = nullptr, false);

      if ( out + 1 >= end )
        return (end = nullptr, false);
              
      *out++ = upper_16( chr );
      *out++ = lower_16( chr );

      if ( out != end )
        *out = 0;
            
      return true;
    }

  // utf16 inline implementation

  inline  bool  utf16::put_char( widestr& str, uint32_t chr )
    {
      if ( chr > 0x0ffff )
        return str.push_back( upper_16( chr ) ), str.push_back( lower_16( chr ) ), true;
      else
        return str.push_back( (chartype)chr ), true;
    }

  inline  auto  utf16::in( const chartype* str, size_t len ) -> input {  return input( str, len );  }
  inline  auto  utf16::in( const string_t& str ) -> input  {  return input( str );  }
  inline  auto  utf16::out( chartype* str, size_t len ) -> outptr {  return outptr( str, len );  }
  inline  auto  utf16::out( string_t& str ) -> outstr  {  return outstr( str );  }
  inline  auto  utf16::out() -> string  {  return string();  }

  inline  auto  utf16::encode( chartype* out, size_t len, uint32_t chr ) -> size_t
    {  return utf::encode( utf16::out( out, len ), chr );  }
  inline  auto  utf16::encode( string_t& str, uint32_t chr ) -> string_t&
    {  return utf::encode( utf16::out( str ), chr );  }
  inline  auto  utf16::encode( uint32_t chr ) -> string_t
    {  return utf::encode( utf16::out(), chr );  }

  inline  auto  utf16::encode( chartype* out, size_t len, const char* str, size_t cch ) -> size_t
    {  return utf::encode( utf16::out( out, len ), utf8::in( str, cch ) );  }
  inline  auto  utf16::encode( chartype* out, size_t len, const uint32_t* str, size_t cch ) -> size_t
    {  return utf::encode( utf16::out( out, len ), utf32::in( str, cch ) );  }

  inline  auto  utf16::encode( string_t& out, const char* str, size_t cch ) -> string_t&
    {  return utf::encode( utf16::out( out ), utf8::in( str, cch ) );  }
  inline  auto  utf16::encode( string_t& out, const uint32_t* str, size_t cch ) -> string_t&
    {  return utf::encode( utf16::out( out ), utf32::in( str, cch ) );  }

  inline  auto  utf16::encode( string_t& out, const utf8::string_t& str ) -> string_t&
    {  return utf::encode( utf16::out( out ), utf8::in( str ) );  }
  inline  auto  utf16::encode( string_t& out, const std::basic_string<uint32_t>& src ) -> string_t&
    {  return utf::encode( utf16::out( out ), utf32::in( src ) );  }

  inline  auto  utf16::encode( const char* str, size_t cch ) -> string_t
    {  return utf::encode( utf16::out(), utf8::in( str, cch ) );  }
  inline  auto  utf16::encode( const uint32_t* str, size_t cch ) -> string_t
    {  return utf::encode( utf16::out(), utf32::in( str, cch ) );  }

  inline  auto  utf16::encode( const utf8::string_t& str ) -> string_t
    {  return utf::encode( utf16::out(), utf8::in( str ) );  }
  inline  auto  utf16::encode( const std::basic_string<uint32_t>& str ) -> string_t
    {  return utf::encode( utf16::out(), utf32::in( str ) );  }

  inline  auto  utf16::expand( const char* str, size_t len ) -> string_t
    {
      widestr out;

      if ( len == (size_t)-1 )
        for ( len = 0; str[len] != 0; ++len ) (void)NULL;

      out.resize( len );

      for ( size_t pos = 0; pos != len; ++pos )
        out[pos] = (chartype)(unsigned char)*str++;

      return out;
    }
  inline  auto  utf16::expand( const charstr& str ) -> string_t
    {  return expand( str.c_str(), str.length() );  }

  // utf32::outptr inline implementation

  inline  bool  utf32::outptr::put_char( uint32_t chr )
    {
      if ( out >= end )  return false;
        else *out++ = chr;

      if ( out < end )
        *out = 0;

      return true;
    }

  // utf32 inline implementation

  inline  auto  utf32::in( const chartype* str, size_t len ) -> input {  return input( str, len );  }
  inline  auto  utf32::in( const string_t& str ) -> input  {  return input( str );  }
  inline  auto  utf32::out( chartype* str, size_t len ) -> outptr {  return outptr( str, len );  }
  inline  auto  utf32::out( string_t& str ) -> outstr  {  return outstr( str );  }
  inline  auto  utf32::out() -> string  {  return string();  }

  inline  auto  utf32::encode( chartype* out, size_t len, uint32_t chr ) -> size_t
    {  return utf::encode( utf32::out( out, len ), chr );  }
  inline  auto  utf32::encode( string_t& str, uint32_t chr ) -> string_t&
    {  return utf::encode( utf32::out( str ), chr );  }
  inline  auto  utf32::encode( uint32_t chr ) -> string_t
    {  return utf::encode( utf32::out(), chr );  }

  inline  auto  utf32::encode( chartype* out, size_t len, const char* str, size_t cch ) -> size_t
    {  return utf::encode( utf32::out( out, len ), utf8::in( str, cch ) );  }
  inline  auto  utf32::encode( chartype* out, size_t len, const widechar* str, size_t cch ) -> size_t
    {  return utf::encode( utf32::out( out, len ), utf16::in( str, cch ) );  }

  inline  auto  utf32::encode( string_t& out, const char* str, size_t cch ) -> string_t&
    {  return utf::encode( utf32::out( out ), utf8::in( str, cch ) );  }
  inline  auto  utf32::encode( string_t& out, const widechar* str, size_t cch ) -> string_t&
    {  return utf::encode( utf32::out( out ), utf16::in( str, cch ) );  }

  inline  auto  utf32::encode( string_t& out, const utf8::string_t& str ) -> string_t&
    {  return utf::encode( utf32::out( out ), utf8::in( str ) );  }
  inline  auto  utf32::encode( string_t& out, const utf16::string_t& src ) -> string_t&
    {  return utf::encode( utf32::out( out ), utf16::in( src ) );  }

  inline  auto  utf32::encode( const char* str, size_t cch ) -> string_t
    {  return utf::encode( utf32::out(), utf8::in( str, cch ) );  }
  inline  auto  utf32::encode( const widechar* str, size_t cch ) -> string_t
    {  return utf::encode( utf32::out(), utf16::in( str, cch ) );  }

  inline  auto  utf32::encode( const utf8::string_t& str ) -> string_t
    {  return utf::encode( utf32::out(), utf8::in( str ) );  }
  inline  auto  utf32::encode( const utf16::string_t& str ) -> string_t
    {  return utf::encode( utf32::out(), utf16::in( str ) );  }

  // utf inline implementation

  template <class target, class in>
  auto  utf::strlen( const target&, in src ) -> size_t
    {
      uint32_t  chr;
      size_t    len = 0;

      while ( src.get_char( chr ) )
        len += target::charsize( chr );

      return len;
    }
  template <class target>
  auto  utf::encode( target out, uint32_t chr ) -> typename target::res_type
    {
      out.put_char( chr );
      return out.finalize();
    }
  template <class target, class in>
  auto  utf::encode( target out, in src ) -> typename target::res_type
    {
      using     res = typename target::res_type;
      uint32_t  chr;

      while ( src.get_char( chr ) && out.put_char( chr ) )
        (void)NULL;

      return (typename std::conditional<std::is_reference<res>::value, res, res&&>::type)(out.finalize());
    }

  inline  auto  utf::strlen( const utf8&  out, const char* str, size_t len )     -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf8&  out, const widechar* str, size_t len ) -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf8&  out, const uint32_t* str, size_t len ) -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf16& out, const char* str, size_t len )     -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf16& out, const widechar* str, size_t len ) -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf16& out, const uint32_t* str, size_t len ) -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf32& out, const char* str, size_t len )     -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf32& out, const widechar* str, size_t len ) -> size_t {  return strlen( out, utf8::in( str, len ) );  }
  inline  auto  utf::strlen( const utf32& out, const uint32_t* str, size_t len ) -> size_t {  return strlen( out, utf8::in( str, len ) );  }

  // compatibility family

  inline  auto  utf::encode( char* out, size_t len, uint32_t wch ) -> size_t
    {  return encode( utf8::out( out, len ), wch );  }
  inline  auto  utf::encode( char* out, size_t len, const widechar* src, size_t cch ) -> size_t
    {  return encode( utf8::out( out, len ), utf16::in( src, cch ) );  }
  inline  auto  utf::encode( const widechar* str, size_t len ) -> charstr
    {  return utf8::encode( str, len );  }
  inline  auto  utf::encode( const widestr& str ) -> charstr
    {  return utf8::encode( str );  }

  inline  auto  utf::decode( widechar* out, size_t len, const char* src, size_t cch ) -> size_t
    {  return encode( utf16::out( out, len ), utf8::in( src, cch ) );  }
  inline  auto  utf::decode( const char* str, size_t len ) -> widestr
    {  return utf16::encode( str, len );  }
  inline  auto  utf::decode( const charstr& str ) -> widestr
    {  return utf16::encode( str );  }

  inline  auto  utf::cbchar( uint32_t uch ) -> size_t                     {  return utf8::charsize( uch );  }
  inline  auto  utf::cbchar( const char* str, size_t  len ) -> size_t     {  return utf8::charsize( str, len );  }

  inline  bool  utf::detect( const char* str, size_t len )                {  return utf8::detect( str, len );  }
  inline  bool  utf::verify( const char* str, size_t len )                {  return utf8::detect( str, len );  }

  inline  auto  utf::strlen( const char* str, size_t len ) -> size_t      {  return utf::strlen( utf16(), str, len );  }

}

# endif   // !__mtc_utf_hpp__
