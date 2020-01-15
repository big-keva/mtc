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

  class utf16
  {
    friend class utf8;
    friend class utf32;

  public:
    typedef widechar  chartype;
    typedef std::basic_string<chartype> string_t;

  };

  class utf32
  {
    friend class utf8;
    friend class utf16;

  public:
    typedef uint32_t  chartype;
    typedef std::basic_string<chartype> string_t;

  };

  class utf8
  {
    friend class utf16;
    friend class utf32;

    static  widechar  upper_16( uint32_t u )  {  return 0xd800 | ((u - 0x10000) >> 10);  }
    static  widechar  lower_16( uint32_t u )  {  return 0xdc00 | ((u & 0x03ff));  }
    template <class chartype>
    static  size_t    checklen( const chartype* str, size_t len, size_t lim = (size_t)-1 )
      {
        if ( len == (size_t)-1 )
          for ( len = 0; str[len] != 0 && len < lim; ++len )  (void)NULL;
        return len;
      }
    template <class chartype>
    static  size_t    strlen16( const chartype* utf, size_t len )
      {
        auto      cch = 0;
        widechar  buf[2];

        for ( auto end = utf + checklen( utf, len ); utf != end; )
        {
          auto  ccu = charsize( utf, end - utf );
            cch += decode( buf, 2, utf, ccu );
          utf += ccu;
        }
      
        return cch;
      }
    template <class chartype>
    static  size_t    strlen32( const chartype* utf, size_t len )
      {
        auto      cch = 0;

        for ( auto end = utf + checklen( utf, len ); utf != end; ++cch )
          utf += charsize( utf, end - utf );
      
        return cch;
      }

   /*
    * detect( pch, cch )
    * возвращает true, если строка однозначно является utf8-кодом без ошибочного кодирования.
    */
    template <class chartype>
    static  bool  detectit( const chartype* pch, size_t cch )
      {
        using uchar = typename std::make_unsigned<chartype>::type;

        const chartype* end = pch;
        bool            upp = false;

        if ( cch == (size_t)-1 )  for ( ; *end != 0; ++end )  (void)NULL;
          else end += cch;

        while ( pch != end )
        {
          auto  chnext = (uchar)*pch++;

          if ( (chnext & ~0x0ff) != 0 )   // upper characters are not parts of utf conversion
            continue;
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

            if ( nadded != 0 )  return false;
              else upp = true;
          }
        }
        return upp;
      }

  protected:  // decoders helpers
    template <class uccstype, class chartype>
    static  auto  decodeit( uccstype* out, size_t cwb, const chartype* pch, size_t cch ) -> size_t
      {
        using uchar = typename std::make_unsigned<chartype>::type;

        auto  org = out;
        auto  lim = out + cwb;
        auto  mod = out;

        for ( auto end = pch + checklen( pch, cch ); pch != end && out != lim; )
        {
          uint32_t  ucchar;

        // on non-utf strings, throw logic_error as impossible call
          if ( ((uchar)*pch & ~0x0ff) != 0 )
            throw std::logic_error( "invalid source string for utf-8 conversion" );

          if ( ((ucchar = (uchar)*pch++) & 0x80) != 0 )
          {
            if ( (ucchar & 0xe0) == 0xc0 )
            {
              if ( pch >= end )
                throw std::logic_error( "invalid source for utf conversion" );
              ucchar = ((ucchar & 0x1f) << 6) | (((uchar)*pch++) & 0x3f);
            }
              else
            if ( (ucchar & 0xf0) == 0xe0 )
            {
              if ( pch + 1 >= end )
                throw std::logic_error( "invalid source for utf conversion" );
              ucchar = ((ucchar & 0x0f) << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
            }
              else
            if ( (ucchar & 0xf8) == 0xf0 )
            {
              if ( pch + 2 >= end )
                throw std::logic_error( "invalid source for utf conversion" );
              ucchar = ((ucchar & 0x07) << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
            }
              else
            if ( (ucchar & 0xfc) == 0xf8 )
            {
              if ( pch + 3 >= end )
                throw std::logic_error( "invalid source for utf conversion" );
              ucchar = ((ucchar & 0x03) << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
            }
              else
            if ( (ucchar & 0xfe) == 0xfc )
            {
              if ( pch + 3 >= end )
                throw std::logic_error( "invalid source for utf conversion" );
              ucchar = ((ucchar & 0x01) << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
              ucchar = (ucchar << 6) | (((uchar)*pch++) & 0x3f);
            }
          }

        // check if output unicode value is simple or extended
          if ( (mod = output( out, lim, ucchar )) == nullptr )  break;
            else out = mod;
        }
        
        return out - org;
      }

    template <class string_t, class chartype>
    static  auto  decodeit( string_t& out, const chartype* pch, size_t cch ) -> string_t&
      {
        out.resize( 2 * (cch = checklen( pch, cch )) );
        out.resize( decode( (typename string_t::pointer)out.c_str(), out.length(), pch, cch ) );
        out.shrink_to_fit();

        return out;
      }

  protected:  // encoders helpers
    template <class chartype>
    static  auto  encodeit( char* out, size_t cwb, const chartype* pch, size_t cch ) -> size_t
      {
        auto  org = out;
        auto  lim = out + cwb;

        if ( cch == (size_t)-1 )
          for ( cch = 0; pch[cch] != 0; ++cch )  (void)NULL;

        for ( auto end = pch + cch; pch != end && out != lim; )
        {
          size_t  enc;

          if ( (enc = encode( out, lim - out, ennext( pch, end ) )) != (size_t)-1 )  out += enc;
            else return enc;
        }

        if ( out != lim )
          *out = '\0';

        return out - org;
      }

    template <class chartype>
    static  auto  encodeit( const chartype* pch, size_t cch ) -> charstr
      {
        auto  out = charstr();

        if ( cch == (size_t)-1 )
          for ( cch = 0; pch[cch] != 0; ++cch ) (void)NULL;

        out.resize( cch * 7 );
        out.resize( encode( (char*)out.c_str(), out.length(), pch, cch ) );
        out.shrink_to_fit();

        return std::move( out );
      }

    template <class chartype>
    static  size_t  charsize( const chartype* utf, size_t len )
      {
        using uchar = typename std::make_unsigned<chartype>::type;

        auto    utforg = utf;
        uchar   chnext;
        size_t  nleast;

      // check for length
        if ( utf == nullptr || (len = checklen( utf, len, 8 )) == 0 )
          return 0;

      // check if 7-bit character
        if ( ((chnext = (uchar)*utf++) & 0x80) == 0 )
          return 1;

      // check the number of bytes in the byte sequence if utf symbol
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
        if ( len < nleast + 1 )
          return 0;

      // check least bits
        while ( nleast-- > 0 )
          if ( (*utf & 0xC0) != 0x80 ) return 0;
            else ++utf;

        return utf - utforg;
      }
    static  auto  output( widechar* out, widechar* end, uint32_t chr ) -> widechar*
      {
        if ( chr > 0x0ffff )
        {
          if ( out + 1 >= end )
            return nullptr;
          *out++ = upper_16( chr );
          *out++ = lower_16( chr );
        }
          else
        {
          if ( out >= end )
            return nullptr;
          *out++ = (widechar)chr;
        }
        return out;
      }
    static  auto  output( uint32_t* out, uint32_t* end, uint32_t chr ) -> uint32_t*
      {  return out != end ? (*out++ = chr, out) : nullptr;  }

  public:     // encoders
    static  auto  encode( char* out, size_t cwb, uint32_t uch ) -> size_t;

    static  auto  encode( char* out, size_t cwb, const widechar* pch, size_t cch = (size_t)-1 ) -> size_t;
    static  auto  encode( char* out, size_t cwb, const uint32_t* pch, size_t cch = (size_t)-1 ) -> size_t;

    static  auto  encode( const widechar* pch, size_t cch = (size_t)-1 ) -> charstr;
    static  auto  encode( const uint32_t* pch, size_t cch = (size_t)-1 ) -> charstr;

    static  auto  encode( const std::basic_string<widechar>& ) -> charstr;
    static  auto  encode( const std::basic_string<uint32_t>& ) -> charstr;

  public:     // decoders
    static  auto  decode( widechar* out, size_t cwb, const char*     pch, size_t = (size_t)-1 ) -> size_t;
    static  auto  decode( widechar* out, size_t cwb, const widechar* pch, size_t = (size_t)-1 ) -> size_t;
    static  auto  decode( widechar* out, size_t cwb, const uint32_t* pch, size_t = (size_t)-1 ) -> size_t;
    static  auto  decode( uint32_t* out, size_t cwb, const char*     pch, size_t = (size_t)-1 ) -> size_t;
    static  auto  decode( uint32_t* out, size_t cwb, const widechar* pch, size_t = (size_t)-1 ) -> size_t;
    static  auto  decode( uint32_t* out, size_t cwb, const uint32_t* pch, size_t = (size_t)-1 ) -> size_t;

    template <class encoding> static  auto  decode( const char*     pch, size_t = (size_t)-1 ) -> typename encoding::string_t;
    template <class encoding> static  auto  decode( const widechar* pch, size_t = (size_t)-1 ) -> typename encoding::string_t;
    template <class encoding> static  auto  decode( const uint32_t* pch, size_t = (size_t)-1 ) -> typename encoding::string_t;

    template <class encoding> static  auto  decode( const std::basic_string<char>&     ) -> typename encoding::string_t;
    template <class encoding> static  auto  decode( const std::basic_string<widechar>& ) -> typename encoding::string_t;
    template <class encoding> static  auto  decode( const std::basic_string<uint32_t>& ) -> typename encoding::string_t;

    template <> static  auto  decode<utf16>( const char*     pch, size_t cch ) -> typename utf16::string_t;
    template <> static  auto  decode<utf16>( const widechar* pch, size_t cch ) -> typename utf16::string_t;
    template <> static  auto  decode<utf16>( const uint32_t* pch, size_t cch ) -> typename utf16::string_t;
    template <> static  auto  decode<utf32>( const char*     pch, size_t cch ) -> typename utf32::string_t;
    template <> static  auto  decode<utf32>( const widechar* pch, size_t cch ) -> typename utf32::string_t;
    template <> static  auto  decode<utf32>( const uint32_t* pch, size_t cch ) -> typename utf32::string_t;

    template <> static  auto  decode<utf16>( const std::basic_string<char>&     src ) -> typename utf16::string_t;
    template <> static  auto  decode<utf16>( const std::basic_string<widechar>& src ) -> typename utf16::string_t;
    template <> static  auto  decode<utf16>( const std::basic_string<uint32_t>& src ) -> typename utf16::string_t;
    template <> static  auto  decode<utf32>( const std::basic_string<char>&     src ) -> typename utf32::string_t;
    template <> static  auto  decode<utf32>( const std::basic_string<widechar>& src ) -> typename utf32::string_t;
    template <> static  auto  decode<utf32>( const std::basic_string<uint32_t>& src ) -> typename utf32::string_t;

  public:     // detectors
    static  bool  detect( const char*     pch, size_t cch = (size_t)-1 );
    static  bool  detect( const widechar* pch, size_t cch = (size_t)-1 );
    static  bool  detect( const uint32_t* pch, size_t cch = (size_t)-1 );

    static  bool  detect( const std::basic_string<char>&     src );
    static  bool  detect( const std::basic_string<widechar>& src );
    static  bool  detect( const std::basic_string<uint32_t>& src );

  public:     // expanders & helpers
    static  auto  mbtowc( const charstr& mb_str ) -> widestr;

  public:     // size helpers
    static  auto  charsize( uint32_t uch ) -> size_t;

    static  auto  charsize( const char*     utf, size_t len = (size_t)-1 ) -> size_t;
    static  auto  charsize( const widechar* utf, size_t len = (size_t)-1 ) -> size_t;
    static  auto  charsize( const uint32_t* utf, size_t len = (size_t)-1 ) -> size_t;

  public:     // strlen
    static  auto  strlen( const utf16&, const char*     utf, size_t len ) -> size_t;
    static  auto  strlen( const utf16&, const widechar* utf, size_t len ) -> size_t;
    static  auto  strlen( const utf16&, const uint32_t* utf, size_t len ) -> size_t;

    static  auto  strlen( const utf16&, const std::basic_string<char>& ) -> size_t;
    static  auto  strlen( const utf16&, const std::basic_string<widechar>& ) -> size_t;
    static  auto  strlen( const utf16&, const std::basic_string<uint32_t>& ) -> size_t;

    static  auto  strlen( const utf32&, const char*     utf, size_t len ) -> size_t;
    static  auto  strlen( const utf32&, const widechar* utf, size_t len ) -> size_t;
    static  auto  strlen( const utf32&, const uint32_t* utf, size_t len ) -> size_t;

    static  auto  strlen( const utf32&, const std::basic_string<char>& ) -> size_t;
    static  auto  strlen( const utf32&, const std::basic_string<widechar>& ) -> size_t;
    static  auto  strlen( const utf32&, const std::basic_string<uint32_t>& ) -> size_t;

  protected:  // helpers
    static  auto  ennext( const widechar*& src, const widechar* end ) -> uint32_t
      {
        uint32_t  uch;

        if ( src < end ) uch = *src++;
          else return 0;
        if ( uch >= 0xd800 && uch <= 0xdfff && src != end )
          uch = 0x10000 + ((uch & 0x07ff) << 10) + (*src++ & 0x3ff);
        return uch;
      }
    static  auto  ennext( const uint32_t*& src, const uint32_t* end ) -> uint32_t
      {
        return src != end ? *src++ : 0;
      }

  };

  // utf inline implementation

  inline  auto  utf8::charsize( uint32_t uch ) -> size_t
    {
      return  uch <= 0x0000007f ? 1 :
              uch <= 0x000007ff ? 2 :
              uch <= 0x0000ffff ? 3 :
              uch <= 0x001fffff ? 4 :
              uch <= 0x03ffffff ? 5 :
              uch <= 0x7fffffff ? 6 : 7;
    }

  inline  auto  utf8::charsize( const char* utf, size_t len ) -> size_t
    {  return charsize<char>( utf, len );  }
  inline  auto  utf8::charsize( const widechar* utf, size_t len ) -> size_t
    {  return charsize<widechar>( utf, len );  }
  inline  auto  utf8::charsize( const uint32_t* utf, size_t len ) -> size_t
    {  return charsize<uint32_t>( utf, len );  }

  inline  auto  utf8::strlen( const utf16&, const char* utf, size_t len ) -> size_t
    {  return strlen16( utf, len );  }
  inline  auto  utf8::strlen( const utf16&, const widechar* utf, size_t len ) -> size_t
    {  return strlen16( utf, len );  }
  inline  auto  utf8::strlen( const utf16&, const uint32_t* utf, size_t len ) -> size_t
    {  return strlen16( utf, len );  }

  inline  auto  utf8::strlen( const utf16&, const std::basic_string<char>& str ) -> size_t
    {  return strlen16( str.c_str(), str.length() );  }
  inline  auto  utf8::strlen( const utf16&, const std::basic_string<widechar>& str ) -> size_t
    {  return strlen16( str.c_str(), str.length() );  }
  inline  auto  utf8::strlen( const utf16&, const std::basic_string<uint32_t>& str ) -> size_t
    {  return strlen16( str.c_str(), str.length() );  }

  inline  auto  utf8::strlen( const utf32&, const char* utf, size_t len ) -> size_t
    {  return strlen32( utf, len );  }
  inline  auto  utf8::strlen( const utf32&, const widechar* utf, size_t len ) -> size_t
    {  return strlen32( utf, len );  }
  inline  auto  utf8::strlen( const utf32&, const uint32_t* utf, size_t len ) -> size_t
    {  return strlen32( utf, len );  }

  inline  auto  utf8::strlen( const utf32&, const std::basic_string<char>& str ) -> size_t
    {  return strlen32( str.c_str(), str.length() );  }
  inline  auto  utf8::strlen( const utf32&, const std::basic_string<widechar>& str ) -> size_t
    {  return strlen32( str.c_str(), str.length() );  }
  inline  auto  utf8::strlen( const utf32&, const std::basic_string<uint32_t>& str ) -> size_t
    {  return strlen32( str.c_str(), str.length() );  }

  // encode family

  inline  auto  utf8::encode( char* out, size_t cwb, uint32_t uch ) -> size_t
    {
      auto  org = out;
      auto  end = out + cwb;

      if ( (uch & ~0x007f) == 0 )
      {
        if ( out < end )  *out++ = (char)uch;
          else return (size_t)-1;
      }
        else
      if ( (uch & ~0x07ff) == 0 )
      {
        if ( out >= end - 1 )
          return (size_t)-1;
        *out++ = (char)(0xC0 | (unsigned char)((uch >> 0x06) & 0x3f));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3f));
      }
        else
      if ( (uch & ~0x0ffff) == 0 )
      {
        if ( out >= end - 2 )
          return (size_t)-1;
        *out++ = (char)(0xE0 | (unsigned char)((uch >> 0x0c) & 0x0f));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x06) & 0x3F));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3F));
      }
        else
      if ( uch <= 0x10ffff )
      {
        if ( out >= end - 3 )
          return (size_t)-1;
        *out++ = (char)(0xf0 | (unsigned char)((uch >> 0x12) & 0x07));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x0c) & 0x3F));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x06) & 0x3F));
        *out++ = (char)(0x80 | (unsigned char)((uch >> 0x00) & 0x3F));
      }
        else
      return (size_t)-1;

      if ( out < end )
        *out = '\0';
        
      return out - org;
    }

  inline  auto  utf8::encode( char* out, size_t cwb, const widechar* pch, size_t cch ) -> size_t
    {  return encodeit( out, cwb, pch, cch );  }
  inline  auto  utf8::encode( char* out, size_t cwb, const uint32_t* pch, size_t cch ) -> size_t
    {  return encodeit( out, cwb, pch, cch );  }

  inline  auto  utf8::encode( const widechar* pch, size_t cch ) -> charstr
    {  return std::move( encodeit( pch, cch ) );  }
  inline  auto  utf8::encode( const uint32_t* pch, size_t cch ) -> charstr
    {  return std::move( encodeit( pch, cch ) );  }

  inline  auto  utf8::encode( const std::basic_string<widechar>& str ) -> charstr
    {  return std::move( encodeit( str.c_str(), str.length() ) );  }
  inline  auto  utf8::encode( const std::basic_string<uint32_t>& str ) -> charstr
    {  return std::move( encodeit( str.c_str(), str.length() ) );  }

  // decode() family

  inline  auto  utf8::decode( widechar* out, size_t cwb, const char*     pch, size_t cch ) -> size_t
    {  return decodeit( out, cwb, pch, cch );  }
  inline  auto  utf8::decode( widechar* out, size_t cwb, const widechar* pch, size_t cch ) -> size_t
    {  return decodeit( out, cwb, pch, cch );  }
  inline  auto  utf8::decode( widechar* out, size_t cwb, const uint32_t* pch, size_t cch ) -> size_t
    {  return decodeit( out, cwb, pch, cch );  }
  inline  auto  utf8::decode( uint32_t* out, size_t cwb, const char*     pch, size_t cch ) -> size_t
    {  return decodeit( out, cwb, pch, cch );  }
  inline  auto  utf8::decode( uint32_t* out, size_t cwb, const widechar* pch, size_t cch ) -> size_t
    {  return decode( out, cwb, pch, cch );  }
  inline  auto  utf8::decode( uint32_t* out, size_t cwb, const uint32_t* pch, size_t cch ) -> size_t
    {  return decodeit( out, cwb, pch, cch );  }

  template <> auto  utf8::decode<utf16>( const char*      pch, size_t cch ) -> utf16::string_t
    {
      typename utf16::string_t out;
      return std::move( decodeit( out, pch, cch ) );
    }
  template <> auto  utf8::decode<utf16>( const widechar*  pch, size_t cch ) -> utf16::string_t
    {
      typename utf16::string_t out;
      return std::move( decodeit( out, pch, cch ) );
    }
  template <> auto  utf8::decode<utf16>( const uint32_t*  pch, size_t cch ) -> utf16::string_t
    {
      typename utf16::string_t out;
      return std::move( decodeit( out, pch, cch ) );
    }

  template <> auto  utf8::decode<utf32>( const char*      pch, size_t cch ) -> utf32::string_t
    {
      typename utf32::string_t out;
      return std::move( decodeit( out, pch, cch ) );
    }
  template <> auto  utf8::decode<utf32>( const widechar*  pch, size_t cch ) -> utf32::string_t
    {
      typename utf32::string_t out;
      return std::move( decodeit( out, pch, cch ) );
    }
  template <> auto  utf8::decode<utf32>( const uint32_t*  pch, size_t cch ) -> utf32::string_t
    {
      typename utf32::string_t out;
      return std::move( decodeit( out, pch, cch ) );
    }

  template <> auto  utf8::decode<utf16>( const std::basic_string<char>& str ) -> utf16::string_t
    {  return std::move( decode<utf16>( str.c_str(), str.length() ) );  }
  template <> auto  utf8::decode<utf16>( const std::basic_string<widechar>& str ) -> utf16::string_t
    {  return std::move( decode<utf16>( str.c_str(), str.length() ) );  }
  template <> auto  utf8::decode<utf16>( const std::basic_string<uint32_t>& str ) -> utf16::string_t
    {  return std::move( decode<utf16>( str.c_str(), str.length() ) );  }

  template <> auto  utf8::decode<utf32>( const std::basic_string<char>& str ) -> utf32::string_t
    {  return std::move( decode<utf32>( str.c_str(), str.length() ) );  }
  template <> auto  utf8::decode<utf32>( const std::basic_string<widechar>& str ) -> utf32::string_t
    {  return std::move( decode<utf32>( str.c_str(), str.length() ) );  }
  template <> auto  utf8::decode<utf32>( const std::basic_string<uint32_t>& str ) -> utf32::string_t
    {  return std::move( decode<utf32>( str.c_str(), str.length() ) );  }

  inline  auto  utf8::mbtowc( const charstr& mb_str ) -> widestr
    {
      auto  wc_str = widestr();  wc_str.reserve( mb_str.length() );

      for ( auto& c: mb_str )
        wc_str.push_back( (widechar)(unsigned char)c );

      return std::move( wc_str );
    }

  inline  bool  utf8::detect( const char*     pch, size_t cch )  {  return detectit( pch, cch );  }
  inline  bool  utf8::detect( const widechar* pch, size_t cch )  {  return detectit( pch, cch );  }
  inline  bool  utf8::detect( const uint32_t* pch, size_t cch )  {  return detectit( pch, cch );  }

  inline  bool  utf8::detect( const std::basic_string<char>&     src ) {  return detect( src.c_str(), src.length() );  }
  inline  bool  utf8::detect( const std::basic_string<widechar>& src ) {  return detect( src.c_str(), src.length() );  }
  inline  bool  utf8::detect( const std::basic_string<uint32_t>& src ) {  return detect( src.c_str(), src.length() );  }

