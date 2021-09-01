# include <map>
# include "../zmap.h"

template <class Insert>
auto  TestInsertStrToInt( Insert insert, uint32_t nloops ) -> std::chrono::milliseconds
{
  auto  tstart = std::chrono::steady_clock::now();

  srand( 0 );

  for ( uint32_t i = 0; i != nloops; ++i )
  {
    auto  intkey = rand();
    auto  strkey = std::to_string( intkey );

    insert( strkey, i );
  }

  return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - tstart );
}

template <class Insert>
auto  TestInsertIntToInt( Insert insert, uint32_t nloops ) -> std::chrono::milliseconds
{
  auto  tstart = std::chrono::steady_clock::now();

  srand( 0 );

  for ( uint32_t i = 0; i != nloops; ++i )
  {
    auto  intkey = rand();

    insert( intkey, i );
  }

  return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - tstart );
}

namespace mtc {

  class xmap
  {
    using key = zmap::key;

    class node_t;
    class data_t;
    class holder;

  public:
    xmap();
    xmap( const xmap& );

    template <class value_type>
    class iterator_type;

  protected:
    template <class assign>
    auto  set( const key&, assign ) -> zval&;
    auto  get() const -> holder;

  public:
    auto  put( const key&, zval&& ) -> zval&;
    auto  put( const key&, const zval& = {} ) -> zval&;

    auto  get( const key& ) -> zval*;
    auto  get( const key& ) const -> const zval*;

  public:
    auto  begin() const -> iterator_type<const zval>;
    auto  end() const -> iterator_type<const zval>;

    auto  begin() -> iterator_type<zval>;
    auto  end() -> iterator_type<zval>;

  public:
    auto  GetBufLen() const -> size_t;
    template <class O>
    O*    Serialize( O* ) const;
    template <class S>
    S*    FetchFrom( S* );

  protected:
    mutable std::atomic<data_t*>  p_data;

  };

  class xmap::node_t
  {
    friend class xmap;

    node_t*                     pnodes;       /* the list of nested nodes   */
    std::unique_ptr<mtc::zval>  pvalue;
    uint32_t                    ustate;       /* state type and node count  */
    uint8_t                     keybuf[12];   /* top key fragment           */

    enum: uint32_t
    {
      O_FRAGLEN = 0x0000000f,
      O_TREELEN = 0x00000ff0,
      O_KEYTYPE = 0x00003000,
      O_TREELIM = 0x0001C000
    };

  public:
    node_t();
    node_t( node_t&& );
    node_t( const node_t& );
   ~node_t();
    node_t& operator = ( node_t&& );

  protected:
    node_t( const uint8_t*  keytop, size_t length );
    node_t( const uint8_t*  keytop, size_t length, node_t*  pitems, uint32_t nitems );

    static
    auto  get_tree_limit( uint32_t ) -> uint32_t;
    auto  get_tree_limit() const -> uint32_t  {  return (ustate & O_TREELIM) >> 14;  }
    auto  get_tree_count() const -> uint32_t  {  return (ustate & O_TREELEN) >> 4;  }
    auto  get_str_length() const -> uint32_t  {  return (ustate & O_FRAGLEN) >> 0;  }
    auto  get_key_type() const -> uint32_t  {  return (ustate & O_KEYTYPE) >> 12;  }
    auto  set_key_type( unsigned k ) -> node_t& {  return ustate = (ustate & ~O_KEYTYPE) | ((k << 12) & O_KEYTYPE), *this;  }

  protected:
    auto  GetBufLen() const -> size_t;
    template <class O>
    auto  Serialize( O* ) const -> O*;
    template <class S>
    auto  FetchFrom( S* ) -> S*;

  public:
    void  clear();
    void  print( FILE*, const std::string& topstr = "", const std::string& othstr = "" );
    int   lines() const;

