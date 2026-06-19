# include "../zmap.h"
# include <type_traits>

namespace mtc
{
  template <class T, class V>
  bool  are_equal( const T& a, const V& b )  {  return (a > b) == (a < b);  }

  // compare procedures

  /*
    базовый функционал сравнения:
      <   0x01
      >   0x02
      ==  0x04
          0x00 - операция не поддерживается
  */
  template <class C> struct is_zmap {  static  constexpr bool  value = false;  };
  template <> struct is_zmap<zmap> {  static  constexpr bool  value = true;  };
  template <> struct is_zmap<zmap::dump> {  static  constexpr bool  value = true;  };

  template <class C> struct is_view {  static  constexpr bool  value = false;  };
  template <> struct is_view<zval> {  static  constexpr bool  value = true;  };
  template <> struct is_view<zval::dump> {  static  constexpr bool  value = true;  };

  template <class C> struct is_uuid {  static  constexpr bool  value = false;  };
  template <> struct is_uuid<uuid> {  static  constexpr bool  value = true;  };

  class compare
  {
    template <class T>  using is_signed = std::is_signed<T>;
    template <class T>  using is_unsigned = std::is_unsigned<T>;
    template <class T>  using is_floating = std::is_floating_point<T>;
    template <class T>  using is_number = std::integral_constant<bool,
      is_signed<T>::value || is_unsigned<T>::value || is_floating<T>::value>;

    template <class C>
    struct is_string {  static  constexpr bool  value = false;  };
    template <class C, class T, class A>  struct is_string<std::basic_string<C, T, A>> {  static  constexpr bool  value = true;  };

    template <class C>
    struct is_vector {  static  constexpr bool  value = false;  };
    template <class V, class A> struct is_vector<std::vector<V, A>> {  static  constexpr bool  value = true;  };
    template <class T1, class T2> struct is_vector<zval::dump::array_t<T1, T2>> {  static  constexpr bool  value = true;  };

    struct gt_value { template <class A, class B> static unsigned test( const A&, const B& )  {  return zval::compare_gt;  } };
    struct lt_value { template <class A, class B> static unsigned test( const A&, const B& )  {  return zval::compare_lt;  } };

    class number_value
    {
      struct easy_diff
      {
        template <class A, class B>
        static  unsigned  test( A a, B b )
          {  return  (a > b) ? zval::compare_gt : (a < b) ? zval::compare_lt : zval::compare_eq;  }
      };

      class signed_value
      {
        struct to_unsigned
        {
          template <class A, class B>
          static  unsigned  test( A a, B b )
          {
            if ( a >= 0 )
            {
              return (((typename std::make_unsigned<A>::type)a) > b) ? zval::compare_gt :
                     (((typename std::make_unsigned<A>::type)a) < b) ? zval::compare_lt : zval::compare_eq;
            }
            return zval::compare_lt;
          }
        };

      public:
        template <class A, class B>
        static  unsigned  test( A a, const B& b )
        {
          static_assert( is_signed<A>::value || is_floating<A>::value,
            "invalid template instantiation logics, first argument must be signed or floating-point" );
          static_assert( is_signed<B>::value || is_floating<B>::value || is_unsigned<B>::value,
            "invalid template instantiation logics, second argument must be numeric or floating-point" );

          using comparator_type =
            typename std::conditional<is_signed<B>::value || is_floating<B>::value, easy_diff, to_unsigned>::type;

          return comparator_type::test( a, b );
        }
      };

      struct unsigned_value
      {
        struct to_signed
        {
          template <class A, class B>
          static  unsigned  test( A a, B b )
          {
            auto  tr = signed_value::test( b, a );

            return tr == zval::compare_gt ? zval::compare_lt :
                   tr == zval::compare_lt ? zval::compare_gt : zval::compare_eq;
          }
        };

