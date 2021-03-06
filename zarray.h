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
# if !defined( __zarray_h__ )
# define __zarray_h__
# include "serialize.h"
# include "serializex.h"
# include "platform.h"
# include "autoptr.h"
# include "wcsstr.h"
# include "array.h"
# include <stdexcept>
# include <cassert>
# include <string>
# include <atomic>

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 )
#   pragma warning( disable: 4702 )
#   pragma warning( disable: 4710 )
#   pragma warning( disable: 4456 )
#   pragma warning( disable: 4996 )
# endif  // _MSC_VER

namespace mtc
{
/*
  supported variable types by id
*/
  enum
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
    z_zarray  = 19,

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
    z_array_zarray  = 51,
    z_array_xvalue  = 52
  };

  inline  bool  z_is_integer_type( const unsigned zt )  {  return zt >= z_char && zt <= z_word64;   }
  inline  bool  z_is_float_type  ( const unsigned zt )  {  return zt == z_float || zt == z_double;  }
  inline  bool  z_is_numeric_type( const unsigned zt )  {  return z_is_integer_type( zt ) || z_is_float_type( zt );  }
  inline  bool  z_is_string_type( const unsigned zt )   {  return zt == z_charstr || zt == z_widestr;   }

  template <class M = def_alloc>  class zarray;
  template <class M = def_alloc>  class xvalue;

  typedef xvalue<>  XValue;
  typedef zarray<>  ZArray;

  template <class M>  std::string to_string( const xvalue<M>& );
  template <class M>  std::string to_string( const zarray<M>& );

  /*
    xvalue handles any data in local buffer; data may be accessed by accessor methods
  */
  template <class M>
  class xvalue
  {
    mutable M malloc;
    byte_t    vxtype;

    template <class X>
    friend class zarray;

    using array_char   = array<char_t, M>;
    using array_byte   = array<byte_t, M>;
    using array_int16  = array<int16_t, M>;
    using array_word16 = array<word16_t, M>;
    using array_int32  = array<int32_t, M>;
    using array_word32 = array<word32_t, M>;
    using array_int64  = array<int64_t, M>;
    using array_word64 = array<word64_t, M>;
    using array_float  = array<float_t, M>;
    using array_double = array<double_t, M>;

    using array_charstr = array<_auto_<char, M>, M>;
    using array_widestr = array<_auto_<widechar, M>, M>;
    using array_zarray  = array<zarray<M>, M>;
    using array_xvalue  = array<xvalue<M>, M>;

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
    using array_zarray_t  = array_zarray;
    using array_xvalue_t  = array_xvalue;

    union
    {
    # define derive_var( _type_ ) _type_##_t  v_##_type_;
      derive_var( char    )
      derive_var( byte    )
      derive_var( int16   )
      derive_var( word16  )
      derive_var( int32   )
      derive_var( word32  )
      derive_var( int64   )
      derive_var( word64  )
      derive_var( float   )
      derive_var( double  )

      char*       v_charstr;
      widechar*   v_widestr;
//      buffer    v_buffer;
      zarray<M>   v_zarray;

      derive_var( array_char    )
      derive_var( array_byte    )
      derive_var( array_int16   )
      derive_var( array_word16  )
      derive_var( array_int32   )
      derive_var( array_word32  )
      derive_var( array_int64   )
      derive_var( array_word64  )
      derive_var( array_float   )
      derive_var( array_double  )

      derive_var( array_charstr   )
      derive_var( array_widestr  )
      derive_var( array_zarray   )
      derive_var( array_xvalue  )
    # undef derive_var
    };

    template <class C, class... args>
    C*   construct( C* p, args... aset )  {  return new( p ) C( aset... );  }
    template <class C>
    void destruct ( C* p )                {  if ( p != nullptr )  p->~C();  }

  public:             // allocator
    M&    GetAllocator() const        {  return malloc;     }
    M&    SetAllocator( const M& m )  {  return malloc = m; }

  public:     // untyped constant
    enum {  undefined_type = 0xff  };

  public:     // untyped element construction
    xvalue();
    xvalue( M& );
    xvalue( const xvalue& );
   ~xvalue();
    xvalue& operator = ( const xvalue& );

    unsigned  gettype() const
      {
        return vxtype;
      }
    xvalue& reset()
      {
        delete_data();
        return *this;
      }
    xvalue  copy() const
      {
        xvalue  o;

        o.SetAllocator( GetAllocator() );

        switch ( vxtype )
        {
          case z_char:    o.set_char( *get_char() );      return o;
          case z_byte:    o.set_byte( *get_byte() );      return o;
          case z_int16:   o.set_int16( *get_int16() );    return o;
          case z_int32:   o.set_int32( *get_int32() );    return o;
          case z_int64:   o.set_int64( *get_int64() );    return o;
          case z_float:   o.set_float( *get_float() );    return o;
          case z_word16:  o.set_word16( *get_word16() );  return o;
          case z_word32:  o.set_word32( *get_word32() );  return o;
          case z_word64:  o.set_word64( *get_word64() );  return o;
          case z_double:  o.set_double( *get_double() );  return o;

          case z_charstr: return o.set_charstr( get_charstr() ) != nullptr ? o : xvalue( GetAllocator() );
          case z_widestr: return o.set_widestr( get_widestr() ) != nullptr ? o : xvalue( GetAllocator() );
//          case z_buffer  = 18,
          case z_zarray:  return o.set_zarray( *get_zarray()  ) != nullptr ? o : xvalue( GetAllocator() );

          case z_array_char:    return o.set_array_char()->Append( *get_array_char() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_byte:    return o.set_array_byte()->Append( *get_array_byte() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_int16:   return o.set_array_int16()->Append( *get_array_int16() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_int32:   return o.set_array_int32()->Append( *get_array_int32() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_int64:   return o.set_array_int64()->Append( *get_array_int64() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_word16:  return o.set_array_word16()->Append( *get_array_word16() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_word32:  return o.set_array_word32()->Append( *get_array_word32() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_word64:  return o.set_array_word64()->Append( *get_array_word64() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_float:   return o.set_array_float ()->Append( *get_array_float () ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_double:  return o.set_array_double()->Append( *get_array_double() ) == 0 ? o : xvalue( GetAllocator() );
          case z_array_zarray:  return o.set_array_zarray()->Append( *get_array_zarray() ) == 0 ? o : xvalue( GetAllocator() );

          case z_array_charstr:
            {
              auto& a = *o.set_array_charstr();
              auto& s = *  get_array_charstr();

              for ( auto str: s )
              {
                _auto_<char, M> p( w_strdup( a.GetAllocator(), str ) );

                if ( p == nullptr || a.Append( static_cast<_auto_<char, M>&&>( p ) ) != 0 )
                  return xvalue();
              }
              
              return o;
            }
          case z_array_widestr:
            {
              auto& a = *o.set_array_widestr();
              auto& s = *  get_array_widestr();

              for ( auto str: s )
              {
                _auto_<widechar, M> p( w_strdup( a.GetAllocator(), str ) );
                
                if ( p == nullptr || a.Append( static_cast<_auto_<widechar, M>&&>( p ) ) != 0 )
                  return xvalue();
              }

              return o;
            }
//          z_array_buffer  = 50,
          case z_array_xvalue:
            {
              auto& a = *o.set_array_xvalue();
              auto& s = *  get_array_xvalue();

              for ( auto x: s )
              {
                xvalue  c( x.copy() );

                if ( c.gettype() == undefined_type || a.Append( c ) != 0 )
                  return xvalue();
              }

              return o;
            }
          default:  break;
        }
        return xvalue( GetAllocator() );
      }

  public:     // typed constructors
    xvalue( char c ):     vxtype( undefined_type )          {  set_char( c );   }
    xvalue( byte_t b ):   vxtype( undefined_type )          {  set_byte( b );   }
    xvalue( int16_t i ):  vxtype( undefined_type )          {  set_int16( i );  }
    xvalue( int32_t i ):  vxtype( undefined_type )          {  set_int32( i );  }
    xvalue( int64_t i ):  vxtype( undefined_type )          {  set_int64( i );  }
    xvalue( word16_t i ): vxtype( undefined_type )          {  set_word16( i ); }
    xvalue( word32_t i ): vxtype( undefined_type )          {  set_word32( i ); }
    xvalue( word64_t i ): vxtype( undefined_type )          {  set_word64( i ); }
    xvalue( float   f ):  vxtype( undefined_type )          {  set_float( f );  }
    xvalue( double d ):   vxtype( undefined_type )          {  set_double( d ); }
    xvalue( const zarray<M>& z ): vxtype( undefined_type )  {  set_zarray( z ); }
    xvalue( const char* s, size_t l = (size_t)-1 ): vxtype( undefined_type ) {  set_charstr( s, l ); }
    xvalue( const widechar* s, size_t l = (size_t)-1 ): vxtype( undefined_type ) {  set_widestr( s, l ); }

  public:     // serialization
                        size_t  GetBufLen(    ) const;
    template <class O>  O*      Serialize( O* ) const;
    template <class S>  S*      FetchFrom( S* );

  public:     // get_? methods
/* ordinal types */
  # define  derive_get( _type_ )                                                                                \
    auto  get_##_type_() const  -> const _type_##_t* {  return vxtype == z_##_type_ ? &v_##_type_ : nullptr;  } \
    auto  get_##_type_()        ->       _type_##_t* {  return vxtype == z_##_type_ ? &v_##_type_ : nullptr;  }

    derive_get( char )
    derive_get( byte )
    derive_get( int16 )
    derive_get( word16 )
    derive_get( int32 )
    derive_get( word32 )
    derive_get( int64 )
    derive_get( word64 )
    derive_get( float )
    derive_get( double )

/* arrays */
    derive_get( array_char )
    derive_get( array_byte )
    derive_get( array_int16 )
    derive_get( array_int32 )
    derive_get( array_word16 )
    derive_get( array_word32 )
    derive_get( array_int64 )
    derive_get( array_word64 )
    derive_get( array_float )
    derive_get( array_double )
    derive_get( array_charstr )
    derive_get( array_widestr )
    derive_get( array_zarray )
    derive_get( array_xvalue )
  # undef derive_get

/* zarray */
    auto  get_zarray() const -> const zarray<>* {  return vxtype == z_zarray ? &v_zarray : nullptr;  }
    auto  get_zarray()       ->       zarray<>* {  return vxtype == z_zarray ? &v_zarray : nullptr;  }

/* regular strings  */
    auto  get_charstr() const ->  const char*     {  return vxtype == z_charstr ? v_charstr : nullptr;  }
    auto  get_charstr()       ->        char*     {  return vxtype == z_charstr ? v_charstr : nullptr;  }
    auto  get_widestr() const ->  const widechar* {  return vxtype == z_widestr ? v_widestr : nullptr;  }
    auto  get_widestr()       ->        widechar* {  return vxtype == z_widestr ? v_widestr : nullptr;  }

    auto  get_holder() const  ->  const void*   {  return &v_charstr;  }
    auto  get_holder()        ->        void*   {  return &v_charstr;  }

public:     // set_?? methods
/* ordinal types */
  # define  derive_set( _type_ )                            \
    auto  set_##_type_( _type_##_t v = 0 )  -> _type_##_t*  \
      {                                                     \
        delete_data();  vxtype = z_##_type_;                \
        return construct( &v_##_type_, v );                 \
      }

    derive_set( char )
    derive_set( byte )
    derive_set( int16 )
    derive_set( word16 )
    derive_set( int32 )
    derive_set( word32 )
    derive_set( int64 )
    derive_set( word64 )
    derive_set( float )
    derive_set( double )

  # undef derive_set

/* set_array_#() macrogeneration  */
  # define derive_set( _type_ )                 \
    auto  set_##_type_() -> _type_##_t*         \
      {                                         \
        delete_data();  vxtype = z_##_type_;    \
        return construct( &v_##_type_ );        \
      }

    derive_set( array_char )
    derive_set( array_byte )
    derive_set( array_int16 )
    derive_set( array_word16 )
    derive_set( array_int32 )
    derive_set( array_word32 )
    derive_set( array_int64 )
    derive_set( array_word64 )
    derive_set( array_float )
    derive_set( array_double )

    derive_set( array_charstr )
    derive_set( array_widestr )
    derive_set( array_zarray )
    derive_set( array_xvalue )

  # undef derive_set

/* regular strings  */
    char*     set_charstr( const char*  pszstr, size_t  cchstr = (size_t)-1 )
      {
        if ( cchstr == (size_t)-1 )
          cchstr = pszstr != nullptr ? w_strlen( pszstr ) : 0;

        delete_data();

        if ( (v_charstr = (char*)GetAllocator().alloc( cchstr + 1 )) == nullptr )
          return nullptr;

        if ( pszstr != nullptr )
          memcpy( v_charstr, pszstr, cchstr );

        v_charstr[cchstr] = '\0';
          vxtype = z_charstr;

        return v_charstr;
      }
    widechar* set_widestr( const widechar*  pszstr, size_t  cchstr = (size_t)-1 )
      {
        if ( cchstr == (size_t)-1 )
          cchstr = pszstr != nullptr ? w_strlen( pszstr ) : 0;

        delete_data();

        if ( (v_widestr = (widechar*)GetAllocator().alloc( sizeof(widechar) * (cchstr + 1) )) == nullptr )
          return nullptr;

        if ( pszstr != nullptr )
          memcpy( v_widestr, pszstr, sizeof(widechar) * cchstr );

        v_widestr[cchstr] = 0;
          vxtype = z_widestr;

        return v_widestr;
      }
    char*&  set_charstr()
      {  delete_data();  vxtype = z_charstr;  return v_charstr = nullptr;  }
    widechar*&  set_widestr()
      {  delete_data();  vxtype = z_widestr;  return v_widestr = nullptr;  }

/* special types: buffer, zarray and array(s) */
    zarray<M>*  set_zarray( M& m )
      {  delete_data();  vxtype = z_zarray;   return construct( &v_zarray, m ); }
    zarray<M>*  set_zarray( const zarray<M>& z = zarray<M>() )
      {  delete_data();  vxtype = z_zarray;   return construct( &v_zarray, z ); }

  protected:  // arithmetic helpers
    template <class A, class B> static  xvalue  GetMul( A a, B b )  {  return a * b;  }
    template <class A, class B> static  xvalue  GetDiv( A a, B b )  {  return a / b;  }
    template <class A, class B> static  xvalue  GetAdd( A a, B b )  {  return a + b;  }
    template <class A, class B> static  xvalue  GetSub( A a, B b )  {  return a - b;  }

    template <class A, class B> static  xvalue  GetAnd( A a, B b )  {  return a & b;  }
    template <class A, class B> static  xvalue  GetXor( A a, B b )  {  return a ^ b;  }
    template <class A, class B> static  xvalue  Get_Or( A a, B b )  {  return a | b;  }
    template <class A, class B> static  xvalue  GetPct( A a, B b )  {  return a % b;  }
    template <class A, class B> static  xvalue  GetShl( A a, B b )  {  return a << b;  }
    template <class A, class B> static  xvalue  GetShr( A a, B b )  {  return a >> b;  }

    /*
      операции над xvalue и целым или дробным значением - макрогенерация:
      template <class V> xvalue  (#)( xvalue, V )
    */
    # define  derive_operation_xvalue_value( funcname )                     \
    template <class V>  xvalue  funcname( V v ) const                       \
      {                                                                     \
        switch ( gettype() )                                                \
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
          default:        return xvalue();                                  \
        }                                                                   \
      }
      derive_operation_xvalue_value( GetMul )
      derive_operation_xvalue_value( GetDiv )
      derive_operation_xvalue_value( GetAdd )
      derive_operation_xvalue_value( GetSub )
    # undef  derive_operation_xvalue_value

    /*
      операции над xvalue и xvalue - макрогенерация:
      xvalue  (#)( const xvalue&, const xvalue& )
    */
    # define  derive_operation_xvalue_xvalue( funcname )                    \
    template <class Z>  xvalue  funcname( const xvalue<Z>& x ) const        \
      {                                                                     \
        switch ( x.gettype() )                                              \
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
          default:        return xvalue();                                  \
        }                                                                   \
      }
      derive_operation_xvalue_xvalue( GetMul )
      derive_operation_xvalue_xvalue( GetDiv )
      derive_operation_xvalue_xvalue( GetSub )
    # undef derive_operation_xvalue_xvalue

    /*
      специализация GetAdd с поддержкой суммирования однотипных строк
    */
    template <class A>
    static  xvalue  StrCat( A, const char* )      {  return xvalue();  }
    template <class A>
    static  xvalue  StrCat( A, const widechar* )  {  return xvalue();  }
    static  xvalue  StrCat( const char* a, const char* b )
      {
        xvalue  o;

        a = a != nullptr ? a : "";
        b = b != nullptr ? b : "";
        if ( o.set_charstr( nullptr, w_strlen( a ) + w_strlen( b ) ) == nullptr )
          return xvalue();
        w_strcat( w_strcpy( o.get_charstr(), a ), b );
          return o;
      }
    static  xvalue  StrCat( const widechar* a, const widechar* b )
      {
        widechar  z = 0;
        xvalue    o;

        a = a != nullptr ? a : &z;
        b = b != nullptr ? b : &z;

        if ( o.set_widestr( nullptr, w_strlen( a ) + w_strlen( b ) ) == nullptr )
          return xvalue();
        w_strcat( w_strcpy( o.get_widestr(), a ), b );
          return o;
      }
    static  xvalue  StrCat( const char*, const widechar* )  {  return xvalue();  }
    static  xvalue  StrCat( const widechar*, const char* )  {  return xvalue();  }

    template <class V>  xvalue  StrCat( V v ) const
      {
        switch ( gettype() )
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
          case z_charstr: return StrCat( get_charstr(), v );
          case z_widestr: return StrCat( get_widestr(), v );
          default:        return xvalue();
        }
      }
    template <class Z>  xvalue  GetAdd( const xvalue<Z>& x ) const
      {
        switch ( x.gettype() )
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
          case z_charstr: return StrCat( x.get_charstr() );
          case z_widestr: return StrCat( x.get_widestr() );
          default:        return xvalue();
        }
      }

    /*
      макрогенерация битовых операций над целочисленными значениями
    */
    # define  derive_math( funcname )                                       \
    template <class V>  xvalue  funcname( V v ) const                       \
      {                                                                     \
        switch ( gettype() )                                                \
        {                                                                   \
          case z_char:    return funcname( *get_char(), v );                \
          case z_byte:    return funcname( *get_byte(), v );                \
          case z_int16:   return funcname( *get_int16(), v );               \
          case z_int32:   return funcname( *get_int32(), v );               \
          case z_int64:   return funcname( *get_int64(), v );               \
          case z_word16:  return funcname( *get_word16(), v );              \
          case z_word32:  return funcname( *get_word32(), v );              \
          case z_word64:  return funcname( *get_word64(), v );              \
          default:        return xvalue();                                  \
        }                                                                   \
      }                                                                     \
    template <class Z>  xvalue  funcname( const xvalue<Z>& x ) const        \
      {                                                                     \
        switch ( x.gettype() )                                              \
        {                                                                   \
          case z_char:    return funcname( *x.get_char() );                 \
          case z_byte:    return funcname( *x.get_byte() );                 \
          case z_int16:   return funcname( *x.get_int16() );                \
          case z_int32:   return funcname( *x.get_int32() );                \
          case z_int64:   return funcname( *x.get_int64() );                \
          case z_word16:  return funcname( *x.get_word16() );               \
          case z_word32:  return funcname( *x.get_word32() );               \
          case z_word64:  return funcname( *x.get_word64() );               \
          default:        return xvalue();                                  \
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
    template <class Z>  xvalue<M> operator * ( const xvalue<Z>& r ) const {  return GetMul( r );  }
    template <class Z>  xvalue<M> operator / ( const xvalue<Z>& r ) const {  return GetDiv( r );  }
    template <class Z>  xvalue<M> operator % ( const xvalue<Z>& r ) const {  return GetPct( r );  }
    template <class Z>  xvalue<M> operator + ( const xvalue<Z>& r ) const {  return GetAdd( r );  }
    template <class Z>  xvalue<M> operator - ( const xvalue<Z>& r ) const {  return GetSub( r );  }
    template <class Z>  xvalue<M> operator << ( const xvalue<Z>& r ) const {  return GetShl( r );  }
    template <class Z>  xvalue<M> operator >> ( const xvalue<Z>& r ) const {  return GetShr( r );  }
    template <class Z>  xvalue<M> operator & ( const xvalue<Z>& r ) const {  return GetAnd( r );  }
    template <class Z>  xvalue<M> operator ^ ( const xvalue<Z>& r ) const {  return GetXor( r );  }
    template <class Z>  xvalue<M> operator | ( const xvalue<Z>& r ) const {  return Get_Or( r );  }

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
    # define  derive_strcmp( c1, c2 )                             \
    static int CompTo( const c1* a, const c2* b )                 \
      {                                                           \
        int    rc = w_strcmp( a, b );                             \
        return rc < 0 ? 0x01 + 0x08 : rc > 0 ? 0x02 + 0x08 : 0x04;\
      }
      derive_strcmp( char, char )
      derive_strcmp( char, widechar )
      derive_strcmp( widechar, char )
      derive_strcmp( widechar, widechar )
    # undef derive_strcmp

    # define  derive_noncmp( c1, c2 ) static  int   CompTo( c1, c2 )  {  return 0;  } \
                                      static  int   CompTo( c2, c1 )  {  return 0;  }
      derive_noncmp( const char*, char )
      derive_noncmp( const char*, byte_t )
      derive_noncmp( const char*, int16_t )
      derive_noncmp( const char*, int32_t )
      derive_noncmp( const char*, int64_t )
      derive_noncmp( const char*, word16_t )
      derive_noncmp( const char*, word32_t )
      derive_noncmp( const char*, word64_t )
      derive_noncmp( const char*, float )
      derive_noncmp( const char*, double )
      derive_noncmp( const widechar*, char )
      derive_noncmp( const widechar*, byte_t )
      derive_noncmp( const widechar*, int16_t )
      derive_noncmp( const widechar*, int32_t )
      derive_noncmp( const widechar*, int64_t )
      derive_noncmp( const widechar*, word16_t )
      derive_noncmp( const widechar*, word32_t )
      derive_noncmp( const widechar*, word64_t )
      derive_noncmp( const widechar*, float )
      derive_noncmp( const widechar*, double )
    # undef derive_noncmp

    template <class B>  int   CompTo( const B& b ) const
      {
        switch ( gettype() )
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

          case z_charstr: return CompTo( get_charstr(), b );
          case z_widestr: return CompTo( get_widestr(), b );

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
          case z_array_zarray:
          case z_array_xvalue:
          default:  break;
        }
        return 0;
      }
    template <class Z>  int CompTo( const xvalue<Z>& x ) const
      {
        switch ( x.gettype() )
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

          case z_charstr: return CompTo( x.get_charstr() );
          case z_widestr: return CompTo( x.get_widestr() );

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
          case z_array_zarray:
          case z_array_xvalue:
          default:  break;
        }
        return 0;
      }

  public:     // compare
    template <class V> bool  operator == ( const V& v ) const { return (CompTo( v ) & 0x04) != 0; }
    template <class V> bool  operator != ( const V& v ) const { return (CompTo( v ) & 0x08) != 0; }
    template <class V> bool  operator < ( const V& v ) const  { return (CompTo( v ) & 0x01) != 0; }
    template <class V> bool  operator > ( const V& v ) const  { return (CompTo( v ) & 0x02) != 0; }
    template <class V> bool  operator <= ( const V& v ) const { return (CompTo( v ) & 0x05) != 0; }
    template <class V> bool  operator >= ( const V& v ) const { return (CompTo( v ) & 0x06) != 0; }

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
    auto  to_string( const char* v ) const  -> std::string  {  return std::move( std::string( v != nullptr ? v : "" ) );  }

    auto  to_string( const widechar* v ) const  -> std::string
      {
        throw std::runtime_error( "not implemented" );
      }

    template <class X>
    auto  to_string( const xvalue<X>& v ) const           ->  std::string {  return std::move( mtc::to_string( v ) );  }
    template <class X>
    auto  to_string( const zarray<X>& v ) const           ->  std::string {  return std::move( mtc::to_string( v ) );  }
    template <class X>
    auto  to_string( const _auto_<char, X>& v ) const     ->  std::string {  return std::move( to_string( v.ptr() ) );  }
    template <class X>
    auto  to_string( const _auto_<widechar, X>& v ) const ->  std::string {  return std::move( to_string( v.ptr() ) );  }

    template <class V, class X>
    auto  to_string( const array<V, X>& arr ) const       ->  std::string 
      {
        std::string out( "{" );

        for ( auto& val: arr )
          out += to_string( val );

        return out.back() = '}', std::move( out );
      }

  public:     // stringize
    std::string to_string() const
      {
        switch ( gettype() )
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

          case z_charstr:       return get_charstr();
          case z_widestr:       return std::move( to_string( get_widestr() ) );

          case z_zarray:        return std::move( to_string( *get_zarray() ) );

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
          case z_array_zarray:  return std::move( to_string( *get_array_zarray() ) );
          case z_array_xvalue:  return std::move( to_string( *get_array_xvalue() ) );
          default:
            throw std::invalid_argument( "undefined xvalue<> type" );
        }
      }
  protected:  // helpers
    void  delete_data()
      {
        switch ( vxtype )
        {
          case z_charstr: if ( v_charstr != nullptr ) GetAllocator().free( v_charstr );  break;
          case z_widestr: if ( v_widestr != nullptr ) GetAllocator().free( v_widestr );  break;

      # define derive_destruct( _type_ )  case  z_##_type_: destruct( &v_##_type_ );  break;
          derive_destruct( zarray )
          derive_destruct( array_char )
          derive_destruct( array_byte )
          derive_destruct( array_int16 )
          derive_destruct( array_word16 )
          derive_destruct( array_int32 )
          derive_destruct( array_word32 )
          derive_destruct( array_int64 )
          derive_destruct( array_word64 )
          derive_destruct( array_float )
          derive_destruct( array_double )
          derive_destruct( array_charstr )
          derive_destruct( array_widestr )
          derive_destruct( array_zarray )
          derive_destruct( array_xvalue )
      # undef derive_destruct

          default:  break;
        }
        vxtype = 0xff;
      }
  };

  template <class M>
  class zarray
  {
    mutable M malloc; // used memory allocator

  public:     /*  integer key to string and reverse value conversion  */
    static  size_t    int_to_key( byte_t* out, unsigned key );
    static  unsigned  key_to_int( const char* src, size_t len );

  public:     /* special exceptions */
    class error
    {
      int         nerror;
      const char* message;
      const char* file;
      int         line;

    public:     // construction
      error( int err, const char* msg = nullptr, const char* szfile = nullptr, int lineid = 0 ):
        nerror( err ), message( msg ), file( szfile ), line( lineid ) {}

    };

  public:             // allocator
    M&    GetAllocator() const        {  return malloc;     }
    M&    SetAllocator( const M& m )  {  return malloc = m; }

  private:    // internal classes
    struct ztree;
    template <class owner, class key> class  zval;

  // private accessors for chain assignments of zval internal class
    template <class R, class K>
    static xvalue<M>* get_xvalue( zval<R, K>& r )
      {
        xvalue<M>*  xv = get_xvalue( *(R*)&r.parent );
        zarray<M>*  za;
        xvalue<M>*  pv;

        if ( (za = xv->get_zarray()) == nullptr
          && (za = xv->set_zarray()) == nullptr )  throw error( ENOMEM, "Out of memory", __FILE__, __LINE__ );
        if ( (pv = za->get_xvalue( r.thekey )) == nullptr
          && (pv = za->put_xvalue( r.thekey )) == nullptr ) throw error( ENOMEM, "Out of memory", __FILE__, __LINE__ );
        return pv;
      }
    template <class K>
    static xvalue<M>* get_xvalue( zval<zarray<M>, K>& r )
      {
        xvalue<M>*  pv;
        if ( (pv = ((zarray<M>*)&r.parent)->get_xvalue( r.thekey )) == nullptr
          && (pv = ((zarray<M>*)&r.parent)->put_xvalue( r.thekey )) == nullptr ) throw error( ENOMEM, "Out of memory", __FILE__, __LINE__ );
        return pv;
      }
    template <class R, class K>
    static const xvalue<M>* get_xvalue( const zval<R, K>& r )
      {
        const xvalue<M>*  xv;
        const zarray<M>*  za;
        return (xv = get_xvalue( r.parent )) != nullptr && (za = xv->get_zarray()) != nullptr ?
          za->get_xvalue( r.thekey ) : nullptr;
      }
    template <class K>
    static const xvalue<M>* get_xvalue( const zval<zarray<M>, K>& r )
      {
        return r.parent.get_xvalue( r.thekey );
      }

    template <class owner, class key>
    class  zval
    {
      friend class zarray;

    // construction
      zval( owner& o, key k ): parent( o ), thekey( k ) {}

    public:     // type conversions
    # define derive_operator( _type_ )                                                                    \
      operator _type_##_t () const                                                                        \
        {                                                                                                 \
          const xvalue<M>*  pv;                                                                           \
          const _type_##_t* pt;                                                                           \
          return (pv = get_xvalue( *this )) != nullptr && (pt = pv->get_##_type_()) != nullptr ? *pt : 0; \
        }

      derive_operator( char )
      derive_operator( byte )
      derive_operator( int16 )
      derive_operator( int32 )
      derive_operator( int64 )
      derive_operator( word16 )
      derive_operator( word32 )
      derive_operator( word64 )
      derive_operator( float )
      derive_operator( double )
    # undef derive_operator
      operator zarray () const          {  const xvalue<M>* p;  return (p = get_xvalue( *this )) != nullptr ? *p->get_zarray() : zarray();  }
      operator const char* () const     {  const xvalue<M>* p;  return (p = get_xvalue( *this )) != nullptr ? p->get_charstr() : nullptr;  }
      operator const widechar* () const {  const xvalue<M>* p;  return (p = get_xvalue( *this )) != nullptr ? p->get_widestr() : nullptr;  }

  /*
    zarray[key].set_xxx

    Поддержка конструкций с явным указанием типизации данных, но с выбрасываемыми исключениями:
      zarray& add = z[1].set_zarray( z )
      int32_t one = z["1"].set_int32( 1 )
      const char* two = z[L"w"].set_charstr( "two" )
  */
    public:     // zarray[key].set_xxx functions
    # define derive_set( _type_ )                              \
      _type_##_t& set_##_type_( const _type_##_t& t )             \
        {  return *get_xvalue( *this )->set_##_type_( t );  }

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
    # undef derive_set

      auto  set_zarray( const zarray& z = zarray() ) -> zarray&   {  return *get_xvalue( *this )->set_zarray( z );  }
      auto  set_charstr( const char* s )             -> char*     {  return  get_xvalue( *this )->set_charstr( s ); }
      auto  set_widestr( const widechar* w )         -> widechar* {  return  get_xvalue( *this )->set_widestr( w ); }

  /*
    zarray[key] = ...

    assignment operators - set typed value with automatic type detection
  */
    public:     // zarray[key] = assignment
      zval&           operator = ( char c )             {  set_char( c );    return *this;  }
      zval&           operator = ( byte_t b )           {  set_byte( b );    return *this;  }
      zval&           operator = ( int16_t i )          {  set_int16( i );   return *this;  }
      zval&           operator = ( int32_t i )          {  set_int32( i );   return *this;  }
      zval&           operator = ( int64_t i )          {  set_int64( i );   return *this;  }
      zval&           operator = ( word16_t i )         {  set_word16( i );  return *this;  }
      zval&           operator = ( word32_t i )         {  set_word32( i );  return *this;  }
      zval&           operator = ( word64_t i )         {  set_word64( i );  return *this;  }
      zval&           operator = ( float f )            {  set_float( f );   return *this;  }
      zval&           operator = ( double d )           {  set_double( d );  return *this;  }
      zval&           operator = ( const zarray& z )    {  set_zarray( z );  return *this;  }
      zval&           operator = ( const char* s )      {  set_charstr( s ); return *this;  }
      zval&           operator = ( const widechar* w )  {  set_widestr( w ); return *this;  }

  /*
    [] operators

    access to assiciative array with defined key
  */
    public:     // [] operators
  # define derive_access_operator( _key_type_ )  \
    auto operator [] ( _key_type_ k )       -> zval<zval, _key_type_>        {  return zval<zval, _key_type_>( *this, k );  }        \
    auto operator [] ( _key_type_ k ) const -> zval<const zval, _key_type_>  {  return zval<const zval, _key_type_>( *this, k );  }

    derive_access_operator( int )
    derive_access_operator( unsigned )
    derive_access_operator( const char* )
    derive_access_operator( const widechar* )
  # undef derive_access_operator

    protected:
      const owner&  parent;
      key           thekey;

    };

  public:     // high-level API
  # define derive_access_operator( _key_type_ )  \
    auto  operator [] ( _key_type_ k )       -> zval<zarray<M>, _key_type_> {  return zval<zarray<M>, _key_type_>( *this, k );  }    \
    auto  operator [] ( _key_type_ k ) const -> zval<zarray<M>, _key_type_> {  return zval<zarray<M>, _key_type_>( *(zarray<M>*)this, k );  }

    derive_access_operator( unsigned )
    derive_access_operator( const char* )
    derive_access_operator( const widechar* )
  # undef derive_access_operator

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

    static  auto  make_key( unsigned key ) -> uintkey {  return uintkey( key );  }
    static  auto  make_key( const char* key ) -> cstrkey {  return cstrkey( key );  }
    static  auto  make_key( const widechar* key ) ->wstrkey {  return wstrkey( key );  }

  private:    // internal structures
    struct  ztree: public array<ztree, M>
    {
      byte_t    chnode;
      byte_t    keyset;     // the key type
      xvalue<M> avalue;     // the element value

    public:     // construction
      ztree( M& memman, byte_t chinit = '\0' );
      ztree( const ztree& );
     ~ztree() {}

    protected:  // search implementation
      template <class self> static
      self* search( self& _me, const uint8_t* key, size_t cch )
        {
          if ( cch > 0 )
          {
            auto  chr = *key;
            auto  top = _me.begin();
            auto  end = _me.end();

            while ( top != end && top->chnode < chr )
              ++top;
            if ( top == end || top->chnode != chr )
              return nullptr;
            return search( *top, key + 1, cch - 1 );
          }
          return &_me;
        }

    public:     // unserialized tree work
      ztree*  insert( const byte_t* ptrkey, size_t cchkey )
        {
          ztree*  expand = this;

          for ( ; ; ++ptrkey, --cchkey )
          {
            if ( cchkey > 0 )
            {
              byte_t  chnext = *ptrkey;
              ztree*  ptrtop = expand->begin();
              ztree*  ptrend = expand->end();

              while ( ptrtop < ptrend && ptrtop->chnode < chnext )
                ++ptrtop;
              if ( ptrtop >= ptrend || ptrtop->chnode != chnext )
              {
                ztree newchr( array<ztree, M>::GetAllocator(), chnext );
                int   newpos;

                if ( expand->Insert( newpos = (int)(ptrtop - *expand), newchr ) != 0 ) return nullptr;
                  else  ptrtop = *expand + newpos;
              }
              expand = ptrtop;
            }
              else
            return expand;
          }
        }
      auto  search( const uint8_t* key, size_t cch ) const -> const ztree* {  return search( *this, key, cch );  }
      auto  search( const uint8_t* key, size_t cch )       ->       ztree* {  return search( *this, key, cch );  }
      int   lookup( byte_t* keybuf, int keylen, int buflen ) const;

      public:     // enumeration
        template <class A>  int     Enumerate( array<byte_t, M>&, int, A );
        template <class A>  int     Enumerate( A a );

      public:     // serialization
                            size_t  GetBufLen(    ) const;
        template <class O>  O*      Serialize( O* ) const;
        template <class S>  S*      FetchFrom( S* );

      protected:  // helpers
        int       plain_branchlen() const;
    };

    struct  zdata: public ztree
    {
      std::atomic_long  rcount;
      int               nitems;

    public:     // construction
      zdata( M& memman ): ztree( memman ), rcount( 1 ), nitems( 0 ) {}

    };

  public:     // construction
    zarray();
    zarray( M& );
    zarray( const zarray& );
   ~zarray();
    zarray& operator = ( const zarray& z );

    zarray  copy() const;

  public:     // compare
    template <class Z>  bool  operator == ( const zarray<Z>& z ) const;
    template <class Z>  bool  operator != ( const zarray<Z>& z ) const  {  return !(*this == z);  }

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

  protected:  // put_xvalue implementation
    template <class K>
    xvalue<M>*  put_xval( const K& k, const xvalue<M>& x = xvalue<M>() )
      {
        ztree*  pfound;

        if ( zhandler == nullptr && (zhandler = allocate_with<zdata>( GetAllocator(), GetAllocator() )) == nullptr )
          return nullptr;

        if ( (pfound = zhandler->insert( k.getkey(), k.getlen() )) != nullptr )  pfound->keyset = k.keyset();
          else  return nullptr;

        if ( pfound->avalue.gettype() == 0xff )
          ++zhandler->nitems;

        if ( x.gettype() != 0xff )
          pfound->avalue = x;

        return &pfound->avalue;
      }
    template <class Z, class K> static
    auto  get_xval( Z& z, const K& k )
      {
        auto  zt = z.zhandler != nullptr ? z.zhandler->search( k.getkey(), k.getlen() ) : nullptr;

        return zt != nullptr ? &zt->avalue : nullptr;
      }
    template <class K>
    auto  get_type( const K& k ) const
      {
        auto  pv = get_xval( k );
        return pv != nullptr ? pv->gettype() : 0xff;
      }

  public:     // put_xvalue family
    auto  put_xvalue( unsigned        key, const xvalue<M>& val = xvalue<M>() ) {  return put_xval( make_key( key ), val );  }
    auto  put_xvalue( const char*     key, const xvalue<M>& val = xvalue<M>() ) {  return put_xval( make_key( key ), val );  }
    auto  put_xvalue( const widechar* key, const xvalue<M>& val = xvalue<M>() ) {  return put_xval( make_key( key ), val );  }

  public:     // get_xvalue family
    auto  get_xvalue( unsigned key )        const {  return get_xval( *this, make_key( key ) );  }
    auto  get_xvalue( const char* key )     const {  return get_xval( *this, make_key( key ) );  }
    auto  get_xvalue( const widechar* key ) const {  return get_xval( *this, make_key( key ) );  }
    auto  get_xvalue( const zkey& key )     const {  return get_xval( *this, key );  }
    auto  get_xvalue( unsigned key )              {  return get_xval( *this, make_key( key ) );  }
    auto  get_xvalue( const char* key )           {  return get_xval( *this, make_key( key ) );  }
    auto  get_xvalue( const widechar* key )       {  return get_xval( *this, make_key( key ) );  }
    auto  get_xvalue( const zkey& key )           {  return get_xval( *this, key );  }

  public:     // access
  # define  derive_access( _type_ )                                                         \
    _type_##_t* set_##_type_( unsigned  k, _type_##_t v = 0 )                               \
      {                                                                                     \
        xvalue<M>*  zv;                                                                     \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##_type_( v ) : nullptr;         \
      }                                                                                     \
    _type_##_t* set_##_type_( const char* k, _type_##_t v = 0 )                             \
      {                                                                                     \
        xvalue<M>*  zv;                                                                     \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##_type_( v ) : nullptr;         \
      }                                                                                     \
    _type_##_t* set_##_type_( const widechar* k, _type_##_t v = 0 )                         \
      {                                                                                     \
        xvalue<M>*  zv;                                                                     \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##_type_( v ) : nullptr;         \
      }                                                                                     \
    const _type_##_t* get_##_type_( unsigned k ) const                                      \
      {                                                                                     \
        auto  xv = get_xvalue( k );                                                         \
        return xv != nullptr ? xv->get_##_type_() : nullptr;                                \
      }                                                                                     \
    const _type_##_t* get_##_type_( const char* k ) const                                   \
      {                                                                                     \
        auto  xv = get_xvalue( k );                                                         \
        return xv != nullptr ? xv->get_##_type_() : nullptr;                                \
      }                                                                                     \
    const _type_##_t* get_##_type_( const widechar* k ) const                               \
      {                                                                                     \
        auto  xv = get_xvalue( k );                                                         \
        return xv != nullptr ? xv->get_##_type_() : nullptr;                                \
      }                                                                                     \
    _type_##_t* get_##_type_( unsigned k )                                                  \
      {                                                                                     \
        auto  xv = get_xvalue( k );                                                         \
        return xv != nullptr ? xv->get_##_type_() : nullptr;                                \
      }                                                                                     \
    _type_##_t* get_##_type_( const char* k )                                               \
      {                                                                                     \
        auto  xv = get_xvalue( k );                                                         \
        return xv != nullptr ? xv->get_##_type_() : nullptr;                                \
      }                                                                                     \
    _type_##_t* get_##_type_( const widechar* k )                                           \
      {                                                                                     \
        auto  xv = get_xvalue( k );                                                         \
        return xv != nullptr ? xv->get_##_type_() : nullptr;                                \
      }                                                                                     \
    _type_##_t get_##_type_( unsigned k, _type_##_t v ) const                               \
      {                                                                                     \
        auto  pval = get_##_type_( k );                                                     \
        return pval != nullptr ? *pval : v;                                                 \
      }                                                                                     \
    _type_##_t get_##_type_( const char* k, _type_##_t v ) const                            \
      {                                                                                     \
        auto  pval = get_##_type_( k );                                                     \
        return pval != nullptr ? *pval : v;                                                 \
      }                                                                                     \
    _type_##_t get_##_type_( const widechar* k, _type_##_t v ) const                        \
      {                                                                                     \
        auto  pval = get_##_type_( k );                                                     \
        return pval != nullptr ? *pval : v;                                                 \
      }
      derive_access( char )
      derive_access( byte )
      derive_access( int16 )
      derive_access( word16 )
      derive_access( int32 )
      derive_access( word32 )
      derive_access( int64 )
      derive_access( word64 )
      derive_access( float )
      derive_access( double )
  # undef   derive_access

  # define  derive_access( k_type, v_type )                                     \
    auto  set_##v_type( k_type k )                                              \
      {                                                                         \
        auto  pxv = put_xvalue( k );                                            \
        return pxv != nullptr ? pxv->set_##v_type( GetAllocator() ) : nullptr;  \
      }                                                                         \
    auto  set_##v_type( k_type k, const v_type& z )                             \
      {                                                                         \
        auto  pxv = put_xvalue( k );                                            \
        return pxv != nullptr ? pxv->set_##v_type( z ) : nullptr;               \
      }                                                                         \
    auto  get_##v_type( k_type k )                                              \
      {                                                                         \
        auto  pxv = get_xvalue( k );                                            \
        return pxv != nullptr ? pxv->get_##v_type() : nullptr;                  \
      }                                                                         \
    auto  get_##v_type( k_type k ) const                                        \
      {                                                                         \
        auto  pxv = get_xvalue( k );                                            \
        return pxv != nullptr ? pxv->get_##v_type() : nullptr;                  \
      }
    derive_access( unsigned, zarray )
    derive_access( const char*, zarray )
    derive_access( const widechar*, zarray )
  # undef derive_access

  # define  derive_access( k_type, t_name, v_type )                               \
    auto  set_##t_name( k_type key, const v_type* str, size_t cch = (size_t)-1 )  \
      {                                                                           \
        auto  pxv = put_xvalue( key );                                            \
        return pxv != nullptr ? pxv->set_##t_name( str, cch ) : nullptr;          \
      }                                                                           \
    auto  get_##t_name( k_type key )                                              \
      {                                                                           \
        auto  pxv = get_xvalue( key );                                            \
        return pxv != nullptr ? pxv->get_##t_name() : nullptr;                    \
      }                                                                           \
    auto  get_##t_name( k_type key, const v_type* def = nullptr ) const           \
      {                                                                           \
        auto  pxv = get_xvalue( key );                                            \
        return pxv != nullptr ? pxv->get_##t_name() : def;                        \
      }
      derive_access( unsigned,        charstr, char )
      derive_access( const char*,     charstr, char )
      derive_access( const widechar*, charstr, char )
      derive_access( unsigned,        widestr, widechar )
      derive_access( const char*,     widestr, widechar )
      derive_access( const widechar*, widestr, widechar )
  # undef derive_access

  # define derive_access( k_type, v_type )                      \
    auto  set_##v_type( k_type k )                              \
      {                                                         \
        auto  pxv = put_xvalue( k );                            \
        return pxv != nullptr ? pxv->set_##v_type() : nullptr;  \
      }                                                         \
    auto  get_##v_type( k_type k ) const                        \
      {                                                         \
        auto  pxv = get_xvalue( k );                            \
        return pxv != nullptr ? pxv->get_##v_type() : nullptr;  \
      }                                                         \
    auto  get_##v_type( k_type k )                              \
      {                                                         \
        auto  pxv = get_xvalue( k );                            \
        return pxv != nullptr ? pxv->get_##v_type() : nullptr;  \
      }

      derive_access( unsigned, array_char )
      derive_access( const char*, array_char )
      derive_access( const widechar*, array_char )

      derive_access( unsigned, array_byte )
      derive_access( const char*, array_byte )
      derive_access( const widechar*, array_byte )

      derive_access( unsigned, array_int16 )
      derive_access( const char*, array_int16 )
      derive_access( const widechar*, array_int16 )

      derive_access( unsigned, array_word16 )
      derive_access( const char*, array_word16 )
      derive_access( const widechar*, array_word16 )

      derive_access( unsigned, array_int32 )
      derive_access( const char*, array_int32 )
      derive_access( const widechar*, array_int32 )

      derive_access( unsigned, array_word32 )
      derive_access( const char*, array_word32 )
      derive_access( const widechar*, array_word32 )

      derive_access( unsigned, array_int64 )
      derive_access( const char*, array_int64 )
      derive_access( const widechar*, array_int64 )

      derive_access( unsigned, array_word64 )
      derive_access( const char*, array_word64 )
      derive_access( const widechar*, array_word64 )

      derive_access( unsigned, array_float )
      derive_access( const char*, array_float )
      derive_access( const widechar*, array_float )

      derive_access( unsigned, array_double )
      derive_access( const char*, array_double )
      derive_access( const widechar*, array_double )

      derive_access( unsigned, array_charstr )
      derive_access( const char*, array_charstr )
      derive_access( const widechar*, array_charstr )

      derive_access( unsigned, array_widestr )
      derive_access( const char*, array_widestr )
      derive_access( const widechar*, array_widestr )

      derive_access( unsigned, array_zarray )
      derive_access( const char*, array_zarray )
      derive_access( const widechar*, array_zarray )

      derive_access( unsigned, array_xvalue )
      derive_access( const char*, array_xvalue )
      derive_access( const widechar*, array_xvalue )
  # undef derive_access

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

    void      DelAllData()
      {
        if ( zhandler != nullptr && --zhandler->rcount == 0 )
          deallocate_with( GetAllocator(), zhandler );
        zhandler = nullptr;
      }

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

  template <class M>  std::string to_string( const xvalue<M>& x ) {  return std::move( x.to_string() );  }
  template <class M>  std::string to_string( const zarray<M>& z ) {  return std::move( z.to_string() );  }
}

// serialization helpers

template <class M>
inline  size_t  GetBufLen( const mtc::zarray<M>& z )
  {
    return z.GetBufLen();
  }
template <class O, class M>
inline  O*      Serialize( O* o, const mtc::zarray<M>& z )
  {
    return z.Serialize( o );
  }
template <class S, class M>
inline  S*      FetchFrom( S* s,       mtc::zarray<M>& z )
  {
    return z.FetchFrom( s );
  }

template <class M>
inline size_t GetBufLen( const mtc::array<mtc::zarray<M>, M>& a )
  {
    size_t  length = ::GetBufLen( a.size() );

    for ( auto p = a.begin(); p < a.end(); ++p )
      length += p->GetBufLen();
    return length;
  }
template <class O, class M>
inline  O*    Serialize( O* o, const mtc::array<mtc::zarray<M>, M>& a )
  {
    if ( (o = ::Serialize( o, a.size() )) != nullptr )
      for ( auto p = a.begin(); p < a.end() && o != nullptr; ++p )
        o = p->Serialize( o );
    return o;
  }
template <class S, class M>
inline  S*    FetchFrom( S* s, mtc::array<mtc::zarray<M>, M>& a )
  {
    int   arsize;

    if ( (s = ::FetchFrom( s, arsize )) == nullptr || a.SetLen( arsize ) != 0 )
      return nullptr;

    for ( auto p = a.begin(); p < a.end(); )
      if ( (s = (p++)->FetchFrom( s )) == nullptr )
        break;
    return s;
  }

template <class M>
inline  size_t  GetBufLen( const mtc::xvalue<M>& x )
  {
    return x.GetBufLen();
  }
template <class M, class O>
inline  O*      Serialize( O* o, const mtc::xvalue<M>& x )
  {
    return x.Serialize( o );
  }
template <class M, class S>
inline  S*      FetchFrom( S* s, mtc::xvalue<M>& x )
  {
    return x.FetchFrom( s );
  }

template <class M>
inline  size_t  GetBufLen( const mtc::array<mtc::xvalue<M>, M>& a )
  {
    size_t  length = ::GetBufLen( a.size() );

    for ( auto p = a.begin(); p < a.end(); ++p )
      length += p->GetBufLen();
    return length;
  }
template <class O, class M>
inline  O*      Serialize( O* o, const mtc::array<mtc::xvalue<M>, M>& a )
  {
    if ( (o = ::Serialize( o, a.size() )) != nullptr )
      for ( auto p = a.begin(); p < a.end() && o != nullptr; ++p )
        o = p->Serialize( o );
    return o;
  }
template <class S, class M>
inline  S*      FetchFrom( S* s, mtc::array<mtc::xvalue<M>, M>& a )
  {
    int   arsize;

    if ( (s = ::FetchFrom( s, arsize )) == nullptr || a.SetLen( arsize ) != 0 )
      return nullptr;

    for ( auto p = a.begin(); p < a.end(); )
      if ( (s = (p++)->FetchFrom( s )) == nullptr )
        break;
    return s;
  }

namespace mtc
{
  // xvalue inline implementation

  template <class M>
  xvalue<M>::xvalue(): vxtype( undefined_type )
  {
  }

  template <class M>
  xvalue<M>::xvalue( M& m ): malloc( m ), vxtype( undefined_type )
  {
  }

  template <class M>
  xvalue<M>::xvalue( const xvalue<M>& v ): malloc( v.malloc ), vxtype( v.vxtype )
  {
    switch ( vxtype )
    {
    # define derive_copy( _type_ )  case z_##_type_:  construct( &v_##_type_, v.v_##_type_ ); break;

      derive_copy( char    )
      derive_copy( byte    )
      derive_copy( int16   )
      derive_copy( word16  )
      derive_copy( int32   )
      derive_copy( word32  )
      derive_copy( int64   )
      derive_copy( word64  )
      derive_copy( float   )
      derive_copy( double  )

      derive_copy( charstr )
      derive_copy( widestr )
//      derive_copy( buffer  )
      derive_copy( zarray  )

      derive_copy( array_char    )
      derive_copy( array_byte    )
      derive_copy( array_int16   )
      derive_copy( array_word16  )
      derive_copy( array_int32   )
      derive_copy( array_word32  )
      derive_copy( array_int64   )
      derive_copy( array_word64  )
      derive_copy( array_float   )
      derive_copy( array_double  )

      derive_copy( array_charstr )
      derive_copy( array_widestr )
//      derive_copy( array_buffer  )
      derive_copy( array_zarray  )
      derive_copy( array_xvalue  )

    # undef derive_copy
    }
    ((xvalue&)v).vxtype = undefined_type;
  }

  template <class M>
  xvalue<M>::~xvalue()
  {
    delete_data();
  }

  template <class M>
  xvalue<M>& xvalue<M>::operator = ( const xvalue<M>& v )
  {
    if ( vxtype != undefined_type )
      delete_data();

    malloc = v.malloc;
    vxtype = v.vxtype;

    switch ( vxtype )
    {
    # define derive_copy( _type_ )  case z_##_type_:  construct( &v_##_type_, v.v_##_type_ ); break;

      derive_copy( char    )
      derive_copy( byte    )
      derive_copy( int16   )
      derive_copy( word16  )
      derive_copy( int32   )
      derive_copy( word32  )
      derive_copy( int64   )
      derive_copy( word64  )
      derive_copy( float   )
      derive_copy( double  )

      derive_copy( charstr )
      derive_copy( widestr )
//      derive_copy( buffer  )
      derive_copy( zarray  )

      derive_copy( array_char    )
      derive_copy( array_byte    )
      derive_copy( array_int16   )
      derive_copy( array_word16  )
      derive_copy( array_int32   )
      derive_copy( array_word32  )
      derive_copy( array_int64   )
      derive_copy( array_word64  )
      derive_copy( array_float   )
      derive_copy( array_double  )

      derive_copy( array_charstr )
      derive_copy( array_widestr )
//      derive_copy( array_buffer  )
      derive_copy( array_zarray  )
      derive_copy( array_xvalue  )

    # undef derive_copy
    }

    ((xvalue&)v).vxtype = undefined_type;
      return *this;
  }

  template <class M>
  inline  size_t  xvalue<M>::GetBufLen() const
  {
    switch ( vxtype )
    {
  # define derive_size_plain( _type_ )  case z_##_type_: return 1 + sizeof(v_##_type_);
  # define derive_size_smart( _type_ )  case z_##_type_: return 1 + ::GetBufLen( v_##_type_ );
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
      derive_size_smart( array_xvalue )
      derive_size_smart( array_zarray )
  # undef derive_size_smart
  # undef derive_size_plain

      case z_charstr:
        {
          size_t  cch = v_charstr != nullptr ? w_strlen( v_charstr ) : 0;

          return 1 + ::GetBufLen( cch ) + cch;
        }
      case z_widestr:
        {
          size_t  cch = v_widestr != nullptr ? w_strlen( v_widestr ) : 0;

          return 1 + ::GetBufLen( cch ) + cch * sizeof(widechar);
        }
//      case z_buffer:
//        return 1 + ::GetBufLen( get_zbuffer()->GetLength() ) + get_zbuffer()->GetLength();
      case z_zarray:
        return 1 + get_zarray()->GetBufLen();

      default:  return 0;
    }
  }

  template <class M>  template <class O>
  inline  O*  xvalue<M>::Serialize( O* o ) const
  {
    switch ( gettype() )
    {
  # define derive_put( _type_ )  case z_##_type_: return ::Serialize( ::Serialize( o, vxtype ), &v_##_type_, sizeof(v_##_type_) );
      derive_put( char )
      derive_put( byte )
      derive_put( int16 )
      derive_put( word16 )

  # undef derive_put
  # define derive_put( _type_ )  case z_##_type_: return ::Serialize( ::Serialize( o, vxtype ), v_##_type_ );
      derive_put( int32 )
      derive_put( word32 )
      derive_put( int64 )
      derive_put( word64 )
      derive_put( float )
      derive_put( double )
      derive_put( zarray )
      derive_put( array_char )
      derive_put( array_byte )
      derive_put( array_float )
      derive_put( array_double )
      derive_put( array_int16 )
      derive_put( array_word16 )
      derive_put( array_int32 )
      derive_put( array_word32 )
      derive_put( array_int64 )
      derive_put( array_word64 )
      derive_put( array_charstr )
      derive_put( array_widestr )
      derive_put( array_xvalue )
      derive_put( array_zarray )
  # undef derive_put

      case z_charstr:
        {
          size_t  cch = v_charstr != nullptr ? w_strlen( v_charstr ) : 0;

          return ::Serialize( ::Serialize( ::Serialize( o, vxtype ), cch ), v_charstr, cch );
        }
      case z_widestr:
        {
          size_t  cch = v_widestr != nullptr ? w_strlen( v_widestr ) : 0;

          return ::Serialize( ::Serialize( ::Serialize( o, vxtype ), cch ), v_widestr, sizeof(widechar) * cch );
        }
/*      case fs_buffer:
        return fsPutBuff( fsPutMemo( fsPutChar( output, vxtype ), get_zbuffer()->GetLength() ),
                                                                  get_zbuffer()->GetBuffer(),
                                                                  get_zbuffer()->GetLength() );*/
      default:  return nullptr;
    }
  }

  template <class M>  template <class S>
  inline  S* xvalue<M>::FetchFrom( S* s )
  {
    byte_t  intype;

    if ( (s = ::FetchFrom( s, intype )) != nullptr ) delete_data();
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

      derive_get_smart( zarray )
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
      derive_get_smart( array_zarray )
      derive_get_smart( array_xvalue )
  # undef derive_get_smart
  # undef derive_get_plain

      case z_charstr:
        {
          unsigned  cch;
          char*     str;

          return (s = ::FetchFrom( s, cch )) != nullptr && (str = set_charstr( nullptr, cch )) != nullptr ?
            ::FetchFrom( s, str, cch ) : nullptr;
        }
      case z_widestr:
        {
          unsigned  cch;
          widechar* str;

          return (s = ::FetchFrom( s, cch )) != nullptr && (str = set_widestr( nullptr, cch )) != nullptr ?
            ::FetchFrom( s, str, sizeof(widechar) * cch ) : nullptr;
        }

      default:  return nullptr;
    }
  }

  // zarray helpers

  template <class M>
  inline  size_t  zarray<M>::int_to_key( byte_t* out, unsigned  key )
  {
    if ( (*out = (key >> 0x18)) != 0 )
    {
      *++out = (key >> 0x10);
      *++out = (key >> 0x08);
      *++out = (key >> 0x00);
      return 4;
    }
    if ( (*out = (key >> 0x10)) != 0 )
    {
      *++out = (key >> 0x08);
      *++out = (key >> 0x00);
      return 3;
    }
    if ( (*out = (key >> 0x08)) != 0 )
    {
      *++out = (key >> 0x00);
      return 2;
    }
    return (*out = key) != 0 ? 1 : 0;
  }

  template <class M>
  inline  unsigned  zarray<M>::key_to_int( const char* key, size_t len )
  {
    unsigned  out = 0;

    for ( auto end = key + len; key != end; )
      out = (out << 8) + (unsigned char)*key++;
    return out;
  }

  // zarray::ztree implementation

  template <class M>
  inline  int zarray<M>::ztree::lookup( byte_t* keybuf, int keylen, int buflen ) const
  {
    const ztree*  ptrtop = this->begin();
    const ztree*  ptrend = this->end();
    int           outlen;

    if ( keylen > 0 )
    {
      while ( ptrtop < ptrend && ptrtop->chnode < *keybuf )
        ++ptrtop;
      if ( ptrtop >= ptrend )
        return -1;
      if ( ptrtop->chnode == keybuf[0] )
      {
        if ( (outlen = ptrtop->lookup( keybuf + 1, keylen - 1, buflen - 1 )) != -1 )
          return 1 + outlen;
        if ( ++ptrtop >= ptrend )
          return -1;
      }
      return 1 + ptrtop->lookup( inc_assign( keybuf, ptrtop->chnode ), 0, buflen - 1 );
    }
    return avalue.gettype() != 0xff ? 0 : ptrtop != nullptr ? 1 + ptrtop->lookup( inc_assign( keybuf, ptrtop->chnode ), 0, buflen - 1 ) : -1;
  }

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

  template <class M>
  inline  size_t  zarray<M>::ztree::GetBufLen() const
  {
    int       branch = plain_branchlen();
    word16_t  lstore = (branch > 0 ? 0x0400 + branch : this->size()) + (avalue.gettype() != 0xff ? 0x0400 : 0);
    size_t    buflen = ::GetBufLen( lstore );

    if ( avalue.gettype() != 0xff )
      buflen += 1 + avalue.GetBufLen();

    if ( branch > 0 )
    {
      const ztree*  pbeg;

      for ( pbeg = this; pbeg->GetLen() == 1 && pbeg->avalue.gettype() == 0xff; pbeg = *pbeg )
        ++buflen;
      return buflen + pbeg->GetBufLen();
    }
      else
    for ( auto p = this->begin(); p < this->end(); ++p, ++buflen )
    {
      size_t  sublen = p->GetBufLen();
      buflen += ::GetBufLen( sublen ) + sublen;
    }

    return buflen;
  }

  template <class M>  template <class O>
  inline  O*   zarray<M>::ztree::Serialize( O* o ) const
  {
    int       branch = plain_branchlen();
      assert( branch <= 0x100 );
      assert( zarray<M>::ztree::size() <= 0x100 );
    word16_t  lstore = (branch > 0 ? 0x0400 + branch : this->size()) + (avalue.gettype() != 0xff ? 0x0200 : 0);

    o = ::Serialize( o, lstore );

    if ( avalue.gettype() != 0xff )
      o = avalue.Serialize( ::Serialize( o, keyset ) );

    if ( branch > 0 )
    {
      const ztree*  pbeg;

      for ( pbeg = *this; pbeg->GetLen() == 1 && pbeg->avalue.gettype() == 0xff; pbeg = *pbeg )
        o = ::Serialize( o, pbeg->chnode );
      o = ::Serialize( o, pbeg->chnode );

      return pbeg->Serialize( o );
    }
      else
    for ( auto p = this->begin(); p < this->end(); ++p )
    {
      size_t  sublen = p->GetBufLen();

      o = p->Serialize( ::Serialize( ::Serialize( o, p->chnode ), sublen ) );
    }
    return o;
  }

  template <class M>  template <class S>
  inline  S*    zarray<M>::ztree::FetchFrom( S* s )
  {
    word16_t  lfetch;

    if ( (s = ::FetchFrom( s, lfetch )) == nullptr )
      return nullptr;

    if ( (lfetch & 0x0200) != 0 && (s = avalue.FetchFrom( ::FetchFrom( s, (char&)keyset ) )) == nullptr )
      return nullptr;

    if ( (lfetch & 0x0400) != 0 )
    {
      ztree*  pbeg = this;
      int     size = lfetch & 0x1ff;

      while ( size-- > 0 )
      {
        ztree ztchar( this->GetAllocator() );

        if ( (s = ::FetchFrom( s, (char&)ztchar.chnode )) == nullptr )
          return nullptr;
        if ( pbeg->Append( ztchar ) == 0 ) pbeg = *pbeg;
          else  return nullptr;
      }

      return pbeg->FetchFrom( s );
    }
      else
    {
      int     size = lfetch & 0x1ff;
     
      while ( s != nullptr && size-- > 0 )
      {
        ztree     ztnext( this->GetAllocator() );
        unsigned  sublen;

        if ( (s = ::FetchFrom( ::FetchFrom( s, (char&)ztnext.chnode ), sublen )) == nullptr )
          return nullptr;
        if ( this->Append( ztnext ) != 0 )
          return nullptr;
        s = this->last().FetchFrom( s );
      }
    }
    return s;
  }

  template <class M>
  inline  int   zarray<M>::ztree::plain_branchlen() const
  {
    const ztree*  pbeg;
    int           size = 0;

    for ( size = 0, pbeg = this; pbeg->GetLen() == 1 && pbeg->avalue.gettype() == 0xff; pbeg = *pbeg )
      ++size;
    return size;
  }

// zarray::ztree implementation

  template <class M>
  zarray<M>::ztree::ztree( M& memman, byte_t chinit ):
    array<ztree, M>( memman ), chnode( chinit ), keyset( 0 ), avalue( memman )
  {
  }

  template <class M>
  zarray<M>::ztree::ztree( const ztree& z ):
    array<ztree, M>( ((ztree&)z).GetAllocator() ), chnode( z.chnode ), keyset( z.keyset ), avalue( z.avalue )
  {
    assert( z.size() == 0 );
  }

// zarray implementation

  template <class M>
  zarray<M>::zarray(): zhandler( nullptr )
  {
  }

  template <class M>
  zarray<M>::zarray( M& m ): malloc( m ), zhandler( nullptr )
  {
  }

  template <class M>
  zarray<M>::zarray( const zarray& z ): malloc( z.malloc )
  {
    if ( (zhandler = z.zhandler) != nullptr )
      ++zhandler->rcount;
  }

  template <class M>
  zarray<M>::~zarray()
  {
    if ( zhandler != nullptr && --zhandler->rcount == 0 )
      deallocate_with( GetAllocator(), zhandler );
  }

  /*
    att: indirect implementation to prevent delete of data while assigning
    structure to itself or return from function
  */
  template <class M>
  zarray<M>& zarray<M>::operator = ( const zarray& z )
  {
    zdata*  phandler;

    if ( (phandler = z.zhandler) != nullptr )
      ++phandler->rcount;

    if ( zhandler != nullptr && --zhandler->rcount == 0 )
      deallocate_with( GetAllocator(), zhandler );

    malloc = z.malloc;
      zhandler = phandler;
    return *this;
  }

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

  template <class M>
  inline  int zarray<M>::GetNextKey( void* keybuf, int cchkey, int buflen ) const
  {
    if ( zhandler == nullptr || cchkey == buflen )
      return (unsigned)-1;
    if ( cchkey >= 0 )
      ((char*)keybuf)[cchkey++] = '\0';
    return zhandler->lookup( (byte_t*)keybuf, cchkey, buflen );
  }

  template <class M>
  inline  unsigned  zarray<M>::GetKeyType( const void* ptrkey, int cchkey ) const
  {
    const ztree*  zvalue = zhandler != nullptr ? zhandler->search( (const byte_t*)ptrkey, cchkey ) : nullptr;
    return zvalue != nullptr ? zvalue->keyset : (unsigned)-1;
  }

  template <class M>
  inline  size_t  zarray<M>::GetBufLen() const
  {
    return zhandler != nullptr ? zhandler->GetBufLen() : 1;
  }

  template <class M>  template <class O>
  inline  O*      zarray<M>::Serialize( O* o ) const
  {
    return zhandler != nullptr ? zhandler->Serialize( o ) : ::Serialize( o, (char)0 );
  }

  template <class M>  template <class S>
  inline  S*      zarray<M>::FetchFrom( S*  s )
  {
    if ( zhandler != nullptr && --zhandler->rcount == 0 )
      deallocate_with( GetAllocator(), zhandler );

    if ( (zhandler = allocate_with<zdata>( GetAllocator(), GetAllocator() )) == nullptr )
      return nullptr;

    return (S*)zhandler->FetchFrom( s );
  }

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

# endif  // __zarray_h__