   /*
    * create( const uint8_t* key, size_t len )
    * Создаёт длинную цепочку элементов, чтобы уместить ключ полностью.
    */
    static
    auto  create( const uint8_t* key, size_t len ) -> node_t;
    static
    auto  getbuf( uint32_t count ) -> node_t*;
    auto  insert( const uint8_t* key, size_t len ) -> node_t*;
    auto  insert( node_t* pos, node_t&& ) -> node_t*;

    bool  remove( const uint8_t* key, size_t len );

  };

  class xmap::data_t: public node_t
  {
  public:
    mutable std::atomic_long  rcount;

  public:
    data_t(): rcount( 0 ) {}
    data_t( const data_t& data ): node_t( data ), rcount( 0 ) {}
    data_t( node_t&& node ): node_t( std::move( node ) ), rcount( 0 ) {}

  };

  class xmap::holder
  {
    data_t* p_data;

  public:
    holder( data_t* p = nullptr ): p_data( p )
      {  if ( p_data != nullptr )  ++p_data->rcount;  }
    holder( const holder& h ): p_data( h.p_data )
      {  if ( p_data != nullptr )  ++p_data->rcount;  }
   ~holder()
      {
        if ( p_data != nullptr && --p_data->rcount == 0 )
          delete p_data;
      }
    auto  operator = ( const holder& h ) -> holder&
      {
        if ( p_data != nullptr && --p_data->rcount == 0 )
          delete p_data;
        if ( (p_data = h.p_data) != nullptr )
          ++p_data->rcount;
        return *this;
      }
    auto  operator -> () const -> data_t*
      {  return p_data;  }
    operator data_t* () const {  return p_data;  }
  };

  template <class value_type>
  class xmap::iterator_type
  {
    friend class xmap;

    std::vector<const node_t*>  atrace;
    std::vector<uint8_t>        keybuf;
    size_t                      keylen;

    struct iterator_data
    {
      zmap::key   key;
      value_type& value;
    };

    iterator_type( const node_t* );
    iterator_type(): pvalue( nullptr )  {}
  public:

  public:
    auto  operator ++ () -> iterator_type&;
    auto  operator ++ ( int ) -> iterator_type;

    bool  operator == ( const iterator_type& it ) const  {  return atrace == it.atrace;  }
    bool  operator != ( const iterator_type& it ) const  {  return !(*this == it);  }

    auto  operator -> () const  {  return (iterator_data*)&x_data;  }
    auto  operator * () const -> const iterator_data& {  return *(iterator_data*)&x_data;  }

  protected:
    auto  getnext() -> iterator_type&;
    void  initkey();

  protected:
    typename std::aligned_storage<sizeof(iterator_data)>::type  x_data;
    value_type*                                                 pvalue;

  };

  // xmap::const_iterator implementation

  template <class value_type>
  xmap::iterator_type<value_type>::iterator_type( const node_t* pn ): iterator_type()
    {  atrace.push_back( pn ), getnext();  }

  template <class value_type>
  auto  xmap::iterator_type<value_type>::getnext() -> iterator_type&
  {
    while ( !atrace.empty() )
    {
      if ( atrace.back()->get_key_type() != 0 )
        break;
      if ( atrace.back()->pnodes != nullptr ) atrace.push_back( atrace.back()->pnodes );
        else
      if ( atrace.size() <= 1 ) atrace.pop_back();
        else
      {
        auto  parent = atrace[atrace.size() - 2];
        auto  ptrend = parent->pnodes + parent->get_tree_count();

        if ( ++atrace.back() < ptrend ) continue;
          else atrace.pop_back();
      }
    }
    return initkey(), *this;
  }

