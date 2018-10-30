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
# include "uuid.h"
# include <cassert>
# include <algorithm>
# include <vector>
# include <string>
# include <atomic>
# include <type_traits>
# include <memory>
# include <limits>
# include <mutex>

namespace mtc
{
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
  using array_uuid    = std::vector<uuid_t>;

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
  using array_uuid_t    = array_uuid;

  std::string to_string( const zval& );
  std::string to_string( const zmap& );

  namespace impl
  {
    template <class ... types>
    class get_max_size
    {
      template <class... L>
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

  public:     // z_%%% types
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

      z_uuid    = 20,

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

      z_array_uuid    = 53,

      z_untyped       = (unsigned)-1
    };

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
    zval( const uuid_t& );

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
    zval& operator = ( const uuid_t& );

    zval( const char*, size_t = (size_t)-1 );
    zval& operator = ( const char* );
    zval( const widechar*, size_t = (size_t)-1 );
    zval& operator = ( const widechar* );

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
    declare_init( array_uuid )

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
  # define  declare_access_type( _type_ )                                 \
    const _type_##_t*  get_##_type_() const;                              \
          _type_##_t*  set_##_type_( const _type_##_t& = _type_##_t() );  \
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
    declare_access_type( uuid )
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
    declare_access_array( uuid )
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

  public:     // arithmetic
    zval  operator *  ( const zval& r ) const;
    zval  operator /  ( const zval& r ) const;
    zval  operator %  ( const zval& r ) const;
    zval  operator +  ( const zval& r ) const;
    zval  operator -  ( const zval& r ) const;
    zval  operator << ( const zval& r ) const;
    zval  operator >> ( const zval& r ) const;
    zval  operator &  ( const zval& r ) const;
    zval  operator ^  ( const zval& r ) const;
    zval  operator |  ( const zval& r ) const;

    zval  operator ~  ()  const;

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

    template <class A>
    static int  CompTo( const A&, const uuid_t& ) {  return 0x08;  }
    template <class B>
    static int  CompTo( const uuid_t&, const B& ) {  return 0x08;  }

    static int  CompTo( const uuid_t& a, const uuid_t& b )
      {
        int    rc = a.compare( b );
        return rc < 0 ? 0x01 + 0x08 : rc > 0 ? 0x02 + 0x08 : 0x04;
      }

    # define  derive_noncmp( c1, c2 ) static  int   CompTo( c1, c2 )  {  return 0x08;  } \
                                      static  int   CompTo( c2, c1 )  {  return 0x08;  }
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

          case z_uuid:    return CompTo( *get_uuid(), b );

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

          case z_uuid:    return CompTo( *x.get_uuid() );

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
        (void)v;  throw std::runtime_error( "not implemented" );
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
    class zdata_t;

  public:     // iterator support
    class key;
    class iterator;
    class const_iterator;

  protected:
    template <class value>
    class iterator_data;
    template <class value, class ztree_iterator>
    class iterator_base;

  protected:
    auto  private_data() -> zdata_t*;
    auto  make_private() -> zmap&;

  public:
    zmap();
    zmap( zmap&& );
    zmap( const zmap& );
    zmap( const std::initializer_list<std::pair<key, zval>>& );
    zmap& operator = ( zmap&& );
    zmap& operator = ( const zmap& );
    zmap& operator = ( const std::initializer_list<std::pair<key, zval>>& );
   ~zmap();

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class O>  O*      Serialize( O* o ) const;
    template <class S>  S*      FetchFrom( S* );

  public:     // put family
    auto  put( const key&, zval&& )               -> zval*;
    auto  put( const key&, const zval& = zval() ) -> zval*;

  public:     // get_zval family
    auto  get( const key& ) const       -> const zval*;
    auto  get( const key& )             ->       zval*;
    auto  get_type( const key& ) const  -> decltype(zval::vx_type);

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
    declare_get_type( uuid    )

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
    declare_get_init( uuid    )

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
    declare_get_type( array_uuid    )

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
    declare_set_copy( uuid )
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
    declare_set_copy( array_uuid    )

    declare_set_pure( array_zmap    )

    declare_set_move( charstr )
    declare_set_move( widestr )
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
    declare_set_move( array_uuid    )
  # undef declare_set_pure
  # undef declare_set_move
  # undef declare_set_copy
  # undef declare_get_type

  auto  set_zmap( const key& ) -> zmap*;
  auto  set_zmap( const key&, zmap&& ) -> zmap*;
  auto  set_zmap( const key&, const zmap& ) -> zmap*;

  public:     // iterators
          iterator  begin();
          iterator  end();
    const_iterator  begin() const;
    const_iterator  end() const;
    const_iterator  cbegin() const;
    const_iterator  cend() const;

  public:     // properties
    auto  empty() const -> bool;
    auto  size() const  -> size_t;

