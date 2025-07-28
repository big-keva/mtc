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
# include <cstdint>
# include <climits>
# include <algorithm>

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

 /*
  * bitset_get() family
  */
  template <class U>
  bool  bitset_get( U u, const std::pair<unsigned, unsigned>& r )
  {
    static_assert( std::is_integral<U>::value, "integer type required" );

    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  l = std::min( std::min( r.first, r.second ), unsigned(element_size) );
    auto  h = std::min( std::max( r.first, r.second ), unsigned(element_size - 1) );

    if ( l >= element_size )
      return false;

    return (u & (((U(1) << h) | ((U(1) << h) - 1)) & ~((U(1) << l) - 1))) != 0;
  }

  template <class U, class A>
  bool  bitset_get( const std::vector<U, A>& v, const std::pair<unsigned, unsigned>& r )
  {
    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  lo = std::min( r.first, r.second );
    auto  hi = std::min( std::max( r.first, r.second ), unsigned(v.size() * element_size - 1) );
    auto  lr = lo % element_size;
    auto  hr = hi % element_size;

    if ( lo / element_size >= v.size() )
      return false;

    if ( hi < ((lo + element_size - 1) & ~(element_size - 1)) )
      return bitset_get( v[lo / element_size], { lr, hr < lr ? element_size - 1 : hr } );

    for ( auto i = (lo / element_size) + 1; i < hi / element_size; ++i, lo = 0 )
      if ( v[i] != 0 )
        return true;

    return bitset_get( v[hi / element_size], { 0, hr } );
  }

  template <class U, size_t N>
  bool  bitset_get( const U (&v)[N], const std::pair<unsigned, unsigned>& r )
  {
    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  lo = std::min( r.first, r.second );
    auto  hi = std::min( std::max( r.first, r.second ), unsigned(std::size( v ) * element_size - 1) );
    auto  lr = lo % element_size;
    auto  hr = hi % element_size;

    if ( lo / element_size >= std::size( v ) )
      return false;

    if ( hi < ((lo + element_size - 1) & ~(element_size - 1)) )
      return bitset_get( v[lo / element_size], { lr, hr < lr ? element_size - 1 : hr } );

    for ( auto i = (lo / element_size) + 1; i < hi / element_size; ++i, lo = 0 )
      if ( v[i] != 0 )
        return true;

    return bitset_get( v[hi / element_size], { 0, hr } );
  }

  template <class B>
  bool  bitset_get( const B& b, unsigned u )
  {
    return bitset_get( b, { u, u } );
  }

 /*
  * bitset_set() family
  */
  template <class U>
  void  bitset_set( U& u, const std::pair<unsigned, unsigned>& r )
  {
    static_assert( std::is_integral<U>::value, "integer type required" );

    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  l = std::min( std::min( r.first, r.second ), unsigned(element_size) );
    auto  h = std::min( std::max( r.first, r.second ), unsigned(element_size) );

    if ( h >= element_size )
      throw std::invalid_argument( "bitset_set output overflow" );

    u |= ((U(1) << h) | ((U(1) << h) - 1)) & ~((U(1) << l) - 1);
  }

  template <class U, class A>
  void  bitset_set( std::vector<U, A>& v, const std::pair<unsigned, unsigned>& r )
  {
    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  l = std::min( r.first, r.second );
    auto  h = std::max( r.first, r.second );

    if ( v.size() <= size_t(h / element_size) )
      v.resize( 1 + (h / element_size) );

  // if only one U word, set bits and return
    if ( h < ((l + element_size - 1) & ~(element_size - 1)) )
      return bitset_set( v[l / element_size], { l % element_size, h % element_size } );

  // set all the lower bits
    bitset_set( v[l / element_size], { l % element_size, element_size - 1 } );

  // set sequence bits
    for ( auto p = v.begin() + (l / element_size) + 1; p < v.begin() + (h / element_size); )
      *p++ = U(-1);

  // set upper bits
    bitset_set( v.at( h / element_size ), { 0, h % element_size } );
  }

  template <class U, size_t N>
  void  bitset_set( U (&v)[N], const std::pair<unsigned, unsigned>& r )
  {
    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  l = std::min( r.first, r.second );
    auto  h = std::max( r.first, r.second );

    if ( std::size(v) <= size_t(h / element_size) )
      throw std::invalid_argument( "bitset_set output overflow" );

  // if only one U word, set bits and return
    if ( h < ((l + element_size - 1) & ~(element_size - 1)) )
      return bitset_set( v[l / element_size], { l % element_size, h % element_size } );

  // set all the lower bits
    bitset_set( v[l / element_size], { l % element_size, element_size - 1 } );

  // set sequence bits
    for ( auto p = std::begin( v ) + (l / element_size) + 1; p < std::begin( v ) + (h / element_size); )
      *p++ = U(-1);

  // set upper bits
    bitset_set( v[h / element_size], { 0, h % element_size } );
  }

  template <class T>
  void  bitset_set( T& v, unsigned u )
  {
    return bitset_set( v, { u, u } );
  }

 /*
  * bitset_del() family
  */
  template <class U>
  void  bitset_del( U& u, const std::pair<unsigned, unsigned>& r )
  {
    static_assert( std::is_integral<U>::value, "integer type required" );

    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  l = std::min( std::min( r.first, r.second ), unsigned(element_size) );
    auto  h = std::min( std::max( r.first, r.second ), unsigned(element_size) );

    if ( l >= element_size )
      return;

    u &= ~(((U(1) << h) | ((U(1) << h) - 1)) & ~((U(1) << l) - 1));
  }

  template <class U, class A>
  void  bitset_del( std::vector<U, A>& v, const std::pair<unsigned, unsigned>& r )
  {
    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  lo = std::min( r.first, r.second );
    auto  hi = std::min( std::max( r.first, r.second ), unsigned(std::size( v ) * element_size - 1) );

    if ( lo / element_size >= v.size() )
      return;

  // if only one U word, del bits and return
    if ( hi < ((lo + element_size - 1) & ~(element_size - 1)) )
      return bitset_del( v[lo / element_size], { lo % element_size, hi % element_size } );

  // del all the lower bits
    bitset_del( v[lo / element_size], { lo % element_size, element_size - 1 } );

  // del sequence bits
    for ( auto p = v.begin() + (lo / element_size) + 1; p < v.begin() + (hi / element_size); )
      *p++ = U(0);

  // del upper bits
    bitset_del( v.at( hi / element_size ), { 0, hi % element_size } );
  }

  template <class U, size_t N>
  void  bitset_del( U (&v)[N], const std::pair<unsigned, unsigned>& r )
  {
    enum: unsigned { element_size = sizeof(U) * CHAR_BIT };

    auto  lo = std::min( r.first, r.second );
    auto  hi = std::min( std::max( r.first, r.second ), unsigned(std::size( v ) * element_size - 1) );

    if ( lo / element_size >= std::size( v ) )
      return;

  // if only one U word, del bits and return
    if ( hi < ((lo + element_size - 1) & ~(element_size - 1)) )
      return bitset_del( v[lo / element_size], { lo % element_size, hi % element_size } );

  // del all the lower bits
    bitset_del( v[lo / element_size], { lo % element_size, element_size - 1 } );

  // del sequence bits
    for ( auto p = std::begin( v ) + (lo / element_size) + 1; p < std::begin( v ) + (hi / element_size); )
      *p++ = U(0);

  // del upper bits
    bitset_del( v[hi / element_size], { 0, hi % element_size } );
  }

  template <class bitset>
  void  bitset_del( bitset& s, unsigned u )
  {
    return bitset_del( s, { u, u } );
  }

