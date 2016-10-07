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
# if !defined( __mtc_platform_h__ )
# define __mtc_platform_h__
# include <cstdlib>
# include <stdint.h>
# include <new>

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

  class rtl_mm
  {
    public: void*   alloc( size_t n ) noexcept  {  return ::malloc( n );  }
    public: void    free( void*  p ) noexcept   {  if ( p ) ::free( p );  }
  };

  template <class mm = rtl_mm>
  class def_alloc: public mm
  {
  public:     // objects allocation
    template <class T, class... constructor_args>
    T*  allocate( constructor_args... args ) noexcept
      {
        T*  t;

        return (t = (T*)mm::alloc( sizeof(T) )) != nullptr ? new( t ) T( args... ) : nullptr;
      }
    template <class T>
    void          deallocate( T* p ) noexcept
      {
        if ( p != nullptr )
        {
          p->~T();
          mm::free( (void*)p );
        }
      }
  };

  template <class T, class... constructor_args>
  T*  allocate( constructor_args... args ) noexcept
    {
      T*  t;

      return (t = (T*)malloc( sizeof(T) )) != nullptr ? new( t ) T( args... ) : nullptr;
    }

}

# endif  // __mtc_platform_h__
