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
# if !defined( __mtc_wcsstr_h__ )
# define  __mtc_wcsstr_h__

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4996 )
# endif  // _MSC_VER

# include <limits.h>
# include <string.h>
# include <stdlib.h>
# include <stdint.h>
# include <stdarg.h>
# include <stdio.h>
# include <assert.h>
# include "platform.h"

# if !defined( __widechar_defined__ )
# define  __widechar_defined__
#   if defined(WCHAR_MAX) && (WCHAR_MAX >> 16) == 0
    typedef wchar_t         widechar;
#   else
    typedef unsigned short  widechar;
#   endif  // size
# endif  // __widechar_defined__

namespace mtc
{

  //
  // strlen() family
  //
  template <class C>  size_t  __impl_strlen( const C* s )
  {
    auto o = s;
      while ( *s++ != (C)0 )  (void)0;
    return s - o - 1;
  }

  inline  size_t w_strlen( const widechar* s )  {  return __impl_strlen( s );  }
  inline  size_t w_strlen( const char* s )      {  return __impl_strlen( s );  }

  //
  // strdup() family
  //

  template <class C>  C*  __impl_strdup( const C* s, size_t l )
  {
    C*  o;
    C*  p;

    if ( l == (size_t)-1 )
      for ( l = 0; s != nullptr && s[l] != (C)0; ++l )  (void)0;

    if ( (o = p = (C*)malloc( sizeof(C) * (l + 1) )) != nullptr )
      {  while ( l-- > 0 ) *p++ = *s++;  *p = (C)0;  }

    return o;
  }

  inline  widechar* w_strdup( const widechar* s, size_t l = (size_t)-1 )
    {  return s != nullptr ? __impl_strdup<widechar>( s, l ) : nullptr;  }

  inline  char*     w_strdup( const char* s, size_t l = (size_t)-1 )
    {  return s != nullptr ? __impl_strdup<    char>( s, l ) : nullptr;  }

  //
  // strcpy() family
  //

  template <class O, class S> O*  __impl_strcpy( O* o, const S* s )
  {
    for ( auto p = o; (*p++ = *s++) != (O)0; )
      (void)0;
    return o;
  }

  inline  widechar* w_strcpy( widechar* o, const widechar* s )  {  return __impl_strcpy( o, s );  }
  inline  widechar* w_strcpy( widechar* o, const char* s )      {  return __impl_strcpy( o, s );  }
  inline  char*     w_strcpy( char* o, const char* s )          {  return __impl_strcpy( o, s );  }

  template <class O, class S> O*  __impl_strncpy( O* o, const S* s, int n )
  {
    O*  p = o;

    while ( n-- > 0 && (*p++ = *s++) != (O)0 )
      (void)0;
    if ( n > 0 )
      *p = (O)0;
    return o;
  }

  inline  widechar* w_strncpy( widechar* o, const widechar* s, int n )  {  return __impl_strncpy( o, s, n );  }
  inline  widechar* w_strncpy( widechar* o, const char* s, int n )      {  return __impl_strncpy( o, s, n );  }
  inline  char*     w_strncpy( char* o, const char* s, int n )          {  return __impl_strncpy( o, s, n );  }

  //
  // strcat() family
  //

  template <class O, class S> O*  __impl_strcat( O* o, const S* s )
  {
    O*  p = o;

    while ( *p++ != (O)0 )
      (void)0;
    w_strcpy( p - 1, s );
      return o;
  }

  inline  widechar* w_strcat( widechar* o, const widechar* s )    {  return __impl_strcat( o, s );  }
  inline  widechar* w_strcat( widechar* o, const char* s )        {  return __impl_strcat( o, s );  }
  inline  char*     w_strcat( char* o, const char* s )            {  return __impl_strcat( o, s );  }

  //
  // strcmp() family
  //
  template <class C>  int __impl_strcmp( const C* s, const C* m )
  {
    int   rc;

    while ( (rc = *s - *m++) == 0 && *s++ != (C)0 )
      (void)0;
    return rc;
  }

  struct __impl_default_getwidechar
  {
    widechar  operator ()( const char*& s )
      {  return (widechar)(unsigned char)*s++;  }
  };

  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strcmp( const char* s, const char* m, ucload l = __impl_default_getwidechar() )
    {  return __impl_strcmp( s, m );  }
  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strcmp( const widechar* s, const widechar* m, ucload l = __impl_default_getwidechar() )
    {  return __impl_strcmp( s, m );  }

  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strcmp( const char* s, const widechar* m, ucload l = __impl_default_getwidechar() )
  {
    int   rc;

    while ( (rc = l( s ) - *m) == 0 && *m != 0 )
      ++m;
    return rc;
  }

  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strcmp( const widechar* m, const char* s, ucload l = __impl_default_getwidechar() )
  {
    return -w_strcmp( s, m, l );
  }

