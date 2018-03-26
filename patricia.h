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
# include "platform.h"
# include "autoptr.h"
# include "array.h"

# if defined( TEST_PATRICIA )
#   include <stdio.h>
# endif  // TEST_PATRICIA

namespace mtc
{

  struct patricia
  {
    struct key
    {
      const char* ptr;
      size_t      len;

    public:
      key(): ptr( nullptr ), len( 0 ) {}
      key( const char* p, size_t l ): ptr( p ), len( l )  {}

    };

    template <class chartype = char>
    static  key make_key( const chartype* k, size_t l ) {  return {  (const char*)k, l * sizeof(*k)  };  }

  };

}

template <class O>
inline  O*      Serialize( O* o, const mtc::patricia::key& k )  {  return ::Serialize( ::Serialize( o, k.len ), k.ptr, k.len );  }
inline  size_t  GetBufLen( const mtc::patricia::key& k )        {  return ::GetBufLen( k.len ) + k.len;  }

namespace mtc
{

  template <class V = patricia::key, class M = mtc::def_alloc>
  class patriciaTree: public patricia
  {
    class iterator;

    class pat_node
    {
      friend class iterator;

      pat_node* p_next = nullptr;
      pat_node* p_list = nullptr;
      uint32_t  uflags;
      char      cvalue[sizeof(V)];
      char      strkey[1];

    public:
      pat_node( const char* key, size_t len, pat_node* nex );
     ~pat_node();

    public:
      static  pat_node* create( const char*, size_t, M&, pat_node* next = nullptr );
      static  size_t    fmatch( const char*, size_t, const char*, size_t );

    public:
            void      delmem( M& );

            pat_node* search( const char* key, size_t len );
      const pat_node* search( const char* key, size_t len ) const;

            pat_node* insert( const char* key, size_t len, M& mem );
            pat_node* remove( const char* key, size_t len, M& mem );

            bool      hasval() const    {  return (uflags & 0x80000000) != 0;  }
            size_t    keylen() const    {  return (uflags & ~0x80000000);  }
            void      setlen( size_t );

            void      delval();

            V*        getval()        {  return (uflags & 0x80000000) != 0 ? (      V*)cvalue : nullptr;  }
      const V*        getval() const  {  return (uflags & 0x80000000) != 0 ? (const V*)cvalue : nullptr;  }

            V*        setval( const V& );
            V*        setval( V&& );

    public:     // serialization
                          size_t  GetBufLen() const;
      template <class S>  S*      FetchFrom( S*, M&, size_t, size_t );
      template <class O>  O*      Serialize( O* ) const;
      template <class P>  void    PrintTree( P, size_t ) const;

    public:
      template <class A>  int     for_each( A action )        {  return for_impl( *this, action );  }
      template <class A>  int     for_each( A action ) const  {  return for_impl( *this, action );  }

    protected:
      template <class N, class A>
                  static  int     for_impl( N&, A );
    };

    class pat_safe
    {
      M&        mem;
      pat_node* ptr;

    public:
      pat_safe( M& m, pat_node* p = nullptr ): mem( m ), ptr( p ) {}
      pat_safe( pat_safe&& p ): mem( p.mem ), ptr( p.ptr )  {  p.ptr = nullptr;  }
     ~pat_safe()  {  replace( nullptr );  }
      pat_safe& operator = ( pat_node* p )  {  return (replace( p ), *this);  }
      pat_safe& operator = ( pat_safe&& p ) {  return (replace( p.ptr ),  p.ptr = nullptr, *this);  }
      pat_safe( const pat_safe& ) = delete;
      pat_safe& operator = ( const pat_safe& ) = delete;

    public:
      operator const pat_node*  () const  {  return ptr;  }
      const pat_node* operator -> () const {  return ptr;  }
      operator pat_node*  ()  {  return ptr;  }
      pat_node* operator -> () {  return ptr;  }
      pat_node* release() {  auto p = ptr;  ptr = nullptr;  return p;  }

    protected:
      pat_node* replace( pat_node* with )
        {
          if ( ptr != nullptr )
            ptr->delmem( mem );
          return ptr = with;
        }

    };

  public:     // key iterator
    class iterator: protected key
    {
      friend class patriciaTree;

    protected:
      iterator( const pat_node* );
      iterator( const iterator& ) = delete;
      iterator& operator = ( const iterator& ) = delete;

