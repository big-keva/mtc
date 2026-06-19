# include "../zmap.h"
# include <type_traits>

namespace mtc
{
  template <class T1, class T2> auto  Mul( T1 t1, T2 t2 ) {  return t1 * t2;  }
  template <class T1, class T2> auto  Div( T1 t1, T2 t2 ) {  return t1 / t2;  }
  template <class T1, class T2> auto  Mod( T1 t1, T2 t2 ) {  return t1 % t2;  }
  template <class T1, class T2> auto  Add( T1 t1, T2 t2 ) {  return t1 + t2;  }
  template <class T1, class T2> auto  Sub( T1 t1, T2 t2 ) {  return t1 - t2;  }
  template <class T1, class T2> auto  Shl( T1 t1, T2 t2 ) {  return t1 << t2;  }
  template <class T1, class T2> auto  Shr( T1 t1, T2 t2 ) {  return t1 >> t2;  }
  template <class T1, class T2> auto  And( T1 t1, T2 t2 ) {  return t1 & t2;  }
  template <class T1, class T2> auto  Xor( T1 t1, T2 t2 ) {  return t1 ^ t2;  }
  template <class T1, class T2> auto  Or ( T1 t1, T2 t2 ) {  return t1 | t2;  }

# define derive_math( Fun )       \
  template <class T>              \
  zval  Fun##Two( const zval& z, T t )  \
  {                                     \
    switch ( z.get_type() )             \
    {                                   \
      case zval::z_char:    return Fun( *z.get_char(), t );   \
      case zval::z_byte:    return Fun( *z.get_byte(), t );   \
      case zval::z_int16:   return Fun( *z.get_int16(), t );  \
      case zval::z_word16:  return Fun( *z.get_word16(), t ); \
      case zval::z_int32:   return Fun( *z.get_int32(), t );  \
      case zval::z_word32:  return Fun( *z.get_word32(), t ); \
      case zval::z_int64:   return Fun( *z.get_int64(), t );  \
      case zval::z_word64:  return Fun( *z.get_word64(), t ); \
      case zval::z_float:   return Fun( *z.get_float(), t );  \
      case zval::z_double:  return Fun( *z.get_double(), t ); \
      default:              return {};                        \
    }                                                         \
  }                                                           \
                                                              \
  template <class T>              \
  zval& Fun##One( zval& z, T t )  \
  {                               \
    switch ( z.get_type() )       \
    {                             \
      case zval::z_char:    return z = Fun( *z.get_char(), t );   \
      case zval::z_byte:    return z = Fun( *z.get_byte(), t );   \
      case zval::z_int16:   return z = Fun( *z.get_int16(), t );  \
      case zval::z_word16:  return z = Fun( *z.get_word16(), t ); \
      case zval::z_int32:   return z = Fun( *z.get_int32(), t );  \
      case zval::z_word32:  return z = Fun( *z.get_word32(), t ); \
      case zval::z_int64:   return z = Fun( *z.get_int64(), t );  \
      case zval::z_word64:  return z = Fun( *z.get_word64(), t ); \
      case zval::z_float:   return z = Fun( *z.get_float(), t );  \
      case zval::z_double:  return z = Fun( *z.get_double(), t ); \
      default:              return z.clear();                     \
    }   \
  }     \
        \
  template <class T> static       \
  auto  Fun##Two( T t, const zval& z ) -> zval   \
  {                                         \
    switch ( z.get_type() )                 \
    {                                       \
      case zval::z_char:    return Fun( t, *z.get_char() );   \
      case zval::z_byte:    return Fun( t, *z.get_byte() );   \
      case zval::z_int16:   return Fun( t, *z.get_int16() );  \
      case zval::z_word16:  return Fun( t, *z.get_word16() ); \
      case zval::z_int32:   return Fun( t, *z.get_int32() );  \
      case zval::z_word32:  return Fun( t, *z.get_word32() ); \
      case zval::z_int64:   return Fun( t, *z.get_int64() );  \
      case zval::z_word64:  return Fun( t, *z.get_word64() ); \
      case zval::z_float:   return Fun( t, *z.get_float() );  \
      case zval::z_double:  return Fun( t, *z.get_double() ); \
      default:              return {};                        \
    }                                                         \
  }                                                           \
                                                              \
  inline                                          \
  zval  Fun##Two( const zval& z, const zval& t )  \
  {                                               \
    switch ( z.get_type() )                       \
    {                                             \
      case zval::z_char:    return Fun##Two( *z.get_char(), t );   \
      case zval::z_byte:    return Fun##Two( *z.get_byte(), t );   \
      case zval::z_int16:   return Fun##Two( *z.get_int16(), t );  \
      case zval::z_word16:  return Fun##Two( *z.get_word16(), t ); \
      case zval::z_int32:   return Fun##Two( *z.get_int32(), t );  \
      case zval::z_word32:  return Fun##Two( *z.get_word32(), t ); \
      case zval::z_int64:   return Fun##Two( *z.get_int64(), t );  \
      case zval::z_word64:  return Fun##Two( *z.get_word64(), t ); \
      case zval::z_float:   return Fun##Two( *z.get_float(), t );  \
      case zval::z_double:  return Fun##Two( *z.get_double(), t ); \
      default:              return {};                        \
    }                                                         \
  }                                                           \
                                                              \
  inline                                    \
  zval& Fun##One( zval& z, const zval& t )  \
  {                                         \
    switch ( z.get_type() )                 \
    {                                       \
      case zval::z_char:    return z = Fun##Two( *z.get_char(), t );   \
      case zval::z_byte:    return z = Fun##Two( *z.get_byte(), t );   \
      case zval::z_int16:   return z = Fun##Two( *z.get_int16(), t );  \
      case zval::z_word16:  return z = Fun##Two( *z.get_word16(), t ); \
      case zval::z_int32:   return z = Fun##Two( *z.get_int32(), t );  \
      case zval::z_word32:  return z = Fun##Two( *z.get_word32(), t ); \
      case zval::z_int64:   return z = Fun##Two( *z.get_int64(), t );  \
      case zval::z_word64:  return z = Fun##Two( *z.get_word64(), t ); \
      case zval::z_float:   return z = Fun##Two( *z.get_float(), t );  \
      case zval::z_double:  return z = Fun##Two( *z.get_double(), t ); \
      default:              return z.clear(); \
    }                                        \
  }

