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
# include <type_traits>
# include <cstddef>
# include <climits>

namespace mtc
{
  namespace bitset_impl
  {
    template <class T, class M>
    void  setlen( std::vector<T, M>& a, size_t l )  {  a.resize( l );  }
  }

  template <class Vector>
  void  bitset_and( Vector& a, const Vector& b )
    {
      for ( size_t i = 0; i < a.size() && i < b.size(); ++i )
        a.at( i ) &= b.at( i );
      if ( b.size() < a.size() )
        bitset_impl::setlen( a, b.size() );
    }

  template <class Vector>
  void  bitset_not( Vector& a, const Vector& b )
    {
      for ( auto i = 0; i < a.size() && i < b.size(); ++i )
        a.at( i ) &= ~b.at( i );
    }

  template <class Vector>
  int   bitset_setmax( Vector& a, size_t m )
    {
      return bitset_impl::setlen( a, (m + sizeof(a.at( 0 )) * CHAR_BIT - 1) / (sizeof(a.at( 0 )) * CHAR_BIT) );
    }

  template <class bitset, class U>
  bool  bitset_get( const bitset& s, const std::pair<U, U>& r )
    {
      for ( auto u = r.first; u <= r.second; )
        if ( bitset_get( s, u++ ) ) return true;
      return false;
    }

  template <class Vector, class U>
  bool  bitset_get( const Vector& s, U b )
    {
      using             element_type = typename std::remove_reference<decltype(s.at( 0 ))>::type;
      constexpr size_t  element_size = sizeof(element_type) * CHAR_BIT;

      return (b / element_size) < (size_t)s.size() && (s[b / element_size] & (1 << (b % element_size))) != 0;
    }

  template <class bitset>
  bool  bitset_get( const bitset& s, const bitset& m )
    {
      int   f = bitset_first( m );
      int   l = bitset_last( m );

      return f != -1 ? bitset_get( s, std::make_pair( f, l ) ) : false;
    }

  template <class bitset>
  int   bitset_first( const bitset& s )
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
  int   bitset_last( const bitset& s )
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
  int   bitset_next( const bitset& s, int p )
    {
      int   last = bitset_last( s );

      if ( last == -1 || p >= last )
        return -1;
      do ++p;
        while ( !bitset_get( s, p ) );
      return p;
    }

  template <class U>
  U     bitsetbits( unsigned l, unsigned h )
    {
      U     bits = ((U)-1) & ~((1 << l) - 1);
      U     mask = ++h < sizeof(U) * CHAR_BIT ? (1 << h) - 1 : (U)-1;

      return bits & mask;
    }

  template <class Vector, class U>
  void  bitset_set( Vector& s, const std::pair<U, U>& r )
    {
      using             element_type = typename std::remove_reference<decltype(s.at( 0 ))>::type;
      using             size_type = decltype(s.size());
      constexpr size_t  element_size = sizeof(element_type) * CHAR_BIT;
      auto              l = r.first;
      auto              h = r.second;

      if ( l > h )
        inplace_swap( l, h );

      if ( s.size() <= size_type(h / element_size) )
        bitset_impl::setlen( s, h / element_size + 1 );

    // set lower bits
      s.at( l / element_size ) |= bitsetbits<element_type>( (unsigned)(l % element_size),
        (unsigned)min( h - (l / element_size) * element_size, element_size - 1 ) );

    // set sequence bits
      for ( auto p = s.begin() + (l / element_size) + 1; p < s.begin() + (h / element_size); )
        *p++ = (element_type)-1;

    // set upper bits
      if ( h / element_size > l / element_size)
        s[h / element_size] |= bitsetbits<element_type>( 0, h % element_size );
    }

  template <class Vector>
  void  bitset_set( Vector& s, int u )  {  return bitset_set( s, std::make_pair( u, u ) );  }

  template <class Vector, class U>
  int   bitset_del( Vector& s, const std::pair<U, U>& r )
    {
      using             element_type = typename std::remove_reference<decltype(s.at( 0 ))>::type;
      using             size_type = decltype(s.size());
      constexpr size_t  element_size = sizeof(element_type) * CHAR_BIT;
      auto              l = r.l;
      auto              h = r.h;

      if ( h >= l && s.size() > size_type(l / element_size) )
      {
        decltype(s.begin()) p;

      // del lower bits
        s[l / element_size] &= ~bitsetbits<element_type>( (unsigned)(l % element_size),
          (unsigned)min( h - (l / element_size) * element_size, element_size - 1 ) );

      // set sequence bits
        for ( p = s.begin() + (l / element_size) + 1; p < s.end() && p < s.begin() + (h / element_size); )
          *p++ = (element_type)0;

      // set upper bits
        if ( p < s.end() && h / element_size > l / element_size )
          *p &= ~bitsetbits<element_type>( 0, h % element_size );
      }
      return 0;
    }

  template <class bitset>
  int   bitset_del( bitset& s, int u )  {  return bitset_del( s, make_range( u ) );  }

# if defined(GNUC) || defined(clang)
#   define popcount32( n )  __builtin_popcount( (n) )
#   define popcount64( n )  __builtin_popcountll(n)
# elif defined(_MSC_VER)
#   define popcount32( n )  static_cast<int>( __popcnt(n) )
#   define popcount64( n )  static_cast<int>(__popcnt64(n));
# else
  inline  int   popcount32( uint32_t n )
  {
    n -= (n >> 1) & 0x55555555;
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    return static_cast<int>(((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
  }

  inline  int   popcount64( uint64_t n )
  {
    n -= (n >> 1) & 0x5555555555555555ULL;
    n = (n & 0x3333333333333333ULL) + ((n >> 2) & 0x3333333333333333ULL);
    return static_cast<int>(((n + (n >> 4)) & 0x0F0F0F0F0F0F0F0FULL) * 0x0101010101010101ULL >> 56);
  }
# endif

  template <class T>
  int   popcount( T t ) noexcept
  {
    static_assert( std::is_integral<T>::value, "Integer type required" );

    using U = std::make_unsigned_t<T>;
    const U uval = static_cast<U>( t );

    if constexpr ( sizeof(T) <= sizeof(uint32_t) )
      return popcount32( uval );
    else
      return popcount64( static_cast<uint64_t>( uval ) );
  }

  template <class T, class A>
  int   popcount( const std::vector<T, A>& v ) noexcept
  {
    int   pcount = 0;

    for ( auto next: v )
      pcount += popcount( next );

    return pcount;
  }

  template <class T, size_t N>
  int   popcount( T (&arr)[N] ) noexcept
  {
    int   pcount = 0;

    for ( auto next: arr )
      pcount += popcount( next );

    return pcount;
  }

}

# endif  // __mtc_bitset_h__