      iterator& setkey();

    public:
      iterator();
      iterator( iterator&& );
      iterator& operator = ( iterator&& );

    public:
      iterator  operator ++ ( int ) = delete;
      iterator& operator ++ ();
      operator const key* () const  {  return this;  }
      const key* operator -> () const {  return this;  }
      bool  operator == ( const iterator& ) const;
      bool  operator != ( const iterator& it ) const  {  return !operator == ( it );  }

    protected:
      using pattrace = array<const pat_node*>;
      using pastring = array<char>;
      
      pattrace  atrace;
      pastring  keybuf;

    };

  public:     // construction
    patriciaTree(): p_tree( memman )  {}
    patriciaTree( M& m ): memman( m ), p_tree( memman ) {}
    patriciaTree( patriciaTree&& );
    patriciaTree& operator = ( patriciaTree&& );
    patriciaTree( const patriciaTree& ) = delete;
    patriciaTree& operator = ( const patriciaTree& ) = delete;

  public:     // API
                                            void  Delete( const key& k );
                                            V*    Insert( const key& k, const V& v = V() )  {  return insert( k, v );  }
                                            V*    Insert( const key& k, V&& v )             {  return insert( k, v );  }
                                      const V*    Search( const key& k ) const              {  return search<const V>( k, *this );  }
                                            V*    Search( const key& k )                    {  return search<      V>( k, *this );  }

    template <class chartype = char>        void  Delete( const chartype* k, size_t l )                   {  return Delete( make_key( k, l ) );  }
    template <class chartype = char>        V*    Insert( const chartype* k, size_t l, const V& v = V() ) {  return Insert( make_key( k, l ), v );  }
    template <class chartype = char>        V*    Insert( const chartype* k, size_t l, V&& v )            {  return Insert( make_key( k, l ), v );  }
    template <class chartype = char>  const V*    Search( const chartype* k, size_t l ) const             {  return search<const V>( make_key( k, l ), *this );  }
    template <class chartype = char>        V*    Search( const chartype* k, size_t l )                   {  return search<      V>( make_key( k, l ), *this );  }

  public:     // iterator
    iterator  begin() const {  return iterator( (const pat_node*)p_tree );  }
    iterator  end() const {  return iterator();  }

  public:     // iterator
    template <class A>  int     for_each( A action )        {  return p_tree != nullptr ? p_tree->for_each( action ) : 0;  }
    template <class A>  int     for_each( A action ) const  {  return p_tree != nullptr ? p_tree->for_each( action ) : 0;  }

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class S>  S*      FetchFrom( S* );
    template <class P>  void    PrintTree( P  ) const;
    template <class O>  O*      Serialize( O* ) const;

  protected:
    template <class arg>                    V*    insert( const key& k, arg  v );
    template <class res, class slf> static  res*  search( const key& k, slf& s );

  protected:  // var
    M         memman;
    pat_safe  p_tree;

  };

  class patriciaDump: public patricia
  {
    enum
    {
      asterisk = 0x0100 + '*',
      question = 0x0100 + '?'
    };

  public:     // iterator

    class iterator: protected key
    {
      friend class patriciaDump;

      struct  patpos
      {
        const char* dicptr;
        const char* endptr;
        size_t      keylen;
        size_t      nnodes;
        bool        bvalue;
      };

      array<patpos> atrace;
      array<char>   keybuf;

    protected:  // first initialization constructor
      iterator( const char* );
      iterator( const iterator& ) = delete;
      iterator& operator = ( const iterator& ) = delete;

    public:     // construction
      iterator();
      iterator( iterator&& );
      iterator& operator = ( iterator&& );

    public:
      iterator& operator ++ ()  {  return Tonext();  }
      iterator  operator ++ (int) = delete;
      operator const key* () const  {  return this;  }
      const key* operator -> () const {  return this;  }
      bool  operator == ( const iterator& it ) const;
      bool  operator != ( const iterator& it ) const  {  return !operator == ( it );  }

    public:     // API
      key   Move( const key& );
      template <class chartype>
      key   Move( const chartype* k, size_t l )
        {
          return Move( make_key( k, l ) );
        }

    protected:
      patpos    GetPat( const char* );
      iterator& setkey()  {  ptr = (const char*)keybuf; len = (size_t)keybuf.size();  return *this;  }
      iterator& Tonext();

    };

