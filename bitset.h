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
# if !defined( __mtc_bitset_h__ )
# define __mtc_bitset_h__
# include "array.h"
# include <limits.h>

namespace mtc
{
  template <class U, class M1, class M2>
  inline  void  bitset_and( array<U, M1>& a, const array<U, M2>& b )
    {
      int i;

      for ( i = 0; i < a.size() && i < b.size(); ++i )
        a[i] &= b[i];
      a.SetLen( i );
    }

  template <class U, class M1, class M2>
  inline  void  bitset_not( array<U, M1>& a, const array<U, M2>& b )
    {
      for ( auto i = 0; i < a.size() && i < b.size(); ++i )
        a[i] &= ~b[i];
    }

  template <class U, class M>
  inline  int   bitset_setmax( array<U, M>&  s, unsigned m )
    {
      return s.SetLen( (m + sizeof(U) * CHAR_BIT - 1) / (sizeof(U) * CHAR_BIT) );
    }

  template <class bitset>
  inline  bool  bitset_get( const bitset& s, unsigned b )
    {
      return s.size() > (int)(b / (sizeof(s.last()) * CHAR_BIT))
        && (s[b / (sizeof(s.last()) * CHAR_BIT)] & (1 << (b % (sizeof(s.last()) * CHAR_BIT)))) != 0;
    }

  template <class U>
  inline  U     bitsetbits( const unsigned l, const unsigned h )
    {
      unsigned  lpos = l % (sizeof(U) * CHAR_BIT);
      U         bits = h < sizeof(U) * CHAR_BIT - 1 ? (1 << (h + 1)) - 1 : (U)-1;
      U         mask = l >= sizeof(U) * CHAR_BIT ? 0 : (lpos > 0 ? ~((1 << lpos) - 1) : (U)-1);

      return bits & mask;
    }

  template <class U, class M>
  inline  int   bitset_set( array<U, M>& s, const unsigned l, const unsigned h )
    {
      if ( s.size() <= (int)(h / (sizeof(U) * CHAR_BIT)) && s.SetLen( h / (sizeof(U) * CHAR_BIT) + 1 ) != 0 )
        return ENOMEM;

    // set lower bits
      s[l / (sizeof(U) * CHAR_BIT)] |= bitsetbits<U>( l % (sizeof(U) * CHAR_BIT),
        (h - l + 1) < (sizeof(U) * CHAR_BIT) ? h % (sizeof(U) * CHAR_BIT) : sizeof(U) * CHAR_BIT );

    // set sequence bits
      for ( auto p = s.begin() + (l + (sizeof(U) * CHAR_BIT) - 1) / (sizeof(U) * CHAR_BIT);
                 p < s.begin() + (h / (sizeof(U) * CHAR_BIT)); )
        *p++ = (U)-1;

    // set upper bits
      s[h / (sizeof(U) * CHAR_BIT)] |= bitsetbits<U>( (h - l + 1) < (sizeof(U) * CHAR_BIT) ? l : 0,
        h % (sizeof(U) * CHAR_BIT) );

      return 0;
    }

  template <class U, class M>
  inline  void  bitset_del( array<U, M>& s, const unsigned l, const unsigned h )
    {
      if ( h >= l && s.size() > (int)(l / (sizeof(U) * CHAR_BIT)) )
      {
        U*  p;

      // del lower bits
        s[l / (sizeof(U) * CHAR_BIT)] &= ~bitsetbits<U>( l % (sizeof(U) * CHAR_BIT),
          (h - l + 1) < (sizeof(U) * CHAR_BIT) ? h % (sizeof(U) * CHAR_BIT) : sizeof(U) * CHAR_BIT );

      // set sequence bits
        for ( p = s.begin() + (l + (sizeof(U) * CHAR_BIT) - 1) / (sizeof(U) * CHAR_BIT);
              p < s.end() && p < s.begin() + (h / (sizeof(U) * CHAR_BIT)); )
          *p++ = 0;

      // set upper bits
        if ( p < s.end() )
          *p &= ~bitsetbits<U>( (h - l + 1) < (sizeof(U) * CHAR_BIT) ? l : 0, h % (sizeof(U) * CHAR_BIT) );
      }
    }

}

# endif  // __mtc_bitset_h__
