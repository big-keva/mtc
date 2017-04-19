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
# include <stdint.h>
# include <fcntl.h>
# include <new>

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

# define array_size( a ) ( sizeof(a) / sizeof((a)[0]) )
# define array_end( a ) ((a) + sizeof(a) / sizeof((a)[0]))

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
# if !defined( __char_t_defined__ )
#   define  __char_t_defined__
    typedef char                char_t;
# endif
# if !defined( __byte_t_defined__ )
#   define  __byte_t_defined__
    typedef uint8_t byte_t;
# endif
# if !defined( __word16_t_defined__ )
#   define  __word16_t_defined__
    typedef uint16_t  word16_t;
# endif
# if !defined( __word32_t_defined__ )
#   define  __word32_t_defined__
    typedef uint32_t  word32_t;
# endif
# if !defined( __word64_t_defined__ )
#   define  __word64_t_defined__
    typedef uint64_t  word64_t;
# endif
# if !defined( __float_t_defined__ )
#   define  __float_t_defined__
    typedef float               float_t;
# endif
# if !defined( __double_t_defined__ )
#   define  __double_t_defined__
    typedef double              double_t;
# endif

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
  plain memory allocation interface class
*/
  struct def_alloc
  {
    static  void*   alloc( size_t n ) noexcept  {  return ::malloc( n );  }
    static  void    free( void*  p ) noexcept   {  if ( p ) ::free( p );  }
  };

  template <class T, class M, class... constructor_args>
  T*    allocate_with( M& m, constructor_args... args ) noexcept
    {
      T*  t;

      return (t = (T*)m.alloc( sizeof(T) )) != nullptr ? new( t ) T( args... ) : nullptr;
    }

  template <class T, class... constructor_args>
  T*    allocate( constructor_args... args ) noexcept
    {
      T*  t;

      return (t = (T*)def_alloc().alloc( sizeof(T) )) != nullptr ? new( t ) T( args... ) : nullptr;
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
      pos = begin - start;
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

          if ( rescmp < 0 ) begin = median + 1;
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

}

# endif  // __mtc_platform_h__
