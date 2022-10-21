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
# if !defined( __prefixtree_h__ )
# define  __prefixtree_h__
# include "serialize.h"
# include <cstdint>
# include <cstdint>
# include <vector>
# include <memory>

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 4514 4786 4710 )
# endif // _MSC_VER

namespace mtc
{
  template <class V, class C = char>
  class prefix_tree
  {
    using char_type = typename std::make_unsigned<C>::type;

    class inplace_val
    {
      unsigned char   valbuf[sizeof(std::unique_ptr<V>)];

      class as_val
      {
      public:
        void  create( unsigned char* buf ) {  new ( buf ) V();  }
        void  create( unsigned char* buf, V&& val )  {  new ( buf ) V( std::move( val ) );  }
        void  create( unsigned char* buf, const V& val ) {  new ( buf ) V( val );  }

        void  assign( unsigned char* buf, V&& val )  {  *((V*)buf) = std::move( val );  }
        void  assign( unsigned char* buf, const V& val ) {  *((V*)buf) = val;  }

        void  clear( unsigned char* buf ) {  ((V*)buf)->~V();  }
      };
      class as_ptr
      {
      public:
        void  create( unsigned char* buf ) {  new ( buf ) std::unique_ptr<V>();  }
        void  create( unsigned char* buf, V&& val )  {  new ( buf ) std::unique_ptr<V>( new V( std::move( val ) ) );  }
        void  create( unsigned char* buf, const V& val ) {  new ( buf ) std::unique_ptr<V>( new V( val ) );  }

        void  assign( unsigned char* buf, V&& val )  {  *((std::unique_ptr<V>*)buf) = std::move( std::unique_ptr<V>( new V( std::move( val ) ) ) );  }
        void  assign( unsigned char* buf, const V& val ) {  *((std::unique_ptr<V>*)buf) = std::move( std::unique_ptr<V>( new V( val ) ) );  }

        void  clear( unsigned char* buf ) {  ((std::unique_ptr<V>*)buf)->~unique_ptr();  }
      };

      using implementation = typename std::conditional<sizeof(V) <= sizeof(valbuf), as_val, as_ptr>::type;

    public:
      inplace_val() = default;
     ~inplace_val() = default;
      inplace_val( inplace_val&&, bool );
      inplace_val( const inplace_val&, bool );
      inplace_val& operator = ( V&& );
      inplace_val& operator = ( const V& );
      inplace_val& operator = ( inplace_val&& );
      inplace_val& operator = ( const inplace_val& );

      auto  alloc() -> V&;
      void  clear( size_t& );
      auto  value() -> V&;
      auto  value() const -> const V&;
    };

    std::vector<prefix_tree>  anodes;
    inplace_val               avalue;
    prefix_tree*              powner = nullptr;
    char_type                 chnode;
    size_t                    ncount = 0;

    bool  has_val() const   {  return ncount & 0x01;  }

  public:
    class key;
    class value_type;

    class iterator;
    class const_iterator;

    using size_type = size_t;

  protected:
    prefix_tree( prefix_tree* p, C c ): powner( p ), chnode( c )  {}

  public:     // createion
    prefix_tree( C = 0 );
    prefix_tree( prefix_tree&& );
    prefix_tree( const prefix_tree& );
    prefix_tree& operator = ( prefix_tree&& );
    prefix_tree& operator = ( const prefix_tree& );
   ~prefix_tree();

  public:     // access
    auto  at( const key& k ) const -> const V&  {  return get_at( this, k );  }
    auto  at( const key& k ) -> V&  {  return get_at( this, k );  }

    auto  operator []( const key& k ) -> V& {  return put_at( k );  }

  public:     // Iterators
  public:     // Capacity
    bool  empty() const;
    auto  size() const -> size_t;

  public:     // Modifiers
    void  clear();

    auto  insert( const value_type& value ) -> std::pair<iterator, bool>;
    template< class P >
    std::pair<iterator, bool> insert( P&& value );
    std::pair<iterator, bool> insert( value_type&& value );
    iterator insert( iterator hint, const value_type& value );
    iterator insert( const_iterator hint, const value_type& value );
    template< class P >
    iterator insert( const_iterator hint, P&& value );
    iterator insert( const_iterator hint, value_type&& value );
    template< class InputIt >
    void insert( InputIt first, InputIt last );
    void insert( std::initializer_list<value_type> ilist );

    template< class... Args >
    std::pair<iterator,bool> emplace( Args&&... args );

    iterator erase( iterator pos );
    iterator erase( const_iterator pos );
    iterator erase( iterator first, iterator last );
    iterator erase( const_iterator first, const_iterator last );
    size_type erase( const key& key );