      public:
        template <class A, class B>
        static  unsigned  test( A a, const B& b )
        {
          static_assert( is_unsigned<A>::value,
            "invalid template instantiation logics, first argument must be unsigned" );
          static_assert( is_signed<B>::value || is_floating<B>::value || is_unsigned<B>::value,
            "invalid template instantiation logics, second argument must be numeric or floating-point" );

          using comparator_type =
            typename std::conditional<is_signed<B>::value, to_signed, easy_diff>::type;

          return comparator_type::test( a, b );
        }
      };

    public:
      template <class A, class B>
      static  unsigned  test( A a, const B& b )
      {
        static_assert( is_signed<A>::value || is_floating<A>::value || is_unsigned<A>::value,
          "invalid template instantiation logics, first argument must be numeric or floating-point" );

        using any_is_float = std::integral_constant<bool,
          is_floating<A>::value || is_floating<B>::value>;
        using comparator_type =
          typename std::conditional<is_zmap  <B>::value, lt_value,
          typename std::conditional<is_vector<B>::value, lt_value,
          typename std::conditional<is_string<B>::value, lt_value,
          typename std::conditional<is_uuid  <B>::value, lt_value,
          typename std::conditional<any_is_float::value, easy_diff,
          typename std::conditional<is_unsigned<A>::value, unsigned_value,
            signed_value>::type>::type>::type>::type>::type>::type;

        return comparator_type::test( a, b );
      }
    };

  // Строка больше численных значений и uuid, но меньше всех остальных - векторов и zmap'ов
    class string_value
    {
      struct to_string
      {
        template <class S1, class S2>
        static  unsigned  test( const S1& s1, const S2& s2 )
        {
          auto  rc = w_strcmp( s1.c_str(), s2.c_str() );

          return rc > 0 ? zval::compare_gt :
                 rc < 0 ? zval::compare_lt : zval::compare_eq;
        }
      };

    public:
      template <class S, class B>
      static  unsigned  test( const S& s, const B& b )
      {
        using comparator_type =
          typename std::conditional<is_number<B>::value, gt_value,
          typename std::conditional<is_uuid  <B>::value, gt_value,
          typename std::conditional<is_string<B>::value, to_string, lt_value>::type>::type>::type;

        return comparator_type::test( s, b );
      }
    };

    // Вектор больше численных значений, uuid и строк, но меньше, чем zmap
    class vector_value
    {
      struct to_vector
      {
        template <class V1, class V2>
        static  unsigned  test( const V1& a, const V2& b )
        {
          using V1_t = typename V1::value_type;
          using V2_t = typename V2::value_type;

          auto  ia = a.begin();
          auto  ib = b.begin();
          auto  rc = (unsigned)0;

          while ( ia != a.end() && ib != b.end() && (rc = compare::test( (V1_t)*ia, (V2_t)*ib )) == zval::compare_eq )
            {  ++ia;  ++ib;  }

          if ( rc == zval::compare_eq )
          {
            rc = a.size() > b.size() ? zval::compare_gt :
                 a.size() < b.size() ? zval::compare_lt : zval::compare_eq;
          }
          return rc;
        }
      };

    public:
      template <class V, class B>
      static  unsigned  test( const V& v, const B& b )
      {
        using comparator_type =
          typename std::conditional<is_zmap  <B>::value, lt_value,
          typename std::conditional<is_vector<B>::value, to_vector, gt_value>::type>::type;

        return comparator_type::test( v, b );
      }
    };

    template <class View>
    class zmap_value
    {
      template <class Pair>
      struct to_zmap
      {
        static  unsigned  test( const View& z1, const Pair& z2 )
        {
          auto  i1 = z1.begin();
          auto  i2 = z2.begin();
          int   rc;

          for ( ; i1 != z1.end() && i1 != z2.end(); ++i1, ++i2 )
          {
            if ( (rc = i1->first.compare( i2->first )) == 0 )
            {
              auto  tr = compare::test( i1->second, i2->second );

              if ( tr != zval::compare_eq )
                return tr;
            } else return rc > 0 ? zval::compare_gt : zval::compare_lt;
          }

          if ( (rc = (i1 != z1.end()) - (i2 != z2.end())) != 0 )
            return rc > 0 ? zval::compare_gt : zval::compare_lt;

          return zval::compare_eq;
        }
      };

