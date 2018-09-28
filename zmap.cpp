# include "zmap.h"

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

  // zmap::key implementation

  zmap::key::key(): _typ( none ), _ptr( nullptr ), _len( 0 )  {}
  zmap::key::key( unsigned t, const uint8_t* b, size_t l ): _typ( t ), _len( l )
    {
      if ( _typ == 0 )  _ptr = (const uint8_t*)memcpy( _buf, b, l );
        else _ptr = b;
    }
  zmap::key::key( unsigned t, const std::string& val ): _typ( t ), _len( val.length() )
    {
      if ( _typ == 0 )  _ptr = (const uint8_t*)memcpy( _buf, val.c_str(), _len );
        else _ptr = (const uint8_t*)val.c_str();
    }
  zmap::key::key( unsigned k ): _typ( uint ), _ptr( _buf ), _len( keys::int_to_key( _buf, k ) )  {}
  zmap::key::key( const char* k ): _typ( cstr ), _ptr( (const uint8_t*)k ), _len( w_strlen( k ) ) {}
  zmap::key::key( const widechar* k ): _typ( wstr ), _ptr( (const uint8_t*)k ), _len( sizeof(widechar) * w_strlen( k ) )  {}
  zmap::key::key( const key& k ): _typ( k._typ ), _ptr( k._ptr ), _len( k._len )
    {  if ( _typ == 0 ) _ptr = (const uint8_t*)memcpy( _buf, k._buf, sizeof(k._buf) );  }
  zmap::key&  zmap::key::operator= ( const key& k )
    {
      _typ = k._typ;
      _len = k._len;
      if ( k._ptr == k._buf ) _ptr = (const uint8_t*)memcpy( _buf, k._buf, k._len );
        else _ptr = k._ptr;
      return *this;
    }

  zmap::key::operator unsigned () const {  return _typ == uint ? keys::key_to_int( _ptr, _len ) : 0;  }
  zmap::key::operator const char* () const {  return _typ == cstr ? (const char*)_ptr : nullptr;  }
  zmap::key::operator const widechar* () const {  return _typ == wstr ? (const widechar*)_ptr : nullptr;  }

  /*
    zmap::z_tree implementation
  */

  auto  zmap::z_tree() const -> const std::unique_ptr<ztree_t>&
    {  return *reinterpret_cast<const std::unique_ptr<ztree_t>*>( z_data );  }

  auto  zmap::z_tree()       ->       std::unique_ptr<ztree_t>&
    {  return *reinterpret_cast<      std::unique_ptr<ztree_t>*>( z_data );  }

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

  zmap::ztree_t::ztree_t( const ztree_t& zt ):
      std::vector<ztree_t>( zt ),
      chnode( zt.chnode ),
      keyset( zt.keyset )
    {
      if ( zt.pvalue != nullptr )
        pvalue = std::move( std::unique_ptr<zval>( new zval( *zt.pvalue.get() ) ) );
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
            ptrtop = static_cast<std::vector<ztree_t>&>( *expand ).insert( ptrtop, std::move( ztree_t( chnext ) ) );
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
        return top == end || top->chnode != chr ? 0 : top->remove( key + 1, cch - 1 );
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
      if ( cch > 0 )
      {
        auto  chr = *key;
        auto  top = _me.begin();
        auto  end = _me.end();

        while ( top != end && top->chnode < chr )
          ++top;
        return top == end || top->chnode != chr ? nullptr : search( *top, key + 1, cch - 1 );
      }
      return &_me;
    }

  auto  zmap::ztree_t::plain_branchlen() const -> int
  {
    const ztree_t*  pbeg;
    int             size = 0;

    for ( size = 0, pbeg = this; pbeg->size() == 1 && pbeg->pvalue == nullptr; pbeg = pbeg->data() )
      ++size;
    return size;
  }

  /*
    zmap::const_place_t implementation
  */
  zmap::const_place_t::const_place_t( const key& k, zmap& m ): refer( k ), owner( m )
    {}

  zmap::const_place_t::const_place_t( const_place_t&& in ): refer( in.refer ), owner( in.owner )
    {}

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

  /*
    zmap::patch_place_t
  */
  # define derive_assign( _type_ )                                                        \
  auto  zmap::patch_place_t::operator= ( const _type_##_t& t ) -> zmap::patch_place_t&  \
  {  return owner.set_##_type_( refer, t ), *this;  }
    derive_assign( char )
    derive_assign( byte )
    derive_assign( int16 )
    derive_assign( int32 )
    derive_assign( int64 )
    derive_assign( word16 )
    derive_assign( word32 )
    derive_assign( word64 )
    derive_assign( float )
    derive_assign( double )
    derive_assign( charstr )
    derive_assign( widestr )
  # undef derive_assign

  auto  zmap::patch_place_t::operator= ( charstr&& s ) -> zmap::patch_place_t&
    {  return owner.set_charstr( refer, std::move( s ) ), *this;  }
  auto  zmap::patch_place_t::operator= ( widestr&& s ) -> zmap::patch_place_t&
    {  return owner.set_widestr( refer, std::move( s ) ), *this;  }

  /*
    zmap implementation
  */

  zmap::zmap(): n_vals( 0 )
    {  new( &z_tree() ) std::unique_ptr<ztree_t>();  }

  zmap::zmap( zmap&& z ): n_vals( z.n_vals )
    {  new( &z_tree() ) std::unique_ptr<ztree_t>( std::move( z.z_tree() ) );  z.n_vals = 0;  }

  zmap::zmap( const zmap& z ): n_vals( z.n_vals )
    {
      if ( n_vals != 0 )
        new( &z_tree() ) std::unique_ptr<ztree_t>( new ztree_t( *z.z_tree().get() ) );
      else
        new( &z_tree() ) std::unique_ptr<ztree_t>( new ztree_t() );
    }

  zmap::zmap( const std::initializer_list<std::pair<key, zval>>& il ): n_vals( 0 )
    {
      new( &z_tree() ) std::unique_ptr<ztree_t>();

      for ( auto& keyval: il )
        put( keyval.first, keyval.second );
    }

  zmap& zmap::operator=( zmap&& z )
    {
      z_tree().operator=( std::move( z.z_tree() ) );
      n_vals = z.n_vals;
      z.n_vals = 0;
      return *this;
    }

  zmap::~zmap()
    {  z_tree().~unique_ptr<ztree_t>();  }

  auto  zmap::copy() const -> zmap
    {
      zmap  make;

      if ( (make.n_vals = n_vals) != 0 )
        make.z_tree() = std::unique_ptr<ztree_t>( new ztree_t( *z_tree() ) );

      return std::move( make );
    }

  auto  zmap::put( const key& k, zval&& v ) -> zval*
    {
      ztree_t*  pfound;

      if ( z_tree() == nullptr )
        z_tree() = std::unique_ptr<ztree_t>( new ztree_t() );

      if ( (pfound = z_tree()->insert( k.data(), k.size() ))->pvalue == nullptr )
        {
          pfound->pvalue = std::unique_ptr<zval>( new zval( std::move( v ) ) );
          ++n_vals;
        }
      pfound->keyset = k.type();

      return pfound->pvalue.get();
    }

  auto  zmap::put( const key& k, const zval& v ) -> zval*
    {
      ztree_t*  pfound;

      if ( z_tree() == nullptr )
        z_tree() = std::unique_ptr<ztree_t>( new ztree_t() );

      if ( (pfound = z_tree()->insert( k.data(), k.size() ))->pvalue == nullptr )
        {
          pfound->pvalue = std::unique_ptr<zval>( new zval( v ) );
          ++n_vals;
        }
      pfound->keyset = k.type();

      return pfound->pvalue.get();
    }

  auto  zmap::get( const key& k ) const -> const zval*
    {
      auto  zt = z_tree() != nullptr ? z_tree()->search( k.data(), k.size() ) : nullptr;

      return zt != nullptr ? zt->pvalue.get() : nullptr;
    }

  auto  zmap::get( const key& k ) -> zval*
    {
      auto& zt = z_tree();
      auto  zv = zt != nullptr ? z_tree()->search( k.data(), k.size() ) : nullptr;

      return zv != nullptr ? zv->pvalue.get() : nullptr;
    }

  auto  zmap::get_type( const key& k ) const -> decltype(zval::vx_type)
    {
      auto  pv = get( k );
      return pv != nullptr ? pv->get_type() : zval::z_untyped;
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
  # define derive_get_init( _type_ )                                                  \
  auto  zmap::get_##_type_( const key& k, const _type_##_t& t ) const -> _type_##_t   \
    {                                                                                 \
      auto  pv = get( k );                                                            \
      return pv != nullptr ? *pv->get_##_type_() : t;                                 \
    }

  # define derive_set_pure( _type_ )                                                  \
  auto  zmap::set_##_type_( const key& k ) -> _type_##_t*                             \
    {  return put( k )->set_##_type_();  }

  # define derive_set_move( _type_ )                                                  \
  auto  zmap::set_##_type_( const key& k, _type_##_t&& t ) -> _type_##_t*             \
    {  return put( k )->set_##_type_( std::move( t ) );  }

  # define derive_set_copy( _type_ )                                                  \
  auto  zmap::set_##_type_( const key& k, const _type_##_t& t ) -> _type_##_t*        \
    {  return put( k )->set_##_type_( t );  }

    derive_get_type( char    )
    derive_get_type( byte    )
    derive_get_type( int16   )
    derive_get_type( int32   )
    derive_get_type( int64   )
    derive_get_type( word16  )
    derive_get_type( word32  )
    derive_get_type( word64  )
    derive_get_type( float   )
    derive_get_type( double  )
    derive_get_type( charstr )
    derive_get_type( widestr )

    derive_get_init( char    )
    derive_get_init( byte    )
    derive_get_init( int16   )
    derive_get_init( int32   )
    derive_get_init( int64   )
    derive_get_init( word16  )
    derive_get_init( word32  )
    derive_get_init( word64  )
    derive_get_init( float   )
    derive_get_init( double  )
    derive_get_init( charstr )
    derive_get_init( widestr )

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
    derive_get_type( array_zmap    )
    derive_get_type( array_zval    )

    derive_set_copy( char    )
    derive_set_copy( byte    )
    derive_set_copy( int16   )
    derive_set_copy( int32   )
    derive_set_copy( int64   )
    derive_set_copy( word16  )
    derive_set_copy( word32  )
    derive_set_copy( word64  )
    derive_set_copy( float   )
    derive_set_copy( double  )
    derive_set_copy( charstr )
    derive_set_copy( widestr )
    derive_set_copy( array_char )
    derive_set_copy( array_byte    )
    derive_set_copy( array_int16   )
    derive_set_copy( array_int32   )
    derive_set_copy( array_int64   )
    derive_set_copy( array_word16  )
    derive_set_copy( array_word32  )
    derive_set_copy( array_word64  )
    derive_set_copy( array_float   )
    derive_set_copy( array_double  )
    derive_set_copy( array_charstr )
    derive_set_copy( array_widestr )
    derive_set_copy( array_zval    )

    derive_set_pure( zmap    )
    derive_set_pure( array_zmap    )

    derive_set_move( charstr )
    derive_set_move( widestr )
    derive_set_move( zmap    )
    derive_set_move( array_char )
    derive_set_move( array_byte    )
    derive_set_move( array_int16   )
    derive_set_move( array_int32   )
    derive_set_move( array_int64   )
    derive_set_move( array_word16  )
    derive_set_move( array_word32  )
    derive_set_move( array_word64  )
    derive_set_move( array_float   )
    derive_set_move( array_double  )
    derive_set_move( array_charstr )
    derive_set_move( array_widestr )
    derive_set_move( array_zmap    )
    derive_set_move( array_zval    )
  # undef derive_set_pure
  # undef derive_set_move
  # undef derive_set_copy
  # undef derive_get_type

  auto  zmap::empty() const -> bool {  return n_vals == 0;  }
  auto  zmap::size() const -> size_t {  return n_vals;  }

  zmap::iterator  zmap::begin()
    {
      if ( z_tree().get() == nullptr )
        return iterator();
      return iterator( z_tree().get()->begin(), z_tree().get()->end() );
    }
  zmap::iterator  zmap::end()  {  return iterator();  }

  zmap::const_iterator  zmap::cbegin() const
    {
      if ( z_tree().get() == nullptr )
        return const_iterator();
      return const_iterator( z_tree().get()->begin(), z_tree().get()->end() );
    }
  zmap::const_iterator  zmap::cend() const  {  return const_iterator();  }

  zmap::const_iterator  zmap::begin() const  {  return cbegin();  }
  zmap::const_iterator  zmap::end() const  {  return cend();  }

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

  auto  zmap::operator []( const key& k ) -> patch_place_t
    {  return std::move( patch_place_t( k, *this ) );  }
  auto  zmap::operator []( const key& k ) const -> const const_place_t
    {  return std::move( const_place_t( k, *(zmap*)this ) );  }

  auto  zmap::clear() -> void
    {
      z_tree().reset();
      n_vals = 0;
    }

  auto  zmap::erase( const key& k ) -> size_t
    {
      size_t  n_dels;

      if ( z_tree() == nullptr )
        return 0;
      if ( (n_vals -= (n_dels = z_tree()->remove( k.data(), k.size() ))) == 0 )
        z_tree().reset();
      return n_dels;
    }

  std::string to_string( const zmap::key& key )
    {
      unsigned  u_k = key;
      const char* psz = key;
      const widechar* wsz = key;

      if ( psz != nullptr )
        return std::string( psz );
      if ( wsz != nullptr )
        return std::string( "widestring" );
      return std::to_string( u_k );
    }

}
