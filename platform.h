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
# include <new>

# define array_size( a ) ( sizeof(a) / sizeof((a)[0]) )
# define array_end( a ) ((a) + sizeof(a) / sizeof((a)[0]))

# if defined( _MSC_VER )
#   define  strcasecmp  strcmpi
#   define  strncasecmp strnicmp
#endif  // _MSC_VER

namespace mtc
{
  typedef char                char_t;
  typedef unsigned char       byte_t;
  typedef short               int16_t;
  typedef unsigned short      word16_t;
  typedef int                 int32_t;
  typedef unsigned int        word32_t;
  typedef long long           int64_t;
  typedef unsigned long long  word64_t;
  typedef float               float_t;
  typedef double              double_t;

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

/*
  template <class T>  inline  T*  allocate()
    {
      T*  t;

      return (t = (T*)malloc( sizeof(T) )) != nullptr ? new( t ) T() : nullptr;
    }
  template <class T, class A>  inline  T*  allocate( A a )
    {
      T*  t;

      return (t = (T*)malloc( sizeof(T) )) != nullptr ? new( t ) T( a ) : nullptr;
    }
  template <class T, class A, class B>  inline  T*  allocate( A a, B b )
    {
      T*  t;

      return (t = (T*)malloc( sizeof(T) )) != nullptr ? new( t ) T( a, b ) : nullptr;
    }
  template <class T, class A, class B, class C>  inline  T*  allocate( A a, B b, C c )
    {
      T*  t;

      return (t = (T*)malloc( sizeof(T) )) != nullptr ? new( t ) T( a, b, c ) : nullptr;
    }
  template <class T, class A, class B, class C, class D>  inline  T*  allocate( A a, B b, C c, D d )
    {
      T*  t;

      return (t = (T*)malloc( sizeof(T) )) != nullptr ? new( t ) T( a, b, c, d ) : nullptr;
    }
  template <class T, class A, class B, class C, class D, class E>  inline  T*  allocate( A a, B b, C c, D d, E e )
    {
      T*  t;

      return (t = (T*)malloc( sizeof(T) )) != nullptr ? new( t ) T( a, b, c, d, e ) : nullptr;
    }
*/
}

# endif  // __mtc_platform_h__
