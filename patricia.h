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
# if !defined( __mtc_patricia_h__ )
# define __mtc_patricia_h__
# include "serialize.h"
# include <cstdint>
# include <cassert>
# include <string>
# include <memory>
# include <vector>

# if defined( TEST_PATRICIA )
#   include <stdio.h>
# endif  // TEST_PATRICIA

namespace mtc       {
namespace patricia  {

  class key
  {
    template <class V, class A>
    friend class tree;
    friend class dump;

    const unsigned char*  ptr;
    size_t                len;

  public:     // construction
    key();
    template <class chartype>
    key( const chartype*, size_t = (size_t)-1 );
    template <class chartype, class traits, class allocate>
    key( const std::basic_string<chartype, traits, allocate>& );

  public:     // key access
    auto    getptr() const -> const unsigned char*  {  return ptr;  }
    auto    getlen() const -> size_t                {  return len;  }

    auto    begin() const -> const unsigned char*   {  return getptr();  }
    auto    end()   const -> const unsigned char*   {  return getptr() + getlen();  }


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

  class tape
  {
    class page;

  public:
    class pred;
    class post;

    template <class O>
    friend O*  Serialize( tape::pred*, const void*, size_t );
    template <class O>
    friend O*  Serialize( tape::post*, const void*, size_t );

  public:
    tape() = default;

  public:
    auto  append() -> post;
    auto  unwind( size_t ) -> pred;

  public:
    size_t  GetBufLen() const;
    template <class O>
    O*      Serialize( O* ) const;

  protected:
    std::unique_ptr<page>   head;
    std::unique_ptr<page>*  last = &head;

  };

  class tape::page
  {
    friend class tape;
    friend class post;
    friend class pred;

    class put_to_tail {};
    class put_to_head {};

    std::unique_ptr<page> next;
    size_t                size;
    char*                 pbeg;
    char*                 pend;

    page( size_t nbytes, std::unique_ptr<page>&& ppnext ):
      next( std::move( ppnext ) ),
      size( nbytes ),
      pbeg( head() + nbytes ),
      pend( pbeg )  {}

  public:
    static  std::unique_ptr<page> create( size_t space );
    static  std::unique_ptr<page> create( size_t space, std::unique_ptr<page>&& );
    static  std::unique_ptr<page> create( size_t space, const put_to_head& );
    static  std::unique_ptr<page> create( size_t space, const put_to_tail& );

  public:
    void  operator delete( void* p )  {  delete[] (char*)p;  }

  public:
    auto  head() -> char* {  return (char*)(this + 1);  }
    auto  tail() -> char* {  return size + (char*)this;  }

  };

  class tape::post
  {
    friend class tape;

    tape& parent;

    post( tape& to ): parent( to )  {}

  public:
    void  operator()( const void*, size_t );
    auto  ptr() -> post*  {  return this;  }
  };

  class tape::pred
  {
    friend class tape;

    std::unique_ptr<page>*  toPage;
    char*                   pstore;

    pred( tape& to ):
      toPage( &to.head ),
      pstore( toPage->get()->pbeg )  {}

  public:
    void  operator()( const void*, size_t );
    auto  ptr() -> pred*  {  return this;  }
  };

}}

template <>
inline  auto  Serialize( mtc::patricia::tape::pred* pred, const void* buff, size_t size ) -> mtc::patricia::tape::pred*
  {  return (*pred)( buff, size ), pred;  }

template <>
inline  auto  Serialize( mtc::patricia::tape::post* post, const void* buff, size_t size ) -> mtc::patricia::tape::post*
  {  return (*post)( buff, size ), post;  }

namespace mtc       {
namespace patricia  {

  namespace
  {
    class nothing {};
  }

// key implementation

  inline  key::key(): ptr( nullptr ), len( 0 )  {}

  template <class chartype>
  key::key( const chartype* s, size_t l ): ptr( (const unsigned char*)s ), len( 0 )
  {
    if ( (len = l) == (size_t)-1 && ptr != nullptr )
      for ( len = 0; ptr[len] != 0; ++len ) (void)NULL;
    len *= sizeof(chartype);
  }

  template <class chartype, class traits, class allocate>
  key::key( const std::basic_string<chartype, traits, allocate>& s ):
    key( s.c_str(), s.length() ) {}

  inline int key::compare( const key& k ) const
  {
    auto  l1 = getlen();
    auto  l2 = k.getlen();
    auto  rc = memcmp( getptr(), k.getptr(), std::min( l1, l2 ) );

    return rc != 0 ? rc : l1 - l2;
  }

  template <class V = nothing, class A/*llocator*/ = std::allocator<char>>
  class tree
  {
    template <class Base, class T>
    using rebind = typename std::allocator_traits<Base>::template rebind_alloc<T>;

    class node;

    template <class value, class nodes>
    class base_iterator
    {
      friend class tree;

      patricia::key patkey;
      value*        patval;

    protected:
      base_iterator( nodes* );
      base_iterator( const base_iterator& ) = delete;
      base_iterator& operator = ( const base_iterator& ) = delete;

    public:     // construction
      base_iterator();
      base_iterator( base_iterator&& );
      base_iterator& operator = ( base_iterator&& );

    public:     // fields
      auto  key() const -> const patricia::key&;
      auto  val() const -> value&;

    public:     // iterator
      base_iterator  operator ++ ( int ) = delete;
      base_iterator& operator ++ ();
      bool  operator == ( const base_iterator& it ) const;
      bool  operator != ( const base_iterator& it ) const  {  return !operator == ( it );  }

    protected:  // helpers
      base_iterator&  setkey();
      base_iterator&  get_lo();

    protected:
      std::vector<nodes*> atrace;
      std::vector<char>   achars;

    };

  public:     // key iterator
    using value_type = V;

    using iterator = base_iterator<V, node>;
    using const_iterator = base_iterator<const V, const node>;

  private:  // hidden implementation
    class node
    {
      template <class R, class S> static
      auto  search( const unsigned char*, size_t, S& ) -> R*;

    public:
      A&            alloc;
      node*         pnext;
      node*         plist;
      uint32_t      usets;
      union
      {
        char        dummy;
        V           value;
      };
      unsigned char chars[1];

    private:
      void  operator delete( void* ) = delete;
     ~node();

    public:
      node( A&, const unsigned char*, size_t, node* );

    public:
      static  auto  Create( A&, const unsigned char*, size_t, node* = nullptr ) -> node*;
              void  Delete();
      static  auto  fmatch( const unsigned char*, size_t, const unsigned char*, size_t ) -> size_t;

    public:
      auto  key_beg() const -> const unsigned char* {  return chars;  }
      auto  key_end() const -> const unsigned char* {  return chars + keylen();  }

    public:
            node*   search( const unsigned char* key, size_t len )        {  return search<node>( key, len, *this );  }
      const node*   search( const unsigned char* key, size_t len ) const  {  return search<node>( key, len, *this );  }

            node*   insert( const unsigned char*, size_t );
            auto    remove( const unsigned char*, size_t ) -> bool;

            bool    hasval() const    {  return (usets & 0x80000000) != 0;  }
            size_t  keylen() const    {  return (usets & ~0x80000000);  }
            void    setlen( size_t );

            void    delval();

            V*      getval()        {  return (usets & 0x80000000) != 0 ? &value : nullptr;  }
      const V*      getval() const  {  return (usets & 0x80000000) != 0 ? &value : nullptr;  }

            V*      setval( const V& );
            V*      setval( V&& );

    public:     // master iterator
      template <class act>  int   for_each( act action )        {  return for_impl( action, *this );  }
      template <class act>  int   for_each( act action ) const  {  return for_impl( action, *this );  }

    protected:
      template <class act, class slf> static  int for_impl( act, slf& );

    public:     // serialization
                          size_t  GetBufLen() const;
      template <class S>  S*      FetchFrom( S*, size_t, size_t );
      template <class O>  O*      Serialize( O* ) const;
      template <class P>  void    PrintTree( P, size_t ) const;

    };

  private:
    tree( const tree& ) = delete;
    tree& operator = ( const tree& ) = delete;

