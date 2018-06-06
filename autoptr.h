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
# if !defined( __mtc_auto_ptr_h__ )
# define  __mtc_auto_ptr_h__
# include "platform.h"
# include <stdlib.h>
# include <assert.h>

namespace mtc
{

  template <class T, class M = def_alloc>
  class _auto_
  {
    T*  p;

  public:
    _auto_( T* v = nullptr ) noexcept: p( v )
      {
      }
    _auto_( const _auto_& a ) noexcept: p( a.p )
      {
        ((_auto_<T, M>&)a).p = nullptr;
      }
   ~_auto_()
      {
        M m;
        deallocate_with( m, p );
      }
    _auto_& operator = ( const _auto_& a )
      {
        M m;

        deallocate_with( m, p );
          p = a.p;
        ((_auto_<T, M>&)a).p = nullptr;
          return *this;
      }
    _auto_& operator = ( T* a )
      {
        M m;

        deallocate_with( m, p );
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

    bool  operator == ( const void* v ) const {  return this == v || p == v;  }
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
