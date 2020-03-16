# include "zmap.h"
# include "serialize.h"
# include <type_traits>

namespace mtc
{
  template <class A, class B> static  zval  GetMul( A a, B b )  {  return a * b;  }
  template <class A, class B> static  zval  GetDiv( A a, B b )  {  return a / b;  }
  template <class A, class B> static  zval  GetAdd( A a, B b )  {  return a + b;  }
  template <class A, class B> static  zval  GetSub( A a, B b )  {  return a - b;  }

  template <class A, class B> static  zval  GetAnd( A a, B b )  {  return a & b;  }
  template <class A, class B> static  zval  GetXor( A a, B b )  {  return a ^ b;  }
  template <class A, class B> static  zval  Get_Or( A a, B b )  {  return a | b;  }
  template <class A, class B> static  zval  GetPct( A a, B b )  {  return a % b;  }
  template <class A, class B> static  zval  GetShl( A a, B b )  {  return a << b;  }
  template <class A, class B> static  zval  GetShr( A a, B b )  {  return a >> b;  }

  /*
    операции над zval и целым или дробным значением - макрогенерация:
    template <class V> zval  (#)( zval, V )
  */
  # define  derive_operation_xvalue_value( funcname )                     \
  template <class V>  zval  funcname( const zval& z, V v )                \
    {                                                                     \
      switch ( z.get_type() )                                             \
      {                                                                   \
        case zval::z_char:    return funcname( *z.get_char(), v );        \
        case zval::z_byte:    return funcname( *z.get_byte(), v );        \
        case zval::z_int16:   return funcname( *z.get_int16(), v );       \
        case zval::z_int32:   return funcname( *z.get_int32(), v );       \
        case zval::z_int64:   return funcname( *z.get_int64(), v );       \
        case zval::z_word16:  return funcname( *z.get_word16(), v );      \
        case zval::z_word32:  return funcname( *z.get_word32(), v );      \
        case zval::z_word64:  return funcname( *z.get_word64(), v );      \
        case zval::z_float:   return funcname( *z.get_float(), v );       \
        case zval::z_double:  return funcname( *z.get_double(), v );      \
        default:        return zval();                                    \
      }                                                                   \
    }
    derive_operation_xvalue_value( GetMul )
    derive_operation_xvalue_value( GetDiv )
    derive_operation_xvalue_value( GetAdd )
    derive_operation_xvalue_value( GetSub )
  # undef  derive_operation_xvalue_value

  /*
    операции над zval и zval - макрогенерация:
    zval  (#)( const zval&, const zval& )
  */
  # define  derive_operation_xvalue_xvalue( funcname )                    \
  zval  funcname( const zval& z, const zval& x )                          \
    {                                                                     \
      switch ( x.get_type() )                                             \
      {                                                                   \
        case zval::z_char:    return funcname( z, *x.get_char() );        \
        case zval::z_byte:    return funcname( z, *x.get_byte() );        \
        case zval::z_int16:   return funcname( z, *x.get_int16() );       \
        case zval::z_int32:   return funcname( z, *x.get_int32() );       \
        case zval::z_int64:   return funcname( z, *x.get_int64() );       \
        case zval::z_word16:  return funcname( z, *x.get_word16() );      \
        case zval::z_word32:  return funcname( z, *x.get_word32() );      \
        case zval::z_word64:  return funcname( z, *x.get_word64() );      \
        case zval::z_float:   return funcname( z, *x.get_float() );       \
        case zval::z_double:  return funcname( z, *x.get_double() );      \
        default:        return zval();                                    \
      }                                                                   \
    }
    derive_operation_xvalue_xvalue( GetMul )
    derive_operation_xvalue_xvalue( GetDiv )
    derive_operation_xvalue_xvalue( GetSub )
  # undef derive_operation_xvalue_xvalue

