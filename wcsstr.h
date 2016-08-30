/*

The MIT License (MIT)

Copyright (c) 2016 Андрей Коваленко aka Keva
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
# include <libcodes/codes.h>
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

  inline  auto w_strlen( const widechar* s )  {  return (size_t)__impl_strlen( s );  }
  inline  auto w_strlen( const char* s )      {  return (size_t)__impl_strlen( s );  }

  //
  // strdup() family
  //

  template <class C, class M>  C*  __impl_strdup( const C* s, size_t l = (size_t)-1 )
  {
    C*  o;
    C*  p;

    if ( l == (size_t)-1 )
      for ( l = 0; s != nullptr && s[l] != (C)0; ++l )  (void)0;

    if ( (o = p = (C*)M().alloc( sizeof(C) * (l + 1) )) != nullptr )
      {  while ( l-- > 0 ) *p++ = *s++;  *p = (C)0;  }

    return o;
  }

  template <class M = def_alloc<>>
  inline  widechar* w_strdup( const widechar* s, size_t l = (size_t)-1 )
    {  return s != nullptr ? __impl_strdup<widechar, M>( s, l ) : nullptr;  }

  template <class M = def_alloc<>>
  inline  char*     w_strdup( const char* s, size_t l = (size_t)-1 )
    {  return s != nullptr ? __impl_strdup<    char, M>( s, l ) : nullptr;  }

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

  inline  int   w_strcmp( const char* s, const char* m )          {  return __impl_strcmp( s, m );  }
  inline  int   w_strcmp( const widechar* s, const widechar* m )  {  return __impl_strcmp( s, m );  }

  inline  int   w_strcmp( const char* s, const widechar* m, unsigned codepage = codepages::codepage_1251 )
  {
    int   rc;

    if ( codepage != codepages::codepage_utf8 )
    {
      while ( (rc = codepages::chartowide( codepage, *s ) - *m++) == 0 && *s++ != 0 )
        (void)0;
    }
      else
    for ( ; ; )
    {
      size_t    cb = codepages::utf8cbchar( s );
      widechar  ch = codepages::utf8dechar( s, cb );

      if ( (rc = ch - *m++) != 0 || *s == '\0' )  break;
        else s += cb;
    }
    return rc;
  }

  inline  int   w_strcmp( const widechar* m, const char* s, unsigned codepage = codepages::codepage_1251 )
  {
    return -w_strcmp( s, m, codepage );
  }

  //
  // strncmp() family
  //
  template <class C, class M> int __impl_strncmp( const C* s, const M* m, size_t n )
  {
    int   rc = 0;

    while ( n-- > 0 && (rc = *s++ - *m++) == 0 )
      (void)0;
    return rc;
  }

  inline  int   w_strncmp( const char* s, const char* m, size_t n )         {  return __impl_strncmp( s, m, n );  }
  inline  int   w_strncmp( const char* s, const widechar* m, size_t n )     {  return __impl_strncmp( s, m, n );  }
  inline  int   w_strncmp( const widechar* s, const widechar* m, size_t n ) {  return __impl_strncmp( s, m, n );  }
  inline  int   w_strncmp( const widechar* s, const char* m, size_t n )     {  return __impl_strncmp( s, m, n );  }

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
  template <class C>  C*  __impl_strstr( const C* s, const C* m )
  {
    while ( *s != (C)0 )
    {
      const C* s1;
      const C* m1;

      while ( *s != (C)0 && *s != *m )
        ++s;

      for ( s1 = s, m1 = m; *m1 != 0 && *s1++ == *m1++; )
        (void)0;

      if ( *m1 == (C)0 ) return (C*)s;
        else ++s;
    }
    return nullptr;
  }

  inline  char*     w_strstr( const char* s, const char* m )          {  return __impl_strstr( s, m );  }
  inline  widechar* w_strstr( const widechar* s, const widechar* m )  {  return __impl_strstr( s, m );  }

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

}  // mtc namespace

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif  // _MSC_VER

# endif // __mtc_wcsstr_h__
