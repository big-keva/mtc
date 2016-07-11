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
# if !defined( __mtc_mitset_h__ )
# define __mtc_bitset_h__
# include "array.h"
# include <limits.h>

namespace mtc
{
  template <class U, class M>
  int   bitset_setmax( array<U, M>&  s, unsigned m )
    {
      return s.SetLen( (m + sizeof(U) * CHAR_BIT - 1) / (sizeof(U) * CHAR_BIT) );
    }

  template <class U, class M>
  bool  bitset_get( const array<U, M>&  s, unsigned b )
    {
      return s.size() > (int)(b / (sizeof(U) * CHAR_BIT)) && (s[b / (sizeof(U) * CHAR_BIT)] & (1 << (b % (sizeof(U) * CHAR_BIT)))) != 0;
    }

  template <class U, class M>
  int   bitset_set( array<U, M>& s, const unsigned l, const unsigned h )
    {
      if ( s.size() <= (int)(h / (sizeof(U) * CHAR_BIT)) && s.SetLen( h / (sizeof(U) * CHAR_BIT) + 1 ) != 0 )
        return ENOMEM;

    // set lower bits
      s[l / (sizeof(U) * CHAR_BIT)] |= ((U)-1) & ((1 << (h - l + 1)) - 1) << (l % (sizeof(U) * CHAR_BIT));

    // set sequence bits
      for ( auto p = s.begin() + (l + (sizeof(U) * CHAR_BIT) - 1) / (sizeof(U) * CHAR_BIT);
                 p < s.begin() + (h / (sizeof(U) * CHAR_BIT)); )
        *p++ = (U)-1;

    // set upper bits
      s[h / (sizeof(U) * CHAR_BIT)] |= ((U)-1) & ((1 << (h % (sizeof(U) * CHAR_BIT))) - 1);

      return 0;
    }

  template <class U, class M>
  void  bitset_del( array<U, M>& s, const unsigned l, const unsigned h )
    {
      if ( h >= l && s.size() > (int)(l / (sizeof(U) * CHAR_BIT)) )
      {
        U*  p;

      // del lower bits
        s[l / (sizeof(U) * CHAR_BIT)] &= ~(((U)-1) & ((1 << (h - l + 1)) - 1) << (l % (sizeof(U) * CHAR_BIT)));

      // set sequence bits
        for ( p = s.begin() + (l + (sizeof(U) * CHAR_BIT) - 1) / (sizeof(U) * CHAR_BIT);
              p < s.end() && p < s.begin() + (h / (sizeof(U) * CHAR_BIT)); )
          *p++ = 0;

      // set upper bits
        if ( p < s.end() )
          *p &= ~(((U)-1) & ((1 << (h % (sizeof(U) * CHAR_BIT))) - 1));
      }
    }

}

# endif  // __mtc_bitset_h__
