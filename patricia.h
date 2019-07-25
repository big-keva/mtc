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
# include <vector>

# if defined( TEST_PATRICIA )
#   include <stdio.h>
# endif  // TEST_PATRICIA

namespace mtc       {
namespace patricia  {

  class key
  {
    template <class V>  friend class tree;
                        friend class dump;

    const unsigned char*  ptr;
    size_t                len;

  public:     // construction
                              key();
    template <class chartype> key( const chartype*, size_t = (size_t)-1 );
    template <class chartype> key( const std::basic_string<chartype>& );

  public:     // key access
    auto    getptr() const -> const unsigned char*  {  return ptr;  }
    auto    getlen() const -> size_t                {  return len;  }

    auto    begin() const -> const unsigned char*   {  return getptr();  }
    auto    end()   const -> const unsigned char*   {  return getptr() + getlen();  }

  public:     // serialization
                        size_t  GetBufLen(      ) const {  return ::GetBufLen( len ) + len;  }
    template <class O>  O*      Serialize( O* o ) const {  return ::Serialize( ::Serialize( o, len ), ptr, len );  }

  public:     // compare
    bool  operator == ( const key& k ) const  {  return len == k.len && (ptr == k.ptr || std::equal( ptr, ptr + len, k.ptr ));  }
    bool  operator != ( const key& k ) const  {  return !(*this == k);  }

  };

}}

template <class O>
inline  O*      Serialize( O* o, const mtc::patricia::key& key )  {  return key.Serialize( o );  }
inline  size_t  GetBufLen(       const mtc::patricia::key& key )  {  return key.GetBufLen(   );  }

namespace mtc       {
namespace patricia  {

  namespace
  {
    class nothing {};
  }

// key implementation

  inline
  key::key(): ptr( nullptr ), len( 0 )
    {}

  template <class chartype>
  key::key( const chartype* s, size_t l ): ptr( (const unsigned char*)s ), len( 0 )
    {
      if ( (len = l) == (size_t)-1 && ptr != nullptr )
        for ( len = 0; ptr[len] != 0; ++len ) (void)NULL;
      len *= sizeof(chartype);
    }

  template <class chartype>
  key::key( const std::basic_string<chartype>& s ): ptr( (const unsigned char*)s.data() ), len( sizeof(chartype) * s.size() )
    {}

  template <class V = nothing>
  class tree
  {
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
      std::unique_ptr<node> _next;
      std::unique_ptr<node> _list;
      uint32_t              _sets;
      union
      {
        char                dummy;
        V                   value;
      };
      unsigned char         chars[1];

    public:
      node( const unsigned char*, size_t, std::unique_ptr<node>&& );
     ~node();

    public:
      static  auto  create( const unsigned char*, size_t, std::unique_ptr<node> = std::unique_ptr<node>() ) -> std::unique_ptr<node>;
      static  auto  fmatch( const unsigned char*, size_t, const unsigned char*, size_t ) -> size_t;

    public:
      auto  key_beg() const -> const unsigned char* {  return chars;  }
      auto  key_end() const -> const unsigned char* {  return chars + keylen();  }

    public:
            node*   search( const unsigned char* key, size_t len )        {  return search<node>( key, len, *this );  }
      const node*   search( const unsigned char* key, size_t len ) const  {  return search<node>( key, len, *this );  }

            node*   insert( const unsigned char*, size_t );
            auto    remove( const unsigned char*, size_t ) -> bool;

            bool    hasval() const    {  return (_sets & 0x80000000) != 0;  }
            size_t  keylen() const    {  return (_sets & ~0x80000000);  }
            void    setlen( size_t );

            void    delval();

            V*      getval()        {  return (_sets & 0x80000000) != 0 ? &value : nullptr;  }
      const V*      getval() const  {  return (_sets & 0x80000000) != 0 ? &value : nullptr;  }

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

  public:     // construction
    tree()  {}
    tree( tree&& );
    tree( const std::initializer_list<std::pair<key, V>>& );
    tree& operator = ( tree&& );
    tree( const tree& ) = delete;
    tree& operator = ( const tree& ) = delete;

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
    auto  begin()  const -> const_iterator {  return const_iterator( p_tree.get() );  }
    auto  begin()        ->       iterator {  return iterator( p_tree.get() );  }
    auto  end()    const -> const_iterator {  return const_iterator();  }
    auto  end()          ->       iterator {  return iterator();  }