    void swap( prefix_tree& other ) noexcept;

    size_type count() const;

    iterator find( const key& key );
    const_iterator find( const key& key ) const;
    template <class K>
    iterator find( const K& x );
    template <class K>
    const_iterator find( const K& x ) const;

    bool contains( const key& key ) const;
    template <class K>
    bool contains( const K& x ) const;

    iterator lower_bound( const key& key );
    const_iterator lower_bound( const key& key ) const;
    template< class K >
    iterator lower_bound( const K& x );
    template< class K >
    const_iterator lower_bound( const K& x ) const;

    iterator upper_bound( const key& key );
    const_iterator upper_bound( const key& key ) const;
    template< class K >
    iterator upper_bound( const K& x );
    template< class K >
    const_iterator upper_bound( const K& x ) const;

  public:     // serialization
    size_t  GetBufLen();
    template <class O>
    O*      Serialize( O* ) const;
    template <class S>
    S*      FetchFrom( S* );

  protected:
    auto  expand( const char_type* pstr, size_t strl ) -> prefix_tree*
    {
      auto  pt = this;

      while ( strl > 0 )
      {
        auto  ptop = pt->anodes.begin();
        auto  pend = pt->anodes.end();
        auto  next = *pstr;

        if ( ptop != pend )
          while ( ptop->chnode < next && ++ptop != pend )
            (void)NULL;

        if ( ptop == pend || ptop->chnode != next )
        {
          ptop = pt->anodes.insert( ptop, std::move( prefix_tree( pt, next ) ) );

          for ( auto& node: pt->anodes )  node.powner = pt;
        }

        pt = pt->anodes.data() + (ptop - pt->anodes.begin());
          ++pstr;
          --strl;
      }

      return pt;
    }
    template <class S> static auto  search( S* self, const char_type* p, size_t l ) -> S*;
    template <class S> static auto  get_at( S* self, const key& k ) -> decltype(self->avalue.value());
    template <class S> static auto  ref_to( S* self ) -> decltype(self->avalue.value());
                              auto  put_at( const key& k ) -> V&;

  };

  template <class V, class C>
  class prefix_tree<V, C>::key
  {
    friend class prefix_tree;

  public:
    key( const C* p ): key( p, (size_t)-1 ) {}
    key( const C* p, size_t l ): ptr( p ), len( l )
    {
      if ( len == (size_t)-1 )
        for ( len = 0; ptr[len] != 0; ++len );
    }
    key( const std::basic_string<C>& s ): key( s.c_str(), s.length() )  {}
    key( const std::basic_string<C>& s, size_t l ): key( s.c_str(), l )  {}

  protected:
    const C*  ptr;
    size_t    len;

  };

  template <class V, class C>
  class prefix_tree<V, C>::value_type
  { public:
    key   k;
    V     v;
  };

  template <class V, class C>
  class prefix_tree<V, C>::iterator
  {
    friend class prefix_tree;

    prefix_tree*  pn;

  protected:
    iterator( prefix_tree* p ): pn( p ) {}

  public:
    iterator(): pn( nullptr ) {}
    iterator( iterator&& it ): pn( it.pn ) {  it.pn = nullptr;  }
    iterator( const iterator& it ): pn( it.pn ) {}

  };

  // prefix_tree::inplace_val implementation

  template <class V, class C>
  prefix_tree<V, C>::inplace_val::inplace_val( inplace_val&& iv, bool bh )
  {
    if ( bh ) implementation().create( valbuf, std::move( iv.value() ) );
      else implementation().create( valbuf );
  }

  template <class V, class C>
  prefix_tree<V, C>::inplace_val::inplace_val( const inplace_val& iv, bool bh )
  {
    if ( bh ) implementation().create( valbuf, iv.value() );
      else implementation().create( valbuf );
  }
  
  template <class V, class C>
  auto  prefix_tree<V, C>::inplace_val::operator = ( V&& val ) -> inplace_val&
  {
    return implementation().assign( valbuf, std::move( val ) ), *this;
  }

  template <class V, class C>
  auto  prefix_tree<V, C>::inplace_val::operator = ( const V& val ) -> inplace_val&
  {
    return implementation().assign( valbuf, val ), *this;
  }
/*
  template <class V, class C>
  auto  prefix_tree<V, C>::inplace_val::operator = ( inplace_val&& ) -> inplace_val&;
  template <class V, class C>
  auto  prefix_tree<V, C>::inplace_val::operator = ( const inplace_val& ) -> inplace_val&;

  template <class V, class C>
  auto  prefix_tree<V, C>::inplace_val::alloc() -> V&;
*/
  template <class V, class C>
  void  prefix_tree<V, C>::inplace_val::clear( size_t& ncount )
  {
    if ( ncount & 0x01 )
      implementation().clear( valbuf );
    ncount &= ~0x01;
  }

/*
  template <class V, class C>
  auto  prefix_tree<V, C>::inplace_val::value() -> V&;
  template <class V, class C>
  auto  prefix_tree<V, C>::inplace_val::value() const -> const V&;
*/

