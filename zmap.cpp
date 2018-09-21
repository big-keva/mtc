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
  zmap::key::key( unsigned k ): _typ( uint ), _ptr( _buf ), _len( keys::int_to_key( _buf, k ) )  {}
  zmap::key::key( const char* k ): _typ( cstr ), _ptr( (const uint8_t*)k ), _len( w_strlen( k ) ) {}
  zmap::key::key( const widechar* k ): _typ( wstr ), _ptr( (const uint8_t*)k ), _len( sizeof(widechar) * w_strlen( k ) )  {}
  zmap::key::key( const key& k ): _typ( k._typ ), _ptr( k._ptr ), _len( k._len )
    {  if ( _typ == 0 ) _ptr = (const uint8_t*)memcpy( _buf, k._buf, sizeof(k._buf) );  }

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
    std::vector<ztree_t>(), chnode( ch ), keyset( 0 ) {}

  zmap::ztree_t::ztree_t( ztree_t&& zt ):
    std::vector<ztree_t>( std::move( zt ) ), chnode( zt.chnode ), keyset( zt.keyset ), pvalue( std::move( zt.pvalue ) ) {  zt.keyset = 0;  }

  zmap::ztree_t&  zmap::ztree_t::operator = ( ztree_t&& zt )
    {
      std::vector<ztree_t>::operator=( std::move( zt ) );
      chnode = zt.chnode;
      keyset = zt.keyset;  zt.keyset = 0;
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

  template <class self>
  static  self* zmap::ztree_t::search( self& _me, const uint8_t* key, size_t cch )
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

  inline  auto  zmap::ztree_t::insert( const byte_t* ptrkey, size_t cchkey ) -> zmap::ztree_t*
    {
      for ( auto expand = this; ; ++ptrkey, --cchkey )
      {
        if ( cchkey > 0 )
        {
          byte_t  chnext = *ptrkey;
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

  inline  auto  zmap::ztree_t::lookup( byte_t* keybuf, int keylen, int buflen ) const -> int
  {
    auto  ptrtop = begin();
    auto  ptrend = end();
    int   outlen;

    if ( keylen > 0 )
    {
      while ( ptrtop < ptrend && ptrtop->chnode < *keybuf )
        ++ptrtop;
      if ( ptrtop >= ptrend )
        return -1;
      if ( ptrtop->chnode == keybuf[0] )
      {
        if ( (outlen = ptrtop->lookup( keybuf + 1, keylen - 1, buflen - 1 )) != -1 )
          return 1 + outlen;
        if ( ++ptrtop >= ptrend )
          return -1;
      }
      return 1 + ptrtop->lookup( (*keybuf++ = ptrtop->chnode, keybuf), 0, buflen - 1 );
    }
    return pvalue != nullptr ? 0 :
           ptrtop != ptrend  ? 1 + ptrtop->lookup( (*keybuf++ = ptrtop->chnode, keybuf), 0, buflen - 1 ) : -1;
  }

  size_t  zmap::ztree_t::GetBufLen() const
  {
    int       branch = plain_branchlen();
    word16_t  lstore = static_cast<word16_t>( (branch > 0 ? 0x0400 + branch : size()) + (pvalue != nullptr ? 0x0200 : 0) );
    size_t    buflen = ::GetBufLen( lstore );

    if ( pvalue != nullptr )
      buflen += 1 + pvalue->GetBufLen();

    if ( branch > 0 )
    {
      auto  pbeg = this;

      while ( pbeg->size() == 1 && pbeg->pvalue == nullptr )
        {  pbeg = pbeg->data();  ++buflen;  }
      return buflen + pbeg->GetBufLen();
    }
      else
    for ( auto p = begin(); p != end(); ++p, ++buflen )
    {
      size_t  sublen = p->GetBufLen();
      buflen += ::GetBufLen( sublen ) + sublen;
    }

    return buflen;
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
    zmap::const_iterator implementation
  */

  zmap::const_iterator::const_iterator_value::const_iterator_value( const key& k, const zval* v ): first( k ), second( *v )
    {
    }

  zmap::const_iterator::const_iterator()
    {
    }

  zmap::const_iterator::const_iterator( const const_iterator& it ):
      zstack( it.zstack ),
      z_buff( it.z_buff )
    {
    }

  zmap::const_iterator::const_iterator( ztree_t::const_iterator beg, ztree_t::const_iterator end )
    {
      if ( beg != end )
      {
        zstack.push_back( std::make_pair( beg, end ) );
        z_buff.push_back( beg->chnode );
        find();
      }
    }

  auto  zmap::const_iterator::operator -> () const -> const_iterator_value
    {
      if ( zstack.size() != 0 || last().first->pvalue != nullptr )
        return const_iterator_value( key( last().first->keyset, (const uint8_t*)z_buff.c_str(), z_buff.length() ), last().first->pvalue.get() );
      return const_iterator_value( key(), nullptr );
    }

  auto  zmap::const_iterator::operator * () const -> const_iterator_value
    {
      if ( zstack.size() != 0 || last().first->pvalue != nullptr )
        return const_iterator_value( key( last().first->keyset, (const uint8_t*)z_buff.c_str(), z_buff.length() ), last().first->pvalue.get() );
      return const_iterator_value( key(), nullptr );
    }

  auto  zmap::const_iterator::operator++() -> const const_iterator& {  return next();  }
  auto  zmap::const_iterator::operator--() -> const const_iterator& {  return prev();  }

  auto  zmap::const_iterator::operator++( int ) -> const_iterator
    {
      const_iterator  _lst( *this );
      return (next(), std::move( _lst ));
    }

  auto  zmap::const_iterator::operator--( int ) -> const_iterator
    {
      const_iterator  _lst( *this );
      return (prev(), std::move( _lst ));
    }

  auto  zmap::const_iterator::operator==( const const_iterator& it ) const -> bool
    {
      return zstack == it.zstack;
    }

  auto  zmap::const_iterator::find() -> const_iterator&
    {
      assert( zstack.size() != 0 );

      while ( last().first->pvalue == nullptr && last().first->size() != 0 )
        down( last().first );

      assert( last().first->pvalue != nullptr );

      return *this;
    }

  /*
    zmap::const_iterator::next()

    Смещает итератор на следующую заполненную пару ключ-значение по ztree_t - дереву.
  */
  auto  zmap::const_iterator::next() -> const_iterator&
    {
      while ( zstack.size() != 0 )
      {
      // считается, что текущий элемент, если он есть, всё равно уже просмотрен;
      //
      // возможные состояния трассы:
      //  - есть вложенные элементы в текущем итераторе;
      //  - вложенных элементов нет, но есть следующие элементы того же уровня;
      //  - на этом уровне больше нет элементов.
      //
      // если есть вложенные элементы, опуститься по дереву максимально глубоко, до первого элемента,
      // у которого есть значение pvalue; если такое значение существует, закончить поиск, иначе
      // продолжить анализ вариантов в новом цикле.
        if ( last().first->size() != 0 )
        {
          do  down( last().first++ );
            while ( last().first->pvalue == nullptr && last().first->size() != 0 );

          if ( last().first->pvalue != nullptr )  return *this;
            else continue;
        }

      // вложенных в текущий элементов нет;
        assert( last().first->size() == 0 );

      // проверить, не последний ли это был элемент на данном уровне и, если он был последним, откатиться
      // вверх по дереву до первого не-последнего элемента
        if ( ++last().first == last().second )
        {
          do  back();
            while ( zstack.size() != 0 && last().first == last().second );

          if ( zstack.size() != 0 )
            z_buff.back() = last().first->chnode;

          continue;
        }

      // элемент был не последним; заместить последний символ поискового ключа на текущий и повторить
      // алгоритм с возможным заходом по дереву
        z_buff.back() = last().first->chnode;

        if ( last().first->pvalue != nullptr )
          return *this;
      }
      return *this;
    }

  auto  zmap::const_iterator::prev() -> const_iterator&
    {
      return *this;
    }

  void  zmap::const_iterator::down( ztree_t::const_iterator it )
    {
      auto  beg = it->begin();
      auto  end = it->end();

      assert( beg != end );

      zstack.push_back( std::make_pair( beg, end ) );
      z_buff.push_back( beg->chnode );
    }

  void  zmap::const_iterator::back()
    {
      zstack.pop_back();
      z_buff.pop_back();
    }

  auto  zmap::const_iterator::last() -> zpos& {  return zstack.back();  }
  auto  zmap::const_iterator::last() const -> const zpos& {  return zstack.back();  }

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
      auto  zt = z_tree() != nullptr ? z_tree()->search( k.data(), k.size() ) : nullptr;

      return zt != nullptr ? zt->pvalue.get() : nullptr;
    }

  auto  zmap::get_type( const key& k ) const -> decltype(zval::vx_type)
    {
      auto  pv = get( k );
      return pv != nullptr ? pv->get_type() : z_untyped;
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

  size_t  zmap::GetBufLen() const
  {
    return z_tree() != nullptr ? z_tree()->GetBufLen() : 1;
  }

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

  size_t  zmap::size() const  {  return n_vals;  }

  auto  zmap::operator []( const key& k ) -> patch_place_t
    {  return std::move( patch_place_t( k, *this ) );  }
  auto  zmap::operator []( const key& k ) const -> const const_place_t
    {  return std::move( const_place_t( k, *(zmap*)this ) );  }

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
