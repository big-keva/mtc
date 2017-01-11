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
# include <new>

# if !defined( win32_decl )
#   if defined( WIN32 )
#     define win32_decl( expr ) expr
#   else
#     define win32_decl( expr )
#   endif
# endif

# if !defined( posix_decl )
#   if defined( WIN32 )
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

# if defined( _MSC_VER )
#   if _MSC_VER >= 12
#     define  strcasecmp      _strcmpi
#     define  strncasecmp     strnicmp
//#     define  open            _open
#     define  fdopen          _fdopen
# else
#     define  strcasecmp  strcmpi
#     define  strncasecmp strnicmp
# endif  // >= 12
#endif  // _MSC_VER

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
    void*   alloc( size_t n ) noexcept  {  return ::malloc( n );  }
    void    free( void*  p ) noexcept   {  if ( p ) ::free( p );  }
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
  void  deallocate( T* p )
    {
      if ( p != nullptr )
      {
        p->~T();
        def_alloc().free( (void*)p );
      }
    }

}

# endif  // __mtc_platform_h__