  template <class value_type>
  void  xmap::iterator_type<value_type>::initkey()
  {
    keylen = 0;

    for ( auto it: atrace )
    {
      auto  nbytes = it->get_str_length();

      keybuf.resize( std::max( keybuf.size(), keylen + nbytes ) );
        memcpy( keybuf.data() + keylen, it->keybuf, nbytes );
      keylen += nbytes;
    }

    if ( pvalue != nullptr )
      ((iterator_data*)&x_data)->~iterator_data(), pvalue = nullptr;

    if ( keylen != 0 )
    {
      keybuf[keylen] = '\0';

      new( &x_data ) iterator_data{ zmap::key( atrace.back()->get_key_type() - 1, keybuf.data(), keylen ),
        *(pvalue = atrace.back()->pvalue.get()) };
    }
  }

  template <class value_type>
  auto  xmap::iterator_type<value_type>::operator++() -> iterator_type&
  {
    if ( atrace.empty() )
      throw std::range_error( "empty iterator 'operator ++()' call" );

    if ( atrace.back()->get_tree_count() != 0 ) atrace.push_back( atrace.back()->pnodes );
      else
    while ( atrace.size() > 1 )
    {
      auto  parent = atrace[atrace.size() - 2];
      auto  ptrend = parent->pnodes + parent->get_tree_count();

      if ( ++atrace.back() >= ptrend ) atrace.pop_back();
        else break;
    }

    if ( atrace.size() == 1 )
      atrace.clear();

    return getnext();
  }

  template <class value_type>
  auto  xmap::iterator_type<value_type>::operator++( int ) -> iterator_type
  {
    auto  self( *this );
    return operator++(), self;
  }

}

namespace mtc {

  using ptr_to_int = typename std::conditional<sizeof(void*) == sizeof(uint64_t),
    uint64_t, uint32_t>::type;

  template <class T>
  auto  break_ptr( T* t ) -> T* {  return (T*)(1 | (ptr_to_int)t);  }
  template <class T>
  auto  clear_ptr( T* t ) -> T* {  return (T*)(~1 & (ptr_to_int)t);  }

  // xmap::node_t implementation

  xmap::node_t::node_t(): pnodes( nullptr ), ustate( 0 )
  {
  }

  xmap::node_t::node_t( node_t&& node ):
    pnodes( node.pnodes ),
    pvalue( std::move( node.pvalue ) ),
    ustate( node.ustate )
  {
    node.pnodes = nullptr;
    memcpy( keybuf, node.keybuf, sizeof(node.keybuf) );
  }

  xmap::node_t::node_t( const node_t& node ):
    ustate( node.ustate )
  {
    if ( node.pnodes == nullptr ) pnodes = nullptr;
      else pnodes = new node_t( *node.pnodes );
    if ( node.pvalue != nullptr )
      pvalue = std::unique_ptr<zval>( new zval( *node.pvalue ) );
    memcpy( keybuf, node.keybuf, sizeof(node.keybuf) );
  }

  xmap::node_t::~node_t()
  {
    clear();
  }

  auto  xmap::node_t::operator = ( node_t&& node ) -> node_t&
  {
    clear();

    pnodes = node.pnodes;  node.pnodes = nullptr;
    pvalue = std::move( node.pvalue );
    ustate = node.ustate;  node.ustate = 0;
    memcpy( keybuf, node.keybuf, sizeof(node.keybuf) );

    return *this;
  }

  xmap::node_t::node_t(
    const uint8_t*  keytop,
    size_t          length ): pnodes( nullptr ), ustate( length )
  {
    assert( length <= sizeof(keybuf) );

    memcpy( keybuf, keytop, length );
  }

  xmap::node_t::node_t(
    const uint8_t*  keytop,
    size_t          length,
    node_t*         pitems,
    uint32_t        nitems ): pnodes( pitems ), ustate( length | (nitems << 4) )
  {
    assert( length <= sizeof(keybuf) );

    memcpy( keybuf, keytop, length );
  }

  auto  xmap::node_t::get_tree_limit( uint32_t count ) -> uint32_t
  {
    return count <= 2   ? 0 :
           count <= 4   ? 1 :
           count <= 8   ? 2 :
           count <= 16  ? 3 :
           count <= 32  ? 4 :
           count <= 64  ? 5 :
           count <= 128 ? 6 : 7;
  }

