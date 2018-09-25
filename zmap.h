/*

The MIT License (MIT)

Copyright (c) 2016 Андрей Коваленко aka Keva
  keva@meta.ua
  keva@rambler.ru
  skype: big_keva
  phone: +7(495)648-4058, +7(916)015-5592

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

=============================================================================

Данная лицензия разрешает лицам, получившим копию данного программного обеспечения
и сопутствующей документации (в дальнейшем именуемыми «Программное Обеспечение»),
безвозмездно использовать Программное Обеспечение без ограничений, включая неограниченное
право на использование, копирование, изменение, слияние, публикацию, распространение,
сублицензирование и/или продажу копий Программного Обеспечения, а также лицам, которым
предоставляется данное Программное Обеспечение, при соблюдении следующих условий:

Указанное выше уведомление об авторском праве и данные условия должны быть включены во
все копии или значимые части данного Программного Обеспечения.

ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ,
ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ТОВАРНОЙ ПРИГОДНОСТИ,
СООТВЕТСТВИЯ ПО ЕГО КОНКРЕТНОМУ НАЗНАЧЕНИЮ И ОТСУТСТВИЯ НАРУШЕНИЙ, НО НЕ ОГРАНИЧИВАЯСЬ
ИМИ.

НИ В КАКОМ СЛУЧАЕ АВТОРЫ ИЛИ ПРАВООБЛАДАТЕЛИ НЕ НЕСУТ ОТВЕТСТВЕННОСТИ ПО КАКИМ-ЛИБО ИСКАМ,
ЗА УЩЕРБ ИЛИ ПО ИНЫМ ТРЕБОВАНИЯМ, В ТОМ ЧИСЛЕ, ПРИ ДЕЙСТВИИ КОНТРАКТА, ДЕЛИКТЕ ИЛИ ИНОЙ
СИТУАЦИИ, ВОЗНИКШИМ ИЗ-ЗА ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ ИЛИ ИНЫХ ДЕЙСТВИЙ
С ПРОГРАММНЫМ ОБЕСПЕЧЕНИЕМ.

*/
# pragma once
# if !defined( __zmap_hpp__ )
# define __zmap_hpp__
# include "serialize.decl.h"
# include "wcsstr.h"
# include <cassert>
# include <algorithm>
# include <vector>
# include <string>
# include <atomic>
# include <type_traits>

namespace mtc
{
  enum z_type: unsigned
  {
    z_char    = 1,
    z_byte    = 2,
    z_int16   = 3,
    z_word16  = 4,
    z_int32   = 5,
    z_word32  = 6,
    z_int64   = 7,
    z_word64  = 8,
    z_float   = 9,
    z_double  = 10,

    z_charstr = 16,
    z_widestr = 17,
    z_buffer  = 18,
    z_zmap    = 19,

    z_array_char    = 32,
    z_array_byte    = 33,
    z_array_int16   = 34,
    z_array_word16  = 35,
    z_array_int32   = 36,
    z_array_word32  = 37,
    z_array_int64   = 38,
    z_array_word64  = 39,
    z_array_float   = 40,
    z_array_double  = 41,

    z_array_charstr = 48,
    z_array_widestr = 49,
    z_array_buffer  = 50,
    z_array_zmap    = 51,
    z_array_zval    = 52,

    z_untyped       = (unsigned)-1
  };

  class zval;
  class zmap;

  using byte   = uint8_t;
  using char_t = char;
  using byte_t = byte;

  using charstr = std::string;
  using widestr = std::basic_string<widechar>;

  using array_char   = std::vector<char_t>;
  using array_byte   = std::vector<byte_t>;
  using array_int16  = std::vector<int16_t>;
  using array_word16 = std::vector<word16_t>;
  using array_int32  = std::vector<int32_t>;
  using array_word32 = std::vector<word32_t>;
  using array_int64  = std::vector<int64_t>;
  using array_word64 = std::vector<word64_t>;
  using array_float  = std::vector<float_t>;
  using array_double = std::vector<double_t>;

  using array_charstr = std::vector<charstr>;
  using array_widestr = std::vector<widestr>;
  using array_zmap    = std::vector<zmap>;
  using array_zval    = std::vector<zval>;

  using charstr_t = charstr;
  using widestr_t = widestr;

  using zval_t = zval;
  using zmap_t = zmap;

  using array_char_t   = array_char;
  using array_byte_t   = array_byte;
  using array_int16_t  = array_int16;
  using array_word16_t = array_word16;
  using array_int32_t  = array_int32;
  using array_word32_t = array_word32;
  using array_int64_t  = array_int64;
  using array_word64_t = array_word64;
  using array_float_t  = array_float;
  using array_double_t = array_double;

  using array_charstr_t = array_charstr;
  using array_widestr_t = array_widestr;
  using array_zmap_t    = array_zmap;
  using array_zval_t    = array_zval;

  std::string to_string( const zval& );
  std::string to_string( const zmap& );

  namespace impl
  {
    template <class ... types>
    class get_max_size
    {
      template <class... types>
      struct get_size;

      template <class T>
      struct get_size<T>  {  enum: size_t {  size = sizeof(T)  };  };

      template <class T, class... L>
      struct get_size<T, L...>
      {
        enum: size_t {  size = (sizeof(T) > get_size<L...>::size ? sizeof(T) : get_size<L...>::size)  };
      };

    public:
      enum: size_t  {  value = get_size<types...>::size  };
    };

    template <size_t N>
    struct align
    {
      enum: size_t {  value = (N + 0x0f) & ~0x0f  };
    };

  }

  class zval
  {
    friend class zmap;

    union inner_t;

  public:     // construction
    zval();
    zval( zval&& );
    zval( const zval& );
    zval& operator = ( zval&& );
    zval& operator = ( const zval& );
   ~zval();

  public:     // special constructors
    zval( char_t t );
    zval( byte_t );
    zval( int16_t );
    zval( int32_t );
    zval( int64_t );
    zval( word16_t );
    zval( word32_t );
    zval( word64_t );
    zval( float_t );
    zval( double_t );

    zval& operator = ( char_t t );
    zval& operator = ( byte_t );
    zval& operator = ( int16_t );
    zval& operator = ( int32_t );
    zval& operator = ( int64_t );
    zval& operator = ( word16_t );
    zval& operator = ( word32_t );
    zval& operator = ( word64_t );
    zval& operator = ( float_t );
    zval& operator = ( double_t );