  /*
    специализации StrCat с поддержкой суммирования однотипных строк
  */
  template <class A, class B>
  zval  StrCat( A, B )  {  return zval();  }
  zval  StrCat( const charstr& a, const charstr& b )  {  return std::move( zval( std::move( a + b ) ) );  }
  zval  StrCat( const widestr& a, const widestr& b )  {  return std::move( zval( std::move( a + b ) ) );  }
  zval  StrCat( char a, const charstr& b )  {  return std::move( zval( std::move( a + b ) ) );  }
  zval  StrCat( const charstr& a, char b )  {  return std::move( zval( std::move( a + b ) ) );  }

  template <class V>
  zval  StrCat( const zval& z, V v )
    {
      switch ( z.get_type() )
      {
        case zval::z_char:    return StrCat( *z.get_char(),   v );
        case zval::z_byte:    return StrCat( *z.get_byte(),   v );
        case zval::z_int16:   return StrCat( *z.get_int16(),  v );
        case zval::z_int32:   return StrCat( *z.get_int32(),  v );
        case zval::z_int64:   return StrCat( *z.get_int64(),  v );
        case zval::z_word16:  return StrCat( *z.get_word16(), v );
        case zval::z_word32:  return StrCat( *z.get_word32(), v );
        case zval::z_word64:  return StrCat( *z.get_word64(), v );
        case zval::z_float:   return StrCat( *z.get_float(),  v );
        case zval::z_double:  return StrCat( *z.get_double(), v );
        case zval::z_charstr: return StrCat( *z.get_charstr(), v );
        case zval::z_widestr: return StrCat( *z.get_widestr(), v );
        default:              return zval();
      }
    }

  auto  GetAdd( const zval& z, const zval& x ) -> zval
    {
      switch ( x.get_type() )
      {
        case zval::z_char:    return mtc::GetAdd( z, *x.get_char() );
        case zval::z_byte:    return mtc::GetAdd( z, *x.get_byte() );
        case zval::z_int16:   return mtc::GetAdd( z, *x.get_int16() );
        case zval::z_int32:   return mtc::GetAdd( z, *x.get_int32() );
        case zval::z_int64:   return mtc::GetAdd( z, *x.get_int64() );
        case zval::z_word16:  return mtc::GetAdd( z, *x.get_word16() );
        case zval::z_word32:  return mtc::GetAdd( z, *x.get_word32() );
        case zval::z_word64:  return mtc::GetAdd( z, *x.get_word64() );
        case zval::z_float:   return mtc::GetAdd( z, *x.get_float() );
        case zval::z_double:  return mtc::GetAdd( z, *x.get_double() );
        case zval::z_charstr: return mtc::StrCat( z, *x.get_charstr() );
        case zval::z_widestr: return mtc::StrCat( z, *x.get_widestr() );
        default:        return zval();
      }
    }

  /*
    макрогенерация битовых операций над целочисленными значениями
  */
  # define  derive_math( funcname )                                       \
  template <class V>  zval  funcname( const zval& z, V v )                \
    {                                                                     \
      switch ( z.get_type() )                                             \
      {                                                                   \
        case zval::z_char:    return funcname( *z.get_char(), v );        \
        case zval::z_byte:    return funcname( *z.get_byte(), v );        \
        case zval::z_int16:   return funcname( *z.get_int16(), v );       \
        case zval::z_int32:   return funcname( *z.get_int32(), v );       \
        case zval::z_int64:   return funcname( *z.get_int64(), v );       \
        case zval::z_word16:  return funcname( *z.get_word16(), v );      \
        case zval::z_word32:  return funcname( *z.get_word32(), v );      \
        case zval::z_word64:  return funcname( *z.get_word64(), v );      \
        default:        return zval();                                    \
      }                                                                   \
    }                                                                     \
  zval  funcname( const zval& z, const zval& x )                          \
    {                                                                     \
      switch ( x.get_type() )                                             \
      {                                                                   \
        case zval::z_char:    return funcname( z, *x.get_char() );        \
        case zval::z_byte:    return funcname( z, *x.get_byte() );        \
        case zval::z_int16:   return funcname( z, *x.get_int16() );       \
        case zval::z_int32:   return funcname( z, *x.get_int32() );       \
        case zval::z_int64:   return funcname( z, *x.get_int64() );       \
        case zval::z_word16:  return funcname( z, *x.get_word16() );      \
        case zval::z_word32:  return funcname( z, *x.get_word32() );      \
        case zval::z_word64:  return funcname( z, *x.get_word64() );      \
        default:        return zval();                                    \
      }                                                                   \
    }
    derive_math( GetPct )
    derive_math( GetShl )
    derive_math( GetShr )
    derive_math( GetAnd )
    derive_math( GetXor )
    derive_math( Get_Or )
  # undef derive_math

