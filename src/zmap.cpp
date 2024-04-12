# include "../zmap.h"
# include "../utf.hpp"
# include "../serialize.h"

namespace mtc
{

  namespace keys
  {

    inline  size_t  int_to_key( uint8_t* out, unsigned  key )
    {
      if ( (*out = (key >> 0x18)) != 0 )
      {
        *++out = (key >> 0x10);
        *++out = (key >> 0x08);
        *++out = (key >> 0x00);
        return 4;
      }
      if ( (*out = (key >> 0x10)) != 0 )
      {
        *++out = (key >> 0x08);
        *++out = (key >> 0x00);
        return 3;
      }
      if ( (*out = (key >> 0x08)) != 0 )
      {
        *++out = (key >> 0x00);
        return 2;
      }
      return (*out = key) != 0 ? 1 : 0;
    }

    inline  unsigned  key_to_int( const uint8_t* key, size_t len )
    {
      unsigned  out = 0;

      for ( auto end = key + len; key != end; )
        out = (out << 8) + (unsigned char)*key++;
      return out;
    }

  }

 /*
  * dump::const_iterator::dump_iterator
  * Реализация итератора по zmap::dump
  */
  class zmap::dump::const_iterator::dump_iterator
  {
    struct iterator_node
    {
      const char* pnext;  // next serialized element in list
      size_t      count;  // siblings in sequense
      const char* ptext;  // string fragment
      size_t      ltext;  // fragment length
      const char* value;  // the val serialization
      byte        xtype;  // key settings type
      const char* level;  // the next level

    public:
      iterator_node( const char* s )  {  init_element( s );  }

    public:
      bool  init_element( const char* );
      bool  move_to_next();
    };

    using element_stack = std::vector<iterator_node>;

    element_stack tree;
    array_byte    buff;
    iterator_data data;

  public:
    dump_iterator( const char* s = nullptr );
    dump_iterator( dump_iterator&& s ): tree( std::move( s.tree ) ) {}
    dump_iterator( const dump_iterator& s ): tree( s.tree ) {}

  public:
    auto  getvalue() const -> const iterator_data&  {  return data;  }
    bool  jumpnext();
    void  makedata();

    template <class T> static
    auto  exchange( T& t, const T& n ) -> T
      {
        auto  r( t );
        return t = n, r;
      }
  };

 /*
  * dump::const_iterator::zmap_iterator
  * Реализация обёртки итератора по zmap
  */
  class zmap::dump::const_iterator::zmap_iterator
  {
    zmap::const_iterator  iter;
    mutable iterator_data data;

  public:
    zmap_iterator( const zmap::const_iterator& zcit ): iter( zcit )  {}
    zmap_iterator( zmap_iterator&& s ): iter( std::move( s.iter ) ) {}
    zmap_iterator( const zmap_iterator& s ): iter( s.iter ) {}

  public:
    auto  getvalue() const -> const iterator_data&  {  return (data = { iter->first, iter->second });  }
    bool  jumpnext();
  };

  // zmap::dump implementation

  zmap::dump::dump( const dump& s ): source( s.source ), pvalue( s.pvalue )
  {
    if ( source != (const char*)-1 && pvalue != nullptr )
      ++*(int*)(1 + pvalue);
  }

  zmap::dump::dump( const zmap& z ): source( nullptr )
  {
    *(int*)(1 + (pvalue = new( new char[sizeof(zmap) + sizeof(int)] ) zmap( z ))) = 1;
  }

  zmap::dump::dump( const zmap* p ): source( (const char*)-1 ), pvalue( p )
    {}

  zmap::dump::~dump()
  {
    if ( pvalue != nullptr && source != (const char*)-1 && --*(int*)(1 + pvalue) == 0 )
      {  pvalue->~zmap(); delete [] (char*)pvalue;  }
  }

  auto  zmap::dump::operator = ( const dump& d ) -> dump&
  {
    if ( this != &d )
    {
      if ( pvalue != nullptr && source != (const char*)-1 && --*(int*)(1 + pvalue) == 0 )
        {  pvalue->~zmap(); delete [] (char*)pvalue;  }

      source = d.source;
      pvalue = d.pvalue;

      if ( pvalue != nullptr && source != (const char*)-1 )
        ++*(int*)(1 + pvalue);
    }

    return *this;
  }

  auto  zmap::dump::operator = ( const zmap& z ) -> dump&
  {
    if ( pvalue != nullptr && source != (const char*)-1 && --*(int*)(1 + pvalue) == 0 )
      {  pvalue->~zmap(); delete [] (char*)pvalue;  }

    *(int*)(1 + (pvalue = new( new char[sizeof(zmap) + sizeof(int)] ) zmap( z ))) = 1;

    return source = nullptr, *this;
  }

  auto  zmap::dump::operator = ( const zmap* z ) -> dump&
  {
    return source = (const char*)-1, pvalue = (zmap*)z, *this;
  }

  auto  zmap::dump::get_dump( const key& k ) const -> zval::dump
  {
    auto  pval = pvalue != nullptr ? pvalue->get( k ) : nullptr;

    return pval != nullptr ? zval::dump( pval ) : zval::dump( serial::find( source, k ) );
  }
    
  template <class T>
  auto  zmap::dump::get( const value_t<T>& v, const T& t ) -> const T&
  {
    return v != nullptr ? *v : t;
  }

  auto  zmap::dump::get( const key& k ) const -> zview_t
  {
    if ( pvalue != nullptr )
      return zview_t( nullptr, pvalue->get( k ) );

    if ( source != nullptr && source != (const char*)-1 )
      return zview_t( serial::find( source, k ), nullptr );

    return zview_t();
  }

