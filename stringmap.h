/*

The MIT License (MIT)

Copyright (c) 2000-2016 Андрей Коваленко aka Keva
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
# if !defined( __mtc_stringmap_h__ )
# define  __mtc_stringmap_h__

# include <assert.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include "platform.h"
# include "wcsstr.h"

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 4996 )
# endif // _MSC_VER

namespace mtc
{

  template <class Chr, class Val, class M = def_alloc>
  class   _base_stringmap_
  {
    struct  keyrec
    {
      Val       val;
      unsigned  pos;
      keyrec*   lpn;

    protected:
            keyrec( const Val&  t, unsigned p, keyrec*  n ): val( t ), pos( p ), lpn( n )
              {}
            keyrec( Val&  t, unsigned p, keyrec*  n ): val( t ), pos( p ), lpn( n )
              {}
            keyrec( unsigned p, keyrec* n ): pos( p ), lpn( n )
              {}
           ~keyrec()
              {}

    public:     // creation
              void    DelAll()
                {
                  if ( lpn != nullptr )
                    lpn->DelAll();
                  this->~keyrec();
                    M().free( this );
                }
      template <class _data_>
      static  keyrec* Create( const Chr*  k, size_t   l, _data_& t,
                              unsigned    p, keyrec*  n )
                {
                  keyrec* newrec;

                  if ( (newrec = (keyrec*)M().alloc( sizeof(keyrec) + (l + 1) * sizeof(Chr) )) != nullptr )
                  {
                    new ( newrec ) keyrec( t, p, n );
                      w_strncpy( (Chr*)(newrec + 1), k, l )[l] = 0;
                  }
                  return newrec;
                }
      static  keyrec* Create( const Chr*  k, size_t   l,
                              unsigned    p, keyrec*  n )
                {
                  keyrec* newrec;

                  if ( (newrec = (keyrec*)M().alloc( sizeof(keyrec) + (l + 1) * sizeof(Chr) )) != nullptr )
                  {
                    new ( newrec ) keyrec( p, n );
                      w_strncpy( (Chr*)(newrec + 1), k, l )[l] = 0;
                  }
                  return newrec;
                }
    };

  private:  // copy prevent section
    _base_stringmap_( const _base_stringmap_<Chr, Val, M>& );
    _base_stringmap_<Chr, Val, M>& operator = ( const _base_stringmap_<Chr, Val, M>& );

  public:   // typedef for abstract algorithm
    typedef Val   DataType;

  public:
                  _base_stringmap_( unsigned tablen = 69959 );
                 ~_base_stringmap_();
  // Map work methods
    int           Delete( const Chr*, size_t l = (size_t)-1 );
    void          DelAll();
    unsigned      GetLen() const;
    const Chr*    KeyStr( const Chr*, size_t l = (size_t)-1 ) const;
    unsigned      MapLen() const;

          Val*    AddKey( const Chr*, size_t l, const Val& v = Val() );
          Val*    Insert( const Chr*, const Val& v = Val() );
    const Val*    Search( const Chr*, size_t l = (size_t)-1 ) const;
          Val*    Search( const Chr*, size_t l = (size_t)-1 );

  // Enumerator support methods
    void*               Enum( const void* );
    static  const Chr*  GetKey( const void* );
    static  Val&        GetVal( const void* );
    template <class action>
    int                 for_each( action ) const;
    template <class action>
    int                 for_each( action );
    template <class ifcond>
    void                DeleteIf( ifcond );

  protected:  // helpers
    int       Alloc()
                {
                  assert( pitems == nullptr );
                  assert( maplen != 0 );

                  if ( pitems != nullptr || maplen == 0 )
                    return EINVAL;
                  if ( (pitems = (keyrec**)M().alloc( maplen * sizeof(keyrec*) )) == nullptr )
                    return ENOMEM;
                  else memset( pitems, 0, maplen * sizeof(keyrec*) );
                    return 0;
                }
    unsigned  gethash( const Chr* pch, size_t  cch ) const
                {
                  unsigned  int nHash = 0;

                  assert( cch != (size_t)-1 );
                  while ( cch-- > 0 )
                    nHash = (nHash << 5) + nHash + *pch++;
                  return nHash;
                }
    bool      iseq( const Chr* p1, const Chr* p2, size_t  cc ) const
                {
                  assert( cc != (size_t)-1 );

                  while ( cc > 0 && *p1++ == *p2++ )
                    --cc;
                  return cc == 0 && *p1 == '\0';
                }
  private:
    keyrec**  pitems;
    unsigned  maplen;
    unsigned  ncount;

  };

  template <class Val, class allocator = def_alloc>
  class stringmap: public _base_stringmap_<char, Val, allocator>
  {
    public: stringmap( unsigned maplen = 69959 ): _base_stringmap_<char, Val, allocator>( maplen ) {}
  };

  template <class Val, class allocator = def_alloc>
  class widestringmap: public _base_stringmap_<widechar, Val, allocator>
  {
    public: widestringmap( unsigned maplen = 69959 ): _base_stringmap_<widechar, Val, allocator>( maplen ) {}
  };

  // Map inline implementation

  template <class Chr, class Val, class M>
  inline  _base_stringmap_<Chr, Val, M>::_base_stringmap_( unsigned tablen ):
    pitems( nullptr ), maplen( tablen ), ncount( 0 )
  {
  }

  template <class Chr, class Val, class M>
  inline  _base_stringmap_<Chr, Val, M>::~_base_stringmap_()
  {
    DelAll();
  }

  template <class Chr, class Val, class M>
  inline  int   _base_stringmap_<Chr, Val, M>::Delete( const Chr* k, size_t l )
  {
    if ( l == (size_t)-1 )
      for ( l = 0; k[l] != 0; ++l ) (void)0;

    if ( pitems != nullptr && ncount != 0 )
    {
      unsigned  nhcode = gethash( k, l ) % maplen;
      auto      ppitem = &pitems[nhcode];
      keyrec*   lpfree;

      assert( *ppitem == nullptr || nhcode == (*ppitem)->pos );

      while ( *ppitem != nullptr && !iseq( (const Chr*)(1 + *ppitem), k, l ) )
        ppitem = &(*ppitem)->lpn;
      if ( *ppitem != nullptr )
      {
        lpfree = *ppitem;
        *ppitem = lpfree->lpn;
        lpfree->lpn = nullptr;
        lpfree->DelAll();
        --ncount;
      }
      return 0;
    }
    return EINVAL;
  }

  template <class Chr, class Val, class M>
  inline  void  _base_stringmap_<Chr, Val, M>::DelAll( )
  {
    if ( pitems != nullptr )
    {
      for ( auto p = pitems; p < pitems + maplen; )
        (*p++)->DelAll();
      M().free( pitems );
        pitems = nullptr;
    }
    ncount = 0;
  }

  template <class Chr, class Val, class M>
  inline  unsigned  _base_stringmap_<Chr, Val, M>::GetLen() const
  {
    return ncount;
  }

  template <class Chr, class Val, class M>
  inline  const Chr* _base_stringmap_<Chr, Val, M>::KeyStr( const Chr* k, size_t l ) const
  {
    if ( l == (size_t)-1 )
      for ( l = 0; k[l] != 0; ++l ) (void)0;

    if ( pitems != nullptr )
    {
      unsigned  nhcode = gethash( k, l ) % maplen;
      keyrec*   lpitem = pitems[nhcode];

      assert( lpitem == nullptr || nhcode == lpitem->pos );

      while ( lpitem != nullptr && !iseq( (Chr*)(lpitem + 1), k, l ) )
        lpitem = lpitem->lpn;
      return ( lpitem != nullptr ? (Chr*)(lpitem + 1) : nullptr );
    }
      else
    return nullptr;
  }

  template <class Chr, class Val, class M>
  inline  unsigned  _base_stringmap_<Chr, Val, M>::MapLen() const
  {
    return maplen;
  }

  template <class Chr, class Val, class M>
  inline  const Val*  _base_stringmap_<Chr, Val, M>::Search( const Chr* k, size_t l ) const
  {
    if ( pitems == nullptr )
      return nullptr;

    if ( l == (size_t)-1 )
      for ( l = 0; k[l] != 0; ++l ) (void)NULL;

    for ( auto p = pitems[gethash( k, l ) % maplen]; p != nullptr; p = p->lpn )
      if ( iseq( (Chr*)(p + 1), k, l ) )
        return &p->val;

    return nullptr;
  }

  template <class Chr, class Val, class M>
  inline  Val*  _base_stringmap_<Chr, Val, M>::Search( const Chr* k, size_t l )
  {
    if ( pitems == nullptr )
      return nullptr;

    if ( l == (size_t)-1 )
      for ( l = 0; k[l] != 0; ++l ) (void)0;

    for ( auto p = pitems[gethash( k, l ) % maplen]; p != nullptr; p = p->lpn )
      if ( iseq( (Chr*)(p + 1), k, l ) )
        return &p->val;

    return nullptr;
  }

  template <class Chr, class Val, class M>
  inline  Val*  _base_stringmap_<Chr, Val, M>::AddKey( const Chr* k, size_t l, const Val& t )
  {
    keyrec*   newrec;
    unsigned  hindex;

  // ensure
    if ( l == (size_t)-1 )
      for ( l = 0; k[l] != 0; ++l ) (void)0;

  // Ensure the map is allocated
    if ( pitems == nullptr && Alloc() != 0 )
      return nullptr;

  // Allocate the item
    hindex = gethash( k, l ) % maplen;

    if ( (newrec = keyrec::Create( k, l, t, hindex, pitems[hindex] )) == NULL )
      return nullptr;
    pitems[hindex] = newrec;
      ++ncount;

    return &newrec->val;
  }

  template <class Chr, class Val, class M>
  inline  Val*  _base_stringmap_<Chr, Val, M>::Insert( const Chr* k, const Val& t )
  {
    size_t    cchstr = w_strlen( k );
    unsigned  nindex = gethash( k, w_strlen( k ) ) % maplen;
    keyrec*   newrec;

  // Ensure the map is allocated
    if ( pitems == nullptr && Alloc() != 0 )
      return nullptr;

  // Allocate the item
    if ( (newrec = keyrec::Create( k, cchstr, t, nindex, pitems[nindex] )) == nullptr )
      return nullptr;
    pitems[nindex] = newrec;
      ++ncount;

    return &newrec->val;
  }

  template <class Chr, class Val, class M>
  inline  void*     _base_stringmap_<Chr, Val, M>::Enum( const void* pvn )
  {
    keyrec*   curguy = (keyrec*)pvn;
    unsigned  nindex;

  // Check pitems initialized
    if ( pitems == nullptr )
      return nullptr;

  // For the first call, make valid object pointer
    if ( curguy == NULL )
    {
      for ( nindex = 0; nindex < maplen; nindex++ )
        if ( (curguy = pitems[nindex]) != nullptr )
          break;
    }
      else
  // Get the next item for the selected one    
    {
      keyrec* lpcurr = curguy;

      if ( (curguy = curguy->lpn) == nullptr )
      {
        for ( nindex = lpcurr->pos + 1; nindex < maplen; nindex++ )
          if ( (curguy = pitems[nindex]) != nullptr )
            break;
      }
    }
    return curguy;
  }

  template <class Chr, class Val, class M>
  inline  const Chr* _base_stringmap_<Chr, Val, M>::GetKey( const void*  pvn )
  {
    assert( pvn != NULL );

    return (Chr*)(1 + (keyrec*)pvn);
  }

  template <class Chr, class Val, class M>
  inline  Val&      _base_stringmap_<Chr, Val, M>::GetVal( const void*  pvn )
  {
    assert( pvn != NULL );

    return ((keyrec*)pvn)->val;
  }

  template <class Chr, class Val, class M>
  template <class action>
  inline  int       _base_stringmap_<Chr, Val, M>::for_each( action _do_ )
  {
    const void* p = nullptr;
    int         e = 0;

    while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
      (void)0;

    return e;
  }

  template <class Chr, class Val, class M>
  template <class action>
  inline  int       _base_stringmap_<Chr, Val, M>::for_each( action _do_ ) const
  {
    const void* p = nullptr;
    int         e = 0;

    while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
      (void)0;

    return e;
  }

  template <class Chr, class Val, class M>
  template <class ifcond>
  inline  void      _base_stringmap_<Chr, Val, M>::DeleteIf( ifcond _if_ )
  {
    for ( void* p = Enum( NULL ); p != NULL; )
      if ( _if_( GetKey( p ), GetVal( p ) ) )
      {
        const char* delkey = GetKey( p );

        p = Enum( p );  Delete( delkey );
      }
        else
      {
        p = Enum( p );
      }
  }

}  // mtc namespace

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif // _MSC_VER

# endif  // __mtc_stringmap_h__
