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
# include "utf.hpp"

namespace mtc {
namespace utf {

  size_t  cbchar( const char* pszstr, size_t  cchstr )
  {
    unsigned char chnext;
    int           nleast;
    const char*   pszorg;
    const char*   pszend;

  // check for length
    if ( pszstr == nullptr )
      return 0;

    if ( cchstr == (size_t)-1 )
      for ( pszend = pszorg = pszstr; *pszend != '\0'; ++pszend )  (void)NULL;
    else
      pszend = (pszorg = pszstr) + cchstr;

  // check if 7-bit character
    if ( ((chnext = (uint8_t)*pszstr++) & 0x80) == 0 )
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
    if ( pszend - pszstr < nleast - 1 )
      return 0;

  // check least bits
    while ( nleast-- > 0 )
      if ( (*pszstr & 0xC0) != 0x80 ) return 0;
        else ++pszstr;

    return pszstr - pszorg;
  }

  size_t  cbchar( uint32_t n )
  {
    return  n <= 0x0000007f ? 1 :
            n <= 0x000007ff ? 2 :
            n <= 0x0000ffff ? 3 :
            n <= 0x001fffff ? 4 :
            n <= 0x03ffffff ? 5 :
            n <= 0x7fffffff ? 6 : 7;
  }

  inline  uint32_t  one( widechar c )
  {
    return 0x10000 + ((c & 0x07ff) << 10);
  }

  inline  uint32_t  two( widechar c )
  {
    return c & 0x3ff;
  }

  inline  uint32_t  get( const widechar*& s, const widechar* e )
  {
    if ( s >= e )
      return 0;
    if ( *s < 0xd800 || *s > 0xdfff || s >= e - 1 )
      return *s++;
    auto ucl = one( *s++ );
    auto uch = two( *s++ );
      return ucl + uch;
  }

  inline  uint16_t  upper_16( uint32_t u )
  {
    return 0xd800 | ((u - 0x10000) >> 10);
  }

  inline  uint16_t  lower_16( uint32_t u )
  {
    return 0xdc00 | ((u & 0x03ff));
  }

  inline  size_t    store_16( widechar* o, size_t l, uint32_t u )
  {
    return l > 0 ? (*o = u, 1) : (size_t)-1;
  }

  inline  size_t    store_21( widechar* o, size_t l, uint32_t u )
  {
    return l > 1 ? (*o++ = upper_16( u ), *o = lower_16( u ), 2) : (size_t)-1;
  }

  inline  size_t    put( widechar* o, size_t l, uint32_t u )
  {
    return u <= 0xffff ? store_16( o, l, u ) : store_21( o, l, u );
  }

  size_t  cbchar( const widechar* pwsstr, size_t  cchstr )
  {
    const widechar* pwsend;
    size_t          length;

    if ( pwsstr == nullptr )
      return 0;

    if ( cchstr == (size_t)-1 )
      for ( auto pwsorg = pwsstr, pwsend = pwsorg; *pwsend != 0; ++pwsend )  (void)NULL;
    else
      pwsend = pwsstr + cchstr;

    for ( length = 0; pwsstr != pwsend; )
      length += cbchar( get( pwsstr, pwsend ) );
      
    return length;
  }

  bool  detect( const char* pszstr, size_t  cchstr )
  {
    const char* pszend;
    bool        uppers = false;

    if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
      else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

    while ( pszstr < pszend )
    {
      size_t  nchars;

      if ( (nchars = cbchar( pszstr, pszend - pszstr )) == 0 )
        return pszend - pszstr <= 1 ? uppers : false;
      uppers |= ((*pszstr & 0x80) != 0);
        pszstr += nchars;
    }
    return uppers;
  }

  bool  verify( const char* pszstr, size_t  cchstr )
  {
    const char* pszend;

    if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
      else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

    while ( pszstr < pszend )
    {
      size_t  nchars;

      if ( (nchars = cbchar( pszstr, pszend - pszstr )) != 0 ) pszstr += nchars;
        else return false;
    }
    return true;
  }

  size_t  strlen( const char* pszstr, size_t  cchstr )
  {
    const char* pszend;
    size_t      cchlen;
    size_t      nchars;

    if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
      else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

    for ( cchlen = 0; pszstr < pszend; pszstr += (nchars ? nchars : 1), cchlen += (nchars ? 1 : 0) )
      nchars = cbchar( pszstr, pszend - pszstr );

    return cchlen;
  }