  public:     // construction
    tree( const std::initializer_list<std::pair<key, V>>&, const A& = A() );
    tree( const A& = A() );
    tree( tree&& );
   ~tree();
    tree& operator = ( tree&& );

  public:     // API
                                            bool  Delete( const key& k );
                                            V*    Insert( const key& k, const V& v = V() )  {  return insert( k, v );  }
                                            V*    Insert( const key& k, V&& v )             {  return insert( k, std::move( v ) );  }
                                      const V*    Search( const key& k ) const              {  return search<const V>( k, *this );  }
                                            V*    Search( const key& k )                    {  return search<      V>( k, *this );  }

    template <class chartype = char>        bool  Delete( const chartype* k, size_t l )               {  return Delete( key( k, l ) );  }
    template <class chartype = char>        V*    Insert( const chartype* k, size_t l, const V& v )   {  return Insert( key( k, l ), v );  }
    template <class chartype = char>        V*    Insert( const chartype* k, size_t l, V&& v )        {  return Insert( key( k, l ), std::move( v ) );  }
    template <class chartype = char>  const V*    Search( const chartype* k, size_t l ) const         {  return Search( key( k, l ) );  }
    template <class chartype = char>        V*    Search( const chartype* k, size_t l )               {  return Search( key( k, l ) );  }

  public:     // iterator
    auto  cbegin() const -> const_iterator {  return begin();  }
    auto  cend()   const -> const_iterator {  return end();  }
    auto  begin()  const -> const_iterator {  return const_iterator( ptree );  }
    auto  begin()        ->       iterator {  return iterator( ptree );  }
    auto  end()    const -> const_iterator {  return const_iterator();  }
    auto  end()          ->       iterator {  return iterator();  }

  public:     // capacity
    auto  empty() const -> bool {  return ptree == nullptr;  }

  public:     // modifiers
    void  clear();
    auto  erase( const key& k ) -> size_t {  return Delete( k ) ? 1 : 0;  }
    template <class InputIt>
    void  insert( InputIt first, InputIt last );
    void  insert( std::initializer_list<std::pair<key, V>> l )  {  insert( l.begin(), l.end() );  }

  public:     // iterable access
    auto  find       ( const key& k ) const -> const_iterator {  return std::move( findit<const_iterator>( k, *this ) );  }
    auto  find       ( const key& k )       ->       iterator {  return std::move( findit<      iterator>( k, *this ) );  }
    auto  lower_bound( const key& k ) const -> const_iterator {  return std::move( lbound<const_iterator>( k, *this ) );  }
    auto  lower_bound( const key& k )       ->       iterator {  return std::move( lbound<      iterator>( k, *this ) );  }
    auto  upper_bound( const key& k ) const -> const_iterator {  return std::move( findit<const_iterator>( k, *this ) );  }
    auto  upper_bound( const key& k )       ->       iterator {  return std::move( findit<      iterator>( k, *this ) );  }

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class S>  S*      FetchFrom( S* );
    template <class P>  void    PrintTree( P  ) const;
    template <class O>  O*      Serialize( O* ) const;

  public:
    template <class act>  int   for_each( act action )        {  return ptree != nullptr ? ptree->for_each( action ) : 0;  }
    template <class act>  int   for_each( act action ) const  {  return ptree != nullptr ? ptree->for_each( action ) : 0;  }

  protected:
                                            V*    insert( const key&, const V& );
                                            V*    insert( const key&,  V&& );
    template <class res, class slf> static  res*  search( const key&, slf& );
    template <class itr, class slf> static  itr   findit( const key&, slf& );
    template <class itr, class slf> static  itr   lbound( const key&, slf& );
    template <class itr, class slf> static  itr   ubound( const key&, slf& );
                                    static  auto  cmpkey( const unsigned char*, size_t, const unsigned char*, size_t ) -> int;
                                    static  auto  cmpkey( const unsigned char*, size_t, const node& ) -> int;

  protected:  // var
    A     alloc;
    node* ptree;

  };

  class dump
  {
    enum
    {
      asterisk = 0x0100 + '*',
      question = 0x0100 + '?'
    };

  public:     // iterator
    class iterator
    {
      friend class dump;

      patricia::key patkey;
      const void*   patval;

      struct  patpos
      {
        const char* dicptr;
        const char* keyptr;
        const char* endptr;
        size_t      keylen;
        size_t      nnodes;
        bool        bvalue;
      };

      std::vector<patpos> atrace;
      std::vector<char>   achars;

    protected:  // first initialization constructor
      iterator( const char* );
      iterator( const iterator& ) = delete;
      iterator& operator = ( const iterator& ) = delete;

    public:     // construction
      iterator();
      iterator( iterator&& );
      iterator& operator = ( iterator&& );

    public:     // fields
      auto  key() const -> const patricia::key&;
      auto  val() const -> const void*;

    public:     // iterator
      iterator  operator ++ ( int ) = delete;
      iterator& operator ++ ();
      bool  operator == ( const iterator& it ) const;
      bool  operator != ( const iterator& it ) const  {  return !operator == ( it );  }

    public:
      iterator& MoveTo( const uint8_t* key, size_t len );

    protected:
      iterator& setkey();
      static
      int       CmpKey( const void*, size_t, const void*, size_t );
      patpos    GetPat( const char* );
      iterator& Tonext();

    };

  public:     // construction
    dump(): serial( nullptr ) {}
    dump( const void* p ): serial( (const char*)p ) {}
      
  public:     // search
                              const char* Search( const key& ) const;
    template <class chartype> const char* Search( const chartype* k, size_t l ) const {  return Search( key( (const char*)k, l * sizeof(*k) ) );  }

    template <class _func_>   int         Select( const void*, size_t, _func_ ) const;

  public:     // iterator
            iterator  begin() const {  return iterator( serial );  }
    static  iterator  end()         {  return iterator();  }

  protected:
    template <class A>
    static  const char*         scantree( const char*, std::vector<char>&, size_t, A );
    
  protected:  // helpers
    const char* Search( const char* keystr,
                        size_t      keylen,
                        const char* thedic ) const;
    template <class _func_>
    int         Select( const char* dicstr, int         diclen,
                        const char* keystr, const char* keyend,
                        const char* thedic, int         nnodes,
                        _func_      insert,
                        char*       buftop,
                        char*       bufend,
                        char*       bufptr ) const;

  protected:  // internals
    static  auto  JumpOver( int, const char* ) -> const char*;

  protected:  // variables
    const char* serial;

  };

  // tape::page implementation

  inline
  std::unique_ptr<tape::page> tape::page::create( size_t space )
  {
    auto  stub = std::unique_ptr<page>();
    auto  make = create( space, std::move( stub ) );
      make->pend =
      make->pbeg = make->head() + (make->size >> 1);
    return make;
  }

  inline
  std::unique_ptr<tape::page> tape::page::create( size_t space, std::unique_ptr<page>&& to )
  {
    auto  nalign = (space + sizeof(page) + 0x1000 - 1) & ~(0x1000 - 1);

    return std::unique_ptr<page>( new ( new char[nalign] ) page(
      nalign - sizeof(page), std::move( to ) ) );
  }

  inline
  std::unique_ptr<tape::page> tape::page::create( size_t space, const put_to_head& )
  {
    auto  palloc = create( space );
      palloc->pbeg =
      palloc->pend = palloc->tail();
    return palloc;
  }

  inline
  std::unique_ptr<tape::page> tape::page::create( size_t space, const put_to_tail& )
  {
    auto  palloc = create( space );
      palloc->pbeg =
      palloc->pend = palloc->head();
    return palloc;
  }

  // tape::post implementation

  inline
  void  tape::post::operator ()( const void* p, size_t l )
  {
    auto  src = (const char*)p;
    auto  end = l + src;

    if ( parent.head == nullptr )
      parent.head = page::create( 0x1000, page::put_to_tail() );

    while ( src != end )
    {
      auto& toPage = *parent.last->get();
      auto  lAvail = toPage.tail() - toPage.pend;
      auto  endptr = std::min( end, src + lAvail );

      if ( lAvail == 0 )
        parent.last = &(toPage.next = page::create( toPage.size, page::put_to_tail() ));
      else while ( src != endptr )  *toPage.pend++ = *src++;
    }
  }

