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
# if !defined( __mtc_arbitrarymap_h__ )
# define  __mtc_arbitrarymap_h__
# include "wcsstr.h"
# include "serialize.h"
# include <assert.h>
# include <stdlib.h>
# include <errno.h>

namespace mtc
{

  template <class V, class A = std::allocator<char>>
  class   arbitrarymap
  {
    template <class Base, class T>
    using rebind = typename std::allocator_traits<Base>::template rebind_alloc<T>;

    A   alloc;

    struct  keyrec
    {
      V         val;
      unsigned  pos;
      keyrec*   lpn;
      unsigned  len;
      char      key[1];

    public:   // construction
      keyrec( const V& v, unsigned p, keyrec* n ): val( v ), pos( p ), lpn( n ) {}
      keyrec(             unsigned p, keyrec* n ):           pos( p ), lpn( n ) {}

    };

  protected:  // allocation
    template <class... constructor_args>
    keyrec* Create( const void* k, unsigned l, constructor_args... a )
    {
      auto    nalloc = (sizeof(keyrec) + l + sizeof(keyrec) - 1) / sizeof(keyrec);
      keyrec* palloc;

      if ( (palloc = rebind<A, keyrec>( alloc ).allocate( nalloc )) != nullptr )
      {
        new( palloc ) keyrec( a... );
          memcpy( palloc->key, k, palloc->len = l );
      }
      return palloc;
    }
    template <class R, class S> static
    auto  Search( const void*, size_t, S& ) -> R*;

  public:     // construction
    arbitrarymap( const A& a = {} ):
      arbitrarymap( 69959, a ) {}
    arbitrarymap( arbitrarymap&& m ): alloc( m.alloc ),
      pitems( m.pitems ),
      maplen( m.maplen ),
      ncount( m.ncount )
    {
      m.pitems = nullptr;
      m.ncount = 0;
    }
    arbitrarymap( unsigned tablen, const A& a = {} ):
      alloc( a ), maplen( tablen )  {}
   ~arbitrarymap();

  protected:
    arbitrarymap( const arbitrarymap& ) = delete;
    arbitrarymap& operator = ( const arbitrarymap& ) = delete;
    