  auto  zmap::dump::get_char( const key& k ) const -> value_t<char> {  return get_dump( k ).get_char();  }
  auto  zmap::dump::get_byte( const key& k ) const -> value_t<byte> {  return get_dump( k ).get_byte();  }
  auto  zmap::dump::get_int16( const key& k ) const -> value_t<int16_t> {  return get_dump( k ).get_int16();  }
  auto  zmap::dump::get_int32( const key& k ) const -> value_t<int32_t> {  return get_dump( k ).get_int32();  }
  auto  zmap::dump::get_int64( const key& k ) const -> value_t<int64_t> {  return get_dump( k ).get_int64();  }
  auto  zmap::dump::get_word16( const key& k ) const -> value_t<word16_t> {  return get_dump( k ).get_word16();  }
  auto  zmap::dump::get_word32( const key& k ) const -> value_t<word32_t> {  return get_dump( k ).get_word32();  }
  auto  zmap::dump::get_word64( const key& k ) const -> value_t<word64_t> {  return get_dump( k ).get_word64();  }
  auto  zmap::dump::get_float( const key& k ) const -> value_t<float> {  return get_dump( k ).get_float();  }
  auto  zmap::dump::get_double( const key& k ) const -> value_t<double> {  return get_dump( k ).get_double();  }
  auto  zmap::dump::get_charstr( const key& k ) const -> value_t<charstr> {  return get_dump( k ).get_charstr();  }
  auto  zmap::dump::get_widestr( const key& k ) const -> value_t<widestr> {  return get_dump( k ).get_widestr();  }
  auto  zmap::dump::get_uuid( const key& k ) const -> value_t<uuid> {  return get_dump( k ).get_uuid();  }
  auto  zmap::dump::get_zmap( const key& k ) const -> value_t<dump> {  return get_dump( k ).get_zmap();  }

  auto  zmap::dump::get_char( const key& k, char c ) const -> char  {  return get( get_char( k ), c );  }
  auto  zmap::dump::get_byte( const key& k, byte b ) const -> byte  {  return get( get_byte( k ), b );  }
  auto  zmap::dump::get_int16( const key& k, int16_t i ) const -> int16_t  {  return get( get_int16( k ), i );  }
  auto  zmap::dump::get_int32( const key& k, int32_t i ) const -> int32_t  {  return get( get_int32( k ), i );  }
  auto  zmap::dump::get_int64( const key& k, int64_t i ) const -> int64_t  {  return get( get_int64( k ), i );  }
  auto  zmap::dump::get_word16( const key& k, word16_t w ) const -> word16_t  {  return get( get_word16( k ), w );  }
  auto  zmap::dump::get_word32( const key& k, word32_t w ) const -> word32_t  {  return get( get_word32( k ), w );  }
  auto  zmap::dump::get_word64( const key& k, word64_t w ) const -> word64_t  {  return get( get_word64( k ), w );  }
  auto  zmap::dump::get_float( const key& k, float f ) const -> float  {  return get( get_float( k ), f );  }
  auto  zmap::dump::get_double( const key& k, double d ) const -> double  {  return get( get_double( k ), d );  }
  auto  zmap::dump::get_charstr( const key& k, const charstr& s ) const -> const charstr& {  return get( get_charstr( k ), s );  }
  auto  zmap::dump::get_widestr( const key& k, const widestr& w ) const -> const widestr& {  return get( get_widestr( k ), w );  }
  auto  zmap::dump::get_uuid( const key& k, const uuid& w ) const -> const uuid&  {  return get( get_uuid( k ), w );  }

  auto  zmap::dump::get_zmap( const key& k, const zmap& w ) const -> dump
  {
    auto  pd = get_zmap( k );

    return pd != nullptr ? *pd : dump( w );
  }

  auto  zmap::dump::get_array_char( const key& k ) const -> value_t<array_t<char>> {  return get_dump( k ).get_array_char();  }
  auto  zmap::dump::get_array_byte( const key& k ) const -> value_t<array_t<byte>> {  return get_dump( k ).get_array_byte();  }
  auto  zmap::dump::get_array_int16( const key& k ) const -> value_t<array_t<int16_t>> {  return get_dump( k ).get_array_int16();  }
  auto  zmap::dump::get_array_int32( const key& k ) const -> value_t<array_t<int32_t>> {  return get_dump( k ).get_array_int32();  }
  auto  zmap::dump::get_array_int64( const key& k ) const -> value_t<array_t<int64_t>> {  return get_dump( k ).get_array_int64();  }
  auto  zmap::dump::get_array_word16( const key& k ) const -> value_t<array_t<word16_t>> {  return get_dump( k ).get_array_word16();  }
  auto  zmap::dump::get_array_word32( const key& k ) const -> value_t<array_t<word32_t>> {  return get_dump( k ).get_array_word32();  }
  auto  zmap::dump::get_array_word64( const key& k ) const -> value_t<array_t<word64_t>> {  return get_dump( k ).get_array_word64();  }
  auto  zmap::dump::get_array_float( const key& k ) const -> value_t<array_t<float>> {  return get_dump( k ).get_array_float();  }
  auto  zmap::dump::get_array_double( const key& k ) const -> value_t<array_t<double>> {  return get_dump( k ).get_array_double();  }
  auto  zmap::dump::get_array_charstr( const key& k ) const -> value_t<array_t<charstr>> {  return get_dump( k ).get_array_charstr();  }
  auto  zmap::dump::get_array_widestr( const key& k ) const -> value_t<array_t<widestr>> {  return get_dump( k ).get_array_widestr();  }
  auto  zmap::dump::get_array_uuid( const key& k ) const -> value_t<array_t<uuid>> {  return get_dump( k ).get_array_uuid();  }
  auto  zmap::dump::get_array_zval( const key& k ) const -> value_t<array_t<zval::dump, zval>> {  return get_dump( k ).get_array_zval();  }
  auto  zmap::dump::get_array_zmap( const key& k ) const -> value_t<array_t<zmap::dump, zmap>> {  return get_dump( k ).get_array_zmap();  }

  # define derive_get_array_ref( _type_ ) \
  auto  zmap::dump::get_array_##_type_( const key& k, const array_##_type_& a ) const -> array_t<_type_##_t>  \
  {                                                                                                           \
    auto  pvalue = get_dump( k ).get_array_##_type_();                                                        \
    if ( pvalue != nullptr )  return *pvalue;                                                                 \
      else return array_t<_type_##_t>( &a );                                                                  \
  }