  auto  xmap::node_t::GetBufLen() const -> size_t
  {
    auto  subcount = get_tree_count();
    auto  cbserial = 1 + get_str_length() + (pvalue != nullptr ? ::GetBufLen( *pvalue.get() ) : 0);

    if ( subcount > 1 )
      cbserial += ::GetBufLen( cbserial >> 1 );

    for ( auto beg = pnodes, end = pnodes + subcount; beg != end; ++beg )
      cbserial += beg->GetBufLen();

    return cbserial;
  }

  template <class O>
  O*   xmap::node_t::Serialize( O* o ) const
  {
    auto  subcount = get_tree_count();
    auto  keybytes = get_str_length();
    auto  keyvalue = get_key_type();
    auto  nodestat = (uint8_t)(
      keybytes | (keyvalue << 4) | (subcount << 6) | (subcount > 1 ? 0x80 : 0x00));

  // write first character, control byte, least characters, value, array length
  // and nested arrays
    o = ::Serialize(
        ::Serialize( o, nodestat ), keybuf, keybytes );

    if ( pvalue != nullptr )
      o = ::Serialize( o, *pvalue.get() );

    if ( subcount > 1 )
      o = ::Serialize( o, subcount >> 1 );

    for ( auto beg = pnodes, end = pnodes + subcount; beg != end; ++beg )
      o = beg->Serialize( ::Serialize( o, beg->GetBufLen() ) );

    return o;
  }

  template <class S>
  S*    xmap::node_t::FetchFrom( S* s )
  {
    uint8_t   nodestat;
    uint32_t  keybytes;
    uint32_t  subcount;

    clear();

    if ( (s = ::FetchFrom( s, nodestat )) == nullptr )
      return nullptr;

    if ( (keybytes = nodestat & 0x0f) != 0 && (s = ::FetchFrom( s, keybuf, keybytes )) == nullptr )
      return nullptr;

    if ( ((nodestat >> 4) & 0x03) != 0 )
    {
      if ( (s = (pvalue = std::unique_ptr<zval>( new zval() ))->FetchFrom( s )) == nullptr )
        return nullptr;
    }

    if ( (nodestat & 0x80) != 0 )
    {
      if ( (s = ::FetchFrom( s, subcount)) != nullptr ) subcount = (subcount << 1) | ((nodestat >> 6) & 0x01);
        else return nullptr;
    } else subcount = (nodestat >> 6) & 0x01;

    if ( subcount != 0 )
    {
      auto  ulimit = get_tree_limit( subcount );
      auto  uitems = 1 << (1 + ulimit);
      auto  subset = getbuf( uitems );

      for ( auto beg = subset, end = subset + subcount; beg != end; ++beg )
        s = (new ( beg ) node_t())->FetchFrom( s );

      pnodes = subset;
      ustate |= (subcount << 4) | (ulimit << 14) | ((nodestat << 8) & O_KEYTYPE);
    }

    return s;
  }

  void  xmap::node_t::clear()
  {
    if ( pnodes != nullptr )
    {
      for ( auto ptr = pnodes, end = ptr + get_tree_count(); ptr != end; ++ptr )
        ptr->~node_t();
      delete[] (char*)pnodes;
    }
    pnodes = nullptr;
    pvalue.reset();
    ustate = 0;
  }