  // compare procedures

  /*
    базовый функционал сравнения:
      <   0x01
      >   0x02
      ==  0x04
          0x00 - операция не поддерживается
  */
  inline  auto  CmpRes( int rc ) -> unsigned
    {
      return rc < 0 ? mtc::zval::compare_lt :
             rc > 0 ? mtc::zval::compare_gt :
                      mtc::zval::compare_eq;
    }

  // default compare functions
  template <class A, class B>
  auto  CompTo( const A&, const B& ) -> unsigned;

  struct signed_unsigned
  {
    template <class A, class B>
    static  bool  diff( const A& a, const B& b )
      {
        static_assert( std::is_signed<A>::value && std::is_unsigned<B>::value,
          "this template compares signed-to-unsigned only" );
        return a >= 0 ? ((typename std::make_unsigned<A>::type)a > b) - ((typename std::make_unsigned<A>::type)a < b) : -1;
      }
  };

  struct unsigned_signed
  {
    template <class A, class B>
    static  bool  diff( const A& a, const B& b )
      {
        static_assert( std::is_unsigned<A>::value && std::is_signed<B>::value,
          "this template compares unsigned-to-signed only" );
        return b >= 0 ? (a > (typename std::make_unsigned<B>::type)b) - (a < (typename std::make_unsigned<B>::type)b) : 1;
      }
  };

  struct same_sign_types
  {
    template <class A, class B>
    static  bool  diff( const A& a, const B& b )
      {
        static_assert( std::is_signed<A>::value == std::is_signed<B>::value
          || std::is_floating_point<A>::value
          || std::is_floating_point<B>::value, "this template compares same-signed types only" );
        return (a > b) - (a < b);
      }
  };

  template <class A, class B>
  auto  mkDiff( const A& a, const B& b ) -> int
  {
    return std::conditional<std::is_floating_point<A>::value || std::is_floating_point<B>::value || std::is_same<A, B>::value,
             same_sign_types,
             typename std::conditional<std::is_signed<A>::value,
               typename std::conditional<std::is_signed<B>::value, same_sign_types, signed_unsigned>::type,
               typename std::conditional<std::is_signed<B>::value, unsigned_signed, same_sign_types>::type>::type>::type::diff( a, b );
  }

  // compare numeric values
  # define derive_compare( _type_ ) \
  inline  auto  CompTo( const _type_& _1, const _type_& _2 ) -> unsigned {  return CmpRes( (_1 > _2) - (_1 < _2) );  }
    derive_compare( char )
    derive_compare( byte )
    derive_compare( int16_t )
    derive_compare( int32_t )
    derive_compare( int64_t )
    derive_compare( word16_t )
    derive_compare( word32_t )
    derive_compare( word64_t )
    derive_compare( float )
    derive_compare( double )
  # undef derive_compare

  # define derive_compare( _t1_, _t2_ ) \
  inline  auto  CompTo( const _t1_& _1, const _t2_& _2 ) -> unsigned {  return CmpRes( mkDiff( _1, _2 ) );  } \
  inline  auto  CompTo( const _t2_& _1, const _t1_& _2 ) -> unsigned {  return CmpRes( mkDiff( _1, _2 ) );  }
    derive_compare( char, byte )
    derive_compare( char, int16_t )
    derive_compare( char, int32_t )
    derive_compare( char, int64_t )
    derive_compare( char, word16_t )
    derive_compare( char, word32_t )
    derive_compare( char, word64_t )
    derive_compare( char, float )
    derive_compare( char, double )