  // tape::pred implementation

  inline
  void  tape::pred::operator()( const void* p, size_t l )
  {
    auto  src = (const char*)p;
    auto  end = l + src;

    while ( src != end )
    {
      auto  nAvail = toPage->get()->pend - pstore;

      if ( pstore < toPage->get()->pbeg || pstore > toPage->get()->pend )
        throw std::range_error( "buffer prepender internal arror" );

      if ( nAvail == 0 )
      {
        if ( (toPage = &toPage->get()->next)->get() == nullptr )
          throw std::range_error( "buffer prepender internal arror: writing more than allocated bytes" );
        pstore = toPage->get()->pbeg;
      }
        else
      for ( auto srcend = std::min( end, src + nAvail ); src != srcend; )
        *pstore++ = *src++;
    }
  }

  // tape inline implementation

  inline
  auto  tape::unwind( size_t size ) -> pred
  {
    if ( head == nullptr )
      head = page::create( 0x1000, page::put_to_head() );

    while ( size != 0 )
    {
      auto& toPage = *head.get();
      auto  nAvail = toPage.pbeg - toPage.head();

      if ( nAvail == 0 )
      {
        head = page::create(
          toPage.size, std::move( head ) );
        if ( last == &head )
          last = &head->next;
      }
        else
      {
        auto  nStore = std::min( size, (size_t)nAvail );

        toPage.pbeg -= nStore;
        size        -= nStore;
      }
    }
    return pred( *this );
  }

  inline
  auto  tape::append() -> post
  {
    return post( *this );
  }

  inline
  size_t tape::GetBufLen() const
  {
    auto  size = 0;

    for ( auto p = &head; p->get() != nullptr; p = &(p->get()->next) )
      size += p->get()->pend - p->get()->pbeg;
    return size;
  }

  template <class O> inline
  O*  tape::Serialize( O* o ) const
  {
    for ( auto p = &head; p->get() != nullptr && o != nullptr; p = &(p->get()->next) )
      o = ::Serialize( o, p->get()->pbeg, p->get()->pend - p->get()->pbeg );
    return o;
  }

  // patricia implementation

  template <class V, class A>
  tree<V, A>::node::node( A& mem, const unsigned char* key, size_t len, node* nex ):
    alloc( mem ),
    pnext( nex ),
    plist( nullptr ),
    usets( len )
  {
    if ( key != nullptr )
      memcpy( chars, key, len );
  }

  template <class V, class A>
  tree<V, A>::node::~node()
  {
    if ( hasval() )
      value.~V();
    if ( pnext != nullptr )
      pnext->Delete();
    if ( plist != nullptr )
      plist->Delete();
  }

  template <class V, class A>
  auto  tree<V, A>::node::Create( A&  mem, const unsigned char* key, size_t len, node* nex ) -> tree<V, A>::node*
  {
    size_t  minlen = len != 0 ? len : 1;
    size_t  cchstr = (minlen + 0x0f) & ~0x0f;
    size_t  nalloc = sizeof(node) - sizeof(node::chars) + cchstr;
    auto    palloc = mem.allocate( nalloc );

    return new ( palloc ) node( mem, key, len, nex );
  }

  template <class V, class A>
  void  tree<V, A>::node::Delete()
  {
    auto  dealloc = rebind<A, node>( alloc );
      this->~node();
    dealloc.deallocate( this, 0 );
  }

  template <class V, class A>
  size_t  tree<V, A>::node::fmatch( const unsigned char* k_1, size_t l_1, const unsigned char* k_2, size_t l_2 )
  {
    auto  k1e = k_1 + l_1;
    auto  k2e = k_2 + l_2;
    auto  k1p = k_1;

    while ( k1p < k1e && k_2 < k2e && *k1p == *k_2++ )
      ++k1p;

    return k1p - k_1;
  }

  template <class V, class A>
  bool  tree<V, A>::node::remove( const unsigned char* key, size_t len )
  {
    auto  keychr = len != 0 ? *key : 0;   // safe 'get' - no SIGFAULT
    auto  pplist = &plist;

  // если длина ключа нулевая, значит, элемент найден и надо удалить ассоциированные с ним данные;
  // проверяется сама возможность этого события;
  // если есть ассоциированные данные, вернуть true
    if ( len == 0 )
    {
      if ( !hasval() )  throw std::logic_error( "empty element being deleted" );
        else delval();

    // вернуть true, если элемент не имеет вложенных элементов или имеет единственный
      return plist == nullptr || plist->pnext == nullptr;
    }

  // найти элемент в списке вложенных подключей, который может содержать искомый ключ;
  // если такой есть, сравнить собственно подключ с текущим фрагментом ключа
    while ( *pplist != nullptr && keychr > (*pplist)->chars[0] )
      pplist = &(*pplist)->pnext;

    if ( *pplist != nullptr )
    {
      auto  curlen = (*pplist)->keylen();
      auto  lmatch = fmatch( key, len, (*pplist)->chars, curlen );

    // если часть ключа текущего элемента в p_scan полностью покрывается искомым ключом, вызвать её рекурсивно
    // и, если remove() вернёт true, проверить, не выродилась ли ветвь;
    // если ветвь выродилась (стала пустой), удалить её;
    // если она имеет единственное продолжение, слить ветвь с ним
      if ( lmatch >= curlen )
      {
        if ( (*pplist)->remove( key + curlen, len - curlen ) )
        {
        // если прочищенный элемент не имеет вложенных элементов, заместить его в списке на следующий
        // за ним одноранговый элемент; если такого нет, вернуть true
          if ( (*pplist)->plist == nullptr )
          {
            auto  pmove = (*pplist)->pnext;
              (*pplist)->pnext = nullptr;
            (*pplist)->Delete();
              *pplist = pmove;
            return !hasval() && plist == nullptr;
          }

          if ( (*pplist)->plist->pnext != nullptr )
            throw std::logic_error( "invalid patricia::node::remove() returns true" );

          auto  palloc = node::Create( alloc, nullptr, (*pplist)->keylen() + (*pplist)->plist->keylen(), (*pplist)->pnext );
            (*pplist)->pnext = nullptr;

          if ( (*pplist)->plist->hasval() )
            palloc->setval( std::move( (*pplist)->plist->value ) );

          std::copy( (*pplist)->plist->key_beg(), (*pplist)->plist->key_end(),
            std::copy( (*pplist)->key_beg(), (*pplist)->key_end(),
              palloc->chars ) );

          (*pplist)->Delete();
            *pplist = palloc;
        }
        return false;
      }
    }
    return false;
  }