  void  xmap::node_t::print( FILE* out, const std::string& topstr, const std::string& othstr )
  {
    auto  sprint = mtc::strprintf( "%s%s'%s'%s",
      topstr.c_str(),
      topstr.size()  ? " " : "",
      std::string( (const char*)keybuf, get_str_length() ).c_str(),
      get_tree_count() != 0 ? " +" : "" );
    auto  sother = othstr + std::string( sprint.length() - othstr.length() - 1, ' ' );

    if ( get_tree_count() != 0 )
    {
      pnodes[0].print( out, sprint, sother + '|' );

      for ( size_t i = 1; i != get_tree_count(); ++i )
        pnodes[i].print( out, sother + '+', sother + (i < get_tree_count() - 1 ? '|' : ' ') );
    }
      else
    fprintf( out, "%s\n", sprint.c_str() );
  }

  auto  xmap::node_t::create( const uint8_t* keyptr, size_t length ) -> node_t
  {
    auto    cchkey = std::min( length, sizeof(keybuf) );
    node_t  newone( keyptr, cchkey );

    if ( length > cchkey )
    {
      auto  ulimit = get_tree_limit( 1 );     // allocation size range
      auto  uitems = 1 << (1 + ulimit);

      newone.pnodes = new ( getbuf( uitems ) ) node_t(
        create( keyptr + cchkey, length - cchkey ) );
      newone.ustate = (newone.ustate & ~(O_TREELEN | O_TREELIM)) | (1 << 4) | (ulimit << 14);
    }
    return newone;
  }

  auto  xmap::node_t::getbuf( uint32_t limit ) -> node_t*
  {
    return (node_t*)new char[sizeof(node_t) * limit];
  }

  auto  xmap::node_t::insert( const uint8_t* keystr, size_t keylen ) -> node_t*
  {
    auto  expand = this;

    while ( keylen != 0 )
    {
      auto  cchkey = expand->ustate & O_FRAGLEN;
      auto  keytop = expand->keybuf;
      auto  keyend = keytop + cchkey;

      // compare matching part of key
      while ( keytop != keyend && keylen != 0 && *keytop == *keystr )
        ++keystr, --keylen, ++keytop;

      // проверить совпадение фрагментов; если фрагмент соответствует ключу полностью,
      // * проверить, полностью ли исчерпан сам ключ; если и ключ сам тоже закончился,
      //   узел найден и можно его вернуть;
      // * иначе пройти по списку вложенных узлов и найти подходящий или точку вставки;
      if ( keytop == keyend )
      {
        if ( keylen != 0 )
        {
          auto  nnodes = (expand->ustate & O_TREELEN) >> 4;
          auto  ptrtop = expand->pnodes;
          auto  ptrend = ptrtop + nnodes;
          auto  chnext = *keystr;

          // Выбрать вложенный элемент, который либо начинается на нужный символ, либо
          // перед которым надо вставлять новый элемент.
          while ( ptrtop != ptrend && ptrtop->keybuf[0] < chnext )
            ++ptrtop;

          // если элемент не найден, создать новый, вставить на нужное место и вернуть
          // последний в цепочке созданных сниз элементов
          if ( ptrtop == ptrend || ptrtop->keybuf[0] != chnext )
          {
            for ( expand = expand->insert( ptrtop, create( keystr, keylen ) ); expand->pnodes != nullptr; )
              expand = expand->pnodes;
          }
            else
          {
            expand = ptrtop;
            continue;
          }
        }
        return expand;
      }
        else
      // иначе совпадение частичное, хотя бы один символ; поделить узел на два, до совпадения,
      // c возможным привешенным значением, и после совпадения - со списком вложенных элементов
      {
        if ( keylen == 0 )
        {
          auto  ulimit = get_tree_limit( 1 );
          auto  uitems = 1 << (1 + ulimit);
          auto  subset = getbuf( uitems );    // остаток существующего ключа

          new ( subset + 0 ) node_t( keytop, keyend - keytop, expand->pnodes, expand->get_tree_count() );
            subset[0].pvalue = std::move( expand->pvalue );
            subset[0].ustate = subset[0].ustate | (expand->ustate & O_KEYTYPE);
          expand->pnodes = subset;
          expand->ustate = (keytop - expand->keybuf) | (1 << 4) | (ulimit << 14);

          return expand;
        }
          else
        {
          auto    ulimit = get_tree_limit( 2 );
          auto    uitems = 1 << (1 + ulimit);
          auto    subset = getbuf( uitems );      // список вложенных элементов для этого узла
          node_t* p_rest;
          node_t* p_push;

          if ( *keytop < *keystr )  {  p_rest = subset + 0;  p_push = subset + 1;  }
            else  {  p_rest = subset + 1;  p_push = subset + 0;  }

          new( p_rest ) node_t( keytop, keyend - keytop, expand->pnodes, expand->get_tree_count() );
            p_rest->pvalue = std::move( expand->pvalue );
            p_rest->ustate = p_rest->ustate | (expand->ustate & O_KEYTYPE);
          new( p_push ) node_t( create( keystr, keylen ) );
            expand->pnodes = subset;
            expand->ustate = (keytop - expand->keybuf) | (2 << 4) | (ulimit << 14);

          for ( expand = p_push; expand->pnodes != nullptr; )
            expand = expand->pnodes;

          return expand;
        }
      }
    }

    return expand;
  }