  inline
  uint32_t  dechar( const char* ptrtop, size_t chsize )
  {
    uint32_t  ucchar;

  // on non-utf strings, return -1 as non-utf string error
    switch ( chsize-- )
    {
      case 0:
        return 0;
      case 1:
        return (unsigned char)*ptrtop;
      case 2:
        ucchar = (unsigned char)(*ptrtop++ & 0x1f);
        break;
      case 3:
        ucchar = (unsigned char)(*ptrtop++ & 0x0f);
        break;
      case 4:
        ucchar = (unsigned char)(*ptrtop++ & 0x07);
        break;
      case 5:
        ucchar = (unsigned char)(*ptrtop++ & 0x03);
        break;
      case 6:
        ucchar = (unsigned char)(*ptrtop++ & 0x01);
        break;
      default:
        return (uint32_t)-1;
    }

    while ( chsize-- > 0 )
      ucchar = (ucchar << 6) | (unsigned char)(*ptrtop++ & 0x3f);

    return ucchar;
  }

  size_t  decode( widechar* output, size_t  maxlen, const char* pszstr, size_t  cchstr )
  {
    auto        outorg = output;
    auto        outend = output + maxlen;
    const char* pszend;

    if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
      else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

    while ( pszstr < pszend )
    {
      unsigned  ucchar;
      size_t    nchars;
      size_t    nstore;

    // on non-utf strings, return -1 as non-utf string error
      if ( (nchars = cbchar( pszstr, pszend - pszstr )) == 0 )
      {
        ++pszstr;
          continue;
      }
      if ( (ucchar = dechar( pszstr, nchars )) == (unsigned)-1 )
        continue;

      if ( (nstore = put( output, outend - output, ucchar )) == (size_t)-1 )
        return (size_t)-1;

      output += nstore;
      pszstr += nchars;
    }

    if ( output < outend )
      *output = 0;
    return output - outorg;
  }

  auto  decode( const char* str, size_t cch ) -> widestr
  {
    auto  len = cch != (size_t)-1 ? cch : w_strlen( str );
    auto  ccw = utf::strlen( str, len );
    auto  out = widestr( ccw, 0x20 );

    return decode( (widechar*)out.c_str(), ccw, str, len ), std::move( out );
  }

  inline
  size_t  encode( char* output, size_t  cchout, uint32_t chnext )
  {
    char* outorg = output;
    char* outend = output + cchout;

    if ( (chnext & ~0x007f) == 0 )
    {
      if ( output < outend )  *output++ = (char)(unsigned char)chnext;
        else return (size_t)-1;
    }
      else
    if ( (chnext & ~0x07ff) == 0 )
    {
      if ( output >= outend - 1 )
        return (size_t)-1;
      *output++ = (char)(0xC0 | (unsigned char)((chnext >> 0x06) & 0x3f));
      *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x00) & 0x3f));
    }
      else
    if ( (chnext & ~0x0ffff) == 0 )
    {
      if ( output >= outend - 2 )
        return (size_t)-1;
      *output++ = (char)(0xE0 | (unsigned char)((chnext >> 0x0c) & 0x0f));
      *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x06) & 0x3F));
      *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x00) & 0x3F));
    }
      else
    if ( chnext <= 0x10ffff )
    {
      if ( output >= outend - 3 )
        return (size_t)-1;
      *output++ = (char)(0xf0 | (unsigned char)((chnext >> 0x12) & 0x07));
      *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x0c) & 0x3F));
      *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x06) & 0x3F));
      *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x00) & 0x3F));
    }
      else
    return (size_t)-1;

    if ( output < outend )
      *output = '\0';
        
    return output - outorg;
  }

  size_t  encode( char* output, size_t  cchout, const widechar* pwsstr, size_t  cchstr )
  {
    char*           outorg = output;
    char*           outend = output + cchout;
    const widechar* pwsend;

    if ( cchstr != (size_t)-1 ) pwsend = pwsstr + cchstr;
      else for ( pwsend = pwsstr; *pwsend != 0; ++pwsend )  (void)NULL;

    while ( pwsstr < pwsend )
    {
      size_t  cchenc;

      if ( (cchenc = encode( output, cchout, get( pwsstr, pwsend ) )) != (size_t)-1 )  output += cchenc;
        else return cchenc;
    }

    if ( output < outend )
      *output = '\0';

    return output - outorg;
  }

  auto  encode( const widechar* str, size_t cch ) -> charstr
  {
    auto  len = cch != (size_t)-1 ? cch : w_strlen( str );
    auto  out = charstr( len * 6, ' ' );

    out.resize( encode( (char*)out.c_str(), out.length(), str, len ) );

    return std::move( out );
  }

}}
