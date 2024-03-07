# include "../zmap.h"
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
  zval  StrCat( const charstr& a, const charstr& b )  {  return zval( a + b );  }
  zval  StrCat( const widestr& a, const widestr& b )  {  return zval( a + b );  }
  zval  StrCat( char a, const charstr& b )  {  return zval( a + b );  }
  zval  StrCat( const charstr& a, char b )  {  return zval( a + b );  }

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
          case zval::z_array_word16:  return compare::test( *a.get_array_int32(), b );
          case zval::z_array_int32:   return compare::test( *a.get_array_int64(), b );
          case zval::z_array_word32:  return compare::test( *a.get_array_float(), b );
          case zval::z_array_int64:   return compare::test( *a.get_array_word16(), b );
          case zval::z_array_word64:  return compare::test( *a.get_array_word32(), b );
          case zval::z_array_float:   return compare::test( *a.get_array_word64(), b );
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

  // zval implementation

  auto  zval::inner() const -> const zval::inner_t& {  return *reinterpret_cast<const inner_t*>( &storage );  }
  auto  zval::inner()       ->       zval::inner_t& {  return *reinterpret_cast<      inner_t*>( &storage );  }

  zval::zval(): vx_type( z_untyped )  {}

  zval::zval( zval&& zv ): vx_type( z_untyped ) {  fetch( std::move( zv ) );  }

  zval::zval( const zval& zv ): vx_type( z_untyped )  {  fetch( zv );  }

  zval& zval::operator = ( zval&& zv )  {  return fetch( std::move( zv ) );  }

  zval& zval::operator = ( const zval& zv ) {  return fetch( zv );  }

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

  bool  zval::empty() const
    {
      return vx_type == z_untyped;
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
        destruct( bool )

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

  auto  zval::is_array() const -> bool
    {  return vx_type >= z_array_char && vx_type <= z_array_uuid;  }

  auto  zval::is_numeric() const -> bool
    {  return vx_type >= z_char && vx_type <= z_double;  }

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

  class casting
  {
    struct  cast_any
    {
      template <class To, class From>
      static  To  cast( const From& v, const To& d )
        {  return v >= std::numeric_limits<To>::min() && v <= std::numeric_limits<To>::max() ? v : d;  }
    };

    struct cast_u2s
    {
      template <class U, class S>
      static  S   cast( const U& u, const S& s )
      {
        static_assert( std::is_signed<S>::value, "signed value must be passed as target and default value" );
        static_assert( std::is_unsigned<U>::value, "unsigned value must be passed as source value" );
        return u <= std::numeric_limits<S>::max() ? u : s;
      }
    };

    struct cast_s2u
    {
      template <class U, class S>
      static  U   cast( const S& s, const U& u )
      {
        static_assert( std::is_signed<S>::value, "signed value must be passed as source value" );
        static_assert( std::is_unsigned<U>::value, "unsigned value must be passed as target and default value" );
        return s >= 0 && s <= std::numeric_limits<U>::max() ? s : u;
      }
    };

  public:
    template <class T1, class T2>
    static  T2  cast( const T1& t1, const T2& t2 )
    {
      using typecast = typename
        std::conditional<std::is_signed<T1>::value && std::is_unsigned<T2>::value, cast_s2u, typename
        std::conditional<std::is_unsigned<T1>::value && std::is_signed<T2>::value, cast_u2s, cast_any>::type>::type;

      return typecast::cast( t1, t2 );
    }
  };

  auto  zval::cast_to_int16( int16_t def ) const -> int16_t
  {
    switch ( get_type() )
    {
      case z_char:    return *get_char();
      case z_byte:    return *get_byte();
      case z_int16:   return *get_int16();
      case z_int32:   return casting::cast<int16_t>( *get_int32(), def );
      case z_int64:   return casting::cast<int16_t>( *get_int64(), def );
      case z_word16:  return casting::cast<int16_t>( *get_word16(), def );
      case z_word32:  return casting::cast<int16_t>( *get_word32(), def );
      case z_word64:  return casting::cast<int16_t>( *get_word64(), def );
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
      case z_int64:   return casting::cast<int32_t>( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return casting::cast<int32_t>( *get_word32(), def );
      case z_word64:  return casting::cast<int32_t>( *get_word64(), def );
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
      default:        return def;
    }
  }

  auto  zval::cast_to_word16( word16_t def ) const -> word16_t
  {
    switch ( get_type() )
    {
      case z_char:    return casting::cast<word16_t>( *get_char(), def );
      case z_byte:    return *get_byte();
      case z_int16:   return casting::cast<word16_t>( *get_int16(), def );
      case z_int32:   return casting::cast<word16_t>( *get_int32(), def );
      case z_int64:   return casting::cast<word16_t>( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return casting::cast<word16_t>( *get_word32(), def );
      case z_word64:  return casting::cast<word16_t>( *get_word64(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_word32( word32_t def ) const -> word32_t
  {
    switch ( get_type() )
    {
      case z_char:    return casting::cast<word32_t>( *get_char(), def );
      case z_byte:    return *get_byte();
      case z_int16:   return casting::cast<word32_t>( *get_int16(), def );
      case z_int32:   return casting::cast<word32_t>( *get_int32(), def );
      case z_int64:   return casting::cast<word32_t>( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return casting::cast<word32_t>( *get_word64(), def );
      default:        return def;
    }
  }

  auto  zval::cast_to_word64( word64_t def ) const -> word64_t
  {
    switch ( get_type() )
    {
      case z_char:    return casting::cast<word64_t>( *get_char(), def );
      case z_byte:    return *get_byte();
      case z_int16:   return casting::cast<word64_t>( *get_int16(), def );
      case z_int32:   return casting::cast<word64_t>( *get_int32(), def );
      case z_int64:   return casting::cast<word64_t>( *get_int64(), def );
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return *get_word64();
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
      case z_int32:   return *get_int32();
      case z_int64:   return *get_int64();
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return *get_word64();
      case z_float:   return *get_float();
      case z_double:  return casting::cast<float_t>( *get_double(), def );
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
      case z_int64:   return *get_int64();
      case z_word16:  return *get_word16();
      case z_word32:  return *get_word32();
      case z_word64:  return *get_word64();
      case z_float:   return *get_float();
      case z_double:  return *get_double();
      default:        return def;
    }
  }

  auto  zval::cast_to_charstr( const charstr& def ) const -> charstr
  {
    return is_array() ? def : to_string();
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
      case z_float:     return *get_float() == *v.get_float();
      case z_double:    return *get_double() == *v.get_double();
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