    public:
      template <class B>
      static  unsigned  test( const View& z, const B& b )
      {
        using comparator_type =
          typename std::conditional<is_zmap<B>::value, to_zmap<B>, gt_value>::type;
        return comparator_type::test( z, b );
      }
    };

    template <class View>
    struct view_value
    {
      template <class B>
      static  unsigned  test( const View& a, const B& b )
      {
        switch ( a.get_type() )
        {
          case zval::z_char:    return compare::test( *a.get_char(), b );
          case zval::z_byte:    return compare::test( *a.get_byte(), b );
          case zval::z_int16:   return compare::test( *a.get_int16(), b );
          case zval::z_int32:   return compare::test( *a.get_int32(), b );
          case zval::z_int64:   return compare::test( *a.get_int64(), b );
          case zval::z_float:   return compare::test( *a.get_float(), b );
          case zval::z_word16:  return compare::test( *a.get_word16(), b );
          case zval::z_word32:  return compare::test( *a.get_word32(), b );
          case zval::z_word64:  return compare::test( *a.get_word64(), b );
          case zval::z_double:  return compare::test( *a.get_double(), b );
          case zval::z_bool:    return compare::test( *a.get_bool(), b );

          case zval::z_uuid:    return compare::test( *a.get_uuid(), b );

          case zval::z_charstr: return compare::test( *a.get_charstr(), b );
          case zval::z_widestr: return compare::test( *a.get_widestr(), b );

          case zval::z_zmap:    return compare::test( *a.get_zmap(), b );

          case zval::z_array_char:    return compare::test( *a.get_array_char(), b );
          case zval::z_array_byte:    return compare::test( *a.get_array_byte(), b );
          case zval::z_array_int16:   return compare::test( *a.get_array_int16(), b );
          case zval::z_array_int32:   return compare::test( *a.get_array_int32(), b );
          case zval::z_array_int64:   return compare::test( *a.get_array_int64(), b );
          case zval::z_array_word16:  return compare::test( *a.get_array_word16(), b );
          case zval::z_array_word32:  return compare::test( *a.get_array_word32(), b );
          case zval::z_array_word64:  return compare::test( *a.get_array_word64(), b );
          case zval::z_array_float:   return compare::test( *a.get_array_float(), b );
          case zval::z_array_double:  return compare::test( *a.get_array_double(), b );

          case zval::z_array_charstr: return compare::test( *a.get_array_charstr(), b );
          case zval::z_array_widestr: return compare::test( *a.get_array_widestr(), b );
          case zval::z_array_zval:    return compare::test( *a.get_array_zval(), b );
          case zval::z_array_zmap:    return compare::test( *a.get_array_zmap(), b );
          case zval::z_array_uuid:    return compare::test( *a.get_array_uuid(), b );
          case zval::z_untyped:       return 0x00;
          default:                    break;
        }
        throw std::logic_error( strprintf( "compare operation for zval type '%s' not supported",
          zval::type_name( (zval::z_type)a.get_type() ) ) );
      }
    };

    class uuid_value
    {
      struct to_uuid
      {
        static  unsigned test( const uuid& u1, const uuid& u2 )
        {
          auto  tr = u1.compare( u2 );

          return tr > 0 ? zval::compare_gt :
                 tr < 0 ? zval::compare_lt : zval::compare_eq;
        }
      };

    public:
      template <class B>
      static  unsigned  test( const uuid& u, const B& b )
      {
        using comparator_type =
          typename std::conditional<is_number<B>::value, gt_value,
          typename std::conditional<is_uuid  <B>::value, to_uuid, lt_value>::type>::type;

        return comparator_type::test( u, b );
      }
    };