    derive_compare( byte, int16_t )
    derive_compare( byte, int32_t )
    derive_compare( byte, int64_t )
    derive_compare( byte, word16_t )
    derive_compare( byte, word32_t )
    derive_compare( byte, word64_t )
    derive_compare( byte, float )
    derive_compare( byte, double )

    derive_compare( int16_t, int32_t )
    derive_compare( int16_t, int64_t )
    derive_compare( int16_t, word16_t )
    derive_compare( int16_t, word32_t )
    derive_compare( int16_t, word64_t )
    derive_compare( int16_t, float )
    derive_compare( int16_t, double )

    derive_compare( int32_t, int64_t )
    derive_compare( int32_t, word16_t )
    derive_compare( int32_t, word32_t )
    derive_compare( int32_t, word64_t )
    derive_compare( int32_t, float )
    derive_compare( int32_t, double )

    derive_compare( int64_t, word16_t )
    derive_compare( int64_t, word32_t )
    derive_compare( int64_t, word64_t )
    derive_compare( int64_t, float )
    derive_compare( int64_t, double )

    derive_compare( word16_t, word32_t )
    derive_compare( word16_t, word64_t )
    derive_compare( word16_t, float )
    derive_compare( word16_t, double )

    derive_compare( word32_t, word64_t )
    derive_compare( word32_t, float )
    derive_compare( word32_t, double )

    derive_compare( word64_t, float )
    derive_compare( word64_t, double )

    derive_compare( float, double )
  # undef derive_compare

  // compare two zval
  template <>
  auto  CompTo( const zval& z1, const zval& z2 ) -> unsigned {  return z1.CompTo( z2 );  }

  // compare two strings
  template <class C, class M>
  auto  CompTo( const std::basic_string<C>& c, const std::basic_string<M>& m ) -> unsigned
    {  return CmpRes( w_strcmp( c.c_str(), m.c_str() ) );  }

  // compare uuids
  auto  CompTo( const uuid_t& a, const uuid_t& b ) -> unsigned
    {  return CmpRes( a.compare( b ) );  }

  // compare vector values
  template <class A, class B>
  auto  CompTo( const std::vector<A>& a, const std::vector<B>& b ) -> unsigned
    {
      auto  ia = a.begin();
      auto  ib = b.begin();
      int   rc = 0x04;

      while ( ia != a.end() && ib != b.end() && (rc = CompTo( *ia, *ib )) == 0x04 )
      {
        ++ia;
        ++ib;
      }

      if ( rc == 0x04 )
        return rc;
      return ia == a.end() ? 0x01 : 0x02;
    }

  // zval implementation

  auto  zval::inner() const -> const zval::inner_t& {  return *reinterpret_cast<const inner_t*>( storage );  }
  auto  zval::inner()       ->       zval::inner_t& {  return *reinterpret_cast<      inner_t*>( storage );  }

  zval::zval(): vx_type( z_untyped )
    {}

  zval::zval( zval&& zv ): vx_type( z_untyped )
    {  fetch( std::move( zv ) );  }

  zval::zval( const zval& zv ): vx_type( z_untyped )
    {  fetch( zv );  }

  zval& zval::operator = ( zval&& zv )
    {  return fetch( std::move( zv ) );  }

  zval& zval::operator = ( const zval& zv )
    {  return fetch( zv );  }

  zval::~zval()
    {  clear();  }

  zval::zval( bool b ): vx_type( z_untyped )
    {  set_byte( b ? 1 : 0 );  }

  zval& zval::operator= ( bool b )
    {  set_byte( b ? 1 : 0 );  return *this;  }

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
  # undef derive_init

  zval::zval( const uuid_t& uuid ): vx_type( z_untyped )
    {  set_uuid( uuid );  }
  zval& zval::operator= ( const uuid_t& uuid )
    {  return set_uuid( uuid ), *this;  }

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
    derive_init( zmap )
    derive_init( charstr )
    derive_init( widestr )

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
    derive_init( array_zval )
    derive_init( array_uuid )
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
    _type_##_t*  zval::set_##_type_( const _type_##_t& t )            \
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
    derive_access_type( uuid )
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

