/*

The MIT License (MIT)

Copyright (c) 2000-2026 Андрей Коваленко aka Keva
  keva@rambler.ru
  phone: +7(495)648-4058, +7(926)513-2991

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
# include <type_traits>

namespace mtc
{

  template <class V, class A = std::allocator<char>>
  class   arbitrarymap
  {
    template <class Base, class T>
    using rebind = typename std::allocator_traits<Base>::template rebind_alloc<T>;

    A   alloc;

    struct  mapkey
    {
      const void* key;
      size_t      len;

    public:
      mapkey( const char* str, size_t cch = (size_t)-1 ): key( str ), len( cch )
      {
        if ( len == (size_t)-1 )
          for ( len = 0; str[len] != 0; ++len ) (void)NULL;
      }
      mapkey( const widechar* str, size_t cch = (size_t)-1 ): key( str ), len( cch )
      {
        if ( len == (size_t)-1 )
          for ( len = 0; str[len] != 0; ++len ) (void)NULL;
        len *= 2;
      }
    };

    struct  keyrec
    {
      V         val;
      unsigned  pos;
      keyrec*   lpn;
      size_t    len;
      char      key[1];

    public:   // construction
      keyrec( const V& v, unsigned p, keyrec* n ): val( v ), pos( p ), lpn( n ) {}
      keyrec( V&& v, unsigned p, keyrec* n ): val( std::move( v ) ), pos( p ), lpn( n ) {}
      keyrec( unsigned p, keyrec* n ):           pos( p ), lpn( n ) {}

    };

  protected:  // allocation
    template <class... constructor_args>
    keyrec* Create( const void* k, size_t l, constructor_args... a )
    {
      auto    nalloc = (sizeof(keyrec) + l + sizeof(keyrec) - 1) / sizeof(keyrec);
      keyrec* palloc;

      if ( (palloc = rebind<A, keyrec>( alloc ).allocate( nalloc )) != nullptr )
      {
        new( palloc ) keyrec( std::forward<constructor_args>( a )... );
          memcpy( palloc->key, k, palloc->len = l );
      }
      return palloc;
    }
    template <class S, class R = typename std::conditional<std::is_const<S>::value, const V, V>::type>
    static  auto  Search( const void*, size_t, S& ) -> R*;
    template <class S, class R = typename std::conditional<std::is_const<S>::value, const void, void>::type>
    static  auto  Lookup( const void*, size_t, S& ) -> R*;

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
    arbitrarymap( unsigned tablen, const A&, const std::initializer_list<std::pair<mapkey, V>>& );
    arbitrarymap( const A& a, const std::initializer_list<std::pair<mapkey, V>>& v ):
      arbitrarymap( 69959, a, v ) {}
    arbitrarymap( unsigned tablen, const std::initializer_list<std::pair<mapkey, V>>& v ):
      arbitrarymap( 69959, {}, v ) {}
    arbitrarymap( const std::initializer_list<std::pair<mapkey, V>>& v ):
      arbitrarymap( 69959, {}, v ) {}
   ~arbitrarymap();

  protected:
    arbitrarymap( const arbitrarymap& ) = delete;
    arbitrarymap& operator = ( const arbitrarymap& ) = delete;

  public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    class Key
    {
      const void* ptr;
      size_type   len;

    public:
      Key( const void* p, size_type l ): ptr( p ), len( l ) {}
      template <class K>
      Key( const K& k ): Key( k.data(), k.size() ) {}

      const void* data() const {  return ptr;  }
      size_type   size() const {  return len;  }
    };

    using key_type = Key;
    using mapped_type = V;
    using allocator_type = A;

    struct value_type
    {
      const Key key;
      V&        value;
    };

    struct const_value_type
    {
      const Key key;
      const V&  value;
    };

    class iterator;
    class const_iterator;

  protected:
    template <class value_t>
    class iterator_base
    {
      friend class arbitrarymap;

      using mapptr_t = typename std::conditional<std::is_const<value_t>::value,
        const arbitrarymap*, arbitrarymap*>::type;
      using objptr_t = typename std::conditional<std::is_const<value_t>::value,
        const void*, void*>::type;

      using vstore_t = typename std::aligned_storage<sizeof(value_t)>::type;

      mapptr_t  parent = nullptr;
      objptr_t  objptr = nullptr;
      vstore_t  avalue;

      iterator_base( mapptr_t p, objptr_t o );
      iterator_base( mapptr_t p ):
        iterator_base( p, p != nullptr ? p->Enum( nullptr ) : nullptr ) {}

    public:
      iterator_base()
        {}
      iterator_base( const iterator_base& );
      iterator_base& operator = ( const iterator_base& );

      bool  operator ==( const iterator_base& it ) const {  return objptr == it.objptr;  }
      bool  operator !=( const iterator_base& it ) const {  return objptr != it.objptr;  }

      iterator_base& operator++();
      iterator_base  operator++( int );

      value_t& operator*();
      const value_t& operator*() const;
      value_t* operator -> ();
      const value_t* operator -> () const;
    };

    const void*   Delete( const void* );
  public:     // methods
    int           Append( const arbitrarymap& );
    size_type     Delete( const void* k, size_type );
    size_type     Delete( const widechar* s ) {  return Delete( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    size_type     Delete( const char* s )     {  return Delete( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    void          DelAll();
    unsigned      GetLen() const  {  return ncount;  }
    unsigned      MapLen() const  {  return maplen;  }
          V*      Insert( const void* k, size_t l, const V& v = V() );
          V*      Insert( const void* k, size_t l, V&& );
          V*      Insert( const widechar* s, const V& v = V() ) {  return Insert( s, sizeof(*s) * (w_strlen( s ) + 1), v );  }
          V*      Insert( const char*     s, const V& v = V() ) {  return Insert( s, sizeof(*s) * (w_strlen( s ) + 1), v );  }
    const V*      Search( const void* k, size_t l ) const {  return Search( k, l, *this);  }
    const V*      Search( const widechar* s ) const {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    const V*      Search( const char*     s ) const {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
          V*      Search( const void* k, size_t l )       {  return Search( k, l, *this ); }
          V*      Search( const widechar* s ) {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
          V*      Search( const char*     s ) {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }

  public:     // serialization
    auto  GetBufLen() const -> size_t;
    template <class O>
    auto  Serialize( O* ) const -> O*;
    template <class S>
    auto  FetchFrom( S* ) -> S*;

  public:     // stl compat
    auto    begin() -> iterator {  return iterator( this );  }
    auto    begin() const -> const_iterator {  return const_iterator( this );  }
    auto    cbegin() const -> const_iterator {  return const_iterator( this );  }

    auto    end() -> iterator {  return iterator();  }
    auto    end() const -> const_iterator {  return const_iterator();  }
    auto    cend() const -> const_iterator {  return const_iterator();  }

    bool    empty() const  {  return GetLen() == 0;  }
    size_type  size() const  {  return GetLen();  }

    void    clear() {  DelAll();  }

  // modifiers
    iterator  insert( const const_value_type& v )
      {  return iterator( this, Insert( v.key, v.value ) );  }
    iterator  erase( iterator it )
      {  return { this, Delete( it.objptr ) };  }
    iterator  erase( const_iterator it )
      {  return { this, Delete( it.objptr ) };  }
    size_type erase( const Key& key )
      {  return Delete( key.ptr, key.len );  }

  // lookup
    V&  at( const Key& );
    const V&  at( const Key& ) const;

    V&  operator []( const Key& );
    size_type count( const Key& ) const;

    iterator  find( const Key& key )
      {  return { this, Lookup( key.data(), key.size(), *this ) };  }
    const_iterator  find( const Key& key ) const
      {  return { this, Lookup( key.data(), key.size(), *this ) };  }

    bool  contains( const Key& key ) const
      {  return Lookup( key.data(), key.size(), *this ) != nullptr;  }

  public:     // template wrappers
    template <class K>  size_type Delete( const K& k )
      {  return Delete( k.data(), k.size() );  }
    template <class K>  V*        Insert( const K& k, const V& v = V() )
      {  return Insert( k.data(), k.size(), v );  }
    template <class K>  V*        Search( const K& k )
      {  return Search( k.data(), k.size() );  }
    template <class K>  const V*  Search( const K& k ) const
      {  return Search( k.data(), k.size() );  }

  // enumerator support methods
    const void*         Enum( const void* ) const;
          void*         Enum( void* );
    static  const void* GetKey( const void* );
    static  size_t      KeyLen( const void* );
    static  const V&    GetVal( const void* );
    static  V&          GetVal(       void* );
          void*         GetPtr( const void*, size_t );

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

  template <class V, class A>
  class arbitrarymap<V, A>::iterator: public iterator_base<value_type>
    {  using iterator_base<value_type>::iterator_base;  };

  template <class V, class A>
  class arbitrarymap<V, A>::const_iterator: public iterator_base<const_value_type>
    {  using iterator_base<const_value_type>::iterator_base;  };

  // arbitratymap::iterator_base implementation

  template <class V, class A>
  template <class value_t>
  arbitrarymap<V, A>::iterator_base<value_t>::iterator_base( mapptr_t p, objptr_t o ):
    parent( p ),
    objptr( o )
  {
    if ( objptr != nullptr )
      new( &avalue ) value_t{ Key( GetKey( objptr ), KeyLen( objptr ) ), GetVal( objptr ) };
  }

  template <class V, class A>
  template <class value_t>
  arbitrarymap<V, A>::iterator_base<value_t>::iterator_base( const iterator_base& it ):
    parent( it.parent ),
    objptr( it.objptr ),
    avalue( it.avalue )
  {
  }

  template <class V, class A>
  template <class value_t>
  auto  arbitrarymap<V, A>::iterator_base<value_t>::operator = ( const iterator_base& it ) -> iterator_base&
  {
    parent = it.parent;
    objptr = it.objptr;
    avalue = it.avalue;
    return *this;
  }

  template <class V, class A>
  template <class value_t>
  auto  arbitrarymap<V, A>::iterator_base<value_t>::operator++() -> iterator_base&
  {
    if ( objptr != nullptr && parent != nullptr )
      if ( (objptr = parent->Enum( objptr )) != nullptr )
        new( &avalue ) value_t{ Key( GetKey( objptr ), KeyLen( objptr ) ), GetVal( objptr ) };
    return *this;
  }

  template <class V, class A>
  template <class value_t>
  auto  arbitrarymap<V, A>::iterator_base<value_t>::operator++( int ) -> iterator_base
  {
    auto  r( *this );
      this->operator++();
    return r;
  }

  template <class V, class A>
  template <class value_t>
  auto  arbitrarymap<V, A>::iterator_base<value_t>::operator*() const -> const value_t&
  {
    if ( objptr != nullptr )
      return *(const value_t*)&avalue;
    throw std::out_of_range( "arbitrarymap::iterator_base::operator*" );
  }

  template <class V, class A>
  template <class value_t>
  auto  arbitrarymap<V, A>::iterator_base<value_t>::operator*() -> value_t&
  {
    if ( objptr != nullptr )
      return *(value_t*)&avalue;
    throw std::out_of_range( "arbitrarymap::iterator_base::operator*" );
  }

  template <class V, class A>
  template <class value_t>
  auto  arbitrarymap<V, A>::iterator_base<value_t>::operator -> () const -> const value_t*
  {
    if ( objptr != nullptr )
      return (const value_t*)&avalue;
    throw std::out_of_range( "arbitrarymap::iterator_base::operator->" );
  }

  template <class V, class A>
  template <class value_t>
  auto  arbitrarymap<V, A>::iterator_base<value_t>::operator -> () -> value_t*
  {
    if ( objptr != nullptr )
      return (value_t*)&avalue;
    throw std::out_of_range( "arbitrarymap::iterator_base::operator->" );
  }

  // arbitrarymap implementation

  template <class V, class A>
  arbitrarymap<V, A>::arbitrarymap( unsigned tablen, const A& a, const std::initializer_list<std::pair<mapkey, V>>& v ):
    arbitrarymap( tablen, a )
  {
    for ( auto& value: v )
      Insert( value.first.key, value.first.len, value.second );
  }

  template <class V, class A>
  arbitrarymap<V, A>::~arbitrarymap()
  {
    if ( pitems != nullptr )
    {
      DelAll();

      rebind<A, keyrec*>( alloc ).deallocate( pitems, maplen );
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
  template <class S, class R>
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
  template <class S, class R>
  auto  arbitrarymap<V, A>::Lookup( const void* k, size_t l, S& s ) -> R*
  {
    if ( s.pitems != nullptr )
    {
      unsigned  pos = gethash( (const unsigned char*)k, l ) % s.maplen;
      auto      ptr = s.pitems[pos];

      while ( ptr != nullptr && !isequal( ptr->key, ptr->len, k, l ) )
        ptr = ptr->lpn;

      return ptr;
    }
    return nullptr;
  }

  template <class V, class A>
  auto  arbitrarymap<V, A>::Delete( const void* node ) -> const void*
  {
    keyrec**  res = nullptr;

    if ( pitems != nullptr && ncount != 0 && node != nullptr )
    {
      unsigned  pos = gethash( (const unsigned char*)GetKey( node ), KeyLen( node ) ) % maplen;
      keyrec**  ptr = &pitems[pos];

      while ( *ptr != nullptr && *ptr != node )
        ptr = &(*ptr)->lpn;

      if ( *ptr != nullptr )
      {
        auto  del = *ptr;

        res = *ptr = del->lpn;
        del->~keyrec();
        rebind<A, keyrec>( alloc ).deallocate( del, 0 );
        --ncount;
      }
    }
    return res;
  }

  template <class V, class A>
  auto  arbitrarymap<V, A>::Delete( const void* k, size_t l ) -> size_type
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
      return 1;
    }
    return 0;
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
  V*    arbitrarymap<V, A>::Insert( const void* k, size_t l, V&& v )
  {
    unsigned  pos = gethash( (const byte_t*)k, l ) % maplen;
    keyrec*   ptr;

    if ( pitems == nullptr && NewMap() != 0 )
      return nullptr;

    if ( (ptr = Create( k, l, std::move( v ), pos, pitems[pos] )) != nullptr ) pitems[pos] = ptr;
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

// std lookup

  template <class V, class A>
  V&  arbitrarymap<V, A>::at( const Key& key )
  {
    auto  pfound = Search( key );

    if ( pfound != nullptr )
      return *pfound;
    throw std::out_of_range( "arbitrarymap<V, A>::at" );
  }

  template <class V, class A>
  const V&  arbitrarymap<V, A>::at( const Key& key ) const
  {
    auto  pfound = Search( key );

    if ( pfound != nullptr )
      return *pfound;
    throw std::out_of_range( "arbitrarymap<V, A>::at" );
  }

  template <class V, class A>
  V&  arbitrarymap<V, A>::operator []( const Key& key )
  {
    auto  pfound = Search( key );

    return pfound != nullptr ? pfound : Insert( key );
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
  size_t  arbitrarymap<V, A>::KeyLen( const void*  pvn )
  {
    return pvn != nullptr ? ((keyrec*)pvn)->len : 0;
  }

  template <class V, class A>
  const V&  arbitrarymap<V, A>::GetVal( const void*  pvn )
  {
    assert( pvn != nullptr );

    return ((const keyrec*)pvn)->val;
  }

  template <class V, class A>
  V&  arbitrarymap<V, A>::GetVal( void*  pvn )
  {
    assert( pvn != nullptr );

    return ((keyrec*)pvn)->val;
  }

  template <class V, class A>
  auto  arbitrarymap<V, A>::GetPtr( const void* k, size_t l ) -> void*
  {
    if ( pitems != nullptr )
    {
      unsigned  pos = gethash( (const unsigned char*)k, l ) % maplen;
      auto      ptr = pitems[pos];

      while ( ptr != nullptr && !isequal( ptr->key, ptr->len, k, l ) )
        ptr = ptr->lpn;

      return ptr;
    }
    return nullptr;
  }

}

# endif  // __mtc_arbitrarymap_h__