  derive_math( Mul )
  derive_math( Div )
  derive_math( Sub )
# undef derive_math

# define derive_ints( Fun )       \
  template <class T>              \
  zval  Fun##Two( const zval& z, T t )  \
  {                                     \
    switch ( z.get_type() )             \
    {                                   \
      case zval::z_char:    return Fun( *z.get_char(), t );   \
      case zval::z_byte:    return Fun( *z.get_byte(), t );   \
      case zval::z_int16:   return Fun( *z.get_int16(), t );  \
      case zval::z_word16:  return Fun( *z.get_word16(), t ); \
      case zval::z_int32:   return Fun( *z.get_int32(), t );  \
      case zval::z_word32:  return Fun( *z.get_word32(), t ); \
      case zval::z_int64:   return Fun( *z.get_int64(), t );  \
      case zval::z_word64:  return Fun( *z.get_word64(), t ); \
      default:              return {};                        \
    }                                                         \
  }                                                           \
                                                              \
  template <class T>              \
  zval& Fun##One( zval& z, T t )  \
  {                               \
    switch ( z.get_type() )       \
    {                             \
      case zval::z_char:    return z = Fun( *z.get_char(), t );   \
      case zval::z_byte:    return z = Fun( *z.get_byte(), t );   \
      case zval::z_int16:   return z = Fun( *z.get_int16(), t );  \
      case zval::z_word16:  return z = Fun( *z.get_word16(), t ); \
      case zval::z_int32:   return z = Fun( *z.get_int32(), t );  \
      case zval::z_word32:  return z = Fun( *z.get_word32(), t ); \
      case zval::z_int64:   return z = Fun( *z.get_int64(), t );  \
      case zval::z_word64:  return z = Fun( *z.get_word64(), t ); \
      default:              return z.clear();                     \
    }     \
  }       \
          \
  template <class T>                            \
  auto  Fun##Two( T t, const zval& z ) -> zval  \
  {                                             \
    switch ( z.get_type() )                     \
    {                                           \
      case zval::z_char:    return Fun( t, *z.get_char() );   \
      case zval::z_byte:    return Fun( t, *z.get_byte() );   \
      case zval::z_int16:   return Fun( t, *z.get_int16() );  \
      case zval::z_word16:  return Fun( t, *z.get_word16() ); \
      case zval::z_int32:   return Fun( t, *z.get_int32() );  \
      case zval::z_word32:  return Fun( t, *z.get_word32() ); \
      case zval::z_int64:   return Fun( t, *z.get_int64() );  \
      case zval::z_word64:  return Fun( t, *z.get_word64() ); \
      default:              return {};                        \
    }                                                         \
  }                                                           \
              \
  inline      \
  zval  Fun##Two( const zval& z, const zval& t )  \
  {                                               \
    switch ( z.get_type() )                       \
    {                                             \
      case zval::z_char:    return Fun##Two( *z.get_char(), t );   \
      case zval::z_byte:    return Fun##Two( *z.get_byte(), t );   \
      case zval::z_int16:   return Fun##Two( *z.get_int16(), t );  \
      case zval::z_word16:  return Fun##Two( *z.get_word16(), t ); \
      case zval::z_int32:   return Fun##Two( *z.get_int32(), t );  \
      case zval::z_word32:  return Fun##Two( *z.get_word32(), t ); \
      case zval::z_int64:   return Fun##Two( *z.get_int64(), t );  \
      case zval::z_word64:  return Fun##Two( *z.get_word64(), t ); \
      default:              return {};                        \
    }                                                         \
  }                                                           \
              \
  inline      \
  zval& Fun##One( zval& z, const zval& t )  \
  {                                         \
    switch ( t.get_type() )                 \
    {                                       \
      case zval::z_char:    return z = Fun##One( z, *t.get_char() );   \
      case zval::z_byte:    return z = Fun##One( z, *t.get_byte() );   \
      case zval::z_int16:   return z = Fun##One( z, *t.get_int16() );  \
      case zval::z_word16:  return z = Fun##One( z, *t.get_word16() ); \
      case zval::z_int32:   return z = Fun##One( z, *t.get_int32() );  \
      case zval::z_word32:  return z = Fun##One( z, *t.get_word32() ); \
      case zval::z_int64:   return z = Fun##One( z, *t.get_int64() );  \
      case zval::z_word64:  return z = Fun##One( z, *t.get_word64() ); \
      default:              return z.clear();                 \
    }                                                         \
  }