    derive_access_type( zmap )
    derive_access_type( charstr )
    derive_access_type( widestr )
  # undef derive_access_type

  auto  zval::set_zmap()  -> zmap_t*
    {
      clear().vx_type = z_zmap;
      return new( &inner().v_zmap ) zmap_t();
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
    derive_access_array( uuid )
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
      vx_type = z_untyped;
      return *this;
    }

  auto  zval::get_type() const -> unsigned
    {  return vx_type;  }

  auto  zval::is_numeric() const -> bool
    {  return get_type() >= z_char && get_type() <= z_double;  }

  zval  zval::operator *  ( const zval& r ) const {  return GetMul( *this, r );  }
  zval  zval::operator /  ( const zval& r ) const {  return GetDiv( *this, r );  }
  zval  zval::operator %  ( const zval& r ) const {  return GetPct( *this, r );  }
  zval  zval::operator +  ( const zval& r ) const {  return GetAdd( *this, r );  }
  zval  zval::operator -  ( const zval& r ) const {  return GetSub( *this, r );  }
  zval  zval::operator << ( const zval& r ) const {  return GetShl( *this, r );  }
  zval  zval::operator >> ( const zval& r ) const {  return GetShr( *this, r );  }
  zval  zval::operator &  ( const zval& r ) const {  return GetAnd( *this, r );  }
  zval  zval::operator ^  ( const zval& r ) const {  return GetXor( *this, r );  }
  zval  zval::operator |  ( const zval& r ) const {  return Get_Or( *this, r );  }

  zval  zval::operator ~ ()  const
    {
      switch ( get_type() )
      {
        case z_char:    return ~*get_char();
        case z_byte:    return ~*get_byte();
        case z_int16:   return ~*get_int16();
        case z_int32:   return ~*get_int32();
        case z_int64:   return ~*get_int64();
        case z_word16:  return ~*get_word16();
        case z_word32:  return ~*get_word32();
        case z_word64:  return ~*get_word64();
        default:        return zval(); 
      }
    }

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

  template <class B>
  auto  zval::CompTo( const B& b ) const -> unsigned
    {
      switch ( get_type() )
      {
        case z_char:    return mtc::CompTo( *get_char(), b );
        case z_byte:    return mtc::CompTo( *get_byte(), b );
        case z_int16:   return mtc::CompTo( *get_int16(), b );
        case z_int32:   return mtc::CompTo( *get_int32(), b );
        case z_int64:   return mtc::CompTo( *get_int64(), b );
        case z_float:   return mtc::CompTo( *get_float(), b );
        case z_word16:  return mtc::CompTo( *get_word16(), b );
        case z_word32:  return mtc::CompTo( *get_word32(), b );
        case z_word64:  return mtc::CompTo( *get_word64(), b );
        case z_double:  return mtc::CompTo( *get_double(), b );

        case z_uuid:    return mtc::CompTo( *get_uuid(), b );

        case z_charstr: return mtc::CompTo( *get_charstr(), b );
        case z_widestr: return mtc::CompTo( *get_widestr(), b );

        case z_array_char:    return mtc::CompTo( *get_char(), b );
        case z_array_byte:    return mtc::CompTo( *get_array_byte(), b );
        case z_array_int16:   return mtc::CompTo( *get_array_int16(), b );
        case z_array_word16:  return mtc::CompTo( *get_array_int32(), b );
        case z_array_int32:   return mtc::CompTo( *get_array_int64(), b );
        case z_array_word32:  return mtc::CompTo( *get_array_float(), b );
        case z_array_int64:   return mtc::CompTo( *get_array_word16(), b );
        case z_array_word64:  return mtc::CompTo( *get_array_word32(), b );
        case z_array_float:   return mtc::CompTo( *get_array_word64(), b );
        case z_array_double:  return mtc::CompTo( *get_array_double(), b );

        case z_array_charstr: return mtc::CompTo( *get_array_charstr(), b );
        case z_array_widestr: return mtc::CompTo( *get_array_widestr(), b );
        case z_array_zval:    return mtc::CompTo( *get_array_zval(), b );
        default:  break;
      }
      return 0x00;
    }

