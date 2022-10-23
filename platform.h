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
# if !defined( __mtc_platform_h__ )
# define __mtc_platform_h__
# include <cstdlib>
# include <cstdint>
# include <climits>
# include <limits>
# include <fcntl.h>
# include <new>

# if !defined( __widechar_defined__ )
# define  __widechar_defined__
#   if defined(WCHAR_MAX) && (WCHAR_MAX >> 16) == 0
    typedef wchar_t         widechar;
#   else
    typedef unsigned short  widechar;
#   endif  // size
# endif  // __widechar_defined__

# if !defined( win32_decl )
#   if defined( _WIN32 )
#     define win32_decl( expr ) expr
#   else
#     define win32_decl( expr )
#   endif
# endif

# if !defined( posix_decl )
#   if defined( _WIN32 )
#     define posix_decl( expr )
#   else
#     define posix_decl( expr ) expr
#   endif
# endif

# if !defined( debug_decl )
#   if defined( _DEBUG )
#     define debug_decl( expr ) expr
#   else
#     define debug_decl( expr )
#   endif
# endif

# define array_size( a )  (size_t)(sizeof(a) / sizeof(0[(a)]))
# define array_end( a )           (sizeof(a) / sizeof(0[(a)]) + (a))

# if !defined( strcasecmp )
#   if defined( _WIN32 )
#     define  strcasecmp  _strcmpi
#   endif
# endif  // !strcasecmp

# if !defined( strncasecmp )
#   if defined( _WIN32 )
#     define  strncasecmp _strnicmp
#   endif
# endif  // !strncasecmp

# if !defined( fdopen )
#   if defined( _WIN32 )
#     define  fdopen      _fdopen
#   endif
# endif  // !fdopen

# if defined( _WIN32 ) && __STDC__
#   if !defined( O_RDONLY )
#     define  O_RDONLY      _O_RDONLY
#     define  O_WRONLY      _O_WRONLY
#     define  O_APPEND      _O_APPEND
#     define  O_RDWR        _O_RDWR
#     define  O_CREAT       _O_CREAT
#     define  O_EXCL        _O_EXCL
#     define  O_TRUNC       _O_TRUNC
#     define  O_RANDOM      _O_RANDOM
#     define  O_SEQUENTIAL  _O_SEQUENTIAL
#     define  O_TEMPORARY   _O_TEMPORARY
#   endif

#   if !defined( open )
#     define  open  _open
#   endif

#   if !defined( close )
#     define  close _close
#   endif

# endif

# if !defined( O_BINARY )
#   define O_BINARY 0
# endif  // !O_BINARY

namespace mtc
{
  using char_t = char;
  using byte_t = uint8_t;
  using word16_t = uint16_t;
  using word32_t = uint32_t;
  using word64_t = uint64_t;
  using float_t = float;
  using double_t = double;

  template <class T>  inline  T max( const T& t1, const T& t2 )
    {  return t1 >= t2 ? t1 : t2;  }
  template <class T>  inline  T min( const T& t1, const T& t2 )
    {  return t1 <  t2 ? t1 : t2;  }

  template <class T>
  void  inplace_swap( T& t1, T& t2 )
    {
      char* c1 = (char*)&t1;
      char* c2 = (char*)&t2;
      int   cc = sizeof(t1);

      while ( cc-- > 0 )  {  char c = *c1;  *c1++ = *c2;  *c2++ = c;  }
    }

/*
  exceptions switch
*/
  class enable_exceptions_t {};
  class disable_exceptions_t{};

  constexpr enable_exceptions_t   enable_exceptions {};
  constexpr disable_exceptions_t  disable_exceptions{};

/*
  plain memory allocation interface class
*/
  struct nothrow_allocator
  {
    static  void*   alloc( size_t n ) noexcept  {  return ::new( std::nothrow ) char[n];  }
    static  void    free( void*  p ) noexcept   {  if ( p ) ::delete[]( (char*)p );       }
  };

  struct std_cxx_allocator
  {
    static  void*   alloc( size_t n )           {  return ::new char[n];                  }
    static  void    free( void*  p )            {  if ( p ) ::delete[]( (char*)p );       }
  };

# if defined( MTC_NO_EXCEPTIONS )
  using  def_alloc = nothrow_allocator;
# else
  using  def_alloc = std_cxx_allocator;
# endif   // MTC_NO_EXCEPTIONS