  derive_get_array_ref( char )
  derive_get_array_ref( byte )
  derive_get_array_ref( int16 )
  derive_get_array_ref( int32 )
  derive_get_array_ref( int64 )
  derive_get_array_ref( word16 )
  derive_get_array_ref( word32 )
  derive_get_array_ref( word64 )
  derive_get_array_ref( float )
  derive_get_array_ref( double )
  derive_get_array_ref( charstr )
  derive_get_array_ref( widestr )
  derive_get_array_ref( uuid )

  auto  zmap::dump::get_array_zval( const key& k, const array_zval& a ) const -> array_t<zval::dump, zval>
  {
    auto  pvalue = get_dump( k ).get_array_zval();

    if ( pvalue != nullptr )  return *pvalue;
      else return array_t<zval::dump, zval>( &a );
  }

  auto  zmap::dump::get_array_zmap( const key& k, const array_zmap& a ) const -> array_t<zmap::dump, zmap>
  {
    auto  pvalue = get_dump( k ).get_array_zmap();

    if ( pvalue != nullptr )  return *pvalue;
      else return array_t<zmap::dump, zmap>( &a );
  }

  bool  zmap::dump::operator == ( const dump& z ) const
  {
    auto  mp = begin();
    auto  zp = z.begin();

    while ( mp != end() && zp != z.end() )
      if ( mp->first != zp->first || mp->second != zp->second ) return false;
        else ++mp, ++zp;

    return mp == end() && zp == z.end();
  }

  zmap::dump::operator zmap() const
  {
    if ( pvalue == nullptr )
    {
      zmap  v;

      if ( source != nullptr )
        v.FetchFrom( source );
      return v;
    }
    return *pvalue;
  }

  auto  zmap::dump::begin() const -> const_iterator
  {
    if ( pvalue != nullptr )
    {
      auto  it = pvalue->cbegin();

      if ( it != pvalue->cend() ) return { it };
    }
    if ( source != nullptr && source != (const char*)-1 )
      return { source };
    return const_iterator();
  }

  auto  zmap::dump::end() const -> const_iterator {  return {};  }
  auto  zmap::dump::cbegin() const -> const_iterator {  return begin();  }
  auto  zmap::dump::cend() const -> const_iterator {  return {};  }

 /*
  * zmap::dump::const_iterator
  */

  zmap::dump::const_iterator::const_iterator( const char* source )
  {
    asDump = std::make_shared<dump_iterator>( source );
  }

  zmap::dump::const_iterator::const_iterator( const zmap::const_iterator& zi )
  {
    asZmap = std::make_shared<zmap_iterator>( zi );
  }

  bool  zmap::dump::const_iterator::operator == ( const const_iterator& match ) const
  {
    if ( !is_empty() && !match.is_empty() )
    {
      auto& me = this->asDump != nullptr ? this->asDump->getvalue() : this->asZmap->getvalue();
      auto& to = match.asDump != nullptr ? match.asDump->getvalue() : match.asZmap->getvalue();

      return me.first == to.first && me.second == to.second;
    }
    return is_empty() && match.is_empty();
  }

  auto  zmap::dump::const_iterator::operator ++() -> const_iterator&
  {
    if ( asDump != nullptr )  {  if ( !asDump->jumpnext() ) asDump = nullptr;  }
      else
    if ( asZmap != nullptr )  {  if ( !asZmap->jumpnext() ) asZmap = nullptr;  }
      else  throw std::logic_error( "invalid iterator call" );
    return *this;
  }

  auto  zmap::dump::const_iterator::operator ++(int) -> const_iterator
  {
    auto  prev( *this );  return operator ++(), prev;
  }

  auto  zmap::dump::const_iterator::operator -> () const -> const iterator_data*
  {
    return asDump != nullptr ? &asDump->getvalue() : &asZmap->getvalue();
  }

  auto  zmap::dump::const_iterator::operator * () const -> const iterator_data&
  {
    return asDump != nullptr ? asDump->getvalue() : asZmap->getvalue();
  }

 /*
  * iterator_node::iterator_node( source )
  *
  * загружает сериализованный узел дерева ztree, переводит ptr на следующий элемент
  * или nullptr, если в этой ветке более нечего грузить, и возвращает указатель
  * на первый вложенный элемент.
  */
  bool  zmap::dump::const_iterator::dump_iterator::iterator_node::init_element( const char* s )
  {
    word32_t  lfetch;
    size_t    sublen;

    s = ::FetchFrom( s, lfetch );

    if ( (lfetch & 0x0200) != 0 )       // check if value at node; save value pointer
      s = zval::SkipToEnd( value = ::FetchFrom( s, xtype ) );
    else {  value = nullptr; xtype = (byte)-1;  }

    if ( (lfetch & 0x0400) != 0 )       // check if patricia-style element
    {
      pnext = nullptr;
      count = 0;
      level = (ptext = s) + (ltext = fragment_len( lfetch ));
    }
      else
    if ( (count = lfetch & 0x1ff) != 0 )
    {
      --count;
        level = ::FetchFrom( (ptext = s) + (ltext = 1), sublen );
        pnext = level + sublen;
    }
      else
    {
      ptext = pnext = level = nullptr;
      ltext = 0;
    }
    return true;
  }

  bool  zmap::dump::const_iterator::dump_iterator::iterator_node::move_to_next()
  {
    size_t  sublen;

    if ( count != 0 )
    {
      --count;
        level = ::FetchFrom( (ptext = pnext) + 1, sublen );
        pnext = level + sublen;
      return true;
    }

    return false;
  }

  zmap::dump::const_iterator::dump_iterator::dump_iterator( const char* s )
  {
    if ( s != nullptr )
    {
      tree.push_back( iterator_node( s ) );

      while ( tree.back().value == nullptr && tree.back().level != nullptr )
      {
        buff.insert( buff.end(), tree.back().ptext, tree.back().ptext + tree.back().ltext );
        tree.push_back( iterator_node( exchange( tree.back().level, (const char*)nullptr ) ) );
      }

      buff.push_back( '\0' ), makedata();
    }
  }

