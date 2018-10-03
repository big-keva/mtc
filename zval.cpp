# include "zmap.h"
# include "serialize.h"

namespace mtc
{
  // zval implementation

  auto  zval::inner() const -> const zval::inner_t& {  return *reinterpret_cast<const inner_t*>( storage );  }
  auto  zval::inner()       ->       zval::inner_t& {  return *reinterpret_cast<      inner_t*>( storage );  }

  zval::zval(): vx_type( z_untyped )
    {}

  zval::zval( zval&& zv )
    {  fetch( std::move( zv ) );  }

  zval::zval( const zval& zv )
    {  fetch( zv );  }

  zval& zval::operator = ( zval&& zv )
    {  return fetch( std::move( zv ) );  }

  zval& zval::operator = ( const zval& zv )
    {  return fetch( zv );  }

  zval::~zval()
    {  clear();  }

  # define derive_init( _type_ )                    \
  zval::zval( _type_##_t t ): vx_type( z_untyped )  \
    {  set_##_type_( t );  }                        \
  zval& zval::operator = ( _type_##_t t )           \
    {   set_##_type_( t );  return *this;  }
    derive_init( char );
    derive_init( byte );
    derive_init( int16 );
    derive_init( int32 );
    derive_init( int64 );
    derive_init( word16 );
    derive_init( word32 );
    derive_init( word64 );
    derive_init( float );
    derive_init( double );
  # undef derive_init

  # define derive_init( _type_ )                            \
  zval::zval( const _type_##_t& t ): vx_type( z_untyped )   \
    {  set_##_type_( t );  }                                \
  zval::zval( _type_##_t&& t ): vx_type( z_untyped )        \
    {  set_##_type_( std::move( t ) );  }                   \
  zval& zval::operator = ( const _type_##_t& t )            \
    {  set_##_type_( t );  return *this;  }                 \
  zval& zval::operator = ( _type_##_t&& t )                 \
    {  set_##_type_( std::move( t ) );  return *this;  }
    derive_init( charstr )
    derive_init( widestr )

  zval::zval( zmap_t&& t ): vx_type( z_untyped )
    {  set_zmap( std::move( t ) );  }
  zval& zval::operator = ( zmap_t&& t )
    {  set_zmap( std::move( t ) );  return *this;  }

    derive_init( array_char )
    derive_init( array_byte )
    derive_init( array_int16 );
    derive_init( array_int32 );
    derive_init( array_int64 );
    derive_init( array_word16 );
    derive_init( array_word32 );
    derive_init( array_word64 );
    derive_init( array_float );
    derive_init( array_double );
    derive_init( array_charstr );
    derive_init( array_widestr );
    derive_init( array_zval );
  # undef derive_init

  zval::zval( array_zmap_t&& t ): vx_type( z_untyped )
    {  set_array_zmap( std::move( t ) );  }
  zval& zval::operator = ( array_zmap_t&& t )
    {  set_array_zmap( std::move( t ) );  return *this;  }

  # define  derive_access_type( _type_ )                              \
    const _type_##_t*  zval::get_##_type_() const                     \
      {                                                               \
        return vx_type == z_##_type_ ? &inner().v_##_type_ : nullptr; \
      }                                                               \
    _type_##_t*  zval::get_##_type_()                                 \
      {                                                               \
        return vx_type == z_##_type_ ? &inner().v_##_type_ : nullptr; \
      }                                                               \
    _type_##_t*  zval::set_##_type_( _type_##_t t )                   \
      {                                                               \
        clear().vx_type = z_##_type_;                                 \
        return new( &inner().v_##_type_ ) _type_##_t( t );            \
      }
    derive_access_type( char )
    derive_access_type( byte )
    derive_access_type( int16 )
    derive_access_type( int32 )
    derive_access_type( int64 )
    derive_access_type( word16 )
    derive_access_type( word32 )
    derive_access_type( word64 )
    derive_access_type( float )
    derive_access_type( double )
  # undef derive_access_type

  # define derive_access_type( _type_ )                                 \
    const _type_##_t*  zval::get_##_type_() const                       \
      {                                                                 \
        return vx_type == z_##_type_ ? &inner().v_##_type_ : nullptr;   \
      }                                                                 \
    _type_##_t*  zval::get_##_type_()                                   \
      {                                                                 \
        return vx_type == z_##_type_ ? &inner().v_##_type_ : nullptr;   \
      }                                                                 \
    _type_##_t*  zval::set_##_type_( _type_##_t&& t )                   \
      {                                                                 \
        clear().vx_type = z_##_type_;                                   \
        return new( &inner().v_##_type_ ) _type_##_t( std::move( t ) ); \
      }                                                                 \
    _type_##_t*  zval::set_##_type_( const _type_##_t& t )              \
      {                                                                 \
        clear().vx_type = z_##_type_;                                   \
        return new( &inner().v_##_type_ ) _type_##_t( t );              \
      }

    derive_access_type( charstr )
    derive_access_type( widestr )
  # undef derive_access_type

  auto  zval::get_zmap() -> zmap_t*             {  return vx_type == z_zmap ? &inner().v_zmap : nullptr;  }
  auto  zval::get_zmap() const -> const zmap_t* {  return vx_type == z_zmap ? &inner().v_zmap : nullptr;  }
  auto  zval::set_zmap()  -> zmap_t*
    {
      clear().vx_type = z_zmap;
      return new( &inner().v_zmap ) zmap_t();
    }
  auto  zval::set_zmap( zmap_t&& t )  -> zmap_t*
    {
      clear().vx_type = z_zmap;
      return new( &inner().v_zmap ) zmap_t( std::move( t ) );
    }

  # define derive_access_array( _type_ )                                        \
  array_##_type_* zval::set_array_##_type_( array_##_type_&& t )                \
    {                                                                           \
      clear().vx_type = z_array_##_type_;                                       \
      return new( &inner().v_array_##_type_ ) array_##_type_( std::move( t ) ); \
    }                                                                           \
  array_##_type_* zval::set_array_##_type_( const array_##_type_& t )           \
    {                                                                           \
      clear().vx_type = z_array_##_type_;                                       \
      return new( &inner().v_array_##_type_ ) array_##_type_( t );              \
    }                                                                           \
  array_##_type_* zval::get_array_##_type_()                                    \
    {                                                                           \
      return vx_type == z_array_##_type_ ? &inner().v_array_##_type_ : nullptr; \
    }                                                                           \
  const array_##_type_* zval::get_array_##_type_() const                        \
    {                                                                           \
      return vx_type == z_array_##_type_ ? &inner().v_array_##_type_ : nullptr; \
    }
    derive_access_array( char )
    derive_access_array( byte )
    derive_access_array( int16 )
    derive_access_array( int32 )
    derive_access_array( int64 )
    derive_access_array( word16 )
    derive_access_array( word32 )
    derive_access_array( word64 )
    derive_access_array( float )
    derive_access_array( double )
    derive_access_array( charstr )
    derive_access_array( widestr )
    derive_access_array( zval )
  # undef derive_access_array

  const array_zmap* zval::get_array_zmap() const
    {
      return vx_type == z_array_zmap ? &inner().v_array_zmap : nullptr;
    }
  array_zmap* zval::get_array_zmap()
    {
      return vx_type == z_array_zmap ? &inner().v_array_zmap : nullptr;
    }
  array_zmap* zval::set_array_zmap()
    {
      clear().vx_type = z_array_zmap;
      return new( &inner().v_array_zmap ) array_zmap();
    }
  array_zmap* zval::set_array_zmap( array_zmap_t&& t )
    {
      clear().vx_type = z_array_zmap;
      return new( &inner().v_array_zmap ) array_zmap( std::move( t ) );
    }

  auto  zval::clear() -> zval&
    {
      switch ( vx_type )
      {
      # define  destruct( _type_ )  case z_##_type_:  inner().v_##_type_.~_type_##_t();  break;
        destruct( char )
        destruct( byte )
        destruct( int16 )
        destruct( int32 )
        destruct( int64 )
        destruct( word16 )
        destruct( word32 )
        destruct( word64 )
        destruct( float )
        destruct( double )

        destruct( charstr )
        destruct( widestr )
        destruct( zmap )

        destruct( array_char )
        destruct( array_byte )
        destruct( array_int16 )
        destruct( array_int32 )
        destruct( array_int64 )
        destruct( array_word16 )
        destruct( array_word32 )
        destruct( array_word64 )
        destruct( array_float )
        destruct( array_double )
        destruct( array_charstr )
        destruct( array_widestr )
        destruct( array_zval )
        destruct( array_zmap )
      # undef destruct
      }
      vx_type = z_untyped;
      return *this;
    }

  auto  zval::get_type() const -> unsigned
    {  return vx_type;  }

  auto  zval::is_numeric() const -> bool
    {  return get_type() >= z_char && get_type() <= z_double;  }

  size_t  zval::GetBufLen() const
  {
    switch ( vx_type )
    {
  # define derive_size_plain( _type_ )  case z_##_type_: return 1 + sizeof(_type_##_t);
  # define derive_size_smart( _type_ )  case z_##_type_: return 1 + ::GetBufLen( inner().v_##_type_ );
      derive_size_plain( char )
      derive_size_plain( byte )
      derive_size_plain( int16 )
      derive_size_plain( word16 )

      derive_size_smart( int32 )
      derive_size_smart( word32 )
      derive_size_smart( int64 )
      derive_size_smart( word64 )
      derive_size_smart( float )
      derive_size_smart( double )
      derive_size_smart( charstr )
      derive_size_smart( widestr )
      derive_size_smart( zmap )
      derive_size_smart( array_char )
      derive_size_smart( array_byte )
      derive_size_smart( array_int16 )
      derive_size_smart( array_word16 )
      derive_size_smart( array_int32 )
      derive_size_smart( array_word32 )
      derive_size_smart( array_int64 )
      derive_size_smart( array_word64 )
      derive_size_smart( array_float )
      derive_size_smart( array_double )
      derive_size_smart( array_charstr )
      derive_size_smart( array_widestr )
      derive_size_smart( array_zval )
      derive_size_smart( array_zmap )
  # undef derive_size_smart
  # undef derive_size_plain
      default:  return 0;
    }
  }

  std::string zval::to_string() const
    {
      switch ( get_type() )
      {
        case z_char:          return std::move( to_string( *get_char() ) );
        case z_byte:          return std::move( to_string( *get_byte() ) );
        case z_int16:         return std::move( to_string( *get_int16() ) );
        case z_int32:         return std::move( to_string( *get_int32() ) );
        case z_int64:         return std::move( to_string( *get_int64() ) );
        case z_word16:        return std::move( to_string( *get_word16() ) );
        case z_word32:        return std::move( to_string( *get_word32() ) );
        case z_word64:        return std::move( to_string( *get_word64() ) );
        case z_float:         return std::move( to_string( *get_float() ) );
        case z_double:        return std::move( to_string( *get_double() ) );

        case z_charstr:       return *get_charstr();
        case z_widestr:       return std::move( to_string( *get_widestr() ) );

        case z_zmap:          return std::move( to_string( *get_zmap() ) );

        case z_array_char:    return std::move( to_string( *get_array_char() ) );
        case z_array_byte:    return std::move( to_string( *get_array_byte() ) );
        case z_array_int16:   return std::move( to_string( *get_array_int16() ) );
        case z_array_int32:   return std::move( to_string( *get_array_int32() ) );
        case z_array_int64:   return std::move( to_string( *get_array_int64() ) );
        case z_array_word16:  return std::move( to_string( *get_array_word16() ) );
        case z_array_word32:  return std::move( to_string( *get_array_word32() ) );
        case z_array_word64:  return std::move( to_string( *get_array_word64() ) );
        case z_array_float:   return std::move( to_string( *get_array_float() ) );
        case z_array_double:  return std::move( to_string( *get_array_double() ) );

        case z_array_charstr: return std::move( to_string( *get_array_charstr() ) );
        case z_array_widestr: return std::move( to_string( *get_array_widestr() ) );
        case z_array_zmap:    return std::move( to_string( *get_array_zmap() ) );
        case z_array_zval:    return std::move( to_string( *get_array_zval() ) );
        default:
          throw std::invalid_argument( "undefined xvalue<> type" );
      }
    }

  auto  zval::fetch( zval&& zv ) -> zval&
    {
      switch ( clear().vx_type = zv.vx_type )
      {
      # define  move( _type_ )  case z_##_type_:  \
        new( &inner().v_##_type_ ) _type_##_t( std::move( zv.inner().v_##_type_ ) );  break;
        move( char )
        move( byte )
        move( int16 )
        move( int32 )
        move( int64 )
        move( word16 )
        move( word32 )
        move( word64 )
        move( float )
        move( double )

        move( charstr )
        move( widestr )
        move( zmap )

        move( array_char )
        move( array_byte )
        move( array_int16 )
        move( array_int32 )
        move( array_int64 )
        move( array_word16 )
        move( array_word32 )
        move( array_word64 )
        move( array_float )
        move( array_double )
        move( array_charstr )
        move( array_widestr )
        move( array_zval )
        move( array_zmap )
      # undef move
      }
      return *this;
    }

  auto  zval::fetch( const zval& zv ) -> zval&
    {
      switch ( clear().vx_type = zv.vx_type )
      {
      # define  copy( _type_ )  case z_##_type_:    \
      new( &inner().v_##_type_ ) _type_##_t( zv.inner().v_##_type_ );  break;
        copy( char )
        copy( byte )
        copy( int16 )
        copy( int32 )
        copy( int64 )
        copy( word16 )
        copy( word32 )
        copy( word64 )
        copy( float )
        copy( double )

        copy( charstr )
        copy( widestr )
        copy( zmap )

        copy( array_char )
        copy( array_byte )
        copy( array_int16 )
        copy( array_int32 )
        copy( array_int64 )
        copy( array_word16 )
        copy( array_word32 )
        copy( array_word64 )
        copy( array_float )
        copy( array_double )
        copy( array_charstr )
        copy( array_widestr )
        copy( array_zval )
        copy( array_zmap )
      # undef copy
      }
      return *this;
    }

}