  derive_ints( Mod )
  derive_ints( Shl )
  derive_ints( Shr )
  derive_ints( And )
  derive_ints( Xor )
  derive_ints( Or )
# undef derive_math

  template <class T>  zval  StrCat( const charstr& s, T t )  {  return s + std::to_string( t );  }
  template <class T>  zval  StrCat( T t, const charstr& s )  {  return std::to_string( t ) + s;  }

  template <class T>  zval  StrCat( const widestr& s, T t )  {  return {};  }
  template <class T>  zval  StrCat( T t, const widestr& s )  {  return {};  }

  inline  zval  StrCat( const charstr& s, const charstr& t )  {  return s + t;  }
  inline  zval  StrCat( const charstr& s, const widestr& t )  {  return {};  }
  inline  zval  StrCat( const widestr& s, const charstr& t )  {  return {};  }
  inline  zval  StrCat( const widestr& s, const widestr& t )  {  return s + t;  }

  inline  zval  StrCat( const zval& z, const charstr& t )
  {
    switch ( z.get_type() )
    {
      case zval::z_char:    return StrCat( *z.get_char(), t );
      case zval::z_byte:    return StrCat( *z.get_byte(), t );
      case zval::z_int16:   return StrCat( *z.get_int16(), t );
      case zval::z_word16:  return StrCat( *z.get_word16(), t );
      case zval::z_int32:   return StrCat( *z.get_int32(), t );
      case zval::z_word32:  return StrCat( *z.get_word32(), t );
      case zval::z_int64:   return StrCat( *z.get_int64(), t );
      case zval::z_word64:  return StrCat( *z.get_word64(), t );
      case zval::z_float:   return StrCat( *z.get_float(), t );
      case zval::z_double:  return StrCat( *z.get_double(), t );
      case zval::z_charstr: return StrCat( *z.get_charstr(), t );
      case zval::z_widestr: return StrCat( *z.get_widestr(), t );
      default:              return {};
    }
  }