  public:     // capacity
    auto  empty() const -> bool;

  public:     // modifiers
    void  clear();
    auto  erase( const key& ) -> size_t;
    template <class InputIt>
    void  insert( InputIt first, InputIt last );
    void  insert( std::initializer_list<std::pair<key, V>> );

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
    template <class act>  int   for_each( act action )        {  return p_tree != nullptr ? p_tree->for_each( action ) : 0;  }
    template <class act>  int   for_each( act action ) const  {  return p_tree != nullptr ? p_tree->for_each( action ) : 0;  }

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
    std::unique_ptr<node> p_tree;

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
                        const char* thedic,
                        size_t      nnodes ) const;
    template <class _func_>
    int         Select( const char* dicstr, int         diclen,
                        const char* keystr, const char* keyend,
                        const char* thedic, int         nnodes,
                        _func_      insert,
                        char*       buftop,
                        char*       bufend,
                        char*       bufptr ) const;

  protected:  // internals
    const char* JumpOver( int nnodes, const char* dicptr ) const;

  protected:  // variables
    const char* serial;

  };

  // patricia implementation

  template <class V>
  tree<V>::node::node( const unsigned char* key, size_t len, std::unique_ptr<node>&& nex ): _next( std::move( nex ) ), _sets( (uint32_t)len )
    {
      if ( key != nullptr )
        memcpy( chars, key, len );
    }

  template <class V>
  tree<V>::node::~node()
    {
      if ( hasval() )
        value.~V();
    }

  template <class V>
  auto  tree<V>::node::create( const unsigned char* key, size_t len, std::unique_ptr<node> nex ) -> std::unique_ptr<typename tree<V>::node>
    {
      size_t    minlen = len != 0 ? len : 1;
      size_t    cchstr = (minlen + 0x0f) & ~0x0f;
      size_t    nalloc = sizeof(node) - sizeof(node::chars) + cchstr;

      return std::move( std::unique_ptr<node>( new ( new char[nalloc] ) node( key, len, std::move( nex ) ) ) );
    }

  template <class V>
  size_t  tree<V>::node::fmatch( const unsigned char* k_1, size_t l_1, const unsigned char* k_2, size_t l_2 )
    {
      auto  k1e = k_1 + l_1;
      auto  k2e = k_2 + l_2;
      auto  k1p = k_1;

      while ( k1p < k1e && k_2 < k2e && *k1p == *k_2++ )
        ++k1p;

      return k1p - k_1;
    }

