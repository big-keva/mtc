/*
The MIT License (MIT)

Copyright (c) 2016-2025 Андрей Коваленко aka Keva
  keva@rambler.ru
  @Big_keva
  phone: +7(495)648-4058, +7(926)513-2991, +7(707)129-1418

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
# if !defined( __mtc_radix_tree_hpp__ )
# define __mtc_radix_tree_hpp__
# include "serialize.h"
# include <type_traits>
# include <stdexcept>
# include <functional>
# include <climits>
# include <string>
# include <vector>

namespace mtc {
namespace radix {

  class key
  {
    template <class V, class A>
      friend class tree;
    template <class S>
      friend class dump;

    const uint8_t*  ptr = nullptr;
    size_t          len = 0;

  public:     // construction
    key() = default;
    template <class chartype>
      key( const chartype*, size_t = (size_t)-1 );
    template <class chartype>
      key( const chartype*, const chartype* );
    template <class chartype, class traits, class alloc>
      key( const std::basic_string<chartype, traits, alloc>& s ): key( s.c_str(), s.length() ) {}

  public:     // key access
    auto    data() const -> const unsigned char*    {  return ptr;  }
    auto    size() const -> size_t                  {  return len;  }

    auto    begin() const -> const unsigned char*   {  return data();  }
    auto    end()   const -> const unsigned char*   {  return data() + size();  }

    auto    to_string() const -> std::string {  return { (const char*)data(), size() };  }

  public:     // serialization
    size_t  GetBufLen(      ) const {  return ::GetBufLen( len ) + len;  }
    template <class O>
    O*      Serialize( O* o ) const {  return ::Serialize( ::Serialize( o, len ), ptr, len );  }

  public:     // compare
    int     compare( const key& ) const;

    bool    operator <  ( const key& k ) const  {  return compare( k ) < 0;  }
    bool    operator == ( const key& k ) const  {  return compare( k ) == 0;  }
    bool    operator != ( const key& k ) const  {  return !(*this == k);  }

  };

  template <class T, class A = std::allocator<T>>
  class tree: protected std::vector<tree<T, A>, A>
  {
    template <class X, class Y>
    friend class tree;

    using string_type = std::basic_string<uint8_t, std::char_traits<uint8_t>, A>;
    using vector_type = std::vector<tree<T, A>, A>;

    template <class From, class To>
    using same_const_as = std::conditional<std::is_const<From>::value,
      const typename std::remove_cv<To>::type, typename std::remove_cv<To>::type>;

    template <class From, class To>
    using same_const_as_t = typename same_const_as<From, To>::type;

    template <class From, class To>
    using rebind = typename std::allocator_traits<From>::template rebind_alloc<To>;

    struct stored_object
    {
      template <class M>
      static  auto  create( const M&, void*& place ) -> T*
        {  return (T*)&place;  }
    };

    struct onheap_object
    {
      template <class M>
      static  auto  create( const M& alloc, void*& place ) -> T*
        {  return (T*)(place = rebind<M, T>( alloc ).allocate( 1 ));  }
    };

    template <class V>
    class iterator_base;

  public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = typename std::allocator_traits<A>::template rebind_alloc<char>;
    using value_type = std::pair<const key, T>;

    using iterator = iterator_base<T>;
    using const_iterator = iterator_base<const T>;

  public:     // std::constructors
    tree() = default;
    explicit tree( const A& );
    template <class InputIterator>
    tree( InputIterator, InputIterator, const A& = A() );
    tree( const key&, const T&, const A& = A() );
    tree( const key&, T&&, const A& = A() );
    tree( const key&, const A& = A() );
    tree( const std::initializer_list<value_type>&, const A& = A() );
    tree( const tree& );
    template <class OtherAllocator>
    tree( const tree<T, OtherAllocator>&, const A& );
    tree( tree&& ) noexcept;
   ~tree();
    tree& operator = ( const tree& );
    tree& operator = ( tree&& );
    tree& operator = ( const std::initializer_list<value_type>& );

  public:     // native API
    bool  Delete( const key& key )  {  return remove( key.begin(), key.end() );  }
    auto  Insert( const key&, const T& = T() ) -> T&;
    auto  Insert( const key&, T&& ) -> T&;
    auto  Search( const key& ) const -> const T*;
    auto  Search( const key& ) -> T*;

  protected:
    static  uint8_t  bytecount( unsigned u )
    {
      return (u & 0xffffff00) == 0 ? 1 :
             (u & 0xffff0000) == 0 ? 2 :
             (u & 0xff000000) == 0 ? 3 : 4;
    }

  public:     // serialization
    auto  GetBufLen() const -> size_t;
    template <class O>
    O*    Serialize( O* ) const;
    template <class S>
    S*    FetchFrom( S* );

  public:     // elemets access
    auto  at( const key& ) const -> const T&;
    auto  at( const key& ) -> T&;
    auto  operator []( const key& ) -> T&;

  public:     // std::iterators
    auto  cbegin() const -> const_iterator;
    auto  cend() const -> const_iterator;
    auto  begin() const -> const_iterator;
    auto  end() const -> const_iterator;
    auto  begin() -> iterator;
    auto  end() -> iterator;

  public:     // std::capacity
    bool  empty() const {  return vector_type::empty() && !has_value();  }
    auto  size() const -> size_t  {  return valCount;  }

  protected:
    auto  insert( const uint8_t*, const uint8_t* ) -> tree*;
    auto  insert( const uint8_t*, const uint8_t*, std::vector<tree*>& ) -> tree*;
    bool  remove( const uint8_t*, const uint8_t* );
    template <class S> static
    auto  search( S*, const uint8_t*, const uint8_t* ) -> S*;
    template <class S> static
    auto  search( S*, const uint8_t*, const uint8_t*, std::vector<S*>& ) -> S*;
    template <class S> static
    auto  lbound( S*, const uint8_t*, const uint8_t*, string_type&, std::vector<S*>& ) -> S*;
    template <class S> static
    auto  ubound( S*, const uint8_t*, const uint8_t*, string_type&, std::vector<S*>& ) -> S*;

  public:     // insert operations
    void  clear();
    auto  insert( const value_type& ) -> std::pair<iterator, bool>;
    auto  insert( value_type&& ) -> std::pair<iterator, bool>;
    template <class InputIterator>
    void  insert( InputIterator, InputIterator );
    void  insert( const std::initializer_list<value_type>& );
    auto  erase( iterator pos ) -> iterator;
    auto  erase( const_iterator ) -> iterator;
    auto  erase( iterator first, iterator last ) -> iterator;
    auto  erase( const_iterator first, const_iterator last ) -> iterator;
    auto  erase( const key& ) -> size_type;

  public:
    auto  find( const key& ) const -> const_iterator;
    auto  find( const key& ) -> iterator;
    bool  contains( const key& ) const;
    auto  lower_bound( const key& ) const -> const_iterator;
    auto  lower_bound( const key& ) -> iterator;
    auto  upper_bound( const key& ) const -> const_iterator;
    auto  upper_bound( const key& ) -> iterator;

  protected:
    void  del_value();
    auto  get_value() const -> const T&;
    auto  get_value() -> T&;
    bool  has_value() const {  return (nodeSets & 0x0100) != 0;  }
    auto  set_value( const T& ) -> T&;
    auto  set_value( T&& ) -> T&;
    template <class N> static
    auto  move_down( N*, string_type&, std::vector<N*>& ) -> N*;

  protected:
    string_type fragment;
    void*       valueBuf = nullptr;
    size_t      valCount = 0;
    uint16_t    nodeSets = 0;   // lower 8 bits are the key character, upper mean value

  private:
    using create = typename std::conditional<sizeof(T) <= sizeof(valueBuf),
      stored_object,
      onheap_object>::type;
  };

  template <class T, class A>
  template <class V>
  class tree<T, A>::iterator_base
  {
    friend class tree;

    using tree_t = same_const_as_t<V, tree>;

    string_type           itkey;
    std::vector<tree_t*>  trace;

  public:
    struct iterator_value
    {
      const radix::key  key;
      V&                value;
      const radix::key& first = key;
      V&                second = value;
    };

  protected:
    typename std::aligned_storage<sizeof(iterator_value),
      alignof(iterator_value)>::type value;

  public:
    iterator_base() = default;
    iterator_base( iterator_base&& );
    iterator_base( const iterator_base& );

    auto  operator = ( iterator_base&& it ) -> iterator_base&;
    auto  operator = ( const iterator_base& it ) -> iterator_base&;

  protected:
    iterator_base( tree_t* );
    iterator_base( const key&, std::vector<tree_t*>&& );

  public:
    template <class M>
    bool  operator == ( const iterator_base<M>& ) const;
    bool  operator != ( const iterator_base& it ) const {  return !(*this == it);  }
    auto  operator-> () const -> const iterator_value*;
    auto  operator* () const -> const iterator_value&;
    auto  operator++() -> iterator_base&;
    auto  operator++( int ) -> iterator_base;

  };

  template <class S>
  class dump
  {
    std::function<S*()> get_source;

  public:
    dump( S* s ): get_source( [s](){  return s;  } ) {}
    dump( std::function<S*()> s ): get_source( s ) {}
    dump( const dump& s ): get_source( s.get_source ) {}
    dump& operator = ( const dump& s )  {  return get_source = s.get_source, *this;  }

  public:
    auto  Search( const key& key ) const -> S*
      {  return search( get_source(), key.begin(), key.end() );  }

  protected:
    template <class U>
    static  auto  getint( S*, U&, size_t ) -> S*;
    static  auto  search( S*, const uint8_t*, const uint8_t* ) -> S*;

  };

  template <>
  class dump<const char>
  {
    const char*  root;

  public:
    dump( const char* s = nullptr ): root( s ) {}
    dump( std::function<const char*()> s ): root( s() ) {}
    dump( const dump& s ): root( s.root ) {}
    dump& operator = ( const dump& s )  {  return root = s.root, *this;  }

  public:
    auto  Search( const key& key ) const -> const char*
      {  return search( root, key.begin(), key.end() );  }

  protected:
    template <class U>
    static  auto  get_1b( const char*, U& ) -> const char*;
    template <class U>
    static  auto  get_2b( const char*, U& ) -> const char*;
    template <class U>
    static  auto  get_3b( const char*, U& ) -> const char*;
    template <class U>
    static  auto  get_4b( const char*, U& ) -> const char*;
    template <class U>
    static  auto  getint( const char*, U&, size_t ) -> const char*;
    static  auto  search( const char*, const uint8_t*, const uint8_t* ) -> const char*;

  };

  // key implementation

  template <class chartype>
  key::key( const chartype* s, size_t l ): ptr( (const unsigned char*)s )
  {
    if ( (len = l) == (size_t)-1 && ptr != nullptr )
      for ( len = 0; ptr[len] != 0; ++len ) (void)NULL;
    len *= sizeof(chartype);
  }

  template <class chartype>
  key::key( const chartype* beg, const chartype* end ):
    ptr( (const unsigned char*)beg ), len( sizeof(chartype) * (end - beg) )  {}

  inline int key::compare( const key& k ) const
  {
    auto  l1 = size();
    auto  l2 = k.size();
    auto  lc = std::min( l1, l2 );
    auto  rc = lc != 0 ? memcmp( data(), k.data(), lc ) : 0;

    return rc != 0 ? rc : l1 - l2;
  }

  template <class T, class A>
  auto  tree<T, A>::insert( const uint8_t* addkey, const uint8_t* addend ) -> tree*
  {
    for ( auto  objptr = this; ; )
    {
      auto  objkey = objptr->fragment.data();
      auto  objend = objptr->fragment.size() + objkey;
      int   rescmp = 0;

    // check partial match
      while ( objkey != objend && addkey != addend && (rescmp = *addkey - *objkey) == 0 )
        ++objkey, ++addkey;

    // check if node key is completely covered;
    // check if found the node or need lookup the branches
      if ( objkey == objend )
      {
        if ( addkey != addend )
        {
          auto  it_beg = ((vector_type*)objptr)->begin();
          auto  it_end = ((vector_type*)objptr)->end();
          auto  it_ptr = it_beg;

          while ( it_ptr != it_end && it_ptr->fragment.front() < *addkey )
            ++it_ptr;

          if ( it_ptr == it_end || it_ptr->fragment.front() != *addkey )
          {
            auto  offset = it_ptr - it_beg;

            ((vector_type*)objptr)->emplace( it_ptr, key{ addkey, addend }, vector_type::get_allocator() );
              ++valCount;
            return &(*(vector_type*)objptr)[offset];
          }

        // jump to the next level of insertion
          objptr = &(*(vector_type*)objptr)[it_ptr - it_beg];
          continue;
        }
        if ( !objptr->has_value() )
          ++valCount;
        return objptr;
      }
        else
    // no, fragment is not completely matching the passed key; so, the key
    // has to be splitted to matching and noon-matching parts:
    // * the matching part becomes the root fragment;
    // * the rest of fragment becomes the first branch;
    // * the rest of key becomes the second branch.
      {
        auto  subvec = std::move( (vector_type&&)*objptr );

        if ( rescmp < 0 )
        {
          objptr->emplace_back( key{ addkey, addend }, vector_type::get_allocator() );
          objptr->emplace_back( key{ objkey, objend }, vector_type::get_allocator() );
            ((vector_type&)objptr->back()) = std::move( subvec );

          if ( objptr->has_value() )
          {
            objptr->back().set_value( std::move( objptr->get_value() ) );
              objptr->del_value();
          }
          objptr->fragment.resize( objkey - objptr->fragment.data() );
            ++valCount;
          return &(*(vector_type*)objptr)[0];
        }
          else
        {
          objptr->emplace_back( key{ objkey, objend }, vector_type::get_allocator() );
            ((vector_type&)objptr->back()) = std::move( subvec );

          if ( objptr->has_value() )
          {
            objptr->front().set_value( std::move( objptr->get_value() ) );
              objptr->del_value();
          }
          objptr->fragment.resize( objkey - objptr->fragment.data() );

          if ( rescmp > 0 )
          {
            objptr->emplace_back( key{ addkey, addend }, vector_type::get_allocator() );
              ++valCount;
            return &(*(vector_type*)objptr)[1];
          }
            else
          {
            if ( !objptr->has_value() )
              ++valCount;
            return objptr;
          }
        }
      }
    }
  }

  template <class T, class A>
  auto  tree<T, A>::insert( const uint8_t* addkey, const uint8_t* addend, std::vector<tree*>& vec ) -> tree*
  {
    for ( auto  objptr = this; ; )
    {
      auto  objkey = objptr->fragment.data();
      auto  objend = objptr->fragment.size() + objkey;
      int   rescmp = 0;

      vec.push_back( objptr );

    // check partial match
      while ( objkey != objend && addkey != addend && (rescmp = *addkey - *objkey) == 0 )
        ++objkey, ++addkey;

    // check if node key is completely covered;
    // check if found the node or need lookup the branches
      if ( objkey == objend )
      {
        if ( addkey != addend )
        {
          auto  it_beg = ((vector_type*)objptr)->begin();
          auto  it_end = ((vector_type*)objptr)->end();
          auto  it_ptr = it_beg;

          while ( it_ptr != it_end && it_ptr->fragment.front() < *addkey )
            ++it_ptr;

          if ( it_ptr == it_end || it_ptr->fragment.front() != *addkey )
          {
            auto  offset = it_ptr - it_beg;

            ((vector_type*)objptr)->emplace( it_ptr, addkey, addend, vector_type::get_allocator() );
              ++valCount;
            vec.push_back( &(*(vector_type*)objptr)[offset] );
              return vec.back();
          }

        // jump to the next level of insertion
          objptr = &(*(vector_type*)objptr)[it_ptr - it_beg];
          continue;
        }
        if ( !objptr->has_value() )
          ++valCount;
        return objptr;
      }
        else
    // no, fragment is not completely matching the passed key; so, the key
    // has to be splitted to matching and noon-matching parts:
    // * the matching part becomes the root fragment;
    // * the rest of fragment becomes the first branch;
    // * the rest of key becomes the second branch.
      {
        auto  subvec = std::move( (vector_type&&)*objptr );

        if ( rescmp < 0 )
        {
          objptr->emplace_back( addkey, addend, vector_type::get_allocator() );
          objptr->emplace_back( objkey, objend, vector_type::get_allocator() );
            ((vector_type&)objptr->back()) = std::move( subvec );

          if ( has_value() )
          {
            objptr->back().set_value( std::move( objptr->get_value() ) );
              objptr->del_value();
          }
          objptr->fragment.resize( objkey - objptr->fragment.data() );
            ++valCount;
          vec.push_back( &(*(vector_type*)objptr)[0] );
            return vec.back();
        }
          else
        {
          objptr->emplace_back( objkey, objend, vector_type::get_allocator() );
            ((vector_type&)objptr->back()) = std::move( subvec );

          if ( has_value() )
          {
            objptr->front().set_value( std::move( objptr->get_value() ) );
              objptr->del_value();
          }
          objptr->fragment.resize( objkey - objptr->fragment.data() );

          if ( rescmp > 0 )
          {
            objptr->emplace_back( addkey, addend, vector_type::get_allocator() );
              vec.push_back( &(*(vector_type*)objptr)[1] );
            return vec.back();
          }
            else
          {
            if ( !objptr->has_value() )
              ++valCount;
            return objptr;
          }
        }
      }
    }
  }

  template <class T, class A>
  bool  tree<T, A>::remove( const uint8_t* key, const uint8_t* end )
  {
    auto  my_top = fragment.data();
    auto  my_end = fragment.size() + my_top;
    int   rescmp = 0;

    // check key match
    while ( my_top != my_end && key != end && (rescmp = *key - *my_top) == 0 )
      ++my_top, ++key;

    // check fragment match to key
    if ( my_top != my_end )
      return false;

    // if key found in the tree, check if key is longer than the matched fragment,
    // // check the branches and forward the rest key to branch
    if ( key != end )
    {
      auto  it_beg = vector_type::begin();
      auto  it_end = vector_type::end();

      while ( it_beg != it_end && it_beg->fragment.front() < *key )
        ++it_beg;

      if ( it_beg != it_end && it_beg->fragment.front() == *key && it_beg->remove( key, end ) )
      {
        // if the rest not has no branches, remove it
        if ( !it_beg->has_value() && ((vector_type&)*it_beg).empty() )
          vector_type::erase( it_beg );
        if ( valCount > 0 )
          --valCount;
        return true;
      }
      return false;
    }

    // key matches completely, check if value is assigned and delete it
    if ( has_value() )
    {
      del_value();

      // if the node with deleted key has only one branch, merge the key
      if ( vector_type::size() == 1 )
      {
        auto  subset = std::move( (vector_type&)vector_type::front() );

        fragment += vector_type::front().fragment;

        if ( vector_type::front().has_value() )
          set_value( std::move( vector_type::front().get_value() ) );

        vector_type::operator=( std::move( subset ) );
      }
      if ( valCount > 0 )
        --valCount;
      return true;
    }
    return false;
  }

  template <class T, class A>
  template <class S>
  auto  tree<T, A>::search( S* ptr, const uint8_t* key, const uint8_t* end ) -> S*
  {
    while ( ptr != nullptr )
    {
      auto  ptrtop = ptr->fragment.data();
      auto  ptrend = ptr->fragment.size() + ptrtop;

      // check partial match
      while ( ptrtop != ptrend && key != end && *key == *ptrtop )
        ++ptrtop, ++key;

      // check if node key is completely covered
      if ( ptrtop != ptrend )
        return nullptr;

      // check if key is found
      if ( key != end )
      {
        auto  it_beg = ((vector_type*)ptr)->data();
        auto  it_end = ((vector_type*)ptr)->size() + it_beg;

        while ( it_beg != it_end && it_beg->fragment.front() < *key )
          ++it_beg;
        ptr = it_beg != it_end && it_beg->fragment.front() == *key ? it_beg : nullptr;
      }
        else
      break;
    }
    return ptr;
  }

  template <class T, class A>
  template <class S>
  auto  tree<T, A>::search( S* ptr,
    const uint8_t*    key,
    const uint8_t*    end,
    std::vector<S*>&  vec ) -> S*
  {
    while ( ptr != nullptr )
    {
      auto  ptrtop = ptr->fragment.data();
      auto  ptrend = ptr->fragment.size() + ptrtop;

      // check partial match
      while ( ptrtop != ptrend && key != end && *key == *ptrtop )
        ++ptrtop, ++key;

      // check if node key is completely covered
      if ( ptrtop != ptrend )
        return nullptr;

      vec.push_back( ptr );

      // check if key is found
      if ( key != end )
      {
        auto  it_beg = ((vector_type*)ptr)->data();
        auto  it_end = ((vector_type*)ptr)->size() + it_beg;

        while ( it_beg != it_end && it_beg->fragment.front() < *key )
          ++it_beg;
        ptr = it_beg != it_end && it_beg->fragment.front() == *key ? it_beg : nullptr;
      }
        else
      break;
    }
    return ptr;
  }

  template <class T, class A>
  template <class S>
  auto  tree<T, A>::lbound( S* ptr,
    const uint8_t*    key,
    const uint8_t*    end,
    string_type&      str,
    std::vector<S*>&  vec ) -> S*
  {
    auto  ptrtop = ptr->fragment.data();
    auto  ptrend = ptr->fragment.size() + ptrtop;

  // check partial match
    while ( ptrtop != ptrend && key != end && *key == *ptrtop )
      ++ptrtop, ++key;

  // if node key is not completely covered, it is lower bound, or it's
  // first descendant is;
  // else lookup nested branches looking for the first one non-less
  // than the key searched for
    if ( ptrtop == ptrend && key != end )
    {
      auto  it_beg = ((vector_type*)ptr)->data();
      auto  it_end = ((vector_type*)ptr)->size() + it_beg;

      while ( it_beg != it_end && it_beg->fragment.front() < *key )
        ++it_beg;

    // - if non-less char not found, return nullptr hoping the level upper will find it;
      if ( it_beg == it_end )
        return nullptr;

      vec.push_back( ptr );
      str += ptr->fragment;

    // - if upper char is found, add this node and return subbranch key;
    // - if equal char is found, add current node and try go deeper level;
    // if not found, remove added key and ptr and go next
      if ( it_beg->fragment.front() == *key )
      {
        if ( (ptr = lbound( it_beg, key, end, str, vec )) != nullptr )
          return ptr;

        if ( ++it_beg == it_end )
        {
          str.resize( str.length() - vec.back()->fragment.length() );
          vec.pop_back();
          return nullptr;
        }
      }
      return move_down( it_beg, str, vec );
    }
    return move_down( ptr, str, vec );
  }

  template <class T, class A>
  void tree<T, A>::del_value()
  {
    if ( has_value() )
    {
      get_value().~T();

      if ( sizeof(T) > sizeof(valueBuf) )
        rebind<A, T>( vector_type::get_allocator() ).deallocate( (T*)valueBuf, 0 );
      nodeSets &= ~0x0100;
    }
  }

  template <class T, class A>
  auto tree<T, A>::get_value() const -> const T&
  {
    if ( has_value() )     // check if valie is initialized
      return sizeof(T) <= sizeof(valueBuf) ? *(const T*)&valueBuf : *(const T*)valueBuf;
    throw std::logic_error( "uninitialized value access" );
  }

  template <class T, class A>
  auto  tree<T, A>::get_value() -> T&
  {
    if ( has_value() )     // check if value is initialized
      return sizeof(T) <= sizeof(valueBuf) ? *(T*)&valueBuf : *(T*)valueBuf;
    return *(new( create::create( vector_type::get_allocator(), valueBuf ) ) T());
  }

  template <class T, class A>
  auto  tree<T, A>::set_value( const T& t ) -> T&
  {
    if ( has_value() )
      return get_value() = t;

    nodeSets |= 0x0100;

    return *(new( create::create( vector_type::get_allocator(), valueBuf ) ) T( t ));
  }

  template <class T, class A>
  auto  tree<T, A>::set_value( T&& t ) -> T&
  {
    if ( has_value() )
      return get_value() = std::move( t );

    nodeSets |= 0x0100;

    return *(new( create::create( vector_type::get_allocator(), valueBuf ) ) T( std::move( t ) ));
  }

  template <class T, class A>
  template <class N>
  auto  tree<T, A>::move_down( N* n, string_type& s, std::vector<N*>& v ) -> N*
  {
    for ( ; ; )
    {
      v.push_back( n );
      s += n->fragment;

      if ( !n->has_value() ) n = &((vector_type*)n)->front();
        else break;
    }

    return n;
  }

// tree::iterator_base implementation

  template <class T, class A>
  template <class V>
  tree<T, A>::iterator_base<V>::iterator_base( iterator_base&& it ):
    itkey( std::move( it.itkey ) ),
    trace( std::move( it.trace ) )
  {
    if ( !trace.empty() && trace.back()->has_value() )
      new( &value ) iterator_value{ itkey, trace.back()->get_value(), itkey, trace.back()->get_value() };
  }

  template <class T, class A>
  template <class V>
  tree<T, A>::iterator_base<V>::iterator_base( const iterator_base& it ):
    itkey( it.itkey ),
    trace( it.trace )
  {
    if ( !trace.empty() && trace.back()->has_value() )
      new( &value ) iterator_value{ itkey, trace.back()->get_value() };
  }

  template <class T, class A>
  template <class V>
  auto  tree<T, A>::iterator_base<V>::operator=( iterator_base&& it ) -> iterator_base&
  {
    itkey = std::move( it.itkey );
    trace = std::move( it.trace );

    if ( !trace.empty() && trace.back()->has_value() )
      new( &value ) iterator_value{ itkey, trace.back()->get_value() };

    return *this;
  }

  template <class T, class A>
  template <class V>
  auto  tree<T, A>::iterator_base<V>::operator=( const iterator_base& it ) -> iterator_base&
  {
    itkey = it.itkey;
    trace = it.trace;

    if ( !trace.empty() && trace.back()->has_value() )
      new( &value ) iterator_value{ itkey, trace.back()->get_value() };
    return *this;
  }

  template <class T, class A>
  template <class V>
  tree<T, A>::iterator_base<V>::iterator_base( tree_t* pn )
  {
    if ( pn != nullptr )
    {
      if ( (pn = move_down( pn, itkey, trace )) != nullptr )
        new( &value ) iterator_value{ itkey, trace.back()->get_value() };
      else throw std::logic_error( "uninitialized iterator" );
    }
  }

  template <class T, class A>
  template <class V>
  tree<T, A>::iterator_base<V>::iterator_base( const key& str, std::vector<tree_t*>&& vec ):
    itkey( str.data(), str.size() ),
    trace( std::move( vec ) )
  {
    if ( !trace.empty() && trace.back()->has_value() )
      new( &value ) iterator_value{ itkey, trace.back()->get_value() };
    else throw std::logic_error( "uninitialized iterator" );
  }

  template <class T, class A>
  template <class V>
  template <class M>
  bool  tree<T, A>::iterator_base<V>::operator == ( const iterator_base<M>& it ) const
  {
    if ( trace.size() == it.trace.size() )
    {
      auto  i1 = trace.begin();
      auto  i2 = it.trace.begin();

      for ( auto is = trace.end(); i1 != is; ++i1, ++i2 )
        if ( *i1 != *i2 ) return false;
      return true;
    }
    return false;
  }

  template <class T, class A>
  template <class V>
  auto  tree<T, A>::iterator_base<V>::operator->() const -> const iterator_value*
  {
    if ( !trace.empty() && trace.back()->has_value() )
      return (const iterator_value*)&value;
    throw std::range_error( "iterator_base<V>::operator->() out of range" );
  }

  template <class T, class A>
  template <class V>
  auto  tree<T, A>::iterator_base<V>::operator*() const -> const iterator_value&
  {
    if ( !trace.empty() && trace.back()->has_value() )
      return *(const iterator_value*)&value;
    throw std::range_error( "iterator_base<V>::operator->() out of range" );
  }

  template <class T, class A>
  template <class V>
  auto  tree<T, A>::iterator_base<V>::operator++() -> iterator_base&
  {
    if ( trace.empty() )
      throw std::range_error( "iterator_base<V>::operator++() out of range" );

  // if the iterator is on some node, check if the node has subbranches; go down
  // until the first key is meeted
    if ( !((vector_type*)trace.back())->empty() )
      for ( ; ; )
      {
        trace.push_back( &((vector_type*)trace.back())->front() );
          itkey += trace.back()->fragment;
        if ( trace.back()->has_value() )
          return new( &value ) iterator_value{ itkey, trace.back()->get_value() }, *this;
      }

    assert( !trace.empty() );

  // if on ending leaf, go up until any more branches
    while ( trace.size() > 1 )
    {
      auto  ppnode = (tree_t*)&((vector_type*)trace[trace.size() - 2])->front();
      auto  pplast = (tree_t*)&((vector_type*)trace[trace.size() - 2])->back() + 1;

    // search node in parent list
      while ( ppnode != pplast && ppnode != trace.back() )
        ++ppnode;

      // ensure the node is in the
      if ( ppnode == pplast )
        throw std::range_error( "iterator_base<V>::operator++() out of range" );

    // delete the key part of current node
      itkey.resize( itkey.length() - trace.back()->fragment.length() );
        trace.pop_back();

    // go to parent next node
      if ( ++ppnode != pplast )
      {
        auto  target = move_down( ppnode, itkey, trace );

        if ( target != nullptr ) new( &value ) iterator_value{ itkey, target->get_value() };
          else throw std::logic_error( "broken iterator_base<V> data" );

        return *this;
      }
    }

    trace.clear();
    itkey.clear();

    return *this;
  }

  template <class T, class A>
  template <class V>
  auto  tree<T, A>::iterator_base<V>::operator++( int ) -> iterator_base
  {
    auto  self( *this );
      operator++();
    return self;
  }

  // tree implementation

  template <class T, class A>
  tree<T, A>::tree( const key& k, const T& val, const A& mem ):
    vector_type( mem ), fragment( k.begin(), k.end(), mem ), nodeSets( 0x0100 )
  {
    new( create::create( mem, valueBuf ) ) T( val );
  }

  template <class T, class A>
  tree<T, A>::tree( const key& k, T&& val, const A& mem ):
    vector_type( mem ), fragment( k.begin(), k.end(), mem ), nodeSets( 0x0100 )
  {
    new( create::create( mem, valueBuf ) ) T( std::move( val ) );
  }

  template <class T, class A>
  tree<T, A>::tree( const key& k, const A& mem ):
    vector_type( mem ), fragment( k.begin(), k.end(), mem ) {}

  template <class T, class A>
  tree<T, A>::tree( const A& mem ):
    vector_type( mem ), fragment( mem ) {}

  template <class T, class A>
  template <class InputIterator>
  tree<T, A>::tree( InputIterator beg, InputIterator end, const A& mem ):
    vector_type( mem ), fragment( mem )
  {
    for ( ; beg != end; ++beg )
      Insert( beg->first, beg->second );
  }

  template <class T, class A>
  tree<T, A>::tree( const std::initializer_list<value_type>& list, const A& mem ):
    tree( list.begin(), list.end(), mem )  {}

  template <class T, class A>
  template <class OtherAllocator>
  tree<T, A>::tree( const tree<T, OtherAllocator>& t, const A& mem ):
    vector_type( mem ), fragment( mem )
  {
    if ( t.has_value() )
      set_value( t.get_value() );
    for ( auto& next: t )
      Insert( next.key, next.value );
  }

  template <class T, class A>
  tree<T, A>::tree( const tree& t ):
    vector_type( t.get_allocator() ), fragment( t.get_allocator() ), valCount( t.valCount )
  {
    if ( t.has_value() )
      set_value( t.get_value() );
  }

  template <class T, class A>
  tree<T, A>::tree( tree&& t ) noexcept:
    vector_type( std::move( t ) ), fragment( std::move( t.fragment ) ), valCount( t.valCount ),
      nodeSets( t.nodeSets & ~0x0100 )
  {
    if ( t.has_value() )
      set_value( std::move( t.get_value() ) );
    t.del_value();
    t.valCount = 0;
  }

  template <class T, class A>
  tree<T, A>::~tree()
  {
    del_value();
  }

  template <class T, class A>
  auto  tree<T, A>::operator=( const tree& t ) -> tree&
  {
    if ( t.has_value() )  set_value( t.get_value() );
      else del_value();

    vector_type::
      operator=( t );
    fragment.
      operator=( t.fragment );
    valCount = t.valCount;

    return *this;
  }

  template <class T, class A>
  auto  tree<T, A>::operator=( tree&& t ) -> tree&
  {
    if ( t.has_value() )  set_value( std::move( t.get_value() ) );
      else del_value();
    vector_type::
      operator=( std::move( t ) );
    fragment.
      operator=( std::move( t.fragment ) );
    valCount = t.valCount;
      t.valCount = 0;
    t.del_value();
      return *this;
  }

  template <class T, class A>
  auto  tree<T, A>::operator=( const std::initializer_list<value_type>& t ) -> tree&
  {
    clear();
    for ( auto& next: t )
      Insert( next.first, next.second );
    return *this;
  }

  template <class T, class A>
  auto  tree<T, A>::Insert( const key& key, const T& value ) -> T&
  {
    return insert( key.begin(), key.end() )->set_value( value );
  }

  template <class T, class A>
  auto  tree<T, A>::Insert( const key& key, T&& value ) -> T&
  {
    return insert( key.begin(), key.end() )->set_value( std::move( value ) );
  }

  template <class T, class A>
  auto  tree<T, A>::Search( const key& key ) const -> const T*
  {
    auto  pfound = search( this, key.begin(), key.end() );

    return pfound != nullptr && pfound->has_value() ? &pfound->get_value() : nullptr;
  }

  template <class T, class A>
  auto  tree<T, A>::Search( const key& key ) -> T*
  {
    auto  pfound = search( this, key.begin(), key.end() );

    return pfound != nullptr && pfound->has_value() ? &pfound->get_value() : nullptr;
  }

  template <class T, class A>
  auto  tree<T, A>::GetBufLen() const -> size_t
  {
    size_t    length = has_value() ? ::GetBufLen( get_value() ) : 0;
    unsigned  ofprev = length;
    uint8_t   offlen = 0;

    // get nested nodes lengths and offsets; calc node shifts
    for ( auto& next: *(vector_type*)this )
    {
      auto  it_len = next.GetBufLen();
      offlen = std::max( offlen, uint8_t(bytecount( ofprev ) - 1) );
      ofprev += it_len;
      length += it_len;
    }

    length += ::GetBufLen( fragment ) + 2 + (offlen + 2) * vector_type::size();

    return length;
  }

  template <class T, class A>
  template <class O>
  O*  tree<T, A>::Serialize( O* o ) const
  {
    unsigned  offset[256];
    unsigned* offptr = offset;
    uint8_t   offlen = 0;
    unsigned  ofprev;

    // get value flags
    if ( has_value() )  ofprev = ::GetBufLen( get_value() );
      else ofprev = 0;

    // get nested nodes lengths and offsets; calc node shifts
    for ( auto& next: *(vector_type*)this )
    {
      offlen = std::max( offlen, uint8_t(bytecount( *offptr++ = ofprev ) - 1) );
      ofprev += next.GetBufLen();
    }

    if ( has_value() )
      offlen |= 0x40;

  // store root element
    if ( (o = ::Serialize( ::Serialize( ::Serialize( o, fragment ), uint8_t(vector_type::size()) ), offlen )) == nullptr )
      return o;

    // store key nodes
    for ( auto& next: *(vector_type*)this )
      o = ::Serialize( o, next.fragment.front() );

  // store relocation table
    for ( size_t i = 0; i < vector_type::size(); ++i )
      for ( auto uvalue = offset[i], u = unsigned(0); u <= (offlen & 0x03); ++u, uvalue >>= 8 )
        o = ::Serialize( o, uint8_t(uvalue) );

  // store value
    if ( (offlen & 0x40) != 0 )
      o = ::Serialize( o, get_value() );

    // store the nodes
    for ( auto& next: *(vector_type*)this )
      o = next.Serialize( o );

    return o;
  }

  template <class T, class A>
  auto  tree<T, A>::at( const key& key ) const -> const T&
  {
    auto  pfound = search( this, key.begin(), key.end() );

    if ( pfound != nullptr )  return pfound->get_value();
      throw std::out_of_range( "key not found" );
  }

  template <class T, class A>
  auto  tree<T, A>::at( const key& key ) -> T&
  {
    auto  pfound = search( this, key.begin(), key.end() );

    if ( pfound != nullptr )  return pfound->get_value();
      throw std::out_of_range( "key not found" );
  }

  template <class T, class A>
  auto tree<T, A>::operator[]( const key& key ) -> T&
  {
    auto  pfound = search( this, key.begin(), key.end() );

    return pfound != nullptr ? pfound->get_value() :
      insert( key.begin(), key.end() )->set_value( T() );
  }

  template <class T, class A>
  auto tree<T, A>::cbegin() const -> const_iterator {  return const_iterator( this );  }

  template <class T, class A>
  auto tree<T, A>::cend() const -> const_iterator {  return const_iterator();  }

  template <class T, class A>
  auto tree<T, A>::begin() const -> const_iterator  {  return const_iterator( this );  }

  template <class T, class A>
  auto tree<T, A>::end() const -> const_iterator {  return const_iterator();  }

  template <class T, class A>
  auto tree<T, A>::begin() -> iterator  {  return iterator( this );  }

  template <class T, class A>
  auto tree<T, A>::end() -> iterator {  return iterator();  }

  template <class T, class A>
  void tree<T, A>::clear()
  {
    valCount = 0;
    ((vector_type&)*this).clear();
    del_value();
  }

  template <class T, class A>
  auto  tree<T, A>::insert( const value_type& ins ) -> std::pair<iterator, bool>
  {
    std::vector<tree*>  vec;
    auto                ptr = insert( ins.first.begin(), ins.first.end(), vec );

    if ( ptr->has_value() )
      return { iterator( ins.first, std::move( vec ) ), false };
    ptr->set_value( ins.second );
      return { iterator( ins.first, std::move( vec ) ), true };
  }

  template <class T, class A>
  auto  tree<T, A>::insert( value_type&& mv ) -> std::pair<iterator, bool>
  {
    std::vector<tree*>  vec;
    auto                ptr = insert( mv.first.begin(), mv.first.end(), vec );

    if ( ptr->has_value() )
      return { iterator( mv.first, std::move( vec ) ), false };
    ptr->set_value( std::move( mv.second ) );
      return { iterator( mv.first, std::move( vec ) ), true };
  }

  template <class T, class A>
  template <class InputIterator>
  void  tree<T, A>::insert( InputIterator beg, InputIterator end )
  {
    for ( ; beg != end; ++beg )
      Insert( beg->first, beg->second );
  }

  template <class T, class A>
  void  tree<T, A>::insert( const std::initializer_list<value_type>& list )  {  insert( list.begin(), list.end() );  }

  template <class T, class A>
  auto  tree<T, A>::erase( iterator pos ) -> iterator
  {
    auto  key = pos->key.data();
    auto  end = pos->key.data() + pos->key.size();

    return remove( key, end ), lower_bound( { key, size_t(end - key) } );
  }

  template <class T, class A>
  auto  tree<T, A>::erase( const_iterator pos ) -> iterator
  {
    auto  key = pos->key.data();
    auto  end = pos->key.data() + pos->key.size();

    return remove( key, end ), lower_bound( { key, end - key } );
  }

  template <class T, class A>
  auto  tree<T, A>::erase( iterator first, iterator last ) -> iterator
  {
    auto  key = last->key;

    while ( first->key != key )
      first = erase( first );
    return lower_bound( key );
  }

  template <class T, class A>
  auto  tree<T, A>::erase( const_iterator first, const_iterator last ) -> iterator
  {
    auto  key = last->key;

    while ( first->key != key )
      first = erase( first );
    return lower_bound( key );
  }

  template <class T, class A>
  auto  tree<T, A>::erase( const key& key ) -> size_type
  {
    return remove( key.data(), key.data() + key.size() ) ? 1 : 0;
  }

  template <class T, class A>
  auto  tree<T, A>::find( const key& key ) const -> const_iterator
  {
    auto  atrace = std::vector<const tree*>();
    auto  pfound = search( this, key.begin(), key.end(), atrace );

    return pfound != nullptr && pfound->has_value() ?
      const_iterator( key, std::move( atrace ) ) : const_iterator();
  }

  template <class T, class A>
  auto  tree<T, A>::find( const key& key ) -> iterator
  {
    auto  atrace = std::vector<tree*>();
    auto  pfound = search( this, key.begin(), key.end(), atrace );

    return pfound != nullptr && pfound->has_value() ?
      iterator( key, std::move( atrace ) ) : iterator();
  }

  template <class T, class A>
  bool  tree<T, A>::contains( const key& key ) const
  {
    auto  pfound = search( this, key.begin(), key.end() );

    return pfound != nullptr && pfound->has_value();
  }

  template <class T, class A>
  auto tree<T, A>::lower_bound( const key& key ) const -> const_iterator
  {
    auto  keystr = string_type();
    auto  atrace = std::vector<const tree*>();
    auto  pfound = lbound( this, key.begin(), key.end(), keystr, atrace );

    return pfound != nullptr && pfound->has_value() ?
      const_iterator( std::move( keystr ), std::move( atrace ) ) : const_iterator();
  }

  template <class T, class A>
  auto tree<T, A>::lower_bound( const key& key ) -> iterator
  {
    auto  keystr = string_type();
    auto  atrace = std::vector<tree*>();
    auto  pfound = lbound( this, key.begin(), key.end(), keystr, atrace );

    return pfound != nullptr && pfound->has_value() ?
      iterator( std::move( keystr ), std::move( atrace ) ) : iterator();
  }
/*
  template <class T, class A>
  auto tree<T, A>::upper_bound( const key& key ) const -> const_iterator
  {
    auto  keystr = string_type();
    auto  atrace = std::vector<const tree*>();
    auto  pfound = ubound( this, key.begin(), key.end(), keystr, atrace );

    return pfound != nullptr && pfound->has_value() ?
      const_iterator( std::move( keystr ), std::move( atrace ) ) : const_iterator();
  }

  template <class T, class A>
  auto tree<T, A>::upper_bound( const key& key ) -> iterator
  {
    auto  keystr = string_type();
    auto  atrace = std::vector<tree*>();
    auto  pfound = ubound( this, key.begin(), key.end(), keystr, atrace );

    return pfound != nullptr && pfound->has_value() ?
      iterator( std::move( keystr ), std::move( atrace ) ) : iterator();
  }
*/
  // dump template implementation

  template <class S>
  template <class U>
  auto dump<S>::getint( S* s, U& u, size_t l ) -> S*
  {
    auto  ushift = 0;

    for ( u = 0; l-- > 0; ushift += CHAR_BIT )
    {
      uint8_t bvalue;

      if ( (s = ::FetchFrom( s, bvalue )) == nullptr )
        return nullptr;
      u |= U(bvalue) << ushift;
    }
    return s;
  }

  template <class S>
  auto dump<S>::search( S* s, const uint8_t* keyptr, const uint8_t* keyend ) -> S*
  {
    uint8_t  mirror[0x100];

    while ( s != nullptr )
    {
      size_t    cchkey;   // level key length
      uint8_t   bcount;   // branch count
      uint8_t   bflags;   // level flags
      uint8_t*  pmatch;
      size_t    offset;
      size_t    vallen;

    // get level key length
      if ( (s = ::FetchFrom( s, cchkey )) == nullptr )
        return nullptr;

    // check key match
      while ( cchkey != 0 )
      {
        auto  cbpart = std::min( cchkey, std::min( size_t(keyend - keyptr), sizeof(mirror) ) );

        if ( (s = ::FetchFrom( s, mirror, cbpart )) == nullptr )
          return nullptr;

        if ( memcmp( keyptr, mirror, cbpart ) != 0 )
          return nullptr;

        keyptr += cbpart;
        cchkey -= cbpart;
      }

    // check if matches
      if ( cchkey != 0 )
        return nullptr;

    // get branch count and level flags
      if ( (s = ::FetchFrom( ::FetchFrom( s, bcount ), bflags )) == nullptr )
        return nullptr;

    // check if the key is completely scanned
      if ( keyptr == keyend )
        return (bflags & 0xfc) == 0xfc ? ::FetchFrom( s, vallen ) : s;

    // check stored value
      if ( (bflags & 0xfc) != 0 )
      {
        if ( (bflags & 0xfc) != 0xfc )  s = ::SkipBytes( s, bflags >> 2 );
          else
        {
          if ( (s = ::FetchFrom( s, vallen )) == nullptr )
            return nullptr;
          s = ::SkipBytes( s, vallen );
        }
      }

    // get level characters
      if ( (s = ::FetchFrom( s, mirror, bcount )) == nullptr )
        return nullptr;

    // find matching branch
      for ( pmatch = mirror; pmatch != mirror + bcount && *pmatch < *keyptr; ++pmatch )
        (void)NULL;

      if ( pmatch == mirror + bcount || *keyptr != *pmatch )
        return nullptr;

    // go to the next branch
      if ( (s = ::SkipBytes( s, (1 + (bflags & 0x3)) * (pmatch - mirror) )) == nullptr )
        return nullptr;

    // get current offset
      if ( (s = getint( s, offset, 1 + (bflags & 0x3) )) == nullptr )
        return nullptr;

    // skip the rest of relocations
      if ( (s = ::SkipBytes( s, offset + (1 + (bflags & 0x3)) * (bcount - (pmatch - mirror) - 1) )) == nullptr )
        return nullptr;
    }
    return s;
  }

  // dump<const char> template specification

  template <class U>
  auto  dump<const char>::get_1b( const char* s, U& u ) -> const char*
  {
    u = U(uint8_t(*s++));
    return s;
  }

  template <class U>
  auto  dump<const char>::get_2b( const char* s, U& u ) -> const char*
  {
    u = U(uint8_t(s[0])) | U(uint8_t(s[1]) << 8);
    return s;
  }

  template <class U>
  auto  dump<const char>::get_3b( const char* s, U& u ) -> const char*
  {
    u = U(uint8_t(s[0])) | U(uint8_t(s[1])) << 8 | U(uint8_t(s[2])) << 16;
    return s;
  }

  template <class U>
  auto  dump<const char>::get_4b( const char* s, U& u ) -> const char*
  {
    u = U(uint8_t(s[0])) | U(uint8_t(s[1])) << 8 | U(uint8_t(s[2])) << 16 | U(uint8_t(s[3])) << 24;
    return s;
  }

  template <class U>
  auto  dump<const char>::getint( const char* s, U& u, size_t l ) -> const char*
  {
    switch ( l )
    {
      case 1: return get_1b( s, u );
      case 2: return get_2b( s, u );
      case 3: return get_3b( s, u );
      case 4: return get_4b( s, u );
      default:  return nullptr;
    }
  }

  inline
  auto  dump<const char>::search( const char* s, const uint8_t* keyptr, const uint8_t* keyend ) -> const char*
  {
    while ( s != nullptr )
    {
      size_t      cchkey;   // level key length
      uint8_t     bcount;   // branch count
      uint8_t     bflags;   // level flags
      const char* pmatch;
      size_t      offset;
      size_t      b2offs;

    // get level key length
      if ( (s = ::FetchFrom( s, cchkey )) == nullptr )
        return nullptr;

    // check key match
      if ( keyptr + cchkey > keyend )
        return nullptr;

      while ( cchkey >= 2 )
      {
        if ( *keyptr++ != uint8_t(*s++) ) return nullptr;
        if ( *keyptr++ != uint8_t(*s++) ) return nullptr;
          cchkey -= 2;
      }
      if ( cchkey != 0 )
        if ( *keyptr++ != uint8_t(*s++) ) return nullptr;

    // get branch count and level flags
      bcount = uint8_t(*s++);
      bflags = uint8_t(*s++);

      b2offs = 1 + (bflags & 0x3);

    // check if the key is completely scanned
      if ( keyptr == keyend )
        return (bflags & 0x40) != 0 ? s + bcount * (1 + b2offs) : nullptr;

    // find matching branch
      for ( pmatch = s; pmatch != s + bcount && uint8_t(*pmatch) < *keyptr; ++pmatch )
        (void)NULL;

      if ( pmatch == s + bcount || *keyptr != uint8_t(*pmatch) )
        return nullptr;

    // get branch offset
      if ( getint( s + bcount + b2offs * (pmatch - s), offset, b2offs ) == nullptr )
        return nullptr;

    // skip the rest of relocations
      s += offset + (1 + b2offs) * bcount;
    }
    return s;
  }

}}

# endif   // !__mtc_radix_tree_hpp__