  template <class T, class M, class ... constructor_args>
  T*  allocate_with( M& m, constructor_args ... args )
  {
    T*  t = (T*)m.alloc( sizeof(T) );
      if ( t != nullptr ) new( t ) T( args... );
    return t;
  }

  template <class M, class T, class ... constructor_args>
  T*  allocate_with( constructor_args ... args )
  {
    M m;
    return allocate_with<T, M>( m, args... );
  }

  template <class T, class ... constructror_args>
  T*  allocate( constructror_args ... args )
  {
    return allocate_with<def_alloc, T>( args... );
  }

  template <class M, class T>
  void  deallocate_with( M& m, T* p ) noexcept
    {
      if ( p != nullptr )
      {
        p->~T();
        m.free( (void*)p );
      }
    }

  template <class M, class T>
  void  deallocate_with( T* p ) noexcept
    {
      M m;
      deallocate_with( m, p );
    }

  template <class M = def_alloc, class T>
  void  deallocate( T* p )
    {
      deallocate_with<M>( p );
    }

/* array searchers */
  template <class T>
  T*  Lookup( T* begin, T* end, const T& match )
    {
      while ( begin < end )
        if ( match == *begin )  break;  else ++begin;
      return begin;
    }

  template <class T, class _test>
  T*  Lookup( T* begin, T* end, _test test )
    {
      while ( begin < end )
        if ( test( *begin ) ) break;  else ++begin;
      return begin;
    }

  template <class T>
  bool    Search( const T* begin, const T* end, const T& match, int& pos )
    {
      const T*  start;
      bool      found = false;

      if ( (start = begin) < end-- )
        for ( found = false; begin <= end; )
        {
          const T*  median = begin + ((end - begin) >> 1);

          if ( *median < match )  begin = median + 1;
            else
          {
            end = median - 1;
            found |= *median == match;
          }
        }
      pos = (int)(begin - start);
      return found;
    }

  template <class T, class _comp>
  bool    Search( const T* begin, const T* end, _comp comp, int& pos )
    {
      const T*  start;
      bool      found = false;

      if ( (start = begin) < end-- )
        for ( found = false; begin <= end; )
        {
          const T*  median = begin + ((end - begin) >> 1);
          int       rescmp = comp( *median );

          if ( rescmp > 0 ) begin = median + 1;
            else
          {
            end = median - 1;
            found |= rescmp == 0;
          }
        }
      pos = (int)(begin - start);
      return found;
    }

  template <class T, class _func>
  int     for_each( T* begin, T* end, _func func )
    {
      int e;

      for ( auto next = begin; next < end; ++next )
        if ( (e = func( *next )) != 0 ) return e;
      return 0;
    }

  template <class T, class _func>
  void    for_all( T* begin, T* end, _func func )
    {
      for ( auto next = begin; next < end; )
        func( *next++ );
    }

//
// range - работа с интервалами
//
  template <class U>
  class range
  {
  public:
    U   l;   // левая
    U   h;   // правая

  public:
    range() noexcept: l( 0 ), h( std::numeric_limits<U>().max() ) {}
    range( U lo, U up ): l( lo ), h( up ) {}
    range( U lh ): l( lh ), h( lh ) {}

  public:     // operators
    auto  operator &= ( const range& r ) -> range&
      {
        l = max( l, r.l );  
        h = min( h, r.h );
        return *this;
      }
    auto  operator |= ( const range& r ) -> range&
      {
        l = min( l, r.l );
        h = max( h, r.h );
        return *this;
      }
    auto  operator & ( const range& r ) const -> range
      {
        range x( *this );
        return x &= r;
      }
    auto  operator | ( const range& r ) const -> range
      {
        range x( *this );
        return x |= r;
      }
    bool  operator == ( const range& r ) const {  return l == r.l && h == r.h;  }
    bool  operator != ( const range& r ) const {  return !(*this == r);  }

  public:
    template <class shift>
    range move( shift s ) {  return range( s + l, s + h );  }
    int   size() const  {  return h - l + 1;  }
    
  };

  template <class U>
  range<U>  make_range( U l, U h )  {  return range<U>( l, h );  }

  template <class U>
  range<U>  make_range( U lh )  {  return range<U>( lh );  }

}

# endif  // __mtc_platform_h__