  template <class V>
  auto  tree<V>::node::remove( const unsigned char* key, size_t len ) -> bool
    {
      auto  keychr = len != 0 ? *key : 0;   // safe 'get' - no SIGFAULT
      auto  pplist = &_list;

    // если длина ключа нулевая, значит, элемент найден и надо удалить ассоциированные с ним данные;
    // проверяется сама возможность этого события;
    // если есть ассоциированные данные, вернуть true
      if ( len == 0 )
      {
        if ( !hasval() )  throw std::logic_error( "empty element begin deleted" );
          else delval();

      // вернуть true, если элемент не имеет вложенных элементов или имеет единственный
        return _list == nullptr || _list->_next == nullptr;
      }

    // найти элемент в списке вложенных подключей, который может содержать искомый ключ;
    // если такой есть, сравнить собственно подключ с текущим фрагментом ключа
      while ( *pplist != nullptr && keychr > (*pplist)->chars[0] )
        pplist = &(*pplist)->_next;

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
            if ( (*pplist)->_list == nullptr )
            {
              *pplist = std::move( (*pplist)->_next );
              return !hasval() && _list == nullptr;
            }

            if ( (*pplist)->_list->_next != nullptr )
              throw std::logic_error( "invalid patricia::node::remove() returns true" );

            auto  palloc = node::create( nullptr, (*pplist)->keylen() + (*pplist)->_list->keylen(),
              std::move( (*pplist)->_next ) );

            if ( (*pplist)->_list->hasval() )
              palloc->setval( std::move( (*pplist)->_list->value ) );

            std::copy( (*pplist)->_list->key_beg(), (*pplist)->_list->key_end(),
              std::copy( (*pplist)->key_beg(), (*pplist)->key_end(),
                palloc->chars ) );

            *pplist = std::move( palloc );
          }
          return false;
        }
      }
      return false;
    }

  template <class V>
  typename tree<V>::node* tree<V>::node::insert( const unsigned char* thekey, size_t cchkey )
    {
    // если поисковый ключ отсканирован полностью, навершить поиск
      if ( cchkey == 0 )
        return this;

    // найти первый элемент во вложенном массиве, у которого первый символ ключа больше
    // либо равен первому символу вставляемого ключа;
    // если таковой не найден, вставить новый узел с требуемым значением полного ключа
    // и вернуть его;
      auto  keychr = *thekey;
      auto  pprepl = &_list;

      while ( *pprepl != nullptr && keychr > (*pprepl)->chars[0] )
        pprepl = &(*pprepl)->_next;

      if ( *pprepl == nullptr || keychr != (*pprepl)->chars[0] )
        return (*pprepl = std::move( create( thekey, cchkey, std::move( *pprepl ) ) )).get();

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
          auto  palloc = create( thekey, lmatch, std::move( (*pprepl)->_next ) );

          memmove( (*pprepl)->chars, (*pprepl)->chars + lmatch, curlen - lmatch );
            (*pprepl)->setlen( curlen - lmatch );
            (*pprepl)->_next = nullptr;

          palloc->_list = std::move( *pprepl );
          *pprepl = std::move( palloc );

          return (*pprepl).get();
        }

    // есть хвост и от добавляемого ключа, и от текущего элемента
      assert( cchkey > lmatch && curlen > lmatch );

      auto  palloc = create( thekey, lmatch, std::move( (*pprepl)->_next ) );
      auto  p_tail = create( thekey + lmatch, cchkey - lmatch );
      auto  rescmp( thekey[lmatch] - (*pprepl)->chars[lmatch] );

      memmove( (*pprepl)->chars, (*pprepl)->chars + lmatch, curlen - lmatch );
        (*pprepl)->setlen( curlen - lmatch );
        (*pprepl)->_next = nullptr;

      if ( rescmp < 0 )
        (palloc->_list = std::move( p_tail ))->_next = std::move( *pprepl );
      else
        (palloc->_list = std::move( *pprepl ))->_next = std::move( p_tail );

      return (*pprepl = std::move( palloc ))->insert( thekey + lmatch, cchkey - lmatch );
    }

  template <class V>
  void  tree<V>::node::delval()
    {
      if ( hasval() )
        value.~V();
      _sets &= ~0x80000000;
    }

  template <class V>
  void  tree<V>::node::setlen( size_t l )
    {
      size_t  minlen = (_sets & ~0x80000000) != 0 ? _sets & ~0x80000000 : 1;
      size_t  maxlen = (minlen + 0x0f) & ~0x0f;   assert( l <= maxlen );

      _sets = (_sets & 0x80000000) | (uint32_t)l;
    }

  template <class V>
  V*    tree<V>::node::setval( const V& v )
    {
      if ( hasval() )
        value.~V();
      _sets |= 0x80000000;
        return new( &value ) V( v );
    }

  template <class V>
  V*    tree<V>::node::setval( V&& v )
    {
      if ( hasval() )
        value.~V();
      _sets |= 0x80000000;
        return new( &value ) V( std::move( v ) );
    }

  template <class V>
  template <class act, class slf>
  int   tree<V>::node::for_impl( act action, slf& r_node )
    {
      auto  pvalue = r_node.getval();
      int   nerror;

      if ( pvalue != nullptr )
        if ( (nerror = action( *pvalue )) != 0 )
          return nerror;

      for ( auto p = r_node._list.get(); p != nullptr; p = p->_next.get() )
        if ( (nerror = p->for_each( action )) != 0 )
          return nerror;

      return 0;
    }

  template <class V>
  size_t  tree<V>::node::GetBufLen() const
    {
      size_t    arsize = 0;
      size_t    ccharr = 0;
      size_t    curlen = keylen();
      const V*  pvalue = getval();

      for ( auto p = _list.get(); p != nullptr; p = p->_next.get(), arsize += 2 )
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

  template <class V>
  template <class S>
  S*  tree<V>::node::FetchFrom( S* s, size_t cchstr, size_t arsize )
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

      for ( auto p = &_list; nitems != 0; p = &(*p)->_next, --nitems )
        {
          size_t  sublen;
          size_t  subarr;

          if ( (s = ::FetchFrom( ::FetchFrom( s, sublen ), subarr )) == nullptr )
            return nullptr;

          *p = node::create( nullptr, sublen );

          if ( (s = (*p)->FetchFrom( s, sublen, subarr )) == nullptr )
            return nullptr;
        }

      return bvalue ? ::FetchFrom( s, *setval( V() ) ) : s;
    }

  template <class V>
  template <class P>
  void  tree<V>::node::PrintTree( P print, size_t before ) const
    {
      for ( auto p = _list.get(); p != nullptr; p = p->_next.get() )
        {
          auto  thekey = p->chars;
          auto  cchkey = p->keylen();
          auto  nwrite = std::min( cchkey, (size_t)(0x20 - 3) );

          for ( size_t i = 0; i < before; ++i )
            print( ' ' );

          for ( size_t i = 0; i < nwrite; ++i )
            print( thekey[i] );

          if ( cchkey > 0x20 - 3 )
            {
              print( '.' );  print( '.' );  print( '.' );
            }

          print( '\n' );

          p->PrintTree( print, before + 2 );
        }
    }

  template <class V>
  template <class O>
  O*  tree<V>::node::Serialize( O* o ) const
    {
      size_t    arsize = 0;
      size_t    ccharr = 0;
      size_t    curlen = keylen();
      const V*  pvalue = getval();

      for ( auto p = _list.get(); p != nullptr; p = p->_next.get(), arsize += 2 )
        {
          ccharr += p->GetBufLen();
        }

      if ( pvalue != nullptr )
        {
          arsize |= 1;
        }

    // store key size, array size and key
      if ( (o = ::Serialize( ::Serialize( ::Serialize( o, curlen ), arsize ), chars, curlen )) == nullptr )
        return nullptr;

    // write the array and value size in bytes
      if ( (o = ::Serialize( o, ccharr + (pvalue != nullptr ? ::GetBufLen( *pvalue ) : 0) )) == nullptr )
        return nullptr;

      for ( auto p = _list.get(); p != nullptr; p = p->_next.get() )
        {
          if ( (o = p->Serialize( o )) == nullptr )
            return nullptr;
        }

      return pvalue != nullptr ? ::Serialize( o, *pvalue ) : o;
    }

  template <class V>
  template <class R, class S>
  auto  tree<V>::node::search( const unsigned char* thekey, size_t cchkey, S& self ) -> R*
    {
      for ( auto  p_this = &self; ; )
      {
        if ( cchkey == 0 )
          return p_this;

      // найти элемент во вложенном массиве, у которого первый символ ключа равен первому символу вставляемого ключа
        auto  keychr = *thekey;
        auto  p_scan = p_this->_list.get();

        while ( p_scan != nullptr && keychr > p_scan->chars[0] )
          p_scan = p_scan->_next.get();

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

  template <class V>
  template <class value, class nodes>
  tree<V>::base_iterator<value, nodes>::base_iterator( nodes* p ): patval( nullptr )
    {
      if ( p != nullptr )
      {
        atrace.push_back( p );
        get_lo();
      }
    }

  template <class V>
  template <class value, class nodes>
  tree<V>::base_iterator<value, nodes>::base_iterator(): patval( nullptr )
    {
    }

  template <class V>
  template <class value, class nodes>
  tree<V>::base_iterator<value, nodes>::base_iterator( base_iterator&& it ):
    patkey( std::move( it.patkey ) ), patval( std::move( it.patval ) ),
      atrace( std::move( it.atrace ) ),
      achars( std::move( it.achars ) )
    {
    }

  template <class V>
  template <class value, class nodes>
  tree<V>::base_iterator<value, nodes>& tree<V>::base_iterator<value, nodes>::operator = ( base_iterator&& it )
    {
      patkey = std::move( it.patkey );
      patval = std::move( it.patval );
      atrace = std::move( it.atrace );
      achars = std::move( it.achars );
      return *this;
    }

  template <class V>
  template <class value, class nodes>
  auto  tree<V>::base_iterator<value, nodes>::key() const -> const patricia::key&
    {
      return patkey;
    }

  template <class V>
  template <class value, class nodes>
  auto  tree<V>::base_iterator<value, nodes>::val() const -> value&
    {
      assert( patval != nullptr );
      return *patval;
    }

  template <class V>
  template <class value, class nodes>
  tree<V>::base_iterator<value, nodes>& tree<V>::base_iterator<value, nodes>::operator ++ ()
    {
      while ( atrace.size() != 0 )
      {
        auto  p_node = atrace.back()->_list.get();

      // если у узла есть вложенные элементы, максимально продвинуться вглубь дерева,
      // но не дальше первого найденного элемента со значением
        if ( p_node != nullptr )
        {
          do atrace.push_back( p_node );
            while ( !p_node->hasval() && (p_node = p_node->_list.get()) != nullptr );
        }
          else
      // иначе, если вложенных элементов нет, перейти к следующему элементу в списке
      // того же горизонтального уровня
        if ( (p_node = atrace.back()->_next.get()) != nullptr )
        {
          atrace.back() = p_node;
        }
          else
      // отмотать вниз по дереву с переходом на следующий элемент до успешного ключа
        {
          do atrace.pop_back();
            while ( atrace.size() != 0 && atrace.back()->_next == nullptr );

          if ( atrace.size() != 0 ) atrace.back() = atrace.back()->_next.get();
            else continue;
        }

      // если найден узел со значением, вернуть его
        if ( atrace.back()->hasval() )
          return setkey();
      }
      atrace.clear();
      return setkey();
    }

  template <class V>
  template <class value, class nodes>
  bool  tree<V>::base_iterator<value, nodes>::operator == ( const base_iterator& it ) const
    {
      return patkey == it.patkey && atrace.size() == it.atrace.size()
        && std::equal( atrace.begin(), atrace.end(), it.atrace.begin() );
    }

  template <class V>
  template <class value, class nodes>
  tree<V>::base_iterator<value, nodes>&  tree<V>::base_iterator<value, nodes>::setkey()
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

  template <class V>
  template <class value, class nodes>
  tree<V>::base_iterator<value, nodes>&  tree<V>::base_iterator<value, nodes>::get_lo()
    {
      while ( atrace.size() != 0 && !atrace.back()->hasval() )
      {
        auto  list = atrace.back()->_list.get();

        if ( list != nullptr )  atrace.push_back( list );
          else atrace.clear();
      }

      if ( atrace.size() != 0 && !atrace.back()->hasval() )
        atrace.clear();

      return setkey();
    }

  // patricia implementation

  template <class V>
  tree<V>::tree( tree&& p ):
    p_tree( std::move( p.p_tree ) ) {}

  template <class V>
  tree<V>::tree( const std::initializer_list<std::pair<key, V>>& list )
    {
      for ( auto& it: list )
        Insert( it.first, it.second );
    }

  template <class V>
  tree<V>& tree<V>::operator = ( tree&& p )
    {
      p_tree = std::move( p.p_tree );
      return *this;
    }

  template <class V>
  bool  tree<V>::Delete( const key& k )
    {
      if ( p_tree != nullptr && p_tree->remove( k.ptr, k.len ) )
        p_tree.reset();
      return true;
    }

  template <class V>
  auto  tree<V>::empty() const -> bool
    {
      return p_tree != nullptr && (p_tree->_next != nullptr || p_tree->_list != nullptr);
    }

  template <class V>
  void  tree<V>::clear()
    {
      p_tree.reset();
    }

  template <class V>
  auto  tree<V>::erase( const key& k ) -> size_t
    {
      return Delete( k ) ? 1 : 0;
    }

  template <class V>
  template <class InputIt>
  void  tree<V>::insert( InputIt first, InputIt end )
    {
      for ( ; first != end; ++first )
        Insert( first->first, first->second );
    }

  template <class V>
  void  tree<V>::insert( std::initializer_list<std::pair<key, V>> list )
    {
      return insert( list.begin(), list.end() );
    }

  template <class V>
  V*    tree<V>::insert( const key& k, const V& v )
    {
      node* pfound;

      if ( p_tree == nullptr )
        p_tree = std::unique_ptr<node>( node::create( nullptr, 0 ) );

      return (pfound = p_tree->insert( k.ptr, k.len )) != nullptr ?
        pfound->setval( v ) : nullptr;
    }

  template <class V>
  V*    tree<V>::insert( const key& k, V&& v )
    {
      node* pfound;

      if ( p_tree == nullptr )
        p_tree = std::unique_ptr<node>( node::create( nullptr, 0 ) );

      return (pfound = p_tree->insert( k.ptr, k.len )) != nullptr ?
        pfound->setval( std::move( v ) ) : nullptr;
    }

  template <class V>
  template <class res, class slf>
  res*  tree<V>::search( const key& k, slf& s )
    {
      if ( s.p_tree != nullptr )
        {
          auto  pfound = s.p_tree->search( k.ptr, k.len );

          return pfound != nullptr ? pfound->getval() : nullptr;
        }
      return nullptr;
    }

  template <class V>
  template <class itr, class slf>
  auto  tree<V>::findit( const key& key, slf& self ) -> itr
    {
      itr   it;
      auto  pk = key.ptr;
      auto  cc = key.len;
      auto  pn = self.p_tree.get();

      for ( it.atrace.push_back( pn ); pn != nullptr; )
      {
        auto    keychr = cc != 0 ? *pk : '\0';
        auto    p_scan = pn->_list.get();
        size_t  l_frag;

        if ( cc == 0 )
          return pn->hasval() ? std::move( it.setkey() ) : self.end();

        while ( p_scan != nullptr && keychr > p_scan->chars[0] )
          p_scan = p_scan->_next.get();

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

  template <class V>
  template <class itr, class slf>
  auto  tree<V>::lbound( const key& key, slf& self ) -> itr
    {
      itr   it;
      auto  pk = key.ptr;
      auto  cc = key.len;

      if ( self.p_tree != nullptr ) it.atrace.push_back( self.p_tree.get() );
        else return self.end();

      while ( it.atrace.size() != 0 )
      {
        if ( cc > 0 )
        {
          auto  p_scan = it.atrace.back()->_list.get();
          int   rescmp = 0;
          
          while ( p_scan != nullptr && (rescmp = cmpkey( pk, cc, *p_scan )) > 0 )
            p_scan = p_scan->_next.get();

          if ( p_scan == nullptr )
          {
            while ( it.atrace.size() != 0 )
            {
              if ( (it.atrace.back() = it.atrace.back()->_next.get()) == nullptr )  it.atrace.pop_back();
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

  template <class V>
  template <class itr, class slf>
  auto  tree<V>::ubound( const key& key, slf& self ) -> itr
    {
      auto  it = lbound( key, self );

      while ( it != self.end() && it.key() == key )
        ++it;

      return std::move( it );
    }

  template <class V>
  auto  tree<V>::cmpkey( const unsigned char* k1, size_t l1, const unsigned char* k2, size_t l2 ) -> int
    {
      auto  cc = std::min( l1, l2 );
      auto  rc = memcmp( k1, k2, cc );

      return rc != 0 ? rc : (cc > l2) - (cc < l2);
    }

  template <class V>
  auto  tree<V>::cmpkey( const unsigned char* kp, size_t kl, const node& pn ) -> int
    {
      return cmpkey( kp, kl, pn.chars, pn.keylen() );
    }

  template <class V>
  size_t  tree<V>::GetBufLen() const
    {
      return p_tree != nullptr ? p_tree->GetBufLen() : 3;
    }

  template <class V>
  template <class S>
  S*      tree<V>::FetchFrom( S* s )
    {
      size_t  cchstr;
      size_t  arsize;

      p_tree = nullptr;

      if ( (s = ::FetchFrom( ::FetchFrom( s, cchstr ), arsize )) == nullptr )
        return nullptr;

      if ( cchstr == 0 && arsize == 0 )
        return s;

      p_tree = node::create( nullptr, cchstr );

      return p_tree->FetchFrom( s, cchstr, arsize );
    }

  template <class V>
  template <class P>
  void    tree<V>::PrintTree( P p ) const
    {
      if ( p_tree != nullptr )
        p_tree->PrintTree( p, 0 );
    }

  template <class V>
  template <class O>
  O*      tree<V>::Serialize( O* o ) const
    {
      return p_tree != nullptr ? p_tree->Serialize( o ) : ::Serialize( ::Serialize( ::Serialize( o, 0 ), 0 ), 0 );
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
  int           dump::Select( const void* k, size_t l, _func_ f ) const
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
  const char* dump::Search( const key& k ) const
  {
    const char* thedic;
    size_t      nchars;
    size_t      nnodes;
    size_t      sublen;
    auto        keyptr = k.ptr;
    auto        keyend = k.ptr + k.len;

    if ( (thedic = ::FetchFrom( ::FetchFrom( serial, nchars ), nnodes )) == nullptr )
      return nullptr;

    assert( nnodes <= 513 );
    assert( nchars <= 256 * 4 );

    if ( k.len < nchars )
      return nullptr;

    for ( auto dicend = thedic + nchars; thedic < dicend; )
      if ( *keyptr++ != *thedic++ )
        return nullptr;

    if ( (thedic = ::FetchFrom( thedic, sublen )) == nullptr )
      return nullptr;

    return Search( (const char*)keyptr, keyend - keyptr, thedic, nnodes );
  }

  template <class _func_>
  int           dump::Select( const char* dicstr, int         diclen,
                                      const char* keystr, const char* keyend,
                                      const char* thedic, int         nnodes,
                                      _func_      addptr,
                                      char*       buftop,
                                      char*       bufend,
                                      char*       bufptr ) const
  {
    bool    bvalue = (nnodes & 1) != 0;
    int     nerror;

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
        const uint8_t* subdic;
        int           cchars;
        int           cnodes;
        int           curlen;

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
  const char* dump::Search( const char* thekey,
                                    size_t      cchkey,
                                    const char* thedic,
                                    size_t      nnodes ) const
  {
    bool    bvalue = (nnodes & 1) != 0;
    uint8_t  chfind;

  // если строка кончилась, то узел должен иметь значение
    if ( cchkey == 0 )
      return bvalue ? JumpOver( (int)(nnodes >> 1), thedic ) : nullptr;

    assert( cchkey > (size_t)0 );

    for ( chfind = *thekey, nnodes >>= 1; nnodes-- > 0; )
    {
      size_t  cchars;
      size_t  cnodes;
      size_t  curlen;
      int     rescmp;

    // извлечь характеристики очередного узла
      thedic = ::FetchFrom( ::FetchFrom( thedic, cchars ), cnodes );

    // проверить на совпадение
      if ( (rescmp = (unsigned char)chfind - (unsigned char)*thedic) < 0 )
        return nullptr;

    // сравнить строку с текущим элементом; если не совпадает, завершить поиск
      if ( rescmp == 0 )
      {
        if ( cchkey < cchars )
          return nullptr;
        while ( cchars > 0 && *thedic++ == *thekey++ )
          {  --cchars;  --cchkey;  }

      // есть совпадение, извлечь размер вложенного массива и значения
        return cchars == 0 ? Search( thekey, cchkey, ::FetchFrom( thedic, curlen ), cnodes ) : nullptr;
      }

    // извлечь вложенный размер
      thedic = ::FetchFrom( thedic + cchars, curlen ) + curlen;
    }

    return nullptr;
  }

  inline
  const char* dump::JumpOver( int nnodes, const char* thedic ) const
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

}}

namespace mtc {

  template <class V>
  using patriciaTree = patricia::tree<V>;
  using patriciaDump = patricia::dump;

}

# endif  // __mtc_patricia_h__
