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
# include "serialize.h"
# include "wcsstr.h"
# include "uuid.h"
# include <cassert>
# include <type_traits>
# include <algorithm>
# include <vector>
# include <string>
# include <atomic>
# include <type_traits>
# include <memory>
# include <limits>
# include <mutex>
# include <cstddef>

namespace mtc
{
  class zval;
  class zmap;

  using byte   = uint8_t;
  using char_t = char;
  using byte_t = byte;

  using bool_t = bool;

# if !defined( mtc_charstr_defined )
  template <class C>
  using strbase = std::basic_string<C, std::char_traits<C>, std::allocator<C>>;

# define mtc_charstr_defined
  using charstr = strbase<char>;
# endif
# if !defined( mtc_widestr_defined )
# define mtc_widestr_defined
  using widestr = strbase<widechar>;
# endif

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

  const char* const invalid = (const char*)-1;

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

  }

  class zval
  {
    friend class zmap;

    union inner_t;

    enum values: size_t
    {
      bytes = impl::get_max_size<uint64_t, double,
        charstr,
        widestr, array_charstr>::value,
      align = alignof(array_charstr)
    };

    using storage_t = typename std::aligned_storage<values::bytes, values::align>::type;

  public:     // z_%%% types
    class dump;
    class view;

    enum z_type: byte_t
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
      z_bool    = 11,

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

      z_untyped       = (byte_t)-1
    };

  public:     // construction
    zval();
    zval( zval&& );
    zval( const zval& );
    zval& operator = ( zval&& );
    zval& operator = ( const zval& );
   ~zval();

  public:     // special constructors
    zval( bool );
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

    zval( const char*, size_t = (size_t)-1 );
    zval& operator = ( const char* );
    zval( const widechar*, size_t = (size_t)-1 );
    zval& operator = ( const widechar* );

    zval& operator = ( bool );
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


  # define declare_init_ref( _type_ )     \
    zval( _type_##_t&& );                 \
    zval( const _type_##_t& );            \
    zval& operator = ( _type_##_t&& );    \
    zval& operator = ( const _type_##_t& );

    declare_init_ref( charstr )
    declare_init_ref( widestr )
    declare_init_ref( uuid )
    declare_init_ref( zmap )
    declare_init_ref( array_char )
    declare_init_ref( array_byte )
    declare_init_ref( array_int16 )
    declare_init_ref( array_int32 )
    declare_init_ref( array_int64 )
    declare_init_ref( array_word16 )
    declare_init_ref( array_word32 )
    declare_init_ref( array_word64 )
    declare_init_ref( array_float )
    declare_init_ref( array_double )
    declare_init_ref( array_charstr )
    declare_init_ref( array_widestr )
    declare_init_ref( array_uuid )
    declare_init_ref( array_zval )
    declare_init_ref( array_zmap )

  public:     // accessors
  # define  declare_access_val( _type_ )                                  \
          _type_##_t*  get_##_type_();                                    \
    const _type_##_t*  get_##_type_() const;                              \
          _type_##_t*  set_##_type_( _type_##_t = _type_##_t() );
  # define  declare_access_ref( _type_ )                                  \
          _type_##_t*  get_##_type_();                                    \
    const _type_##_t*  get_##_type_() const;                              \
          _type_##_t*  set_##_type_( _type_##_t&& );                      \
          _type_##_t*  set_##_type_( const _type_##_t& = _type_##_t() );

    declare_access_val( char )
    declare_access_val( byte )
    declare_access_val( int16 )
    declare_access_val( int32 )
    declare_access_val( int64 )
    declare_access_val( word16 )
    declare_access_val( word32 )
    declare_access_val( word64 )
    declare_access_val( float )
    declare_access_val( double )
    declare_access_val( bool )

    declare_access_ref( charstr )
    declare_access_ref( widestr )

    auto  set_charstr( const char*, size_t = -1 ) -> charstr*;
    auto  set_widestr( const widechar*, size_t = -1 ) -> widestr*;

    declare_access_ref( uuid )

    auto  get_zmap() -> zmap*;
    auto  get_zmap() const -> const zmap*;
    auto  set_zmap() -> zmap*;
    auto  set_zmap( zmap&& ) -> zmap*;
    auto  set_zmap( const zmap& ) -> zmap*;

    declare_access_ref( array_char )
    declare_access_ref( array_byte )
    declare_access_ref( array_int16 )
    declare_access_ref( array_int32 )
    declare_access_ref( array_int64 )
    declare_access_ref( array_word16 )
    declare_access_ref( array_word32 )
    declare_access_ref( array_word64 )
    declare_access_ref( array_float )
    declare_access_ref( array_double )

    declare_access_ref( array_charstr )
    declare_access_ref( array_widestr )
    declare_access_ref( array_uuid )
    declare_access_ref( array_zval )
    declare_access_ref( array_zmap )

  # undef declare_access_ref
  # undef declare_access_val

  public:     // operations
    bool  empty() const;
    auto  clear() -> zval&;
    auto  get_type() const -> unsigned;
    auto  is_array() const -> bool;
    auto  is_numeric() const -> bool;

  public:     // conversions
    auto  cast_to_int16 ( int16_t = 0 ) const -> int16_t;
    auto  cast_to_int32 ( int32_t = 0 ) const -> int32_t;
    auto  cast_to_int64 ( int64_t = 0 ) const -> int64_t;
    auto  cast_to_word16( word16_t = 0 ) const -> word16_t;
    auto  cast_to_word32( word32_t = 0 ) const -> word32_t;
    auto  cast_to_word64( word64_t = 0 ) const -> word64_t;
    auto  cast_to_float ( float_t = 0 ) const -> float_t;
    auto  cast_to_double( double_t = 0 ) const -> double_t;
    auto  cast_to_charstr( const charstr& = {} ) const -> charstr;

  public:     // serialization
    size_t  GetBufLen(    ) const;
    template <class O>
    O*      Serialize( O* ) const;
    template <class S>
    S*      FetchFrom( S* );
    template <class S> static
    S*      SkipToEnd( S* );

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

  public:     // direct compare
    int   compare( const zval& ) const;

    bool  operator == ( const zval& v ) const {  return compare( v ) == 0;  }
    bool  operator <  ( const zval& v ) const {  return compare( v ) <  0;  }
    bool  operator != ( const zval& v ) const {  return compare( v ) != 0; }

  public:
    enum: unsigned
    {
      compare_lt = 0x01,
      compare_gt = 0x02,
      compare_eq = 0x04,
      compare_le = 0x05,
      compare_ge = 0x06
    };

  public:     // soft values compare
    auto  CompTo( const zval& x ) const -> unsigned;

    bool  lt( const zval& z ) const {  return CompTo( z ) == compare_lt;  }
    bool  gt( const zval& z ) const {  return CompTo( z ) == compare_gt;  }
    bool  eq( const zval& z ) const {  return CompTo( z ) == compare_eq;  }
    bool  le( const zval& z ) const {  return (CompTo( z ) & compare_le) != 0;  }
    bool  ge( const zval& z ) const {  return (CompTo( z ) & compare_ge) != 0;  }
    bool  ne( const zval& z ) const {  return !eq( z );  }

  protected:  // stringize helpers
    static  auto  to_string( char c )     -> std::string  {  return std::string( { '\'', c, '\'', 0 } );  }
    static  auto  to_string( byte_t v )   -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( int16_t v )  -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( int32_t v )  -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( int64_t v )  -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( uint16_t v ) -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( uint32_t v ) -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( uint64_t v ) -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( float v )    -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( double v )   -> std::string  {  return std::to_string( v );  }
    static  auto  to_string( bool v )     -> std::string  {  return v ? "true" : "false";  }

    static  auto  to_string( const widechar* v )  -> std::string
      {
        std::string out;

        while ( *v != 0 ) out += (char)*v++;

        return out;
      }

    static  auto  to_string( const zval& v ) ->  std::string {  return mtc::to_string( v );  }
    static  auto  to_string( const zmap& v ) ->  std::string {  return mtc::to_string( v );  }
    static  auto  to_string( const charstr& v ) ->  const std::string& {  return v;  }
    static  auto  to_string( const widestr& v ) ->  std::string {  return to_string( v.c_str() );  }

    template <class V>
    static  auto  to_string( const std::vector<V>& arr ) ->  std::string
      {
        std::string out( "[" );

        for ( auto& val: arr )
          out += (to_string( val )+',');

        if ( out.back() == ',' )  out.back() = ']';
          else out += ']';

        return out;
      }

  public:     // stringize
    static
    const char* type_name( z_type type );
    std::string to_string() const;

  protected:  // helpers
    auto  fetch( zval&& ) -> zval&;
    auto  fetch( const zval& ) -> zval&;

    auto  inner() const -> const inner_t&;
    auto  inner()       ->       inner_t&;

  protected:  // inplace storage
    storage_t storage;
    byte_t    vx_type;

  };

  class zmap
  {
    class ztree_t;
    class zdata_t;
    class zbuff_t;

    template <class S>  class storage;
    template <class S>  class limiter;

  public:     // iterator support
    class key;
    class iterator;
    class const_iterator;
    class serial;
    class dump;

  protected:
    template <class value>
    class iterator_data;
    template <class value, class ztree_iterator>
    class iterator_base;

  private:
    template <class obj>
    class place_t;

    class const_place_t;
    class patch_place_t;

    template <class obj>
    friend  bool  is_set( const place_t<obj>& );
    template <class obj>
    friend  bool  get_type( const place_t<obj>& );
    template <class obj>
    friend  bool  is_array( const place_t<obj>& );

  protected:
    auto  private_data() -> zdata_t*;
    static
    auto  fragment_len( word32_t u ) -> size_t {  assert( (u & 0x0400) != 0 );  return (u & 0x1ff) | ((u >> 2) & ~0x1ff);  }

  public:
    zmap() = default;
    zmap( zmap&& );
    zmap( const zmap& );
    zmap( const std::initializer_list<std::pair<key, zval>>& );
    zmap( const zmap&, const std::initializer_list<std::pair<key, zval>>& );
    zmap& operator = ( zmap&& );
    zmap& operator = ( const zmap& );
    zmap& operator = ( const std::initializer_list<std::pair<key, zval>>& );
   ~zmap();

  public:     // serialization
    size_t  GetBufLen(    ) const;
    template <class O>
    O*      Serialize( O* o ) const;
    template <class S>
    S*      FetchFrom( S* );
    template <class S> static
    S*  SkipToEnd( S* );


  public:     // put family
    auto  put( const key&, zval&& )               -> zval*;
    auto  put( const key&, const zval& = zval() ) -> zval*;

  public:     // get_zval family
    auto  get( const key& ) const       -> const zval*;
    auto  get( const key& )             ->       zval*;
    auto  get_type( const key& ) const  -> decltype(zval::vx_type);

  public:     // getters by type
   /*
    * get_xxx( key ) -> xxx* family
    */
    auto  get_char( const key& ) -> char*;
    auto  get_byte( const key& ) -> byte*;
    auto  get_int16( const key& ) -> int16_t*;
    auto  get_int32( const key& ) -> int32_t*;
    auto  get_int64( const key& ) -> int64_t*;
    auto  get_word16( const key& ) -> word16_t*;
    auto  get_word32( const key& ) -> word32_t*;
    auto  get_word64( const key& ) -> word64_t*;
    auto  get_float( const key& ) -> float*;
    auto  get_double( const key& ) -> double*;
    auto  get_bool( const key& ) -> bool*;

    auto  get_charstr( const key& ) -> charstr*;
    auto  get_widestr( const key& ) -> widestr*;
    auto  get_uuid( const key& ) -> uuid*;
    auto  get_zmap( const key& ) -> zmap*;

    auto  get_array_char( const key& ) -> array_char*;
    auto  get_array_byte( const key& ) -> array_byte*;
    auto  get_array_int16( const key& ) -> array_int16_t*;
    auto  get_array_int32( const key& ) -> array_int32_t*;
    auto  get_array_int64( const key& ) -> array_int64_t*;
    auto  get_array_word16( const key& ) -> array_word16_t*;
    auto  get_array_word32( const key& ) -> array_word32_t*;
    auto  get_array_word64( const key& ) -> array_word64_t*;
    auto  get_array_float( const key& ) -> array_float*;
    auto  get_array_double( const key& ) -> array_double*;
    auto  get_array_charstr( const key& ) -> array_charstr*;
    auto  get_array_widestr( const key& ) -> array_widestr*;
    auto  get_array_uuid( const key& ) -> array_uuid*;
    auto  get_array_zval( const key& ) -> array_zval*;
    auto  get_array_zmap( const key& ) -> array_zmap*;

   /*
    * get_xxx( key ) const -> const xxx*
    */
    auto  get_char( const key& ) const -> const char*;
    auto  get_byte( const key& ) const -> const byte*;
    auto  get_int16( const key& ) const -> const int16_t*;
    auto  get_int32( const key& ) const -> const int32_t*;
    auto  get_int64( const key& ) const -> const int64_t*;
    auto  get_word16( const key& ) const -> const word16_t*;
    auto  get_word32( const key& ) const -> const word32_t*;
    auto  get_word64( const key& ) const -> const word64_t*;
    auto  get_float( const key& ) const -> const float*;
    auto  get_double( const key& ) const -> const double*;
    auto  get_bool( const key& ) const -> const bool*;

    auto  get_charstr( const key& ) const -> const charstr*;
    auto  get_widestr( const key& ) const -> const widestr*;
    auto  get_uuid( const key& ) const -> const uuid*;
    auto  get_zmap( const key& ) const -> const zmap*;

    auto  get_array_char( const key& ) const -> const array_char*;
    auto  get_array_byte( const key& ) const -> const array_byte*;
    auto  get_array_int16( const key& ) const -> const array_int16_t*;
    auto  get_array_int32( const key& ) const -> const array_int32_t*;
    auto  get_array_int64( const key& ) const -> const array_int64_t*;
    auto  get_array_word16( const key& ) const -> const array_word16_t*;
    auto  get_array_word32( const key& ) const -> const array_word32_t*;
    auto  get_array_word64( const key& ) const -> const array_word64_t*;
    auto  get_array_float( const key& ) const -> const array_float*;
    auto  get_array_double( const key& ) const -> const array_double*;
    auto  get_array_charstr( const key& ) const -> const array_charstr*;
    auto  get_array_widestr( const key& ) const -> const array_widestr*;
    auto  get_array_uuid( const key& ) const -> const array_uuid*;
    auto  get_array_zval( const key& ) const -> const array_zval*;
    auto  get_array_zmap( const key& ) const -> const array_zmap*;

   /*
    * get_xxx( key, default_value ) const -> default_value family
    */
    auto  get_char( const key&, char ) const -> char;
    auto  get_byte( const key&, byte ) const -> byte;
    auto  get_int16( const key&, int16_t ) const -> int16_t;
    auto  get_int32( const key&, int32_t ) const -> int32_t;
    auto  get_int64( const key&, int64_t ) const -> int64_t;
    auto  get_word16( const key&, word16_t ) const -> word16_t;
    auto  get_word32( const key&, word32_t ) const -> word32_t;
    auto  get_word64( const key&, word64_t ) const -> word64_t;
    auto  get_float( const key&, float ) const -> float;
    auto  get_double( const key&, double ) const -> double;
    auto  get_bool( const key&, bool ) const -> bool;

    auto  get_charstr( const key&, const charstr& ) const -> const charstr&;
    auto  get_widestr( const key&, const widestr& ) const -> const widestr&;
    auto  get_uuid( const key&, const uuid& ) const -> const uuid&;
    auto  get_zmap( const key&, const zmap& ) const -> const zmap&;

    auto  get_array_char( const key&, const array_char& ) const -> const array_char&;
    auto  get_array_byte( const key&, const array_byte& ) const -> const array_byte&;
    auto  get_array_int16( const key&, const array_int16_t& ) const -> const array_int16_t&;
    auto  get_array_int32( const key&, const array_int32_t& ) const -> const array_int32_t&;
    auto  get_array_int64( const key&, const array_int64_t& ) const -> const array_int64_t&;
    auto  get_array_word16( const key&, const array_word16_t& ) const -> const array_word16_t&;
    auto  get_array_word32( const key&, const array_word32_t& ) const -> const array_word32_t&;
    auto  get_array_word64( const key&, const array_word64_t& ) const -> const array_word64_t&;
    auto  get_array_float( const key&, const array_float& ) const -> const array_float&;
    auto  get_array_double( const key&, const array_double& ) const -> const array_double&;
    auto  get_array_charstr( const key&, const array_charstr& ) const -> const array_charstr&;
    auto  get_array_widestr( const key&, const array_widestr& ) const -> const array_widestr&;
    auto  get_array_uuid( const key&, const array_uuid& ) const -> const array_uuid&;
    auto  get_array_zval( const key&, const array_zval& ) const -> const array_zval&;
    auto  get_array_zmap( const key&, const array_zmap& ) const -> const array_zmap&;

   /*
    * set_xxx( key ) family
    */
    auto  set_char( const key& ) -> char*;
    auto  set_byte( const key& ) -> byte*;
    auto  set_int16( const key& ) -> int16_t*;
    auto  set_int32( const key& ) -> int32_t*;
    auto  set_int64( const key& ) -> int64_t*;
    auto  set_word16( const key& ) -> word16_t*;
    auto  set_word32( const key& ) -> word32_t*;
    auto  set_word64( const key& ) -> word64_t*;
    auto  set_float( const key& ) -> float*;
    auto  set_double( const key& ) -> double*;
    auto  set_bool( const key& ) -> bool*;

    auto  set_charstr( const key& ) -> charstr*;
    auto  set_widestr( const key& ) -> widestr*;
    auto  set_uuid( const key& ) -> uuid*;
    auto  set_zmap( const key& ) -> zmap*;

    auto  set_array_char( const key& ) -> array_char*;
    auto  set_array_byte( const key& ) -> array_byte*;
    auto  set_array_int16( const key& ) -> array_int16_t*;
    auto  set_array_int32( const key& ) -> array_int32_t*;
    auto  set_array_int64( const key& ) -> array_int64_t*;
    auto  set_array_word16( const key& ) -> array_word16_t*;
    auto  set_array_word32( const key& ) -> array_word32_t*;
    auto  set_array_word64( const key& ) -> array_word64_t*;
    auto  set_array_float( const key& ) -> array_float*;
    auto  set_array_double( const key& ) -> array_double*;
    auto  set_array_charstr( const key& ) -> array_charstr*;
    auto  set_array_widestr( const key& ) -> array_widestr*;
    auto  set_array_uuid( const key& ) -> array_uuid*;
    auto  set_array_zval( const key& ) -> array_zval*;
    auto  set_array_zmap( const key& ) -> array_zmap*;

   /*
    * set_xxx( key, initializer ) family
    */
    auto  set_char( const key&, char ) -> char*;
    auto  set_byte( const key&, byte ) -> byte*;
    auto  set_int16( const key&, int16_t ) -> int16_t*;
    auto  set_int32( const key&, int32_t ) -> int32_t*;
    auto  set_int64( const key&, int64_t ) -> int64_t*;
    auto  set_word16( const key&, word16_t ) -> word16_t*;
    auto  set_word32( const key&, word32_t ) -> word32_t*;
    auto  set_word64( const key&, word64_t ) -> word64_t*;
    auto  set_float( const key&, float ) -> float*;
    auto  set_double( const key&, double ) -> double*;
    auto  set_bool( const key&, bool ) -> bool*;

    auto  set_charstr( const key&, const charstr& ) -> charstr*;
    auto  set_widestr( const key&, const widestr& ) -> widestr*;
    auto  set_uuid( const key&, const uuid& ) -> uuid*;
    auto  set_zmap( const key&, const zmap& ) -> zmap*;

    auto  set_charstr( const key&, const char*, size_t = (size_t)-1 ) -> charstr*;
    auto  set_widestr( const key&, const widechar*, size_t = (size_t)-1 ) -> widestr*;

    auto  set_array_char( const key&, const array_char& ) -> array_char*;
    auto  set_array_byte( const key&, const array_byte& ) -> array_byte*;
    auto  set_array_int16( const key&, const array_int16_t& ) -> array_int16_t*;
    auto  set_array_int32( const key&, const array_int32_t& ) -> array_int32_t*;
    auto  set_array_int64( const key&, const array_int64_t& ) -> array_int64_t*;
    auto  set_array_word16( const key&, const array_word16_t& ) -> array_word16_t*;
    auto  set_array_word32( const key&, const array_word32_t& ) -> array_word32_t*;
    auto  set_array_word64( const key&, const array_word64_t& ) -> array_word64_t*;
    auto  set_array_float( const key&, const array_float& ) -> array_float*;
    auto  set_array_double( const key&, const array_double& ) -> array_double*;
    auto  set_array_charstr( const key&, const array_charstr& ) -> array_charstr*;
    auto  set_array_widestr( const key&, const array_widestr& ) -> array_widestr*;
    auto  set_array_uuid( const key&, const array_uuid& ) -> array_uuid*;
    auto  set_array_zval( const key&, const array_zval& ) -> array_zval*;
    auto  set_array_zmap( const key&, const array_zmap& ) -> array_zmap*;

  /*
   * set_xxx( key, initializer&& ) family
   */
    auto  set_charstr( const key&, charstr&& ) -> charstr*;
    auto  set_widestr( const key&, widestr&& ) -> widestr*;
    auto  set_uuid( const key&, uuid&& ) -> uuid*;
    auto  set_zmap( const key&, zmap&& ) -> zmap*;

    auto  set_array_char( const key&, array_char&& ) -> array_char*;
    auto  set_array_byte( const key&, array_byte&& ) -> array_byte*;
    auto  set_array_int16( const key&, array_int16_t&& ) -> array_int16_t*;
    auto  set_array_int32( const key&, array_int32_t&& ) -> array_int32_t*;
    auto  set_array_int64( const key&, array_int64_t&& ) -> array_int64_t*;
    auto  set_array_word16( const key&, array_word16_t&& ) -> array_word16_t*;
    auto  set_array_word32( const key&, array_word32_t&& ) -> array_word32_t*;
    auto  set_array_word64( const key&, array_word64_t&& ) -> array_word64_t*;
    auto  set_array_float( const key&, array_float&& ) -> array_float*;
    auto  set_array_double( const key&, array_double&& ) -> array_double*;
    auto  set_array_charstr( const key&, array_charstr&& ) -> array_charstr*;
    auto  set_array_widestr( const key&, array_widestr&& ) -> array_widestr*;
    auto  set_array_uuid( const key&, array_uuid&& ) -> array_uuid*;
    auto  set_array_zval( const key&, array_zval&& ) -> array_zval*;
    auto  set_array_zmap( const key&, array_zmap&& ) -> array_zmap*;

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
    int  compare( const zmap& ) const;
    bool operator== ( const zmap& z ) const {  return compare( z ) == 0;  }
    bool operator!= ( const zmap& z ) const {  return compare( z ) != 0;  }
    bool operator<  ( const zmap& z ) const {  return compare( z ) < 0;  }
    bool operator<= ( const zmap& z ) const {  return compare( z ) <= 0;  }
    bool operator>  ( const zmap& z ) const {  return compare( z ) >  0;  }
    bool operator>= ( const zmap& z ) const {  return compare( z ) >= 0;  }

  protected:
    zdata_t*  p_data = nullptr;

  };

 /*
  * zmap::key
  *
  * mutable associative key
  */
  class zmap::key
  {
    friend class zmap;
    template <class value, class z_iterator>
    friend class iterator_base;

  private:
    unsigned          _typ;
    union
    {
      const uint8_t*  _psz;
      const widechar* _wsz;
    };
    size_t            _len;

  private:
    uint8_t           _buf[4];

  public:     // key types
    enum: uint8_t
    {
      uint = 0,
      cstr = 1,
      wstr = 2,
      none = (uint8_t)-1
    };

  public:
    key();
    key( unsigned typ, const uint8_t* buf, size_t len );

  public: // construction
    key( unsigned );
    key( const char* );
    key( const widechar* );
    key( const char*, size_t );
    key( const widechar*, size_t );
    key( const charstr& );
    key( const widestr& );
    key( const key& );
    key& operator = ( const key& );

  public:
    auto  operator == ( const key& k ) const -> bool;
    auto  operator != ( const key& k ) const -> bool {  return !(*this == k);  }
    bool  operator <  ( const key& k ) const  {  return compare( k ) < 0;  }

    auto  compare( const key& ) const -> int;

  public:
    static
    auto  null() -> key {  return key();  }

  public: // data
    auto  type() const  -> unsigned       {  return _typ;  }
    auto  data() const  -> const uint8_t* {  return _psz;  }
    auto  size() const  -> size_t         {  return _len;  }

  public: // operators
    operator unsigned () const;
    operator const char* () const;
    operator const widechar* () const;

  public: // accessors
    auto  is_charstr() const -> bool  {  return _typ == cstr;  }
    auto  is_widestr() const -> bool  {  return _typ == wstr;  }
    auto  to_charstr() const -> const char* {  return (const char*)*this;  }
    auto  to_widestr() const -> const widechar* {  return (const widechar*)*this;  }

  };

 /*
  * zval::dump class declaration
  *
  * Доступ к сериализованным данным без тотальной десериализации.
  */
  class zval::dump
  {
    struct zvalue
    {
      zval  value;
      int   count;
    };

    const char* source;
    union
    {
      zvalue*   holder;
      zval*     pvalue;
    };

    template <class T>
    class store_t;

  protected:
    void  delete_it();

  public:
    template <class T>
    class value_t;
    class zview_t;
    template <class T1, class T2=T1>
    class array_t;

  public:
    dump( const char* s = nullptr );
    dump( const dump& );
    dump( const zval& );
    dump( const zval* );
   ~dump();

  public:
    dump& operator = ( const dump& );
    dump& operator = ( const zval& );
    dump& operator = ( const zval* );

  public:
    auto  get_type() const -> unsigned;

  public:     // access
    auto  get_char() const -> value_t<char>;
    auto  get_byte() const -> value_t<byte>;
    auto  get_int16() const -> value_t<int16_t>;
    auto  get_int32() const -> value_t<int32_t>;
    auto  get_int64() const -> value_t<int64_t>;
    auto  get_word16() const -> value_t<word16_t>;
    auto  get_word32() const -> value_t<word32_t>;
    auto  get_word64() const -> value_t<word64_t>;
    auto  get_float() const -> value_t<float>;
    auto  get_double() const -> value_t<double>;
    auto  get_bool() const -> value_t<bool>;

    auto  get_charstr() const -> value_t<charstr>;
    auto  get_widestr() const -> value_t<widestr>;
    auto  get_uuid() const -> value_t<uuid>;
    auto  get_zmap() const -> value_t<zmap::dump>;

    auto  get_array_char() const -> value_t<array_t<char>>;
    auto  get_array_byte() const -> value_t<array_t<byte>>;
    auto  get_array_int16() const -> value_t<array_t<int16_t>>;
    auto  get_array_int32() const -> value_t<array_t<int32_t>>;
    auto  get_array_int64() const -> value_t<array_t<int64_t>>;
    auto  get_array_word16() const -> value_t<array_t<word16_t>>;
    auto  get_array_word32() const -> value_t<array_t<word32_t>>;
    auto  get_array_word64() const -> value_t<array_t<word64_t>>;
    auto  get_array_float() const -> value_t<array_t<float>>;
    auto  get_array_double() const -> value_t<array_t<double>>;
    auto  get_array_charstr() const -> value_t<array_t<charstr>>;
    auto  get_array_widestr() const -> value_t<array_t<widestr>>;
    auto  get_array_uuid() const -> value_t<array_t<uuid>>;
    auto  get_array_zval() const -> value_t<array_t<dump, zval>>;
    auto  get_array_zmap() const -> value_t<array_t<zmap::dump, zmap>>;

  public:
    auto  CompTo( const dump& x ) const -> unsigned;

    bool  lt( const dump& z ) const {  return CompTo( z ) == compare_lt;  }
    bool  gt( const dump& z ) const {  return CompTo( z ) == compare_gt;  }
    bool  eq( const dump& z ) const {  return CompTo( z ) == compare_eq;  }
    bool  le( const dump& z ) const {  return (CompTo( z ) & compare_le) != 0;  }
    bool  ge( const dump& z ) const {  return (CompTo( z ) & compare_ge) != 0;  }
    bool  ne( const dump& z ) const {  return !eq( z );  }

  public:
    bool  operator == ( const dump& v ) const;
    bool  operator != ( const dump& v ) const {  return !(*this == v);  }

    bool  operator == ( const zval& v ) const {  return *this == dump( &v );  }
    bool  operator != ( const zval& v ) const {  return !(*this == v);  }

    bool  operator == ( const char* s ) const {  return *this == charstr( s );  }
    bool  operator != ( const char* s ) const {  return !(*this == s);  }

    bool  operator == ( const widechar* w ) const {  return *this == widestr( w );  }
    bool  operator != ( const widechar* w ) const {  return !(*this == w);  }

  public:
    operator  zval() const;

  public:
    const char* FetchFrom( const char* s )  {  return ::SkipToEnd( source = s, (const zval*)nullptr );  }
    static
    const char* SkipToEnd( const char* s )  {  return ::SkipToEnd( s, (const zval*)nullptr );  }

  };

 /*
  * zmap::dump
  */
  class zmap::dump
  {
    const char* source;   /* source == nullptr => object was loaded if pvalue != nullptr;   */
                          /* source == (char*)-1 => object was passed by external pointer;  */
                          /* else object is absent and would be loaded on access.           */
    const zmap* pvalue;

  public:     // construction
    template <class T>
    using value_t = zval::dump::value_t<T>;
    using zview_t = zval::dump::zview_t;
    template <class T1, class T2=T1>
    using array_t = zval::dump::array_t<T1, T2>;

  protected:
    template <class T> static auto  get( const value_t<T>&, const T& ) -> const T&;
    auto  get_dump( const key& ) const -> zval::dump;

    class const_iterator;

  public:     // construction
    dump( const char* s = nullptr ): source( s ), pvalue( nullptr ) {}
    dump( const dump& );
    dump( const zmap& );
    dump( const zmap* );
   ~dump();

  public:
    dump& operator = ( const dump& );
    dump& operator = ( const zmap& );
    dump& operator = ( const zmap* );

  public:     // simple get
    auto  get( const key& ) const -> zview_t;

  public:     // get specializations
    auto  get_char( const key& ) const -> value_t<char>;
    auto  get_byte( const key& ) const -> value_t<byte>;
    auto  get_int16( const key& ) const -> value_t<int16_t>;
    auto  get_int32( const key& ) const -> value_t<int32_t>;
    auto  get_int64( const key& ) const -> value_t<int64_t>;
    auto  get_word16( const key& ) const -> value_t<word16_t>;
    auto  get_word32( const key& ) const -> value_t<word32_t>;
    auto  get_word64( const key& ) const -> value_t<word64_t>;
    auto  get_float( const key& ) const -> value_t<float>;
    auto  get_double( const key& ) const -> value_t<double>;
    auto  get_charstr( const key& ) const -> value_t<charstr>;
    auto  get_widestr( const key& ) const -> value_t<widestr>;
    auto  get_uuid( const key& ) const -> value_t<uuid>;
    auto  get_zmap( const key& ) const -> value_t<zmap::dump>;

    auto  get_array_char( const key& ) const -> value_t<array_t<char>>;
    auto  get_array_byte( const key& ) const -> value_t<array_t<byte>>;
    auto  get_array_int16( const key& ) const -> value_t<array_t<int16_t>>;
    auto  get_array_int32( const key& ) const -> value_t<array_t<int32_t>>;
    auto  get_array_int64( const key& ) const -> value_t<array_t<int64_t>>;
    auto  get_array_word16( const key& ) const -> value_t<array_t<word16_t>>;
    auto  get_array_word32( const key& ) const -> value_t<array_t<word32_t>>;
    auto  get_array_word64( const key& ) const -> value_t<array_t<word64_t>>;
    auto  get_array_float( const key& ) const -> value_t<array_t<float>>;
    auto  get_array_double( const key& ) const -> value_t<array_t<double>>;
    auto  get_array_charstr( const key& ) const -> value_t<array_t<charstr>>;
    auto  get_array_widestr( const key& ) const -> value_t<array_t<widestr>>;
    auto  get_array_uuid( const key& ) const -> value_t<array_t<uuid>>;
    auto  get_array_zval( const key& ) const -> value_t<array_t<zval::dump, zval>>;
    auto  get_array_zmap( const key& ) const -> value_t<array_t<zmap::dump, zmap>>;

    auto  get_char( const key&, char ) const -> char;
    auto  get_byte( const key&, byte ) const -> byte;
    auto  get_int16( const key&, int16_t ) const -> int16_t;
    auto  get_int32( const key&, int32_t ) const -> int32_t;
    auto  get_int64( const key&, int64_t ) const -> int64_t;
    auto  get_word16( const key&, word16_t ) const -> word16_t;
    auto  get_word32( const key&, word32_t ) const -> word32_t;
    auto  get_word64( const key&, word64_t ) const -> word64_t;
    auto  get_float( const key&, float ) const -> float;
    auto  get_double( const key&, double ) const -> double;
    auto  get_charstr( const key&, const charstr& ) const -> const charstr&;
    auto  get_widestr( const key&, const widestr& ) const -> const widestr&;
    auto  get_uuid( const key&, const uuid& ) const -> const uuid&;
    auto  get_zmap( const key&, const zmap& ) const -> dump;

    auto  get_array_char( const key&, const array_char& ) const -> array_t<char>;
    auto  get_array_byte( const key&, const array_byte& ) const -> array_t<byte>;
    auto  get_array_int16( const key&, const array_int16& ) const -> array_t<int16_t>;
    auto  get_array_int32( const key&, const array_int32& ) const -> array_t<int32_t>;
    auto  get_array_int64( const key&, const array_int64& ) const -> array_t<int64_t>;
    auto  get_array_word16( const key&, const array_word16& ) const -> array_t<word16_t>;
    auto  get_array_word32( const key&, const array_word32& ) const -> array_t<word32_t>;
    auto  get_array_word64( const key&, const array_word64& ) const -> array_t<word64_t>;
    auto  get_array_float( const key&, const array_float& ) const -> array_t<float>;
    auto  get_array_double( const key&, const array_double& ) const -> array_t<double>;
    auto  get_array_charstr( const key&, const array_charstr& ) const -> array_t<charstr>;
    auto  get_array_widestr( const key&, const array_widestr& ) const -> array_t<widestr>;
    auto  get_array_uuid( const key&, const array_uuid& ) const -> array_t<uuid>;
    auto  get_array_zval( const key&, const array_zval& ) const -> array_t<zval::dump, zval>;
    auto  get_array_zmap( const key&, const array_zmap& ) const -> array_t<zmap::dump, zmap>;

  public:     // iterator
    auto  begin() const -> const_iterator;
    auto  end() const -> const_iterator;
    auto  cbegin() const -> const_iterator;
    auto  cend() const -> const_iterator;

    bool  empty() const {  return pvalue == nullptr && source == nullptr;  }

  public:
    bool  operator == ( const dump& v ) const;
    bool  operator == ( const zmap& v ) const {  return *this == dump( &v );  }
    bool  operator != ( const dump& v ) const {  return !(*this == v);  }
    bool  operator != ( const zmap& v ) const {  return !(*this == v);  }

  public:
    operator zmap() const;

  public:     // serialization support
    const char* FetchFrom( const char* s )  {  return ::SkipToEnd( source = s, (const zmap*)nullptr );  }
    static
    const char* SkipToEnd( const char* s )  {  return ::SkipToEnd( s, (const zmap*)nullptr );  }

  };

  class zmap::dump::const_iterator
  {
    struct iterator_data
    {
      zmap::key   first;
      zval::dump  second;
    };

    class dump_iterator;
    class zmap_iterator;

    friend class zmap::dump;

    std::shared_ptr<dump_iterator>  asDump;
    std::shared_ptr<zmap_iterator>  asZmap;

    const_iterator( const char* );
    const_iterator( const zmap::const_iterator& );

  public:
    const_iterator() = default;
    const_iterator( const const_iterator& s ): asDump( s.asDump ), asZmap( s.asZmap ) {}

  public:
    bool  operator == ( const const_iterator& ) const;
    bool  operator != ( const const_iterator& it ) const {  return !(*this == it);  }

  public:
    auto  operator ++() -> const_iterator&;
    auto  operator ++(int) -> const_iterator;
  public:
    auto  operator -> () const -> const iterator_data*;
    auto  operator * () const -> const iterator_data&;

  protected:
    bool  is_empty() const  {  return asDump == nullptr && asZmap == nullptr;  }

  };

 /*
  * serial::*() methods
  *
  * static methods allowing direct access to the field of serialized zmap structure
  * special fast implementation for 'const char*' source also provided
  */
  class zmap::serial final
  {
    template <class S>  static  S*    find( S*, const byte_t*, size_t, unsigned );

  public:     // serial::* family
    struct skip;

    template <class S>  static  S*    find(        S*, const key& );
    template <class S>  static  S*    load( zval&, S*, const key& );
    template <class S>  static  zval  load(        S*, const key& );
  };

  struct zmap::serial::skip final
  {
    template <class S>  static auto  size( S* s, size_t l ) -> S*  {  return ::SkipBytes( s, l );  }
    template <class S>  static auto  zval( S* s ) -> S*  {  return ::SkipToEnd( s, (const mtc::zval*)nullptr );  }
    template <class S>  static auto  zmap( S* s ) -> S*  {  return ::SkipToEnd( s, (const mtc::zmap*)nullptr );  }
  };

 /*
  * store_t handles:
  *   - serialized data pointer as 'source', or
  *   - source == nullptr && cached/avaluated data as 'stored', or
  *   - source == (const char*)-1 && 'stored' pointing to real data T*.
  */
  template <class T>
  class zval::dump::store_t
  {
    friend class zval::dump;
    friend class zmap::dump;

    struct zvalue
    {
              T   value;
      mutable int count;
    };

  protected:
    mutable const char* source;
    union
    {
      mutable const zvalue* holder;
      mutable const void*   object;
    };

  protected:
    store_t( const char* s, const T* p ):
      source( s ),
      object( p )  {}
    store_t( std::nullptr_t, const T& t ):
      source( nullptr ),
      holder( new zvalue{ t, 1 } )  {}

  protected:
    auto  fetch() const -> const T&
    {
      if ( holder != nullptr )
      {
        if ( source == nullptr )
          return holder->value;
        if ( source == invalid )
          return *(T*)object;
        throw std::logic_error( "mtc::zval::dump::store_t initialized with invalid data" );
      }

      if ( source == invalid )
        throw std::logic_error( "mtc::zval::dump::store_t initialized with invalid data" );

      if ( source == nullptr )
        throw std::logic_error( "mtc::zval::dump::store_t is not initialized" );

      ::FetchFrom( source, ((zvalue*)(holder = new zvalue{ T(), 1 }))->value );
        source = nullptr;

      return holder->value;
    }
    void  delete_it()
    {
      if ( holder != nullptr && source != invalid && --holder->count == 0 )
        delete holder;
    }
  public:
    store_t():
      source( nullptr ),
      holder( nullptr ) {}
    store_t( const store_t& t ):
      source( t.source ),
      holder( t.holder )
    {
      if ( holder != nullptr && source == nullptr )
        ++holder->count;
    }
    auto  operator = ( const store_t& t ) -> store_t&
    {
      if ( &t == this )
        return *this;

      delete_it();

      holder = t.holder;
      source = t.source;

      if ( holder != nullptr && source == nullptr )
        ++holder->count;

      return *this;
    }
   ~store_t() {  delete_it();  }

  public:
    auto  operator * () const -> const T& {  return fetch();  }
    auto  operator -> () const -> const T*  {  return &fetch();  }

  public:
    bool  operator == ( std::nullptr_t ) const
      {  return holder == nullptr && (source == nullptr || source == invalid);  }
    bool  operator != ( std::nullptr_t ) const
      {  return !(*this == nullptr);  }
  };

  template <class T>
  class zval::dump::value_t: public store_t<T>
  {
    using store_t<T>::store_t;

    friend class zval::dump;
    friend class zmap::dump;

  public:     // custom constructors
    value_t( const T* t ):
      store_t<T>( invalid, t )  {}

  public:     // value assignment
    auto  operator = ( const T* t ) -> value_t&
    {
      this->delete_it();
        this->source = invalid;
        this->object = (T*)t;
      return *this;
    }
  };

  class zval::dump::zview_t: public store_t<zval::dump>
  {
    using store_t<zval::dump>::store_t;

    friend class zval::dump;
    friend class zmap::dump;

  public:     // value assignment
    auto  operator = ( const zval::dump* t ) -> zview_t&
      {
        delete_it();
          source = invalid,
          object = (void*)t;
        return *this;
      }
    auto  operator = ( const zval* t ) -> zview_t&
      {
        delete_it();
          source = nullptr;
          holder = new zvalue{ t, 1 };
        return *this;
      }
  };

  template <class T1, class T2>
  class zval::dump::array_t
  {
    using vector_t = std::vector<T2>;

    const char*     source = nullptr;
    size_t          ncount = 0;
    const vector_t* parray = nullptr;

  public:
    typedef   T1  value_type;

  protected:
    class const_iterator
    {
      using iterator = typename vector_t::const_iterator;

      const char* first = nullptr;
      size_t      count = 0;
      iterator    a_beg;
      iterator    a_end;

      struct as_struct: public T1
      {
        as_struct() = default;
        as_struct( const T1& t ): T1( t ) {}

        auto  operator -> () const -> const T1*  {  return this;  }
        auto  FetchFrom( const char* s ) -> const char* {  return ::FetchFrom( s, (T1&)*this );  }
      };

      using element = typename std::conditional<std::is_fundamental<T1>::value,
        T1, as_struct>::type;

      auto  get_element() const -> element;

    public:
      const_iterator() = default;
      const_iterator( const array_t& a ):
        first( a.source ),
        count( a.ncount ),
        a_beg( a.parray != nullptr ? a.parray->begin() : iterator() ),
        a_end( a.parray != nullptr ? a.parray->end() : iterator() ) {}

    public:
      auto  operator ++() -> const_iterator&
      {
        if ( first == nullptr && a_beg == a_end )
          throw std::invalid_argument( "invalid iterator operation" );
        if ( count > 0 )  first = --count == 0 ? nullptr : ::SkipToEnd( first, (const T1*)nullptr );
          else
        if ( ++a_beg == a_end ) a_beg = a_end = iterator();
        return *this;
      }
      auto  operator ++( int ) -> const_iterator
      {
        auto  it( *this );
        return operator ++(), it;
      }

      bool  operator == ( const const_iterator& i ) const
        {
          return first == i.first
              && count == i.count
              && a_beg == i.a_beg;
        }
      bool  operator != ( const const_iterator& i ) const {  return !(*this == i);  }

      auto  operator * () const -> element {  return get_element();  }
      auto  operator -> () const -> element {  return get_element();  }
    };

  public:
    array_t() = default;
    array_t( const array_t& t ):
      source( t.source ),
      ncount( t.ncount ),
      parray( t.parray ) {}
    array_t( const vector_t* a ):
      source( nullptr ),
      ncount( 0 ),
      parray( a ) {}
    array_t& operator = ( const array_t& t )
      {
        return source = t.source,
               ncount = t.ncount,
               parray = t.parray, *this;
      }

  public:
    bool  operator == ( const array_t& a ) const;
    bool  operator != ( const array_t& a ) const;

  public:
    size_t  size() const {  return parray != nullptr ? parray->size() : ncount;  }
    bool    empty() const {  return size() == 0;  }

  public:
    const_iterator  begin() const {  return { *this };  }
    const_iterator  end() const {  return {};  }

  public:
    const char* FetchFrom( const char* s )  {  return source = ::FetchFrom( s, ncount );  }

  };

  template <class T1, class T2>
  bool  zval::dump::array_t<T1, T2>::operator == ( const array_t& a ) const
    {
      auto  mp = begin(), me = end();
      auto  ap = a.begin(), ae = a.end();

      while ( mp != me && ap != ae )
        if ( *mp != *ap ) return false;
          else { ++mp; ++ap; }

      return mp == me && ap == ae;
    }

  template <class T1, class T2>
  bool  zval::dump::array_t<T1, T2>::operator != ( const array_t& a ) const
    {
      auto  mp = begin(), me = end();
      auto  ap = a.begin(), ae = a.end();

      while ( mp != me && ap != ae )
        if ( *mp != *ap ) return true;
          else { ++mp; ++ap; }

      return mp != me || ap != ae;
    }

  template <class T1, class T2>
  auto  zval::dump::array_t<T1, T2>::const_iterator::get_element() const -> element
  {
    element el;

    if ( (first == nullptr || count == 0) && a_beg == a_end )
      throw std::range_error( "iterator limits out of bounds" );
    if ( a_beg != a_end ) return (T1)*a_beg;
      else return ::FetchFrom( first, el ), std::move( el );
  }

  inline  std::string to_string( const zval& z ) {  return z.to_string();  }
          std::string to_string( const zmap::key& );

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
    derive_var( bool )

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
      derive_put_plain( bool )

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
      case z_untyped: return ::Serialize( o, vx_type );
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
      derive_get_plain( bool )

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
      derive_get_smart( array_widestr )
      derive_get_smart( array_zmap )
      derive_get_smart( array_zval )
      derive_get_smart( array_uuid )
  # undef derive_get_smart
  # undef derive_get_plain
      case z_untyped: return s;
      default:  return nullptr;
    }
  }

  template <class S>
  S*  zval::SkipToEnd( S* s )
  {
    byte_t    vxtype;

    if ( (s = ::FetchFrom( s, vxtype )) == nullptr )
      return nullptr;
    switch ( vxtype )
    {
      case zval::z_char:    return ::SkipBytes( s, sizeof(char) );
      case zval::z_byte:    return ::SkipBytes( s, sizeof(byte) );
      case zval::z_int16:   return ::SkipToEnd( s, (const int16_t*)nullptr );
      case zval::z_int32:   return ::SkipToEnd( s, (const int32_t*)nullptr );
      case zval::z_int64:   return ::SkipToEnd( s, (const int64_t*)nullptr );
      case zval::z_word16:  return ::SkipToEnd( s, (const word16_t*)nullptr );
      case zval::z_word32:  return ::SkipToEnd( s, (const word32_t*)nullptr );
      case zval::z_word64:  return ::SkipToEnd( s, (const word64_t*)nullptr );
      case zval::z_float:   return ::SkipToEnd( s, (const float*)nullptr );
      case zval::z_double:  return ::SkipToEnd( s, (const double*)nullptr );
      case zval::z_charstr: return ::SkipToEnd( s, (const charstr*)nullptr );
      case zval::z_widestr: return ::SkipToEnd( s, (const widestr*)nullptr );
      case zval::z_uuid:    return ::SkipToEnd( s, (const uuid*)nullptr );
      case zval::z_zmap:    return ::SkipToEnd( s, (const mtc::zmap*)nullptr );

      case zval::z_array_char:    return ::SkipToEnd( s, (const array_char*)nullptr );
      case zval::z_array_byte:    return ::SkipToEnd( s, (const array_byte*)nullptr );
      case zval::z_array_int16:   return ::SkipToEnd( s, (const array_int16*)nullptr );
      case zval::z_array_int32:   return ::SkipToEnd( s, (const array_int32*)nullptr );
      case zval::z_array_int64:   return ::SkipToEnd( s, (const array_int64*)nullptr );
      case zval::z_array_word16:  return ::SkipToEnd( s, (const array_word16*)nullptr );
      case zval::z_array_word32:  return ::SkipToEnd( s, (const array_word32*)nullptr );
      case zval::z_array_word64:  return ::SkipToEnd( s, (const array_word64*)nullptr );
      case zval::z_array_float:   return ::SkipToEnd( s, (const array_float*)nullptr );
      case zval::z_array_double:  return ::SkipToEnd( s, (const array_double*)nullptr );
      case zval::z_array_charstr: return ::SkipToEnd( s, (const array_charstr*)nullptr );
      case zval::z_array_widestr: return ::SkipToEnd( s, (const array_widestr*)nullptr );
      case zval::z_array_zmap:    return ::SkipToEnd( s, (const mtc::array_zmap*)nullptr );
      case zval::z_array_zval:    return ::SkipToEnd( s, (const mtc::array_zval*)nullptr );
      case zval::z_array_uuid:    return ::SkipToEnd( s, (const array_uuid*)nullptr );
      case zval::z_untyped:       return s;
      default:                    return nullptr;
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
            auto  plain_branchlen() const -> int;
            auto  plain_ctl_bytes() const -> word32_t;

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

  class zmap::zbuff_t: private std::vector<char>
  {
    using inherited = std::vector<char>;
    using inherited::vector;

  public:
    void  push_back( char );
    void  pop_back();
    auto  back() -> char&;
    auto  back() const -> char;
    auto  size() const -> size_t;
    auto  data() const -> const char*;

  };

  template <class S>
  class zmap::storage
  {
    S*      source;

  public:
    storage( S* s, size_t ): source( s ) {}

  public:
    template <class T>
    S*  operator()( T& t, size_t n )  {  return t.FetchFrom( source, n );  }
  };

  template <class S>
  class zmap::limiter: protected pmr::storage
  {
    S*      source;
    size_t  length;

  public:
    limiter( S* s, size_t l = (size_t)-1 ):
      source( s ),
      length( l )  {}

  public:
    template <class T>
    S*  operator()( T& t, size_t n )
    {
      return t.FetchFrom( (pmr::storage*)this, n ), this->source;
    }

  protected:
    auto  FetchFrom( void* p, size_t l ) -> storage* override
    {
      if ( source != nullptr && l <= length && (source = ::FetchFrom( source, p, l )) != nullptr )
        return length -= l, this;
      else
        return source = nullptr, nullptr;
    }
  };

  template <class value>
  class zmap::iterator_data
  {
    template <class v, class zit>
    friend class iterator_base;

    constexpr value*  null_value() const {  return nullptr;  }

    iterator_data(): second( *null_value() ) {}
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
    iterator_base() = default;
    iterator_base( iterator_base&& );
    iterator_base( const iterator_base& );

  public:
    auto  operator -> () const -> const value*;
    auto  operator * () const -> const value&;
    auto  operator ++ () -> const iterator_base&;
    auto  operator -- () -> const iterator_base&;
    auto  operator ++ ( int ) -> iterator_base;
    auto  operator -- ( int ) -> iterator_base;
    bool  operator == ( const iterator_base& it ) const;
    bool  operator != ( const iterator_base& it ) const {  return !(*this == it);  }

  protected:
    struct zpos
    {
      z_iterator  beg;
      z_iterator  end;

    public:
      bool  empty() const {  return beg == end;  }
      bool  operator == ( const zpos& z ) const {  return beg == z.beg && end == z.end;  }
    };

    auto  init() -> iterator_base&;
    auto  find() -> iterator_base&;
    auto  next() -> iterator_base&;
    auto  prev() -> iterator_base&;
    void  down( z_iterator );
    void  back();
    auto  last()       ->       zpos& {  return zstack.back();  }
    auto  last() const -> const zpos& {  return zstack.back();  }

  protected:
    value             zvalue;
    std::vector<zpos> zstack;
    zbuff_t           keybuf;

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
  * [] zmap zccess
  */
  template <class obj>
  class zmap::place_t
  {
    template <class map>
    friend  bool  is_set( const place_t<map>& );
    template <class map>
    friend  bool  get_type( const place_t<map>& );
    template <class map>
    friend  bool  is_array( const place_t<map>& );

  public:
    place_t( const key& k, obj& m ): refer( k ), owner( m ) {}
    place_t( const place_t& p ): refer( p.refer ), owner( p.owner ) {}
    place_t( place_t&& p ): refer( std::move( p.refer ) ), owner( p.owner ) {}

  public:
    bool  operator == ( const zval& z ) const;
    bool  operator != ( const zval& z ) const {  return !(*this == z);  }

    bool  operator < ( const zval& z ) const;
    bool  operator <= ( const zval& z ) const;

    bool  operator > ( const zval& z ) const;
    bool  operator >= ( const zval& z ) const;

  protected:
    key   refer;
    obj&  owner;

  };

  class zmap::const_place_t: protected place_t<const zmap>
  {
    friend class zmap;

    using place_t<const zmap>::place_t;

  public:
    /*
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
    operator zmap         () const;
    */

  public:
    /*
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
    */

  };

  class zmap::patch_place_t: public place_t<zmap>
  {
    friend class zmap;

    using place_t<zmap>::place_t;

  public:
    auto  operator = ( zval&& ) -> patch_place_t&;
    auto  operator = ( const zval& ) -> patch_place_t&;

  };

 /*
  * helpers
  */
  template <class map>
  bool  is_set( const zmap::place_t<map>& place )
  {  return place.owner.get( place.refer ) != nullptr;  }

  template <class map>
  bool  get_type( const zmap::place_t<map>& place )
  {  return place.owner.get_typ.get_type( place.refer );  }

  template <class map>
  bool  is_array( const zmap::place_t<map>& place )
  {
    auto  zt = get_type( place );

    return zt >= zval::z_array_byte
        && zt <= zval::z_array_uuid;
  }

 /*
  *  zmap::ztree_t inline implementation
  */

  inline
  size_t  zmap::ztree_t::GetBufLen() const
  {
    auto  lbytes = plain_ctl_bytes();
    auto  buflen = ::GetBufLen( lbytes );

    if ( (lbytes & 0x0200) != 0 )
    {
      assert( pvalue != nullptr );
      buflen += 1 + pvalue->GetBufLen();
    }

    if ( (lbytes & 0x0400) != 0 )
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
    auto  lbytes = plain_ctl_bytes();

    o = ::Serialize( o, lbytes );

    if ( (lbytes & 0x0200) != 0 )
    {
      assert( pvalue != nullptr );
      o = pvalue->Serialize( ::Serialize( o, keyset ) );
    }

    if ( (lbytes & 0x0400) != 0 )
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
    word32_t  lfetch;

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

      for ( auto  size = fragment_len( lfetch ); size-- > 0; pbeg = &pbeg->back() )
      {
        byte_t  chnext;

        if ( (s = ::FetchFrom( s, (char&)chnext )) == nullptr )
          return nullptr;
        pbeg->push_back( ztree_t( chnext ) );
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

        if ( (s = ::FetchFrom( ::FetchFrom( s, (char&)chnext ), sublen )) != nullptr )
        {
          using reader_t = typename std::conditional<std::is_same<S, pmr::storage>::value,
            storage<S>, limiter<S>>::type;

          push_back( ztree_t( chnext ) );
            s = reader_t( s, sublen )( back(), n );
        }
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
    if ( p_data != nullptr && p_data->detach() == 0 )
      delete p_data;
    (p_data = new zdata_t())->attach();

    return p_data->FetchFrom( s, p_data->n_vals );
  }

  template <class S>
  S*  zmap::SkipToEnd( S* s )
  {
    size_t  lfetch;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
    {
      byte  keyset;

      if ( (s = zval::SkipToEnd( ::FetchFrom( s, keyset ) )) == nullptr )
        return nullptr;
    }

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
      return (s = ::SkipBytes( s, fragment_len( lfetch ) )) != nullptr ? SkipToEnd( s ) : nullptr;

    for ( auto arrlen = lfetch & 0x1ff; arrlen-- > 0; )
    {
      unsigned  sublen;
      byte_t    chnext;

      if ( (s = ::FetchFrom( ::FetchFrom( s, (char&)chnext ), sublen )) == nullptr )
        return nullptr;
      if ( (s = ::SkipBytes( s, sublen )) == nullptr )
        return nullptr;
    }
    return s;
  }

  /* zmap::iterator_base inline implementation */

  template <class value, class z_iterator>
  zmap::iterator_base<value, z_iterator>::iterator_base( iterator_base&& it ):
    zstack( std::move( it.zstack ) ),
    keybuf( std::move( it.keybuf ) ) {  init();  }

  template <class value, class z_iterator>
  zmap::iterator_base<value, z_iterator>::iterator_base( const iterator_base& it ):
    zstack( it.zstack ),
    keybuf( it.keybuf ) {  init();  }

  template <class value, class z_iterator>
  zmap::iterator_base<value, z_iterator>::iterator_base( z_iterator beg, z_iterator end )
    {
      if ( beg != end )
      {
        zstack.push_back( { beg, end } );
        keybuf.push_back( beg->chnode );
        find();
      }
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator -> () const -> const value*
    {
      if ( &zvalue.second == zvalue.null_value() )
        throw std::invalid_argument( "invalid call to zmap::iterator_base<value, z_iterator>::operator ->" );
      return &zvalue;
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::operator * () const -> const value&
    {
      if ( &zvalue.second == zvalue.null_value() )
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
        new( &zvalue )  value( key( last().beg->keyset, (const uint8_t*)keybuf.data(), keybuf.size() ), last().beg->pvalue.get() );
      else
        new( &zvalue )  value();

      return *this;
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::find() -> iterator_base&
    {
      assert( zstack.size() != 0 );

      while ( !last().empty() && !last().beg->empty() && last().beg->pvalue == nullptr )
        down( last().beg++ );

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
        if ( !last().empty() && !last().beg->empty() )
        {
          do  down( last().beg++ );
            while ( !last().empty() && !last().beg->empty() && last().beg->pvalue == nullptr );

          if ( !last().empty() && last().beg->pvalue != nullptr )  return init();
            else continue;
        }

      // вложенных в текущий элементов нет;
        assert( last().empty() || last().beg->empty() );

      // проверить, не последний ли это был элемент на данном уровне и, если он был последним, откатиться
      // вверх по дереву до первого не-последнего элемента
        if ( last().empty() || ++last().beg == last().end )
        {
          do  back();
            while ( zstack.size() != 0 && last().empty() );
        }

      // элемент был не последним; заместить последний символ поискового ключа на текущий и повторить
      // алгоритм с возможным заходом по дереву
        if ( zstack.size() != 0 )
        {
          keybuf.back() = last().beg->chnode;

          if ( last().beg->pvalue != nullptr )
            return init();
        }
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

      zstack.push_back( { beg, end } );
      keybuf.push_back( beg->chnode );
    }

  template <class value, class z_iterator>
  void  zmap::iterator_base<value, z_iterator>::back()
    {
      zstack.pop_back();
      keybuf.pop_back();
    }

  /* zmap::place_t implementation */

  template <class map>
  bool  zmap::place_t<map>::operator == ( const zval& z ) const
    {
      auto  pval = owner.get( refer );

      return pval != nullptr && *pval == z;
    }

  template <class map>
  bool  zmap::place_t<map>::operator < ( const zval& z ) const
    {
      auto  pval = owner.get( refer );

      return pval != nullptr && *pval < z;
    }

  template <class map>
  bool  zmap::place_t<map>::operator <= ( const zval& z ) const
    {
      auto  pval = owner.get( refer );

      return pval != nullptr && *pval <= z;
    }

  template <class map>
  bool  zmap::place_t<map>::operator > ( const zval& z ) const
    {
      auto  pval = owner.get( refer );

      return pval != nullptr && *pval > z;
    }

  template <class map>
  bool  zmap::place_t<map>::operator >= ( const zval& z ) const
    {
      auto  pval = owner.get( refer );

      return pval != nullptr && *pval >= z;
    }

  /* zmap::serial getters */

  template <class S>
  S*  zmap::serial::find( S* s, const byte_t* k, size_t l, unsigned t )
  {
    word32_t  lfetch;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
    {
      byte  keyset;

      if ( (s = ::FetchFrom( s, keyset )) == nullptr )
        return nullptr;
      if ( l == 0 ) return keyset == t ? s : nullptr;
        else s = skip::zval( s );
    }
      else
    if ( l == 0 )
      return nullptr;

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
    {
      auto    patlen = fragment_len( lfetch );
      byte_t  chload;

      while ( patlen-- > 0 && l > 0 )
      {
        if ( (s = ::FetchFrom( s, (char&)chload )) != nullptr && chload == *k++ ) --l;
          else return nullptr;
      }

      return patlen == (decltype(patlen))-1 ? find( s, k, l, t ) : nullptr;
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

        if ( chfind == chnext ) return find( s, k, l - 1, t );
          else
        if ( chfind <  chnext ) return nullptr;
          else
        if ( (s = skip::size( s, sublen )) == nullptr )
          return nullptr;
      }
      return nullptr;
    }
  }

  template <class S>
  S*  zmap::serial::find( S* s, const key& k )
  {
    return find( s, k.data(), k.size(), k.type() );
  }

  template <class S>
  S*  zmap::serial::load( zval& o, S* s, const key& k )
  {
    auto  p = find( s, k );

    return p != nullptr && (p = o.FetchFrom( p )) != nullptr ? p : nullptr;
  }

  template <class S>
  zval  zmap::serial::load( S* s, const key& k )
  {
    zval  v;

    return load( v, s, k ) != nullptr ? std::move( v ) : zval();
  }

}

template <class T1, class T2, class T3>
bool  operator == ( const mtc::zval::dump::array_t<T1, T2>& _1, const std::vector<T3>& _2 )
{
  if ( _1.size() != _2.size() ) return false;
    else
  {
    auto  i1 = _1.begin();
    auto  i2 = _2.begin();

    while ( i1 != _1.end() )
      if ( *i1++ != *i2++ ) return false;

    return true;
  }
}

inline
bool  operator == ( const mtc::zval& _1, const mtc::zval::dump& _2 )
  {  return _2 == _1;  }

inline
bool  operator == ( const mtc::zmap& _1, const mtc::zmap::dump& _2 )
  {  return _2 == _1;  }

template <class T1, class T2, class T3>
bool  operator == ( const std::vector<T1>& _1, const mtc::zval::dump::array_t<T2, T3>& _2 )
  {  return _2 == _1;  }

# endif  // __zmap_hpp__