    template <class T, class View>
    static  unsigned  test_with_view( const T& _1, const View& _2 )
    {
      switch ( _2.get_type() )
      {
        case zval::z_char:    return compare::test( _1, *_2.get_char() );
        case zval::z_byte:    return compare::test( _1, *_2.get_byte() );
        case zval::z_int16:   return compare::test( _1, *_2.get_int16() );
        case zval::z_int32:   return compare::test( _1, *_2.get_int32() );
        case zval::z_int64:   return compare::test( _1, *_2.get_int64() );
        case zval::z_float:   return compare::test( _1, *_2.get_float() );
        case zval::z_word16:  return compare::test( _1, *_2.get_word16() );
        case zval::z_word32:  return compare::test( _1, *_2.get_word32() );
        case zval::z_word64:  return compare::test( _1, *_2.get_word64() );
        case zval::z_double:  return compare::test( _1, *_2.get_double() );
        case zval::z_bool:    return compare::test( _1, *_2.get_bool() );

        case zval::z_uuid:    return compare::test( _1, *_2.get_uuid() );

        case zval::z_charstr: return compare::test( _1, *_2.get_charstr() );
        case zval::z_widestr: return compare::test( _1, *_2.get_widestr() );

        case zval::z_zmap:    return compare::test( _1, *_2.get_zmap() );

        case zval::z_array_char:    return compare::test( _1, *_2.get_array_char() );
        case zval::z_array_byte:    return compare::test( _1, *_2.get_array_byte() );
        case zval::z_array_int16:   return compare::test( _1, *_2.get_array_int16() );
        case zval::z_array_word16:  return compare::test( _1, *_2.get_array_int32() );
        case zval::z_array_int32:   return compare::test( _1, *_2.get_array_int64() );
        case zval::z_array_word32:  return compare::test( _1, *_2.get_array_float() );
        case zval::z_array_int64:   return compare::test( _1, *_2.get_array_word16() );
        case zval::z_array_word64:  return compare::test( _1, *_2.get_array_word32() );
        case zval::z_array_float:   return compare::test( _1, *_2.get_array_word64() );
        case zval::z_array_double:  return compare::test( _1, *_2.get_array_double() );

        case zval::z_array_charstr: return compare::test( _1, *_2.get_array_charstr() );
        case zval::z_array_widestr: return compare::test( _1, *_2.get_array_widestr() );
        case zval::z_array_zval:    return compare::test( _1, *_2.get_array_zval() );
        case zval::z_array_zmap:    return compare::test( _1, *_2.get_array_zmap() );
        case zval::z_array_uuid:    return compare::test( _1, *_2.get_array_uuid() );
        case zval::z_untyped:       return 0x00;
        default:  break;
      }
      throw std::logic_error( strprintf( "compare operation for zval type '%u' not supported",
        _2.get_type() ) );
    }
  public:
    template <class T1, class T2>
    static  unsigned  test( const T1& _1, const T2& _2 )
    {
      using comparator_type =
        typename std::conditional<is_number<T1>::value, number_value,
        typename std::conditional<is_string<T1>::value, string_value,
        typename std::conditional<is_vector<T1>::value, vector_value,
        typename std::conditional<is_uuid  <T1>::value, uuid_value,
        typename std::conditional<is_view  <T1>::value, view_value<T1>, zmap_value<T1>>::type>::type>::type>::type>::type;

      return comparator_type::test( _1, _2 );
    }
    template <class T1>
    static  unsigned  test( const T1& _1, const zval& _2 )
      {  return test_with_view( _1, _2 );  }
    template <class T1>
    static  unsigned  test( const T1& _1, const zval::dump& _2 )
      {  return test_with_view( _1, _2 );  }

  };

  auto  zval::CompTo( const zval& x ) const -> unsigned
  {
    return compare::test( *this, x );
  }

  namespace impl
  {
    template <class T>
    int   compare( const T& t1, const T& t2 ) {  return t1.compare( t2 );  }

    template <class T>
    int   compare( const std::vector<T>& v1, const std::vector<T>& v2 )
    {
      auto  i1 = v1.begin();
      auto  i2 = v2.begin();
      int   rc;

      while ( i1 != v1.end() && i2 != v2.end() )
        if ( (rc = compare( *i1++, *i2++ )) != 0 )
          return rc;

      return (i1 != v1.end()) - (i2 != v2.end());
    }