  //
  // strncmp() family
  //
  template <class C> int  __impl_strncmp( const C* s, const C* m, size_t n )
  {
    int   rc = 0;

    while ( n-- > 0 && (rc = *s++ - *m++) == 0 )
      (void)0;
    return rc;
  }

  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strncmp( const char* s, const char* m, size_t n, ucload l = __impl_default_getwidechar() )
    {  return __impl_strncmp( s, m, n );  }
  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strncmp( const widechar* s, const widechar* m, size_t n, ucload l = __impl_default_getwidechar() )
    {  return __impl_strncmp( s, m, n );  }

  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strncmp( const char* s, const widechar* m, size_t n, ucload l = __impl_default_getwidechar() )
  {
    int   rc = 0;

    while ( n-- > 0 && (rc = l( s ) - *m++) == 0 )
      (void)0;
    return rc;
  }

  template <class ucload = __impl_default_getwidechar>
  inline  int   w_strncmp( const widechar* s, const char* m, size_t n, ucload l = __impl_default_getwidechar() )
  {
    int   rc = 0;

    while ( n-- > 0 && (rc = *s++ - l( m )) == 0 )
      (void)0;
    return rc;
  }

  //
  // strchr() family
  //
  template <class C>  C*  __impl_strchr( const C* s, int c )
  {
    while ( c != *s && *s != (C)0 )
      ++s;
    return c == *s ? (C*)s : nullptr;
  }

  inline  char*     w_strchr( const char* s, int c )              {  return __impl_strchr( s, c );  }
  inline  widechar* w_strchr( const widechar* s, int c )          {  return __impl_strchr( s, c );  }

  //
  // strstr family
  //
  template <class C>  const C*  __impl_strstr( const C* s, const C* m )
  {
    while ( *s != (C)0 )
    {
      const C* s1;
      const C* m1;

      while ( *s != (C)0 && *s != *m )
        ++s;

      if ( *s == (C)0 )
        break;

      for ( s1 = s, m1 = m; *m1 != 0 && *s1++ == *m1; ++m1 )
        (void)0;

      if ( *m1 == (C)0 ) return s;
        else ++s;
    }
    return nullptr;
  }

  template <class C = __impl_default_getwidechar>  inline  const char*     w_strstr( const char* s, const char* m, C l = __impl_default_getwidechar() )
    {  return __impl_strstr( s, m );  }
  template <class C = __impl_default_getwidechar>  inline  const widechar* w_strstr( const widechar* s, const widechar* m, C l = __impl_default_getwidechar() )
    {  return __impl_strstr( s, m );  }

  template <class ucload = __impl_default_getwidechar>
  inline  const char*     w_strstr( const char* s, const widechar* m, ucload l = __impl_default_getwidechar() )
  {
    if ( *m == 0 )
      return s;

    while ( *s != '\0' )
    {
      const char* o = s;

      if ( l( s ) == *m )
      {
        const char*     s1;
        const widechar* m1;

        for ( s1 = s, m1 = 1 + m; *m1 != 0 && l( s1 ) == *m1; ++m1 )
          (void)0;
        if ( *m1 == 0 )
          return o;
      }
    }
    return nullptr;
  }

  template <class ucload = __impl_default_getwidechar>
  inline  const widechar* w_strstr( const widechar* s, const char* m, ucload l = __impl_default_getwidechar() )
  {
    if ( *m == 0 )
      return s;

    while ( *s != 0 )
    {
      const widechar* o = s;
      const char*     c = m;

      if ( *s++ == l( c ) )
      {
        const widechar* s1;
        const char*     c1;

        for ( s1 = s; *c != '\0' && *s++ == l( c1 = c ); c = c1 )
          (void)0;
        if ( *c == '\0' )
          return o;
      }
    }
    return nullptr;
  }

  //
  // strtod family
  //
  inline  double  w_strtod( const widechar* pwsstr, widechar**  pwsend )
  {
    double          bigger = 0.0;
    double          decone;
    double          decpow;

    while ( *pwsstr >= widechar('0') && *pwsstr <= widechar('9') )
      bigger = (bigger * 10) + *pwsstr++ - widechar('0');
    if ( *pwsstr++ != '.' )
    {
      if ( pwsend != NULL )
        *pwsend = (widechar*)pwsstr - 1;
      return bigger;
    }

    decpow = 1.0;
    decone = 0.0;

    while ( *pwsstr >= widechar('0') && *pwsstr <= widechar('9') )
    {
      decone = (decone * 10) + *pwsstr++ - widechar('0');
      decpow *= 10.0;
    }

    if ( pwsend != NULL )
      *pwsend = (widechar*)pwsstr;
    return bigger + decone / decpow;
  }