  template <class V, class A>
  typename tree<V, A>::node* tree<V, A>::node::insert( const unsigned char* thekey, size_t cchkey )
  {
  // если поисковый ключ отсканирован полностью, навершить поиск
    if ( cchkey == 0 )
      return this;

  // найти первый элемент во вложенном массиве, у которого первый символ ключа больше
  // либо равен первому символу вставляемого ключа;
  // если таковой не найден, вставить новый узел с требуемым значением полного ключа
  // и вернуть его;
    auto  keychr = *thekey;
    auto  pprepl = &plist;

    while ( *pprepl != nullptr && keychr > (*pprepl)->chars[0] )
      pprepl = &(*pprepl)->pnext;

    if ( *pprepl == nullptr || keychr != (*pprepl)->chars[0] )
      return *pprepl = Create( alloc, thekey, cchkey, *pprepl );

    assert( keychr == (*pprepl)->chars[0] );

  // есть некоторое совпадение, полное или частичное, искомого ключа с частичным ключом
  // найденного узла в списке вложенных узлов;
  // определить длину совпадения ключа с ключевой последовательностью узла;
  // если совпадение полное, вызвать метод рекурсивно, поправив указатель на ключ
    auto  curlen = (*pprepl)->keylen();
    auto  lmatch = fmatch( thekey, cchkey, (*pprepl)->chars, curlen );  assert( lmatch > 0 && lmatch <= curlen );

    if ( lmatch == curlen )
      return (*pprepl)->insert( thekey + curlen, cchkey - curlen );

    assert( lmatch < curlen );

  // добавляемый ключ частично совпадает с ключом найденного вложенного элемента;
  // если длина совпадения равна длине добавляемого ключа, создать новый элемент
  // вместо p_scan с таким частичным ключом, а ключ у p_scan модифицировать (укоротить)
  // добавляемый ключ длиннее длины совпадения;
  // создать новый элемент для узла и добавить в него два элемента:
  // остаток добавляемого ключа и найденный p_scan с усечённым ключом
    if ( lmatch == cchkey )
    {
      auto  palloc = Create( alloc, thekey, lmatch, (*pprepl)->pnext );
        (*pprepl)->pnext = nullptr;

      memmove( (*pprepl)->chars, (*pprepl)->chars + lmatch, curlen - lmatch );
        (*pprepl)->setlen( curlen - lmatch );
        (*pprepl)->pnext = nullptr;

      palloc->plist = *pprepl;
        *pprepl = palloc;

      return *pprepl;
    }

  // есть хвост и от добавляемого ключа, и от текущего элемента
    assert( cchkey > lmatch && curlen > lmatch );

    auto  palloc = Create( alloc, thekey, lmatch, (*pprepl)->pnext );
      (*pprepl)->pnext = nullptr;
    auto  p_tail = Create( alloc, thekey + lmatch, cchkey - lmatch );
    auto  rescmp( thekey[lmatch] - (*pprepl)->chars[lmatch] );

    memmove( (*pprepl)->chars, (*pprepl)->chars + lmatch, curlen - lmatch );
      (*pprepl)->setlen( curlen - lmatch );
      (*pprepl)->pnext = nullptr;

    if ( rescmp < 0 )
    {
      (palloc->plist = p_tail)->pnext = *pprepl;
        *pprepl = nullptr;
    }
      else
    {
      (palloc->plist = *pprepl)->pnext = p_tail;
        *pprepl = nullptr;
        p_tail = nullptr;
    }

    return (*pprepl = std::move( palloc ))->insert( thekey + lmatch, cchkey - lmatch );
  }

  template <class V, class A>
  void  tree<V, A>::node::delval()
  {
    if ( hasval() )
      value.~V();
    usets &= ~0x80000000;
  }

  template <class V, class A>
  void  tree<V, A>::node::setlen( size_t l )
  {
    size_t  minlen = (usets & ~0x80000000) != 0 ? usets & ~0x80000000 : 1;
    size_t  maxlen = (minlen + 0x0f) & ~0x0f;   assert( l <= maxlen );

    usets = (usets & 0x80000000) | (uint32_t)l;
  }

  template <class V, class A>
  V*    tree<V, A>::node::setval( const V& v )
  {
    if ( hasval() )
      value.~V();
    return (usets |= 0x80000000),
      new( &value ) V( v );
  }

  template <class V, class A>
  V*    tree<V, A>::node::setval( V&& v )
  {
    if ( hasval() )
      value.~V();
    return (usets |= 0x80000000),
      new( &value ) V( std::move( v ) );
  }

  template <class V, class A>
  template <class act, class slf>
  int   tree<V, A>::node::for_impl( act action, slf& r_node )
  {
    auto  pvalue = r_node.getval();
    int   nerror;

    if ( pvalue != nullptr )
      if ( (nerror = action( *pvalue )) != 0 )
        return nerror;

    for ( auto p = r_node.plist; p != nullptr; p = p->pnext )
      if ( (nerror = p->for_each( action )) != 0 )
        return nerror;

    return 0;
  }

  template <class V, class A>
  size_t  tree<V, A>::node::GetBufLen() const
  {
    size_t    arsize = 0;
    size_t    ccharr = 0;
    size_t    curlen = keylen();
    const V*  pvalue = getval();

    for ( auto p = plist; p != nullptr; p = p->pnext, arsize += 2 )
    {
      ccharr += p->GetBufLen();
    }

    if ( pvalue != nullptr )
    {
      ccharr += ::GetBufLen( *pvalue );
      arsize |= 1;
    }

  // serialize byte count of nested branches
    return ::GetBufLen( curlen ) + curlen + ::GetBufLen( ccharr ) + ccharr + ::GetBufLen( arsize );
  }

  template <class V, class A>
  template <class S>
  S*  tree<V, A>::node::FetchFrom( S* s, size_t cchstr, size_t arsize )
  {
    auto    bvalue( (arsize & 1) != 0 );
    auto    nitems = arsize >> 1;
    size_t  cbjump;

    if ( cchstr != 0 )
    {
      if ( cchstr != keylen() )
        return nullptr;
      if ( (s = ::FetchFrom( s, chars, cchstr )) == nullptr )
        return nullptr;
    }

    s = ::FetchFrom( s, cbjump );

    for ( auto p = &plist; nitems != 0; p = &(*p)->pnext, --nitems )
    {
      size_t  sublen;
      size_t  subarr;

      if ( (s = ::FetchFrom( ::FetchFrom( s, sublen ), subarr )) == nullptr )
        return nullptr;

      *p = node::Create( alloc, nullptr, sublen );

      if ( (s = (*p)->FetchFrom( s, sublen, subarr )) == nullptr )
        return nullptr;
    }

    return bvalue ? ::FetchFrom( s, *setval( V() ) ) : s;
  }

  template <class V, class A>
  template <class P>
  void  tree<V, A>::node::PrintTree( P print, size_t before ) const
  {
    for ( auto p = plist; p != nullptr; p = p->pnext )
    {
      auto  thekey = p->chars;
      auto  cchkey = p->keylen();
      auto  nwrite = std::min( cchkey, (size_t)(0x20 - 3) );

      for ( size_t i = 0; i < before; ++i )
        print( ' ' );

      for ( size_t i = 0; i < nwrite; ++i )
        print( thekey[i] );

      if ( cchkey > 0x20 - 3 )
        print( '.' ), print( '.' ), print( '.' );

      print( '\n' );

      p->PrintTree( print, before + 2 );
    }
  }

  template <class V, class A>
  template <class O>
  O*  tree<V, A>::node::Serialize( O* o ) const
  {
    size_t    arsize = 0;
    size_t    ccharr = 0;
    size_t    curlen = keylen();
    const V*  pvalue = getval();

    for ( auto p = plist; p != nullptr; p = p->pnext, arsize += 2 )
      ccharr += p->GetBufLen();

    if ( pvalue != nullptr )
      arsize |= 1;

  // store key size, array size and key
    if ( (o = ::Serialize( ::Serialize( ::Serialize( o, curlen ), arsize ), chars, curlen )) == nullptr )
      return nullptr;

  // write the array and value size in bytes
    if ( (o = ::Serialize( o, ccharr + (pvalue != nullptr ? ::GetBufLen( *pvalue ) : 0) )) == nullptr )
      return nullptr;

    for ( auto p = plist; p != nullptr; p = p->pnext )
    {
      if ( (o = p->Serialize( o )) == nullptr )
        return nullptr;
    }

    return pvalue != nullptr ? ::Serialize( o, *pvalue ) : o;
  }

  template <class V, class A>
  template <class R, class S>
  auto  tree<V, A>::node::search( const unsigned char* thekey, size_t cchkey, S& self ) -> R*
    {
      for ( auto  p_this = &self; ; )
      {
        if ( cchkey == 0 )
          return p_this;

      // найти элемент во вложенном массиве, у которого первый символ ключа равен первому символу вставляемого ключа
        auto  keychr = *thekey;
        auto  p_scan = p_this->plist;

        while ( p_scan != nullptr && keychr > p_scan->chars[0] )
          p_scan = p_scan->pnext;

        if ( p_scan != nullptr && keychr == p_scan->chars[0] )
          {
            auto  curlen = p_scan->keylen();

            if ( cchkey >= curlen && memcmp( thekey, p_scan->chars, curlen ) == 0 )
              {
                p_this = p_scan;
                thekey += curlen;
                cchkey -= curlen;
                continue;
              }
          }

        return nullptr;
      }
    }

  // tree::base_iterator implementation

  template <class V, class A>
  template <class value, class nodes>
  tree<V, A>::base_iterator<value, nodes>::base_iterator( nodes* p ): patval( nullptr )
    {
      if ( p != nullptr )
      {
        atrace.push_back( p );
        get_lo();
      }
    }