  bool  zmap::dump::const_iterator::dump_iterator::jumpnext()
  {
    if ( !tree.empty() )
    {
      buff.resize( buff.size() - (buff.empty() ? 0 : 1) );  // remove trailing '\0'

      for ( ; !tree.empty(); )
      {
        if ( tree.back().level != nullptr )
        {
          do  {
            buff.insert( buff.end(), tree.back().ptext, tree.back().ptext + tree.back().ltext );
            tree.push_back( iterator_node( exchange( tree.back().level, (const char*)nullptr ) ) );
          } while ( tree.back().value == nullptr && tree.back().level != nullptr );

          break;
        }

        if ( tree.back().move_to_next() )
        {
          while ( tree.back().value == nullptr && tree.back().level != nullptr )
          {
            buff.insert( buff.end(), tree.back().ptext, tree.back().ptext + tree.back().ltext );
            tree.push_back( iterator_node( exchange( tree.back().level, (const char*)nullptr ) ) );
          }

          break;
        }
          else
        {
          tree.pop_back();
          buff.resize( buff.size() - (tree.empty() ? 0 : tree.back().ltext) );
        }
      }
      if ( buff.size() != 0 )
        return buff.push_back( '\0' ), makedata(), true;
    }
    return false;
  }

  void  zmap::dump::const_iterator::dump_iterator::makedata()
  {
    if ( tree.empty() != 0 )  data = { key(), zval::dump() };
      else data = { key( tree.back().xtype, buff.data(), buff.size() - 1 ), zval::dump( tree.back().value ) };
  }

  bool  zmap::dump::const_iterator::zmap_iterator::jumpnext()
  {
    return iter != zmap::const_iterator() && ++iter != zmap::const_iterator();
  }

  // zmap::key implementation

  zmap::key::key(): _typ( none ), _psz( nullptr ), _len( 0 )  {}

  zmap::key::key( unsigned t, const uint8_t* b, size_t l ): _typ( t ), _len( l )
  {
    if ( _typ == 0 )  _psz = (const uint8_t*)memcpy( _buf, b, l );
      else _psz = b;
  }

  zmap::key::key( unsigned k ): _typ( uint ), _psz( _buf ), _len( keys::int_to_key( _buf, k ) )  {}
  zmap::key::key( const char* k ): _typ( cstr ), _psz( (const uint8_t*)k ), _len( w_strlen( k ) ) {}
  zmap::key::key( const widechar* k ): _typ( wstr ), _psz( (const uint8_t*)k ), _len( sizeof(widechar) * w_strlen( k ) )  {}
  zmap::key::key( const char* k, size_t l ): _typ( cstr ), _psz( (const uint8_t*)k ), _len( l ) {}
  zmap::key::key( const widechar* k, size_t l ): _typ( wstr ), _psz( (const uint8_t*)k ), _len( l )  {}
  zmap::key::key( const charstr& k ): _typ( cstr ), _psz( (const uint8_t*)k.c_str() ), _len( k.length() ) {}
  zmap::key::key( const widestr& k ): _typ( wstr ), _psz( (const uint8_t*)k.c_str() ), _len( sizeof(widechar) * k.length() )  {}
  zmap::key::key( const key& k ): _typ( k._typ ), _psz( k._psz ), _len( k._len )
    {  if ( _typ == 0 ) _psz = (const uint8_t*)memcpy( _buf, k._buf, sizeof(k._buf) );  }
  zmap::key&  zmap::key::operator= ( const key& k )
  {
    _typ = k._typ;
    _len = k._len;
    if ( k._psz == k._buf ) _psz = (const uint8_t*)memcpy( _buf, k._buf, k._len );
      else _psz = k._psz;
    return *this;
  }

  auto  zmap::key::operator == ( const key& k ) const -> bool
  {
    if ( _typ != k._typ )
      return false;
    switch ( _typ )
    {
      case uint:  return (unsigned)*this == (unsigned)k;
      case cstr:  return w_strcmp( (const char*)*this, (const char*)k ) == 0;
      case wstr:  return w_strcmp( (const widechar*)*this, (const widechar*)k ) == 0;
      default  :  return true;
    }
  }

  auto  zmap::key::compare( const key& k ) const -> int
  {
    int   rc;

    if ( (rc = _typ - k._typ) != 0 )
      return rc;
    switch ( _typ )
      {
        case uint:  return (unsigned)*this - (unsigned)k;
        case cstr:  return w_strcmp( (const char*)*this, (const char*)k );
        case wstr:  return w_strcmp( (const widechar*)*this, (const widechar*)k );
        default  :  throw std::logic_error( "undefined key type for compare" );
      }
  }

  zmap::key::operator unsigned () const {  return _typ == uint ? keys::key_to_int( _psz, _len ) : 0;  }
  zmap::key::operator const char* () const {  return _typ == cstr ? (const char*)_psz : nullptr;  }
  zmap::key::operator const widechar* () const {  return _typ == wstr ? (const widechar*)_wsz : nullptr;  }

  /*
    zmap::z_tree implementation
  */

  zmap::ztree_t::ztree_t( byte_t ch ):
    std::vector<ztree_t>(), chnode( ch ), keyset( key::none ) {}

  zmap::ztree_t::ztree_t( ztree_t&& zt ):
    std::vector<ztree_t>( std::move( zt ) ), chnode( zt.chnode ), keyset( zt.keyset ), pvalue( std::move( zt.pvalue ) ) {  zt.keyset = key::none;  }

  zmap::ztree_t&  zmap::ztree_t::operator = ( ztree_t&& zt )
  {
    std::vector<ztree_t>::operator=( std::move( zt ) );
    chnode = zt.chnode;
    keyset = zt.keyset;  zt.keyset = key::none;
    pvalue = std::move( zt.pvalue );
    return *this;
  }