  inline  zval  StrCat( const charstr& s, const zval& z )
  {
    switch ( z.get_type() )
    {
      case zval::z_char:    return StrCat( s, *z.get_char() );
      case zval::z_byte:    return StrCat( s, *z.get_byte() );
      case zval::z_int16:   return StrCat( s, *z.get_int16() );
      case zval::z_word16:  return StrCat( s, *z.get_word16() );
      case zval::z_int32:   return StrCat( s, *z.get_int32() );
      case zval::z_word32:  return StrCat( s, *z.get_word32() );
      case zval::z_int64:   return StrCat( s, *z.get_int64() );
      case zval::z_word64:  return StrCat( s, *z.get_word64() );
      case zval::z_float:   return StrCat( s, *z.get_float() );
      case zval::z_double:  return StrCat( s, *z.get_double() );
      case zval::z_charstr: return StrCat( s, *z.get_charstr() );
      case zval::z_widestr: return StrCat( s, *z.get_widestr() );
      default:              return {};
    }
  }

  inline  zval  StrCat( const zval& z, const widestr& t )
  {
    switch ( z.get_type() )
    {
      case zval::z_char:    return StrCat( *z.get_char(), t );
      case zval::z_byte:    return StrCat( *z.get_byte(), t );
      case zval::z_int16:   return StrCat( *z.get_int16(), t );
      case zval::z_word16:  return StrCat( *z.get_word16(), t );
      case zval::z_int32:   return StrCat( *z.get_int32(), t );
      case zval::z_word32:  return StrCat( *z.get_word32(), t );
      case zval::z_int64:   return StrCat( *z.get_int64(), t );
      case zval::z_word64:  return StrCat( *z.get_word64(), t );
      case zval::z_float:   return StrCat( *z.get_float(), t );
      case zval::z_double:  return StrCat( *z.get_double(), t );
      case zval::z_charstr: return StrCat( *z.get_charstr(), t );
      case zval::z_widestr: return StrCat( *z.get_widestr(), t );
      default:              return {};
    }
  }

  inline  zval  StrCat( const widestr& s, const zval& z )
  {
    switch ( z.get_type() )
    {
      case zval::z_char:    return StrCat( s, *z.get_char() );
      case zval::z_byte:    return StrCat( s, *z.get_byte() );
      case zval::z_int16:   return StrCat( s, *z.get_int16() );
      case zval::z_word16:  return StrCat( s, *z.get_word16() );
      case zval::z_int32:   return StrCat( s, *z.get_int32() );
      case zval::z_word32:  return StrCat( s, *z.get_word32() );
      case zval::z_int64:   return StrCat( s, *z.get_int64() );
      case zval::z_word64:  return StrCat( s, *z.get_word64() );
      case zval::z_float:   return StrCat( s, *z.get_float() );
      case zval::z_double:  return StrCat( s, *z.get_double() );
      case zval::z_charstr: return StrCat( s, *z.get_charstr() );
      case zval::z_widestr: return StrCat( s, *z.get_widestr() );
      default:              return {};
    }
  }

  template <class T>
  zval  AddTwo( const zval& z, T t )
  {
    switch ( z.get_type() )
    {
      case zval::z_char:    return Add( *z.get_char(), t );
      case zval::z_byte:    return Add( *z.get_byte(), t );
      case zval::z_int16:   return Add( *z.get_int16(), t );
      case zval::z_word16:  return Add( *z.get_word16(), t );
      case zval::z_int32:   return Add( *z.get_int32(), t );
      case zval::z_word32:  return Add( *z.get_word32(), t );
      case zval::z_int64:   return Add( *z.get_int64(), t );
      case zval::z_word64:  return Add( *z.get_word64(), t );
      case zval::z_float:   return Add( *z.get_float(), t );
      case zval::z_double:  return Add( *z.get_double(), t );
      case zval::z_charstr: return StrCat( *z.get_charstr(), t );
      case zval::z_widestr: return StrCat( *z.get_widestr(), t );
      default:              return {};
    }
  }

