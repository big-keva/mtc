/*

The MIT License (MIT)

Copyright (c) 2000-2022 Андрей Коваленко aka Keva
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
# if !defined( __mtc_arbitrary_map_h__ )
# define  __mtc_arbitrary_map_h__
# include "platform.h"
# include <functional>
# include <cstring>

namespace mtc
{
  class arbitrary_hash
  {
    public: size_t operator ()( const void* k, size_t l ) const;
  };

  template <class T, class H = arbitrary_hash>
  class arbitrary_map
  {
    enum: size_t {  default_map_len = 69959 };

  public:
    struct key_type
    {
      const char* data;
      size_t      size;

      bool  operator == ( const key_type& k ) const
      {  return size == k.size && memcmp( data, k.data, size ) == 0;  }
    };

    class Hash;

    using mapped_type     = T;
    using value_type      = std::pair<key_type, mapped_type>;
    using size_type       = std::size_t;
    using diff_type       = std::ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
//    pointer
//    const_pointer
    class iterator;
    class const_iterator;

  protected:
    class base_iterator;

  public:
    arbitrary_map(): arbitrary_map( default_map_len ) {}
    arbitrary_map( size_type mlen, const H& hash = H() );
    arbitrary_map( const arbitrary_map& other );
    arbitrary_map( arbitrary_map&& other );
    template <class InputIt>
    arbitrary_map( InputIt first, InputIt last,
      size_type map_len = default_map_len,
      const H& hash = H() );
    arbitrary_map( std::initializer_list<value_type> init,
      size_type map_len = default_map_len,
      const H& hash = H() );
   ~arbitrary_map();

    arbitrary_map& operator=( const arbitrary_map& other );
    arbitrary_map& operator=( arbitrary_map&& other );
    arbitrary_map& operator=( std::initializer_list<value_type> ilist );

    auto  begin() -> iterator {  return iterator( this );  }
    auto  begin() const -> const_iterator {  return const_iterator( (arbitrary_map<T, H>*)this );  }
    auto  cbegin() const -> const_iterator {  return const_iterator( (arbitrary_map<T, H>*)this );  }

    auto  end() -> iterator {  return iterator();  }
    auto  end() const -> const_iterator {  return const_iterator();  }
    auto  cend() const -> const_iterator {  return const_iterator();  }

    bool  empty() const;
    auto  size() const -> size_type;
    void  clear();

    auto  insert( const value_type& value ) -> std::pair<iterator,bool> {  return set<const T&>( value.first, value.second );  }
    auto  insert( value_type&& value ) -> std::pair<iterator,bool>  {  return set<T&&>( value.first, std::move( value.second ) );  }
    template< class InputIt >
    void  insert( InputIt first, InputIt last );
    void  insert( std::initializer_list<value_type> ilist );

    auto  insert_or_assign( const key_type& k, mapped_type&& obj ) -> iterator;

    auto  erase( const_iterator pos ) -> iterator;
    auto  erase( const_iterator first, const_iterator last ) -> iterator;
    auto  erase( const key_type& key ) -> size_type;

    void  swap( arbitrary_map& other ) noexcept;

    auto  at( const key_type& key ) -> T& {  return at<T>( *this, key );  }
    auto  at( const key_type& key ) const -> const T& {  return at<const T>( *this, key );  }

    auto  operator []( const key_type& key ) -> T&  {  return insert( { key, T() } ).first->second;  }

    auto  count( const key_type& key ) const -> size_type;

    auto  find( const key_type& key ) -> iterator {  return find<iterator>( *this, key );  }
    auto  find( const key_type& key ) const -> const_iterator {  return find<const_iterator>( *this, key );  }

    bool  contains( const key_type& key ) const;

    auto  equal_range( const key_type& key ) -> std::pair<iterator,iterator>;
    auto  equal_range( const key_type& key ) const -> std::pair<const_iterator,const_iterator>;

  protected:
    struct  keyrec
    {
      T         val;
      unsigned  pos;
      keyrec*   lpn = nullptr;
      size_t    len;
      char      key[1];

    public:   // construction
      keyrec( const T& v, unsigned p ): val( v ), pos( p ) {}
      keyrec( T&& v,      unsigned p ): val( std::move( v ) ), pos( p ) {}
      keyrec(             unsigned p ): pos( p ) {}

    };

  protected:  // allocation
    template <class R, class S>
    static  R&    at( S& map, const key_type& key );
    template <class R, class S>
    static  R     find( S& map, const key_type& key );
    template <class R, class S>
    static  R*    get( S& map, const key_type& key );
    template <class C>
            auto  set( const key_type& key, C val ) -> std::pair<iterator, bool>;

  private:
    keyrec**  pitems;
    unsigned  maplen;
    unsigned  ncount;
    H         hasher;

  };

  template <class T, class H>
  class arbitrary_map<T, H>::base_iterator
  {
    using keyrec = typename arbitrary_map<T, H>::keyrec;

  protected:
    base_iterator( arbitrary_map<T, H>* s, keyrec* n = nullptr ): pmap( s ), next( n )
    {
      if ( n == nullptr && s != nullptr && s->pitems != nullptr )
        for ( auto p = s->pitems, e = s->pitems + s->maplen; p != e && next == nullptr; ++p )
          next = *p;
    }

    template <class V>
    struct base_value
    {
      key_type  first;
      V&        second;

      bool  operator == ( const value_type& to ) const {  return first == to.first && second == to.second;  }
      bool  operator != ( const value_type& to ) const {  return !(*this == to);  }
    };

    using storage_t = typename std::aligned_storage<sizeof(base_value<int>)>::type;

  public:
    base_iterator() = default;
    base_iterator(
      const base_iterator& s ): pmap( s.pmap ), next( s.next )  {}

    bool  operator == ( const base_iterator& it ) const
    {  return pmap == nullptr ? it.pmap == nullptr : next == it.next;  }
    bool  operator != ( const base_iterator& it ) const
    {  return !(*this== it);  }

    auto  operator ++ () -> base_iterator&
    {
      if ( pmap != nullptr && next != nullptr )
      {
        auto  pptr = pmap->pitems + next->pos;

        if ( next->pos >= pmap->maplen )
          throw std::range_error( "invalid iterator operation" );

        while ( *pptr != nullptr && *pptr != next )
          pptr = &(*pptr)->lpn;

        if ( *pptr != nullptr ) pptr = &(*pptr)->lpn;
          else throw std::range_error( "invalid iterator operation" );

        if ( *pptr == nullptr )
        {
          auto  pend = pmap->pitems + pmap->maplen;

          for ( pptr = pmap->pitems + next->pos + 1; pptr != pend && *pptr == nullptr; ++pptr )
            (void)NULL;

          if ( pptr == pend )
            return next = nullptr, pval = nullptr, *this;
        }

        return next = *pptr, pval = nullptr, *this;
      }
      throw std::range_error( "invalid iterator operation" );
    }
    auto  operator ++ (int) -> base_iterator
    {
      auto  curr( *this );
        this->operator++();
      return curr;
    }

  protected:
    template <class R>
    base_value<R>*  GetPtr() const
    {
      if ( pval != nullptr )
        return pval;
      if ( pmap == nullptr || next == nullptr )
        throw std::range_error( "invalid iterator dereferencing" );
      return (base_value<R>*)new( &cval ) base_value<T>{ { next->key, next->len }, next->val };
    }

  protected:
    arbitrary_map<T, H>*    pmap = nullptr;
    keyrec*                 next = nullptr;
    mutable storage_t       cval;
    mutable base_value<T>*  pval = nullptr;

  };

  template <class T, class H>
  class arbitrary_map<T, H>::const_iterator: public base_iterator
  {
    friend class arbitrary_map<T, H>;

    using iterator_value = typename base_iterator::template base_value<const T>;

  public:
    using base_iterator::base_iterator;
    using base_iterator::operator++;

    auto  operator * () const -> const iterator_value&  {  return *this->template GetPtr<const T>();  }
    auto  operator -> () const -> const iterator_value*  {  return this->template GetPtr<const T>();  }

  };

  template <class T, class H>
  class arbitrary_map<T, H>::iterator: public const_iterator
  {
    friend class arbitrary_map<T, H>;

    using iterator_value = typename base_iterator::template base_value<T>;

  public:
    using const_iterator::const_iterator;
    using const_iterator::operator++;

    auto  operator * () const -> iterator_value&  {  return *this->template GetPtr<T>();  }
    auto  operator -> () const -> iterator_value*  {  return this->template GetPtr<T>();  }

  };

  // arbitrary_hash implementation

  inline
  size_t  arbitrary_hash::operator ()( const void* p, size_t l ) const
  {
    auto    pData = (const unsigned char*)p;
    size_t  nHash = 0;

    while ( l-- > 0 )
      nHash = (nHash << 5) + nHash + *pData++;
    return nHash;
  }

  // arbitrary_map implementation

  template <class T, class H>
  arbitrary_map<T, H>::arbitrary_map(
    size_type mlen,
    const H&  hash ): pitems( nullptr ), maplen( mlen ), ncount( 0 ), hasher( hash )  {}

  template <class T, class H>
  arbitrary_map<T, H>::arbitrary_map(
    const arbitrary_map& other ): arbitrary_map( other.begin(), other.end(), other.maplen ) {}

  template <class T, class H>
  arbitrary_map<T, H>::arbitrary_map( arbitrary_map&& other )
  {
    pitems = other.pitems;  other.pitems = nullptr;
    ncount = other.ncount;  other.ncount = 0;
    maplen = other.maplen;
  }

  template <class T, class H>
  arbitrary_map<T, H>::arbitrary_map(
    std::initializer_list<value_type> init,
    size_type                         mlen,
    const H&                          hash ):
      arbitrary_map( init.begin(), init.end(), mlen, hash ) {}

  template <class T, class H>
  template <class InputIt>
  arbitrary_map<T, H>::arbitrary_map(
    InputIt   _beg,
    InputIt   _end,
    size_type _len,
    const H&  _hsh ): arbitrary_map( _len, _hsh )
  {
    while ( _beg != _end )
      insert( *_beg++ );
  }

  template <class T, class H>
  arbitrary_map<T, H>::~arbitrary_map()
  {
    if ( pitems != nullptr )
    {
      clear();
      delete [] pitems;
    }
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::operator = ( const arbitrary_map& other ) -> arbitrary_map&
  {
    clear();
    return insert( other.begin(), other.end() ), *this;
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::operator = ( arbitrary_map&& other ) -> arbitrary_map&
  {
    clear();

    if ( pitems != nullptr )
      delete [] pitems;

    pitems = other.pitems;  other.pitems = nullptr;
    ncount = other.ncount;  other.ncount = 0;
    maplen = other.maplen;
    return *this;
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::operator = ( std::initializer_list<value_type> ilist ) -> arbitrary_map&
  {
    clear();
    return insert( ilist.begin(), ilist.end() ), *this;
  }

  template <class T, class H>
  bool  arbitrary_map<T, H>::empty() const {  return ncount == 0;  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::size() const -> size_type {  return ncount;  }

  template <class T, class H>
  void  arbitrary_map<T, H>::clear()
  {
    if ( pitems != nullptr )
    {
      for ( auto p = pitems, e = pitems + maplen; p != e; ++p )
      {
        for ( auto del = *p; del != nullptr; )
        {
          auto  lpn = del->lpn;
            del->~keyrec();
            delete [] (char*)del;
          del = lpn;
        }
        *p = nullptr;
      }
    }
  }

  template <class T, class H>
  template< class InputIt >
  void  arbitrary_map<T, H>::insert( InputIt first, InputIt last )
  {
    while ( first != last )
      insert( *first++ );
  }

  template <class T, class H>
  void  arbitrary_map<T, H>::insert( std::initializer_list<value_type> ilist )
  {
    return insert( ilist.begin(), ilist.end() );
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::insert_or_assign( const key_type& key, mapped_type&& val ) -> iterator
  {
    auto  hash = hasher( key.data, key.size );
    auto  hpos = hash % maplen;
    auto  pptr = pitems + hpos;

    if ( pitems == nullptr )
    {
      pitems = new keyrec* [maplen];
        memset( pitems, 0, sizeof(keyrec*) * maplen );
      pptr = pitems + hpos;
        ncount = 0;
    }

    while ( *pptr != nullptr )
      if ( key == key_type{ (*pptr)->key, (*pptr)->len } )
      {
        (*pptr)->val = std::move( val );
        return { this, *pptr };
      }
        else
      pptr = &(*pptr)->lpn;

    new( *pptr = (keyrec*)new char[sizeof(keyrec) + key.size - 1] )
      keyrec( std::move( val ), hpos );
    memcpy( (*pptr)->key, key.data, (*pptr)->len = key.size );

    return { this, *pptr };
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::erase( const_iterator pos ) -> iterator
  {
    if ( pos.pmap == nullptr || pos.next == nullptr )
      return iterator();

    if ( pos.pmap == this && pos.next->pos < maplen )
    {
      auto  pptr = pitems + pos.next->pos;
      auto  eptr = pitems + maplen;

      while ( *pptr != nullptr && *pptr != pos.next )
        *pptr = &(*pptr)->lpn;

      if ( *pptr == pos.next )
      {
        auto  free = *pptr;

        *pptr = free->lpn;

        free->~keyrec();
        delete [] (char*)free;
      }

      while ( pptr != eptr && *pptr == nullptr )
        ++pptr;

      if ( pptr != eptr ) return iterator( this, *pptr );
        else return iterator();
    }
    throw std::logic_error( "invalid iterator" );
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::erase( const_iterator first, const_iterator last ) -> iterator
  {
    auto  it = iterator();

    while ( first != last )
      it = erase( first++ );

    return it;
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::erase( const key_type& key ) -> size_type
  {
    auto  hash = hasher( key.data, key.size );
    auto  hpos = hash % maplen;
    auto  pptr = pitems + hpos;
    auto  ndel = (size_type)0;

    if ( pitems == nullptr )
      return 0;
    while ( *pptr != nullptr )
      if ( key == key_type{ (*pptr)->key, (*pptr)->len } )
      {
        auto  pdel = *pptr;
          *pptr = (*pptr)->lpn;
        pdel->~keyrec();
          delete [] (char*)pdel;
        ++ndel;
      }
        else
      pptr = &(*pptr)->lpn;

    return ndel;
  }

  template <class T, class H>
  void  arbitrary_map<T, H>::swap( arbitrary_map& other ) noexcept
  {
    if ( this != &other )
    {
      std::swap( pitems, other.pitems );
      std::swap( maplen, other.maplen );
      std::swap( ncount, other.ncount );
      std::swap( hasher, other.hasher );
    }
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::count( const key_type& key ) const -> size_type
  {
    return get<keyrec>( *this, key ) != nullptr ? 1 : 0;
  }

  template <class T, class H>
  bool  arbitrary_map<T, H>::contains( const key_type& key ) const
  {
    auto  hash = hasher( key.data, key.size );
    auto  hpos = hash % maplen;
    auto  pptr = pitems + hpos;

    if ( pitems == nullptr )
      return false;

    while ( *pptr != nullptr )
      if ( key == key_type{ (*pptr)->key, (*pptr)->len } )  return 1;
        else pptr = &(*pptr)->lpn;

    return 0;
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::equal_range( const key_type& key ) -> std::pair<iterator, iterator>
  {
    auto  it = find( key );
    return { it, it };
  }

  template <class T, class H>
  auto  arbitrary_map<T, H>::equal_range( const key_type& key ) const -> std::pair<const_iterator, const_iterator>
  {
    auto  it = find( key );
    return { it, it };
  }

  template <class T, class H>
  template <class R, class S>
  R&  arbitrary_map<T, H>::at( S& map, const key_type& key )
  {
    auto  prec = get<typename S::keyrec>( map, key );

    if ( prec != nullptr )  return prec->val;
      else throw std::out_of_range( "argument out of range" );
  }

  template <class T, class H>
  template <class R, class S>
  R  arbitrary_map<T, H>::find( S& map, const key_type& key )
  {
    auto  prec = get<typename S::keyrec>( map, key );

    if ( prec != nullptr )  return { &map, prec };
      else return {};
  }

  template <class T, class H>
  template <class R, class S>
  R* arbitrary_map<T, H>::get( S& self, const key_type& key )
  {
    auto  hash = self.hasher( key.data, key.size );
    auto  hpos = hash % self.maplen;
    auto  pptr = self.pitems + hpos;

    if ( self.pitems == nullptr )
      return nullptr;

    while ( *pptr != nullptr )
      if ( key == key_type{ (*pptr)->key, (*pptr)->len } )  return *pptr;
        else pptr = &(*pptr)->lpn;

    return nullptr;
  }

  template <class T, class H>
  template <class C>
  auto  arbitrary_map<T, H>::set( const key_type& key, C val ) -> std::pair<iterator,bool>
  {
    auto  hash = hasher( key.data, key.size );
    auto  hpos = hash % maplen;
    auto  pptr = pitems + hpos;

    if ( pitems == nullptr )
    {
      pitems = new keyrec* [maplen];
        memset( pitems, 0, sizeof(keyrec*) * maplen );
      pptr = pitems + hpos;
        ncount = 0;
    }

    while ( *pptr != nullptr )
    {
      if ( key == key_type{ (*pptr)->key, (*pptr)->len } )
        return { iterator( this, *pptr ), false };
      else pptr = &(*pptr)->lpn;
    }

    new( *pptr = (keyrec*)new char[sizeof(keyrec) + key.size - 1] )
      keyrec( static_cast<C>( val ), hpos );
    memcpy( (*pptr)->key, key.data, (*pptr)->len = key.size );

    return { iterator( this, *pptr ), true };
  }

}

# endif  // __mtc_arbitrary_map_h__
