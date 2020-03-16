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

  namespace impl
  {
    template <class T>  class att
      {  public: void operator ()( T* p ){  ((typename std::remove_cv<T>::type*)p)->Attach();  }  };
    template <class T>  class det
      {  public: void operator ()( T* p ){  ((typename std::remove_cv<T>::type*)p)->Detach();  }  };

  }

  template <class iface, class attach = impl::att<iface>, class detach = impl::det<iface>>  class api;
  template <class iface, class attach = impl::att<iface>, class detach = impl::det<iface>>  class API;

  /*
    non-thread-safe api pointer; is faster but may not be used for pointers can be modified while working
  */
  template <class iface, class attach, class detach>
  class api
  {
    mutable iface*  piface;

  public:       // construction/destruction
    api( iface* p = nullptr ) noexcept
      {
        if ( (piface = p) != nullptr )
          attach()( p );
      }
    api( const api& a ) noexcept
      {
        if ( (piface = a.piface) != nullptr )
          attach()( piface );
      }
    api( api&& a ) noexcept: piface( a.piface )
      {
        a.piface = nullptr;
      }
    api( const API<iface, attach, detach>& a ) noexcept
      {
        if ( (piface = a.ptr()) != nullptr )
          attach()( piface );
      }
   ~api()
      {
        if ( piface != nullptr )
          detach()( piface );
      }

  public:     // operators
    api& operator = ( iface* p )
      {
        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = p) != nullptr )
          attach()( piface );
        return *this;
      }
    api& operator = ( const api& a )
      {
        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = a.piface) != nullptr )
          attach()( piface );
        return *this;
      }
    api& operator = ( api&& a ) noexcept
      {
        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = a.piface) != nullptr )
          a.piface = nullptr;
        return *this;
      }
    api& operator = ( const API<iface, attach, detach>& a )
      {
        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = a.ptr()) != nullptr )
          attach()( piface );
        return *this;
      }

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

    operator void**() {  return &piface;  }
    operator iface**()  {  return &piface;  }

  };

  template <class i, class att, class det>
  bool  operator == ( const api<i, att, det>& i1, const api<i, att, det>& i2 ) {  return i1.ptr() == i2.ptr();  }
  template <class i, class att, class det>
  bool  operator != ( const api<i, att, det>& i1, const api<i, att, det>& i2 ) {  return !(i1 == i2);  }

  template <class i, class att, class det>
  bool  operator == ( const api<i, att, det>& ci, nullptr_t np ) {  return ci.ptr() == np;  }
  template <class i, class att, class det>
  bool  operator != ( const api<i, att, det>& ci, nullptr_t np ) {  return !(ci == np);  }

  template <class i, class att, class det>
  bool  operator == ( nullptr_t np, const api<i, att, det>& ci ) {  return ci.ptr() == np;  }
  template <class i, class att, class det>
  bool  operator != ( nullptr_t np, const api<i, att, det>& ci ) {  return !(ci == np);  }

  template <class iface, class attach, class detach>
  class API
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
    class ppvoid
    {
      friend class API;

    protected:
      ppvoid( API& a ): papi( &a )          {  papi->locker.lock();  }
      ppvoid( ppvoid&& p ): papi( p.papi )  {  p.papi = nullptr;  }
     ~ppvoid()                              {  if ( papi != nullptr ) papi->locker.unlock();  }
      ppvoid( const ppvoid& ) = delete;
      ppvoid& operator = ( const ppvoid& ) = delete;

    public:
      operator iface**  ()  {  assert( papi != nullptr );  return &papi->piface;  }
      operator  void**  ()  {  return (void**)(iface**)*this;  }

    protected:
      API*  papi;

    };

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

  public:       // construction/destruction
    API( iface* p = nullptr )
      {
        if ( (piface = p) != nullptr )
          attach()( p );
      }
    API( const API& a )
      {
        auto  avalue = a.ptr();

        if ( (piface = (iface*)(const iface*)avalue) != nullptr )
          attach()( piface );
      }
    API( const api<iface, attach, detach>& a )
      {
        if ( (piface = a.ptr()) != nullptr )
          attach()( piface );
      }
   ~API()
      {
        autolock  aulock( locker );

        if ( piface != nullptr )
          detach()( piface );
      }

  public:     // operators
    API& operator = ( iface* p )
      {
        autolock  aulock( locker );

        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = p) != nullptr )
          attach()( piface );
        return *this;
      }
    API& operator = ( const API& a )
      {
        autolock  aulock( locker );

        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = (iface*)(const iface*)a.ptr()) != nullptr )
          attach()( piface );
        return *this;
      }
    API& operator = ( const api<iface, attach, detach>& a )
      {
        autolock  aulock( locker );

        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = (iface*)(const iface*)a.ptr()) != nullptr )
          attach()( piface );
        return *this;
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

# define  implement_lifetime_control                                \
  protected:  mtc::reference_counter lifetime_counter;              \
    template <class T>                                              \
    void delete_this( T* p )                                        \
    {  p->~T();  free( p );  }                                      \
  public:     virtual long  Attach()  noexcept override             \
    {  return ++lifetime_counter;  }                                \
  public:     virtual long  Detach()  noexcept override             \
    {                                                               \
      long rcount;                                                  \
      if ( (rcount = --lifetime_counter) == 0 )                     \
        delete_this( this );                                        \
      return rcount;                                                \
    }
# define  implement_lifetime_stub                                   \
  public:     virtual long  Attach()  noexcept override             \
    {  return 1;  }                                                 \
  public:     virtual long  Detach()  noexcept override             \
    {  return 1;  }

# endif  // __interfaces_h__