  template <class T>
  zval& AddOne( zval& z, T t )
  {
    switch ( z.get_type() )
    {
      case zval::z_char:    return z = Add( *z.get_char(), t );
      case zval::z_byte:    return z = Add( *z.get_byte(), t );
      case zval::z_int16:   return z = Add( *z.get_int16(), t );
      case zval::z_word16:  return z = Add( *z.get_word16(), t );
      case zval::z_int32:   return z = Add( *z.get_int32(), t );
      case zval::z_word32:  return z = Add( *z.get_word32(), t );
      case zval::z_int64:   return z = Add( *z.get_int64(), t );
      case zval::z_word64:  return z = Add( *z.get_word64(), t );
      case zval::z_float:   return z = Add( *z.get_float(), t );
      case zval::z_double:  return z = Add( *z.get_double(), t );
      case zval::z_charstr: return z = StrCat( *z.get_charstr(), t );
      case zval::z_widestr: return z = StrCat( *z.get_widestr(), t );
      default:              return z.clear();
    }
  }

  template <class T>
  zval  AddTwo( T t, const zval& z )
  {
    switch ( z.get_type() )
    {
      case zval::z_char:    return Add( t, *z.get_char() );
      case zval::z_byte:    return Add( t, *z.get_byte() );
      case zval::z_int16:   return Add( t, *z.get_int16() );
      case zval::z_word16:  return Add( t, *z.get_word16() );
      case zval::z_int32:   return Add( t, *z.get_int32() );
      case zval::z_word32:  return Add( t, *z.get_word32() );
      case zval::z_int64:   return Add( t, *z.get_int64() );
      case zval::z_word64:  return Add( t, *z.get_word64() );
      case zval::z_float:   return Add( t, *z.get_float() );
      case zval::z_double:  return Add( t, *z.get_double() );
      case zval::z_charstr: return StrCat( t, *z.get_charstr() );
      case zval::z_widestr: return StrCat( t, *z.get_widestr() );
      default:              return {};
    }
  }

  inline  zval  AddTwo( const zval& z, const zval& t )
  {
    switch ( t.get_type() )
    {
      case zval::z_char:    return AddTwo( z, *t.get_char() );
      case zval::z_byte:    return AddTwo( z, *t.get_byte() );
      case zval::z_int16:   return AddTwo( z, *t.get_int16() );
      case zval::z_word16:  return AddTwo( z, *t.get_word16() );
      case zval::z_int32:   return AddTwo( z, *t.get_int32() );
      case zval::z_word32:  return AddTwo( z, *t.get_word32() );
      case zval::z_int64:   return AddTwo( z, *t.get_int64() );
      case zval::z_word64:  return AddTwo( z, *t.get_word64() );
      case zval::z_float:   return AddTwo( z, *t.get_float() );
      case zval::z_double:  return AddTwo( z, *t.get_double() );
      case zval::z_charstr: return StrCat( z, *t.get_charstr() );
      case zval::z_widestr: return StrCat( z, *t.get_widestr() );
      default:              return {};
    }
  }

  inline  zval& AddOne( zval& z, const zval& t )
  {
    switch ( t.get_type() )
    {
      case zval::z_char:    return AddOne( z, *t.get_char() );
      case zval::z_byte:    return AddOne( z, *t.get_byte() );
      case zval::z_int16:   return AddOne( z, *t.get_int16() );
      case zval::z_word16:  return AddOne( z, *t.get_word16() );
      case zval::z_int32:   return AddOne( z, *t.get_int32() );
      case zval::z_word32:  return AddOne( z, *t.get_word32() );
      case zval::z_int64:   return AddOne( z, *t.get_int64() );
      case zval::z_word64:  return AddOne( z, *t.get_word64() );
      case zval::z_float:   return AddOne( z, *t.get_float() );
      case zval::z_double:  return AddOne( z, *t.get_double() );
      case zval::z_charstr: return z = StrCat( z, *t.get_charstr() );
      case zval::z_widestr: return z = StrCat( z, *t.get_widestr() );
      default:              return z.clear();
    }
  }

  zval  zval::operator *  ( const zval& r ) const {  return MulTwo( *this, r );  }
  zval  zval::operator /  ( const zval& r ) const {  return DivTwo( *this, r );  }
  zval  zval::operator %  ( const zval& r ) const {  return ModTwo( *this, r );  }
  zval  zval::operator +  ( const zval& r ) const {  return AddTwo( *this, r );  }
  zval  zval::operator -  ( const zval& r ) const {  return SubTwo( *this, r );  }
  zval  zval::operator << ( const zval& r ) const {  return ShlTwo( *this, r );  }
  zval  zval::operator >> ( const zval& r ) const {  return ShrTwo( *this, r );  }
  zval  zval::operator &  ( const zval& r ) const {  return AndTwo( *this, r );  }
  zval  zval::operator ^  ( const zval& r ) const {  return XorTwo( *this, r );  }
  zval  zval::operator |  ( const zval& r ) const {  return  OrTwo( *this, r );  }