  template <class V, class A>
  template <class value, class nodes>
  tree<V, A>::base_iterator<value, nodes>::base_iterator(): patval( nullptr )
    {
    }

  template <class V, class A>
  template <class value, class nodes>
  tree<V, A>::base_iterator<value, nodes>::base_iterator( base_iterator&& it ):
    patkey( std::move( it.patkey ) ), patval( std::move( it.patval ) ),
      atrace( std::move( it.atrace ) ),
      achars( std::move( it.achars ) )
    {
    }

  template <class V, class A>
  template <class value, class nodes>
  auto  tree<V, A>::base_iterator<value, nodes>::operator = ( base_iterator&& it ) -> tree<V, A>::base_iterator<value, nodes>&
    {
      patkey = std::move( it.patkey );
      patval = std::move( it.patval );
      atrace = std::move( it.atrace );
      achars = std::move( it.achars );
      return *this;
    }

  template <class V, class A>
  template <class value, class nodes>
  auto  tree<V, A>::base_iterator<value, nodes>::key() const -> const patricia::key&
    {
      return patkey;
    }

  template <class V, class A>
  template <class value, class nodes>
  auto  tree<V, A>::base_iterator<value, nodes>::val() const -> value&
    {
      assert( patval != nullptr );
      return *patval;
    }

  template <class V, class A>
  template <class value, class nodes>
  auto  tree<V, A>::base_iterator<value, nodes>::operator ++ () -> tree<V, A>::base_iterator<value, nodes>&
    {
      while ( atrace.size() != 0 )
      {
        auto  p_node = atrace.back()->plist;

      // если у узла есть вложенные элементы, максимально продвинуться вглубь дерева,
      // но не дальше первого найденного элемента со значением
        if ( p_node != nullptr )
        {
          do atrace.push_back( p_node );
            while ( !p_node->hasval() && (p_node = p_node->plist) != nullptr );
        }
          else
      // иначе, если вложенных элементов нет, перейти к следующему элементу в списке
      // того же горизонтального уровня
        if ( (p_node = atrace.back()->pnext) != nullptr )
        {
          atrace.back() = p_node;
        }
          else
      // отмотать вниз по дереву с переходом на следующий элемент до успешного ключа
        {
          do atrace.pop_back();
            while ( atrace.size() != 0 && atrace.back()->pnext == nullptr );

          if ( atrace.size() != 0 ) atrace.back() = atrace.back()->pnext;
            else continue;
        }

      // если найден узел со значением, вернуть его
        if ( atrace.back()->hasval() )
          return setkey();
      }
      atrace.clear();
      return setkey();
    }

  template <class V, class A>
  template <class value, class nodes>
  bool  tree<V, A>::base_iterator<value, nodes>::operator == ( const base_iterator& it ) const
    {
      return patkey == it.patkey && atrace.size() == it.atrace.size()
        && std::equal( atrace.begin(), atrace.end(), it.atrace.begin() );
    }

  template <class V, class A>
  template <class value, class nodes>
  auto  tree<V, A>::base_iterator<value, nodes>::setkey() -> tree<V, A>::base_iterator<value, nodes>&
    {
      assert( atrace.size() == 0 || atrace.back()->hasval() );

      if ( atrace.size() != 0 )
      {
        achars.clear();

        for ( auto nd: atrace )
          achars.insert( achars.end(), nd->key_beg(), nd->key_end() );

        patkey = std::move( patricia::key( achars.data(), achars.size() ) );
        patval = &atrace.back()->value;
      }
        else
      {
        patkey = patricia::key();
        patval = nullptr;
      }
      return *this;
    }

  template <class V, class A>
  template <class value, class nodes>
  auto  tree<V, A>::base_iterator<value, nodes>::get_lo() -> tree<V, A>::base_iterator<value, nodes>&
    {
      while ( atrace.size() != 0 && !atrace.back()->hasval() )
      {
        auto  list = atrace.back()->plist;

        if ( list != nullptr )  atrace.push_back( list );
          else atrace.clear();
      }

      if ( atrace.size() != 0 && !atrace.back()->hasval() )
        atrace.clear();

      return setkey();
    }

    template <class V>
    class sink
    {
      class value_t;
      class chunk_t;

      using chunk_p = std::unique_ptr<chunk_t>;

    protected:
      template <class chartype>
      auto  insert( chartype* k, size_t l ) -> chunk_t*;

    public:
      V*  Insert( const key&, V&& );
      V*  Insert( const key&, const V& );

    public:
      size_t  GetBufLen() const
      {  return pat != nullptr ? pat->GetBufLen() : ::GetBufLen( 0 ) + ::GetBufLen( 0 ) + ::GetBufLen( 0 );  }
      template <class O>
      O*    Serialize( O* o ) const
      {  return pat != nullptr ? pat->Serialize( o ) : ::Serialize( ::Serialize( ::Serialize( o, 0 ), 0 ), 0 );  }

    protected:
      chunk_p   pat;

    };

    template <class V>
    class sink<V>::value_t
    {
      using place = typename std::aligned_storage<sizeof(V), alignof(V)>::type;

      place aval;
      V*    pval;

    public:
      value_t(): pval( nullptr )  {}
     ~value_t() {  clear();  }

    public:
      bool  isset() const {  return pval != nullptr;  }
      void  clear();

    public:
      auto  operator = ( V&& v ) -> V*        {  return clear(), pval = new( &aval ) V( std::move( v ) );  }
      auto  operator = ( const V& v ) -> V*   {  return clear(), pval = new( &aval ) V( v );  }

    public:     // serialization
      auto  GetBufLen() const -> size_t
      {  return isset() ? ::GetBufLen( *pval ) : 0;  };
      template <class O>
      O*    Serialize( O* o ) const
      {  return isset() ? ::Serialize( o, *pval ) : o;  }

    };

    template <class V>
    class sink<V>::chunk_t
    {
      std::string key;
      value_t     val;
      tape        set;
      size_t      cnt = 0;
      chunk_p     sub;

    public:
      template <class chartype>
      chunk_t( chartype* k, size_t l ): key( (const char*)k, l )  {}

    public:
      template <class chartype>
      auto  insert( chartype* k, size_t l ) -> chunk_t*;
      auto  setval( V&& v ) -> V* {  return val = std::move( v );  }
      auto  setval( const V& v ) -> V*  {  return val = v;  }

    public:     // serialization
      auto  GetBufLen() const -> size_t;
      template <class O>
      O*    Serialize( O* ) const;

    protected:
      template <class chartype>
      auto  lmatch( chartype* k, size_t l ) -> std::tuple<int, int>;
    };

    // patricia implementation

  template <class V, class A>
  tree<V, A>::tree( const A& a ):
    alloc( a ), ptree( nullptr )  {}

  template <class V, class A>
  tree<V, A>::tree( tree&& p ):
    alloc( std::move( p.alloc ) ),
    ptree( p.ptree ) {  p.ptree = nullptr;  }

  template <class V, class A>
  tree<V, A>::tree( const std::initializer_list<std::pair<key, V>>& l, const A& a ):
    tree( a )
  {
    for ( auto& it: l )
      Insert( it.first, it.second );
  }

  template <class V, class A>
  tree<V, A>::~tree()
  {
    if ( ptree != nullptr )
      ptree->Delete();
  }

  template <class V, class A>
  auto  tree<V, A>::operator = ( tree&& t ) -> tree&
  {
    clear();
    alloc = std::move( t.alloc );
      ptree = t.ptree;
      t.ptree = nullptr;
    return *this;
  }

  template <class V, class A>
  bool  tree<V, A>::Delete( const key& k )
  {
    if ( ptree != nullptr && ptree->remove( k.ptr, k.len ) )
      clear();
    return true;
  }

  template <class V, class A>
  void  tree<V, A>::clear()
  {
    if ( ptree != nullptr )
      ptree->Delete();
    ptree = nullptr;
  }

  template <class V, class A>
  template <class InputIt>
  void  tree<V, A>::insert( InputIt first, InputIt end )
  {
    for ( ; first != end; ++first )
      Insert( first->first, first->second );
  }