  # define declare_init( _type_ )         \
    zval( _type_##_t&& );                 \
    zval( const _type_##_t& );            \
    zval& operator = ( _type_##_t&& );    \
    zval& operator = ( const _type_##_t& );

    declare_init( zmap )
    declare_init( charstr )
    declare_init( widestr )
    declare_init( array_char )
    declare_init( array_byte )
    declare_init( array_int16 )
    declare_init( array_int32 )
    declare_init( array_int64 )
    declare_init( array_word16 )
    declare_init( array_word32 )
    declare_init( array_word64 )
    declare_init( array_float )
    declare_init( array_double )
    declare_init( array_charstr )
    declare_init( array_widestr )
    declare_init( array_zval )
    declare_init( array_zmap )

  public:     // access operators
  /*
    operator char_t   () const;
    operator byte_t   () const;
    operator int16_t  () const;
    operator word32_t () const;
    operator word64_t () const;
    operator word16_t () const;
    operator float_t  () const;
    operator double_t () const;
    operator charstr  () const;
    operator widestr  () const;
  */

  public:     // accessors
  # define  declare_access_type( _type_ )                           \
    const _type_##_t*  get_##_type_() const;                        \
          _type_##_t*  set_##_type_( _type_##_t = _type_##_t() );   \
          _type_##_t*  get_##_type_();

    declare_access_type( char )
    declare_access_type( byte )
    declare_access_type( int16 )
    declare_access_type( int32 )
    declare_access_type( int64 )
    declare_access_type( word16 )
    declare_access_type( word32 )
    declare_access_type( word64 )
    declare_access_type( float )
    declare_access_type( double )
  # undef declare_access_type

  # define declare_access_type( _type_ )  \
    _type_##_t*  get_##_type_();            \
    const _type_##_t*  get_##_type_() const;   \
    _type_##_t*  set_##_type_( _type_##_t&& );   \
    _type_##_t*  set_##_type_( const _type_##_t& = _type_##_t() );

    declare_access_type( charstr )
    declare_access_type( widestr )
  # undef declare_access_type

          zmap* get_zmap();
    const zmap* get_zmap() const;
          zmap* set_zmap();
          zmap* set_zmap( zmap&& );
          zmap* set_zmap( const zmap_t& );

  # define declare_access_array( _type_ )                                             \
    array_##_type_* set_array_##_type_( array_##_type_&& );                           \
    array_##_type_* set_array_##_type_( const array_##_type_& = array_##_type_() );   \
    array_##_type_* get_array_##_type_();                                             \
    const array_##_type_* get_array_##_type_() const;

    declare_access_array( char )
    declare_access_array( byte )
    declare_access_array( int16 )
    declare_access_array( int32 )
    declare_access_array( int64 )
    declare_access_array( word16 )
    declare_access_array( word32 )
    declare_access_array( word64 )
    declare_access_array( float )
    declare_access_array( double )
    declare_access_array( charstr )
    declare_access_array( widestr )
    declare_access_array( zval )
  # undef declare_access_array

    const array_zmap* get_array_zmap() const;
          array_zmap* get_array_zmap();
          array_zmap* set_array_zmap();
          array_zmap* set_array_zmap( array_zmap_t&& );

  public:     // operations
    auto  clear() -> zval&;
    auto  get_type() const -> unsigned;
    auto  is_numeric() const -> bool;

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class O>  O*      Serialize( O* ) const;
    template <class S>  S*      FetchFrom( S* );

  protected:  // arithmetic helpers
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
    template <class V>  zval  funcname( V v ) const                         \
      {                                                                     \
        switch ( get_type() )                                               \
        {                                                                   \
          case z_char:    return funcname( *get_char(), v );                \
          case z_byte:    return funcname( *get_byte(), v );                \
          case z_int16:   return funcname( *get_int16(), v );               \
          case z_int32:   return funcname( *get_int32(), v );               \
          case z_int64:   return funcname( *get_int64(), v );               \
          case z_word16:  return funcname( *get_word16(), v );              \
          case z_word32:  return funcname( *get_word32(), v );              \
          case z_word64:  return funcname( *get_word64(), v );              \
          case z_float:   return funcname( *get_float(), v );               \
          case z_double:  return funcname( *get_double(), v );              \
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
    zval  funcname( const zval& x ) const                                   \
      {                                                                     \
        switch ( x.get_type() )                                             \
        {                                                                   \
          case z_char:    return funcname( *x.get_char() );                 \
          case z_byte:    return funcname( *x.get_byte() );                 \
          case z_int16:   return funcname( *x.get_int16() );                \
          case z_int32:   return funcname( *x.get_int32() );                \
          case z_int64:   return funcname( *x.get_int64() );                \
          case z_word16:  return funcname( *x.get_word16() );               \
          case z_word32:  return funcname( *x.get_word32() );               \
          case z_word64:  return funcname( *x.get_word64() );               \
          case z_float:   return funcname( *x.get_float() );                \
          case z_double:  return funcname( *x.get_double() );               \
          default:        return zval();                                    \
        }                                                                   \
      }
      derive_operation_xvalue_xvalue( GetMul )
      derive_operation_xvalue_xvalue( GetDiv )
      derive_operation_xvalue_xvalue( GetSub )
    # undef derive_operation_xvalue_xvalue

    /*
      специализация GetAdd с поддержкой суммирования однотипных строк
    */
    template <class A, class B>
    static  zval  StrCat( A, B )  {  return zval();  }
    static  zval  StrCat( const charstr& a, const charstr& b )  {  return std::move( zval( std::move( a + b ) ) );  }
    static  zval  StrCat( const widestr& a, const widestr& b )  {  return std::move( zval( std::move( a + b ) ) );  }
    static  zval  StrCat( char a, const charstr& b )  {  return std::move( zval( std::move( a + b ) ) );  }
    static  zval  StrCat( const charstr& a, char b )  {  return std::move( zval( std::move( a + b ) ) );  }

    template <class V>  zval  StrCat( V v ) const
      {
        switch ( get_type() )
        {
          case z_char:    return StrCat( *get_char(),   v );
          case z_byte:    return StrCat( *get_byte(),   v );
          case z_int16:   return StrCat( *get_int16(),  v );
          case z_int32:   return StrCat( *get_int32(),  v );
          case z_int64:   return StrCat( *get_int64(),  v );
          case z_word16:  return StrCat( *get_word16(), v );
          case z_word32:  return StrCat( *get_word32(), v );
          case z_word64:  return StrCat( *get_word64(), v );
          case z_float:   return StrCat( *get_float(),  v );
          case z_double:  return StrCat( *get_double(), v );
          case z_charstr: return StrCat( *get_charstr(), v );
          case z_widestr: return StrCat( *get_widestr(), v );
          default:        return zval();
        }
      }
    zval  GetAdd( const zval& x ) const
      {
        switch ( x.get_type() )
        {
          case z_char:    return GetAdd( *x.get_char() );
          case z_byte:    return GetAdd( *x.get_byte() );
          case z_int16:   return GetAdd( *x.get_int16() );
          case z_int32:   return GetAdd( *x.get_int32() );
          case z_int64:   return GetAdd( *x.get_int64() );
          case z_word16:  return GetAdd( *x.get_word16() );
          case z_word32:  return GetAdd( *x.get_word32() );
          case z_word64:  return GetAdd( *x.get_word64() );
          case z_float:   return GetAdd( *x.get_float() );
          case z_double:  return GetAdd( *x.get_double() );
          case z_charstr: return StrCat( *x.get_charstr() );
          case z_widestr: return StrCat( *x.get_widestr() );
          default:        return zval();
        }
      }

    /*
      макрогенерация битовых операций над целочисленными значениями
    */
    # define  derive_math( funcname )                                       \
    template <class V>  zval  funcname( V v ) const                         \
      {                                                                     \
        switch ( get_type() )                                               \
        {                                                                   \
          case z_char:    return funcname( *get_char(), v );                \
          case z_byte:    return funcname( *get_byte(), v );                \
          case z_int16:   return funcname( *get_int16(), v );               \
          case z_int32:   return funcname( *get_int32(), v );               \
          case z_int64:   return funcname( *get_int64(), v );               \
          case z_word16:  return funcname( *get_word16(), v );              \
          case z_word32:  return funcname( *get_word32(), v );              \
          case z_word64:  return funcname( *get_word64(), v );              \
          default:        return zval();                                    \
        }                                                                   \
      }                                                                     \
    zval  funcname( const zval& x ) const                                   \
      {                                                                     \
        switch ( x.get_type() )                                             \
        {                                                                   \
          case z_char:    return funcname( *x.get_char() );                 \
          case z_byte:    return funcname( *x.get_byte() );                 \
          case z_int16:   return funcname( *x.get_int16() );                \
          case z_int32:   return funcname( *x.get_int32() );                \
          case z_int64:   return funcname( *x.get_int64() );                \
          case z_word16:  return funcname( *x.get_word16() );               \
          case z_word32:  return funcname( *x.get_word32() );               \
          case z_word64:  return funcname( *x.get_word64() );               \
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

  public:     // arithmetic
    zval operator *   ( const zval& r ) const {  return GetMul( r );  }
    zval operator /   ( const zval& r ) const {  return GetDiv( r );  }
    zval operator %   ( const zval& r ) const {  return GetPct( r );  }
    zval operator +   ( const zval& r ) const {  return GetAdd( r );  }
    zval operator -   ( const zval& r ) const {  return GetSub( r );  }
    zval operator <<  ( const zval& r ) const {  return GetShl( r );  }
    zval operator >>  ( const zval& r ) const {  return GetShr( r );  }
    zval operator &   ( const zval& r ) const {  return GetAnd( r );  }
    zval operator ^   ( const zval& r ) const {  return GetXor( r );  }
    zval operator |   ( const zval& r ) const {  return Get_Or( r );  }

  protected:  // compare helpers
    /*
      базовый функционал сравнения:
        <   0x01
        >   0x02
        ==  0x04
        !=  0x08
            0x00 - операция не поддерживается
    */
    template <class A, class B>
    static  int   CompTo( A a, B b )
      {
        int    rc = (double(a) - double(b) > 0) - (double(a) - double(b) < 0);
        return rc < 0 ? 0x01 + 0x08 : rc > 0 ? 0x02 + 0x08 : 0x04;
      }
    template <class c1, class c2>
    static int  CompTo( const std::basic_string<c1>& s1, const std::basic_string<c2>& s2 )
      {
        int    rc = w_strcmp( s1.c_str(), s2.c_str() );
        return rc < 0 ? 0x01 + 0x08 : rc > 0 ? 0x02 + 0x08 : 0x04;
      }

    # define  derive_noncmp( c1, c2 ) static  int   CompTo( c1, c2 )  {  return 0;  } \
                                      static  int   CompTo( c2, c1 )  {  return 0;  }
      derive_noncmp( charstr, char )
      derive_noncmp( charstr, byte_t )
      derive_noncmp( charstr, int16_t )
      derive_noncmp( charstr, int32_t )
      derive_noncmp( charstr, int64_t )
      derive_noncmp( charstr, word16_t )
      derive_noncmp( charstr, word32_t )
      derive_noncmp( charstr, word64_t )
      derive_noncmp( charstr, float )
      derive_noncmp( charstr, double )
      derive_noncmp( widestr, char )
      derive_noncmp( widestr, byte_t )
      derive_noncmp( widestr, int16_t )
      derive_noncmp( widestr, int32_t )
      derive_noncmp( widestr, int64_t )
      derive_noncmp( widestr, word16_t )
      derive_noncmp( widestr, word32_t )
      derive_noncmp( widestr, word64_t )
      derive_noncmp( widestr, float )
      derive_noncmp( widestr, double )
    # undef derive_noncmp

    template <class B>  int   CompTo( const B& b ) const
      {
        switch ( get_type() )
        {
          case z_char:    return CompTo( *get_char(), b );
          case z_byte:    return CompTo( *get_byte(), b );
          case z_int16:   return CompTo( *get_int16(), b );
          case z_int32:   return CompTo( *get_int32(), b );
          case z_int64:   return CompTo( *get_int64(), b );
          case z_float:   return CompTo( *get_float(), b );
          case z_word16:  return CompTo( *get_word16(), b );
          case z_word32:  return CompTo( *get_word32(), b );
          case z_word64:  return CompTo( *get_word64(), b );
          case z_double:  return CompTo( *get_double(), b );

          case z_charstr: return CompTo( *get_charstr(), b );
          case z_widestr: return CompTo( *get_widestr(), b );

          case z_array_char:
          case z_array_byte:
          case z_array_int16:
          case z_array_word16:
          case z_array_int32:
          case z_array_word32:
          case z_array_int64:
          case z_array_word64:
          case z_array_float:
          case z_array_double:

          case z_array_charstr:
          case z_array_widestr:
          case z_array_buffer:
          case z_array_zmap:
          case z_array_zval:
          default:  break;
        }
        return 0;
      }
    int CompTo( const zval& x ) const
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

          case z_charstr: return CompTo( *x.get_charstr() );
          case z_widestr: return CompTo( *x.get_widestr() );

          case z_array_char:
          case z_array_byte:
          case z_array_int16:
          case z_array_word16:
          case z_array_int32:
          case z_array_word32:
          case z_array_int64:
          case z_array_word64:
          case z_array_float:
          case z_array_double:

          case z_array_charstr:
          case z_array_widestr:
          case z_array_buffer:
          case z_array_zmap:
          case z_array_zval:
          default:  break;
        }
        return 0;
      }
  public:     // compare
    bool  operator == ( const zval& v ) const { return (CompTo( v ) & 0x04) != 0; }
    bool  operator != ( const zval& v ) const { return (CompTo( v ) & 0x08) != 0; }
    bool  operator <  ( const zval& v ) const { return (CompTo( v ) & 0x01) != 0; }
    bool  operator >  ( const zval& v ) const { return (CompTo( v ) & 0x02) != 0; }
    bool  operator <= ( const zval& v ) const { return (CompTo( v ) & 0x05) != 0; }
    bool  operator >= ( const zval& v ) const { return (CompTo( v ) & 0x06) != 0; }

  protected:  // stringize helpers
    auto  to_string( char c ) const         -> std::string  {  return std::move( std::string( { '\'', c, '\'', 0 } ) );  }
    auto  to_string( byte_t v ) const       -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( int16_t v ) const      -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( int32_t v ) const      -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( int64_t v ) const      -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( uint16_t v ) const     -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( uint32_t v ) const     -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( uint64_t v ) const     -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( float v ) const        -> std::string  {  return std::move( std::to_string( v ) );  }
    auto  to_string( double v ) const       -> std::string  {  return std::move( std::to_string( v ) );  }

    auto  to_string( const widechar* v ) const  -> std::string
      {
        throw std::runtime_error( "not implemented" );
      }

    auto  to_string( const zval& v ) const  ->  std::string {  return std::move( mtc::to_string( v ) );  }
    auto  to_string( const zmap& v ) const  ->  std::string {  return std::move( mtc::to_string( v ) );  }
    auto  to_string( const charstr& v ) const ->  std::string {  return std::move( '"'+v+'"' );  }
    auto  to_string( const widestr& v ) const ->  std::string {  return std::move( '"' + to_string( v.c_str() ) + '"' );  }

    template <class V>
    auto  to_string( const std::vector<V>& arr ) const  ->  std::string 
      {
        std::string out( "[" );

        for ( auto& val: arr )
          out += (to_string( val )+',');

        return out.back() = ']', std::move( out );
      }

  public:     // stringize
    std::string to_string() const;

  protected:  // helpers
    auto  fetch( zval&& ) -> zval&;
    auto  fetch( const zval& ) -> zval&;

    auto  inner() const -> const inner_t&;
    auto  inner()       ->       inner_t&;

  protected:  // inplace storage
    char      storage[impl::align<impl::get_max_size<uint64_t, double,
      charstr,
      widestr,
      std::vector<uint64_t>,
      std::vector<widestr>>::value>::value];
    unsigned  vx_type;

  };

  class zmap
  {
    class ztree_t;
    template <class map>
    class silly_place_t;
    class const_place_t;
    class patch_place_t;

  protected:
    template <class value>
    class iterator_data;
    template <class value, class ztree_iterator>
    class iterator_base;

  public:     // iterator support
    class key;
    class iterator;
    class const_iterator;

  public:
    zmap();
    zmap( zmap&& );
    zmap( const std::initializer_list<std::pair<key, zval>>& );
    zmap( const zmap& ) = delete;
    zmap& operator = ( zmap&& );
   ~zmap();

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class O>  O*      Serialize( O* o ) const;
    template <class S>  S*      FetchFrom( S* );

  public:     // copying
    auto  copy() const  -> zmap;

  public:     // put family
    auto  put( const key&, zval&& )               -> zval*;
    auto  put( const key&, const zval& = zval() ) -> zval*;

  public:     // get_zval family
    auto  get( const key& ) const        -> const zval*;
    auto  get( const key& )              ->       zval*;
    auto  get_type( const key& ) const        -> decltype(zval::vx_type);

  public:     // getters by type
  # define declare_get_type( _type_ )                                 \
    auto  get_##_type_( const key& )       ->       _type_##_t*;      \
    auto  get_##_type_( const key& ) const -> const _type_##_t*;

  # define declare_get_init( _type_ )                                 \
    auto  get_##_type_( const key&, const _type_##_t& ) const -> _type_##_t;

  # define declare_set_pure( _type_ )                                 \
    auto  set_##_type_( const key& ) -> _type_##_t*;

  # define declare_set_move( _type_ )                                 \
    auto  set_##_type_( const key&, _type_##_t&& ) -> _type_##_t*;

  # define declare_set_copy( _type_ )                                 \
    auto  set_##_type_( const key&, const _type_##_t& = _type_##_t() ) -> _type_##_t*;

    declare_get_type( char    )
    declare_get_type( byte    )
    declare_get_type( int16   )
    declare_get_type( int32   )
    declare_get_type( int64   )
    declare_get_type( word16  )
    declare_get_type( word32  )
    declare_get_type( word64  )
    declare_get_type( float   )
    declare_get_type( double  )
    declare_get_type( charstr )
    declare_get_type( widestr )

    declare_get_init( char    )
    declare_get_init( byte    )
    declare_get_init( int16   )
    declare_get_init( int32   )
    declare_get_init( int64   )
    declare_get_init( word16  )
    declare_get_init( word32  )
    declare_get_init( word64  )
    declare_get_init( float   )
    declare_get_init( double  )
    declare_get_init( charstr )
    declare_get_init( widestr )

    declare_get_type( zmap    )
    declare_get_type( array_char )
    declare_get_type( array_byte    )
    declare_get_type( array_int16   )
    declare_get_type( array_int32   )
    declare_get_type( array_int64   )
    declare_get_type( array_word16  )
    declare_get_type( array_word32  )
    declare_get_type( array_word64  )
    declare_get_type( array_float   )
    declare_get_type( array_double  )
    declare_get_type( array_charstr )
    declare_get_type( array_widestr )
    declare_get_type( array_zmap    )
    declare_get_type( array_zval    )

    declare_set_copy( char    )
    declare_set_copy( byte    )
    declare_set_copy( int16   )
    declare_set_copy( int32   )
    declare_set_copy( int64   )
    declare_set_copy( word16  )
    declare_set_copy( word32  )
    declare_set_copy( word64  )
    declare_set_copy( float   )
    declare_set_copy( double  )
    declare_set_copy( charstr )
    declare_set_copy( widestr )
    declare_set_copy( array_char )
    declare_set_copy( array_byte    )
    declare_set_copy( array_int16   )
    declare_set_copy( array_int32   )
    declare_set_copy( array_int64   )
    declare_set_copy( array_word16  )
    declare_set_copy( array_word32  )
    declare_set_copy( array_word64  )
    declare_set_copy( array_float   )
    declare_set_copy( array_double  )
    declare_set_copy( array_charstr )
    declare_set_copy( array_widestr )
    declare_set_copy( array_zval    )

    declare_set_pure( zmap    )
    declare_set_pure( array_zmap    )

    declare_set_move( charstr )
    declare_set_move( widestr )
    declare_set_move( zmap    )
    declare_set_move( array_char )
    declare_set_move( array_byte    )
    declare_set_move( array_int16   )
    declare_set_move( array_int32   )
    declare_set_move( array_int64   )
    declare_set_move( array_word16  )
    declare_set_move( array_word32  )
    declare_set_move( array_word64  )
    declare_set_move( array_float   )
    declare_set_move( array_double  )
    declare_set_move( array_charstr )
    declare_set_move( array_widestr )
    declare_set_move( array_zmap    )
    declare_set_move( array_zval    )
  # undef declare_set_pure
  # undef declare_set_move
  # undef declare_set_copy
  # undef declare_get_type

  public:     // iterators
          iterator  begin();
          iterator  end();
    const_iterator  begin() const;
    const_iterator  end() const;
    const_iterator  cbegin() const;
    const_iterator  cend() const;

  public:     // properties
    constexpr
    auto  max_size() const -> size_t {  return std::numeric_limits<size_t>::max();  }
    auto  empty() const -> bool;
    auto  size() const  -> size_t;

  public:     // direct element access
    auto  at( const key& )       ->       zval&;
    auto  at( const key& ) const -> const zval&;

  public:     // c++ access
    auto  operator []( const key& )       ->       patch_place_t;
    auto  operator []( const key& ) const -> const const_place_t;

  public:     // modifiers
    auto  clear() -> void;

    /*
    auto  insert( const key&, zval&& ) -> std::pair<iterator, bool>;
    auto  insert( const key&, const zval& ) -> std::pair<iterator, bool>;
    auto  insert( const std::pair<key, zval>& ) -> std::pair<iterator, bool>;
    auto  insert( const std::initializer_list<std::pair<key, zval>>& ) -> void;
    */
    /*
    auto  erase( const_iterator ) -> iterator;
    auto  erase( const_iterator, const_iterator ) -> iterator;
    */
    auto  erase( const key& ) -> size_t;

  public:     // search
    /*
    auto  find( const key& ) -> iterator;
    auto  find( const key& ) const -> const_iterator;

    auto  lower_bound( const key& ) -> iterator;
    auto  lower_bound( const key& ) const -> const_iterator;

    auto  upper_bound( const key& ) -> iterator;
    auto  upper_bound( const key& ) const -> const_iterator;
    */

  public:     // compare operators
    bool operator== ( const zmap& z ) const;
    bool operator!= ( const zmap& z ) const {  return !(*this == z);  }
    bool operator<  ( const zmap& z ) const;
    bool operator<= ( const zmap& z ) const;
    bool operator>  ( const zmap& z ) const;
    bool operator>= ( const zmap& z ) const;

  protected:
    auto  z_tree() const -> const std::unique_ptr<ztree_t>&;
    auto  z_tree()       ->       std::unique_ptr<ztree_t>&;

  protected:
    char    z_data[impl::get_max_size<std::unique_ptr<ztree_t>>::value];
    size_t  n_vals;

  };

  inline  std::string to_string( const zval& z ) {  return std::move( z.to_string() );  }
  inline  std::string to_string( const zmap& z ) {  return std::move( ""/*z.to_string() */);  }
          std::string to_string( const zmap::key& );

}

inline  size_t  GetBufLen( const mtc::zval& v ) {  return v.GetBufLen();  }
inline  size_t  GetBufLen( const mtc::zmap& m ) {  return m.GetBufLen();  }

template <class O>
inline  O*      Serialize( O* o, const mtc::zval& v ) {  return v.Serialize( o );  }
template <class O>
inline  O*      Serialize( O* o, const mtc::zmap& m ) {  return m.Serialize( o );  }

template <class S>
inline  S*      FetchFrom( S* s, mtc::zval& v ) {  return v.FetchFrom( s );  }
template <class S>
inline  S*      FetchFrom( S* s, mtc::zmap& m ) {  return m.FetchFrom( s );  }

namespace mtc
{
  // zval implementation

  union zval::inner_t
  {
   ~inner_t() = delete;

  # define derive_var( _type_ ) _type_##_t  v_##_type_;
    derive_var( char )
    derive_var( byte )
    derive_var( int16 )
    derive_var( int32 )
    derive_var( int64 )
    derive_var( word16 )
    derive_var( word32 )
    derive_var( word64 )
    derive_var( float )
    derive_var( double )

    derive_var( charstr )
    derive_var( widestr )
    derive_var( zmap )

    derive_var( array_char )
    derive_var( array_byte )
    derive_var( array_int16 )
    derive_var( array_int32 )
    derive_var( array_int64 )
    derive_var( array_word16 )
    derive_var( array_word32 )
    derive_var( array_word64 )
    derive_var( array_float )
    derive_var( array_double )
    derive_var( array_charstr )
    derive_var( array_widestr )
    derive_var( array_zval )
    derive_var( array_zmap )
  # undef derive_var
  };

  template <class O>
  inline  O*  zval::Serialize( O* o ) const
  {
    switch ( vx_type )
    {
  # define derive_put_plain( _type_ )  case z_##_type_: return ::Serialize( ::Serialize( o, vx_type ), &inner().v_##_type_, sizeof(_type_##_t) );
  # define derive_put_smart( _type_ )  case z_##_type_: return ::Serialize( ::Serialize( o, vx_type ),  inner().v_##_type_ );
      derive_put_plain( char )
      derive_put_plain( byte )
      derive_put_plain( int16 )
      derive_put_plain( word16 )

      derive_put_smart( int32 )
      derive_put_smart( word32 )
      derive_put_smart( int64 )
      derive_put_smart( word64 )
      derive_put_smart( float )
      derive_put_smart( double )
      derive_put_smart( charstr )
      derive_put_smart( widestr )
      derive_put_smart( zmap )
      derive_put_smart( array_char )
      derive_put_smart( array_byte )
      derive_put_smart( array_float )
      derive_put_smart( array_double )
      derive_put_smart( array_int16 )
      derive_put_smart( array_word16 )
      derive_put_smart( array_int32 )
      derive_put_smart( array_word32 )
      derive_put_smart( array_int64 )
      derive_put_smart( array_word64 )
      derive_put_smart( array_charstr )
      derive_put_smart( array_widestr )
      derive_put_smart( array_zval )
      derive_put_smart( array_zmap )
  # undef derive_put_smart
  # undef derive_put_plain
      default:  return nullptr;
    }
  }

  template <class S>
  inline  S* zval::FetchFrom( S* s )
  {
    byte_t  intype;

    if ( (s = ::FetchFrom( s, intype )) != nullptr ) clear();
      else  return nullptr;

    switch ( intype )
    {
  # define derive_get_plain( _type_ )                                                 \
    case z_##_type_: return ::FetchFrom( s, set_##_type_(), sizeof(_type_##_t) );
  # define derive_get_smart( _type_ )                                                 \
    case z_##_type_: return ::FetchFrom( s, *set_##_type_() );
      derive_get_plain( char )
      derive_get_plain( byte )
      derive_get_plain( int16 )
      derive_get_plain( word16 )

      derive_get_smart( int32 )
      derive_get_smart( word32 )
      derive_get_smart( int64 )
      derive_get_smart( word64 )
      derive_get_smart( float )
      derive_get_smart( double )
      derive_get_smart( charstr )
      derive_get_smart( widestr )
      derive_get_smart( zmap )
      derive_get_smart( array_char )
      derive_get_smart( array_byte )
      derive_get_smart( array_float )
      derive_get_smart( array_double )
      derive_get_smart( array_int16 )
      derive_get_smart( array_word16 )
      derive_get_smart( array_int32 )
      derive_get_smart( array_word32 )
      derive_get_smart( array_int64 )
      derive_get_smart( array_word64 )
      derive_get_smart( array_charstr )
      derive_get_smart( array_zmap )
      derive_get_smart( array_zval )
  # undef derive_get_smart
  # undef derive_get_plain
      default:  return nullptr;
    }
  }

  // zmap:: classes

  /*
    zmap::ztree_t - собственно префиксное дерево, хранящее все данные
  */
  class zmap::ztree_t: public std::vector<ztree_t>
  {
    friend class zmap;
    template <class value, class z_iterator>
    friend class iterator_base;

    uint8_t               chnode;
    uint8_t               keyset;     // the key type
    std::unique_ptr<zval> pvalue;     // the element value

  public:
    ztree_t( byte_t chinit = '\0' );
    ztree_t( ztree_t&& );
    ztree_t& operator = ( ztree_t&& );

    ztree_t( const ztree_t& );
    ztree_t& operator = ( const ztree_t& ) = delete;

  protected:  // search implementation
    template <class self> static
    auto  search( self& _me, const uint8_t* key, size_t cch ) -> self*;

  public:     // unserialized tree work
    auto  insert( const uint8_t* key, size_t cch ) -> ztree_t*;
    auto  remove( const uint8_t* key, size_t cch ) -> size_t;
    auto  search( const uint8_t* key, size_t cch ) const  {  return search( *this, key, cch );  }
    auto  search( const uint8_t* key, size_t cch )        {  return search( *this, key, cch );  }

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class O>  O*      Serialize( O* ) const;
    template <class S>  S*      FetchFrom( S* );

  protected:  // helpers
    int   plain_branchlen() const;

  };

  class zmap::key
  {
    friend class zmap;
    template <class value, class z_iterator>
    friend class iterator_base;

  private:
    unsigned        _typ;
    const uint8_t*  _ptr;
    size_t          _len;

  private:
    uint8_t         _buf[4];

  public:     // key types
    enum: uint8_t
    {
      uint = 0,
      cstr = 1,
      wstr = 2,
      none = (uint8_t)-1
    };

  protected:
    key();
    key( unsigned typ, const uint8_t* buf, size_t len );
    key( unsigned typ, const std::string& val );

  public: // construction
    key( unsigned );
    key( const char* );
    key( const widechar* );
    key( const key& );
    key& operator = ( const key& );

  public: // data
    auto  type() const  -> unsigned       {  return _typ;  }
    auto  data() const  -> const uint8_t* {  return _ptr;  }
    auto  size() const  -> size_t         {  return _len;  }

  public: // operators
    operator unsigned () const;
    operator const char* () const;
    operator const widechar* () const;
  };

  template <class value>
  class zmap::iterator_data
  {
    template <class value, class z_iterator>
    friend class iterator_base;

    iterator_data(): second( *(value*)nullptr ) {}
    iterator_data( const key& k, value* v ): first( k ), second( *v ) {}

  public:
    key     first;
    value&  second;

  };

  template <class value, class z_iterator>
  class zmap::iterator_base
  {
    friend class zmap;

  protected:
    iterator_base( z_iterator beg, z_iterator end );

  public:
    iterator_base();
    iterator_base( const iterator_base& );

  public:
    auto  operator -> () const -> const value*;
    auto  operator * () const -> const value&;
    auto  operator ++ () -> const iterator_base&;
    auto  operator -- () -> const iterator_base&;
    auto  operator ++ ( int ) -> iterator_base;
    auto  operator -- ( int ) -> iterator_base;
    auto  operator == ( const iterator_base& it ) const -> bool;
    auto  operator != ( const iterator_base& it ) const -> bool {  return !(*this == it);  }

  protected:
    using zpos = std::pair<z_iterator, z_iterator>;

    auto  init() -> iterator_base&;
    auto  find() -> iterator_base&;
    auto  next() -> iterator_base&;
    auto  prev() -> iterator_base&;
    void  down( z_iterator );
    void  back();
    auto  last()       ->       zpos&;
    auto  last() const -> const zpos&;

  protected:
    value             zvalue;
    std::vector<zpos> zstack;
    std::string       z_buff;

  };

  class zmap::const_iterator: public iterator_base<iterator_data<const zval>, ztree_t::const_iterator>
  {
    friend class zmap;

    using iterator_base::iterator_base;
  };

  class zmap::iterator: public iterator_base<iterator_data<zval>, ztree_t::iterator>
  {
    friend class zmap;

    using iterator_base::iterator_base;
  };

  class zmap::const_place_t
  {
    friend class zmap;

    const_place_t( const key&, zmap& );
    const_place_t( const_place_t&& );

  public:
    operator char         () const;
    operator byte_t       () const;
    operator int16_t      () const;
    operator int32_t      () const;
    operator int64_t      () const;
    operator word16_t     () const;
    operator word32_t     () const;
    operator word64_t     () const;
    operator float_t      () const;
    operator double_t     () const;
    operator charstr      () const;
    operator widestr      () const;
    operator const zmap&  () const;

  protected:
    zmap_t    empty;
    key       refer;
    zmap_t&   owner;
  };

  class zmap::patch_place_t: public const_place_t
  {
    friend class zmap;

    using const_place_t::const_place_t;

  public:
  # define declare_assign( _type_ )   auto  operator = ( const _type_##_t& ) -> patch_place_t&;
    declare_assign( char )
    declare_assign( byte )
    declare_assign( int16 )
    declare_assign( int32 )
    declare_assign( int64 )
    declare_assign( word16 )
    declare_assign( word32 )
    declare_assign( word64 )
    declare_assign( float )
    declare_assign( double )
    declare_assign( charstr )
    declare_assign( widestr )
  # undef declare_assign
    auto  operator = ( charstr&& s ) -> patch_place_t&;
    auto  operator = ( widestr&& s ) -> patch_place_t&;
  };

  /*
    zmap::ztree_t inline implementation
  */

  template <class O>
  O*   zmap::ztree_t::Serialize( O* o ) const
  {
    int       branch = plain_branchlen();
      assert( branch <= 0x100 );
      assert( size() <= 0x100 );
    word16_t  lstore = static_cast<word16_t>( (branch > 0 ? 0x0400 + branch : size()) + (pvalue != nullptr ? 0x0200 : 0) );

    o = ::Serialize( o, lstore );

    if ( pvalue != nullptr )
      o = pvalue->Serialize( ::Serialize( o, keyset ) );

    if ( branch > 0 )
    {
      auto  pbeg = data();

      while ( pbeg->size() == 1 && pbeg->pvalue == nullptr )
        {  o = ::Serialize( o, pbeg->chnode );  pbeg = pbeg->data();  }

      o = ::Serialize( o, pbeg->chnode );

      return pbeg->Serialize( o );
    }
      else
    for ( auto p = begin(); p != end(); ++p )
    {
      size_t  sublen = p->GetBufLen();

      o = p->Serialize( ::Serialize( ::Serialize( o, p->chnode ), sublen ) );
    }
    return o;
  }

  template <class S>
  S*    zmap::ztree_t::FetchFrom( S* s )
  {
    word16_t  lfetch;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

    if ( (lfetch & 0x0200) != 0 )
    {
      pvalue = std::unique_ptr<ztree_t>( new zval() );

      if ( (s = pvalue->FetchFrom( ::FetchFrom( s, (char&)keyset ) )) == nullptr )
        return nullptr;
    }

    if ( (lfetch & 0x0400) != 0 )
    {
      ztree_t*  pbeg = this;

      for ( auto  size = lfetch & 0x1ff; size-- > 0; pbeg = &pbeg->back() )
      {
        byte_t  chnext;

        if ( (s = ::FetchFrom( s, (char&)chnext )) == nullptr )
          return nullptr;
        pbeg->push_back( std::move( ztree_t( chnext ) ) );
      }

      return pbeg->FetchFrom( s );
    }
      else
    {
      auto  size = lfetch & 0x1ff;

      while ( s != nullptr && size-- > 0 )
      {
        byte_t  chnext;
        size_t  sublen;

        if ( (s = ::FetchFrom( ::FetchFrom( s, (char&)chnext ), sublen )) == nullptr )
          return nullptr;
        push_back( std::move( ztree_t( chnext ) ) );
          s = back().FetchFrom( s );
      }
    }
    return s;
  }

  /*
    zmap inline implementation
  */

  template <class O>
  O*  zmap::Serialize( O* o ) const
  {
    return z_tree() != nullptr ? z_tree()->Serialize( o ) : ::Serialize( o, (char)0 );
  }

  template <class S>
  S*  zmap::FetchFrom( S*  s )
  {
    if ( z_tree() != nullptr )
      z_tree() = std::unique_ptr<ztree_t>( new ztree_t() );

    return z_tree()->FetchFrom( s );
  }

  /* zmap::iterator_base inline implementation */

  template <class value, class z_iterator>
  zmap::iterator_base<value, z_iterator>::iterator_base()
    {}

  template <class value, class z_iterator>
  zmap::iterator_base<value, z_iterator>::iterator_base( const iterator_base& it ):
      zvalue( it.zvalue ),
      zstack( it.zstack ),
      z_buff( it.z_buff )
    {}

  template <class value, class z_iterator>
  zmap::iterator_base<value, z_iterator>::iterator_base( z_iterator beg, z_iterator end )
    {
      if ( beg != end )
      {
        zstack.push_back( std::make_pair( beg, end ) );
        z_buff.push_back( beg->chnode );
        find();
      }
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator -> () const -> const value*
    {
      if ( &zvalue.second == nullptr )
        throw std::invalid_argument( "invalid call to zmap::iterator_base<value, z_iterator>::operator ->" );
      return &zvalue;
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator * () const -> const value&
    {
      if ( &zvalue.second == nullptr )
        throw std::invalid_argument( "invalid call to zmap::iterator_base<value, z_iterator>::operator ->" );
      return zvalue;
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator++() -> const iterator_base& {  return next();  }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator--() -> const iterator_base& {  return prev();  }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator++( int ) -> iterator_base
    {
      iterator_base  _lst( *this );
      return (next(), std::move( _lst ));
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator--( int ) -> iterator_base
    {
      iterator_base  _lst( *this );
      return (prev(), std::move( _lst ));
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator==( const iterator_base& it ) const -> bool
    {
      return zstack == it.zstack;
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::init() -> iterator_base&
    {
      zvalue.~value();

      if ( zstack.size() != 0 )
        new( &zvalue )  value( key( last().first->keyset, z_buff ), last().first->pvalue.get() );
      else
        new( &zvalue )  value();

      return *this;
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::find() -> iterator_base&
    {
      assert( zstack.size() != 0 );

      while ( last().first->pvalue == nullptr && last().first->size() != 0 )
        down( last().first );

      assert( last().first->pvalue != nullptr );

      return init();
    }

  /*
    zmap::iterator_base::next()

    Смещает итератор на следующую заполненную пару ключ-значение по ztree_t - дереву.
  */
  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::next() -> iterator_base&
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

          if ( last().first->pvalue != nullptr )  return init();
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
          return init();
      }
      return init();
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::prev() -> iterator_base&
    {
      return *this;
    }

  template <class value, class z_iterator>
  void  zmap::iterator_base<value, z_iterator>::down( z_iterator it )
    {
      auto  beg = it->begin();
      auto  end = it->end();

      assert( beg != end );

      zstack.push_back( std::make_pair( beg, end ) );
      z_buff.push_back( beg->chnode );
    }

  template <class value, class z_iterator>
  void  zmap::iterator_base<value, z_iterator>::back()
    {
      zstack.pop_back();
      z_buff.pop_back();
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::last() -> zpos& {  return zstack.back();  }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::last() const -> const zpos& {  return zstack.back();  }

}

# if 0

  inline  bool  z_is_integer_type( const unsigned zt )  {  return zt >= z_char && zt <= z_word64;   }
  inline  bool  z_is_float_type( const unsigned zt )    {  return zt == z_float || zt == z_double;  }
  inline  bool  z_is_string_type( const unsigned zt )   {  return zt == z_charstr || zt == z_widestr;   }

  class zarray
  {
    struct ztree;

  public:     /*  integer key to string and reverse value conversion  */
    static  size_t    int_to_key( byte_t* out, unsigned key );
    static  unsigned  key_to_int( const char* src, size_t len );

  private:    // internal classes
  /*
    zval - поддерка синтаксиса работы с элементами zarray как с массивом, с адресацией элементов по псевдоключам.

    z[key]
  */
    class  zval
    {
      friend class zarray;

      struct owner_ref
      {
        virtual  ~owner_ref() = default;

        virtual   char_t*     set_char  ( char_t*   = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   byte_t*     set_byte  ( byte_t*   = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   int16_t*    set_int16 ( int16_t*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   int32_t*    set_int32 ( int32_t*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   int64_t*    set_int64 ( int64_t*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   word16_t*   set_word16( word16_t* = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   word32_t*   set_word32( word32_t* = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   word64_t*   set_word64( word64_t* = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   float_t*    set_float ( float_t*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   double_t*   set_double( double_t* = nullptr ) {  throw std::runtime_error( "not implemented" );  }

        virtual   char*       set_charstr( const char*     = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   widechar*   set_widestr( const widechar* = nullptr ) {  throw std::runtime_error( "not implemented" );  }

        virtual   zarray<M>*  set_zarray( zarray<M>* = nullptr )  {  throw std::runtime_error( "not implemented" );  }

        virtual   array<char_t, M>*   set_array_char  ( array<char_t, M>*   = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<byte_t, M>*   set_array_byte  ( array<byte_t, M>*   = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<int16_t, M>*  set_array_int16 ( array<int16_t, M>*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<int32_t, M>*  set_array_int32 ( array<int32_t, M>*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<int64_t, M>*  set_array_int64 ( array<int64_t, M>*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<word16_t, M>* set_array_word16( array<word16_t, M>* = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<word32_t, M>* set_array_word32( array<word32_t, M>* = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<word64_t, M>* set_array_word64( array<word64_t, M>* = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<float_t, M>*  set_array_float ( array<float_t, M>*  = nullptr ) {  throw std::runtime_error( "not implemented" );  }
        virtual   array<double_t, M>* set_array_double( array<double_t, M>* = nullptr ) {  throw std::runtime_error( "not implemented" );  }

        virtual   array<_auto_<char, M>, M>*     set_array_charstr( array<_auto_<char, M>, M>*     = nullptr )  {  throw std::runtime_error( "not implemented" );  }
        virtual   array<_auto_<widechar, M>, M>* set_array_widestr( array<_auto_<widechar, M>, M>* = nullptr )  {  throw std::runtime_error( "not implemented" );  }
      };

      template <class K>  class root_ref: public owner_ref
      {
        zarray<M>&  zar;
        K           key;

      public:
        root_ref( zarray<M>& r, K k ): zar( r ), key( k ) {}

        virtual   char_t*     set_char  ( char_t*   = nullptr ) override;
        virtual   byte_t*     set_byte  ( byte_t*   = nullptr ) override;
        virtual   int16_t*    set_int16 ( int16_t*  = nullptr ) override;
        virtual   int32_t*    set_int32 ( int32_t*  = nullptr ) override;
        virtual   int64_t*    set_int64 ( int64_t*  = nullptr ) override;
        virtual   word16_t*   set_word16( word16_t* = nullptr ) override;
        virtual   word32_t*   set_word32( word32_t* = nullptr ) override;
        virtual   word64_t*   set_word64( word64_t* = nullptr ) override;
        virtual   float_t*    set_float ( float_t*  = nullptr ) override;
        virtual   double_t*   set_double( double_t* = nullptr ) override;

        virtual   char*       set_charstr( const char*     = nullptr ) override;
        virtual   widechar*   set_widestr( const widechar* = nullptr ) override;

        virtual   zarray<M>*  set_zarray( zarray<M>* = nullptr )  override;

        virtual   array<char_t, M>*   set_array_char  ( array<char_t, M>*   = nullptr ) override;
        virtual   array<byte_t, M>*   set_array_byte  ( array<byte_t, M>*   = nullptr ) override;
        virtual   array<int16_t, M>*  set_array_int16 ( array<int16_t, M>*  = nullptr ) override;
        virtual   array<int32_t, M>*  set_array_int32 ( array<int32_t, M>*  = nullptr ) override;
        virtual   array<int64_t, M>*  set_array_int64 ( array<int64_t, M>*  = nullptr ) override;
        virtual   array<word16_t, M>* set_array_word16( array<word16_t, M>* = nullptr ) override;
        virtual   array<word32_t, M>* set_array_word32( array<word32_t, M>* = nullptr ) override;
        virtual   array<word64_t, M>* set_array_word64( array<word64_t, M>* = nullptr ) override;
        virtual   array<float_t, M>*  set_array_float ( array<float_t, M>*  = nullptr ) override;
        virtual   array<double_t, M>* set_array_double( array<double_t, M>* = nullptr ) override;

        virtual   array<_auto_<char, M>, M>*     set_array_charstr( array<_auto_<char, M>, M>*     = nullptr )  override;
        virtual   array<_auto_<widechar, M>, M>* set_array_widestr( array<_auto_<widechar, M>, M>* = nullptr )  override;
      };

      template <class K>  class next_ref: public owner_ref
      {
        owner_ref&  owr;
        K           key;

      public:
        next_ref( owner_ref& r, K k ): owr( r ), key( k ) {}

        virtual   word32_t*   set_word32( word32_t* = nullptr ) override;
      };

      char  place[0x20];

    private:
      auto  inner() const -> const owner_ref&;
      auto  inner()       ->       owner_ref&;

    // construction
      template <class K>  zval( zarray<M>&, K );
      template <class K>  zval( owner_ref&, K );

    private:
      template <class T>
      array<T, M>*  set_array( array<T, M>* p, const std::initializer_list<T>& l )
        {
          if ( p != nullptr )
            for ( auto& t: l )  p->Append( t );
          return p;
        }
      template <class C>
      array<_auto_<C, M>, M>*  set_array( array<_auto_<C, M>, M>* p, const std::initializer_list<const C*>& l )
        {
          if ( p != nullptr )
            for ( auto& t: l )  p->Append( w_strdup( t ) );
          return p;
        }

    public:     // direct setting values
      char_t*     set_char  ( char_t = 0 );
      byte_t*     set_byte  ( byte_t = 0 );
      int16_t*    set_int16 ( int16_t = 0 );
      int32_t*    set_int32 ( int32_t = 0 );
      int64_t*    set_int64 ( int64_t = 0 );
      word16_t*   set_word16( word16_t = 0 );
      word32_t*   set_word32( word32_t = 0 );
      word64_t*   set_word64( word64_t = 0 );
      float_t*    set_float ( float_t = 0 );
      double_t*   set_double( double_t = 0 );

      char*       set_charstr( const char* = nullptr );
      widechar*   set_widestr( const widechar* = nullptr );

      array<char_t,  M>*  set_array_char  ();
      array<byte_t,  M>*  set_array_byte  ();
      array<int16_t, M>*  set_array_int16 ();
      array<int32_t, M>*  set_array_int32 ();
      array<int64_t, M>*  set_array_int64 ();
      array<word16_t,M>*  set_array_word16();
      array<word32_t,M>*  set_array_word32();
      array<word64_t,M>*  set_array_word64();
      array<float_t, M>*  set_array_float ();
      array<double_t,M>*  set_array_double();

      array<char_t,  M>*  set_array_char  ( const std::initializer_list<char_t>& );
      array<byte_t,  M>*  set_array_byte  ( const std::initializer_list<byte_t>& );
      array<int16_t, M>*  set_array_int16 ( const std::initializer_list<int16_t>& );
      array<int32_t, M>*  set_array_int32 ( const std::initializer_list<int32_t>& );
      array<int64_t, M>*  set_array_int64 ( const std::initializer_list<int64_t>& );
      array<word16_t,M>*  set_array_word16( const std::initializer_list<word16_t>& );
      array<word32_t,M>*  set_array_word32( const std::initializer_list<word32_t>& );
      array<word64_t,M>*  set_array_word64( const std::initializer_list<word64_t>& );
      array<float_t, M>*  set_array_float ( const std::initializer_list<float>& );
      array<double_t,M>*  set_array_double( const std::initializer_list<double>& );

      array<_auto_<char, M>, M>*      set_array_charstr();
      array<_auto_<widechar, M>, M>*  set_array_widestr();

      array<_auto_<char, M>, M>*      set_array_charstr( const std::initializer_list<const char*>& );
      array<_auto_<widechar, M>, M>*  set_array_widestr( const std::initializer_list<const widechar*>& );

    public:     // assignment
      zval& operator = ( char_t c ) {  return (set_char( c ), *this);  }
      zval& operator = ( byte_t b ) {  return (set_byte( b ), *this);  }
      zval& operator = ( int16_t i ) {  return (set_int16( i ), *this);  }
      zval& operator = ( int32_t i ) {  return (set_int32( i ), *this);  }
      zval& operator = ( int64_t i ) {  return (set_int64( i ), *this);  }
      zval& operator = ( word16_t w ) {  return (set_word16( w ), *this);  }
      zval& operator = ( word32_t w ) {  return (set_word32( w ), *this);  }
      zval& operator = ( word64_t w ) {  return (set_word64( w ), *this);  }
      zval& operator = ( float_t f ) {  return (set_float( f ), *this);  }
      zval& operator = ( double_t d ) {  return (set_double( d ), *this);  }
      zval& operator = ( const char* s )  {  return (set_charstr( s ), *this);  }
      zval& operator = ( const widechar* w )  {  return (set_widestr( w ), *this);  }

    public:     // assignment
      zval& operator = ( const std::initializer_list<char_t>& l ) {  return (set_array_char( l ), *this);  }
      zval& operator = ( const std::initializer_list<byte_t>& l ) {  return (set_array_byte( l ), *this);  }
      zval& operator = ( const std::initializer_list<int16_t>& l ) {  return (set_array_int16( l ), *this);  }
      zval& operator = ( const std::initializer_list<int32_t>& l ) {  return (set_array_int32( l ), *this);  }
      zval& operator = ( const std::initializer_list<int64_t>& l ) {  return (set_array_int64( l ), *this);  }
      zval& operator = ( const std::initializer_list<word16_t>& l ) {  return (set_array_word16( l ), *this);  }
      zval& operator = ( const std::initializer_list<word32_t>& l ) {  return (set_array_word32( l ), *this);  }
      zval& operator = ( const std::initializer_list<word64_t>& l ) {  return (set_array_word64( l ), *this);  }
      zval& operator = ( const std::initializer_list<float>& l ) {  return (set_array_float( l ), *this);  }
      zval& operator = ( const std::initializer_list<double>& l ) {  return (set_array_double( l ), *this);  }
      zval& operator = ( const std::initializer_list<const char*>& l ) {  return (set_array_charstr( l ), *this);  }
      zval& operator = ( const std::initializer_list<const widechar*>& l ) {  return (set_array_widestr( l ), *this);  }

    public:     // index access
      zval  operator []( unsigned key ) {  return std::move( zval( inner(), key ) );  }
      zval  operator []( const char* key ) {  return std::move( zval( inner(), key ) );  }
      zval  operator []( const widechar* key ) {  return std::move( zval( inner(), key ) );  }
    };

  public:     // high-level API
    zval  operator []( unsigned key ) {  return std::move( zval( *this, key ) );  }
    zval  operator []( const char* key ) {  return std::move( zval( *this, key ) );  }
    zval  operator []( const widechar* key ) {  return std::move( zval( *this, key ) );  }

  public:     // enumeration support

    class zkey
    {
      friend class  zarray;
      friend struct zarray::ztree;

      const void* key;
      size_t      len;
      unsigned    set;

    public:     // constructors
      zkey(): key( nullptr ), len( 0 ), set( -1 )  {}
      zkey( const zkey& z ): key( z.key ), len( z.len ), set( z.set ) {}

    private:    // real initialization constructor
      zkey( const byte_t* k, size_t l, unsigned t ): key( k ), len( l ), set( t ) {}

    public:     // key access
      constexpr const uint8_t*  getkey() const  {  return (const uint8_t*)key;  }
      constexpr size_t          getlen() const  {  return len;  }
      constexpr unsigned        keyset() const  {  return set;  }

    public:     // types
      operator unsigned() const
        {  return set == 0 ? key_to_int( (const char*)key, len ) : 0;  }
      operator const char*() const
        {  return set == 1 ? (const char*)key: nullptr;  }
      operator const widechar*() const
        {  return set == 2 ? (const widechar*)key : nullptr;  }
    };

  protected:  // key wrappers
    class uintkey
    {
      uint8_t thekey[4];
      size_t  keylen;

    public:     // construction && api
      uintkey( unsigned key ): keylen( int_to_key( thekey, key ) )  {}

      constexpr const uint8_t*  getkey() const  {  return thekey;  }
      constexpr size_t          getlen() const  {  return keylen;  }
      constexpr unsigned        keyset() const  {  return 0;       }
    };

    class cstrkey
    {
      const char* strkey;
      size_t      keylen;

    public:     // construction && api
      cstrkey( const char* str ): strkey( str ), keylen( w_strlen( str ) )  {}

      constexpr const uint8_t*  getkey() const  {  return (const uint8_t*)strkey;  }
      constexpr size_t          getlen() const  {  return keylen;  }
      constexpr unsigned        keyset() const  {  return 1;       }
    };

    class wstrkey
    {
      const widechar* wcskey;
      size_t          keylen;

    public:     // construction && api
      wstrkey( const widechar* wcs ): wcskey( wcs ), keylen( sizeof(widechar) * w_strlen( wcs ) )  {}

      constexpr const uint8_t*  getkey() const  {  return (const uint8_t*)wcskey;  }
      constexpr size_t          getlen() const  {  return keylen;  }
      constexpr unsigned        keyset() const  {  return 2;       }
    };

    static  auto  make_key( unsigned key )  {  return uintkey( key );  }
    static  auto  make_key( const char* key )  {  return cstrkey( key );  }
    static  auto  make_key( const widechar* key )  {  return wstrkey( key );  }

  private:    // internal structures
      public:     // enumeration
        template <class A>  int     Enumerate( array<byte_t, M>&, int, A );
        template <class A>  int     Enumerate( A a );

    };

  protected:  // helpers
    template <class Z>
    bool    has_keyval( const zkey& k, const xvalue<Z>& v ) const
      {
        auto  pvalue = get_xvalue( k );

        return pvalue != nullptr && *pvalue == v;
      }
    static
    byte_t* inc_assign( byte_t* p, byte_t c )
      {
        *p++ = c;  return p;
      }

  public:     // enumeration helpers
    template <class action>
    int       Enumerate( action _do_ )
      {  return zhandler != nullptr ? zhandler->Enumerate( _do_ ) : 0;  }
    template <class action>
    int       Enumerate( action _do_ ) const
      {  return zhandler != nullptr ? zhandler->Enumerate( _do_ ) : 0;  }
    template <class action>
    int       for_each( action _do_ )
      {  return Enumerate( _do_ );  }
    template <class action>
    int       for_each( action _do_ ) const
      {  return Enumerate( _do_ );  }

    int       GetNextKey(       void*     keybuf, int cchkey, int buflen ) const;
    unsigned  GetKeyType( const void*     ptrkey, int cchkey ) const;

    int       size() const  {  return zhandler != nullptr ? zhandler->nitems : 0;  }
    int       haskeys() const  {  return zhandler != nullptr && zhandler->size() > 0;  }

    unsigned  TypeOfData( unsigned key ) const  {  return get_type( make_key( key ) );  }
    unsigned  TypeOfData( const char* key ) const  {  return get_type( make_key( key ) );  }
    unsigned  TypeOfData( const widechar* key ) const  {  return get_type( make_key( key ) );  }

  public:     // stringize
    std::string to_string() const
      {
        return "{}";
      }

  public:     // serialization
                        size_t  GetBufLen(            ) const;
    template <class O>  O*      Serialize( O*  output ) const;
    template <class S>  S*      FetchFrom( S*  source );

  /*
    serial_get_*() methods

    static methods allowing direct access to the field of serialized zarray<> structure

    special fast implementation for 'const char*' source also provided
  */
  protected:  // serial_* helpers
    template <class S>          static  S*  serial_jump_length( S*, unsigned );
    template <class S>          static  S*  serial_skip_xvalue( S* );
    template <class S>          static  S*  serial_skip_zarray( S* );
    template <class T, class S> static  S*  serial_skip_array_values( S* );
    template <class T, class S> static  S*  serial_skip_array_string( S* );
    template <class S>          static  S*  serial_skip_array_zarray( S* );
    template <class S>          static  S*  serial_skip_array_xvalue( S* );
    template <class S>          static  S*  serial_get_untyped( S*, const byte_t*, int, char );

    static  const char* serial_jump_length( const char*, unsigned );
    static  const char* serial_skip_xvalue( const char* );
    static  const char* serial_skip_zarray( const char* );
    static  const char* serial_get_untyped( const char*, const byte_t*, int, char );

  public:     // serial_get_* methods
    template <class S>  static  S*  serial_get_untyped( S*, unsigned );
    template <class S>  static  S*  serial_get_untyped( S*, const char* );
    template <class S>  static  S*  serial_get_untyped( S*, const widechar* );

    template <class S, class Z, class K>
    static  int serial_get_xvalue( xvalue<Z>&, S*, K );

  # define derive_get_plain( _type_ ) template <class S, class K>                             \
    static  _type_##_t  serial_get_##_type_( S* s, K k, _type_##_t d = 0 )                    \
    {                                                                                         \
      char        t;                                                                          \
      _type_##_t  v;                                                                          \
      if ( (s = ::FetchFrom( serial_get_untyped( s, k ), t )) == nullptr || t != z_##_type_ ) \
        return d;                                                                             \
      return ::FetchFrom( s, &v, sizeof(v) ) != nullptr ? v : d;                              \
    }
  # define derive_get_smart( _type_ ) template <class S, class K>                             \
    static  _type_##_t  serial_get_##_type_( S* s, K k, _type_##_t d = 0 )                    \
    {                                                                                         \
      char        b;                                                                          \
      _type_##_t  t;                                                                          \
      if ( (s = ::FetchFrom( serial_get_untyped( s, k ), b )) == nullptr || b != z_##_type_ ) \
        return d;                                                                             \
      return ::FetchFrom( s, t ) != nullptr ? t : d;                                          \
    }
    derive_get_plain( char )
    derive_get_plain( byte )
    derive_get_plain( int16 )
    derive_get_plain( word16 )
    derive_get_plain( float )
    derive_get_plain( double )
    derive_get_smart( int32 )
    derive_get_smart( word32 )
    derive_get_smart( int64 )
    derive_get_smart( word64 )

  # undef derive_get_plain
  # undef derive_get_smart

  template <class S, class K>
  static  int   serial_get_string( char* o, unsigned l, S* s, K k )
  {
    unsigned  u;
    byte_t    t;

    if ( (s = ::FetchFrom( serial_get_untyped( s, k ), t )) == nullptr || t != z_charstr )
      return EINVAL;
    if ( (s = ::FetchFrom( s, u )) == nullptr || u >= l )
      return EFAULT;
    if ( (s = ::FetchFrom( s, o, u )) != nullptr )  o[u] = 0;
      else return EFAULT;
    return 0;
  }

  protected:  // variables
    zdata*  zhandler;

  };

}

// serialization helpers

namespace mtc
{
  // xvalue inline implementation

  // zarray::ztree implementation

  template <class M>  template <class A>
  inline  int zarray<M>::ztree::Enumerate( array<byte_t, M>& b, int l, A a )
  {
    int   nerror;

  // check if element has the data attached; create the key representation and call
  // enumeration processing function
    if ( avalue.gettype() != 0xff && keyset != (unsigned)-1 )
      if ( (nerror = a( zkey( b.begin(), l, keyset ), avalue )) != 0 )
        return nerror;

  // lookup other elements
    for ( auto p = this->begin(); p < this->end(); ++p )
    {
      if ( l + 1 + sizeof(widechar) >= (unsigned)b.size() )
        if ( b.SetLen( l + 0x100 ) != 0 )
          return ENOMEM;

      b[l + 0] = p->chnode;
      b[l + 1] = 0;
      b[l + 2] = 0;

      if ( (nerror = p->Enumerate( b, l + 1, a )) != 0 )
        return nerror;
    }
    return 0;
  }

  template <class M>  template <class A>
  inline  int zarray<M>::ztree::Enumerate( A a )
  {
    array<byte_t, M> keybuf;

    return Enumerate( keybuf, 0, a );
  }

// zarray implementation

  template <class M>
  zarray<M> zarray<M>::copy() const
  {
    zarray<M> newone( GetAllocator() );

    for_each( [&]( const zkey& k, const xvalue<M>& v )
      {
        xvalue<M>*  p;

        if ( (p = newone.put_xval( k )) == nullptr )
          return ENOMEM;
        *p = v.copy();
          return 0;
      } );

    return newone;
  }

  template <class M> template <class Z>
  inline  bool  zarray<M>::operator == ( const zarray<Z>& z ) const
  {
    if ( size() != z.size() )
      return false;

    return   for_each( [&]( const zkey& k, const xvalue<M>& v ){  return z.has_keyval( k, v ) ? 0 : 1;  } ) == 0
        && z.for_each( [&]( const zkey& k, const xvalue<Z>& v ){  return   has_keyval( k, v ) ? 0 : 1;  } ) == 0;
  }

  //
  // zval представляет способ модификации zarray
  //
  template <class M> template <class K>
  zarray<M>::zval::zval( zarray<M>& r, K k )
    {  new( place )  root_ref<K>( r, k );  }

  template <class M> template <class K>
  zarray<M>::zval::zval( owner_ref& r, K k )
    {  new( place )  next_ref<K>( r, k );  }

  # define derive_root_ref_set( _type_ )                                    \
  template <class M> template <class K>                                   \
    _type_##_t* zarray<M>::zval::root_ref<K>::set_##_type_( _type_##_t* t ) \
      {  return t != nullptr ? zar.set_##_type_( key, *t ) : zar.get_##_type_( key ); }

    derive_root_ref_set( char )
    derive_root_ref_set( byte )
    derive_root_ref_set( int16 )
    derive_root_ref_set( int32 )
    derive_root_ref_set( int64 )
    derive_root_ref_set( word16 )
    derive_root_ref_set( word32 )
    derive_root_ref_set( word64 )
    derive_root_ref_set( float )
    derive_root_ref_set( double )
  # undef derive_root_ref_set

  template <class M> template <class K>
  char* zarray<M>::zval::root_ref<K>::set_charstr( const char* s )
    {  return s != nullptr ? zar.set_charstr( key, s ) : zar.get_charstr( key );  }

  template <class M> template <class K>
  widechar* zarray<M>::zval::root_ref<K>::set_widestr( const widechar* s )
    {  return s != nullptr ? zar.set_widestr( key, s ) : zar.get_widestr( key );  }

  template <class M> template <class K>
  zarray<M>* zarray<M>::zval::root_ref<K>::set_zarray( zarray<M>* z )
    {  return z != nullptr ? zar.set_zarray( key, *z ) : zar.get_zarray( key );  }

  # define derive_root_ref_set_array( _type_ )                                  \
  template <class M> template <class K>                                         \
    array<_type_##_t, M>* zarray<M>::zval::root_ref<K>::set_array_##_type_( array<_type_##_t, M>* p ) \
      {                                                                                               \
        if ( p != nullptr )                                                                           \
        {                                                                                             \
          auto  o = zar.set_array_##_type_( key );                                                    \
            *o = std::move( *p );                                                                     \
          return o;                                                                                   \
        }                                                                                             \
        return zar.get_array_##_type_( key );                                                         \
      }

    derive_root_ref_set_array( char )
    derive_root_ref_set_array( byte )
    derive_root_ref_set_array( int16 )
    derive_root_ref_set_array( int32 )
    derive_root_ref_set_array( int64 )
    derive_root_ref_set_array( word16 )
    derive_root_ref_set_array( word32 )
    derive_root_ref_set_array( word64 )
    derive_root_ref_set_array( float )
    derive_root_ref_set_array( double )
  # undef derive_root_ref_set_array

  template <class M> template <class K>
  array<_auto_<char, M>, M>* zarray<M>::zval::root_ref<K>::set_array_charstr()  {  return zar.set_array_charstr( key );  }

  template <class M> template <class K>
  array<_auto_<widechar, M>, M>* zarray<M>::zval::root_ref<K>::set_array_widestr()  {  return zar.set_array_widestr( key );  }

  template <class M> template<class K>
  word32_t* zarray<M>::zval::next_ref<K>::set_word32( word32_t w )
    {
      zarray<M>*          pz = owr.get_zarray();
      array<word32_t, M>* pa = owr.get_array_word32();

      assert( pz == nullptr || pa == nullptr );

      if ( pa != nullptr )
      {
        array<word32_t, M>  ar( std::move( *pa ) );

        if ( (pz = owr.set_zarray()) != nullptr )
          for ( int i = 0; i != ar.size(); ++i )
            pz->set_word32( i, ar[i] );
      }
      if ( pz == nullptr )
        pz = owr.set_zarray();

      return pz != nullptr ? pz->set_word32( key, w ) : nullptr;
    }

  template <class M>
  auto  zarray<M>::zval::inner() const -> const typename zarray<M>::zval::owner_ref&  {  return *(const owner_ref*)place;  }

  template <class M>
  auto  zarray<M>::zval::inner() -> typename zarray<M>::zval::owner_ref&  {  return *(owner_ref*)place;  }

  # define derive_set( _type_ )  \
    template <class M>              \
    _type_##_t* zarray<M>::zval::set_##_type_( _type_##_t t )  {  return inner().set_##_type_( t );  }

    derive_set( char )
    derive_set( byte )
    derive_set( int16 )
    derive_set( int32 )
    derive_set( int64 )
    derive_set( word16 )
    derive_set( word32 )
    derive_set( word64 )
    derive_set( float )
    derive_set( double )
  # undef derive_assign

  template <class M>
  char* zarray<M>::zval::set_charstr( const char* s ) {  return inner().set_charstr( s );  }

  template <class M>
  widechar* zarray<M>::zval::set_widestr( const widechar* w ) {  return inner().set_widestr( w );  }

  # define derive_assign_array( _type_ )  \
  template <class M> array<_type_##_t, M>* zarray<M>::zval::set_array_##_type_()            \
    {  return inner().set_array_##_type_();  }                                              \
  template <class M> array<_type_##_t, M>* zarray<M>::zval::set_array_##_type_( const std::initializer_list<_type_##_t>& set )  \
    {  return set_array( set_array_##_type_(), set );  }

    derive_assign_array( char )
    derive_assign_array( byte )
    derive_assign_array( int16 )
    derive_assign_array( int32 )
    derive_assign_array( int64 )
    derive_assign_array( word16 )
    derive_assign_array( word32 )
    derive_assign_array( word64 )
    derive_assign_array( float )
    derive_assign_array( double )
  # undef derive_assign_array

  template <class M>
  array<_auto_<char, M>, M>* zarray<M>::zval::set_array_charstr() {  return inner().set_array_charstr();  }

  template <class M>
  array<_auto_<widechar, M>, M>* zarray<M>::zval::set_array_widestr() {  return inner().set_array_widestr();  }

  template <class M>
  array<_auto_<char, M>, M>* zarray<M>::zval::set_array_charstr( const std::initializer_list<const char*>& set )
    {  return set_array( set_array_charstr(), set );  }

  template <class M>
  array<_auto_<widechar, M>, M>* zarray<M>::zval::set_array_widestr( const std::initializer_list<const widechar*>& set )
    {  return set_array( set_array_widestr(), set );  }

  /* serial_* methods section for direct access to fields   */  

  template <class M> template <class S>
  S*  zarray<M>::serial_jump_length( S* s, unsigned l )
  {
    char      slocal[0x100];
    unsigned  cbpart;

    while ( (cbpart = l < sizeof(slocal) ? l : sizeof(slocal)) > 0 )
      if ( (s = ::FetchFrom( s, slocal, cbpart )) != nullptr )  l -= cbpart;
        else return nullptr;
    return s;
  }

  template <class M> template <class S>         
  S*  zarray<M>::serial_skip_xvalue( S* s )
  {
    char      vatype;
    unsigned  sublen;

    if ( (s = ::FetchFrom( s, vatype )) == nullptr )
      return nullptr;
    switch ( vatype )
    {
    # define  derive_skip_plain( _type_ ) case z_##_type_: return serial_jump_length( s, sizeof(_type_##_t) );
      derive_skip_plain( char )
      derive_skip_plain( byte )
      derive_skip_plain( int16 )
      derive_skip_plain( word16 )
      derive_skip_plain( float )
      derive_skip_plain( double )
    # undef derive_skip_plain

    # define  derive_skip_smart( _type_ ) case z_##_type_: {  _type_##_t  t;  return ::FetchFrom( s, t ); }
      derive_skip_smart( int32 )
      derive_skip_smart( int64 )
      derive_skip_smart( word32 )
      derive_skip_smart( word64 )
    # undef derive_skip_smart

      case z_charstr:
        return (s = ::FetchFrom( s, sublen )) != nullptr ? serial_jump_length( s, sublen ) : nullptr;
      case z_widestr:
        return (s = ::FetchFrom( s, sublen )) != nullptr ? serial_jump_length( s, sizeof(widechar) * sublen ) : nullptr;
//        case z_buffer  = 18,
      case z_zarray:
        return serial_skip_zarray( s );

    # define  derive_skip_array_plain( _type_ ) case z_array_##_type_:  \
        return (s = ::FetchFrom( s, sublen )) != nullptr ? serial_jump_length( s, sublen * sizeof(_type_##_t) ) : nullptr;

      derive_skip_array_plain( char )
      derive_skip_array_plain( byte )
      derive_skip_array_plain( float )
      derive_skip_array_plain( double )
    # undef derive_skip_array_plain

      case z_array_int16:   return serial_skip_array_values<int16_t>( s );
      case z_array_int32:   return serial_skip_array_values<int32_t>( s );
      case z_array_int64:   return serial_skip_array_values<int64_t>( s );
      case z_array_word16:  return serial_skip_array_values<word16_t>( s );
      case z_array_word32:  return serial_skip_array_values<word32_t>( s );
      case z_array_word64:  return serial_skip_array_values<word64_t>( s );

      case z_array_charstr: return serial_skip_array_string<char>( s );
      case z_array_widestr: return serial_skip_array_string<widechar>( s );
//        case z_array_buffer  = 50,
      case z_array_zarray:  return serial_skip_array_zarray( s );
      case z_array_xvalue:  return serial_skip_array_xvalue( s );
      default:
        return nullptr;
    }
  }

  template <class M> template <class S>
  S*  zarray<M>::serial_skip_zarray( S* s )
  {
    word16_t  lfetch;
    int       arrlen;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
    {
      char  keyset;

      if ( (s = serial_skip_xvalue( ::FetchFrom( s, keyset ))) == nullptr )
        return nullptr;
    }

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
      return (s = serial_jump_length( s, lfetch & 0x1ff )) != nullptr ? serial_skip_zarray( s ) : nullptr;

    for ( arrlen = lfetch & 0x1ff; arrlen-- > 0; )
    {
      unsigned  sublen;
      byte_t    chnext;

      if ( (s = ::FetchFrom( ::FetchFrom( s, (char&)chnext ), sublen )) == nullptr )
        return nullptr;
      if ( (s = serial_jump_length( s, sublen )) == nullptr )
        return nullptr;
    }
    return s;
  }

  template <class M> template <class T, class S>
  S*  zarray<M>::serial_skip_array_values( S* s )
  {
    int   nitems;
    T     itnext;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itnext )) != nullptr )
      (void)0;
    return s;
  }

  template <class M> template <class T, class S>
  S*  zarray<M>::serial_skip_array_string( S* s )
  {
    int       nitems;
    unsigned  itelen;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itelen )) != nullptr && (s = serial_jump_length( s, sizeof(T) * itelen )) != nullptr )
      (void)0;
    return s;
  }

  template <class M> template <class S>
  S*  zarray<M>::serial_skip_array_zarray( S* s )
  {
    int       nitems;
    unsigned  itelen;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itelen )) != nullptr && (s = serial_skip_zarray( s )) != nullptr )
      (void)0;
    return s;
  }

  template <class M> template <class S>
  S*  zarray<M>::serial_skip_array_xvalue( S* s )
  {
    int       nitems;
    unsigned  itelen;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itelen )) != nullptr && (s = serial_skip_xvalue( s )) != nullptr )
      (void)0;
    return s;
  }

  template <class M> template <class S>
  S*  zarray<M>::serial_get_untyped( S* s, const byte_t* k, int l, char t )
  {
    word16_t  lfetch;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
    {
      char  keyset;

      if ( (s = ::FetchFrom( s, keyset )) == nullptr )
        return nullptr;
      if ( l == 0 ) return keyset == t ? s : nullptr;
        else s = skip_xvalue( s );
    }
      else
    if ( l == 0 )
      return nullptr;

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
    {
      int     patlen = lfetch & 0x1ff;
      byte_t  chload;

      while ( patlen-- > 0 && l > 0 )
      {
        if ( (s = ::FetchFrom( s, (char&)chload )) != nullptr && chload == *k++ ) --l;
          else return nullptr;
      }

      return patlen < 0 ? serial_get_untyped( s, k, l, t ) : nullptr;
    }
      else
    {
      int     arrlen = lfetch & 0x1ff;
      byte_t  chfind = *k++;

      while ( arrlen-- > 0 )
      {
        unsigned  sublen;
        byte_t    chnext;

        if ( (s = ::FetchFrom( ::FetchFrom( s, (char&)chnext ), sublen )) == nullptr )
          return nullptr;

        if ( chfind == chnext ) return serial_get_untyped( s, k, l - 1, t );
          else
        if ( chfind <  chnext ) return nullptr;
          else
        if ( (s = skip_buffer( s, sublen )) == nullptr )
          return nullptr;
      }
      return nullptr;
    }
  }

  template <class M>
  const char* zarray<M>::serial_jump_length( const char* s, unsigned l )
  {
    return s + l;
  }

  template <class M>
  const char* zarray<M>::serial_skip_xvalue( const char* s )
  {
    char      vatype = *s++;
    unsigned  sublen;

    switch ( vatype )
    {
      case z_char:    return s + sizeof(char);
      case z_byte:    return s + sizeof(byte_t);
      case z_int16:   return s + sizeof(int16_t);
      case z_word16:  return s + sizeof(word16_t);
      case z_float:   return s + sizeof(float);
      case z_double:  return s + sizeof(double);

      case z_int32:
      case z_int64:
      case z_word32:
      case z_word64:  while ( *s++ & 0x80 ) (void)0;  return s;

      case z_charstr: return ::FetchFrom( s, sublen ) + sublen;
      case z_widestr: return ::FetchFrom( s, sublen ) + sizeof(widechar) * sublen;
//        case z_buffer  = 18,
      case z_zarray:  return serial_skip_zarray( s );

      case z_array_char:    return ::FetchFrom( s, sublen ) + sublen;
      case z_array_byte:    return ::FetchFrom( s, sublen ) + sublen;
      case z_array_float:   return ::FetchFrom( s, sublen ) + sublen * sizeof(float);
      case z_array_double:  return ::FetchFrom( s, sublen ) + sublen * sizeof(double);

      case z_array_int16:   return serial_skip_array_values<int16_t>( s );
      case z_array_int32:   return serial_skip_array_values<int32_t>( s );
      case z_array_int64:   return serial_skip_array_values<int64_t>( s );
      case z_array_word16:  return serial_skip_array_values<word16_t>( s );
      case z_array_word32:  return serial_skip_array_values<word32_t>( s );
      case z_array_word64:  return serial_skip_array_values<word64_t>( s );

      case z_array_charstr: return serial_skip_array_string<char>( s );
      case z_array_widestr: return serial_skip_array_string<widechar>( s );
//        case z_array_buffer  = 50,
      case z_array_zarray:  return serial_skip_array_zarray( s );
      case z_array_xvalue:  return serial_skip_array_xvalue( s );
      default:
        return nullptr;
    }
  }

  template <class M>
  const char* zarray<M>::serial_skip_zarray( const char* s )
  {
    word16_t  lfetch;
    unsigned  sublen;
    int       arrlen;

    if ( (lfetch = (byte_t)*s++) & 0x80 )
      lfetch = (lfetch & 0x7f) | (((word16_t)(byte_t)*s++) << 7);

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
      s = serial_skip_xvalue( ++s );

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
      return serial_skip_zarray( s + (lfetch & 0x1ff) );

    for ( arrlen = lfetch & 0x1ff; arrlen-- > 0; )
    {
      s = ::FetchFrom( ++s, sublen );
      s += sublen;
    }

    return s;
  }

  template <class M>
  const char* zarray<M>::serial_get_untyped( const char* s, const byte_t* k, int l, char t )
  {
    word16_t  lfetch;

    if ( (lfetch = (byte_t)*s++) & 0x80 )
      lfetch = (lfetch & 0x7f) | (((word16_t)(byte_t)*s++) << 7);

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
    {
      if ( l == 0 ) return *s++ == t ? s : nullptr;
        else s = serial_skip_xvalue( ++s );
    }
      else
    if ( l == 0 )
      return nullptr;

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
    {
      int     patlen = lfetch & 0x1ff;

      if ( (l -= patlen) < 0 )
        return nullptr;
      while ( patlen-- > 0 )
        if ( *s++ != *k++ ) return nullptr; 

      return serial_get_untyped( s, k, l, t );
    }
      else
    {
      int     arrlen = lfetch & 0x1ff;
      byte_t  chfind = *k++;

      while ( arrlen-- > 0 )
      {
        unsigned  sublen;
        byte_t    chnext;

        if ( (chnext = (byte_t)*s++) == chfind )
        {
          while ( (*s++ & 0x80) != 0 )
            (void)0;
          return serial_get_untyped( s, k, l - 1, t );
        }
          else
        if ( chnext > chfind )
          return nullptr;
        s = ::FetchFrom( s, sublen ) + sublen;
      }
      return nullptr;
    }
  }

  template <class M> template <class S>
  S*  zarray<M>::serial_get_untyped( S* s, unsigned k )
  {
    byte_t  thekey[4];

    return serial_get_untyped( s, thekey, int_to_key( thekey, k ), 0 );
  }

  template <class M> template <class S>
  S*  zarray<M>::serial_get_untyped( S* s, const char* k )
  {
    return serial_get_untyped( s, (const byte_t*)k, (int)w_strlen( k ), 1 );
  }

  template <class M> template <class S>
  S*  zarray<M>::serial_get_untyped( S* s, const widechar* k )
  {
    return serial_get_untyped( s, (const byte_t*)k, sizeof(widechar) * w_strlen( k ), 2 );
  }

  template <class M> template <class S, class Z, class K>
  int zarray<M>::serial_get_xvalue( xvalue<Z>& z, S* s, K k )
  {
    return (s = serial_get_untyped( s, k )) != nullptr ? z.FetchFrom( s ) != nullptr ? 0 : EFAULT : ENOENT;
  }

}

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif  // _MSC_VER
# endif // 0

# endif  // __zarray_h__
