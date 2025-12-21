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
# if !defined( __interfaces_h__ )
# define  __interfaces_h__
# include "platform.h"
# include <cassert>
# include <cstddef>
# include <atomic>
# include <mutex>
# include <type_traits>

namespace mtc
{

  struct  Iface
  {
    virtual long  Attach() = 0;
    virtual long  Detach() = 0;

  protected:
   ~Iface() {}
  };

  template <class iface>  class API;
  template <class iface>  class api;

  class api_impl
  {
  protected:
    template <class iface>
      static  long  attach( iface* p )
        {  return ((typename std::remove_const<iface>::type*)p)->Attach();  }
    template <class iface>
      static  long  detach( iface* p )
        {  return ((typename std::remove_const<iface>::type*)p)->Detach();  }
  };

  /*
    non-thread-safe api pointer; is faster but may not be used for pointers can be modified while working
  */
  template <class iface>
  class api: public api_impl
  {
    mutable iface*  piface;

  protected:
    api( decltype(API<iface>( nullptr ).ptr()) p ):
      piface( p != nullptr && attach( p ) > 0 ? p : nullptr ) {}
    api&  operator = ( decltype(API<iface>( nullptr ).ptr()) p )
    {
      if ( piface != nullptr )
        detach( piface );
      piface = p != nullptr && attach( p ) > 0 ? p : nullptr;
        return *this;
    }

  public:       // construction/destruction
    api( iface* p = nullptr ) noexcept:
      piface( p != nullptr && attach( p ) > 0 ? p : nullptr ) {}
    api( const api& a ) noexcept:
      api( a.piface ) {}
    api( api&& a ) noexcept:
      piface( a.piface )  {  a.piface = nullptr;  }
    api( const API<iface>& a ) noexcept:
      api( a.ptr() ) {}
   ~api()
    {
      if ( piface != nullptr )
        detach( piface );
    }

  public:     // operators
    api& operator = ( iface* p )
    {
      if ( piface != nullptr )
        detach( piface );
      piface = p != nullptr && attach( p ) > 0 ? p : nullptr;
        return *this;
    }
    api& operator = ( api&& a ) noexcept
    {
      if ( piface != nullptr )
        detach( piface );
      if ( (piface = a.piface) != nullptr )
        a.piface = nullptr;
      return *this;
    }
    api& operator = ( const api& a )  {  return operator=( a.piface );  }
    api& operator = ( const API<iface>& a )  {  return operator=( a.ptr() );  }

  public:     // methods
    iface*  release()
    {
      auto  p = piface;
        piface = nullptr;
      return p;
    }

  public:     // conversions
    iface*  operator -> () const {  assert( piface != nullptr );  return piface;  }
    iface*  ptr() const          {  return piface;  }

    operator iface*() const {  return piface;  }
    operator void**() {  return reinterpret_cast<void**>( &piface );  }
    operator iface**()  {  return &piface;  }

  };

  template <class i>
  bool  operator == ( const api<i>& i1, const api<i>& i2 ) {  return i1.ptr() == i2.ptr();  }
  template <class i>
  bool  operator != ( const api<i>& i1, const api<i>& i2 ) {  return !(i1 == i2);  }

  template <class i>
  bool  operator == ( const api<i>& ci, std::nullptr_t np ) {  return ci.ptr() == np;  }
  template <class i>
  bool  operator != ( const api<i>& ci, std::nullptr_t np ) {  return !(ci == np);  }

  template <class i>
  bool  operator == ( std::nullptr_t np, const api<i>& ci ) {  return ci.ptr() == np;  }
  template <class i>
  bool  operator != ( std::nullptr_t np, const api<i>& ci ) {  return !(ci == np);  }

  template <class iface>
  class API: public api_impl
  {
    using usemutex = std::recursive_mutex;
    using autolock = std::lock_guard<usemutex>;

    mutable usemutex  locker;
    mutable iface*    piface;

    template <class _do>  auto interlocked( _do do_ ) -> decltype(do_())
      {  autolock  aulock( locker );  return do_();  }
    template <class _do>  auto interlocked( _do do_ ) const -> decltype(do_())
      {  autolock  aulock( locker );  return do_();  }

  protected:

    class pvalue
    {
      friend class API;

