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
# include <vector>
# include <limits>
# include <type_traits>

namespace mtc
{
  template <class Vector>
  inline  void  bitset_and( Vector& a, const Vector& b )
    {
      for ( auto i = 0; i < a.size() && i < b.size(); ++i )
        a.at( i ) &= b.at( i );
      if ( b.size() < a.size() )
        a.resize( b.size() );
    }

  template <class Vector>
  inline  void  bitset_not( Vector& a, const Vector& b )
    {
      for ( auto i = 0; i < a.size() && i < b.size(); ++i )
        a.at( i ) &= ~b.at( i );
    }

  template <class Vector>
  inline  void  bitset_setmax( Vector& a, size_t m )
    {
      enum: size_t { element_size = sizeof(a[0]), element_bits = element_size * CHAR_BIT };

      a.resize( m + element_bits - 1 / element_bits );
    }

  template <class bitset, class U>
  inline  bool  bitset_get( const bitset& s, const range<U>& r )
    {
      for ( auto u = r.l; u <= r.h; )
        if ( bitset_get( s, u++ ) ) return true;
      return false;
    }

  template <class Vector, class U>
  inline  bool  bitset_get( const Vector& s, U b )
    {
      using             element_type = typename std::remove_reference<decltype(s.at( 0 ))>::type;
      constexpr size_t  element_size = sizeof(element_type) * CHAR_BIT;

      return (b / element_size) < (size_t)s.size() && (s[b / element_size] & (1 << (b % element_size))) != 0;
    }

  template <class bitset>
  inline  bool  bitset_get( const bitset& s, const bitset& m )
    {
      int   f = bitset_first( m );
      int   l = bitset_last( m );

      return f != -1 ? bitset_get( s, make_range( f, l ) ) : false;
    }

  template <class bitset>
  inline  int   bitset_first( const bitset& s )
    {
      for ( auto p = s.begin(); p < s.end(); ++p )
        if ( *p != 0 )
        {
          auto      uvalue = *p;
          unsigned  nshift = 0;

          while ( (uvalue & 0x01) == 0 )
            {  uvalue >>= 1;  ++nshift;  }

          return (unsigned)(nshift + sizeof(s.last()) * CHAR_BIT * (p - s.begin()));
        }
      return -1;
    }

  template <class bitset>
  inline  int   bitset_last( const bitset& s )
    {
      for ( auto p = s.end(); p > s.begin(); --p )
        if ( p[-1] != 0 )
        {
          auto      uvalue = p[-1];
          unsigned  nshift = sizeof(uvalue) * CHAR_BIT - 1;

          while ( (uvalue & (1 << nshift)) == 0 )
            --nshift;

          return (unsigned)(nshift + sizeof(s.last()) * CHAR_BIT * (p - s.begin() - 1));
        }
      return -1;
    }

  template <class bitset>
  inline  int   bitset_next( const bitset& s, int p )
    {
      int   last = bitset_last( s );

      if ( last == -1 || p >= last )
        return -1;
      do ++p;
        while ( !bitset_get( s, p ) );
      return p;
    }

  template <class U>
  inline  U     bitsetbits( unsigned l, unsigned h )
    {
      assert( l < sizeof(U) * CHAR_BIT );
      assert( h < sizeof(U) * CHAR_BIT );

      U     bits = ((U)-1) & ~((1 << l) - 1);
      U     mask = ++h < sizeof(U) * CHAR_BIT ? (1 << h) - 1 : (U)-1;

      return bits & mask;
    }

  template <class Vector, class U>
  inline  void  bitset_set( Vector& s, const range<U>& r )
    {
      using             size_type = decltype(s.size());
      using             element_type = typename std::remove_reference<decltype(s.at( 0 ))>::type;
      constexpr size_t  element_bits = sizeof(element_type) * CHAR_BIT;
      auto              l = r.l;
      auto              h = r.h;

      if ( l > h )
        inplace_swap( l, h );

      if ( s.size() <= size_type(h / element_bits) )
        s.resize( h / element_bits + 1 );

    // set lower bits
      s.at( l / element_bits ) |= bitsetbits<element_type>( (unsigned)(l % element_bits),
        (unsigned)min( h - (l / element_bits) * element_bits, element_bits - 1 ) );

    // set sequence bits
      for ( auto p = s.begin() + (l / element_bits) + 1; p < s.begin() + (h / element_bits); )
        *p++ = (element_type)-1;

    // set upper bits
      if ( h / element_bits > l / element_bits)
        s[h / element_bits] |= bitsetbits<element_type>( 0, h % element_bits );
    }

  template <class Vector>
  inline  void  bitset_set( Vector& s, int u )  {  return bitset_set( s, make_range( u ) );  }

  template <class Vector, class U>
  inline  void  bitset_del( Vector& s, const range<U>& r )
    {
      using             element_type = typename std::remove_reference<decltype(s.at( 0 ))>::type;
      constexpr size_t  element_bits = sizeof(element_type) * CHAR_BIT;
      using             size_type = decltype(s.size());
      auto              l = r.l;
      auto              h = r.h;

      if ( h >= l && s.size() > size_type(l / element_bits) )
      {
        decltype(s.begin()) p;

      // del lower bits
        s[l / element_bits] &= ~bitsetbits<element_type>( (unsigned)(l % element_bits),
          (unsigned)min( h - (l / element_bits) * element_bits, element_bits - 1 ) );

      // set sequence bits
        for ( p = s.begin() + (l / element_bits) + 1; p < s.end() && p < s.begin() + (h / element_bits); )
          *p++ = (element_type)0;

      // set upper bits
        if ( p < s.end() && h / element_bits > l / element_bits )
          *p &= ~bitsetbits<element_type>( 0, h % element_bits );
      }
    }

  template <class bitset>
  inline  void  bitset_del( bitset& s, int u )  {  return bitset_del( s, make_range( u ) );  }

}

# endif  // __mtc_bitset_h__