  template <class V, class A>
  V*    tree<V, A>::insert( const key& k, const V& v )
  {
    node* pfound;

    if ( ptree == nullptr )
      ptree = node::Create( alloc, nullptr, 0 );

    return (pfound = ptree->insert( k.ptr, k.len )) != nullptr ?
      pfound->setval( v ) : nullptr;
  }

  template <class V, class A>
  V*    tree<V, A>::insert( const key& k, V&& v )
  {
    node* pfound;

    if ( ptree == nullptr )
      ptree = node::Create( alloc, nullptr, 0 );

    return (pfound = ptree->insert( k.ptr, k.len )) != nullptr ?
      pfound->setval( std::move( v ) ) : nullptr;
  }

  template <class V, class A>
  template <class res, class slf>
  res*  tree<V, A>::search( const key& k, slf& s )
  {
    if ( s.ptree != nullptr )
    {
      auto  pfound = s.ptree->search( k.ptr, k.len );

      return pfound != nullptr ? pfound->getval() : nullptr;
    }
    return nullptr;
  }

  template <class V, class A>
  template <class itr, class slf>
  auto  tree<V, A>::findit( const key& key, slf& self ) -> itr
  {
    itr   it;
    auto  pk = key.ptr;
    auto  cc = key.len;
    auto  pn = self.ptree;

    for ( it.atrace.push_back( pn ); pn != nullptr; )
    {
      auto    keychr = cc != 0 ? *pk : '\0';
      auto    p_scan = pn->plist;
      size_t  l_frag;

      if ( cc == 0 )
        return pn->hasval() ? std::move( it.setkey() ) : self.end();

      while ( p_scan != nullptr && keychr > p_scan->chars[0] )
        p_scan = p_scan->pnext;

      if ( p_scan != nullptr && keychr == p_scan->chars[0] )
      {
        l_frag = p_scan->keylen();

        if ( node::fmatch( pk, cc, p_scan->chars, l_frag ) == l_frag )
        {
          it.atrace.push_back( pn = p_scan );
            pk += l_frag;
            cc -= l_frag;
          continue;
        }
      }
      return self.end();
    }

    return self.end();
  }

  template <class V, class A>
  template <class itr, class slf>
  auto  tree<V, A>::lbound( const key& key, slf& self ) -> itr
  {
    itr   it;
    auto  pk = key.ptr;
    auto  cc = key.len;

    if ( self.ptree != nullptr ) it.atrace.push_back( self.ptree );
      else return self.end();

    while ( it.atrace.size() != 0 )
    {
      if ( cc > 0 )
      {
        auto  p_scan = it.atrace.back()->plist;
        int   rescmp = 0;

        while ( p_scan != nullptr && (rescmp = cmpkey( pk, cc, *p_scan )) > 0 )
          p_scan = p_scan->pnext;

        if ( p_scan == nullptr )
        {
          while ( it.atrace.size() != 0 )
          {
            if ( (it.atrace.back() = it.atrace.back()->pnext) == nullptr )  it.atrace.pop_back();
              else break;
          }
          return std::move( it.get_lo() );
        }

        it.atrace.push_back( p_scan );

        if ( rescmp == 0 )
        {
          pk += it.atrace.back()->keylen();
          cc -= it.atrace.back()->keylen();
        }
          else
        return std::move( it.get_lo() );
      }
        else
      return std::move( it.get_lo() );
    }
    return self.end();
  }

  template <class V, class A>
  template <class itr, class slf>
  auto  tree<V, A>::ubound( const key& key, slf& self ) -> itr
  {
    auto  it = lbound( key, self );

    while ( it != self.end() && it.key() == key )
      ++it;

    return std::move( it );
  }

  template <class V, class A>
  auto  tree<V, A>::cmpkey( const unsigned char* k1, size_t l1, const unsigned char* k2, size_t l2 ) -> int
  {
    auto  cc = std::min( l1, l2 );
    auto  rc = memcmp( k1, k2, cc );

    return rc != 0 ? rc : (cc > l2) - (cc < l2);
  }

  template <class V, class A>
  auto  tree<V, A>::cmpkey( const unsigned char* kp, size_t kl, const node& pn ) -> int
  {
    return cmpkey( kp, kl, pn.chars, pn.keylen() );
  }

  template <class V, class A>
  size_t  tree<V, A>::GetBufLen() const
  {
    return ptree != nullptr ? ptree->GetBufLen() : 3;
  }

  template <class V, class A>
  template <class S>
  S*      tree<V, A>::FetchFrom( S* s )
  {
    size_t  cchstr;
    size_t  arsize;

    if ( ptree != nullptr )
      ptree->Delete();

    if ( (s = ::FetchFrom( ::FetchFrom( s, cchstr ), arsize )) == nullptr )
      return nullptr;

    if ( cchstr == 0 && arsize == 0 )
      return s;

    ptree = node::Create( alloc, nullptr, cchstr );

    return ptree->FetchFrom( s, cchstr, arsize );
  }

  template <class V, class A>
  template <class P>
  void    tree<V, A>::PrintTree( P p ) const
  {
    if ( ptree != nullptr )
      ptree->PrintTree( p, 0 );
  }

  template <class V, class A>
  template <class O>
  O*      tree<V, A>::Serialize( O* o ) const
  {
    return ptree != nullptr ? ptree->Serialize( o ) : ::Serialize( ::Serialize( ::Serialize( o, 0 ), 0 ), 0 );
  }

  // dump::iterator implementation

  inline
  dump::iterator::iterator( const char* stored ): patval( nullptr )
    {
      if ( stored != nullptr )
      {
        atrace.push_back( GetPat( stored ) );

        if ( !atrace.back().bvalue )
          Tonext();
      }
    }

  inline
  dump::iterator::iterator(): patval( nullptr )
    {
    }

  inline
  dump::iterator::iterator( iterator&& it ):
      patkey( std::move( it.patkey ) ),
      patval( std::move( it.patval ) ),
      atrace( std::move( it.atrace ) ),
      achars( std::move( it.achars ) )
    {
      it.setkey();
    }

  inline
  typename dump::iterator&  dump::iterator::operator = ( iterator&& it )
    {
      patkey = std::move( it.patkey );
      patval = std::move( it.patval );
      atrace = std::move( it.atrace );
      achars = std::move( it.achars );
      return setkey();
    }

  inline
  auto  dump::iterator::key() const -> const patricia::key&
    {
      return patkey;
    }

  inline
  auto  dump::iterator::val() const -> const void*
    {
      assert( patval != nullptr );
      return patval;
    }

  inline
  typename dump::iterator&  dump::iterator::operator ++ ()
    {
      return Tonext();
    }

  inline
  bool  dump::iterator::operator == ( const iterator& it ) const
    {
      if ( atrace.size() != it.atrace.size() )
        return false;
      for ( size_t i = 0; i != atrace.size(); ++i )
        if ( atrace[i].dicptr != it.atrace[i].dicptr )
          return false;
      return true;
    }

  inline
  int   dump::iterator::CmpKey( const void* k1, size_t c1, const void* k2, size_t c2 )
    {
      size_t  cc = c1 <= c2 ? c1 : c2;
      int     rc = memcmp( k1, k2, cc );

      return rc != 0 ? rc : (c1 > cc) - (c1 < cc);
    }

  inline
  typename dump::iterator::patpos dump::iterator::GetPat( const char* stored )
    {
      patpos  thepat;
      size_t  sublen;

    // вычитать первый из вложенных узлов
      thepat.keyptr = ::FetchFrom( ::FetchFrom( stored, thepat.keylen ), thepat.nnodes );
      thepat.dicptr = thepat.keyptr + thepat.keylen;
      thepat.bvalue = (thepat.nnodes & 0x01) != 0;
      thepat.nnodes >>= 1;
      thepat.dicptr = ::FetchFrom( thepat.dicptr, sublen );
      thepat.endptr = thepat.dicptr + sublen;

      return thepat;
    }