namespace utf
{
  /*
   *  cbchar( uint32_t uc )
   *  Возвращает количество символов, которыми кодируется этот символ.
   */
  inline  size_t  cbchar( uint32_t uch )
    {  return utf8::charsize( uch );  }

  /*
   *  cbchar( utfstr )
   *  Возвращает количество байт, кодирующих первый символ строки.
   */
  inline  size_t  cbchar( const char* str, size_t  len = (size_t)-1 )
    {  return utf8::charsize( str, len );  }

  /*
   * detect( utfstr )
   * Возвращает признак того, что строка является utf8-строкой.
   */
  inline  bool    detect( const char* str, size_t len = (size_t)-1 )
    {  return utf8::detect( str, len );  }

  /*
   * verify( utfstr )
   * Возвращает признак того, что строка может быть корректной utf8-строкой.
   */
  inline  bool    verify( const char* str, size_t len = (size_t)-1 )
    {  return utf8::detect( str, len );  }

  /*
   * strlen( utfstr )
   * Возвращает количество символов, закодированных utf8-строкой.
   */
  inline  size_t  strlen( const char* str, size_t len = (size_t)-1 )
    {  return utf8::strlen( utf16(), str, len );  }

  /*
   *  encode( pszout, cchout, pwssrc, cchsrc )
   *  Кодирует символ в utf8
   */  
  inline  size_t  encode( char* out, size_t len, uint32_t wch )
    {  return utf8::encode( out, len, wch );  }

  /*
   *  encode( pszout, cchout, pwssrc, cchsrc )
   *  Кодирует строку в utf8
   */  
  inline  size_t  encode( char* out, size_t len, const widechar* src, size_t cch = (size_t)-1 )
    {  return utf8::encode( out, len, src, cch );  }
  inline  auto    encode( const widechar* src, size_t len = (size_t)-1 ) -> charstr
    {  return std::move( utf8::encode( src, len ) );  }
  inline  auto    encode( const widestr& str ) -> charstr
    {  return std::move( encode( str.c_str(), str.length() ) );  }

  /*
   *  decode( pwsout, cchout, pszsrc, cchsrc )
   *  Декодирует строку из utf8 в utf16
   */
  inline  size_t  decode( widechar* out, size_t len, const char* src, size_t cch = (size_t)-1 )
    {  return utf8::decode( out, len, src, cch );  }
  inline  auto    decode( const char* src, size_t len = (size_t)-1 ) -> widestr
    {  return utf8::decode<utf16>( src, len );  }
  inline  auto  decode( const charstr& str ) -> widestr
    {  return std::move( decode( str.c_str(), str.length() ) );  }

}}

# endif   // !__mtc_utf_hpp__