  auto  xmap::node_t::insert( node_t* pos, node_t&& put ) -> node_t*
  {
    auto  count = get_tree_count();
    auto  limit = get_tree_limit();
    auto  nodes = pnodes != nullptr ? 1U << (1 + limit) : 0U;
    auto  index = pos - pnodes;

    if ( count < nodes )
    {
      for ( auto o = pnodes + count, s = o, e = pnodes + index; s != e; s->~node_t() )
        new( o-- ) node_t( std::move( *--s ) );
    }
      else
    {
      auto  ulimit = get_tree_limit( count + 1 );
      auto  uitems = 1 << (1 + ulimit);
      auto  newset = getbuf( uitems );

      if ( pnodes != nullptr )
      {
        auto  output = newset;
        auto  srcptr = pnodes;

        for ( auto e = pnodes + index; srcptr != e; (srcptr++)->~node_t() )
          new( output++ ) node_t( std::move( *srcptr ) );

        ++output;

        for ( auto e = pnodes + count; srcptr != e; (srcptr++)->~node_t() )
          new( output++ ) node_t( std::move( *srcptr ) );

        delete[] (char*)pnodes;
      }

      pnodes = newset;
      ustate = (ustate & ~O_TREELIM) | (ulimit << 14);
    }
    ustate = (ustate & ~O_TREELEN) | ((count + 1) << 4);
      new( pnodes + index ) node_t( std::move( put ) );
    return pnodes + index;
  }

  bool  xmap::node_t::remove( const uint8_t* keystr, size_t length )
  {
    auto  keytop = keybuf;
    auto  keyend = keybuf + get_str_length();

    // проверить корректность вызова - есть ли тут ключ
    while ( keytop != keyend && *keytop == *keystr )
      ++keytop, ++keystr, --length;

    if ( keytop != keyend )
      return false;

    if ( length != 0 || get_tree_count() != 0 )
    {
      auto  ncount = get_tree_count();
      auto  ptrtop = pnodes;
      auto  ptrend = pnodes + ncount;
      auto  chfind = *keystr;

      while ( ptrtop != ptrend && ptrtop->keybuf[0] < chfind )
        ++ptrtop;

      if ( ptrtop != ptrend && ptrtop->keybuf[0] == chfind && ptrtop->remove( keystr, length ) )
      {
        if ( ncount == 1 && (pvalue == nullptr || pvalue->empty()) )
          return true;

        ptrtop->~node_t();

        for ( auto o = ptrtop++; ptrtop != ptrend; )
          *o++ = std::move( *ptrtop++ );
        ustate = (ustate & ~O_TREELEN) | ((ncount - 1) << 4);
          return false;
      }
    }
    return true;
  }

  // xmap implementation

  xmap::xmap(): p_data( nullptr ) {}