  inline
  typename dump::iterator&  dump::iterator::MoveTo( const uint8_t* key, size_t len )
    {
      const uint8_t*  keyptr = key;
      const uint8_t*  keyend = key + len;
      size_t          ntrace;

    // пропустить уже совпадающую часть текущего ключа в итераторе с искомой последовательностью
      for ( ntrace = 0; ntrace != (size_t)atrace.size() && keyptr != keyend; keyptr += atrace[ntrace++].keylen )
      {
        if ( keyptr + atrace[ntrace].keylen > keyend )
          break;
        if ( memcmp( atrace[ntrace].keyptr, keyptr, atrace[ntrace].keylen ) != 0 )
          break;
      }

    // укоротить текущий ключ в итераторе и трассу к нему до текущей совпадающей последовательности
      while ( (size_t)atrace.size() != ntrace )
      {
        patpos& thepos = atrace.back();

        if ( atrace.size() > 1 )
          atrace[atrace.size() - 2].dicptr = thepos.endptr;
        atrace.pop_back();
      }

    // итеративно сместить позицию сканирования дерева на первый ключ, больше либо равный искомому
      while ( atrace.size() != 0 )
      {
        patpos& thepos = atrace.back();

      // если узел - последний, проверить остаток ключа и, если нашли, вернуть его
        if ( thepos.bvalue )
        {
          if ( keyptr == keyend ) thepos.bvalue = false;
            else atrace.clear();
          break;
        }

      // если узел имеет вложенные узлы, вычитать очередной вложенный узел и сравнить его ключ с искомым
        if ( thepos.nnodes != 0 )
        {
          assert( keyptr >= key );
          assert( thepos.keylen <= (size_t)(keyptr - key) );
          assert( memcmp( keyptr - thepos.keylen, thepos.keyptr, thepos.keylen ) == 0 );

          patpos  patnew = GetPat( thepos.dicptr );
          int     rescmp = CmpKey( patnew.keyptr, patnew.keylen, keyptr, keyend - keyptr );

          --thepos.nnodes;
            thepos.dicptr = patnew.endptr;

        // если ключ на следующем уровне меньше минимального искомого ключа, пропустить его
        // и перейти к следующему ключу этого уровня
          if ( rescmp >= 0 )  atrace.push_back( patnew );
            else continue;

        // если ключ следующего уровня больше искомого ключа, завершить поиск, перейдя на ветку
        // простого вычитывания пеового ключа (Tonext)
        // иначе ключи равны; перейти на следующий уровень вложенности поиска по дереву
          if ( rescmp > 0 ) return Tonext();
            else  keyptr += patnew.keylen;
        }
          else
        break;
      }

      return setkey();
    }

  inline
  typename dump::iterator&  dump::iterator::setkey()
    {
      auto  chrtop = achars.begin();
      auto  chrend = achars.end();

      for ( const auto& t: atrace )
        {
          auto  ofbase = chrtop - achars.begin();

          if ( ofbase + t.keylen > achars.size() )
          {
            achars.resize( (ofbase + t.keylen + 0xff) & ~0xff );

            chrtop = achars.begin() + ofbase;
            chrend = achars.end();
          }
          chrtop = std::copy( t.keyptr, t.keyptr + t.keylen, chrtop );
        }

      patkey = std::move( patricia::key( achars.data(), chrtop - achars.begin() ) );
      if ( chrtop - achars.begin() != 0 )
        patval = JumpOver( atrace.back().nnodes, atrace.back().dicptr );
      else
        patval = nullptr;
      return *this;
    }

  inline
  typename dump::iterator&  dump::iterator::Tonext()
    {
      while ( atrace.size() != 0 )
      {
        patpos& thepos = atrace.back();

        if ( thepos.bvalue )
        {
          thepos.bvalue = false;
          return setkey();
        }
        if ( thepos.nnodes != 0 )
        {
          patpos  patnew = GetPat( thepos.dicptr );

            thepos.dicptr = patnew.dicptr;
          --thepos.nnodes;
            atrace.push_back( patnew );
        }
          else
        {
          if ( atrace.size() > 1 )
            atrace[atrace.size() - 2].dicptr = thepos.endptr;
          atrace.pop_back();
        }
      }
      return setkey();
    }

  // dump implementation

  template <class _func_>
  int   dump::Select( const void* k, size_t l, _func_ f ) const
  {
    uint8_t       thekey[0x100];
    const char*   thedic;
    int           nchars;
    int           nnodes;
    int           sublen;

    if ( (thedic = ::FetchFrom( ::FetchFrom( ::FetchFrom( serial, nchars ), nnodes ), sublen )) == nullptr )
      return 0;

    assert( nnodes <= 513 );
    assert( nchars <= 256 * 4 );

    return Select( thedic, nchars, (const uint8_t*)k, l + (const uint8_t*)k, thedic + nchars, nnodes, f,
      thekey, thekey + sizeof(thekey) / sizeof(thekey[0]), thekey );
  }

  inline
  auto  dump::Search( const key& k ) const -> const char*
  {
    return Search( (const char*)k.getptr(), k.getlen(), serial );
  }

  inline
  auto  dump::Search( const char* thekey,
                      size_t      cchkey,
                      const char* thedic ) const -> const char*
  {
    size_t  nchars;
    size_t  nnodes;

    for ( thedic = ::FetchFrom( ::FetchFrom( thedic, nchars ), nnodes ); ; )
    {
      size_t  sublen;
      auto    hasval = (nnodes & 0x01) != 0;  nnodes >>= 1;

      if ( nnodes > 256 )
        throw std::logic_error( "node count may not be greater than 256" );

      // check key match
      if ( cchkey < nchars )
        return nullptr;

      for ( auto keyend = thedic + nchars; thedic != keyend; --cchkey )
        if ( *thedic++ != *thekey++ )
          return nullptr;

      // check if value
      if ( cchkey == 0 )
        return hasval ? JumpOver( nnodes, ::FetchFrom( thedic, sublen ) ) : nullptr;

      // loop over the nested nodes, select the node to contain the key
      for ( thedic = ::FetchFrom( thedic, sublen ); nnodes != 0; --nnodes )
      {
        size_t  cchars;
        size_t  cnodes;
        int     rescmp;

        thedic = ::FetchFrom( ::FetchFrom( thedic, cchars ), cnodes );
        rescmp = (uint8_t)*thekey - (uint8_t)*thedic;

        if ( rescmp > 0 )
        {
          thedic = ::FetchFrom( thedic + cchars, sublen );
          thedic += sublen;
        }
          else
        if ( rescmp == 0 )
        {
          nchars = cchars;
          nnodes = cnodes;
          break;
        }
          else
        return nullptr;
      }
      if ( nnodes == 0 )
        return nullptr;
    }
  }

    template <class _func_>
  int   dump::Select( const char* dicstr, int         diclen,
                      const char* keystr, const char* keyend,
                      const char* thedic, int         nnodes,
                      _func_      addptr,
                      char*       buftop,
                      char*       bufend,
                      char*       bufptr ) const
  {
    bool  bvalue = (nnodes & 1) != 0;
    int   nerror;

    assert( diclen >= 0 );
    assert( keyend >= keystr );

  // сравнить соответствующие друг другу фрагменты строки и шаблона
    while ( diclen > 0 && keystr < keyend && (*keystr == *dicstr || *keystr == '?') )
    {
      if ( buftop != nullptr )
      {
        if ( bufptr < bufend )  *bufptr++ = *dicstr;
          else return E2BIG;
      }
      ++dicstr;
      ++keystr;
      --diclen;
    }

  // если строка запроса отсканирована полностью:
  //  - если строка узла словаря исчерпана и есть данные, зарегистрировать их;
  //  - иначе оборвать сканирование
    if ( keystr == keyend )
      return diclen == 0 && bvalue ? addptr( buftop, bufptr - buftop, JumpOver( nnodes >> 1, thedic ) ) : 0;

    assert( keystr < keyend );

  // если в строке словаря остались символы, а шаблон - не звёздочка, оборвать сканирование
    if ( diclen > 0 )
    {
      if ( *keystr != '*' )
        return 0;

    // если символ шаблона '*', зайти рекурсивно, предположив соответствие 0..N символов фрагмента
      for ( auto nmatch = 0; nmatch <= diclen; ++nmatch )
      {
        if ( buftop != nullptr && nmatch > 0 )
        {
          if ( bufend > bufptr )  *bufptr++ = dicstr[nmatch - 1];
            else return E2BIG;
        }
        if ( (nerror = Select( dicstr + nmatch, diclen - nmatch, keystr + 1, keyend, thedic, nnodes, addptr, buftop, bufend, bufptr )) != 0 )
          return nerror;
      }
      return nnodes > 1 ? Select( dicstr + diclen, 0, keystr, keyend, thedic, nnodes, addptr, buftop, bufend, bufptr ) : 0;
    }
      else
  // если строка в узле словаря исчерпана, а у узла нету вложенных узлов, то соответствие
  // может быть только в случае '*', во всех остальных - обрыв сканирования
    {
      if ( nnodes <= 1 )
        return keyend - keystr == 1 && *keystr == '*' ? addptr( buftop, bufptr - buftop, thedic ) : 0;

      for ( nnodes >>= 1; nnodes-- > 0; )
      {
        const char* subdic;
        int         cchars;
        int         cnodes;
        int         curlen;

      // извлечь характеристики очередного узла
        thedic = ::FetchFrom( ::FetchFrom( thedic, cchars ), cnodes );
        subdic = ::FetchFrom( thedic + cchars, curlen );

        if ( (nerror = Select( thedic, cchars, keystr, keyend, subdic, cnodes, addptr, buftop, bufend, bufptr )) != 0 )
          return nerror;

        thedic = subdic + curlen;
      }
    }

    return 0;
  }