  auto  zmap::ztree_t::insert( const uint8_t* key, size_t cch ) -> zmap::ztree_t*
  {
    for ( auto expand = this; ; ++key, --cch )
    {
      if ( cch > 0 )
      {
        uint8_t chnext = *key;
        auto    ptrtop = expand->begin();
        auto    ptrend = expand->end();

        while ( ptrtop < ptrend && ptrtop->chnode < chnext )
          ++ptrtop;
        if ( ptrtop >= ptrend || ptrtop->chnode != chnext )
          ptrtop = static_cast<std::vector<ztree_t>&>( *expand ).insert( ptrtop, ztree_t( chnext ) );
        expand = expand->data() + (ptrtop - expand->begin());
      }
        else
      return expand;
    }
  }

  auto  zmap::ztree_t::remove( const uint8_t* key, size_t cch ) -> size_t
  {
    if ( cch > 0 )
    {
      auto  chr = *key;
      auto  top = this->begin();
      auto  end = this->end();

      while ( top != end && top->chnode < chr )
        ++top;

      if ( top == end || top->chnode != chr )
        return 0;

      if ( top->remove( key + 1, cch - 1 ) == 0 )
        return 0;

      if ( top->size() == 0 && top->pvalue == nullptr )
        this->erase( top );

      return 1;
    }
      else
    if ( pvalue != nullptr )
    {
      keyset = key::none;
      pvalue.reset();
      return 1;
    }
    return 0;
  }

  template <class self>
  auto  zmap::ztree_t::search( self& _me, const uint8_t* key, size_t cch ) -> self*
  {
    auto  ptop = &_me;

    while ( cch-- > 0 )
    {
      auto  chr = *key++;
      auto  top = ptop->begin();
      auto  end = ptop->end();

      while ( top != end && top->chnode < chr )
        ++top;
      if ( top == end || top->chnode != chr )
        return nullptr;

      ptop = ptop->data() + (top - ptop->begin());
    }
    return ptop;
  }

  auto  zmap::ztree_t::copy() const -> ztree_t
  {
    ztree_t mkcopy( chnode );

    for ( auto ptr = begin(); ptr != end(); ++ptr )
      mkcopy.push_back( ptr->copy() );

    mkcopy.keyset = keyset;

    if ( pvalue != nullptr )
      mkcopy.pvalue = std::unique_ptr<zval>( new zval( *pvalue.get(), zval::force_copy() ) );

    return mkcopy;
  }

  auto  zmap::ztree_t::plain_branchlen() const -> int
  {
    const ztree_t*  pbeg;
    int             size = 0;

    for ( size = 0, pbeg = this; pbeg->size() == 1 && pbeg->pvalue == nullptr; pbeg = pbeg->data() )
      ++size;
    return size;
  }

  auto  zmap::ztree_t::plain_ctl_bytes() const -> word32_t
  {
    auto  lplain = plain_branchlen();
      assert( lplain <= 0x3fffffff );
      assert( size() <= 0x100 );
    auto  lbytes = static_cast<word32_t>( lplain > 0 ? 0x400 + (lplain & 0x1ff) + ((lplain << 2) & ~0x7ff) : size() );

    return (pvalue != nullptr ? 0x0200 : 0) + lbytes;
  }

  /*
    zmap::zdata_t implementation
  */

  zmap::zdata_t::zdata_t():
    n_vals( 0 ), nrefer( 0 )  {}

  zmap::zdata_t::zdata_t( ztree_t&& t, size_t n ):
    ztree_t( std::move( t ) ), n_vals( n ), nrefer( 0 ) {}

  long  zmap::zdata_t::attach()
  {
    return ++nrefer;
  }

  long  zmap::zdata_t::detach()
  {
    auto  rcount = --nrefer;

    if ( rcount == 0 )
      delete this;
    return rcount;
  }

 /*
  * for copy-on-write, returns 1-counted object pointer which is:
  *   either itself, if the object is 1-counted,
  *   or a copy of existing object, if it is >1-counted
  */
  auto  zmap::zdata_t::docopy() -> zdata_t*
  {
    zdata_t*  pcopy;

    assert( _refer > 0 );

    if ( nrefer == 1 )
      return this;

    (pcopy = new zdata_t( ztree_t::copy(), n_vals ))->nrefer = 1;
      --nrefer;

    return pcopy;
  }

  /*
    zmap::zbuff_t implementation
  */
  void  zmap::zbuff_t::push_back( char ch )
    {
      assert( empty() || inherited::size() >= 3 );

      if ( inherited::empty() ) {  inherited::push_back( ch );  inherited::push_back( 0 );  }
        else at( inherited::size() - 2 ) = ch;

      inherited::push_back( 0 );
    }

  void  zmap::zbuff_t::pop_back()
    {
      assert( inherited::size() >= 3 );

      inherited::at( inherited::size() - 3 ) = 0;
      inherited::pop_back();
    }

  auto  zmap::zbuff_t::back() -> char&
    {
      assert( inherited::size() >= 3 );

      return inherited::at( inherited::size() - 3 );
    }

  auto  zmap::zbuff_t::back() const -> char
    {
      assert( inherited::size() >= 3 );

      return inherited::at( inherited::size() - 3 );
    }

  auto  zmap::zbuff_t::size() const -> size_t
    {
      assert( inherited::size() >= 3 );

      return inherited::size() - 2;
    }

  auto  zmap::zbuff_t::data() const -> const char*
    {
      return inherited::data();
    }