  xmap::xmap( const xmap& x )
  {
    auto  data = clear_ptr( x.p_data.load() );

    while ( data != nullptr && !x.p_data.compare_exchange_strong( data, break_ptr( data ) ) )
      data = clear_ptr( data );

    if ( data != nullptr )
      ++data->rcount;

    x.p_data = p_data = data;
  }

  // xmap::put( const key&, ... ) family

  template <class assign>
  auto  xmap::set( const key& k, assign ptr ) -> zval&
  {
    auto    data = clear_ptr( p_data.load() );
    node_t* last;

    while ( !p_data.compare_exchange_strong( data, break_ptr( data ) ) )
      data = clear_ptr( data );

    if ( data == nullptr )
    {
      ++(data = new data_t( node_t::create( k.data(), k.size() ) ))->rcount;

      for ( last = data; last->pnodes != nullptr; last = last->pnodes )
        (void)NULL;

      last->set_key_type( 1 + k.type() );
      last->pvalue = std::move( ptr() );

      return p_data = data, *last->pvalue.get();
    }

    if ( data->rcount > 1 )
    {
      auto  copy = new data_t( *data );

      if ( --data->rcount == 0 )
        delete data;
      ++(data = copy)->rcount;
    }

    last = data->insert( k.data(), k.size() );
    last->set_key_type( 1 + k.type() );
    last->pvalue = std::move( ptr() );

    return p_data = data, *last->pvalue.get();
  }

  auto  xmap::put( const key& k, zval&& v ) -> zval&
  {
    return set( k, [&](){  return std::unique_ptr<zval>( new zval( std::move( v ) ) );  } );
  }

  auto  xmap::put( const key& k, const zval& v ) -> zval&
  {
    return set( k, [&](){  return std::unique_ptr<zval>( new zval( v ) );  } );
  }

  auto  xmap::get() const -> holder
  {
    auto  data = clear_ptr( p_data.load() );
    auto  outp = holder();

    while ( p_data != nullptr && !p_data.compare_exchange_strong( data, break_ptr( data ) ) )
      data = clear_ptr( data );

    if ( data != nullptr )
      p_data = outp = holder( data );

    return outp;
  }

  auto  xmap::begin() const -> iterator_type<const zval>
    {  return iterator_type<const zval>( p_data.load() );  }

  auto  xmap::end() const -> iterator_type<const zval>
    {  return iterator_type<const zval>();  }

  auto  xmap::begin() -> iterator_type<zval>
    {  return iterator_type<zval>( p_data.load() );  }

  auto  xmap::end() -> iterator_type<zval>
    {  return iterator_type<zval>();  }

  template <class O>
  O*  xmap::Serialize( O* o ) const
  {
    auto  data = get();

    return data != nullptr ? data->Serialize( o ) :  ::Serialize( o, (char)0 );
  }

}

