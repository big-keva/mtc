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

}

template <>  inline
auto  GetBufLen( const mtc::zval& ) -> size_t;
template <>  inline
auto  GetBufLen( const mtc::zmap& ) -> size_t;

template <class O>  inline
auto  Serialize( O*, const mtc::zval& ) -> O*;
template <class O>  inline
auto  Serialize( O*, const mtc::zmap& ) -> O*;

template <class S>  inline
auto  FetchFrom( S*, mtc::zval& ) -> S*;
template <class S>  inline
auto  FetchFrom( S*, mtc::zmap& ) -> S*;

namespace mtc
{
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
    zval( const uuid_t& );

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

  public:
    enum: unsigned
    {
      compare_lt = 0x01,
      compare_gt = 0x02,
      compare_eq = 0x04,
      compare_le = 0x05,
      compare_ge = 0x06
    };

  protected:  // compare helpers
    template <class B>
    auto  CompTo( const B& ) const -> unsigned;

  public:
    auto  CompTo( const zval& x ) const -> unsigned;

    bool  lt( const zval& z ) const {  return CompTo( z ) == compare_lt;  }
    bool  gt( const zval& z ) const {  return CompTo( z ) == compare_gt;  }
    bool  eq( const zval& z ) const {  return CompTo( z ) == compare_eq;  }
    bool  le( const zval& z ) const {  return (CompTo( z ) & compare_le) != 0;  }
    bool  ge( const zval& z ) const {  return (CompTo( z ) & compare_ge) != 0;  }
    bool  ne( const zval& z ) const {  return !eq( z );  }

  public:     // compare
    bool  operator == ( const zval& v ) const;
    bool  operator != ( const zval& v ) const { return !(*this == v); }

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
        std::string out;

        while ( *v != 0 ) out += (char)*v++;

        return out;
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
    static
    const char* type_name( z_type type );
    std::string to_string() const;

  protected:  // helpers
    auto  fetch( zval&& ) -> zval&;
    auto  fetch( const zval& ) -> zval&;

    auto  inner() const -> const inner_t&;
    auto  inner()       ->       inner_t&;

  protected:  // inplace storage
    char    storage[impl::align<impl::get_max_size<uint64_t, double,
      charstr,
      widestr,
      std::vector<uint64_t>,
      std::vector<widestr>>::value>::value];
    byte_t  vx_type;

  };

  class zmap
  {
    class ztree_t;
    class zdata_t;
    class zbuff_t;

  public:     // iterator support
    class key;
    class iterator;
    class const_iterator;
    class serial;

  protected:
    template <class value>
    class iterator_data;
    template <class value, class ztree_iterator>
    class iterator_base;

  private:
    template <class map>
    class place_t;

    class const_place_t;
    class patch_place_t;

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
    declare_get_type( zmap    )

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
    declare_get_init( zmap    )

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
  # undef declare_get_init
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
    zdata_t*  p_data = nullptr;

  };

  /*
    serial::*() methods

    static methods allowing direct access to the field of serialized zmap structure

    special fast implementation for 'const char*' source also provided
  */
  class zmap::serial
  {
    template <class S>  static  S*    find( S*, const byte_t*, size_t, unsigned );

  public:     // serial::* family
    class skip;

    template <class S>  static  S*    find(        S*, const key& );
    template <class S>  static  S*    load( zval&, S*, const key& );
    template <class S>  static  zval  load(        S*, const key& );
  };

  class zmap::serial::skip
  {
    template <class T,
              class S>  static  S*  array_vals( S* );
    template <class C,
              class S>  static  S*  array_strs( S* );
    template <class S>  static  S*  array_zmap( S* );
    template <class S>  static  S*  array_zval( S* );

  public:
    template <class S>  static  auto  size( S*, size_t ) -> S*;
    template <class S>  static  auto  zval( S*         ) -> S*;
    template <class S>  static  auto  zmap( S*         ) -> S*;

  public:
                        static  auto  size( const char*, size_t ) -> const char*;
                        static  auto  zval( const char*         ) -> const char*;
                        static  auto  zmap( const char*         ) -> const char*;
  };

  inline  std::string to_string( const zval& z ) {  return std::move( z.to_string() );  }
          std::string to_string( const zmap::key& );

}

template <> inline  size_t  GetBufLen( const mtc::zval& v ) {  return v.GetBufLen();  }
template <> inline  size_t  GetBufLen( const mtc::zmap& m ) {  return m.GetBufLen();  }