    protected:
      pvalue( iface* p )
        {
          if ( (piface = p) != nullptr )
            getptr()->Attach();
        }
      
    public:
      pvalue( const pvalue& p )
        {
          if ( (piface = p.piface) != nullptr )
            getptr()->Attach();
        }
     ~pvalue()
        {
          if ( piface != nullptr )
            getptr()->Detach();
        }
      pvalue& operator = ( const pvalue& p )
        {
          if ( piface != nullptr )
            getptr()->Detach();
          if ( (piface = p.piface) != nullptr )
            getptr()->Attach();
          return *this;
        }

    public:
      operator iface* () const          {  return piface;  }
      iface*  operator -> ()            {  assert( piface != nullptr );  return piface;  }
      const iface* operator -> () const {  assert( piface != nullptr );  return piface;  }
      bool  operator == ( const void* p ) const {  return piface == p;  }
      bool  operator != ( const void* p ) const {  return !(*this == p);  }

    protected:
      auto    getptr() -> typename std::remove_const<iface>::type* {  return (typename std::remove_const<iface>::type*)piface;  }

    protected:
      mutable iface*  piface;

    };

  protected:
    API( pvalue p ):
      piface( p != nullptr && attach( p.piface ) > 0 ? p : nullptr ) {}
  public:       // construction/destruction
    API( iface* p = nullptr ):
      piface( p != nullptr && attach( p ) > 0 ? p : nullptr ) {}
    API( const API& a ):
      API( a.ptr() )  {}
    API( const api<iface>& a ):
      API( a.ptr() )  {}
   ~API()
    {
      if ( piface != nullptr )
        detach( piface );
    }

  public:     // operators
    API& operator = ( iface* p )
    {
      autolock  aulock( locker );

      if ( piface != nullptr )
        detach( piface );
      piface = p != nullptr && attach( p ) > 0 ? p : nullptr;
        return *this;
    }
    API& operator = ( const API& a )
    {
      return operator =( a.ptr() );
    }
    API& operator = ( const api<iface>& a )
    {
      return operator =( a.ptr() );
    }

  public:     // conversions
          pvalue operator -> ()       {  return interlocked( [&]{  return pvalue( piface );  } );  }
    const pvalue operator -> () const {  return interlocked( [&]{  return pvalue( piface );  } );  }
          pvalue ptr()                {  return interlocked( [&]{  return pvalue( piface );  } );  }
    const pvalue ptr() const          {  return interlocked( [&]{  return pvalue( piface );  } );  }

    bool  operator == ( const void* p ) const {  return interlocked( [&](){  return (const void*)piface == p;  } );  }
    bool  operator != ( const void* p ) const {  return !(*this == p);  }

    operator const iface* () const {  return interlocked( [&]{  return pvalue( piface );  } );  }
    operator iface*() {  return interlocked( [&]{  return pvalue( piface );  } );  }

    operator void**() {  return ppvoid( *this );  }
    operator iface**()  {  return ppvoid( *this );  }

  };

  struct reference_counter: public std::atomic_int
  {
    reference_counter() noexcept: std::atomic_int( 0 ) {}
  };

}  // mtc namespace

# define  implement_lifetime_control                        \
  protected:  mtc::reference_counter lifetime_counter;      \
  public:     long  Attach()  noexcept override             \
    {  return ++lifetime_counter;  }                        \
  public:     long  Detach()  noexcept override             \
    {                                                       \
      long rcount;                                          \
      if ( (rcount = --lifetime_counter) == 0 )             \
        delete this;                                        \
      return rcount;                                        \
    }
# define  implement_nonvirtual_lifetime_control             \
  protected:  mtc::reference_counter lifetime_counter;      \
  public:     long  Attach()  noexcept                      \
    {  return ++lifetime_counter;  }                        \
  public:     long  Detach()  noexcept                      \
    {                                                       \
      long rcount;                                          \
      if ( (rcount = --lifetime_counter) == 0 )             \
        delete this;                                        \
      return rcount;                                        \
    }
# define  implement_lifetime_stub                           \
  public:     long  Attach()  noexcept override             \
    {  return 1;  }                                         \
  public:     long  Detach()  noexcept override             \
    {  return 1;  }

# endif  // __interfaces_h__