  /*
    zmap::const_place_t implementation
  */
  /*
  template <class out, class val, class act>
  out   map_value( val& ref, act map )
    {
      auto  p = map( ref );
      return p != nullptr ? (out)*p : out();
    }
  template <class out, class val, class act, class... set>
  out   map_value( val& ref, act map, set... lst )
    {
      auto  p = map( ref );
      return p != nullptr ? (out)*p : map_value<out>( ref, lst... );
    }
  template <class out, class val, class... act>
  out  get_operator( val& fld, const zmap::key& key, act... set )
    {
      auto  pzv = fld.get( key ); // parent handler
      return pzv != nullptr ? map_value<out>( *pzv, set... ) : out();
    }

  zmap::const_place_t::operator char() const
    {  return get_operator<char>( owner, refer,
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator int16_t() const
    {  return get_operator<int16_t>( owner, refer,
        []( const zval& v ){  return v.get_int16(); },
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator int32_t() const
    {  return get_operator<int32_t>( owner, refer,
        []( const zval& v ){  return v.get_int32(); },
        []( const zval& v ){  return v.get_word16(); },
        []( const zval& v ){  return v.get_int16(); },
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator int64_t() const
    {  return get_operator<int64_t>( owner, refer,
        []( const zval& v ){  return v.get_int64(); },
        []( const zval& v ){  return v.get_word32(); },
        []( const zval& v ){  return v.get_int32(); },
        []( const zval& v ){  return v.get_word16(); },
        []( const zval& v ){  return v.get_int16(); },
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator byte_t() const
    {  return get_operator<const char>( owner, refer,
        []( const zval& v ){  return v.get_byte();  } );  }
  zmap::const_place_t::operator word16_t() const
    {  return get_operator<word16_t>( owner, refer,
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator word32_t() const
    {  return get_operator<word32_t>( owner, refer,
        []( const zval& v ){  return v.get_word32(); },
        []( const zval& v ){  return v.get_word16(); },
        []( const zval& v ){  return v.get_int16(); },
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator word64_t() const
    {  return get_operator<word64_t>( owner, refer,
        []( const zval& v ){  return v.get_word64(); },
        []( const zval& v ){  return v.get_word32(); },
        []( const zval& v ){  return v.get_int32(); },
        []( const zval& v ){  return v.get_word16(); },
        []( const zval& v ){  return v.get_int16(); },
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator float_t() const
    {  return get_operator<float>( owner, refer,
        []( const zval& v ){  return v.get_float(); },
        []( const zval& v ){  return v.get_word64(); },
        []( const zval& v ){  return v.get_int64(); },
        []( const zval& v ){  return v.get_word32(); },
        []( const zval& v ){  return v.get_int32(); },
        []( const zval& v ){  return v.get_word16(); },
        []( const zval& v ){  return v.get_int16(); },
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }
  zmap::const_place_t::operator double_t() const
    {  return get_operator<double>( owner, refer,
        []( const zval& v ){  return v.get_double(); },
        []( const zval& v ){  return v.get_float(); },
        []( const zval& v ){  return v.get_word64(); },
        []( const zval& v ){  return v.get_int64(); },
        []( const zval& v ){  return v.get_word32(); },
        []( const zval& v ){  return v.get_int32(); },
        []( const zval& v ){  return v.get_word16(); },
        []( const zval& v ){  return v.get_int16(); },
        []( const zval& v ){  return v.get_byte();  },
        []( const zval& v ){  return v.get_char();  } );  }

  zmap::const_place_t::operator charstr() const
    {
      auto  pstr = owner.get_charstr( refer );
      return pstr != nullptr ? *pstr : "";
    }

  zmap::const_place_t::operator widestr() const
    {
      auto  pstr = owner.get_widestr( refer );
      widechar  zero( 0 );
      return pstr != nullptr ? *pstr : &zero;
    }

  zmap::const_place_t::operator const zmap& () const
    {
      auto  pmap = owner.get_zmap( refer );   // parent handler
      return pmap != nullptr ? *pmap : empty;
    }

  bool  zmap::const_place_t::operator == ( const charstr& s ) const
    {
      auto  pstr = owner.get_charstr( refer );
      return pstr != nullptr && *pstr == s;
    }

  bool  zmap::const_place_t::operator == ( const widestr& s ) const
    {
      auto  pstr = owner.get_widestr( refer );
      return pstr != nullptr && *pstr == s;
    }
  */

  /*
    zmap::patch_place_t
  */
  auto  zmap::patch_place_t::operator= ( zval&& v ) -> zmap::patch_place_t&
    {  return owner.put( refer, std::move( v ) ), *this;  }

  auto  zmap::patch_place_t::operator= ( const zval& v ) -> zmap::patch_place_t&
    {  return owner.put( refer, v ), *this;  }

  /*
    zmap implementation
  */

  zmap::zmap( zmap&& z ): p_data( z.p_data )
  {
    z.p_data = nullptr;
  }

  zmap::zmap( const zmap& z ): p_data( z.p_data )
  {
    if ( p_data != nullptr )
      p_data->attach();
  }

  zmap::zmap( const std::initializer_list<std::pair<key, zval>>& il ): zmap()
  {
    for ( auto& keyval: il )
      put( keyval.first, keyval.second );
  }

  zmap::zmap( const zmap& from, const std::initializer_list<std::pair<key, zval>>& il ): zmap( from )
  {
    for ( auto& keyval: il )
      put( keyval.first, keyval.second );
  }

  zmap& zmap::operator=( zmap&& z )
  {
    if ( p_data != nullptr )
      p_data->detach();
    if ( (p_data = z.p_data) != nullptr )
      z.p_data = nullptr;
    return *this;
  }

  zmap& zmap::operator=( const zmap& z )
  {
    if ( p_data != nullptr )
      p_data->detach();
    if ( (p_data = z.p_data) != nullptr )
      p_data->attach();
    return *this;
  }

  zmap& zmap::operator= ( const std::initializer_list<std::pair<key, zval>>& il )
  {
    clear();

    for ( auto& keyval: il )
      put( keyval.first, keyval.second );

    return *this;
  }

  zmap::~zmap()
  {
    clear();
  }

  auto  zmap::private_data() -> zdata_t*
  {
    if ( p_data == nullptr )  (p_data = new zdata_t())->attach();
      else p_data = p_data->docopy();
    return p_data;
  }

  auto  zmap::put( const key& k, zval&& v ) -> zval*
  {
    auto      mydata = private_data();
    ztree_t*  pfound;

    if ( (pfound = mydata->insert( k.data(), k.size() ))->pvalue == nullptr )
    {
      pfound->pvalue = std::unique_ptr<zval>( new zval( std::move( v ) ) );
      ++mydata->n_vals;
    }
      else
    {
      *pfound->pvalue = std::move( v );
    }
    pfound->keyset = k.type();

    return pfound->pvalue.get();
  }

