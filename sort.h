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
# if !defined( __mtc_sort_h__ )
# define __mtc_sort_h__
# include "platform.h"

namespace mtc
{
  template<class _iterator, class T, class compare>
  void  __inplace_sort__( _iterator first, _iterator last, const T& pv, compare cmp )
  {
      const T*  m = first + (last - first)/2;
      _iterator f = first;
      _iterator l = last;

      do
      {
        while ( f < last  && cmp( *f, *m ) < 0 ) ++f;
        while ( l > first && cmp( *m, *l ) < 0 ) --l;

        if ( f <= l )
        {
          m = m == f ? l :
              m == l ? f : m;
          inplace_swap( *f++, *l-- );
        }
      } while ( f <= l );

      if ( l > first )
        __inplace_sort__( first, l, pv, cmp );
      if ( last > f )
        __inplace_sort__( f, last, pv, cmp );
  }

  template<class _iterator, class T>
  void  __inplace_sort__( _iterator first, _iterator last, const T& pv )
  {
      const T*  m = first + (last - first)/2;
      _iterator f = first;
      _iterator l = last;

      do
      {
        while ( f < last  && *f < *m ) ++f;
        while ( l > first && *m < *l ) --l;

        if ( f <= l )
        {
          m = m == f ? l :
              m == l ? f : m;
          inplace_swap( *f++, *l-- );
        }
      } while ( f <= l );

      if ( l > first )
        __inplace_sort__( first, l, pv );
      if ( last > f )
        __inplace_sort__( f, last, pv );
  }

  template<typename _iterator>
  void  inplace_sort(_iterator first, _iterator last )
  {
    if ( first < --last )
      __inplace_sort__( first, last, *first );    
  }

  template<typename _iterator, typename compare>
  void  inplace_sort(_iterator first, _iterator last, compare cmp )
  {
    if ( first < --last )
      __inplace_sort__( first, last, *first, cmp );    
  }

}

# endif  // __mtc_sort_h__