int   main()
{
/*
  fprintf( stdout, "%u\n", sizeof(node_t) );

  node_t  zmap;

  zmap = std::move( node_t::create( (const uint8_t*)"abcdedghijknmnop", 16 ) );
  zmap.insert( (const uint8_t*)"abcdedghijknnode", 16 );
  zmap.insert( (const uint8_t*)"abcdedqqqqqqqqqq", 16 );
  zmap.insert( (const uint8_t*)"abcdedghijknnome", 16 );
  zmap.insert( (const uint8_t*)"abcdedghijknnoeq", 16 );
  zmap.insert( (const uint8_t*)"abcdedghijknpoeq", 16 );
  zmap.insert( (const uint8_t*)"abcdedghijknport", 16 );
  zmap.insert( (const uint8_t*)"abcded", 6 );

  zmap.print( stdout );

  zmap.remove( (const uint8_t*)"abcdedghijknnoeq", 16 );
  zmap.print( stdout );

  return 0;
  zmap.insert( (const uint8_t*)"abcdef", 6 );
  zmap.insert( (const uint8_t*)"abcdf", 5 );
  zmap.insert( (const uint8_t*)"abcdb", 5 );
  zmap.insert( (const uint8_t*)"ab", 2 );

  zmap.insert( (const uint8_t*)"bbb", 3 );

  zmap.print( stdout );

  return 0;
*/
  {
    auto  mtcmap = mtc::zmap();
    auto  stdmap = std::map<std::string, uint32_t>();
    auto  newmap = mtc::xmap();
    auto  icount = (uint32_t)1000000;
/*    auto  tmbase = TestInsertStrToInt( [&]( const std::string& key, uint32_t val )
      {  (void)key, (void)val;  }, icount );
    auto  cctime = TestInsertStrToInt( [&]( const std::string& key, uint32_t val )
      {  stdmap.insert( { key, val } );  }, icount );*/
    auto  mytime = TestInsertStrToInt( [&]( const std::string& key, uint32_t val )
      {  mtcmap.set_word32( key, val );  }, icount );
    auto  notime = TestInsertStrToInt( [&]( const std::string& key, uint32_t val )
      {  newmap.put( key, val );  }, icount );

    auto  i1 = mtcmap.begin();
    auto  i2 = newmap.begin();

    while ( i1 != mtcmap.end() && i2 != newmap.end() )
    {
      if ( i1->first != i2->key )
      {
        fprintf( stderr, "keys differ: %s ? %s\n", i1->first.to_charstr(), i2->key.to_charstr() );
      }
      if ( i1->second != i2->value )
      {
        fprintf( stderr, "values differ: %s ? %s\n", i1->second.to_string().c_str(), i2->value.to_string().c_str() );
      }
//      assert( i1->second == i2->value );
      ++i1;
      ++i2;
    }

    fclose( mtcmap.Serialize( fopen( "dump.str.zmap", "wb" ) ) );
    fclose( newmap.Serialize( fopen( "dump.str.xmap", "wb" ) ) );
/*
    fprintf( stdout,
      "========= insert %u (string, uint) records =========\n"
      "\tmtc::zmap                    %ums\n"
      "\tstd::map<string, uint32_t>   %ums\n"
      "\tnode_t                       %ums\n",
        icount,
        mytime.count() - tmbase.count(),
        cctime.count() - tmbase.count(),
        notime.count() - tmbase.count() );*/
  }
  {
    auto  mtcmap = mtc::zmap();
    auto  stdmap = std::map<uint32_t, uint32_t>();
    auto  newmap = mtc::xmap();
    auto  icount = (uint32_t)1000000;
    /*
    auto  tmbase = TestInsertIntToInt( [&]( uint32_t key, uint32_t val )
      {  (void)key, (void)val;  }, icount );
    auto  cctime = TestInsertIntToInt( [&]( uint32_t key, uint32_t val )
      {  stdmap.insert( { key, val } );  }, icount );*/
    auto  mytime = TestInsertIntToInt( [&]( uint32_t key, uint32_t val )
      {  mtcmap.set_word32( key, val );  }, icount );
    auto  notime = TestInsertIntToInt( [&]( uint32_t key, uint32_t val )
      {
        newmap.put( mtc::zmap::key( key ), val );
      }, icount );

    fclose( mtcmap.Serialize( fopen( "dump.int.zmap", "wb" ) ) );
    fclose( newmap.Serialize( fopen( "dump.int.xmap", "wb" ) ) );
    /*
    fprintf( stdout,
      "========= insert %u (uint32_t, uint32_t) records =========\n"
      "\tmtc::zmap                    %ums\n"
      "\tstd::map<uint32_t, uint32_t> %ums\n"
      "\tnode_t                       %ums\n",
        icount,
        mytime.count() - tmbase.count(),
        cctime.count() - tmbase.count(),
        notime.count() - tmbase.count() );*/
  }
  return 0;
}