  auto  zmap::put( const key& k, const zval& v ) -> zval*
  {
    auto      mydata = private_data();
    ztree_t*  pfound;

    if ( (pfound = mydata->insert( k.data(), k.size() ))->pvalue == nullptr )
    {
      pfound->pvalue = std::unique_ptr<zval>( new zval( v ) );
      ++mydata->n_vals;
    }
      else
    {
      *pfound->pvalue = v;
    }
    pfound->keyset = k.type();

    return pfound->pvalue.get();
  }

  auto  zmap::get( const key& k ) const -> const zval*
  {
    const ztree_t*  p_tree;

    if ( p_data != nullptr && (p_tree = p_data->search( k.data(), k.size() )) != nullptr )
      return p_tree->pvalue.get();
    return nullptr;
  }

  auto  zmap::get( const key& k ) -> zval*
  {
    ztree_t*  p_tree;

    if ( p_data != nullptr && (p_tree = p_data->search( k.data(), k.size() )) != nullptr )
      return p_tree->pvalue.get();
    return nullptr;
  }

  auto  zmap::get_type( const key& k ) const -> decltype(zval::vx_type)
  {
    auto  pv = get( k );

    return pv != nullptr ? pv->get_type() : decltype(zval::vx_type)(zval::z_untyped);
  }

  /* zmap get_xxx/set_xxx impl */

  # define derive_get_type( _type_ )                                                  \
  auto  zmap::get_##_type_( const key& k ) -> _type_##_t*                             \
    {                                                                                 \
      auto  pv = get( k );                                                            \
      return pv != nullptr ? pv->get_##_type_() : nullptr;                            \
    }                                                                                 \
  auto  zmap::get_##_type_( const key& k ) const -> const _type_##_t*                 \
    {                                                                                 \
      auto  pv = get( k );                                                            \
      return pv != nullptr ? pv->get_##_type_() : nullptr;                            \
    }
  # define derive_get_init_val( _type_ )                                              \
  auto  zmap::get_##_type_( const key& k, _type_##_t t ) const -> _type_##_t          \
    {                                                                                 \
      auto  pv = get_##_type_( k );                                                   \
      return pv != nullptr ? *pv : t;                                                 \
    }
  # define derive_get_init_ref( _type_ )                                              \
  auto  zmap::get_##_type_( const key& k, const _type_##_t& t ) const -> const _type_##_t&  \
    {                                                                                 \
      auto  pv = get_##_type_( k );                                                   \
      return pv != nullptr ? *pv : t;                                                 \
    }

   /*
    * get_xxx( key ) [const] -> [const] xxx* family
    */
    derive_get_type( char   )
    derive_get_type( byte   )
    derive_get_type( int16  )
    derive_get_type( int32  )
    derive_get_type( int64  )
    derive_get_type( word16 )
    derive_get_type( word32 )
    derive_get_type( word64 )
    derive_get_type( float  )
    derive_get_type( double )
    derive_get_type( bool   )

    derive_get_type( charstr )
    derive_get_type( widestr )
    derive_get_type( uuid    )
    derive_get_type( zmap    )

    derive_get_type( array_char )
    derive_get_type( array_byte    )
    derive_get_type( array_int16   )
    derive_get_type( array_int32   )
    derive_get_type( array_int64   )
    derive_get_type( array_word16  )
    derive_get_type( array_word32  )
    derive_get_type( array_word64  )
    derive_get_type( array_float   )
    derive_get_type( array_double  )
    derive_get_type( array_charstr )
    derive_get_type( array_widestr )
    derive_get_type( array_uuid    )
    derive_get_type( array_zval    )
    derive_get_type( array_zmap    )

   /*
    * get_xxx( key, value ) -> value family
    */
    derive_get_init_val( char   )
    derive_get_init_val( byte   )
    derive_get_init_val( int16  )
    derive_get_init_val( int32  )
    derive_get_init_val( int64  )
    derive_get_init_val( word16 )
    derive_get_init_val( word32 )
    derive_get_init_val( word64 )
    derive_get_init_val( float  )
    derive_get_init_val( double )
    derive_get_init_val( bool   )

    derive_get_init_ref( charstr )
    derive_get_init_ref( widestr )
    derive_get_init_ref( uuid    )
    derive_get_init_ref( zmap    )

    derive_get_init_ref( array_char )
    derive_get_init_ref( array_byte    )
    derive_get_init_ref( array_int16   )
    derive_get_init_ref( array_int32   )
    derive_get_init_ref( array_int64   )
    derive_get_init_ref( array_word16  )
    derive_get_init_ref( array_word32  )
    derive_get_init_ref( array_word64  )
    derive_get_init_ref( array_float   )
    derive_get_init_ref( array_double  )
    derive_get_init_ref( array_charstr )
    derive_get_init_ref( array_widestr )
    derive_get_init_ref( array_zmap    )
    derive_get_init_ref( array_zval    )
    derive_get_init_ref( array_uuid    )

  # undef derive_get_init_ref
  # undef derive_get_init_val
  # undef derive_get_type