  public:     // direct element access
    auto  at( const key& )       ->       zval&;
    auto  at( const key& ) const -> const zval&;

  public:     // c++ access
  /*
    auto  operator []( const key& )       ->       patch_place_t;
    auto  operator []( const key& ) const -> const const_place_t;
  */

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
    zdata_t*  p_data = nullptr;

  };

  inline  std::string to_string( const zval& z ) {  return std::move( z.to_string() );  }
  inline  std::string to_string( const zmap& z ) {  (void)z;  return std::move( ""/*z.to_string() */);  }
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
    derive_var( uuid )

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
    derive_var( array_uuid )
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
      derive_put_smart( uuid )
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
      derive_put_smart( array_uuid )
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
      derive_get_smart( uuid )
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
      derive_get_smart( array_uuid )
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

  protected:
    auto  copyit() const -> ztree_t;

  public:
    ztree_t( byte_t chinit = '\0' );
    ztree_t( ztree_t&& );
    ztree_t& operator = ( ztree_t&& );

    ztree_t( const ztree_t& ) = delete;
    ztree_t& operator = ( const ztree_t& ) = delete;

  protected:  // search implementation
    template <class self> static
    auto  search( self& _me, const uint8_t* key, size_t cch ) -> self*;

  public:     // unserialized tree work
    auto  insert( const uint8_t* key, size_t cch ) -> ztree_t*;
    auto  remove( const uint8_t* key, size_t cch ) -> size_t;
    auto  search( const uint8_t* key, size_t cch ) const -> const ztree_t* {  return search( *this, key, cch );  }
    auto  search( const uint8_t* key, size_t cch )       ->       ztree_t* {  return search( *this, key, cch );  }

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class O>  O*      Serialize( O* ) const;
    template <class S>  S*      FetchFrom( S*, size_t& );

  protected:  // helpers
    int   plain_branchlen() const;

  };

  class zmap::zdata_t: public ztree_t
  {
    friend class zmap;

    zdata_t( zdata_t&& ) = delete;
    zdata_t( const zdata_t& ) = delete;
    zdata_t&  operator= ( zdata_t&& ) = delete;
    zdata_t&  operator= ( const zdata_t& ) = delete;

    zdata_t( ztree_t&&, size_t );

  public:
    zdata_t();

  public:
    long  attach();
    long  detach();
    auto  copyit() -> zdata_t*;

  public:
    size_t      n_vals;

  protected:
    std::mutex  _mutex;
    long        _refer;
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
    key( const charstr& );
    key( const widestr& );
    key( const key& );
    key& operator = ( const key& );

  public:
    auto  operator == ( const key& k ) const -> bool;
    auto  operator != ( const key& k ) const -> bool {  return !(*this == k);  }

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
    template <class v, class zit>
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

  /*
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

  public:
    bool  operator == ( char     ) const;
    bool  operator == ( byte_t   ) const;
    bool  operator == ( int16_t  ) const;
    bool  operator == ( int32_t  ) const;
    bool  operator == ( int64_t  ) const;
    bool  operator == ( word16_t ) const;
    bool  operator == ( word32_t ) const;
    bool  operator == ( word64_t ) const;
    bool  operator == ( float_t  ) const;
    bool  operator == ( double_t ) const;
    bool  operator == ( const charstr& ) const;
    bool  operator == ( const widestr& ) const;

    bool  operator != ( const charstr& s ) const  {  return !(*this == s);  }
    bool  operator != ( const widestr& s ) const  {  return !(*this == s);  }

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
  */

  /*
    zmap::ztree_t inline implementation
  */

  inline
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
  S*    zmap::ztree_t::FetchFrom( S* s, size_t& n )
  {
    word16_t  lfetch;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

    if ( (lfetch & 0x0200) != 0 )
    {
      pvalue = std::unique_ptr<zval>( new zval() );

      if ( (s = pvalue->FetchFrom( ::FetchFrom( s, (char&)keyset ) )) == nullptr )
        return nullptr;
      else ++n;
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

      return pbeg->FetchFrom( s, n );
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
          s = back().FetchFrom( s, n );
      }
    }
    return s;
  }

  /*
    zmap inline implementation
  */

  inline
  size_t  zmap::GetBufLen() const
  {
    return p_data != nullptr ? p_data->GetBufLen() : 1;
  }

  template <class O>
  O*  zmap::Serialize( O* o ) const
  {
    return p_data != nullptr ? p_data->Serialize( o ) : ::Serialize( o, (char)0 );
  }

  template <class S>
  S*  zmap::FetchFrom( S*  s )
  {
    if ( p_data == nullptr )
      (p_data = new zdata_t())->attach();

    return p_data->FetchFrom( s, p_data->n_vals );
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
        down( last().first++ );

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

# include "serialize.h"

# endif  // __zmap_hpp__
