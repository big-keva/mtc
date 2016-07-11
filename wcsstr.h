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

  inline  auto w_strlen( const widechar* s )  {  return __impl_strlen( s );  }
  inline  auto w_strlen( const char* s )      {  return __impl_strlen( s );  }

  //
  // strdup() family
  //

  template <class C, class M>  C*  __impl_strdup( const C* s )
  {
    size_t  l = w_strlen( s );
    C*      p;

    if ( (p = (C*)M().alloc( sizeof(C) * (l + 1) )) == nullptr )
      return nullptr;
    while ( (*p++ = *s++) != (C)0 )
      (void)0;
    return p - l - 1;
  }

  template <class M = def_alloc<>>
  inline  widechar* w_strdup( const widechar* s )   {  return s != nullptr ? __impl_strdup<widechar, M>( s ) : nullptr;  }
  template <class M = def_alloc<>>
  inline  char*     w_strdup( const char* s )       {  return s != nullptr ? __impl_strdup<    char, M>( s ) : nullptr;  }

  //
  // strcpy() family
  //

  template <class O, class S>  O*  __impl_strcpy( O* o, const S* s )
  {
    for ( auto p = o; (*p++ = *s++) != (O)0; )
      (void)0;
    return o;
  }

  inline  widechar* w_strcpy( widechar* o, const widechar* s )  {  return __impl_strcpy( o, s );  }
  inline  widechar* w_strcpy( widechar* o, const char* s )      {  return __impl_strcpy( o, s );  }
  inline  char* w_strcpy( char* o, const char* s )              {  return __impl_strcpy( o, s );  }

  inline  widechar* w_strncpy( widechar* pwsout, const widechar* pwssrc, int n )
  {
    widechar* pwstop = pwsout;

    while ( n-- > 0 && (*pwsout = *pwssrc++) != 0 )
      ++pwsout;
    while ( n-- > 0 )
      *pwsout++ = 0;

    return pwstop;
  }

  inline  char*     w_strncpy( char* pszout, const char* pszsrc, int n )
  {
    return ::strncpy( pszout, pszsrc, n );
  }

  //
  // strcat() family
  //

  inline  widechar* w_strcat( widechar* pwsout, const widechar* pwssrc )
  {
    widechar* pstore = pwsout;

    while ( *pstore++ != 0 )  (void)NULL;

    w_strcpy( pstore - 1, pwssrc );

    return pwsout;
  }

  inline  char*     w_strcat( char* pszout, const char* pszsrc )
  {
    return ::strcat( pszout, pszsrc );
  }

  //
  // strcmp() family
  //
  inline  int     w_strcmp( const char*     pszone,
                            const char*     psztwo )
  {
    return ::strcmp( pszone, psztwo );
  }

  inline  int     w_strcmp( const widechar* pszone,
                            const widechar* psztwo )
  {
    for ( ; ; )
    {
      widechar  chrone;
      int       rescmp;

      if ( (rescmp = (chrone = *pszone++) - *psztwo++) != 0 )
        return rescmp;
      if ( chrone == 0 )
        return 0;
    }
  }

  //
  // strchr() family
  //
  inline  char*     w_strchr( const char*     string, int c )
  {
    return (char*)::strchr( string, c );
  }

  inline  widechar* w_strchr( const widechar* string, int c )
  {
    while ( *string != 0 && c != *string )
      ++string;
    return (widechar*)( c == *string ? string : NULL );
  }

  //
  // strncmp() family
  //
  inline  int     w_strncmp( const widechar* pwsst1, const widechar* pwsst2, size_t cchstr )
  {
    int   rescmp;

    while ( cchstr-- > 0 )
      if ( (rescmp = *pwsst1++ - *pwsst2++) != 0 )
        return rescmp;
    return 0;
  }

  inline  int     w_strncmp( const widechar* pwsstr, const char* pszstr, size_t cchstr )
  {
    int   rescmp;

    while ( cchstr-- > 0 )
      if ( (rescmp = *pwsstr++ - widechar(*pszstr++)) != 0 )
        return rescmp;
    return 0;
  }

  inline  int     w_strncmp( const char* pszstr, const widechar* pwsstr, size_t cchstr )
  {
    int   rescmp;

    while ( cchstr-- > 0 )
      if ( (rescmp = widechar(*pszstr++) - *pwsstr++) != 0 )
        return rescmp;
    return 0;
  }

  inline  int     w_strncmp( const char* pszst1, const char* pszst2, size_t cchstr )
  {
    int   rescmp;

    while ( cchstr-- > 0 )
      if ( (rescmp = (unsigned char)*pszst1++ - (unsigned char)*pszst2++) != 0 )
        return rescmp;
    return 0;
  }

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
