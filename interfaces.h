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

namespace mtc
{

  struct  Iface
  {
    virtual long  Attach() = 0;
    virtual long  Detach() = 0;
  };

  class _lifetime_reference_counter_
  {
    long  rcount;

  public: _lifetime_reference_counter_(): rcount( 0 )   {}
    long  AddRef() {  return ++rcount;  }
    long  DecRef() {  return --rcount;  }
  };

  template <class iface>
  class API
  {
    iface*    piface;

  public:       // construction/destruction
    API( iface* p = nullptr )
      {
        if ( (piface = p) != nullptr )
          piface->Attach();
      }
    API( const API& a )
      {
        if ( (piface = a.piface) != nullptr )
          piface->Attach();
      }
   ~API()
      {
        if ( piface != nullptr )
          piface->Detach();
      }

  public:     // operators
    API& operator = ( iface* p )
      {
        if ( piface != nullptr )
          piface->Detach();
        if ( (piface = p) != nullptr )
          piface->Attach();
        return *this;
      }
    API& operator = ( const API& a )
      {
        if ( piface != nullptr )
          piface->Detach();
        if ( (piface = a.piface) != nullptr )
          piface->Attach();
        return *this;
      }
    operator iface** ()             {  return &piface; }
    operator iface* ()              {  return piface;  }
    operator const iface* () const  {  return piface;  }
    iface*  operator -> ()
      {
        assert( piface != nullptr );
        return piface;
      }
    const iface* operator -> () const
      {
        assert( piface != nullptr );
        return piface;
      }
    bool  operator == ( const void* p )
      {  return (const void*)piface == p;  }
    bool  operator != ( const void* p )
      {  return (const void*)piface != p;  }
  };

}  // mtc namespace

# define  implement_lifetime_control                                \
  protected:  mtc::_lifetime_reference_counter_  lifetime_counter;  \
  public:     virtual long  Attach()  noexcept                      \
    {  return lifetime_counter.AddRef();  }                         \
  public:     virtual long  Detach()  noexcept                      \
    {                                                               \
      long rcount;                                                  \
      if ( (rcount = lifetime_counter.DecRef()) == 0 )              \
        delete this;                                                \
      return rcount;                                                \
    }
# define  implement_lifetime_stub                                   \
  public:     virtual long  Attach()  noexcept override             \
    {  return 1;  }                                                 \
  public:     virtual long  Detach()  noexcept override             \
    {  return 1;  }


# endif  // __interfaces_h__