  public:     // methods
    int           Append( const arbitrarymap& );
    int           Delete( const void* k, size_t );
    int           Delete( const widechar* s ) {  return Delete( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    int           Delete( const char* s )     {  return Delete( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    void          DelAll();
    unsigned      GetLen() const  {  return ncount;  }
    unsigned      MapLen() const  {  return maplen;  }
          V*      Insert( const void* k, size_t l, const V& v = V() );
          V*      Insert( const widechar* s, const V& v = V() ) {  return Insert( s, sizeof(*s) * (w_strlen( s ) + 1), v );  }
          V*      Insert( const char*     s, const V& v = V() ) {  return Insert( s, sizeof(*s) * (w_strlen( s ) + 1), v );  }
    const V*      Search( const void* k, size_t l ) const {  return Search<const V>( k, l, *this);  }
    const V*      Search( const widechar* s ) const {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    const V*      Search( const char*     s ) const {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
          V*      Search( const void* k, size_t l )       {  return Search<V>( k, l, *this ); }
          V*      Search( const widechar* s ) {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
          V*      Search( const char*     s ) {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }

  public:     // serialization
    auto  GetBufLen() const -> size_t;;
    template <class O>
    auto  Serialize( O* ) const -> O*;
    template <class S>
    auto  FetchFrom( S* ) -> S*;

  public:     // stl compat
    int     size() const  {  return GetLen();  }

  public:     // template wrappers
    template <class K>  int   Delete( const K& k )  {  return Delete( k.begin(), k.size() );  }
    template <class K>  V*    Insert( const K& k, const V& v = V() )  {  return Insert( k.begin(), k.size(), v );  }

  // enumerator support methods
    const void*         Enum( const void* ) const;
          void*         Enum( void* );
    static  const void* GetKey( const void* );
    static  unsigned    KeyLen( const void* );
    static  const V&    GetVal( const void* );
    static  V&          GetVal(       void* );

  protected:  // helpers
    int     NewMap()
      {
        if ( pitems != nullptr || maplen == 0 )
          return EINVAL;
        if ( (pitems = rebind<A, keyrec*>( alloc ).allocate( maplen )) == nullptr )
          return ENOMEM;
        for ( auto p = pitems; p < pitems + maplen; )
          *p++ = nullptr;
        return 0;
      }
    static unsigned gethash( const unsigned char* p, size_t c )
      {
        unsigned  int nHash = 0;
        while ( c-- > 0 )
          nHash = (nHash << 5) + nHash + *p++;
        return nHash;
      }
    static bool     isequal(
      const void* p1, size_t l1,
      const void* p2, size_t l2 )  {  return l1 == l2 && memcmp( p1, p2, l1 ) == 0;  }

  private:
    keyrec**  pitems = nullptr;
    unsigned  maplen;
    unsigned  ncount = 0;

  };

  // Map inline implementation

  template <class V, class A>
  arbitrarymap<V, A>::~arbitrarymap()
  {
    if ( pitems != nullptr )
    {
      DelAll();
      rebind<A, keyrec*>( alloc ).deallocate( pitems, 0 );
    }
  }

  template <class V, class A>
  int   arbitrarymap<V, A>::Append( const arbitrarymap<V, A>& s )
  {
    for ( const void* p = nullptr; (p = s.Enum( p )) != nullptr; )
      if ( Insert( GetKey( p ), KeyLen( p ), GetVal( p ) ) == nullptr )
        return ENOMEM;

    return 0;
  }

  template <class V, class A>
  template <class R, class S>
  auto  arbitrarymap<V, A>::Search( const void* k, size_t l, S& s ) -> R*
  {
    if ( s.pitems != nullptr )
    {
      unsigned  pos = gethash( (const unsigned char*)k, l ) % s.maplen;
      auto      ptr = s.pitems[pos];

      while ( ptr != nullptr && !isequal( ptr->key, ptr->len, k, l ) )
        ptr = ptr->lpn;

      return ptr != nullptr ? &ptr->val : nullptr;
    }
    return nullptr;
  }

  template <class V, class A>
  int   arbitrarymap<V, A>::Delete( const void* k, size_t l )
  {
    if ( pitems != nullptr && ncount != 0 )
    {
      unsigned  pos = gethash( (const byte_t*)k, l ) % maplen;
      keyrec**  ptr = &pitems[pos];

      while ( *ptr != nullptr && !isequal( (*ptr)->key, (*ptr)->len, k, l ) )
        ptr = &(*ptr)->lpn;

      if ( *ptr != nullptr )
      {
        keyrec* del = *ptr;

        *ptr = del->lpn;
          del->~keyrec();
          rebind<A, keyrec>( alloc ).deallocate( del, 0 );
        --ncount;
      }
      return 0;
    }
    return EINVAL;
  }

  template <class V, class A>
  void  arbitrarymap<V, A>::DelAll()
  {
    keyrec* del;

    if ( pitems != nullptr )
      for ( auto p = pitems; p < pitems + maplen; ++p )
        while ( (del = *p) != nullptr )
        {
          *p = del->lpn;
            del->~keyrec();
          rebind<A, keyrec>( alloc ).deallocate( del, 0 );
        }
    ncount = 0;
  }

  template <class V, class A>
  V*    arbitrarymap<V, A>::Insert( const void* k, size_t l, const V& v )
  {
    unsigned  pos = gethash( (const byte_t*)k, l ) % maplen;
    keyrec*   ptr;

    if ( pitems == nullptr && NewMap() != 0 )
      return nullptr;

    if ( (ptr = Create( k, l, v, pos, pitems[pos] )) != nullptr ) pitems[pos] = ptr;
      else return nullptr;

    return ++ncount, &ptr->val;
  }

  template <class V, class A>
  size_t  arbitrarymap<V, A>::GetBufLen() const
  {
    auto  res = ::GetBufLen( ncount );

    for ( const void* p = nullptr; (p = Enum( p )) != nullptr; )
    {
      auto  len = KeyLen( p );
      auto& val = GetVal( p );

      res += ::GetBufLen( len ) + len + ::GetBufLen( val );
    }
    return res;
  }

  template <class V, class A>
  template <class O>
  O*  arbitrarymap<V, A>::Serialize( O* o ) const
  {
    o = ::Serialize( o, ncount );

    for ( const void* p = nullptr; o != nullptr && (p = Enum( p )) != nullptr; )
    {
      auto  key = GetKey( p );
      auto  len = KeyLen( p );
      auto& val = GetVal( p );

      o = ::Serialize( ::Serialize( ::Serialize( o, len ), key, len ), val );
    }
    return o;
  }

  template <class V, class A>
  const void* arbitrarymap<V, A>::Enum( const void* pvn ) const
  {
    auto  ppktop = pitems;
    auto  ppkend = pitems + maplen;

  // Check pitems initialized
    if ( pitems == nullptr )
      return nullptr;

  // For the first call, make valid object pointer
    if ( pvn != nullptr )
    {
      const keyrec* lpnext;

      if ( (lpnext = ((const keyrec*)pvn)->lpn) != nullptr )
        return lpnext;

      ppktop = pitems + ((const keyrec*)pvn)->pos + 1;
    }
      else
    ppktop = pitems;

    for ( ppkend = pitems + maplen; ppktop < ppkend && *ppktop == nullptr; ++ppktop )
      (void)NULL;

    return ppktop < ppkend ? *ppktop : nullptr;
  }

  template <class V, class A>
  void* arbitrarymap<V, A>::Enum( void* pvn )
  {
    keyrec**  ppktop;
    keyrec**  ppkend;

  // Check pitems initialized
    if ( pitems == nullptr )
      return nullptr;

  // For the first call, make valid object pointer
    if ( pvn != nullptr )
    {
      keyrec* lpnext;

      if ( (lpnext = ((keyrec*)pvn)->lpn) != nullptr )
        return lpnext;

      ppktop = pitems + ((const keyrec*)pvn)->pos + 1;
    }
      else
    ppktop = pitems;

    for ( ppkend = pitems + maplen; ppktop < ppkend && *ppktop == nullptr; ++ppktop )
      (void)NULL;

    return ppktop < ppkend ? *ppktop : nullptr;
  }

  template <class V, class A>
  const void* arbitrarymap<V, A>::GetKey( const void*  pvn )
  {
    return pvn != nullptr ? ((keyrec*)pvn)->key : nullptr;
  }

  template <class V, class A>
  unsigned    arbitrarymap<V, A>::KeyLen( const void*  pvn )
  {
    return pvn != nullptr ? ((keyrec*)pvn)->len : 0;
  }

  template <class V, class A>
  inline  const V&  arbitrarymap<V, A>::GetVal( const void*  pvn )
  {
    assert( pvn != nullptr );

    return ((const keyrec*)pvn)->val;
  }

  template <class V, class A>
  inline  V&  arbitrarymap<V, A>::GetVal( void*  pvn )
  {
    assert( pvn != nullptr );

    return ((const keyrec*)pvn)->val;
  }

}

# endif  // __mtc_arbitrarymap_h__
