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
# include <string>
# include "platform.h"
# include "autoptr.h"

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

  namespace impl
  {
  
    template <class chartype>
    inline  bool  w_in_lim( chartype ch, chartype lo, chartype hi )
    {
      return ch >= lo && ch <= hi;
    }

    template <class chartype, class compchar>
    inline  bool  w_is_chr( chartype ch, compchar cn )
    {
      return ch == (chartype)cn;
    }

    template <class chartype, class compchar, class... charlist>
    inline  bool  w_is_chr( chartype ch, compchar cn, charlist... cl )
    {
      return ch == (chartype)cn || w_is_chr( ch, cl... );
    }

    template <class chartype>
    inline  bool  w_is_num( chartype ch )
    {
      return w_in_lim( ch, (chartype)'0', (chartype)'9' );
    }

    struct getwidechar
    {
      widechar  operator ()( const char*& s )
        {  return (widechar)(unsigned char)*s++;  }
      widechar  operator ()( const widechar*& s )
        {  return *s++;  }
    };

    //
    // strlen() family
    //
    template <class C>  size_t  strlen( const C* s )
    {
      auto o = s;
        while ( *s++ != (C)0 )  (void)0;
      return s - o - 1;
    }

    //
    // strdup() family
    //
    template <class C, class M>  C* strdup( const C* s, size_t l, M& m )
    {
      C*  o;
      C*  p;

      if ( l == (size_t)-1 )
        for ( l = 0; s != nullptr && s[l] != (C)0; ++l )  (void)0;

      if ( (o = p = (C*)m.alloc( sizeof(C) * (l + 1) )) != nullptr )
        {  while ( l-- > 0 ) *p++ = *s++;  *p = (C)0;  }

      return o;
    }

    //
    // strcpy() family
    //
    template <class O, class S> O*  strcpy( O* o, const S* s )
    {
      for ( auto p = o; (*p++ = *s++) != (O)0; )
        (void)0;
      return o;
    }

    template <class O, class S> O*  strncpy( O* o, const S* s, size_t n )
    {
      O*  p = o;

      while ( n-- > 0 && (*p++ = *s++) != (O)0 )
        (void)0;
      if ( n > 0 )
        *p = (O)0;
      return o;
    }

    //
    // strcat() family
    //

    template <class O, class S> O*  strcat( O* o, const S* s )
    {
      O*  p = o;

      while ( *p++ != (O)0 )
        (void)0;
      w_strcpy( p - 1, s );
        return o;
    }

    //
    // strcasecmp() family
    //
    template <class A, class B, class L>  int strcasecmp( const A* s, const B* m, L l )
    {
      int       rc;
      widechar  lc;

      while ( (rc = (lc = l( s )) - l( m )) == 0 && lc != 0 )
        (void)0;
      return rc;
    }

    template <class A, class B, class L>  int strncasecmp( const A* s, const B* m, size_t n, L l )
    {
      int       rc = 0;
      widechar  lc;

      while ( n-- > 0 && (rc = (lc = l( s )) - l( m )) == 0 && lc != 0 )
        (void)0;
      return rc;
    }

    //
    // strchr() family
    //
    template <class C>  C*  strchr( const C* s, int c )
    {
      while ( c != *s && *s != (C)0 )
        ++s;
      return c == *s ? (C*)s : nullptr;
    }

    template <class C>  C*  strrchr( const C* s, int c )
    {
      const C*  p;

      for ( p = s; *p++ != (C)0; )  (void)0;

      while ( p >= s && *p != c )
        --p;

      return p >= s ? (C*)p : nullptr;
    }

    //
    // strstr family
    //
    template <class S, class M, class L>  const S*  strstr( const S* s, const M* m, L l )
    {
      const S*  s1;
      const M*  m1;
      const M*  m2;

      for ( auto  m_ = l( m1 = m ); *s != (S)0; l( s ) )
      {
        while ( *s != (S)0 && l( s1 = s ) != l( m1 = m ) )
          s = s1;

        if ( *s == (S)0 )
          break;

        for ( s1 = s, m1 = m; *m1 != (M)0 && l( s1 ) == l( m2 = m1 ); m1 = m2 )
          (void)0;

        if ( *m1 == (M)0 )
          return s;
      }
      return nullptr;
    }

    //
    // strtoll family
    //
    template <class chartype>
    unsigned long long strtoull( const chartype* str, chartype**  end, int dwbase )
    {
      long long result = 0;

      if ( dwbase == 0 || dwbase == 16 )
      {
        if ( str[0] == (chartype)'0' && impl::w_is_chr( str[1], 'x', 'X' ) )
        {
          dwbase = 16;
          str += 2;
        }
        if ( dwbase == 0 )
          dwbase = 10;
      }

      if ( dwbase == 10 )
      {
        while ( impl::w_is_num( *str ) )
          result = result * 10 + *str++ - '0';
      }
        else
      if ( dwbase == 16 )
      {
        for ( ; ; )
        {
          if ( impl::w_in_lim( *str, (chartype)'0', (chartype)'9' ) ) result = result * 16 + *str++ - '0';
            else
          if ( impl::w_in_lim( *str, (chartype)'A', (chartype)'F' ) ) result = result * 16 + *str++ - 'A' + 10;
            else
          if ( impl::w_in_lim( *str, (chartype)'a', (chartype)'f' ) ) result = result * 16 + *str++ - 'a' + 10;
            else
          break;
        }
      }
      if ( end != NULL )
        *end = (chartype*)str;
      return result;
    }

  }

  //
  // strlen() family
  //
  inline  size_t w_strlen( const widechar* s )  {  return impl::strlen( s );  }
  inline  size_t w_strlen( const char* s )      {  return impl::strlen( s );  }

  //
  // strdup() family
  //
  template <class M>
  inline  widechar* w_strdup( const widechar* s, size_t l, M& m )
    {
      return s != nullptr ? impl::strdup( s, l, m ) : nullptr;
    }

  template <class M>
  inline  char*     w_strdup( const char* s, size_t l, M& m )
    {
      return s != nullptr ? impl::strdup( s, l, m ) : nullptr;
    }

  inline  char*     w_strdup( const char* s, size_t l = (size_t)-1 )  
    {
      def_alloc m;
      return w_strdup( s, l, m );
    }

  inline  widechar* w_strdup( const widechar* s, size_t l = (size_t)-1 )
    {
      def_alloc m;
      return w_strdup( s, l, m );
    }

  //
  // strcpy() family
  //
  inline  widechar* w_strcpy( widechar* o, const widechar* s )  {  return impl::strcpy( o, s );  }
  inline  widechar* w_strcpy( widechar* o, const char* s )      {  return impl::strcpy( o, s );  }
  inline  char*     w_strcpy( char* o, const char* s )          {  return impl::strcpy( o, s );  }

  inline  widechar* w_strncpy( widechar* o, const widechar* s, size_t n )  {  return impl::strncpy( o, s, n );  }
  inline  widechar* w_strncpy( widechar* o, const char* s, size_t n )      {  return impl::strncpy( o, s, n );  }
  inline  char*     w_strncpy( char* o, const char* s, size_t n )          {  return impl::strncpy( o, s, n );  }

  //
  // strcat() family
  //
  inline  widechar* w_strcat( widechar* o, const widechar* s )    {  return impl::strcat( o, s );  }
  inline  widechar* w_strcat( widechar* o, const char* s )        {  return impl::strcat( o, s );  }
  inline  char*     w_strcat( char* o, const char* s )            {  return impl::strcat( o, s );  }

  //
  // strcmp() family
  //
  template <class getchr = impl::getwidechar>
  inline  int   w_strcmp( const char* s, const char* m, getchr l = getchr() )           {  return impl::strcasecmp( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strcmp( const widechar* s, const widechar* m, getchr l = getchr() )   {  return impl::strcasecmp( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strcmp( const char* s, const widechar* m, getchr l = getchr() )       {  return impl::strcasecmp( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strcmp( const widechar* s, const char* m, getchr l = getchr() )       {  return impl::strcasecmp( s, m, l );  }

  //
  // strncmp() family
  //
  template <class getchr = impl::getwidechar>
  inline  int   w_strncmp( const char* s, const char* m, size_t n, getchr l = getchr() )          {  return impl::strncasecmp( s, m, n, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strncmp( const widechar* s, const widechar* m, size_t n, getchr l = getchr() )  {  return impl::strncasecmp( s, m, n, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strncmp( const char* s, const widechar* m, size_t n, getchr l = getchr() )      {  return impl::strncasecmp( s, m, n, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strncmp( const widechar* s, const char* m, size_t n, getchr l = getchr() )      {  return impl::strncasecmp( s, m, n, l );  }

  //
  // strcasecmp() family
  //
  template <class getchr = impl::getwidechar>
  inline  int   w_strcasecmp( const char* s, const char* m, getchr l = getchr() )           {  return impl::strcasecmp( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strcasecmp( const widechar* s, const widechar* m, getchr l = getchr() )   {  return impl::strcasecmp( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strcasecmp( const widechar* s, const char* m, getchr l = getchr() )       {  return impl::strcasecmp( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strcasecmp( const char* s, const widechar* m, getchr l = getchr() )       {  return impl::strcasecmp( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strncasecmp( const char* s, const char* m, int n, getchr l = getchr() )         {  return impl::strncasecmp( s, m, n, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strncasecmp( const widechar* s, const widechar* m, int n, getchr l = getchr() ) {  return impl::strncasecmp( s, m, n, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strncasecmp( const char* s, const widechar* m, int n, getchr l = getchr() )     {  return impl::strncasecmp( s, m, n, l );  }

  template <class getchr = impl::getwidechar>
  inline  int   w_strncasecmp( const widechar* s, const char* m, int n, getchr l = getchr() )     {  return impl::strncasecmp( s, m, n, l );  }

  //
  // strchr() family
  //
  inline  char*     w_strchr( const char* s, int c )              {  return impl::strchr( s, c );  }
  inline  widechar* w_strchr( const widechar* s, int c )          {  return impl::strchr( s, c );  }

  inline  char*     w_strrchr( const char* s, int c )              {  return impl::strrchr( s, c );  }
  inline  widechar* w_strrchr( const widechar* s, int c )          {  return impl::strrchr( s, c );  }

  //
  // strstr family
  //
  template <class getchr = impl::getwidechar>
  inline  const char*     w_strstr( const char* s, const char* m, getchr l = getchr() )         {  return impl::strstr( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  const widechar* w_strstr( const widechar* s, const widechar* m, getchr l = getchr() ) {  return impl::strstr( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  const char*     w_strstr( const char* s, const widechar* m, getchr l = getchr() )     {  return impl::strstr( s, m, l );  }

  template <class getchr = impl::getwidechar>
  inline  const widechar* w_strstr( const widechar* s, const char* m, getchr l = getchr() )     {  return impl::strstr( s, m, l );  }

  //
  // strtod family
  //
  template <class chartype>
  inline  double  w_strtod( const chartype* str, chartype** end )
  {
    double  bigger = 0.0;
    double  decone;
    double  decpow;

    while ( impl::w_is_num( *str ) )
      bigger = (bigger * 10) + *str++ - chartype('0');

    if ( *str++ != '.' )
    {
      if ( end != NULL )
        *end = (chartype*)str - 1;
      return bigger;
    }

    decpow = 1.0;
    decone = 0.0;

    while ( impl::w_is_num( *str ) )
    {
      decone = (decone * 10) + *str++ - chartype('0');
      decpow *= 10.0;
    }

    if ( end != NULL )
      *end = (chartype*)str;
    return bigger + decone / decpow;
  }

  inline  double  w_strtod( const char*     str, char**     end ) {  return w_strtod<char>( str, end );  }
  inline  double  w_strtod( const widechar* str, widechar** end ) {  return w_strtod<widechar>( str, end );  }

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
  // strtoll family
  //
  inline  unsigned long long w_strtoull( const widechar* s, widechar** e, int base )  {  return impl::strtoull( s, e, base );  }
  inline  unsigned long long w_strtoull( const char* s, char** e, int base )          {  return impl::strtoull( s, e, base );  }

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
    (void)store;
    (void)value;
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
    def_alloc m;
    va_list   v;
    char*     p;
    int       l;

    va_copy( v, vaargs );
      p = (char*)m.alloc( l = vsnprintf( nullptr, 0, format, v ) + 1 );
    va_end( v );

    if ( p != nullptr )
      vsnprintf( p, l, format, vaargs );

    return p;
  }

  inline  char* strduprintf( const char* format, ... )
  {
    va_list   vaargs;

    va_start( vaargs, format );
      auto output = vstrduprintf( format, vaargs );
    va_end( vaargs );

    return output;
  }

  inline  std::string vstrprintf( const char* format, va_list vaargs )
  {
    _auto_<char>  output = vstrduprintf( format, vaargs );

    return output != nullptr ? std::string( output.ptr() ) : "";
  }

  inline  std::string strprintf( const char* format, ... )
  {
    va_list   vaargs;

    va_start( vaargs, format );
      auto output = vstrduprintf( format, vaargs );
    va_end( vaargs );

    return output;
  }

// ltrim
  inline  bool        isspace( char c )
  {
    return c != '\0' && (unsigned char)c <= 0x20;
  }

  inline  const char* ltrim( const char* s )
  {
    while ( *s != '\0' && isspace( (unsigned char)*s ) ) ++s;
    return s;
  }

  inline  char*       ltrim( char* s )
  {
    while ( *s != '\0' && isspace( (unsigned char)*s ) ) ++s;
    return s;
  }

}  // mtc namespace

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif  // _MSC_VER

# endif // __mtc_wcsstr_h__