# if defined(GNUC) || defined(clang)
#   define bitset_count32( n )  __builtin_popcount( (n) )
#   define bitset_count64( n )  __builtin_popcountll(n)
# elif defined(_MSC_VER)
#   define bitset_count32( n )  static_cast<int>( __popcnt(n) )
#   define bitset_count64( n )  static_cast<int>(__popcnt64(n));
# else
  inline  int   bitset_count32( uint32_t n )
  {
    n -= (n >> 1) & 0x55555555;
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    return static_cast<int>(((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
  }

  inline  int   bitset_count64( uint64_t n )
  {
    n -= (n >> 1) & 0x5555555555555555ULL;
    n = (n & 0x3333333333333333ULL) + ((n >> 2) & 0x3333333333333333ULL);
    return static_cast<int>(((n + (n >> 4)) & 0x0F0F0F0F0F0F0F0FULL) * 0x0101010101010101ULL >> 56);
  }
# endif

  template <class T>
  int   bitset_count( T t ) noexcept
  {
    static_assert( std::is_integral<T>::value, "Integer type required" );

    if constexpr ( sizeof(T) <= sizeof(uint32_t) )
      return bitset_count32( static_cast<std::make_unsigned_t<T>>( t ) );
    else
      return bitset_count64( static_cast<uint64_t>( static_cast<std::make_unsigned_t<T>>( t ) ) );
  }

  template <class T, class A>
  int   bitset_count( const std::vector<T, A>& v ) noexcept
  {
    int   pcount = 0;

    for ( auto next: v )
      pcount += bitset_count( next );

    return pcount;
  }

  template <class T, size_t N>
  int   bitset_count( T (&arr)[N] ) noexcept
  {
    int   pcount = 0;

    for ( auto next: arr )
      pcount += bitset_count( next );

    return pcount;
  }

  // bitset_empty family

  template <class T>
  bool  bitset_empty( T n ) noexcept
  {
    return n == 0;
  }

  template <class T, class A>
  bool  bitset_empty( const std::vector<T, A>& v ) noexcept
  {
    for ( auto& next: v )
      if ( next != 0 )
        return false;
    return true;
  }

  template <class T, size_t N>
  bool  bitset_empty( T (&arr)[N] ) noexcept
  {
    for ( auto& next: arr )
      if ( next != 0 )
        return false;
    return true;
  }

  // bitset_first family

  template <class U>
  int   bitset_first( U n ) noexcept
  {
    static_assert( std::is_integral<U>::value, "Integer type required" );

    return n & (1 + ~n);
  }

  template <class U, class A>
  int   bitset_first( const std::vector<U, A>& v )
  {
    for ( auto p = std::begin( v ); p != std::end( v ); ++p )
      if ( *p != 0 )
        return CHAR_BIT * (p - std::begin( v )) + bitset_first( *p );
    return -1;
  }

  template <class U, size_t N>
  int   bitset_first( U (&arr)[N] )
  {
    for ( auto p = std::begin( arr ); p != std::end( arr ); ++p )
      if ( *p != 0 )
        return CHAR_BIT * (p - std::begin( arr )) + bitset_first( *p );
    return -1;
  }

  // bitset_last

  template <class U>
  int   bitset_last( U u )
  {
    int  nshift = sizeof(u) * CHAR_BIT - 1;

    while ( nshift >= 0 && (u & (1 << nshift)) == 0 )
      --nshift;

    return nshift >= 0 ? nshift : -1;
  }

  template <class U, class A>
  int   bitset_last( const std::vector<U, A>& s )
  {
    for ( auto p = s.end(); p > s.begin(); --p )
      if ( p[-1] != 0 )
        return CHAR_BIT * (p - s.begin()) + bitset_last( *p );
    return -1;
  }

  template <class U, size_t N>
  int   bitset_last( const U (&arr)[N] )
  {
    for ( auto p = std::end( arr ); p > std::begin( arr ); --p )
      if ( p[-1] != 0 )
        return CHAR_BIT * (p - std::begin( arr )) + bitset_last( *p );
    return -1;
  }

  template <class T>
  int   bitset_next( const T& s, int p )
  {
    int   last = bitset_last( s );

    if ( last == -1 || p >= last )
      return -1;

    do ++p;
      while ( !bitset_get( s, p ) );

    return p;
  }

}

# endif  // __mtc_bitset_h__
