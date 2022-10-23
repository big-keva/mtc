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

# include <climits>
# include <cstring>
# include <cstdlib>
# include <cstdint>
# include <cstdarg>
# include <cstdio>
# include <cassert>
# include <string>
# include <cmath>
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

# if !defined( mtc_charstr_defined )
# define mtc_charstr_defined
  using charstr = std::string;
# endif
# if !defined( mtc_widestr_defined )
# define mtc_widestr_defined
  using widestr = std::basic_string<widechar>;
# endif

  struct getwidechar
  {
    widechar  operator ()( const char*& s ) const     {  return (widechar)(unsigned char)*s++;  }
    widechar  operator ()( const widechar*& s ) const {  return *s++;  }
  };

  template <class Allocator = std::allocator<char>>
  auto  w_strdup( const Allocator& m, const char* s, size_t l = (size_t)-1 ) -> char*;
  template <class Allocator = std::allocator<widechar>>
  auto  w_strdup( const Allocator& m, const widechar* s, size_t l = (size_t)-1 ) -> widechar*;

  template <class getchr = getwidechar>
  auto  w_strcpy( widechar*, const char*, getchr = getchr() ) -> widechar*;

  template <class getchr = getwidechar>
  auto  w_strncpy( widechar*, const char*, size_t, getchr = getchr() ) -> widechar*;

  template <class getchr = getwidechar>
  auto  w_strcat( widechar*, const char*, getchr = getchr() ) -> widechar*;

  template <class getchr = getwidechar>
  int   w_strcmp( const char* s, const widechar* m, getchr = getchr() );
  template <class getchr = getwidechar>
  int   w_strcmp( const widechar* s, const char* m, getchr = getchr() );

  template <class getchr = getwidechar>
  int   w_strncmp( const char*, const widechar*, size_t, getchr = getchr() );
  template <class getchr = getwidechar>
  int   w_strncmp( const widechar*, const char*, size_t, getchr = getchr() );

  template <class getchr = getwidechar>
  int   w_strcasecmp( const widechar*, const char*, getchr = getchr() );
  template <class getchr = getwidechar>
  int   w_strcasecmp( const char*, const widechar*, getchr = getchr() );

  template <class getchr = getwidechar>
  int   w_strncasecmp( const char*, const widechar*, size_t, getchr = getchr() );
  template <class getchr = getwidechar>
  int   w_strncasecmp( const widechar*, const char*, size_t, getchr = getchr() );

  template <class getchr = getwidechar>
  const char*     w_strstr( const char*, const widechar*, getchr = getchr() );
  template <class getchr = getwidechar>
  const widechar* w_strstr( const widechar*, const char*, getchr = getchr() );

  class __impl_strings final
  {
    template <class Allocator>
    class string_formatter
    {
      Allocator allocator;

    public:
      using string_type = std::basic_string<char, std::char_traits<char>, Allocator>;

    public:
      string_formatter( const Allocator& alloc ): allocator( alloc )  {}
      string_formatter( const string_formatter& alloc ): allocator( alloc.allocator )  {}

    protected:
      auto  length( const char* format, va_list vaargs ) const -> size_t
        {
          va_list v;
          size_t  l;

          va_copy( v, vaargs );
            l = vsnprintf( nullptr, 0, format, v );
          va_end( v );

          return l;
        }
      auto  strdup( const char* format, va_list vaargs ) const -> char*
        {
          auto    a( allocator );
          auto    l = length( format, vaargs );
          char*   p;

          p = a.allocate( l + 1 );

          return vsnprintf( p, l + 1, format, vaargs ), p;
        }
      auto  format( const char* format, va_list vaargs ) const -> string_type
        {
          auto    o = std::basic_string<char, std::char_traits<char>, Allocator>( allocator );
          auto    l = length( format, vaargs );

          return o.resize( l ), vsnprintf( const_cast<char*>( o.c_str() ), l + 1, format, vaargs ), o;
        }
    };

    template <class Allocator>
    class strprintf: protected string_formatter<Allocator>
    {
      using string_formatter<Allocator>::string_formatter;

    public:
      auto  operator () ( const char* format, ... ) const -> typename string_formatter<Allocator>::string_type
      {
        va_list vaargs;

        va_start( vaargs, format );
          auto  out = this->format( format, vaargs );
        va_end( vaargs );

        return out;
      }
    };

    template <class Allocator>
    class vstrprintf: protected string_formatter<Allocator>
    {
      using string_formatter<Allocator>::string_formatter;

    public:
      auto  operator () ( const char* format, va_list vaargs ) const -> typename string_formatter<Allocator>::string_type
        {  return string_formatter<Allocator>::format( format, vaargs );  }
    };

    template <class Allocator>
    class strduprintf: protected string_formatter<Allocator>
    {
      using string_formatter<Allocator>::string_formatter;

    public:
      auto  operator () ( const char* format, ... ) const -> char*
        {
          va_list vaargs;

          va_start( vaargs, format );
            auto  out = this->format( format, vaargs );
          va_end( vaargs );

          return out;
        }
    };

    template <class Allocator>
    class vstrduprintf: protected string_formatter<Allocator>
    {
      using string_formatter<Allocator>::string_formatter;

    public:
      auto  operator () ( const char* format, va_list vaargs ) const -> char*
        {  return string_formatter<Allocator>::strdup( format, vaargs );  }
    };

    template <class Allocator>
    friend  auto  strprintf( const Allocator& ) -> strprintf<Allocator>;
    template <class Allocator>
    friend  auto  vstrprintf( const Allocator& ) -> vstrprintf<Allocator>;
    template <class Allocator>
    friend  auto  strduprintf( const Allocator& ) -> strduprintf<Allocator>;
    template <class Allocator>
    friend  auto  vstrduprintf( const Allocator& ) -> vstrduprintf<Allocator>;

  private:
    template <class chartype, class comptype>
    static  bool  w_in_lim( chartype ch, comptype lo, comptype hi )
      {
        using uctype = typename std::make_unsigned<chartype>::type;
        using cmtype = typename std::make_unsigned<comptype>::type;

        return ((uctype)ch) >= ((cmtype)lo) && ((uctype)ch) <= ((cmtype)hi);
      }
    template <class chartype, class compchar>
    static  bool  w_is_chr( chartype ch, compchar cn )
      {  return ch == (chartype)cn;  }
    template <class chartype, class compchar, class... charlist>
    static  bool  w_is_chr( chartype ch, compchar cn, charlist... cl )
      {  return ch == (chartype)cn || w_is_chr( ch, cl... );  }
    template <class chartype>
    static  bool  w_is_num( chartype ch )
      {  return w_in_lim( ch, (chartype)'0', (chartype)'9' );  }

    template <class getchar>
    class to_low_case
    {
      getchar  getc;

    public:
      to_low_case( const getchar& get ): getc( get )  {}

    public:
      widechar  operator ()( const char*& s ) const {  return to_lower( getc( s ) );  }
      widechar  operator ()( const widechar*& s ) const {  return to_lower( getc( s ) );  }

    protected:
      widechar  to_lower( widechar ch ) const
        {
          return w_in_lim( ch, (widechar)'A',   (widechar)'Z'   ) ? ch + ('a' - 'A'):
                 w_in_lim( ch, (widechar)0x401, (widechar)0x42F ) ? ch + (0x430 - 0x401) : ch;
        }
    };

    //
    // strlen() family
    //
    template <class C>  static  size_t  strlen( const C* s )
    {
      auto o = s;
        while ( *s++ != (C)0 )  (void)0;
      return s - o - 1;
    }

    //
    // strdup() family
    //
    template <class C, class Allocator>  static C*  strdup( Allocator& a, const C* s, size_t l )
    {
      C*    o;
      C*    p;

      if ( l == (size_t)-1 )
        for ( l = 0; s != nullptr && s[l] != (C)0; ++l )  (void)0;

      if ( (o = p = a.allocate( l + 1 )) != nullptr )
        {  while ( l-- > 0 ) *p++ = *s++;  *p = (C)0;  }

      return o;
    }

    //
    // strcpy() family
    //
    template <class C>  static  C*  strcpy( C* o, const C* s )
    {
      for ( auto p = o; (*p++ = *s++) != (C)0; )
        (void)0;
      return o;
    }

    template <class C, class L> static  widechar* strcpy( widechar* o, const C* s, L l )
    {
      for ( auto p = o; (*p++ = l( s )) != (widechar)0; )
        (void)0;
      return o;
    }

    //
    // strncpy() family
    //
    template <class C> static  C* strncpy( C* o, const C* s, size_t n )
    {
      auto p = o;

      while ( n > 0 && (*p++ = *s++) != (C)0 )
        --n;
      for ( ; n > 0; --n )
        *p++ = (C)0;
      return o;
    }

    template <class C, class L> static  widechar* strncpy( widechar* o, const C* s, size_t n, L l )
    {
      auto p = o;

      while ( n > 0 && (*p++ = l( s )) != (widechar)0 )
        --n;
      for ( ; n > 0; --n )
        *p = (widechar)0;
      return o;
    }

    //
    // strcat() family
    //
    template <class C> static  C*  strcat( C* o, const C* s )
    {
      auto  p = o;

      while ( *p != (C)0 )
        ++p;
      return strcpy( p, s ), o;
    }

    template <class C, class L> static  widechar* strcat( widechar* o, const C* s, L l )
    {
      auto  p = o;

      while ( *p != (widechar)0 )
        ++p;
      return strcpy( p, s, l ), o;
    }

    //
    // strcmp() family
    //
    template <class A, class B, class L = getwidechar>  static  int strcmp( const A* s, const B* m, L l = L() )
    {
      int       rc;
      widechar  lc;

      while ( (rc = (lc = l( s )) - l( m )) == 0 && lc != 0 )
        (void)0;
      return rc;
    }

    template <class A, class B, class L = getwidechar>  static  int strncmp( const A* s, const B* m, size_t n, L l = L() )
    {
      int       rc = 0;
      widechar  lc;

      while ( n > 0 && (rc = (lc = l( s )) - l( m )) == 0 && lc != 0 )
        --n;
      return rc;
    }

    //
    // strchr() family
    //
    template <class C>  static  C*  strchr( const C* s, int c )
    {
      while ( c != *s && *s != (C)0 )
        ++s;
      return c == *s ? const_cast<C*>( s ) : nullptr;
    }

    template <class C>  static  C*  strrchr( const C* s, int c )
    {
      auto  p = s;

      while ( *p != (C)0 )
        ++p;
      while ( p >= s && *p != c )
        --p;
      return p >= s ? const_cast<C*>( p ) : nullptr;
    }

    //
    // strstr family
    //
    template <class S, class M, class L = getwidechar>  static  auto  strstr( const S* s, const M* m, L l = L() ) -> const S*
    {
      const S*  s1;
      const M*  m1;
      const M*  m2;

      for ( auto  m_ = l( m1 = m ); *s != (S)0; l( s ) )
      {
        while ( *s != (S)0 && l( s1 = s ) != m_ )
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
    // strtox family
    //
    template <class val_type, class chartype>
    static  auto  strtou( const chartype* str, chartype**  end, int dwbase ) -> val_type
    {
      using u_type = typename std::make_unsigned<chartype>::type;

      val_type result = 0;

      if ( dwbase == 0 )
      {
        if ( w_is_chr( *str, '0' ) && w_is_chr( str[1], 'x', 'X' ) )
          {  dwbase = 16; str += 2;  }
        if ( dwbase == 0 )
          {  dwbase = 10;  }
      }

      if ( dwbase == 10 )
      {
        while ( w_is_num( *str ) )
          result = result * 10 + ((u_type)*str++) - '0';
      }
        else
      if ( dwbase == 16 )
      {
        for ( ; ; )
        {
          if ( w_in_lim( *str, '0', '9' ) ) result = result * 16 + ((u_type)*str++) - '0';
            else
          if ( w_in_lim( *str, 'A', 'F' ) ) result = result * 16 + ((u_type)*str++) - 'A' + 10;
            else
          if ( w_in_lim( *str, 'a', 'f' ) ) result = result * 16 + ((u_type)*str++) - 'a' + 10;
            else
          break;
        }
      }
      if ( end != NULL )
        *end = (chartype*)str;
      return result;
    }

    template <class val_type, class chartype>
    static  auto  strtoi( const chartype* str, chartype**  end, int dwbase ) -> val_type
    {
      using u_type = typename std::make_unsigned<chartype>::type;
        static_assert( sizeof(u_type) == sizeof(chartype), "invlid type conversion" );

      val_type result = 0;
      val_type imulti = 1;

      if ( w_is_chr( *str, '-' ) )
        {  imulti = -1;  ++str;  }

      if ( dwbase == 16 )
      {
        if ( w_is_chr( *str, '0' ) && w_is_chr( str[1], 'x', 'X' ) )
          str += 2;
      }
        else
      if ( dwbase == 0 )
      {
        if ( w_is_chr( *str, '0' ) && w_is_chr( str[1], 'x', 'X' ) )
          {  dwbase = 16; str += 2;  }
        else dwbase = 10;
      }

      if ( dwbase == 10 )
      {
        while ( w_is_num( *str ) )
          result = result * 10 + ((u_type)*str++) - '0';
      }
        else
      if ( dwbase == 16 )
      {
        for ( ; ; )
        {
          if ( w_in_lim( *str, '0', '9' ) ) result = result * 16 + ((u_type)*str++) - '0';
            else
          if ( w_in_lim( *str, 'A', 'F' ) ) result = result * 16 + ((u_type)*str++) - 'A' + 10;
            else
          if ( w_in_lim( *str, 'a', 'f' ) ) result = result * 16 + ((u_type)*str++) - 'a' + 10;
            else
          break;
        }
      }
      if ( end != NULL )
        *end = const_cast<chartype*>( str );
      return imulti * result;
    }

    friend  size_t w_strlen( const widechar* );
    friend  size_t w_strlen( const char* );

    template <class Allocator>
    friend  auto  w_strdup( const Allocator& m, const char* s, size_t l ) -> char*;
    template <class Allocator>
    friend  auto  w_strdup( const Allocator& m, const widechar* s, size_t l ) -> widechar*;

    friend  auto  w_strcpy( widechar* o, const widechar* s ) -> widechar*;
    friend  auto  w_strcpy( char* o, const char* s ) -> char*;

    template <class getchr>
    friend  auto  w_strcpy( widechar*, const char*, getchr l ) -> widechar*;

    friend  auto  w_strncpy( char*, const char*, size_t ) -> char*;
    friend  auto  w_strncpy( widechar*, const widechar*, size_t ) -> widechar*;

    template <class getchr>
    friend  auto  w_strncpy( widechar*, const char*, size_t n, getchr ) -> widechar*;

    friend  auto  w_strcat( widechar* o, const widechar* s ) -> widechar*;
    friend  auto  w_strcat( char* o, const char* s ) -> char*;

    template <class getchr>
    friend  auto  w_strcat( widechar*, const char*, getchr ) -> widechar*;

    friend  int   w_strcmp( const char*, const char* );
    friend  int   w_strcmp( const widechar*, const widechar* );

    template <class getchr>
    friend  int   w_strcmp( const char*, const widechar*, getchr l );
    template <class getchr>
    friend  int   w_strcmp( const widechar*, const char*, getchr l );

    friend  int   w_strncmp( const char*, const char*, size_t );
    friend  int   w_strncmp( const widechar*, const widechar*, size_t );

    template <class getchr>
    friend  int   w_strncmp( const char*, const widechar*, size_t, getchr );
    template <class getchr>
    friend  int   w_strncmp( const widechar*, const char*, size_t, getchr );

    friend  int   w_strcasecmp( const char*, const char* );
    friend  int   w_strcasecmp( const widechar*, const widechar* );

    template <class getchr>
    friend  int   w_strcasecmp( const widechar*, const char*, getchr );
    template <class getchr>
    friend  int   w_strcasecmp( const char*, const widechar*, getchr );

    friend  int   w_strncasecmp( const char*, const char*, size_t );
    friend  int   w_strncasecmp( const widechar*, const widechar*, size_t );

    template <class getchr>
    friend  int   w_strncasecmp( const char*, const widechar*, size_t, getchr );
    template <class getchr>
    friend  int   w_strncasecmp( const widechar*, const char*, size_t, getchr );

    template <class getchr>
    friend  int   w_strncasecmp( const char*, const widechar*, size_t, getchr );
    template <class getchr>
    friend  int   w_strncasecmp( const widechar*, const char*, size_t, getchr );

    friend  char*     w_strchr( const char*, int );
    friend  widechar* w_strchr( const widechar*, int );
  
    friend  char*     w_strrchr( const char*, int );
    friend  widechar* w_strrchr( const widechar*, int );

    friend  const char*     w_strstr( const char*, const char* );
    friend  const widechar* w_strstr( const widechar*, const widechar* );

    template <class getchr>
    friend  const char*     w_strstr( const char*, const widechar*, getchr );
    template <class getchr>
    friend  const widechar* w_strstr( const widechar*, const char*, getchr );

    template <class chartype>
    friend  double    w_strtod( const chartype*, chartype** );

    friend  auto  w_strtol( const char*, char**, int ) -> long int;
    friend  auto  w_strtol( const widechar*, widechar**, int ) -> long int;

    friend  auto  w_strtoul( const char*, char**, int ) -> unsigned long int;
    friend  auto  w_strtoul( const widechar*, widechar**, int ) -> unsigned long int;

    friend  auto  w_strtoll( const char*, char**, int ) -> long long int;
    friend  auto  w_strtoll( const widechar*, widechar**, int ) -> long long int;

    friend  auto  w_strtoull( const char*, char**, int ) -> unsigned long long int;
    friend  auto  w_strtoull( const widechar*, widechar**, int ) -> unsigned long long int;
  };

  //
  // strlen() family
  //
  inline  size_t w_strlen( const widechar* s )  {  return __impl_strings::strlen( s );  }
  inline  size_t w_strlen( const char* s )      {  return __impl_strings::strlen( s );  }

  //
  // strdup() family
  //
  template <class Allocator>
  auto  w_strdup( const Allocator& m, const char* s, size_t l ) -> char*
    {
      auto  a( m );
      return s != nullptr ? __impl_strings::strdup( a, s, l ) : nullptr;
    }

  template <class Allocator>
  auto  w_strdup( const Allocator& m, const widechar* s, size_t l ) -> widechar*
    {
      auto  a( m );
      return s != nullptr ? __impl_strings::strdup( a, s, l ) : nullptr;
    }

  inline  char*     w_strdup( const char* s, size_t l = (size_t)-1 )
    {  return w_strdup( std::allocator<char>(), s, l );  }
  inline  widechar* w_strdup( const widechar* s, size_t l = (size_t)-1 )
    {  return w_strdup( std::allocator<widechar>(), s, l );  }

  //
  // strcpy() family
  //
  inline  widechar* w_strcpy( widechar* o, const widechar* s )  {  return __impl_strings::strcpy( o, s );  }
  inline  char*     w_strcpy( char* o, const char* s )          {  return __impl_strings::strcpy( o, s );  }

  template <class getchr>
  inline  widechar* w_strcpy( widechar* o, const char* s, getchr l ) {  return __impl_strings::strcpy( o, s, l );  }

  inline  widechar* w_strncpy( widechar* o, const widechar* s, size_t n )  {  return __impl_strings::strncpy( o, s, n );  }
  inline  char*     w_strncpy( char* o, const char* s, size_t n )          {  return __impl_strings::strncpy( o, s, n );  }

  template <class getchr>
  inline  widechar* w_strncpy( widechar* o, const char* s, size_t n, getchr l ) {  return __impl_strings::strncpy( o, s, n, l );  }

  //
  // strcat() family
  //
  inline  widechar* w_strcat( widechar* o, const widechar* s )    {  return __impl_strings::strcat( o, s );  }
  inline  char*     w_strcat( char* o, const char* s )            {  return __impl_strings::strcat( o, s );  }

  template <class getchr>
  inline  widechar* w_strcat( widechar* o, const char* s, getchr l ) {  return __impl_strings::strcat( o, s, l );  }

  //
  // strcmp() family
  //
  inline  int   w_strcmp( const char* s, const char* m )  {  return __impl_strings::strcmp( s, m, getwidechar() );  }
  inline  int   w_strcmp( const widechar* s, const widechar* m )  {  return __impl_strings::strcmp( s, m, getwidechar() );  }

  template <class getchr>
  inline  int   w_strcmp( const char* s, const widechar* m, getchr l )       {  return __impl_strings::strcmp( s, m, l );  }
  template <class getchr>
  inline  int   w_strcmp( const widechar* s, const char* m, getchr l )       {  return __impl_strings::strcmp( s, m, l );  }

  //
  // strncmp() family
  //
  inline  int   w_strncmp( const char* s, const char* m, size_t n ) {  return __impl_strings::strncmp( s, m, n, getwidechar() );  }
  inline  int   w_strncmp( const widechar* s, const widechar* m, size_t n ) {  return __impl_strings::strncmp( s, m, n, getwidechar() );  }

  template <class getchr>
  inline  int   w_strncmp( const char* s, const widechar* m, size_t n, getchr l ) {  return __impl_strings::strncmp( s, m, n, l );  }
  template <class getchr>
  inline  int   w_strncmp( const widechar* s, const char* m, size_t n, getchr l ) {  return __impl_strings::strncmp( s, m, n, l );  }

  //
  // strcasecmp() family
  //
  inline  int   w_strcasecmp( const char* s, const char* m )
    {  return __impl_strings::strcmp( s, m, __impl_strings::to_low_case<getwidechar>( getwidechar() ) );  }
  inline  int   w_strcasecmp( const widechar* s, const widechar* m )
    {  return __impl_strings::strcmp( s, m, __impl_strings::to_low_case<getwidechar>( getwidechar() ) );  }

  template <class getchr>
  inline  int   w_strcasecmp( const widechar* s, const char* m, getchr l )
    {  return __impl_strings::strcmp( s, m, __impl_strings::to_low_case<getchr>( l ) ); }
  template <class getchr>
  inline  int   w_strcasecmp( const char* s, const widechar* m, getchr l )
    {  return __impl_strings::strcmp( s, m, __impl_strings::to_low_case<getchr>( l ) ); }

  inline  int   w_strncasecmp( const char* s, const char* m, size_t n )
    {  return __impl_strings::strncmp( s, m, n, __impl_strings::to_low_case<getwidechar>( getwidechar() ) );  }
  inline  int   w_strncasecmp( const widechar* s, const widechar* m, size_t n ) 
    {  return __impl_strings::strncmp( s, m, n, __impl_strings::to_low_case<getwidechar>( getwidechar() ) );  }

  template <class getchr>
  inline  int   w_strncasecmp( const char* s, const widechar* m, size_t n, getchr l )
    {  return __impl_strings::strncmp( s, m, n, __impl_strings::to_low_case<getchr>( l ) ); }
  template <class getchr>
  inline  int   w_strncasecmp( const widechar* s, const char* m, size_t n, getchr l )
    {  return __impl_strings::strncmp( s, m, n, __impl_strings::to_low_case<getchr>( l ) ); }

  //
  // strchr() family
  //
  inline  char*     w_strchr( const char* s, int c )      {  return __impl_strings::strchr( s, c ); }
  inline  widechar* w_strchr( const widechar* s, int c )  {  return __impl_strings::strchr( s, c ); }

  inline  char*     w_strrchr( const char* s, int c )     {  return __impl_strings::strrchr( s, c ); }
  inline  widechar* w_strrchr( const widechar* s, int c ) {  return __impl_strings::strrchr( s, c ); }

  //
  // strstr family
  //
  inline  const char*     w_strstr( const char* s, const char* m )  {  return __impl_strings::strstr( s, m ); }
  inline  const widechar* w_strstr( const widechar* s, const widechar* m )  {  return __impl_strings::strstr( s, m ); }

  template <class getchr>
  inline  const char*     w_strstr( const char* s, const widechar* m, getchr l )  {  return __impl_strings::strstr( s, m, l );  }
  template <class getchr>
  inline  const widechar* w_strstr( const widechar* s, const char* m, getchr l )  {  return __impl_strings::strstr( s, m, l );  }

  //
  // strtod family
  //
  template <class chartype>
  inline  double  w_strtod( const chartype* str, chartype** end )
  {
    auto      checkp = []( const chartype*& p, chartype c ) {  bool  b = *p == c;  if ( b ) ++p;  return b;  };
    bool      bminus = checkp( str, '-' );
    bool      bfloat = checkp( str, '.' );
    double    dvalue;

    if ( !__impl_strings::w_is_num( *str ) )
    {
      if ( end != NULL )
        *end = const_cast<chartype*>( str ) - 1;
      return 0.0;
    }

    if ( !bfloat )
    {
      uint64_t  uvalue;

      for ( uvalue = 0;  __impl_strings::w_is_num( *str ); ++str )
        uvalue = uvalue * 10 + *str - '0';

      if ( *str != '.' && *str != 'E' && *str != 'e' )
      {
        if ( end != nullptr )
          *end = const_cast<chartype*>( str );
        return bminus ? -1.0 * uvalue : uvalue;
      }

      bfloat = checkp( str, '.' );

      dvalue = (double)uvalue;
    }
      else
    dvalue = 0.0;

    if ( bfloat )
    {
      double    drange = 1.0;
      uint64_t  uvalue;

      for ( uvalue = 0; __impl_strings::w_is_num( *str ); ++str, drange *= 10 )
        uvalue = uvalue * 10 + *str - '0';

      if ( drange >= 10.0 )
        dvalue += uvalue / drange;
    }

    if ( ((*str == 'e' || *str == 'E') && __impl_strings::w_is_num( str[1] )) || str[1] == '-' )
    {
      bool    divide;
      double  fpower;

      if ( (divide = *++str == '-') )
        ++str;
      for ( fpower = 0.0; __impl_strings::w_is_num( *str ); ++str )
        fpower = fpower * 10 + *str - '0';

      if ( divide )
        dvalue /= ::pow( 10.0, fpower );
      else
        dvalue *= ::pow( 10.0, fpower );
    }

    if ( end != nullptr )
      *end = const_cast<chartype*>( str );

    return (bminus ? -1 : 1) * dvalue;
  }

  inline  double  w_strtod( const char*     str, char**     end ) {  return w_strtod<char>( str, end );  }
  inline  double  w_strtod( const widechar* str, widechar** end ) {  return w_strtod<widechar>( str, end );  }

  //
  // strtox family
  //
  inline  auto  w_strtol( const char* s, char** e, int base ) -> long int
    {  return __impl_strings::strtoi<long int>( s, e, base );  }
  inline  auto  w_strtol( const widechar* s, widechar** e, int base ) -> long int
    {  return __impl_strings::strtoi<long int>( s, e, base );  }

  inline  auto  w_strtoul( const char* s, char** e, int base ) -> unsigned long int
    {  return __impl_strings::strtoi<unsigned long int>( s, e, base );  }
  inline  auto  w_strtoul( const widechar* s, widechar** e, int base ) -> unsigned long int
    {  return __impl_strings::strtoi<unsigned long int>( s, e, base );  }

  inline  auto  w_strtoll( const char* s, char** e, int base ) -> long long int
    {  return __impl_strings::strtoi<long long int>( s, e, base );  }
  inline  auto  w_strtoll( const widechar* s, widechar** e, int base ) -> long long int
    {  return __impl_strings::strtoi<long long int>( s, e, base );  }

  inline  auto  w_strtoull( const char* s, char** e, int base ) -> unsigned long long int
    {  return __impl_strings::strtoi<unsigned long long int>( s, e, base );  }
  inline  auto  w_strtoull( const widechar* s, widechar** e, int base ) -> unsigned long long int
    {  return __impl_strings::strtoi<unsigned long long int>( s, e, base );  }

  //
  // char* strduprintf( format, ... ) family
  //
  template <class Allocator>
  inline  auto  vstrduprintf( const Allocator& alloc ) -> __impl_strings::vstrduprintf<Allocator>
    {  return __impl_strings::vstrduprintf<Allocator>( alloc );  }
  template <class Allocator>
  inline  auto  strduprintf( const Allocator& alloc ) -> __impl_strings::strduprintf<Allocator>
    {  return __impl_strings::strduprintf<Allocator>( alloc );  }

  inline  char* vstrduprintf( const char* format, va_list vaargs )
    {  return vstrduprintf( std::allocator<char>() )( format, vaargs );  }

  inline  char* strduprintf( const char* format, ... )
    {
      va_list   vaargs;

      va_start( vaargs, format );
        auto output = vstrduprintf( format, vaargs );
      va_end( vaargs );

      return output;
    }

  template <class Allocator>
  inline  auto  vstrprintf( const Allocator& alloc ) -> __impl_strings::vstrprintf<Allocator>
    {  return __impl_strings::vstrprintf<Allocator>( alloc );  }
  template <class Allocator>
  inline  auto  strprintf( const Allocator& alloc ) -> __impl_strings::strprintf<Allocator>
    {  return __impl_strings::strprintf<Allocator>( alloc );  }

  inline  auto  vstrprintf( const char* format, va_list vaargs ) -> std::string
    {  return mtc::vstrprintf( std::allocator<char>() )( format, vaargs );  }

  inline  auto  strprintf( const char* format, ... ) -> std::string
    {
      va_list vaargs;

      va_start( vaargs, format );
        auto  out = vstrprintf( format, vaargs );
      va_end( vaargs );

      return out;
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