  //
  // strtoup family
  //
  inline  unsigned long w_strtoul( const widechar* pwsstr, widechar**  pwsend, int dwbase )
  {
    unsigned long result = 0;

    if ( dwbase == 0 || dwbase == 16 )
    {
      if ( pwsstr[0] == (widechar)'0' && (pwsstr[1] == (widechar)'x'
        || pwsstr[1] == (widechar)'X') )
      {
        dwbase = 16;
        pwsstr += 2;
      }
      if ( dwbase == 0 )
        dwbase = 10;
    }

    if ( dwbase == 10 )
    {
      while ( *pwsstr >= (widechar)'0' && *pwsstr <= (widechar)'9' )
        result = result * 10 + *pwsstr++ - (widechar)'0';
    }
      else
    if ( dwbase == 16 )
    {
      for ( ; ; )
      {
        if ( *pwsstr >= (widechar)'0' && *pwsstr <= (widechar)'9' )
        {
          result = result * 16 + *pwsstr++ - (widechar)'0';
        }
          else
        if ( *pwsstr >= (widechar)'A' && *pwsstr <= (widechar)'F' )
        {
          result = result * 16 + *pwsstr++ - (widechar)'A' + 10;
        }
          else
        if ( *pwsstr >= (widechar)'a' && *pwsstr <= (widechar)'f' )
        {
          result = result * 16 + *pwsstr++ - (widechar)'a' + 10;
        }
          else
        break;
      }
    }
    if ( pwsend != NULL )
      *pwsend = (widechar*)pwsstr;
    return result;
  }

  inline  unsigned long w_strtoul( const char* pszstr, char**  pszend, int dwbase )
  {
    return ::strtoul( pszstr, pszend, dwbase );
  }

  //
  // strtol family
  //
  inline  long          w_strtol( const widechar* pwsstr, widechar**  pwsend, int dwbase )
  {
    if ( *pwsstr == (widechar)'-' )
      return 0 - w_strtoul( ++pwsstr, pwsend, dwbase );
    return w_strtoul( pwsstr, pwsend, dwbase );
  }

  inline  long          w_strtol( const char*     pszstr, char**  pszend, int dwbase )
  {
    return ::strtol( pszstr, pszend, dwbase );
  }

  //
  // sprintf family
  //
  template <class C, class I> C   __impl_inttochr( I value, int radix, bool lower )
  {
    int   n = value % radix;
    return n < 10 ? n + '0' : n + 'A' + (lower ? 'a' - 'A' : 0);
  }

  template <class C, class I> C*  __impl_inttostr( C* store, I value, int radix = 10, bool lower = true )
  {
    C*  stptr = store;

    assert( radix == 10 || radix == 16 );

    do  *stptr++ = __impl_inttochr<C>( value, radix, lower );
      while ( (value /= radix) != 0 );
    for ( auto t = store, e = stptr - 1; t < e; ++t, --e )
      {  C c = *t;  *t = *e;  *e = c;  }
    *stptr = 0;  return store;
  }

  template <class C>  C*  __impl_dbltostr( C* store, double value )
  {
  }

/*
  inline  widechar*   w_vsnprintf( widechar* o, size_t l, const widechar* t, va_list v )
  {
  }

  inline  widechar*   w_snprintf( widechar* o, size_t l, const widechar* t, ... )
  {
  }
*/

  //
  // char* strduprintf( format, ... ) family
  //
  inline  char* vstrduprintf( const char* format, va_list vaargs )
  {
    va_list v;
    char*   p;
    int     l;

    va_copy( v, vaargs );
      p = (char*)malloc( l = vsnprintf( nullptr, 0, format, v ) + 1 );
    va_end( v );

    if ( p != nullptr )
      vsnprintf( p, l, format, vaargs );

    return p;
  }

  inline  char* strduprintf( const char* format, ... )
  {
    va_list vaargs;
    char*   output;

    va_start( vaargs, format );
      output = vstrduprintf( format, vaargs );
    va_end( vaargs );
    return output;
  }

}  // mtc namespace

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif  // _MSC_VER

# endif // __mtc_wcsstr_h__