  # define derive_set( _type_ )                                                       \
  auto  zmap::set_##_type_( const key& k ) -> _type_##_t*                             \
    {  return put( k )->set_##_type_();  }

  # define derive_set_val( _type_ )                                                   \
  auto  zmap::set_##_type_( const key& k, _type_##_t t ) -> _type_##_t*               \
    {  return put( k, t )->get_##_type_();  }

  # define derive_set_ref( _type_ )                                                   \
  auto  zmap::set_##_type_( const key& k, const _type_##_t& t ) -> _type_##_t*        \
    {  return put( k, zval( t ) )->get_##_type_();  }                                 \
  auto  zmap::set_##_type_( const key& k, _type_##_t&& t ) -> _type_##_t*             \
    {  return put( k, zval( std::move( t ) ) )->get_##_type_();  }

    derive_set( char    )
    derive_set( byte    )
    derive_set( int16   )
    derive_set( int32   )
    derive_set( int64   )
    derive_set( word16  )
    derive_set( word32  )
    derive_set( word64  )
    derive_set( float   )
    derive_set( double  )
    derive_set( bool )

    derive_set( charstr )
    derive_set( widestr )
    derive_set( uuid )
    derive_set( zmap )
    derive_set( array_char )
    derive_set( array_byte )
    derive_set( array_int16 )
    derive_set( array_int32 )
    derive_set( array_int64 )
    derive_set( array_word16 )
    derive_set( array_word32 )
    derive_set( array_word64 )
    derive_set( array_float )
    derive_set( array_double )
    derive_set( array_charstr )
    derive_set( array_widestr )
    derive_set( array_zval )
    derive_set( array_uuid )
    derive_set( array_zmap )

    derive_set_val( char   )
    derive_set_val( byte   )
    derive_set_val( int16  )
    derive_set_val( int32  )
    derive_set_val( int64  )
    derive_set_val( word16 )
    derive_set_val( word32 )
    derive_set_val( word64 )
    derive_set_val( float  )
    derive_set_val( double )
    derive_set_val( bool   )

    derive_set_ref( charstr )
    derive_set_ref( widestr )

    auto  zmap::set_charstr( const key& k, const char* psz, size_t len ) -> charstr*
    {
      return put( k, zval( psz, len ) )->get_charstr();
    }

    auto  zmap::set_widestr( const key& k, const widechar* pws, size_t len ) -> widestr*
    {
      return put( k, zval( pws, len ) )->get_widestr();
    }

    derive_set_ref( uuid )
    derive_set_ref( zmap )
    derive_set_ref( array_char )
    derive_set_ref( array_byte )
    derive_set_ref( array_int16 )
    derive_set_ref( array_int32 )
    derive_set_ref( array_int64 )
    derive_set_ref( array_word16 )
    derive_set_ref( array_word32 )
    derive_set_ref( array_word64 )
    derive_set_ref( array_float )
    derive_set_ref( array_double )
    derive_set_ref( array_charstr )
    derive_set_ref( array_widestr )
    derive_set_ref( array_zval )
    derive_set_ref( array_uuid )
    derive_set_ref( array_zmap )

  # undef derive_set_ref
  # undef derive_set_val
  # undef derive_set

  auto  zmap::empty() const -> bool
  {
    return size() == 0;
  }

  auto  zmap::size() const -> size_t
  {
    return p_data != nullptr ? p_data->n_vals : 0;
  }

  // !!! iterator would block the map
  zmap::iterator  zmap::begin()
  {
    return p_data != nullptr ? iterator( p_data->begin(), p_data->end() ) : iterator();
  }

  zmap::iterator  zmap::end()
  {
    return iterator();
  }

  zmap::const_iterator  zmap::cbegin() const
  {
    return p_data != nullptr ? const_iterator( p_data->begin(), p_data->end() ) : const_iterator();
  }

  zmap::const_iterator  zmap::cend() const
  {
    return const_iterator();
  }

  zmap::const_iterator  zmap::begin() const
  {
    return cbegin();
  }

  zmap::const_iterator  zmap::end() const
  {
    return cend();
  }

  auto  zmap::at( const key& k ) -> zval&
    {
      auto  pval = get( k );

      if ( pval == nullptr )
        throw std::out_of_range( "key has no match in zmap" );
      return *pval;
    }

  auto  zmap::at( const key& k ) const -> const zval&
    {
      auto  pval = get( k );

      if ( pval == nullptr )
        throw std::out_of_range( "key has no match in zmap" );
      return *pval;
    }

  auto  zmap::operator []( const key& k ) const -> const const_place_t
    {  return const_place_t( k, *this );  }

  auto  zmap::operator []( const key& k ) -> patch_place_t
    {  return patch_place_t( k, *this );  }

  auto  zmap::copy() const -> mtc::zmap
  {
    auto  newmap = zmap();

    if ( p_data != nullptr )
      (newmap.p_data = new zdata_t( p_data->copy(), p_data->n_vals ))->nrefer = 1;

    return newmap;
  }

  auto  zmap::clear() -> void
  {
    if ( p_data != nullptr )
      p_data->detach();
    p_data = nullptr;
  }

  auto  zmap::erase( const key& k ) -> size_t
  {
    auto  n_dels = size_t(0);

    if ( p_data != nullptr )
    {
      p_data = p_data->docopy();

      if ( (p_data->n_vals -= (n_dels = p_data->remove( k.data(), k.size() ))) == 0 )
      {
        p_data->detach();
        p_data = nullptr;
      }
    }

    return n_dels;
  }

  int   zmap::compare( const mtc::zmap& z ) const
  {
    auto  i1 = cbegin();
    auto  i2 = z.cbegin();
    int   rc;

    for ( ; i1 != cend() && i2 != z.cend(); ++i1, ++i2 )
    {
      if ( (rc = i1->first.compare( i2->first )) != 0 )
        return rc;
      if ( (rc = i1->second.compare( i2->second )) != 0 )
        return rc;
    }
    return (i1 != cend()) - (i2 != z.cend());
  }

  auto  to_string( const zmap::key& key ) -> std::string
    {
      unsigned  u_k = key;
      const char* psz = key;
      const widechar* wsz = key;

      if ( psz != nullptr )
        return '"' + std::string( psz ) + '"';
      if ( wsz != nullptr )
        return '"' + utf8::encode( wsz ) + '"';
      return std::to_string( u_k );
    }

  auto  to_string( const zmap& map ) -> std::string
  {
    std::string out;

    for ( auto it: map )
    {
      auto& key = it.first;
      auto& val = it.second;

      if ( out.empty() )
        out += "{ {" + to_string( key ) + ", ";
      else
        out += ", {" + to_string( key ) + ", ";

      if ( val.get_type() == zval::z_charstr )  out += "\"" + *val.get_charstr() + "\"}";
        else
      if ( val.get_type() == zval::z_widestr )  out += "\"" + utf8::encode( *val.get_widestr() ) + "\"}";
        else
      out += to_string( val ) + "}";
    }

    return out.empty() ? "{}" : out + " }";
  }

}