  // prefix_tree  implementation

  template <class V, class C>
  prefix_tree<V, C>::prefix_tree( C c ):
    chnode( (char_type) c ) {}

  template <class V, class C>
  prefix_tree<V, C>::prefix_tree( prefix_tree&& t ):
    anodes( std::move( t.anodes ) ),
    avalue( std::move( t.avalue ), t.has_val() ),
    chnode( t.chnode ),
    ncount( t.ncount )
  {
  }

  template <class V, class C>
  prefix_tree<V, C>::prefix_tree( const prefix_tree& t ):
    anodes( t.anodes ),
    avalue( t.avalue, t.has_val() ),
    chnode( t.chnode ),
    ncount( t.ncount )
  {
  }

  template <class V, class C>
  auto  prefix_tree<V, C>::operator = ( prefix_tree&& t ) -> prefix_tree&
  {
    anodes = std::move( t.anodes );
    if ( has_val() )
    chnode = std::move( t.chnode );
    ncount = std::move( t.ncount );

    if ( has_val() )
      avalue = std::move( t.avalue );

    return *this;
  }

  template <class V, class C>
  auto  prefix_tree<V, C>::operator = ( const prefix_tree& t ) -> prefix_tree&
  {
    if ( has_val() )
      avalue.clear();

    anodes = t.anodes;
    chnode = t.chnode;
    ncount = t.ncount;

    if ( has_val() )
      avalue = t.avalue;

    return *this;
  }

  template <class V, class C>
  prefix_tree<V, C>::~prefix_tree()
  {
    avalue.clear( ncount );
  }

/*
  template <class V, class counter, class M>
  inline  V*  prefixtree<V, counter, M>::Insert( const char* pszstr, size_t cchstr, const V& object )
  {
    V*  pvalue;

    if ( (pvalue = Insert( pszstr, cchstr )) != nullptr )
      *pvalue = object;

    return pvalue;
  }
  
  template <class V, class counter, class M>
  inline  V*  prefixtree<V, counter, M>::Insert( const char* pszstr, size_t cchstr, V&& object )
  {
    V*  pvalue;

    if ( (pvalue = Insert( pszstr, cchstr )) != nullptr )
      *pvalue = static_cast<V&&>( object );

    return pvalue;
  }
  
  template <class V, class counter, class M>
  inline  V*  prefixtree<V, counter, M>::Insert( const char* pszstr, size_t cchstr )
  {
    auto  expand = this;

    while ( cchstr > 0 )
    {
      auto  ptrtop = expand->begin();
      auto  ptrend = expand->end();
      auto  chfind = (unsigned char)*pszstr;
  
      if ( ptrtop < ptrend )
        while ( ptrtop->chnode > chfind && ++ptrtop < ptrend )
          (void)NULL;

      if ( ptrtop >= ptrend || ptrtop->chnode != chfind )
      {
        int   inspos;
  
        if ( ((nested*)expand)->Insert( inspos = (int)(ptrtop - expand->begin()), prefixtree( chfind ) ) == 0 )
          ptrtop = expand->begin() + inspos;  else return nullptr;
      }
      expand = ptrtop;
        ++pszstr;
        --cchstr;
    }

    if ( !expand->hasval )
      expand->hasval = true;

    return &expand->avalue;
  }
  
  template <class V, class counter, class M>
  inline  V*  prefixtree<V, counter, M>::Search( const char* pszstr, size_t cchstr )
  {
    return (V*)((const prefixtree<V, counter, M>*)this)->Search( pszstr, cchstr );
  }
  
  template <class V, class counter, class M>
  inline  const V*  prefixtree<V, counter, M>::Search( const char* pszstr, size_t cchstr ) const
  {
    auto  search = this;

    while ( cchstr > 0 )
    {
      auto  ptrtop = search->begin();
      auto  ptrend = search->end();
      auto  chfind = (unsigned char)*pszstr;
  
      if ( ptrtop < ptrend )
        while ( ptrtop->chnode > chfind && ++ptrtop < ptrend )
          (void)NULL;

      if ( ptrtop >= ptrend || ptrtop->chnode != chfind ) return nullptr;
        else search = ptrtop;

      ++pszstr;
      --cchstr;
    }
  
    return search->hasval ? &search->avalue : nullptr;
  }

  template <class V, class counter, class M>
  inline  void    prefixtree<V, counter, M>::DelAll()
  {
    if ( hasval )
      avalue.~V();
    this->SetLen( 0 );
  }
  
  template <class V, class counter, class M>
  inline  size_t  prefixtree<V, counter, M>::GetBufLen()
  {
    size_t    buflen = CounterGetBufLen();
    size_t    sublen;
  
    for ( auto p = this->begin(); p < this->end(); ++p )
      buflen += ::GetBufLen( sublen = p->GetBufLen() ) + sublen + 1;
  
    if ( hasval )
      buflen += ::GetBufLen( avalue );
  
    return (size_t)(length = (unsigned)buflen);
  }
  
  template <class V, class counter, class M>
  template <class O>
  inline  O*  prefixtree<V, counter, M>::Serialize( O* o ) const
  {
    if ( (o = CounterSerialize( o, (int)this->size(), hasval )) == nullptr )
      return nullptr;
  
    for ( auto p = this->begin(); p < this->end(); ++p )
      o = p->Serialize( ::Serialize( ::Serialize( o, p->chnode ), p->length ) );
  
    if ( hasval )
      o = ::Serialize( o, avalue );
  
    return o;
  }
  
  template <class V, class counter, class M>
  template <class A>
  inline  int       prefixtree<V, counter, M>::Enumerate( const A& action ) const
  {
    array<char, M>  keybuf( 0x40 );

    return ((prefixtree<V, counter, M>*)this)->enumerate( (A&)action, keybuf );
  }

  template <class V, class counter, class M>
  template <class A>
  inline  int       prefixtree<V, counter, M>::enumerate( A& action, array<char, M>& keybuf, int l )
  {
    int   nerror;
  
    for ( auto p = this->begin(); p < this->end(); ++p )
    {
      if ( keybuf.size() <= l && keybuf.SetLen( l + 0x10 ) != 0 )
        return ENOMEM;

      keybuf[l] = p->chnode;

      if ( (nerror = p->enumerate( action, keybuf, l + 1 )) != 0 )
        return nerror;
    }
  
    return hasval ? action( keybuf, l, avalue ) : 0;
  }
*/
  template <class V, class C>
  void  prefix_tree<V, C>::clear()
  {
    anodes.clear();
    if ( has_val() )
      avalue.clear();
    chnode = 0;
    ncount = 0;
  }