    template <> int compare( const bool& b1, const bool& b2 )
    {
      return b1 ? (b2 ? 0 : 1) : (b2 ? -1 : 0);
    }

    # define derive_compare( _type_ ) template <> int compare( const _type_& t1, const _type_& t2 ) \
      {  return t1 - t2;  }
      derive_compare( char )
      derive_compare( uint8_t )
      derive_compare( int16_t )
      derive_compare( uint16_t )
      derive_compare( int32_t )
      derive_compare( uint32_t )
    # undef derive_compare

    # define derive_compare( _type_ ) template <> int compare( const _type_& t1, const _type_& t2 ) \
      {  return (t1 > t2) - (t1 < t2);  }
      derive_compare( int64_t )
      derive_compare( uint64_t )
      derive_compare( float )
      derive_compare( double )
    # undef derive_compare
  }

  int   zval::compare( const zval& z ) const
  {
    int   rc = impl::compare( get_type(), z.get_type() );

    if ( rc != 0 )
      return rc;

    switch ( get_type() )
    {
    # define derive_compare( _type_ ) case z_##_type_:  return impl::compare( *get_##_type_(), *z.get_##_type_() );
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
      derive_compare( bool )

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
      derive_compare( array_zval )
      derive_compare( array_uuid )

    # undef derive_compare
      default:  return 0;
    }
  }

  // zval::dump implementation

  auto  zval::dump::CompTo( const dump& x ) const -> unsigned
  {
    return compare::test( *this, x );
  }

  bool  zval::dump::operator==( const dump& v ) const
  {
    auto  mytype = get_type();

    if ( mytype != v.get_type() )
      return false;

    switch ( mytype )
    {
      case z_char:      return *get_char() == *v.get_char();
      case z_byte:      return *get_byte() == *v.get_byte();
      case z_int16:     return *get_int16() == *v.get_int16();
      case z_int32:     return *get_int32() == *v.get_int32();
      case z_int64:     return *get_int64() == *v.get_int64();
      case z_word16:    return *get_word16() == *v.get_word16();
      case z_word32:    return *get_word32() == *v.get_word32();
      case z_word64:    return *get_word64() == *v.get_word64();
      case z_float:     return are_equal( *get_float(), *v.get_float() );
      case z_double:    return are_equal( *get_double(), *v.get_double() );
      case z_charstr:   return *get_charstr() == *v.get_charstr();
      case z_widestr:   return *get_widestr() == *v.get_widestr();
      case z_uuid:      return *get_uuid() == *v.get_uuid();
      case z_zmap:      return *get_zmap() == *v.get_zmap();

      case z_array_char:      return *get_array_char() == *v.get_array_char();
      case z_array_byte:      return *get_array_byte() == *v.get_array_byte();
      case z_array_int16:     return *get_array_int16() == *v.get_array_int16();
      case z_array_int32:     return *get_array_int32() == *v.get_array_int32();
      case z_array_int64:     return *get_array_int64() == *v.get_array_int64();
      case z_array_word16:    return *get_array_word16() == *v.get_array_word16();
      case z_array_word32:    return *get_array_word32() == *v.get_array_word32();
      case z_array_word64:    return *get_array_word64() == *v.get_array_word64();
      case z_array_float:     return *get_array_float() == *v.get_array_float();
      case z_array_double:    return *get_array_double() == *v.get_array_double();
      case z_array_charstr:   return *get_array_charstr() == *v.get_array_charstr();
      case z_array_widestr:   return *get_array_widestr() == *v.get_array_widestr();
      case z_array_uuid:      return *get_array_uuid() == *v.get_array_uuid();
      case z_array_zval:      return *get_array_zval() == *v.get_array_zval();
      case z_array_zmap:      return *get_array_zmap() == *v.get_array_zmap();

      default:  return false;
    }
  }

}
