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

# if !defined( __widechar_defined__ )
# define  __widechar_defined__
#   if defined(WCHAR_MAX) && (WCHAR_MAX >> 16) == 0
    typedef wchar_t         widechar;
#   else
    typedef unsigned short  widechar;
#   endif  // size
# endif  // __widechar_defined__

namespace mtc {
namespace utf {

  auto    wide32( const widechar*, size_t = (size_t)-1 ) -> uint32_t;

  /*
   *  cbchar( utfstr )
   *  Возвращает количество байт, кодирующих первый символ строки.
   */
  size_t  cbchar( const char* str, size_t  len = (size_t)-1 );

  /*
   *  cbchar( uint32_t uc )
   *  Возвращает количество символов, которыми кодируется этот символ.
   */
  size_t  cbchar( uint32_t n );

  /*
   *  cbchar( const widechar* wcs, size_t len )
   *  Возвращает количество байт, кодирующих строку
   */
  size_t  cbchar( const widechar* wcs, size_t len = (size_t)-1 );

  /*
   * detect( utfstr )
   * Возвращает признак того, что строка является utf8-строкой.
   */
  bool    detect( const char* str, size_t len = (size_t)-1 );

  /*
   * verify( utfstr )
   * Возвращает признак того, что строка может быть корректной utf8-строкой.
   */
  bool    verify( const char* str, size_t len = (size_t)-1 );

  /*
   * strlen( utfstr )
   * Возвращает количество символов, закодированных utf8-строкой.
   */
  size_t  strlen( const char* str, size_t len = (size_t)-1 );

  /*
   *  encode( pszout, cchout, pwssrc, cchsrc )
   *  Кодирует wcs-строку в utf8
   */  
  size_t  encode( char* out, size_t, uint32_t );
  size_t  encode( char* out, size_t, const widechar* src, size_t = (size_t)-1 );
  auto    encode( const widechar* src, size_t = (size_t)-1 ) -> charstr;

  /*
   *  decode( pwsout, cchout, pszsrc, cchsrc )
   *  Декодирует строку из utf8 в utf16
   */
  size_t  decode( widechar* out, size_t, const char* src, size_t = (size_t)-1 );
  auto    decode( const char* src, size_t = (size_t)-1 ) -> widestr;

  inline  auto  encode( const widestr& str ) -> charstr
    {  return std::move( encode( str.c_str(), str.length() ) );  }
  inline  auto  decode( const charstr& str ) -> widestr
    {  return std::move( decode( str.c_str(), str.length() ) );  }

}}

# endif   // !__mtc_utf_hpp__