  auto  zval::CompTo( const zval& x ) const -> unsigned
    {
      switch ( x.get_type() )
      {
        case z_char:    return CompTo( *x.get_char() );
        case z_byte:    return CompTo( *x.get_byte() );
        case z_int16:   return CompTo( *x.get_int16() );
        case z_int32:   return CompTo( *x.get_int32() );
        case z_int64:   return CompTo( *x.get_int64() );
        case z_float:   return CompTo( *x.get_float() );
        case z_word16:  return CompTo( *x.get_word16() );
        case z_word32:  return CompTo( *x.get_word32() );
        case z_word64:  return CompTo( *x.get_word64() );
        case z_double:  return CompTo( *x.get_double() );

        case z_uuid:    return CompTo( *x.get_uuid() );

        case z_charstr: return CompTo( *x.get_charstr() );
        case z_widestr: return CompTo( *x.get_widestr() );

        case z_array_char:    return CompTo( *x.get_array_char() );
        case z_array_byte:    return CompTo( *x.get_array_byte() );
        case z_array_int16:   return CompTo( *x.get_array_int16() );
        case z_array_word16:  return CompTo( *x.get_array_int32() );
        case z_array_int32:   return CompTo( *x.get_array_int64() );
        case z_array_word32:  return CompTo( *x.get_array_float() );
        case z_array_int64:   return CompTo( *x.get_array_word16() );
        case z_array_word64:  return CompTo( *x.get_array_word32() );
        case z_array_float:   return CompTo( *x.get_array_word64() );
        case z_array_double:  return CompTo( *x.get_array_double() );

        case z_array_charstr: return CompTo( *x.get_array_charstr() );
        case z_array_widestr: return CompTo( *x.get_array_widestr() );
        case z_array_zval:    return CompTo( *x.get_array_zval() );
        default:  break;
      }
      return 0x00;
    }

  bool  zval::operator == ( const zval& z ) const
    {
      if ( get_type() != z.get_type() )
        return false;
      switch ( get_type() )
      {
      # define derive_compare( _type_ ) case z_##_type_:  return *get_##_type_() == *z.get_##_type_();
        derive_compare( char )
        derive_compare( byte )
        derive_compare( int16 )
        derive_compare( word16 )
        derive_compare( int32 )
        derive_compare( word32 )
        derive_compare( int64 )
        derive_compare( word64 )
        derive_compare( float )
        derive_compare( double )

        derive_compare( charstr )
        derive_compare( widestr )

        derive_compare( zmap )
        derive_compare( uuid )

        derive_compare( array_char )
        derive_compare( array_byte )
        derive_compare( array_int16 )
        derive_compare( array_word16 )
        derive_compare( array_int32 )
        derive_compare( array_word32 )
        derive_compare( array_int64 )
        derive_compare( array_word64 )
        derive_compare( array_float )
        derive_compare( array_double )

        derive_compare( array_charstr )
        derive_compare( array_widestr )

        derive_compare( array_zmap )
        derive_compare( array_uuid )

      # undef derive_compare
        default:  return get_type() == z_untyped;
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

        case z_charstr:       return std::move( to_string( *get_charstr() ) );
        case z_widestr:       return std::move( to_string( *get_widestr() ) );

        case z_zmap:          return std::move( to_string( *get_zmap() ) );
        case z_uuid:          return std::move( mtc::to_string( *get_uuid() ) );

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
        case z_array_uuid:    return std::move( to_string( *get_array_uuid() ) );

        case z_untyped:       return "<untyped>";
        default:
          throw std::invalid_argument( "undefined xvalue<> type" );
      }
    }

  auto  zval::fetch( zval&& zv ) -> zval&
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

}