  zval& zval::operator *=  ( const zval& r ) {  return MulOne( *this, r );  }
  zval& zval::operator /=  ( const zval& r ) {  return DivOne( *this, r );  }
  zval& zval::operator %=  ( const zval& r ) {  return ModOne( *this, r );  }
  zval& zval::operator +=  ( const zval& r ) {  return AddOne( *this, r );  }
  zval& zval::operator -=  ( const zval& r ) {  return SubOne( *this, r );  }
  zval& zval::operator <<= ( const zval& r ) {  return ShlOne( *this, r );  }
  zval& zval::operator >>= ( const zval& r ) {  return ShrOne( *this, r );  }
  zval& zval::operator &=  ( const zval& r ) {  return AndOne( *this, r );  }
  zval& zval::operator ^=  ( const zval& r ) {  return XorOne( *this, r );  }
  zval& zval::operator |=  ( const zval& r ) {  return  OrOne( *this, r );  }

  zval  zval::operator ~ ()  const
  {
    switch ( get_type() )
    {
      case z_char:    return (char)~*get_char();
      case z_byte:    return (uint8_t)~*get_byte();
      case z_int16:   return (int16_t)~*get_int16();
      case z_int32:   return (int32_t)~*get_int32();
      case z_int64:   return (int64_t)~*get_int64();
      case z_word16:  return (uint16_t)~*get_word16();
      case z_word32:  return (uint32_t)~*get_word32();
      case z_word64:  return (uint64_t)~*get_word64();
      default:        return zval();
    }
  }

  template <typename T> typename std::enable_if<std::is_arithmetic<T>::value,
    zval>::type operator * ( T t, const zval& z ) {  return MulTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_arithmetic<T>::value,
    zval>::type operator / ( T t, const zval& z ) {  return DivTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_arithmetic<T>::value,
    zval>::type operator + ( T t, const zval& z ) {  return AddTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_arithmetic<T>::value,
    zval>::type operator - ( T t, const zval& z ) {  return SubTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_integral<T>::value,
    zval>::type operator % ( T t, const zval& z ) {  return ModTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_integral<T>::value,
    zval>::type operator << ( T t, const zval& z ) {  return ShlTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_integral<T>::value,
    zval>::type operator >> ( T t, const zval& z ) {  return ShrTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_integral<T>::value,
    zval>::type operator & ( T t, const zval& z ) {  return AndTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_integral<T>::value,
    zval>::type operator ^ ( T t, const zval& z ) {  return XorTwo( t, z );  }
  template <typename T> typename std::enable_if<std::is_integral<T>::value,
    zval>::type operator | ( T t, const zval& z ) {  return OrTwo( t, z );  }

# define derive_impl( op )    \
  template zval operator op ( char, const zval& );      \
  template zval operator op ( uint8_t, const zval& );   \
  template zval operator op ( int16_t, const zval& );   \
  template zval operator op ( word16_t, const zval& );  \
  template zval operator op ( int32_t, const zval& );   \
  template zval operator op ( word32_t, const zval& );  \
  template zval operator op ( int64_t, const zval& );   \
  template zval operator op ( word64_t, const zval& );

  derive_impl( * )
  template zval operator * ( float, const zval& );
  template zval operator * ( double, const zval& );

  derive_impl( / )
  template zval operator / ( float, const zval& );
  template zval operator / ( double, const zval& );

  derive_impl( + )
  template zval operator + ( float, const zval& );
  template zval operator + ( double, const zval& );

  derive_impl( - )
  template zval operator - ( float, const zval& );
  template zval operator - ( double, const zval& );

  derive_impl( % )
  derive_impl( << )
  derive_impl( >> )
  derive_impl( & )
  derive_impl( ^ )
  derive_impl( | )

# undef derive_impl

  zval  operator + ( const char* s, const zval& z ) {  return StrCat( s, z );  }
  zval  operator + ( const charstr& s, const zval& z ) {  return StrCat( s, z );  }

  zval  operator + ( const widechar* s, const zval& z ) {  return StrCat( s, z );  }
  zval  operator + ( const widestr& s, const zval& z ) {  return StrCat( s, z );  }

}
