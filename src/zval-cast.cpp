# include "../zmap.h"
# include <type_traits>

namespace mtc
{
  class casting
  {
    struct  cast_any
    {
      template <class From, class To>
      static  To  cast( const From& v, const To& d )
        {  return v >= std::numeric_limits<To>::min() && v <= std::numeric_limits<To>::max() ? To(v) : d;  }
    };

    struct cast_u2s
    {
      template <class From, class To>
      static  To  cast( const From& u, const To& s )
      {
        static_assert( std::is_signed<To>::value, "signed value must be passed as target and default value" );
        static_assert( std::is_unsigned<From>::value, "unsigned value must be passed as source value" );
        return u <= std::make_unsigned_t<To>(std::numeric_limits<To>::max()) ? To(u) : s;
      }
    };

    struct cast_s2u
    {
      template <class From, class To>
      static  To  cast( const From& s, const To& u )
      {
        static_assert( std::is_signed<From>::value, "signed value must be passed as source value" );
        static_assert( std::is_unsigned<To>::value, "unsigned value must be passed as target and default value" );
        return s >= 0 && std::make_unsigned_t<From>(s) <= std::numeric_limits<To>::max() ? To(s) : u;
      }
    };

    struct cast_f2s
    {
      template <class From, class To>
      static  To  cast( const From& s, const To& u )
      {
        static_assert( std::is_floating_point<From>::value, "floating-point value must be passed as source value" );
        static_assert( std::is_signed<To>::value, "signed integer value must be passed as target and default value" );
        return s >= std::numeric_limits<To>::min() && s <= std::numeric_limits<To>::max() ? To(s) : u;
      }
    };

    struct cast_f2u
    {
      template <class From, class To>
      static  To  cast( const From& s, const To& u )
      {
        static_assert( std::is_floating_point<From>::value, "floating-point value must be passed as source value" );
        static_assert( std::is_unsigned<To>::value, "unsigned integer value must be passed as target and default value" );
        return s >= 0 && s <= std::numeric_limits<To>::max() ? To(s) : u;
      }
    };

  public:
    template <class From, class To>
    static  To  cast( const From& t1, const To& t2 )
    {
      using cast_int =
        std::conditional_t<std::is_signed_v<From> && std::is_unsigned_v<To>, cast_s2u,
        std::conditional_t<std::is_unsigned_v<From> && std::is_signed_v<To>, cast_u2s, cast_any>>;
      using cast_f2i =
        std::conditional_t<std::is_signed_v<To>, cast_f2s, cast_f2u>;
      using cast_all =
        std::conditional_t<std::is_floating_point_v<From>,
          std::conditional_t<std::is_floating_point_v<To>, cast_any, cast_f2i>,
          std::conditional_t<std::is_floating_point_v<To>, cast_any, cast_int>>;

      return cast_all::cast( t1, t2 );
    }
  };

  auto  zval::cast_to_int16( int16_t def ) const -> int16_t
  {
    switch ( get_type() )
    {
      case z_char:    return *get_char();
      case z_byte:    return *get_byte();
      case z_int16:   return *get_int16();
      case z_int32:   return casting::cast( *get_int32(), def );
      case z_int64:   return casting::cast( *get_int64(), def );
      case z_word16:  return casting::cast( *get_word16(), def );
      case z_word32:  return casting::cast( *get_word32(), def );
      case z_word64:  return casting::cast( *get_word64(), def );
      case z_float:   return casting::cast( *get_float(), def );
      case z_double:  return casting::cast( *get_double(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_int32( int32_t def ) const -> int32_t
  {
    switch ( get_type() )
    {
      case z_char:    return *get_char();
      case z_byte:    return *get_byte();
      case z_int16:   return *get_int16();
      case z_int32:   return *get_int32();
      case z_int64:   return casting::cast( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return casting::cast( *get_word32(), def );
      case z_word64:  return casting::cast( *get_word64(), def );
      case z_float:   return casting::cast( *get_float(), def );
      case z_double:  return casting::cast( *get_double(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_int64( int64_t def ) const -> int64_t
  {
    switch ( get_type() )
    {
      case z_char:    return *get_char();
      case z_byte:    return *get_byte();
      case z_int16:   return *get_int16();
      case z_int32:   return *get_int32();
      case z_int64:   return *get_int64();
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return casting::cast<int64_t>( *get_word64(), def );
      case z_float:   return casting::cast( *get_float(), def );
      case z_double:  return casting::cast( *get_double(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_word16( word16_t def ) const -> word16_t
  {
    switch ( get_type() )
    {
      case z_char:    return casting::cast( *get_char(), def );
      case z_byte:    return *get_byte();
      case z_int16:   return casting::cast( *get_int16(), def );
      case z_int32:   return casting::cast( *get_int32(), def );
      case z_int64:   return casting::cast( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return casting::cast( *get_word32(), def );
      case z_word64:  return casting::cast( *get_word64(), def );
      case z_float:   return casting::cast( *get_float(), def );
      case z_double:  return casting::cast( *get_double(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_word32( word32_t def ) const -> word32_t
  {
    switch ( get_type() )
    {
      case z_char:    return casting::cast( *get_char(), def );
      case z_byte:    return *get_byte();
      case z_int16:   return casting::cast( *get_int16(), def );
      case z_int32:   return casting::cast( *get_int32(), def );
      case z_int64:   return casting::cast( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return casting::cast( *get_word64(), def );
      case z_float:   return casting::cast( *get_float(), def );
      case z_double:  return casting::cast( *get_double(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_word64( word64_t def ) const -> word64_t
  {
    switch ( get_type() )
    {
      case z_char:    return casting::cast( *get_char(), def );
      case z_byte:    return *get_byte();
      case z_int16:   return casting::cast( *get_int16(), def );
      case z_int32:   return casting::cast( *get_int32(), def );
      case z_int64:   return casting::cast( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return *get_word64();
      case z_float:   return casting::cast( *get_float(), def );
      case z_double:  return casting::cast( *get_double(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_float( float_t def ) const -> float_t
  {
    switch ( get_type() )
    {
      case z_char:    return *get_char();
      case z_byte:    return *get_byte();
      case z_int16:   return *get_int16();
      case z_int32:   return casting::cast( *get_int32(), def );
      case z_int64:   return casting::cast( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return casting::cast( *get_word32(), def );
      case z_word64:  return casting::cast( *get_word64(), def );
      case z_float:   return *get_float();
      case z_double:  return casting::cast( *get_double(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_double( double_t def ) const -> double_t
  {
    switch ( get_type() )
    {
      case z_char:    return *get_char();
      case z_byte:    return *get_byte();
      case z_int16:   return *get_int16();
      case z_int32:   return *get_int32();
      case z_int64:   return casting::cast( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return casting::cast( *get_word64(), def );
      case z_float:   return *get_float();
      case z_double:  return *get_double();
      default:        return def;
    }
  }

  auto  zval::cast_to_charstr( const charstr& def ) const -> charstr
  {
    return is_array() ? def : to_string();
  }

}
