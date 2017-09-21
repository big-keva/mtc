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
# include <atomic>
# include <mutex>

namespace mtc
{

  struct  Iface
  {
    virtual long  Attach() = 0;
    virtual long  Detach() = 0;
  };

  template <class T>  class default_API_attach
    {  public: auto operator ()( T* p ){  return ((Iface*)p)->Attach();  }  };
  template <class T>  class default_API_detach
    {  public: auto operator ()( T* p ){  return ((Iface*)p)->Detach();  }  };

  template <class iface, class attach = default_API_attach<iface>, class detach = default_API_detach<iface>>
  class API
  {
    using usemutex = std::recursive_mutex;
    using autolock = std::lock_guard<usemutex>;

    mutable usemutex  locked;
    mutable iface*    piface;

    template <class _do>  auto interlocked( _do do_ )
      {  autolock  aulock( locked );  return do_();  }
    template <class _do>  auto interlocked( _do do_ ) const
      {  autolock  aulock( locked );  return do_();  }

  protected:
    class pvalue
    {
      API&  riface;

    public:
      pvalue( API& a ): riface( a ) {  riface.locked.lock();  }
      pvalue( const API& a ): riface( (API&)a ) {  riface.locked.lock();  }
     ~pvalue()  {  riface.locked.unlock();  }
      pvalue& operator = ( const pvalue& ) = delete;

    public:
      operator iface** ()               {  return &riface.piface;  }
      operator void** ()                {  return (void**)&riface.piface;  }
      operator iface* ()                {  return riface.piface;  }
      operator const iface* () const    {  return riface.piface;  }
      iface*  operator -> ()            {  return riface.piface;  }
      const iface* operator -> () const {  return riface.piface;  }

    };

  public:       // construction/destruction
    API( iface* p = nullptr )
      {
        autolock  aulock( locked );

        if ( (piface = p) != nullptr )
          attach()( p );
      }
    API( const API& a )
      {
        autolock  aulock( locked );
        auto      avalue = a.ptr();

        if ( (piface = (iface*)(const iface*)avalue) != nullptr )
          attach()( piface );
      }
   ~API()
      {
        autolock  aulock( locked );

        if ( piface != nullptr )
          detach()( piface );
      }

  public:     // operators
    API& operator = ( iface* p )
      {
        autolock  aulock( locked );

        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = p) != nullptr )
          attach()( piface );
        return *this;
      }
    API& operator = ( const API& a )
      {
        autolock  aulock( locked );
        auto      avalue = a.ptr();

        if ( piface != nullptr )
          detach()( piface );
        if ( (piface = (iface*)(const iface*)avalue) != nullptr )
          attach()( piface );
        return *this;
      }

  public:     // conversions
          pvalue operator -> ()       {  return pvalue( *this );  }
    const pvalue operator -> () const {  return pvalue( *this );  }
          pvalue ptr()                {  return pvalue( *this );  }
    const pvalue ptr() const          {  return pvalue( *this );  }

    bool  operator == ( const void* p ) const {  return interlocked( [&](){  return (const void*)piface == p;  } );  }
    bool  operator != ( const void* p ) const {  return interlocked( [&](){  return (const void*)piface != p;  } );  }

  };

  struct reference_counter: public std::atomic_int
  {
    reference_counter(): std::atomic_int( 0 ) {}
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