template <class O>
O*      Serialize( O* o, const mtc::zval& v ) {  return v.Serialize( o );  }
template <class O>
O*      Serialize( O* o, const mtc::zmap& m ) {  return m.Serialize( o );  }

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
    static  auto  plain_ctl_bytes( word32_t encode ) -> size_t;

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

  public:
    static
    auto  null() -> key {  return key();  }

  public: // data
    auto  type() const  -> unsigned       {  return _typ;  }
    auto  data() const  -> const uint8_t* {  return _ptr;  }
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

  template <class map>
  class zmap::place_t
  {
  public:
    place_t( const key& k, map& m ): refer( k ), owner( m ) {}
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
    map&  owner;
    
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

  };

  /*
    zmap::ztree_t inline implementation
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

      for ( auto  size = plain_ctl_bytes( lfetch ); size-- > 0; pbeg = &pbeg->back() )
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
    if ( p_data != nullptr )
      p_data->detach();
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
      keybuf( it.keybuf )
    {}

  template <class value, class z_iterator>
  zmap::iterator_base<value, z_iterator>::iterator_base( z_iterator beg, z_iterator end )
    {
      if ( beg != end )
      {
        zstack.push_back( std::make_pair( beg, end ) );
        keybuf.push_back( beg->chnode );
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
        new( &zvalue )  value( key( last().first->keyset, (const uint8_t*)keybuf.data(), keybuf.size() ), last().first->pvalue.get() );
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
          {
            keybuf.back() = last().first->chnode;

            if ( last().first->pvalue != nullptr )
              return init();
          }

          continue;
        }

      // элемент был не последним; заместить последний символ поискового ключа на текущий и повторить
      // алгоритм с возможным заходом по дереву
        keybuf.back() = last().first->chnode;

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
      keybuf.push_back( beg->chnode );
    }

  template <class value, class z_iterator>
  void  zmap::iterator_base<value, z_iterator>::back()
    {
      zstack.pop_back();
      keybuf.pop_back();
    }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::last() -> zpos& {  return zstack.back();  }

  template <class value, class z_iterator>
  auto  zmap::iterator_base<value, z_iterator>::last() const -> const zpos& {  return zstack.back();  }

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

  /* zmap::serial::skip implementation */

  template <class T, class S>
  S*  zmap::serial::skip::array_vals( S* s )
  {
    int   nitems;
    T     itnext;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itnext )) != nullptr )
      (void)0;
    return s;
  }

  template <class C, class S>
  S*  zmap::serial::skip::array_strs( S* s )
  {
    int     nitems;
    size_t  itelen;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itelen )) != nullptr && (s = size( s, sizeof(C) * itelen )) != nullptr )
      (void)0;
    return s;
  }

  template <class S>
  S*  zmap::serial::skip::array_zmap( S* s )
  {
    int     nitems;
    size_t  itelen;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itelen )) != nullptr && (s = skip::zmap( s )) != nullptr )
      (void)0;
    return s;
  }

  template <class S>
  S*  zmap::serial::skip::array_zval( S* s )
  {
    int     nitems;
    size_t  itelen;

    if ( (s = ::FetchFrom( s, nitems )) == nullptr )
      return nullptr;
    while ( nitems-- > 0 && (s = ::FetchFrom( s, itelen )) != nullptr && (s = skip::zval( s )) != nullptr )
      (void)0;
    return s;
  }

  template <class S>  S*  zmap::serial::skip::size( S* s, size_t l )
  {
    char      slocal[0x100];
    unsigned  cbpart;

    while ( (cbpart = l < sizeof(slocal) ? l : sizeof(slocal)) > 0 )
      if ( (s = ::FetchFrom( s, slocal, cbpart )) != nullptr )  l -= cbpart;
        else return nullptr;
    return s;
  }

  template <class S>  S*  zmap::serial::skip::zval( S* s )
  {
    byte_t    vxtype;
    unsigned  sublen;

    if ( (s = ::FetchFrom( s, vxtype )) == nullptr )
      return nullptr;
    switch ( vxtype )
    {
    # define  derive_skip_plain( _type_ ) case zval::z_##_type_: return skip::size( s, sizeof(_type_##_t) );
    # define  derive_skip_smart( _type_ ) case zval::z_##_type_: {  _type_##_t  t;  return ::FetchFrom( s, t ); }
      derive_skip_plain( char )
      derive_skip_plain( byte )
      derive_skip_plain( int16 )
      derive_skip_plain( word16 )

      derive_skip_smart( int32 )
      derive_skip_smart( int64 )
      derive_skip_smart( word32 )
      derive_skip_smart( word64 )
      derive_skip_smart( float )
      derive_skip_smart( double )
    # undef derive_skip_smart
    # undef derive_skip_plain

      case zval::z_charstr: return (s = ::FetchFrom( s, sublen )) != nullptr ? skip::size( s, sublen ) : nullptr;
      case zval::z_widestr: return (s = ::FetchFrom( s, sublen )) != nullptr ? skip::size( s, sizeof(widechar) * sublen ) : nullptr;
      case zval::z_zmap:    return skip::zmap( s );
      case zval::z_uuid:    return skip::size( s, uuid::length );

    # define  derive_skip_array_plain( _type_ ) case zval::z_array_##_type_:  \
        return (s = ::FetchFrom( s, sublen )) != nullptr ? skip::size( s, sublen * sizeof(_type_##_t) ) : nullptr;
      derive_skip_array_plain( char )
      derive_skip_array_plain( byte )
      derive_skip_array_plain( float )
      derive_skip_array_plain( double )
    # undef derive_skip_array_plain

      case zval::z_array_int16:   return array_vals<int16_t>  ( s );
      case zval::z_array_int32:   return array_vals<int32_t>  ( s );
      case zval::z_array_int64:   return array_vals<int64_t>  ( s );
      case zval::z_array_word16:  return array_vals<word16_t> ( s );
      case zval::z_array_word32:  return array_vals<word32_t> ( s );
      case zval::z_array_word64:  return array_vals<word64_t> ( s );
      case zval::z_array_charstr: return array_strs<char>     ( s );
      case zval::z_array_widestr: return array_strs<widechar> ( s );
      case zval::z_array_zmap:    return array_zmap( s );
      case zval::z_array_zval:    return array_zval( s );
      case zval::z_array_uuid:    return array_vals<uuid_t>   ( s );
      case zval::z_untyped:       return s;
      default:
        return nullptr;
    }
  }

  template <class S>  S*  zmap::serial::skip::zmap( S* s )
  {
    word32_t  lfetch;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
    {
      char  keyset;

      if ( (s = skip::zval( ::FetchFrom( s, keyset ) )) == nullptr )
        return nullptr;
    }

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
      return (s = skip::size( s, ztree_t::plain_ctl_bytes( lfetch ) )) != nullptr ? skip::zmap( s ) : nullptr;

    for ( auto arrlen = lfetch & 0x1ff; arrlen-- > 0; )
    {
      unsigned  sublen;
      byte_t    chnext;

      if ( (s = ::FetchFrom( ::FetchFrom( s, (char&)chnext ), sublen )) == nullptr )
        return nullptr;
      if ( (s = skip::size( s, sublen )) == nullptr )
        return nullptr;
    }
    return s;
  }

  /* zmap::serial::skip* secialization for 'const char*' */

  inline  const char* zmap::serial::skip::size( const char* s, size_t l )
  {
    return s + l;
  }

  inline  const char* zmap::serial::skip::zval( const char* s )
  {
    auto    vatype = (unsigned char)*s++;
    size_t  sublen;

    switch ( vatype )
    {
      case zval::z_char:    return s + sizeof(char);
      case zval::z_byte:    return s + sizeof(byte_t);
      case zval::z_int16:   return s + sizeof(int16_t);
      case zval::z_word16:  return s + sizeof(word16_t);
      case zval::z_float:   return s + sizeof(float);
      case zval::z_double:  return s + sizeof(double);

      case zval::z_int32:
      case zval::z_int64:
      case zval::z_word32:
      case zval::z_word64:  while ( *s++ & 0x80 ) (void)0;  return s;

      case zval::z_charstr: return ::FetchFrom( s, sublen ) + sublen;
      case zval::z_widestr: return ::FetchFrom( s, sublen ) + sizeof(widechar) * sublen;
      case zval::z_zmap:    return skip::zmap( s );

      case zval::z_array_char:    return ::FetchFrom( s, sublen ) + sublen;
      case zval::z_array_byte:    return ::FetchFrom( s, sublen ) + sublen;
      case zval::z_array_float:   return ::FetchFrom( s, sublen ) + sublen * sizeof(float);
      case zval::z_array_double:  return ::FetchFrom( s, sublen ) + sublen * sizeof(double);

      case zval::z_array_int16:   return array_vals<int16_t>( s );
      case zval::z_array_int32:   return array_vals<int32_t>( s );
      case zval::z_array_int64:   return array_vals<int64_t>( s );
      case zval::z_array_word16:  return array_vals<word16_t>( s );
      case zval::z_array_word32:  return array_vals<word32_t>( s );
      case zval::z_array_word64:  return array_vals<word64_t>( s );

      case zval::z_array_charstr: return array_strs<char>( s );
      case zval::z_array_widestr: return array_strs<widechar>( s );
      case zval::z_array_zmap:    return array_zmap( s );
      case zval::z_array_zval:    return array_zval( s );
      case zval::z_untyped:       return s;
      default:
        return nullptr;
    }
  }

  inline  const char* zmap::serial::skip::zmap( const char* s )
  {
    word32_t  lfetch;
    size_t    sublen;

  /* get control length */
    s = ::FetchFrom( s, lfetch );

  /*  check if value is stored before other data; either skip or return */
    if ( (lfetch & 0x0200) != 0 )
      s = skip::zval( ++s );

  /*  check if branch is patricia-like: check exact match               */
    if ( (lfetch & 0x0400) != 0 )
      return skip::zmap( s + ztree_t::plain_ctl_bytes( lfetch ) );

    for ( auto arrlen = lfetch & 0x1ff; arrlen-- > 0; s += sublen )
      s = ::FetchFrom( ++s, sublen );

    return s;
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
      char  keyset;

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
      auto    patlen = ztree_t::plain_ctl_bytes( lfetch );
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

# endif  // __zmap_hpp__