  public:     // construction
    patriciaDump( const void* p ): serial( (const char*)p ) {}
      
  public:     // search
                              const char* Search( const key& ) const;
    template <class chartype> const char* Search( const chartype* k, size_t l ) const {  return Search( key( (const char*)k, l * sizeof(*k) ) );  }

    template <class _func_>   int         Select( const void*, size_t, _func_ ) const;

  public:     // iterator
    iterator  begin() const {  return iterator( serial );  }
    iterator  end() const {  return iterator();  }

  public:     // iterator
    template <class A>  int     for_each( A action ) const;

  protected:
    template <class A>
    static  const char*         scantree( const char*, array<char>&, size_t, A );
    
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

  template <class V, class M>
  patriciaTree<V, M>::pat_node::pat_node( const char* key, size_t len, pat_node* nex ): uflags( len ), p_next( nex )
    {
      if ( key != nullptr )
        memcpy( strkey, key, len );
    }

  template <class V, class M>
  patriciaTree<V, M>::pat_node::~pat_node()
    {
      if ( uflags & 0x80000000 )
        ((V*)cvalue)->~V();
    }

  template <class V, class M>
  void    patriciaTree<V, M>::pat_node::delmem( M& m )
    {
      if ( p_list != nullptr )
        p_list->delmem( m );
      if ( p_next != nullptr )
        p_next->delmem( m );
      mtc::deallocate_with( m, this );
    }

  template <class V, class M>
  typename patriciaTree<V, M>::pat_node* patriciaTree<V, M>::pat_node::create( const char* key, size_t len, M& mem, pat_node* next )
    {
      pat_node* palloc;
      size_t    minlen = len != 0 ? len : 1;
      size_t    cchstr = (minlen + 0x0f) & ~0x0f;
      size_t    nalloc = sizeof(*palloc) - sizeof(palloc->strkey) + cchstr;

      if ( (palloc = (pat_node*)mem.alloc( nalloc )) != nullptr )
        new( palloc ) pat_node( key, len, next );
      return palloc;
    }

  template <class V, class M>
  size_t  patriciaTree<V, M>::pat_node::fmatch( const char* k_1, size_t l_1, const char* k_2, size_t l_2 )
    {
      auto  k1e = k_1 + l_1;
      auto  k2e = k_2 + l_2;
      auto  k1p = k_1;

      while ( k1p < k1e && k_2 < k2e && *k1p == *k_2++ )
        ++k1p;

      return k1p - k_1;
    }

  template <class V, class M>
  typename patriciaTree<V, M>::pat_node* patriciaTree<V, M>::pat_node::remove( const char* key, size_t len, M& mem )
    {
      return nullptr;
    }

