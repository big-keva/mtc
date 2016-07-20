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
# if !defined( __mtc_auto_ptr_h__ )
# define  __mtc_auto_ptr_h__
# include "platform.h"
# include <stdlib.h>
# include <assert.h>

namespace mtc
{

  template <class T, class M = def_alloc<>>
  class _auto_
  {
    T*  p;

  public:
    _auto_( T* v = nullptr ): p( v )
      {
      }
    _auto_( const _auto_& a ): p( a.p )
      {
        ((_auto_<T, M>&)a).p = nullptr;
      }
   ~_auto_()
      {
        M().deallocate( p );
      }
    _auto_& operator = ( const _auto_& a )
      {
        M().deallocate( p );
          p = a.p;  ((_auto_<T, M>&)a).p = nullptr;
        return *this;
      }
    _auto_& operator = ( T* a )
      {
        if ( p )
          M().deallocate( p );
        p = a;
          return *this;
      }
    operator T* ()              {  return p;  }
    operator const T* () const  {  return p;  }
    operator T** ()             {  return &p; }
    operator const T** () const {  return &p; }
    T* ptr()                {  return p;  }
    const T* ptr() const    {  return p;  }
    T** pptr()              {  return &p; }
    const T** pptr() const  {  return &p; }

    bool  operator == ( const void* v ) const {  return this == v || (this != nullptr && p == v);  }
    bool  operator != ( const void* v ) const {  return (*this == v) == false;  }
        
    T*  operator -> ()              {  assert( p != nullptr );  return p;  }
    const T* operator -> () const   {  assert( p != nullptr );  return p;  }
    T*  detach()
      {
        T*  r = p;
            p = 0;
        return r;
      }

    const T& operator []( int i ) const
      {
        assert( p != nullptr );
        return p[i];
      }
    T& operator []( int i )
      {
        assert( p != nullptr );
        return p[i];
      }
  };

}  // mtc namespace

# endif  // __mtc_auto_ptr_h__
