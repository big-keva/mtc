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
# include <assert.h>

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

  /*  integer key to string value conversion                  */
  inline  int       zarray_int_to_key( byte_t* outkey, unsigned  ndwkey )
  {
    if ( (*outkey = (ndwkey >> 0x18)) != 0 )
    {
      *++outkey = (ndwkey >> 0x10);
      *++outkey = (ndwkey >> 0x08);
      *++outkey = (ndwkey >> 0x00);
      return 4;
    }
    if ( (*outkey = (ndwkey >> 0x10)) != 0 )
    {
      *++outkey = (ndwkey >> 0x08);
      *++outkey = (ndwkey >> 0x00);
      return 3;
    }
    if ( (*outkey = (ndwkey >> 0x08)) != 0 )
    {
      *++outkey = (ndwkey >> 0x00);
      return 2;
    }
    return (*outkey = (ndwkey >> 0x00)) != 0 ? 1 : 0;
  }

  inline  unsigned  zarray_key_to_int( const char* srckey, int ntrace )
  {
    unsigned  intkey = 0;

    for ( auto i = 0; i < ntrace; ++i )
      intkey = (intkey << 8) + (unsigned char)*srckey++;
    return intkey;
  }

  template <class M = def_alloc<>>  class zarray;
  template <class M = def_alloc<>>  class xvalue;

  typedef xvalue<>  XValue;
  typedef zarray<>  ZArray;

  /*
    xvalue handles any data in local buffer; data may be accessed by accessor methods
  */
  template <class M>
  class xvalue
  {
    unsigned char vxtype;
    char          chdata[sizeof(array<char>)];

  public:     // untyped element construction
    xvalue(): vxtype( 0xff )
      {
      }
    xvalue( const xvalue& v ): vxtype( v.vxtype )
      {
        if ( vxtype != 0 )
          memcpy( chdata, v.chdata, sizeof(chdata) );
        ((xvalue&)v).vxtype = 0;
      }
    ~xvalue()
      {
        delete_data();
      }
    xvalue& operator = ( const xvalue& v )
      {
        if ( vxtype != 0 )
          delete_data();
        if ( (vxtype = v.vxtype) != 0 )
          memcpy( chdata, v.chdata, sizeof(chdata) );
        ((xvalue&)v).vxtype = 0;
          return *this;
      }
    unsigned  gettype() const
      {
        return vxtype;
      }

  public:     // serialization
                        unsigned  GetBufLen(    ) const;
    template <class O>  O*        Serialize( O* ) const;
    template <class S>  S*        FetchFrom( S* );

  public:     // get_? methods
/* ordinal types */
  # define  derive_get( _type_ )  \
    const _type_##_t* get_##_type_() const {  return vxtype == z_##_type_ ? (const _type_##_t*)&chdata : nullptr;  }  \
          _type_##_t* get_##_type_()       {  return vxtype == z_##_type_ ?       (_type_##_t*)&chdata : nullptr;  }
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
  # undef derive_get

/* regular strings  */
    const char*     get_charstr() const {  return vxtype == z_charstr ? *(const char**)&chdata : nullptr;  }
    const widechar* get_widestr() const {  return vxtype == z_widestr ? *(const widechar**)&chdata : nullptr;  }
          char*     get_charstr()       {  return vxtype == z_charstr ? *(char**)&chdata : nullptr;  }
          widechar* get_widestr()       {  return vxtype == z_widestr ? *(widechar**)&chdata : nullptr;  }

/* special types: buffer and zarray */
//    const char*     get_buffer() const {  return vxtype == z_charstr ? *(const char**)&chdata : nullptr;  }
    const zarray<M>*  get_zarray() const  {  return vxtype == z_zarray ? (const zarray<M>*)&chdata : nullptr;  }
          zarray<M>*  get_zarray()        {  return vxtype == z_zarray ? (zarray<M>*)&chdata : nullptr;  }

/* arrays */
  # define  derive_get( _type_ )  \
    const array<_type_##_t>*  get_array_##_type_() const                                      \
      {  return vxtype == z_array_##_type_ ? (const array<_type_##_t>*)&chdata : nullptr;  }  \
          array<_type_##_t>*  get_array_##_type_()                                            \
      {  return vxtype == z_array_##_type_ ? (array<_type_##_t>*)&chdata : nullptr;  } 
      derive_get( char )
      derive_get( byte )
      derive_get( int16 )
      derive_get( int32 )
      derive_get( word16 )
      derive_get( word32 )
      derive_get( int64 )
      derive_get( word64 )
      derive_get( float )
      derive_get( double )
  # undef derive_get

/* special arrays */
  const array<_auto_<char, M>, M>*  get_array_charstr() const
    {  return vxtype == z_array_charstr ? (const array<_auto_<char, M>, M>*)&chdata : nullptr;  }
  array<_auto_<char, M>, M>*        get_array_charstr()
    {  return vxtype == z_array_charstr ? (array<_auto_<char, M>, M>*)&chdata : nullptr;  }
  const array<_auto_<widechar, M>, M>*  get_array_widestr() const
    {  return vxtype == z_array_charstr ? (const array<_auto_<widechar, M>, M>*)&chdata : nullptr;  }
  array<_auto_<widechar, M>, M>*        get_array_widestr()
    {  return vxtype == z_array_charstr ? (array<_auto_<widechar, M>, M>*)&chdata : nullptr;  }
  const array<zarray<M>, M>*            get_array_zarray() const
    {  return vxtype == z_array_zarray  ? (const array<zarray<M>, M>*)&chdata : nullptr;  }
  array<zarray<M>, M>*                  get_array_zarray()
    {  return vxtype == z_array_zarray ? (array<zarray<M>, M>*)&chdata : nullptr;  }
  const array<xvalue<M>, M>*            get_array_xvalue() const
    {  return vxtype == z_array_xvalue  ? (const array<xvalue<M>, M>*)&chdata : nullptr;  }
  array<xvalue<M>, M>*                  get_array_xvalue()
    {  return vxtype == z_array_xvalue ? (array<xvalue<M>, M>*)&chdata : nullptr;  }

public:     // set_?? methods
/* ordinal types */
  # define  derive_set( _type_ )                                                                  \
    _type_##_t* set_##_type_( _type_##_t v = 0 )                                                  \
      {  delete_data();  vxtype = z_##_type_;  return __safe_array_construct_cpy( (_type_##_t*)&chdata, v );  }

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

/* regular strings  */
    char*     set_charstr( const char*  pszstr, unsigned  cchstr = (unsigned)-1 )
      {
        if ( cchstr == (unsigned)-1 )
          cchstr = pszstr != nullptr ? (unsigned)w_strlen( pszstr ) : 0;

        if ( cchstr == (unsigned)-1 )
          cchstr = pszstr != nullptr ? (unsigned)w_strlen( pszstr ) : 0;
        delete_data();

        if ( (*(char**)&chdata = (char*)M().alloc( cchstr + 1 )) == nullptr )
          return nullptr;

        if ( pszstr != nullptr )
          memcpy( *(char**)&chdata, pszstr, cchstr );

        (*(char**)&chdata)[cchstr] = '\0';
          vxtype = z_charstr;

        return *(char**)&chdata;
      }
    char*&    set_charstr()
      {
        delete_data();
          vxtype = z_charstr;
        return *(char**)&chdata = nullptr;
      }
    widechar* set_widestr( const widechar*  pszstr, unsigned  cchstr = (unsigned)-1 )
      {
        if ( cchstr == (unsigned)-1 )
          cchstr = pszstr != nullptr ? (unsigned)w_strlen( pszstr ) : 0;

        delete_data();

        if ( (*(widechar**)&chdata = (widechar*)M().alloc( sizeof(widechar) * (cchstr + 1) )) == nullptr )
          return nullptr;

        if ( pszstr != nullptr )
          memcpy( *(widechar**)&chdata, pszstr, sizeof(widechar) * cchstr );

        (*(widechar**)&chdata)[cchstr] = 0;
          vxtype = z_widestr;

        return *(widechar**)&chdata;
      }
    widechar*&  set_widestr()
      {
        delete_data();
          vxtype = z_widestr;
        return *(widechar**)&chdata = nullptr;
      }

/* special types: buffer, zarray and array(s) */
    zarray<M>*  set_zarray()
      {
        delete_data();  vxtype = z_zarray;
          __safe_array_construct_def( (zarray<M>*)&chdata, 1 );
        return (zarray<M>*)&chdata;
      }
    zarray<M>*  set_zarray( const zarray<M>& z )
      {
        delete_data();  vxtype = z_zarray;
        return __safe_array_construct_cpy( (zarray<M>*)&chdata, z );
      }

  # define  derive_set( _type_ )                                              \
    array<_type_##_t, M>*  set_array_##_type_()                               \
    {                                                                         \
      delete_data();  vxtype = z_array_##_type_;                              \
      return new( (array<_type_##_t, M>*)&chdata ) array<_type_##_t, M>();    \
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

    array<_auto_<char, M>, M>*      set_array_charstr()
      {
        delete_data();  vxtype = z_array_charstr;
        return new( (array<_auto_<char, M>, M>*)&chdata ) array<_auto_<char, M>, M>();
      }
    array<_auto_<widechar, M>, M>*  set_array_widestr()
      {
        delete_data();  vxtype = z_array_widestr;
        return new( (array<_auto_<widechar, M>, M>*)&chdata ) array<_auto_<widechar, M>, M>();
      }
    array<zarray<M>, M>*            set_array_zarray()
      {
        delete_data();  vxtype = z_array_zarray;
        return new( (array<zarray<M>, M>*)&chdata ) array<zarray<M>, M>();
      }
    array<xvalue<M>, M>*            set_array_xvalue()
      {
        delete_data();  vxtype = z_array_xvalue;
        return new( (array<xvalue<M>, M>*)&chdata )array<xvalue<M>, M>();
      }

  protected:  // helpers
    void  delete_data()
      {
        switch ( vxtype )
        {
//          case z_buffer:  __safe_array_destruct( (_freebuffer_*)&chdata, 1 ); break;
          case z_charstr:
          case z_widestr: M().free( *(char**)&chdata );                     break;
          case z_zarray:  __safe_array_destruct( (zarray<M>*)&chdata, 1 );  break;

      # define derive_destruct( _type_ )          \
        case  z_array_##_type_: __safe_array_destruct( (array<_type_##_t, M>*)&chdata, 1 );  break;
          derive_destruct( char )
          derive_destruct( byte )
          derive_destruct( int16 )
          derive_destruct( word16 )
          derive_destruct( int32 )
          derive_destruct( word32 )
          derive_destruct( int64 )
          derive_destruct( word64 )
          derive_destruct( float )
          derive_destruct( double )
      # undef derive_destruct

          case z_array_charstr: __safe_array_destruct( (array<_auto_<char, M>, M>*)&chdata, 1 );  break;
          case z_array_widestr: __safe_array_destruct( (array<_auto_<widechar, M>, M>*)&chdata, 1 );  break;
          case z_array_zarray:  __safe_array_destruct( (array<zarray<M>, M>*)&chdata, 1 );  break;
          case z_array_xvalue:  __safe_array_destruct( (array<xvalue<M>, M>*)&chdata, 1 );  break;
          default:  break;
        }
        vxtype = 0xff;
      }
  };

  template <class M>
  class zarray
  {
    struct ztree;

    class  zval
    {
      friend class zarray;

    protected:  // construction
      zval( const xvalue<M>* p = nullptr ): pvalue( (xvalue<M>*)p )
        {
        }

    public:     // type conversions
      operator const char* () const
        {  return pvalue != nullptr ? pvalue->get_charstr() : nullptr;  }
      operator const widechar* () const
        {  return pvalue != nullptr ? pvalue->get_widestr() : nullptr;  }
      operator double () const
        {  return pvalue != nullptr && pvalue->get_double() != nullptr ? *pvalue->get_double() : 0.0;  }
      operator int32_t () const
        {  return pvalue != nullptr && pvalue->get_int32() != nullptr ? *pvalue->get_int32() : 0;  }
      operator int64_t () const
        {  return pvalue != nullptr && pvalue->get_int64() != nullptr ? *pvalue->get_int64() : 0;  }
      operator word32_t () const
        {  return pvalue != nullptr && pvalue->get_word32() != nullptr ? *pvalue->get_word32() : 0;  }
      operator word64_t () const
        {  return pvalue != nullptr && pvalue->get_word64() != nullptr ? *pvalue->get_word64() : 0;  }

    public:     // assignment
      bool  operator = ( const char* s )
        {  return pvalue != nullptr && pvalue->set_charstr( s ) != nullptr;  }
      bool  operator = ( const widechar* s )
        {  return pvalue != nullptr && pvalue->set_widestr( s ) != nullptr;  }
      bool  operator = ( double f )
        {  return pvalue != nullptr && pvalue->set_double( f ) != nullptr;  }
      bool  operator = ( int32_t i )
        {  return pvalue != nullptr && pvalue->set_int32( i ) != nullptr;  }
      bool  operator = ( int64_t i )
        {  return pvalue != nullptr && pvalue->set_int64( i ) != nullptr;  }
      bool  operator = ( word32_t u )
        {  return pvalue != nullptr && pvalue->set_word32( u ) != nullptr;  }
      bool  operator = ( word64_t u )
        {  return pvalue != nullptr && pvalue->set_word64( u ) != nullptr;  }

    protected:
      xvalue<M>*  pvalue;

    };

  public:     // high-level API
  # define derive_access_operator( _key_type_ )             \
    zval  operator [] ( _key_type_ k )                      \
      {                                                     \
        zval  zv;                                           \
        if ( (zv.pvalue = get_xvalue( k )) == nullptr )     \
          zv.pvalue = put_xvalue( k );                      \
        return zv;                                          \
      }                                                     \
    const zval  operator [] ( _key_type_ k ) const          \
      {  return zval( get_xvalue( k ) );  }

    derive_access_operator( unsigned )
    derive_access_operator( const char* )
    derive_access_operator( const widechar* )
  # undef derive_access_operator

  public:     // enumeration support

    class zkey
    {
      friend class  zarray;
      friend struct zarray::ztree;

      const char* keybuf;
      unsigned    keylen;
      unsigned    keyset;

    public:     // constructors
      zkey(): keybuf( nullptr ), keylen( 0 ), keyset( -1 )
        {
        }
      zkey( const zkey& z ): keybuf( z.keybuf ), keylen( z.keylen ), keyset( z.keyset )
        {
        }

    private:    // real initialization constructor
      zkey( const byte_t* k, unsigned l, unsigned t ): keybuf( (const char*)k ), keylen( l ), keyset( t )
        {
        }

    public:     // types
      operator unsigned() const
        {  return keyset == 0 ? zarray_key_to_int( keybuf, keylen ) : 0;  }
      operator const char*() const
        {  return keyset == 1 ? (const char*)keybuf : nullptr;  }
      operator const widechar*() const
        {  return keyset == 2 ? (const widechar*)keybuf : nullptr;  }
    };

  private:    // internal structures
    struct  ztree: public array<ztree, M>
    {
      byte_t    chnode;
      byte_t    keyset;     // the key type
      xvalue<M> avalue;     // the element value

    public:     // construction
      ztree( byte_t chinit = '\0' ): chnode( chinit ), keyset( 0 )  {}
      ztree( const ztree& z ): chnode( z.chnode ), keyset( z.keyset ) {  assert( z.size() == 0 );  }
     ~ztree()  {}

    public:     // unserialized tree work
      ztree*        insert( const byte_t* ptrkey, unsigned cchkey )
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
                int   newpos;

                if ( expand->Insert( newpos = (int)(ptrtop - *expand), ztree( chnext ) ) != 0 ) return nullptr;
                  else  ptrtop = *expand + newpos;
              }
              expand = ptrtop;
            }
              else
            return expand;
          }
        }
      const ztree*  search( const byte_t* ptrkey, unsigned cchkey ) const
        {
          if ( cchkey > 0 )
          {
            byte_t        chnext = *ptrkey;
            const ztree*  ptrtop = this->begin();
            const ztree*  ptrend = this->end();

            while ( ptrtop < ptrend && ptrtop->chnode < chnext )
              ++ptrtop;
            if ( ptrtop >= ptrend || ptrtop->chnode != chnext )
              return nullptr;
            return ptrtop->search( ptrkey + 1, cchkey - 1 );
          }
          return this;
        }
      int           lookup( byte_t* keybuf, int keylen, int buflen ) const;

      public:     // enumeration
        template <class A>  int       Enumerate( array<byte_t, M>&, int, A );
        template <class A>  int       Enumerate( A a );

      public:     // serialization
                            unsigned  GetBufLen(    ) const;
        template <class O>  O*        Serialize( O* ) const;
        template <class S>  S*        FetchFrom( S* );

      protected:  // helpers
        int       plain_branchlen() const;
    };

    struct  zdata: public ztree
    {
      int     nitems;
      int     rcount;

    public:     // construction
      zdata():
        ztree(), nitems( 0 ), rcount( 1 ) {}

    };

    struct  zstate
    {
      const ztree*  object;
      const ztree*  ptrtop;
    };

  public:     // construction
    zarray(): zhandler( nullptr )
      {
      }
    zarray( const zarray& z )
      {
        if ( (zhandler = z.zhandler) != nullptr )
          ++zhandler->rcount;
      }
   ~zarray()
      {
        if ( zhandler != nullptr && --zhandler->rcount == 0 )
          M().deallocate( zhandler );
      }
  zarray& operator = ( const zarray& z )
      {
        if ( zhandler != nullptr && --zhandler->rcount == 0 )
          M().deallocate( zhandler );
        if ( (zhandler = z.zhandler) != nullptr )
          ++zhandler->rcount;
        return *this;
      }

  protected:  // helpers
    static  byte_t* inc_assign( byte_t* p, byte_t c )
      {
        *p++ = c;  return p;
      }
    xvalue<M>*  put_xvalue( const byte_t* p, unsigned l, unsigned k )
      {
        ztree*  pfound;

        if ( zhandler == nullptr && (zhandler = M().template allocate<zdata>()) == nullptr )
          return nullptr;

        if ( (pfound = zhandler->insert( p, l )) != nullptr )  pfound->keyset = k;
          else  return nullptr;

        if ( pfound->avalue.gettype() == 0xff )
          ++zhandler->nitems;

        return &pfound->avalue;
      }

  protected:  // put_xvalue family
    xvalue<M>*  put_xvalue( unsigned    ndwkey )
      {
        byte_t  thekey[4];

        return put_xvalue( thekey, zarray_int_to_key( thekey, ndwkey ), 0 );
      }
    xvalue<M>*  put_xvalue( const char*  pszkey )
      {
        return put_xvalue( (const byte_t*)pszkey, (unsigned)strlen( pszkey ), 1 );
      }
    xvalue<M>*  put_xvalue( const widechar*  pszkey )
      {
        return put_xvalue( (const byte_t*)pszkey, (unsigned)(sizeof(widechar) * w_strlen( pszkey )), 2 );
      }
    xvalue<M>*  put_xvalue( const widechar* wszkey, const xvalue<M>& xv )
      {
        xvalue<M>*  pv;
        return (pv = put_xvalue( wszkey )) != nullptr ? &(*pv = xv) : nullptr;
      }
    xvalue<M>*  put_xvalue( unsigned    ndwkey, const xvalue<M>& xv )
      {
        xvalue<M>*  pv;
        return (pv = put_xvalue( ndwkey )) != nullptr ? &(*pv = xv) : nullptr;
      }
    xvalue<M>*  put_xvalue( const char* pszkey, const xvalue<M>& xv )
      {
        xvalue<M>*  pv;
        return (pv = put_xvalue( pszkey )) != nullptr ? &(*pv = xv) : nullptr;
      }

  protected:  // get_untyped family
    const ztree*  get_untyped( unsigned     ndwkey ) const
      {
        byte_t  thekey[4];

        return zhandler != nullptr ? zhandler->search( thekey, zarray_int_to_key( thekey, ndwkey ) ) : nullptr;
      }
    const ztree*  get_untyped( const char*  pszkey ) const
      {
        return zhandler != NULL ? zhandler->search( (const byte_t*)pszkey,
          (unsigned)strlen( pszkey ) ) : nullptr;
      }
    const ztree*  get_untyped( const widechar*  pszkey ) const
      {
        return zhandler != NULL ? zhandler->search( (const byte_t*)pszkey,
          (unsigned)(sizeof(widechar) * w_strlen( pszkey )) ) : nullptr;
      }
    ztree*  get_untyped( unsigned     thekey )
      {
        byte_t  keybuf[4];

        return zhandler != nullptr ? (ztree*)zhandler->search( keybuf, zarray_int_to_key( keybuf, thekey ) ) : nullptr;
      }
    ztree*  get_untyped( const char*  thekey )
      {
        return zhandler != nullptr ? (ztree*)zhandler->search( (const byte_t*)thekey,
          (unsigned)strlen( thekey ) ) : nullptr;
      }
    ztree*  get_untyped( const widechar*  thekey )
      {
        return zhandler != nullptr ? (ztree*)zhandler->search( (const byte_t*)thekey,
          (unsigned)(sizeof(widechar) * w_strlen( thekey )) ) : nullptr;
      }

  public:     // get_?
  # define  derive_get_xvalue( _type_ )                                           \
    const xvalue<M>* get_xvalue( _type_ thekey ) const                            \
      {                                                                           \
        const ztree*  zt;                                                         \
        return (zt = get_untyped( thekey )) != nullptr ? &zt->avalue : nullptr;   \
      }                                                                           \
    xvalue<M>*       get_xvalue( _type_ thekey )                                  \
      {                                                                           \
        const ztree*  zt;                                                         \
        return (zt = get_untyped( thekey )) != nullptr ? (xvalue<M>*)&zt->avalue : nullptr;   \
      }
    derive_get_xvalue( unsigned )
    derive_get_xvalue( const char* )
    derive_get_xvalue( const widechar* )
  # undef derive_get_xvalue

  public:     // access
  # define  derive_access( _type_ )                                                         \
    auto set_##_type_( unsigned  k, _type_##_t v = 0 )                                      \
      {                                                                                     \
        xvalue<M>*  zv;                                                                     \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##_type_( v ) : nullptr;         \
      }                                                                                     \
    auto set_##_type_( const char* k, _type_##_t v = 0 )                             \
      {                                                                                     \
        xvalue<M>*  zv;                                                                     \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##_type_( v ) : nullptr;         \
      }                                                                                     \
    auto set_##_type_( const widechar* k, _type_##_t v = 0 )                         \
      {                                                                                     \
        xvalue<M>*  zv;                                                                     \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##_type_( v ) : nullptr;         \
      }                                                                                     \
    auto get_##_type_( unsigned k ) const                                      \
      {                                                                                     \
        const ztree*  zt;                                                                   \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_##_type_( const char* k ) const                                   \
      {                                                                                     \
        const ztree*  zt;                                                                   \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_##_type_( const widechar* k ) const                               \
      {                                                                                     \
        const ztree*  zt;                                                                   \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_##_type_( unsigned k )                                                  \
      {                                                                                     \
        ztree*  zt;                                                                         \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_##_type_( const char* k )                                               \
      {                                                                                     \
        ztree*  zt;                                                                         \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_##_type_( const widechar* k )                                           \
      {                                                                                     \
        ztree*  zt;                                                                         \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_##_type_( unsigned k, _type_##_t v ) const                                     \
      {                                                                                     \
        auto  pval = get_##_type_( k );                                                     \
        return pval != nullptr ? *pval : v;                                                 \
      }                                                                                     \
    auto get_##_type_( const char* k, _type_##_t v ) const                                  \
      {                                                                                     \
        auto  pval = get_##_type_( k );                                                     \
        return pval != nullptr ? *pval : v;                                                 \
      }                                                                                     \
    auto get_##_type_( const widechar* k, _type_##_t v ) const                              \
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

//      derive_access( string )
//      derive_access( buffer )

//      derive_access( xvalue )
  # undef   derive_access

  # define  derive_put_type( k_type, v_type )                                                     \
    auto  set_##v_type( k_type k )                                                                \
      {                                                                                           \
        xvalue<M>*  zv;                                                                           \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##v_type() : nullptr;                  \
      }                                                                                           \
    auto  set_##v_type( k_type k, const v_type& z )                                               \
      {                                                                                           \
        xvalue<M>*  zv;                                                                           \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_##v_type( z ) : nullptr;               \
      }                                                                                           \
    auto  get_##v_type( k_type k )                                                                \
      {                                                                                           \
        ztree*  zt;                                                                               \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##v_type() : nullptr;          \
      }                                                                                           \
    auto  get_##v_type( k_type k ) const                                                          \
      {                                                                                           \
        const ztree*  zt;                                                                         \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_##v_type() : nullptr;          \
      }
    derive_put_type( unsigned, zarray )
    derive_put_type( const char*, zarray )
    derive_put_type( const widechar*, zarray )
  # undef derive_put_type

  # define  derive_put_string( k_type, t_name, v_type )                                           \
    v_type* set_##t_name( k_type thekey, const v_type* pszstr, unsigned cchstr = (unsigned)-1 )   \
      {                                                                                           \
        xvalue<M>* zv;                                                                            \
        return (zv = put_xvalue( thekey )) != NULL ? zv->set_##t_name( pszstr, cchstr ) : nullptr;\
      }                                                                                           \
    v_type* get_##t_name( k_type thekey )                                                         \
      {                                                                                           \
        ztree*  zt;                                                                               \
        return (zt = get_untyped( thekey )) != NULL ? zt->avalue.get_##t_name()  : NULL;          \
      }                                                                                           \
    const v_type* get_##t_name( k_type thekey ) const                                             \
      {                                                                                           \
        const ztree*  zt;                                                                         \
        return (zt = get_untyped( thekey )) != NULL ? zt->avalue.get_##t_name()  : NULL;          \
      }
      derive_put_string( unsigned,        charstr, char )
      derive_put_string( const char*,     charstr, char )
      derive_put_string( const widechar*, charstr, char )
      derive_put_string( unsigned,        widestr, widechar )
      derive_put_string( const char*,     widestr, widechar )
      derive_put_string( const widechar*, widestr, widechar )
  # undef derive_put_string

  # define  derive_access( _type_ )                                                         \
    auto set_array_##_type_( unsigned  k )                                                  \
      {                                                                                     \
        xvalue<M>* zv;                                                                      \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_array_##_type_() : nullptr;      \
      }                                                                                     \
    auto set_array_##_type_( const char* k )                                                \
      {                                                                                     \
        xvalue<M>* zv;                                                                      \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_array_##_type_() : nullptr;      \
      }                                                                                     \
    auto set_array_##_type_( const widechar* k )                                            \
      {                                                                                     \
        xvalue<M>* zv;                                                                      \
        return (zv = put_xvalue( k )) != nullptr ? zv->set_array_##_type_() : nullptr;      \
      }                                                                                     \
    auto get_array_##_type_( unsigned k ) const                                             \
      {                                                                                     \
        const ztree*  zt;                                                                   \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_array_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_array_##_type_( const char* k ) const                                          \
      {                                                                                     \
        const ztree*  zt;                                                                   \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_array_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_array_##_type_( const widechar* k ) const                                      \
      {                                                                                     \
        const ztree*  zt;                                                                   \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_array_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_array_##_type_( unsigned k )                                                   \
      {                                                                                     \
        ztree*  zt;                                                                         \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_array_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_array_##_type_( const char* k )                                                \
      {                                                                                     \
        ztree*  zt;                                                                         \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_array_##_type_()  : nullptr;   \
      }                                                                                     \
    auto get_array_##_type_( const widechar* k )                                            \
      {                                                                                     \
        ztree*  zt;                                                                         \
        return (zt = get_untyped( k )) != nullptr ? zt->avalue.get_array_##_type_()  : nullptr;   \
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

      derive_access( charstr )
      derive_access( widestr )
//      derive_access( buffer )

      derive_access( zarray )
      derive_access( xvalue )

  # undef derive_access
/*
  # define  derive_put_buffer( k_type )                                                           \
    void*  put_buffer( k_type k, unsigned l = 0, const void* p = NULL )                           \
      {                                                                                           \
        xvalue*   zv;                                                                             \
        return (zv = put_xvalue( k )) != NULL ? zv->set_buffer( l, p ) : NULL;                    \
      }
    derive_put_buffer( unsigned )
    derive_put_buffer( const char* )
    derive_put_buffer( const widechar* )
  # undef   derive_put_buffer
*/

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
          M().deallocate( zhandler );
        zhandler = nullptr;
      }

    int       size() const
      {
        return zhandler != nullptr ? zhandler->nitems : 0;
      }
    int       haskeys() const
      {
        return zhandler != nullptr && zhandler->size() > 0;
      }

  # define  derive_TypeOfData( _type_ )                                                   \
    unsigned  TypeOfData( _type_  thekey ) const                                          \
      {                                                                                     \
        const ztree*  zt;                                                                   \
        return (zt = get_untyped( thekey )) != NULL ? zt->avalue.gettype() : 0xff;        \
      }
    derive_TypeOfData( unsigned )
    derive_TypeOfData( const char* )
    derive_TypeOfData( const widechar* )
  # undef derive_TypeOfData

  public:     // serialization
                        unsigned  GetBufLen(            ) const;
    template <class O>  O*        Serialize( O*  output ) const;
    template <class S>  S*        FetchFrom( S*  source );

  protected:  // variables
    zdata*  zhandler;

  };

}
// serialization helpers

template <class M>
inline  unsigned  GetBufLen( const mtc::zarray<M>& z )
  {
    return z.GetBufLen();
  }
template <class O, class M>
inline  O*        Serialize( O* o, const mtc::zarray<M>& z )
  {
    return z.Serialize( o );
  }
template <class S, class M>
inline  S*        FetchFrom( S* s,       mtc::zarray<M>& z )
  {
    return z.FetchFrom( s );
  }

template <class M>
inline unsigned   GetBufLen( const mtc::array<mtc::zarray<M>, M>& a )
  {
    unsigned  length = ::GetBufLen( a.size() );

    for ( auto p = a.begin(); p < a.end(); ++p )
      length += p->GetBufLen();
    return length;
  }
template <class O, class M>
inline  O*        Serialize( O* o, const mtc::array<mtc::zarray<M>, M>& a )
  {
    if ( (o = ::Serialize( o, a.size() )) != nullptr )
      for ( auto p = a.begin(); p < a.end() && o != nullptr; ++p )
        o = p->Serialize( o );
    return o;
  }
template <class S, class M>
inline  S*        FetchFrom( S* s, mtc::array<mtc::zarray<M>, M>& a )
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
inline  unsigned  GetBufLen( const mtc::xvalue<M>& x )
  {
    return x.GetBufLen();
  }
template <class M, class O>
inline  O*        Serialize( O* o, const mtc::xvalue<M>& x )
  {
    return x.Serialize( o );
  }
template <class M, class S>
inline  S*        FetchFrom( S* s, mtc::xvalue<M>& x )
  {
    return x.FetchFrom( s );
  }

template <class M>
inline  unsigned  GetBufLen( const mtc::array<mtc::xvalue<M>, M>& a )
  {
    unsigned  length = ::GetBufLen( a.size() );

    for ( auto p = a.begin(); p < a.end(); ++p )
      length += p->GetBufLen();
    return length;
  }
template <class O, class M>
inline  O*        Serialize( O* o, const mtc::array<mtc::xvalue<M>, M>& a )
  {
    if ( (o = ::Serialize( o, a.size() )) != nullptr )
      for ( auto p = a.begin(); p < a.end() && o != nullptr; ++p )
        o = p->Serialize( o );
    return o;
  }
template <class S, class M>
inline  S*        FetchFrom( S* s, mtc::array<mtc::xvalue<M>, M>& a )
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
  inline  unsigned  xvalue<M>::GetBufLen() const
  {
    switch ( vxtype )
    {
  # define derive_size_plain( _type_ )  case z_##_type_: return 1 + sizeof(_type_##_t);
  # define derive_size_smart( _type_ )  case z_##_type_: return 1 + ::GetBufLen( *(_type_##_t*)&chdata );
      derive_size_plain( char )
      derive_size_plain( byte )
      derive_size_plain( int16 )
      derive_size_plain( word16 )
      derive_size_plain( float )
      derive_size_plain( double )
      derive_size_smart( int32 )
      derive_size_smart( word32 )
      derive_size_smart( int64 )
      derive_size_smart( word64 )
  # undef derive_size_smart
  # undef derive_size_plain

      case z_charstr:
        {
          const char* pch = *(const char**)&chdata;
          unsigned    cch = (unsigned)(pch != nullptr ? w_strlen( pch ) : 0);

          return 1 + ::GetBufLen( cch ) + cch;
        }
      case z_widestr:
        {
          const widechar* pws = *(const widechar**)&chdata;
          unsigned        cch = (unsigned)(pws != NULL ? w_strlen( pws ) : 0);

          return 1 + ::GetBufLen( cch ) + cch * sizeof(widechar);
        }
//      case z_buffer:
//        return 1 + ::GetBufLen( get_zbuffer()->GetLength() ) + get_zbuffer()->GetLength();
      case z_zarray:
        return 1 + get_zarray()->GetBufLen();

      case z_array_char:    return 1 + ::GetBufLen( *(array<char_t, M>*)&chdata );
      case z_array_byte:    return 1 + ::GetBufLen( *(array<byte_t, M>*)&chdata );
      case z_array_int16:   return 1 + ::GetBufLen( *(array<int16_t, M>*)&chdata );
      case z_array_word16:  return 1 + ::GetBufLen( *(array<word16_t, M>*)&chdata );
      case z_array_int32:   return 1 + ::GetBufLen( *(array<int32_t, M>*)&chdata );
      case z_array_word32:  return 1 + ::GetBufLen( *(array<word32_t, M>*)&chdata );
      case z_array_int64:   return 1 + ::GetBufLen( *(array<int64_t, M>*)&chdata );
      case z_array_word64:  return 1 + ::GetBufLen( *(array<word64_t, M>*)&chdata );
      case z_array_float:   return 1 + ::GetBufLen( *(array<float_t, M>*)&chdata );
      case z_array_double:  return 1 + ::GetBufLen( *(array<double_t, M>*)&chdata );
      case z_array_charstr:  return 1 + ::GetBufLen( *(array<_auto_<char, M>, M>*)&chdata );
      case z_array_widestr:  return 1 + ::GetBufLen( *(array<_auto_<widechar, M>, M>*)&chdata );
      case z_array_xvalue:  return 1 + ::GetBufLen( *get_array_xvalue() );
      case z_array_zarray:  return 1 + ::GetBufLen( *get_array_zarray() );

      default:  return 0;
    }
  }

  template <class M>  template <class O>
  inline  O*  xvalue<M>::Serialize( O* o ) const
  {
    switch ( gettype() )
    {
  # define derive_put_plain( _type_ )  case z_##_type_: return ::Serialize( ::Serialize( o, vxtype ), &chdata, sizeof(_type_##_t) );
  # define derive_put_smart( _type_ )  case z_##_type_: return ::Serialize( ::Serialize( o, vxtype ), *(_type_##_t*)&chdata );
      derive_put_plain( char )
      derive_put_plain( byte )
      derive_put_plain( int16 )
      derive_put_plain( word16 )
      derive_put_plain( float )
      derive_put_plain( double )
      derive_put_smart( int32 )
      derive_put_smart( word32 )
      derive_put_smart( int64 )
      derive_put_smart( word64 )
  # undef derive_put_smart
  # undef derive_put_plain

      case z_charstr:
        {
          const char* pch = *(const char**)&chdata;
          unsigned    cch = (unsigned)(pch != nullptr ? w_strlen( pch ) : 0);

          return ::Serialize( ::Serialize( ::Serialize( o, vxtype ), cch ), pch, cch );
        }
      case z_widestr:
        {
          const widechar* pws = *(const widechar**)&chdata;
          unsigned        cch = (unsigned)(pws != nullptr ? w_strlen( pws ) : 0);

          return ::Serialize( ::Serialize( ::Serialize( o, vxtype ), cch ), pws, sizeof(widechar) * cch );
        }
/*      case fs_buffer:
        return fsPutBuff( fsPutMemo( fsPutChar( output, vxtype ), get_zbuffer()->GetLength() ),
                                                                  get_zbuffer()->GetBuffer(),
                                                                  get_zbuffer()->GetLength() );*/
      case z_zarray:
        return get_zarray()->Serialize( ::Serialize( o, vxtype ) );

  # define  derive_put_array( _type_ ) case z_array_##_type_: return ::Serialize( ::Serialize( o, vxtype ), *(array<_type_##_t, M>*)&chdata );
      derive_put_array( char )
      derive_put_array( byte )
      derive_put_array( float )
      derive_put_array( double )
      derive_put_array( int16 )
      derive_put_array( word16 )
      derive_put_array( int32 )
      derive_put_array( word32 )
      derive_put_array( int64 )
      derive_put_array( word64 )
  # undef derive_put_array
      case z_array_charstr: return ::Serialize( ::Serialize( o, vxtype ), *(array<_auto_<char, M>, M>*)&chdata );
      case z_array_widestr: return ::Serialize( ::Serialize( o, vxtype ), *(array<_auto_<widechar, M>, M>*)&chdata );
      case z_array_xvalue: return ::Serialize( ::Serialize( o, vxtype ), *(array<xvalue<M>, M>*)&chdata );
      case z_array_zarray: return ::Serialize( ::Serialize( o, vxtype ), *(array<zarray<M>, M>*)&chdata );

      default:  return 0;
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
      derive_get_plain( float )
      derive_get_plain( double )
      derive_get_smart( int32 )
      derive_get_smart( word32 )
      derive_get_smart( int64 )
      derive_get_smart( word64 )
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

  # define  derive_get_array( _type_ )                                \
    case z_##_type_: return ::FetchFrom( s, *set_##_type_() );
      derive_get_array( zarray )
      derive_get_array( array_char )
      derive_get_array( array_byte )
      derive_get_array( array_float )
      derive_get_array( array_double )
      derive_get_array( array_int16 )
      derive_get_array( array_word16 )
      derive_get_array( array_int32 )
      derive_get_array( array_word32 )
      derive_get_array( array_int64 )
      derive_get_array( array_word64 )
      derive_get_array( array_charstr )
      derive_get_array( array_zarray )
      derive_get_array( array_xvalue )
  # undef derive_get_array

      default:  return 0;
    }
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
    return avalue.gettype() != 0xff ? 0 : ptrtop != NULL ? 1 + ptrtop->lookup( inc_assign( keybuf, ptrtop->chnode ), 0, buflen - 1 ) : -1;
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
      if ( b.GetLen() <= (int)(l + sizeof(widechar)) && b.SetLen( l + 0x100 ) != 0 )  return ENOMEM;
        else {  b[l] = p->chnode;  b[l + 1] = 0;  b[l + 2] = 0;  }

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
  inline  unsigned  zarray<M>::ztree::GetBufLen() const
  {
    int       branch = plain_branchlen();
    word16_t  lstore = (branch > 0 ? 0x8000 + branch : size()) + (avalue.gettype() != 0xff ? 0x4000 : 0);
    unsigned  buflen = ::GetBufLen( lstore );

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
      unsigned  sublen = p->GetBufLen();
      buflen += ::GetBufLen( sublen ) + sublen;
    }

    return buflen;
  }

  template <class M>  template <class O>
  inline  O*   zarray<M>::ztree::Serialize( O* o ) const
  {
    int       branch = plain_branchlen();
    word16_t  lstore = (branch > 0 ? 0x8000 + branch : this->GetLen()) + (avalue.gettype() != 0xff ? 0x4000 : 0);

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
      unsigned  sublen = p->GetBufLen();

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

    if ( (lfetch & 0x4000) != 0 && (s = avalue.FetchFrom( ::FetchFrom( s, (char&)keyset ) )) == nullptr )
      return nullptr;

    if ( (lfetch & 0x8000) != 0 )
    {
      ztree*  pbeg = this;
      int     size = lfetch & 0xfff;

      while ( size-- > 0 )
      {
        if ( pbeg->SetLen( 1 ) == 0 ) pbeg = *pbeg;
          else  return nullptr;
        if ( (s = ::FetchFrom( s, (char&)pbeg->chnode )) == nullptr )
          return nullptr;
      }

      return pbeg->FetchFrom( s );
    }
      else
    if ( this->SetLen( lfetch & 0xfff ) == 0 )
    {
      for ( auto p = this->begin(); p < this->end() && s != nullptr; ++p )
      {
        unsigned  sublen;

        s = p->FetchFrom( ::FetchFrom( ::FetchFrom( s, (char&)p->chnode ), sublen ) );
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

  template <class M>
  inline  int zarray<M>::GetNextKey( void* keybuf, int cchkey, int buflen ) const
  {
    if ( zhandler == NULL || cchkey == buflen )
      return (unsigned)-1;
    if ( cchkey >= 0 )
      ((char*)keybuf)[cchkey++] = '\0';
    return zhandler->lookup( (byte_t*)keybuf, cchkey, buflen );
  }

  template <class M>
  inline  unsigned  zarray<M>::GetKeyType( const void* ptrkey, int cchkey ) const
  {
    const ztree*  zvalue = zhandler != NULL ? zhandler->search( (const byte_t*)ptrkey, cchkey ) : NULL;
    return zvalue != NULL ? zvalue->keyset : (unsigned)-1;
  }

  template <class M>
  inline  unsigned  zarray<M>::GetBufLen() const
  {
    return zhandler != NULL ? zhandler->GetBufLen() : 1;
  }

  template <class M>  template <class O>
  inline  O*        zarray<M>::Serialize( O* o ) const
  {
    return zhandler != nullptr ? zhandler->Serialize( o ) : ::Serialize( o, (char)0 );
  }

  template <class M>  template <class S>
  inline  S*        zarray<M>::FetchFrom( S*  s )
  {
    if ( zhandler != nullptr && --zhandler->rcount == 0 )
      delete zhandler;
    if ( (zhandler = M().template allocate<zdata>()) == nullptr )
      return nullptr;

    return (S*)zhandler->FetchFrom( s );
  }

}

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif  // _MSC_VER

# endif  // __zarray_h__
