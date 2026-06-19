# include "../zmap.h"
# include "../utf.hpp"

namespace mtc
{
  // zval implementation

  auto  zval::inner() const -> const zval::inner_t& {  return *reinterpret_cast<const inner_t*>( &storage );  }
  auto  zval::inner()       ->       zval::inner_t& {  return *reinterpret_cast<      inner_t*>( &storage );  }

  zval::zval(): vx_type( z_untyped )  {}

  zval::zval( zval&& zv ): vx_type( z_untyped ) {  fetch( std::move( zv ) );  }

  zval::zval( const zval& zv ): vx_type( z_untyped )  {  fetch( zv );  }

  zval::zval( const zval& zv, const force_copy& force ): vx_type( z_untyped ) {  fetch( zv, force );  }

  zval& zval::operator = ( zval&& zv )  {  return &zv != this ? fetch( std::move( zv ) ) : *this;  }

  zval& zval::operator = ( const zval& zv ) {  return &zv != this ? fetch( std::move( zv ) ) : *this;  }

  zval::~zval() {  clear();  }

  # define derive_init( _type_ )                    \
  zval::zval( _type_##_t t ): vx_type( z_untyped )  \
    {  set_##_type_( t );  }                        \
  zval& zval::operator = ( _type_##_t t )           \
    {   set_##_type_( t );  return *this;  }
    derive_init( char )
    derive_init( byte )
    derive_init( int16 )
    derive_init( int32 )
    derive_init( int64 )
    derive_init( word16 )
    derive_init( word32 )
    derive_init( word64 )
    derive_init( float )
    derive_init( double )
    derive_init( bool )
  # undef derive_init

  zval::zval( const char* psz, size_t len ): vx_type( z_untyped )
    {  set_charstr( charstr( psz, len != (size_t)-1 ? len : w_strlen( psz ) ) );  }
  zval& zval::operator = ( const char* psz )
    {  return set_charstr( charstr( psz ) ), *this;  }

  zval::zval( const widechar* psz, size_t len ): vx_type( z_untyped )
    {  set_widestr( widestr( psz, len != (size_t)-1 ? len : w_strlen( psz ) ) );  }
  zval& zval::operator = ( const widechar* psz )
    {  return set_widestr( widestr( psz ) ), *this;  }

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
    derive_init( uuid )
    derive_init( zmap )

    derive_init( array_char )
    derive_init( array_byte )
    derive_init( array_int16 )
    derive_init( array_int32 )
    derive_init( array_int64 )
    derive_init( array_word16 )
    derive_init( array_word32 )
    derive_init( array_word64 )
    derive_init( array_float )
    derive_init( array_double )
    derive_init( array_charstr )
    derive_init( array_widestr )
    derive_init( array_uuid )
    derive_init( array_zval )
    derive_init( array_zmap )
  # undef derive_init

# if __cplusplus >= 201703L

  zval::zval( const std::string_view& s )
    {  set_charstr( s.data(), s.size() );  }

  zval& zval::operator = ( const std::string_view& s )
    {  return set_charstr( s.data(), s.size() ), *this;  }

  zval::zval( const std::basic_string_view<widechar>& s )
    {  set_widestr( s.data(), s.size() );  }

  zval& zval::operator = ( const std::basic_string_view<widechar>& s )
    {  return set_widestr( s.data(), s.size() ), *this;  }

# endif

  # define  derive_access_val( _type_ )                               \
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
  # define  derive_access_ref( _type_ )                               \
    const _type_##_t*  zval::get_##_type_() const                     \
      {                                                               \
        return vx_type == z_##_type_ ? &inner().v_##_type_ : nullptr; \
      }                                                               \
    _type_##_t*  zval::get_##_type_()                                 \
      {                                                               \
        return vx_type == z_##_type_ ? &inner().v_##_type_ : nullptr; \
      }                                                               \
    _type_##_t*  zval::set_##_type_( _type_##_t&& t )                 \
      {                                                               \
        clear().vx_type = z_##_type_;                                 \
        return new( &inner().v_##_type_ )                             \
          _type_##_t( std::move( t ) );                               \
      }                                                               \
    _type_##_t*  zval::set_##_type_( const _type_##_t& t )            \
      {                                                               \
        clear().vx_type = z_##_type_;                                 \
        return new( &inner().v_##_type_ ) _type_##_t( t );            \
      }
    derive_access_val( char )
    derive_access_val( byte )
    derive_access_val( int16 )
    derive_access_val( int32 )
    derive_access_val( int64 )
    derive_access_val( word16 )
    derive_access_val( word32 )
    derive_access_val( word64 )
    derive_access_val( float )
    derive_access_val( double )
    derive_access_val( bool )

    derive_access_ref( charstr )
    derive_access_ref( widestr )
    derive_access_ref( uuid )

  auto  zval::set_charstr( const char* psz, size_t len ) -> charstr*
  {
    clear().vx_type = z_charstr;
    return new( &inner().v_charstr ) charstr( psz, len != (size_t)-1 ? len : w_strlen( psz ) );
  }

  auto  zval::set_widestr( const widechar* pws, size_t len ) -> widestr*
  {
    clear().vx_type = z_widestr;
    return new( &inner().v_widestr ) widestr( pws, len != (size_t)-1 ? len : w_strlen( pws ) );
  }

  auto  zval::get_zmap() -> zmap_t*
  {
    return vx_type == z_zmap ? &inner().v_zmap : nullptr;
  }

  auto  zval::get_zmap() const -> const zmap_t*
  {
    return vx_type == z_zmap ? &inner().v_zmap : nullptr;
  }

  auto  zval::set_zmap() -> zmap_t*
  {
    clear().vx_type = z_zmap;
    return new( &inner().v_zmap ) zmap_t();
  }

  auto  zval::set_zmap( zmap&& z ) -> zmap_t*
  {
    clear().vx_type = z_zmap;
    return new( &inner().v_zmap ) zmap_t( std::move( z ) );
  }

  auto  zval::set_zmap( const zmap& z ) -> zmap_t*
  {
    clear().vx_type = z_zmap;
    return new( &inner().v_zmap ) zmap_t( z );
  }

    derive_access_ref( array_char )
    derive_access_ref( array_byte )
    derive_access_ref( array_int16 )
    derive_access_ref( array_int32 )
    derive_access_ref( array_int64 )
    derive_access_ref( array_word16 )
    derive_access_ref( array_word32 )
    derive_access_ref( array_word64 )
    derive_access_ref( array_float )
    derive_access_ref( array_double )

    derive_access_ref( array_charstr )
    derive_access_ref( array_widestr )
    derive_access_ref( array_uuid )
    derive_access_ref( array_zval )
    derive_access_ref( array_zmap )

  # undef derive_access_ref
  # undef derive_access_val

  auto  zval::copy() const -> zval
    {  return zval( *this, force_copy() );  }

  bool  zval::empty() const
    {  return vx_type == z_untyped;  }

  void  zval::dispose()
    {
      switch ( vx_type )
      {
      # define  destruct( _type_ )  case z_##_type_:  inner().v_##_type_.~_type_##_t();  break;
        destruct( charstr )
        destruct( widestr )
        destruct( zmap )
        destruct( uuid )

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
        destruct( array_uuid )
      # undef destruct
      }
    }

  auto  zval::get_type() const -> unsigned
    {  return vx_type;  }

  auto  zval::is_array() const -> bool
    {  return vx_type >= z_array_char && vx_type <= z_array_uuid;  }

  auto  zval::is_numeric() const -> bool
    {  return vx_type >= z_char && vx_type <= z_double;  }

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
      derive_size_plain( bool )

      derive_size_smart( int32 )
      derive_size_smart( word32 )
      derive_size_smart( int64 )
      derive_size_smart( word64 )
      derive_size_smart( float )
      derive_size_smart( double )
      derive_size_smart( charstr )
      derive_size_smart( widestr )
      derive_size_smart( zmap )
      derive_size_smart( uuid )
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
      derive_size_smart( array_uuid )
  # undef derive_size_smart
  # undef derive_size_plain
      case z_untyped: return ::GetBufLen( vx_type );
      default:  return 0;
    }
  }

  auto  zval::type_name( z_type type ) -> const char*
    {
      switch ( type )
      {
        case z_char:          return "char";
        case z_byte:          return "byte";
        case z_int16:         return "int16";
        case z_int32:         return "int32";
        case z_int64:         return "int64";
        case z_word16:        return "word16";
        case z_word32:        return "word32";
        case z_word64:        return "word64";
        case z_float:         return "float";
        case z_double:        return "double";
        case z_bool:          return "bool";

        case z_charstr:       return "charstr";
        case z_widestr:       return "widestr";

        case z_zmap:          return "zmap";
        case z_uuid:          return "uuid";

        case z_array_char:    return "array_char";
        case z_array_byte:    return "array_byte";
        case z_array_int16:   return "array_int16";
        case z_array_int32:   return "array_int32";
        case z_array_int64:   return "array_int64";
        case z_array_word16:  return "array_word16";
        case z_array_word32:  return "array_word32";
        case z_array_word64:  return "array_word64";
        case z_array_float:   return "array_float";
        case z_array_double:  return "array_double";

        case z_array_charstr: return "array_charstr";
        case z_array_widestr: return "array_widestr";
        case z_array_zmap:    return "array_zmap";
        case z_array_zval:    return "array_zval";
        case z_array_uuid:    return "array_uuid";

        case z_untyped:       return "untyped";
        default:              throw std::invalid_argument( "undefined zval type" );
      }
    }

  std::string zval::to_string() const
    {
      switch ( get_type() )
      {
        case z_char:          return to_string( *get_char() );
        case z_byte:          return to_string( *get_byte() );
        case z_int16:         return to_string( *get_int16() );
        case z_int32:         return to_string( *get_int32() );
        case z_int64:         return to_string( *get_int64() );
        case z_word16:        return to_string( *get_word16() );
        case z_word32:        return to_string( *get_word32() );
        case z_word64:        return to_string( *get_word64() );
        case z_float:         return to_string( *get_float() );
        case z_double:        return to_string( *get_double() );
        case z_bool:          return to_string( *get_bool() );

        case z_charstr:       return to_string( *get_charstr() );
        case z_widestr:       return to_string( *get_widestr() );

        case z_zmap:          return to_string( *get_zmap() );
        case z_uuid:          return mtc::to_string( *get_uuid() );

        case z_array_char:    return to_string( *get_array_char() );
        case z_array_byte:    return to_string( *get_array_byte() );
        case z_array_int16:   return to_string( *get_array_int16() );
        case z_array_int32:   return to_string( *get_array_int32() );
        case z_array_int64:   return to_string( *get_array_int64() );
        case z_array_word16:  return to_string( *get_array_word16() );
        case z_array_word32:  return to_string( *get_array_word32() );
        case z_array_word64:  return to_string( *get_array_word64() );
        case z_array_float:   return to_string( *get_array_float() );
        case z_array_double:  return to_string( *get_array_double() );

        case z_array_charstr: return to_string( *get_array_charstr() );
        case z_array_widestr: return to_string( *get_array_widestr() );
        case z_array_zmap:    return to_string( *get_array_zmap() );
        case z_array_zval:    return to_string( *get_array_zval() );
        case z_array_uuid:    return to_string( *get_array_uuid() );

        case z_untyped:       return "<untyped>";
        default:
          throw std::invalid_argument( "undefined xvalue<> type" );
      }
    }

  auto  zval::fetch( zval&& zv ) noexcept -> zval&
    {
      if ( this != &zv )
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
          move( bool )

          move( charstr )
          move( widestr )
          move( zmap )
          move( uuid )

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
          move( array_uuid )
        # undef move
        }
      return *this;
    }

  auto  zval::fetch( const zval& zv ) -> zval&
  {
    if ( this != &zv )
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
        copy( bool )

        copy( charstr )
        copy( widestr )
        copy( zmap )
        copy( uuid )

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
        copy( array_uuid )
      # undef copy
      }
    return *this;
  }

  auto  zval::fetch( const zval& zv, const force_copy& ) -> zval&
  {
    if ( this != &zv )
      switch ( clear().vx_type = zv.vx_type )
      {
      # define  copy_value( _type_ )  case z_##_type_:    \
      new( &inner().v_##_type_ ) _type_##_t( zv.inner().v_##_type_ );  break;
        copy_value( char )
        copy_value( byte )
        copy_value( int16 )
        copy_value( int32 )
        copy_value( int64 )
        copy_value( word16 )
        copy_value( word32 )
        copy_value( word64 )
        copy_value( float )
        copy_value( double )
        copy_value( bool )

        copy_value( charstr )
        copy_value( widestr )
        copy_value( uuid )

        copy_value( array_char )
        copy_value( array_byte )
        copy_value( array_int16 )
        copy_value( array_int32 )
        copy_value( array_int64 )
        copy_value( array_word16 )
        copy_value( array_word32 )
        copy_value( array_word64 )
        copy_value( array_float )
        copy_value( array_double )
        copy_value( array_charstr )
        copy_value( array_widestr )
        copy_value( array_uuid )

        case z_zmap:
          new( &inner().v_zmap ) zmap( zv.inner().v_zmap.copy() );  break;

        case z_array_zval:
          {
            auto& source = zv.inner().v_array_zval;
            auto  target = new( &inner().v_array_zval ) array_zval();

            for ( auto& value: source )
              target->emplace_back( value, force_copy() );
            break;
          }

        case z_array_zmap:
          {
            auto& source = zv.inner().v_array_zmap;
            auto  target = new( &inner().v_array_zmap ) array_zmap();

            for ( auto& value: source )
              target->emplace_back( value.copy() );
            break;
          }
      # undef copy
      }
    return *this;
  }

  // zval::dump implementation

 /*
  *  copy as:
  *  - serial;
  *  - allocated serial;
  *  - stored pointer.
  */
  zval::dump::dump( const char* s ):
    source( s ),
    holder( nullptr ) {}

  zval::dump::dump( const dump& d ):
    source( d.source ),
    holder( d.holder )
  {
    if ( holder != nullptr && source == nullptr )
      ++holder->count;
  }

  zval::dump::dump( const zval& z ):
    source( nullptr ),
    holder( new zvalue{ z, 1 } )  {}

  zval::dump::dump( const zval* z ):
    source( z != nullptr ? invalid : nullptr ),
    pvalue( (zval*)z )  {}

  zval::dump::~dump() {  delete_it();  }

  void  zval::dump::delete_it()
  {
    if ( holder != nullptr && source == nullptr && --holder->count == 0 )
      delete holder;
  }

  auto  zval::dump::operator = ( const dump& s ) -> dump&
  {
    if ( this != &s )
    {
      delete_it();

      source = s.source;
      holder = s.holder;

      if ( holder != nullptr && source == nullptr )
        ++holder->count;
    }
    return *this;
  }

  auto  zval::dump::operator = ( const zval& z ) -> dump&
  {
    delete_it();
      source = nullptr;
      holder = new zvalue{ z, 1 };
    return *this;
  }

  auto  zval::dump::operator = ( const zval* z ) -> dump&
  {
    delete_it();
      source = invalid;
      pvalue = (zval*)z;
    return *this;
  }

  auto  zval::dump::get_type() const -> unsigned
  {
    unsigned t = z_untyped;

    if ( holder != nullptr )
    {
      if ( source == nullptr )
        return holder->value.get_type();
      if ( source == invalid )
        return pvalue->get_type();
      throw std::logic_error( "zval::dump was not properly initialized" );
    }

    if ( source != nullptr )
      ::FetchFrom( source, t );

    return t;
  }

# define  derive_get_dump( id, type )                                       \
  {                                                                         \
    if ( holder != nullptr )                                                \
    {                                                                       \
      if ( source == nullptr )                                              \
        return value_t<type##_t>( invalid, holder->value.get_##type() );    \
      if ( source == invalid )                                              \
        return value_t<type##_t>( invalid, pvalue->get_##type() );          \
      throw std::logic_error( "zval::dump was not properly initialized" );  \
    }                                                                       \
    if ( source == invalid )                                                \
      throw std::logic_error( "zval::dump was not properly initialized" );  \
    if ( source != nullptr && (byte)*source == id )                         \
      return value_t<type##_t>( 1 + source, nullptr );                      \
    return value_t<type##_t>();                                             \
  }

  auto  zval::dump::get_char() const -> value_t<char> derive_get_dump( z_char, char )
  auto  zval::dump::get_byte() const -> value_t<byte> derive_get_dump( z_byte, byte )
  auto  zval::dump::get_int16() const -> value_t<int16_t> derive_get_dump( z_int16, int16 )
  auto  zval::dump::get_int32() const -> value_t<int32_t> derive_get_dump( z_int32, int32 )
  auto  zval::dump::get_int64() const -> value_t<int64_t> derive_get_dump( z_int64, int64 )
  auto  zval::dump::get_word16() const -> value_t<word16_t> derive_get_dump( z_word16, word16 )
  auto  zval::dump::get_word32() const -> value_t<word32_t> derive_get_dump( z_word32, word32 )
  auto  zval::dump::get_word64() const -> value_t<word64_t> derive_get_dump( z_word64, word64 )
  auto  zval::dump::get_float() const -> value_t<float> derive_get_dump( z_float, float )
  auto  zval::dump::get_double() const -> value_t<double> derive_get_dump( z_double, double )
  auto  zval::dump::get_bool() const -> value_t<bool> derive_get_dump( z_bool, bool )
  auto  zval::dump::get_charstr() const -> value_t<charstr> derive_get_dump( z_charstr, charstr )
  auto  zval::dump::get_widestr() const -> value_t<widestr> derive_get_dump( z_widestr, widestr )
  auto  zval::dump::get_uuid() const -> value_t<uuid> derive_get_dump( z_uuid, uuid )
# undef derive_get_dump

  auto  zval::dump::get_zmap() const -> value_t<zmap::dump>
  {
    if ( holder != nullptr )
    {
      auto  pmap = source == nullptr ? holder->value.get_zmap() :
                   source == invalid ? pvalue->get_zmap() :
        throw std::logic_error( "zval::dump was not properly initialized" );

      return pmap != nullptr ?
        value_t<zmap::dump>( nullptr, zmap::dump( pmap ) ) : value_t<zmap::dump>();
    }

    if ( source == invalid )
      throw std::logic_error( "zval::dump was not properly initialized" );

    return source != nullptr && (byte)*source == zval::z_zmap ?
      value_t<zmap::dump>( 1 + source, nullptr ) : value_t<zmap::dump>();
  }

# define derive_get_dump( element )                                                           \
  {                                                                                           \
    if ( holder != nullptr )                                                                  \
    {                                                                                         \
      if ( source == nullptr )                                                                \
        return value_t<array_t<element##_t>>( nullptr, holder->value.get_array_##element() ); \
      if ( source == invalid )                                                                \
        return value_t<array_t<element##_t>>( nullptr, pvalue->get_array_##element() );       \
      throw std::logic_error( "zval::dump was not properly initialized" );                    \
    }                                                                                         \
    if ( source == invalid )                                                                  \
      throw std::logic_error( "zval::dump was not properly initialized" );                    \
    if ( source != nullptr && (byte)*source == z_array_##element )                            \
      return value_t<array_t<element##_t>>( 1 + source, nullptr );                            \
    return value_t<array_t<element##_t>>();                                                   \
  }

  auto  zval::dump::get_array_char() const -> value_t<array_t<char>>  derive_get_dump( char )
  auto  zval::dump::get_array_byte() const -> value_t<array_t<byte>> derive_get_dump( byte )
  auto  zval::dump::get_array_int16() const -> value_t<array_t<int16_t>> derive_get_dump( int16 )
  auto  zval::dump::get_array_int32() const -> value_t<array_t<int32_t>> derive_get_dump( int32 )
  auto  zval::dump::get_array_int64() const -> value_t<array_t<int64_t>> derive_get_dump( int64 )
  auto  zval::dump::get_array_word16() const -> value_t<array_t<word16_t>> derive_get_dump( word16 )
  auto  zval::dump::get_array_word32() const -> value_t<array_t<word32_t>> derive_get_dump( word32 )
  auto  zval::dump::get_array_word64() const -> value_t<array_t<word64_t>> derive_get_dump( word64 )
  auto  zval::dump::get_array_float() const -> value_t<array_t<float>> derive_get_dump( float )
  auto  zval::dump::get_array_double() const -> value_t<array_t<double>> derive_get_dump( double )
  auto  zval::dump::get_array_charstr() const -> value_t<array_t<charstr>> derive_get_dump( charstr )
  auto  zval::dump::get_array_widestr() const -> value_t<array_t<widestr>> derive_get_dump( widestr )
  auto  zval::dump::get_array_uuid() const -> value_t<array_t<uuid>> derive_get_dump( uuid )
# undef derive_get_dump

  auto  zval::dump::get_array_zval() const -> value_t<array_t<dump, zval>>
  {
    if ( holder != nullptr )
    {
      if ( source == nullptr )
        return value_t<array_t<dump, zval>>( nullptr, holder->value.get_array_zval() );
      if ( source == invalid )
        return value_t<array_t<dump, zval>>( nullptr, pvalue->get_array_zval() );
      throw std::logic_error( "zval::dump was not properly initialized" );
    }
    if ( source == invalid )
      throw std::logic_error( "zval::dump was not properly initialized" );
    if ( source != nullptr && (byte)*source == z_array_zval )
      return value_t<array_t<dump, zval>>( 1 + source, nullptr );
    return value_t<array_t<dump, zval>>();
  }

  auto  zval::dump::get_array_zmap() const -> value_t<array_t<zmap::dump, zmap>>
  {
    if ( holder != nullptr )
    {
      if ( source == nullptr )
        return value_t<array_t<zmap::dump, zmap>>( nullptr, holder->value.get_array_zmap() );
      if ( source == invalid )
        return value_t<array_t<zmap::dump, zmap>>( nullptr, pvalue->get_array_zmap() );
      throw std::logic_error( "zval::dump was not properly initialized" );
    }
    if ( source == invalid )
      throw std::logic_error( "zval::dump was not properly initialized" );
    if ( source != nullptr && (byte)*source == z_array_zmap )
      return value_t<array_t<zmap::dump, zmap>>( 1 + source, nullptr );
    return value_t<array_t<zmap::dump, zmap>>();
  }

  zval::dump::operator zval() const
  {
    zval v;

    if ( holder != nullptr )
    {
      return source == nullptr ? holder->value :
             source == invalid ? *pvalue :
        throw std::logic_error( "zval::dump was not properly initialized" );
    }

    if ( source != invalid )
      v.FetchFrom( source );

    return v;
  }

}