  template <class V, class C>
  auto  prefix_tree<V, C>::insert( const value_type& keyval ) -> std::pair<iterator, bool>
  {
    auto  node = expand( (const char_type*)keyval.k.ptr, keyval.k.len );

    if ( node->pvalue != nullptr )
      return { iterator( node ), false };
    node->pvalue = std::move( std::unique_ptr<V>( new V( keyval.v ) ) );
      return { iterator( node ), true };
  }

  template <class V, class C>
  template <class S>
  auto  prefix_tree<V, C>::search( S* self, const char_type* pszstr, size_t cchstr ) -> S*
  {
    while ( cchstr > 0 )
    {
      auto  ptop = self->anodes.begin();
      auto  pend = self->anodes.end();
      auto  next = *pszstr;

      if ( ptop != pend )
        while ( ptop->chnode < next && ++ptop != pend ) (void)NULL;

      if ( ptop == pend || ptop->chnode != next ) return nullptr;
        else self = self->anodes.data() + (ptop - self->anodes.begin());

      ++pszstr;
      --cchstr;
    }

    return self->has_val() ? self : nullptr;
  }

  template <class V, class C>
  template <class S>
  auto  prefix_tree<V, C>::get_at( S* self, const key& k ) -> decltype(self->avalue.value())
  {
    return ref_to( prefix_tree::template search( self, (const char_type*)k.ptr, k.len ) );
  }

  template <class V, class C>
  template <class S>
  auto  prefix_tree<V, C>::ref_to( S* node ) -> decltype(node->avalue.value())
  {
    if ( node == nullptr || !node->has_val() )
      throw std::range_error( "key not found" );
    return node->avalue.value();
  }

  template <class V, class C>
  auto  prefix_tree<V, C>::put_at( const key& k ) -> V&
  {
    auto  node = expand( (const char_type*)k.ptr, k.len );

    if ( !node->has_val() ) return node->ncount |= 0x01, node->avalue.alloc();
      else return node->avalue.value();
  }

}  // mtc namespace

# endif  // __prefixtree_h__