  inline
  auto  dump::JumpOver( int nnodes, const char* thedic ) -> const char*
  {
    while ( nnodes-- > 0 )
    {
      int   cchars;
      int   cnodes;
      int   curlen;

      thedic = ::FetchFrom( ::FetchFrom( thedic, cchars ), cnodes );
      thedic = ::FetchFrom( thedic + cchars, curlen );
      thedic = thedic + curlen;
    }
    return thedic;
  }

  /*
    Реализация рекурсивного сканера сериализованного дерева.

    Вызывает переданный примитив для каждой пары "ключ-сериализованное значение".
  */
  template <class A>
  const char* dump::scantree( const char* serial, std::vector<char>& achars, size_t keylen, A action )
  {
    size_t  cchstr;
    size_t  nitems;
    size_t  ccharr;

    if ( (serial = ::FetchFrom( ::FetchFrom( serial, cchstr ), nitems )) != nullptr )
    {
      const char* setptr;

      if ( cchstr != 0 )
      {
        if ( (size_t)achars.size() < keylen + cchstr )
          achars.resize( (keylen + cchstr + 0x0f) & ~0x0f );

        if ( (serial = ::FetchFrom( serial, keylen + achars.data(), cchstr )) == nullptr )
          return nullptr;
      }

      if ( (setptr = serial = ::FetchFrom( serial, ccharr )) == nullptr )
        return serial;

      for ( auto n = nitems >> 1; n != 0 && serial != nullptr; --n )
        serial = scantree( serial, achars, keylen + cchstr, action );

      if ( (nitems & 0x1) != 0 )
        action( achars, keylen + cchstr, serial, setptr + ccharr - serial );

      return setptr + ccharr;
    }
    return serial;
  }

  // sink<V>::value_t implementation

  template <class V>
  void  sink<V>::value_t::clear()
  {
    V*  pdel;

    if ( (pdel = pval) != nullptr )
      pval = nullptr, pdel->~V();
  }

  // sink<V>::chunk_t implementation

  template <class V>
  auto  sink<V>::chunk_t::GetBufLen() const ->size_t
  {
    auto  arrlen = (val.isset() ? 1 : 0) + cnt * 2;
    auto  ccjump = val.GetBufLen() + set.GetBufLen();

    if ( sub != nullptr )
    {
      arrlen += 2;
      ccjump += sub->GetBufLen();
    }

    return ::GetBufLen( key.size() ) + key.size()
         + ::GetBufLen( arrlen )
         + ::GetBufLen( ccjump ) + ccjump;
  }

  template <class V>
  template <class O>
  O*    sink<V>::chunk_t::Serialize( O* o ) const
  {
    auto  arrlen = (val.isset() ? 1 : 0) + cnt * 2;
    auto  ccjump = val.GetBufLen() + set.GetBufLen();

    if ( sub != nullptr )
    {
      arrlen += 2;
      ccjump += sub->GetBufLen();
    }

    o = set.Serialize( ::Serialize( ::Serialize( ::Serialize( ::Serialize( o, key.size() ), arrlen ),
      key.c_str(), key.size() ), ccjump ) );

    if ( sub != nullptr )
      o = sub->Serialize( o );

    return val.isset() ? val.Serialize( o ) : o;
  }

  template <class V>
  template <class chartype>
  auto  sink<V>::chunk_t::insert( chartype* k, size_t l ) -> chunk_t*
  {
    int     cmpres;
    size_t  equlen;

    std::tie( cmpres, equlen ) = lmatch( k, l );

    // check complete match node key
    if ( equlen == key.size() )
    {
      if ( l == equlen )
        throw std::logic_error( "sink must receive keys in increasing order" );

      if ( sub == nullptr )
        return (sub = chunk_p( new chunk_t( k + equlen, l - equlen ) )).get();

      if ( (chartype)sub->key.front() == k[equlen] )
        return sub->insert( k + equlen, l - equlen );

      // next character differs from next character in tree; move (possibly) existing
      // 'sub'node to serialized set and create the new one
      sub->Serialize( set.append().ptr() );
        ++cnt;
      return (sub = chunk_p( new chunk_t( k + equlen, l - equlen ) )).get();
    }
    if ( cmpres <= 0 )
      throw std::logic_error( "sink must receive keys in increasing order" );

    // two cases:
    //  - key is splitted to two keys;
    //  - match length, is zero, the key changes completely
    // have partial match; check if inserted key is strict greater than the node key
    // split the key in two
    {
      auto  curkey = key.c_str() + equlen;
      auto  curlen = key.size() - equlen;
      auto  arrlen = (val.isset() ? 1 : 0) + cnt * 2 + (sub != nullptr ? 2 : 0);
      auto  ccjump = val.GetBufLen() + set.GetBufLen() + (sub != nullptr ? sub->GetBufLen() : 0);
      auto  tohead = set.unwind(
        ::GetBufLen( curlen ) + curlen +
        ::GetBufLen( arrlen ) + ::GetBufLen( ccjump ) );
      auto  totail = set.append();

      ::Serialize( ::Serialize( ::Serialize( ::Serialize( tohead.ptr(),
        curlen ), arrlen ), curkey, curlen ), ccjump );
      if ( sub != nullptr )
        sub->Serialize( totail.ptr() );
      val.Serialize( totail.ptr() );
        cnt = 1;
    }
    key.resize( equlen );
    val.clear();

    return (sub = chunk_p( new chunk_t( k + equlen, l - equlen ) )).get();
  }

  template <class V>
  template <class chartype>
  auto  sink<V>::chunk_t::lmatch( chartype* k, size_t l ) -> std::tuple<int, int>
  {
    auto  kptr = (const unsigned char*)key.c_str();
    auto  kend = kptr + key.size();
    auto  mptr = (const unsigned char*)k;
    auto  mend = mptr + l;
    int   rcmp;

    for ( rcmp = 0; kptr != kend && mptr != mend && (rcmp = *mptr - *kptr) == 0; ++kptr, ++mptr )
      (void)NULL;

    return std::make_tuple( rcmp, kptr - (const unsigned char*)key.c_str() );
  }

  // sink<V> implementation

  template <class V>
  template <class chartype>
  auto  sink<V>::insert( chartype* k, size_t l ) -> chunk_t*
  {
    if ( pat == nullptr )
      pat = chunk_p( new chunk_t( (const char*)nullptr, 0 ) );
    return pat->insert( k, l );
  }

  template <class V>
  V*  sink<V>::Insert( const key& k, V&& v )         {  return insert( k.getptr(), k.getlen() )->setval( std::move( v ) );  }

  template <class V>
  V*  sink<V>::Insert( const key& k, const V& v )    {  return insert( k.getptr(), k.getlen() )->setval( v );  }

}}

namespace mtc {

  template <class V, class A>
  using patriciaTree = patricia::tree<V, A>;
  using patriciaDump = patricia::dump;

}

# endif  // __mtc_patricia_h__