  template <class V, class M>
  typename patriciaTree<V, M>::pat_node* patriciaTree<V, M>::pat_node::search( const char* thekey, size_t cchkey )
    {
      for ( auto  p_this = this; ; )
      {
        if ( cchkey == 0 )
          return p_this;

      // найти элемент во вложенном массиве, у которого первый символ ключа равен первому символу вставляемого ключа
        auto  keychr( *thekey );
        auto  p_scan(  p_this->p_list );

        while ( p_scan != nullptr && (unsigned char)keychr > (unsigned char)p_scan->strkey[0] )
          p_scan = p_scan->p_next;

        if ( p_scan != nullptr && keychr == p_scan->strkey[0] )
          {
            auto  curlen = p_scan->keylen();

            if ( cchkey >= curlen && memcmp( thekey, p_scan->strkey, curlen ) == 0 )
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

  template <class V, class M>
  const typename patriciaTree<V, M>::pat_node* patriciaTree<V, M>::pat_node::search( const char* thekey, size_t cchkey ) const
    {
      for ( auto  p_this = this; ; )
      {
        if ( cchkey == 0 )
          return p_this;

      // найти элемент во вложенном массиве, у которого первый символ ключа равен первому символу вставляемого ключа
        auto  keychr( *thekey );
        auto  p_scan(  p_this->p_list );

        while ( p_scan != nullptr && (unsigned char)keychr > (unsigned char)p_scan->strkey[0] )
          p_scan = p_scan->p_next;

        if ( p_scan != nullptr && keychr == p_scan->strkey[0] )
          {
            auto  curlen = p_scan->keylen();

            if ( cchkey >= curlen && memcmp( thekey, p_scan->strkey, curlen ) == 0 )
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

  template <class V, class M>
  typename patriciaTree<V, M>::pat_node* patriciaTree<V, M>::pat_node::insert( const char* thekey, size_t cchkey, M& memman )
    {
    // если поисковый ключ отсканирован полностью, навершить поиск
      if ( cchkey == 0 )
        return this;

    // найти первый элемент во вложенном массиве, у которого первый символ ключа больше
    // либо равен первому символу вставляемого ключа;
    // если таковой не найден, вставить новый узел с требуемым значением полного ключа
    // и вернуть его;
      auto  keychr( *thekey );
      auto  pprepl( &p_list );
      auto  p_scan( *pprepl );  assert( p_scan == p_list );

      while ( p_scan != nullptr && (unsigned char)keychr > (unsigned char)p_scan->strkey[0] )
        p_scan = *(pprepl = &(*pprepl)->p_next);

      if ( p_scan == nullptr || keychr != p_scan->strkey[0] )
        {
          pat_node* palloc;

          if ( (palloc = create( thekey, cchkey, memman, p_scan )) == nullptr )
            return nullptr;

          return *pprepl = palloc;
        }

      assert( p_scan != nullptr );
      assert( keychr == p_scan->strkey[0] );

    // есть некоторое совпадение, полное или частичное, искомого ключа с частичным ключом
    // найденного узла в списке вложенных узлов;
    // определить длину совпадения ключа с ключевой последовательностью узла;
    // если совпадение полное, вызвать метод рекурсивно, поправив указатель на ключ
      auto  curlen = p_scan->keylen();
      auto  lmatch = fmatch( thekey, cchkey, p_scan->strkey, curlen );  assert( lmatch > 0 && lmatch <= curlen );

      if ( lmatch == curlen )
        return p_scan->insert( thekey + curlen, cchkey - curlen, memman );

      assert( lmatch < curlen );

    // добавляемый ключ частично совпадает с ключом найденного вложенного элемента;
    // если длина совпадения равна длине добавляемого ключа, создать новый элемент
    // вместо p_scan с таким частичным ключом, а ключ у p_scan модифицировать (укоротить)
    // добавляемый ключ длиннее длины совпадения;
    // создать новый элемент для узла и добавить в него два элемента:
    // остаток добавляемого ключа и найденный p_scan с усечённым ключом
      if ( lmatch == cchkey )
        {
          pat_safe  palloc( memman );

          if ( (palloc = create( thekey, lmatch, memman, p_scan->p_next )) == nullptr )   // замещающий элемент
            return nullptr;

          memmove( p_scan->strkey, p_scan->strkey + lmatch, curlen - lmatch );
            p_scan->setlen( curlen - lmatch );
            p_scan->p_next = nullptr;

          palloc->p_list = p_scan;

          return *pprepl = palloc.release();
        }

    // есть хвост и от добавляемого ключа, и от текущего элемента
      assert( cchkey > lmatch && curlen > lmatch );

      pat_safe  palloc( memman );
      pat_safe  p_tail( memman );
      auto      rescmp( (unsigned char)thekey[lmatch] - (unsigned char)p_scan->strkey[lmatch] );

      if ( (palloc = create( thekey, lmatch, memman, p_scan->p_next )) == nullptr )   // замещающий элемент
        return nullptr;

      if ( (p_tail = create( thekey + lmatch, cchkey - lmatch, memman )) == nullptr )
        return nullptr;

      memmove( p_scan->strkey, p_scan->strkey + lmatch, curlen - lmatch );
        p_scan->setlen( curlen - lmatch );
        p_scan->p_next = nullptr;

      if ( rescmp < 0 )
        (palloc->p_list = p_tail.release())->p_next = p_scan;
      else
        (palloc->p_list = p_scan)->p_next = p_tail.release();

      return (*pprepl = palloc.release())->insert( thekey + lmatch, cchkey - lmatch, memman );
    }

  template <class V, class M>
  void  patriciaTree<V, M>::pat_node::delval()
    {
      if ( (uflags & 0x80000000) != 0 )
        ((V*)cvalue)->~V();
      uflags &= ~0x80000000;
    }

  template <class V, class M>
  void  patriciaTree<V, M>::pat_node::setlen( size_t l )
    {
      size_t  minlen = (uflags & ~0x80000000) != 0 ? uflags & ~0x80000000 : 1;
      size_t  maxlen = (minlen + 0x0f) & ~0x0f;   assert( l <= maxlen );

      uflags = (uflags & 0x80000000) | l;
    }

  template <class V, class M>
  V*    patriciaTree<V, M>::pat_node::setval( const V& v )
    {
      if ( (uflags & 0x80000000) != 0 )
        ((V*)cvalue)->~V();
      uflags |= 0x80000000;
        return new( (V*)cvalue ) V( v );
    }

  template <class V, class M>
  V*    patriciaTree<V, M>::pat_node::setval( V&& v )
    {
      if ( (uflags & 0x80000000) != 0 )
        ((V*)cvalue)->~V();
      uflags |= 0x80000000;
        return new( (V*)cvalue ) V( v );
    }

  template <class V, class M>
  size_t  patriciaTree<V, M>::pat_node::GetBufLen() const
    {
      size_t    arsize = 0;
      size_t    ccharr = 0;
      size_t    curlen = keylen();
      const V*  pvalue = (uflags & 0x80000000) != 0 ? (const V*)cvalue : nullptr;

      for ( auto p = p_list; p != nullptr; p = p->p_next, arsize += 2 )
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

  template <class V, class M>
  template <class S>
  S*  patriciaTree<V, M>::pat_node::FetchFrom( S* s, M& memman, size_t cchstr, size_t arsize )
    {
      auto    bvalue( (arsize & 1) != 0 );
      auto    nitems = arsize >> 1;
      size_t  cbjump;

      if ( cchstr != 0 )
        {
          if ( cchstr != keylen() )
            return nullptr;
          if ( (s = ::FetchFrom( s, strkey, cchstr )) == nullptr )
            return nullptr;
        }

      s = ::FetchFrom( s, cbjump );

      for ( auto p = &p_list; nitems != 0; p = &(*p)->p_next, --nitems )
        {
          size_t  sublen;
          size_t  subarr;

          if ( (s = ::FetchFrom( ::FetchFrom( s, sublen ), subarr )) == nullptr )
            return nullptr;
          if ( (*p = pat_node::create( nullptr, sublen, memman )) == nullptr )
            return nullptr;
          if ( (s = (*p)->FetchFrom( s, memman, sublen, subarr )) == nullptr )
            return nullptr;
        }

      return bvalue ? ::FetchFrom( s, *setval( V() ) ) : s;
    }

  template <class V, class M>
  template <class P>
  void  patriciaTree<V, M>::pat_node::PrintTree( P print, size_t before ) const
    {
      for ( auto p = p_list; p != nullptr; p = p->p_next )
        {
          auto  thekey = p->strkey;
          auto  cchkey = p->keylen();
          auto  nwrite = mtc::min( cchkey, (size_t)(0x20 - 3) );

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

  template <class V, class M>
  template <class O>
  O*  patriciaTree<V, M>::pat_node::Serialize( O* o ) const
    {
      size_t    arsize = 0;
      size_t    ccharr = 0;
      size_t    curlen = keylen();
      const V*  pvalue = (uflags & 0x80000000) != 0 ? (const V*)cvalue : nullptr;

      for ( auto p = p_list; p != nullptr; p = p->p_next, arsize += 2 )
        {
          ccharr += p->GetBufLen();
        }

      if ( pvalue != nullptr )
        {
          arsize |= 1;
        }

    // store key size, array size and key
      if ( (o = ::Serialize( ::Serialize( ::Serialize( o, curlen ), arsize ), strkey, curlen )) == nullptr )
        return nullptr;

    // write the array and value size in bytes
      if ( (o = ::Serialize( o, ccharr + (pvalue != nullptr ? ::GetBufLen( *pvalue ) : 0) )) == nullptr )
        return nullptr;

      for ( auto p = p_list; p != nullptr; p = p->p_next )
        {
          if ( (o = p->Serialize( o )) == nullptr )
            return nullptr;
        }

      return pvalue != nullptr ? ::Serialize( o, *pvalue ) : o;
    }

  template <class V, class M>
  template <class N, class A>
  int     patriciaTree<V, M>::pat_node::for_impl( N& r_node, A action )
    {
      auto  pvalue = r_node.getval();
      int   nerror;

      if ( pvalue != nullptr )
        if ( (nerror = action( *pvalue )) != 0 )
          return nerror;

      for ( auto p_scan = r_node.p_list; p_scan != nullptr; p_scan = p_scan->p_next )
        if ( (nerror = p_scan->for_each( action )) != 0 )
          return nerror;

      return 0;
    }

  // patriciaTree::iterator implementation

  template <class V, class M>
  patriciaTree<V, M>::iterator::iterator( const pat_node* p ): key()
    {
      for ( ; p != nullptr && (atrace.size() == 0 || !atrace.last()->hasval()); p = p->p_list )
      {
        atrace.Append( p );
        keybuf.Append( p->keylen(), p->strkey );
      }
      ptr = (const char*)keybuf;
      len = (size_t)keybuf.size();
    }

  template <class V, class M>
  typename patriciaTree<V, M>::iterator&  patriciaTree<V, M>::iterator::setkey()
    {
      ptr = (const char*)keybuf;
      len = (size_t)keybuf.size();
      return *this;
    }

  template <class V, class M>
  patriciaTree<V, M>::iterator::iterator(): key()
    {
    }

  template <class V, class M>
  patriciaTree<V, M>::iterator::iterator( iterator&& it ): key( it ),
      atrace( static_cast<pattrace&&>( it.atrace ) ),
      keybuf( static_cast<pastring&&>( it.keybuf ) )
    {
      it.setkey();
    }

  template <class V, class M>
  typename patriciaTree<V, M>::iterator& patriciaTree<V, M>::iterator::operator = ( iterator&& it )
    {
      artace.operator = ( static_cast<pattrace&&>( it.atrace ) );
      keybuf.operator = ( static_cast<pastring&&>( it.keybuf ) ); it.setkey();
      return *this;
    }

  template <class V, class M>
  typename patriciaTree<V, M>::iterator& patriciaTree<V, M>::iterator::operator ++ ()
    {
      while ( atrace.size() != 0 )
      {
        const pat_node* p_node;

      // если у узла есть вложенные элементы, максимально продвинуться вглубь дерева,
      // но не дальше первого найденного элемента со значением
        if ( (p_node = atrace.last()->p_list) != nullptr )
        {
          do
          {
            atrace.Append( p_node );
            keybuf.Append( p_node->keylen(), p_node->strkey );
          } while ( !p_node->hasval() && (p_node = p_node->p_list) != nullptr );
        }
          else
      // иначе, если вложенных элементов нет, перейти к следующему элементу в списке
      // того же горизонтального уровня
        if ( (p_node = atrace.last()->p_next) != nullptr )
        {
          keybuf.SetLen( keybuf.GetLen() - atrace.last()->keylen() );
          keybuf.Append( p_node->keylen(), p_node->strkey );
          atrace.last() = p_node;
        }
          else
      // отмотать вниз по дереву с переходом на следующий элемент до успешного ключа
        {
          do
          {
            keybuf.SetLen( keybuf.GetLen() - atrace.last()->keylen() );
            atrace.SetLen( atrace.GetLen() - 1 );
          } while ( atrace.size() != 0 && atrace.last()->p_next == nullptr );

          if ( atrace.size() != 0 )
          {
            assert( atrace.last() != nullptr );

            keybuf.SetLen( keybuf.GetLen() - atrace.last()->keylen() );
            atrace.last() = atrace.last()->p_next;
            keybuf.Append( atrace.last()->keylen(), atrace.last()->strkey );
          }
            else
          continue;
        }

      // если найден узел со значением, вернуть его
        if ( atrace.last()->hasval() )
          return setkey();
      }
      atrace.SetLen( 0 );
      keybuf.SetLen( 0 );
      return setkey();
    }

  template <class V, class M>
  bool  patriciaTree<V, M>::iterator::operator == ( const iterator& it ) const
    {
      return ptr == it.ptr && len == it.len
        && atrace.size() == it.atrace.size() && keybuf.size() == it.keybuf.size()
        && memcmp( atrace.begin(), it.atrace.begin(), atrace.size() * sizeof(*atrace.begin()) ) == 0
        && memcmp( keybuf.begin(), it.keybuf.begin(), keybuf.size() * sizeof(*keybuf.begin()) ) == 0;
    }

  // patricia implementation

  template <class V, class M>
  patriciaTree<V, M>::patriciaTree( patriciaTree&& p ):
    memman( static_cast<M&&>( p.memman ) ), p_tree( static_cast<pat_safe&&>( p.p_tree ) ) {}

  template <class V, class M>
  patriciaTree<V, M>& patriciaTree<V, M>::operator = ( patriciaTree&& p )
    {
      memman.operator = ( static_cast<M&&>( p.memman ) );
      p_tree.operator = ( static_cast<pat_safe&&>( p.p_tree ) );
      return *this;
    }

  template <class V, class M>
  void      patriciaTree<V, M>::Delete( const key& k )
    {
      if ( p_tree != nullptr )
        {
          auto  pfound = p_tree->search( k.ptr, k.len );

          if ( pfound != nullptr )
            pfound->delval();
        }
    }

  template <class V, class M>
  template <class vref>
  V*    patriciaTree<V, M>::insert( const key& k, vref v )
    {
      pat_node* pfound;

      if ( p_tree == nullptr && (p_tree = pat_node::create( nullptr, 0, memman )) == nullptr )
        return nullptr;

      return (pfound = p_tree->insert( k.ptr, k.len, memman )) != nullptr ?
        pfound->setval( v ) : nullptr;
    }

  template <class V, class M>
  template <class res, class slf>
  res*  patriciaTree<V, M>::search( const key& k, slf& s )
    {
      if ( s.p_tree != nullptr )
        {
          auto  pfound = s.p_tree->search( k.ptr, k.len );

          return pfound != nullptr ? pfound->getval() : nullptr;
        }
      return nullptr;
    }

  template <class V, class M>
  size_t  patriciaTree<V, M>::GetBufLen() const
    {
      return p_tree != nullptr ? p_tree->GetBufLen() : 3;
    }

  template <class V, class M>
  template <class S>
  S*      patriciaTree<V, M>::FetchFrom( S* s )
    {
      size_t  cchstr;
      size_t  arsize;

      p_tree = nullptr;

      if ( (s = ::FetchFrom( ::FetchFrom( s, cchstr ), arsize )) == nullptr )
        return nullptr;

      if ( cchstr == 0 && arsize == 0 )
        return s;

      if ( (p_tree = pat_node::create( nullptr, cchstr, memman )) == nullptr )
        return nullptr;

      return p_tree->FetchFrom( s, memman, cchstr, arsize );
    }

  template <class V, class M>
  template <class P>
  void    patriciaTree<V, M>::PrintTree( P p ) const
    {
      if ( p_tree != nullptr )
        p_tree->PrintTree( p, 0 );
    }

  template <class V, class M>
  template <class O>
  O*      patriciaTree<V, M>::Serialize( O* o ) const
    {
      return p_tree != nullptr ? p_tree->Serialize( o ) : ::Serialize( ::Serialize( ::Serialize( o, 0 ), 0 ), 0 );
    }

  // patriciaDump::iterator implementation

  inline
  patriciaDump::iterator::iterator( const char* stored ): key()
    {
      if ( stored != nullptr )
        atrace.Append( GetPat( stored ) );
      if ( !atrace.last().bvalue )
        Tonext();
    }

  inline
  patriciaDump::iterator::iterator(): key()
    {
    }

  inline
  patriciaDump::iterator::iterator( iterator&& it ): key( it ),
      atrace( static_cast<decltype(atrace)&&>( it.atrace ) ),
      keybuf( static_cast<decltype(keybuf)&&>( it.keybuf ) )
    {
      it.setkey();
    }

  inline
  typename patriciaDump::iterator&  patriciaDump::iterator::operator = ( iterator&& it )
    {
      atrace.operator = ( static_cast<decltype(atrace)&&>( it.atrace ) );
      keybuf.operator = ( static_cast<decltype(keybuf)&&>( it.keybuf ) ); it.setkey();
      return *this;
    }

  inline
  bool  patriciaDump::iterator::operator == ( const iterator& it ) const
    {
      return atrace.size() == it.atrace.size()
          && keybuf.size() == it.keybuf.size()
          && memcmp( (const char*)keybuf, (const char*)it.keybuf, keybuf.size() ) == 0;
    }

  inline
  typename patriciaDump::iterator::patpos patriciaDump::iterator::GetPat( const char* stored )
    {
      patpos  thepat;
      size_t  sublen;

    // вычитать первый из вложенных узлов
      thepat.dicptr = ::FetchFrom( ::FetchFrom( stored, thepat.keylen ), thepat.nnodes );

      if ( thepat.keylen != 0 )
      {
        keybuf.SetLen( keybuf.GetLen() + thepat.keylen );
        thepat.dicptr = ::FetchFrom( thepat.dicptr, (char*)keybuf + keybuf.size() - thepat.keylen, thepat.keylen );
      }

      thepat.bvalue = (thepat.nnodes & 0x01) != 0;
      thepat.nnodes >>= 1;
      thepat.dicptr = ::FetchFrom( thepat.dicptr, sublen );
      thepat.endptr = thepat.dicptr + sublen;

      return thepat;
    }

  inline
  typename patriciaDump::iterator&  patriciaDump::iterator::Tonext()
    {
      while ( atrace.size() != 0 )
      {
        patpos& thepos = atrace.last();

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
            atrace.Append( patnew );
        }
          else
        {
          atrace[atrace.size() - 2].dicptr = atrace.last().endptr;
          keybuf.SetLen( keybuf.GetLen() - thepos.keylen );
          atrace.SetLen( atrace.GetLen() - 1 );
        }
      }
      return setkey();
    }

  template <class A>
  int   patriciaDump::for_each( A action ) const
    {
      array<char>  keybuf;

      return (scantree( (const char*)serial, keybuf, 0, action ), 0);
    }

  // patriciaDump implementation

  template <class _func_>
  int           patriciaDump::Select( const void* k, size_t l, _func_ f ) const
  {
    byte_t        thekey[0x100];
    const byte_t* thedic;
    int           nchars;
    int           nnodes;
    int           sublen;

    if ( (thedic = ::FetchFrom( ::FetchFrom( ::FetchFrom( serial, nchars ), nnodes ), sublen )) == nullptr )
      return 0;

    assert( nnodes <= 513 );
    assert( nchars <= 256 * 4 );

    return Select( thedic, nchars, (const byte_t*)k, l + (const byte_t*)k, thedic + nchars, nnodes, f, thekey, array_end( thekey ), thekey );
  }

  inline
  const char* patriciaDump::Search( const key& k ) const
  {
    const char* thedic;
    size_t      nchars;
    size_t      nnodes;
    size_t      sublen;
    const char* keyptr = k.ptr;
    const char* keyend = k.ptr + k.len;

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

    return Search( keyptr, keyend - keyptr, thedic, nnodes );
  }

  template <class _func_>
  int           patriciaDump::Select( const char* dicstr, int         diclen,
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
        const byte_t* subdic;
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
  const char* patriciaDump::Search( const char* thekey,
                                    size_t      cchkey,
                                    const char* thedic,
                                    size_t      nnodes ) const
  {
    bool    bvalue = (nnodes & 1) != 0;
    byte_t  chfind;

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
  const char* patriciaDump::JumpOver( int nnodes, const char* thedic ) const
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
  const char* patriciaDump::scantree( const char* serial, array<char>& keybuf, size_t keylen, A action )
  {
    size_t  cchstr;
    size_t  nitems;
    size_t  ccharr;

    if ( (serial = ::FetchFrom( ::FetchFrom( serial, cchstr ), nitems )) != nullptr )
    {
      const char* setptr;

      if ( cchstr != 0 )
      {
        if ( (size_t)keybuf.size() < keylen + cchstr )
        {
          if ( keybuf.SetLen( (keylen + cchstr + 0x0f) & ~0x0f ) != 0 )
            return nullptr;
        }

        if ( (serial = ::FetchFrom( serial, keylen + (char*)keybuf, cchstr )) == nullptr )
          return nullptr;
      }

      if ( (setptr = serial = ::FetchFrom( serial, ccharr )) == nullptr )
        return serial;

      for ( auto n = nitems >> 1; n != 0 && serial != nullptr; --n )
        serial = scantree( serial, keybuf, keylen + cchstr, action );

      if ( (nitems & 0x1) != 0 )
        action( keybuf, keylen + cchstr, serial, setptr + ccharr - serial );

      return setptr + ccharr;
    }
    return serial;
  }

}

# endif  // __mtc_patricia_h__
