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

  template <class C, class V, class M = def_alloc>
  class   _base_stringmap_
  {
    M   allocator;

    struct  keyrec
    {
      V         val;
      unsigned  pos;
      keyrec*   lpn;

    public:
      keyrec( const V&  t, unsigned p, keyrec* n ): val( t ), pos( p ), lpn( n )  {}
      keyrec(              unsigned p, keyrec* n ): pos( p ), lpn( n )  {}
    };

  protected:  // allocation
    template <class... constructor_args>
    keyrec* Create( const C*  k, size_t l, constructor_args... a )
      {
        keyrec* palloc;

        if ( (palloc = (keyrec*)allocator.alloc( sizeof(keyrec) + (l + 1) * sizeof(C) )) != nullptr )
          w_strncpy( (C*)(1 + new( palloc ) keyrec( a... )), k, l )[l] = 0;

        return palloc;
      }

  public:   // typedef for abstract algorithm
    typedef V   DataType;

  public:
    _base_stringmap_( unsigned tablen = 69959 ): pitems( nullptr ), maplen( tablen ), ncount( 0 ) {}
   ~_base_stringmap_();
    _base_stringmap_( const _base_stringmap_& ) = delete;
    _base_stringmap_& operator = ( const _base_stringmap_& ) = delete;

  public:     // methods
    int           Append( const _base_stringmap_& );
    int           Delete( const C*, size_t l = (size_t)-1 );
    void          DelAll();
    unsigned      GetLen() const  {  return ncount;  }
    unsigned      MapLen() const  {  return maplen;  }
    const C*      KeyStr( const C*, size_t l = (size_t)-1 ) const;

          V*      AddKey( const C*, size_t l, const V& v = V() );
          V*      Insert( const C*,           const V& v = V() );
    const V*      Search( const C*, size_t l = (size_t)-1 ) const;
          V*      Search( const C*, size_t l = (size_t)-1 );

  public:     // stl compat
    int     size() const  {  return GetLen();  }

  // Enumerator support methods
    const void*         Enum( const void* ) const;
          void*         Enum( void* );
    static  const C*    GetKey( const void* pvn ) {  assert( pvn != nullptr ); return (const C*)(1 + (keyrec*)pvn);  }
    static  const V&    GetVal( const void* pvn ) {  assert( pvn != nullptr ); return ((keyrec*)pvn)->val;  }
    static        V&    GetVal(       void* pvn ) {  assert( pvn != nullptr ); return ((keyrec*)pvn)->val;  }

    template <class action>
    int           for_each( action ) const;
    template <class action>
    int           for_each( action );
    template <class ifcond>
    void          DeleteIf( ifcond _if_ );

  protected:  // helpers
    int     NewMap()
      {
        assert( pitems == nullptr && maplen != 0 );

        if ( pitems != nullptr || maplen == 0 )
          return EINVAL;
        if ( (pitems = (keyrec**)allocator.alloc( maplen * sizeof(keyrec*) )) == nullptr )
          return ENOMEM;
        for ( auto p = pitems; p < pitems + maplen; )
          *p++ = nullptr;
        return 0;
      }

    unsigned  gethash( const C* pch, size_t  cch ) const
      {
        unsigned  int nHash = 0;

        assert( cch != (size_t)-1 );
        while ( cch-- > 0 )
          nHash = (nHash << 5) + nHash + *pch++;
        return nHash;
      }
    bool      iseq( const C* p1, const C* p2, size_t  cc ) const
      {
        assert( cc != (size_t)-1 );

        while ( cc > 0 && *p1++ == *p2++ )
          --cc;
        return cc == 0 && *p1 == 0;
      }

  private:
    keyrec**  pitems;
    unsigned  maplen;
    unsigned  ncount;

  };

  template <class V, class M = def_alloc>
  class stringmap: public _base_stringmap_<char, V, M>
  {
    public: stringmap( unsigned maplen = 69959 ): _base_stringmap_<char, V, M>( maplen ) {}
  };

  template <class V, class M = def_alloc>
  class widestringmap: public _base_stringmap_<widechar, V, M>
  {
    public: widestringmap( unsigned maplen = 69959 ): _base_stringmap_<widechar, V, M>( maplen ) {}
  };

  // Map inline implementation

  template <class C, class V, class M>
  _base_stringmap_<C, V, M>::~_base_stringmap_()
  {
    if ( pitems != nullptr )
    {
      DelAll();
      allocator.free( pitems );
    }
  }

  template <class C, class V, class M>
  int   _base_stringmap_<C, V, M>::Append( const _base_stringmap_<C, V, M>& s )
    {
      for ( const void* p = nullptr; (p = e.Enum( p )) != nullptr; )
        if ( Insert( GetKey( p ), GetVal( p ) ) == nullptr )
          return ENOMEM;

      return 0;
    }

  template <class C, class V, class M>
  int   _base_stringmap_<C, V, M>::Delete( const C* k, size_t l )
    {
      if ( l == (size_t)-1 )
        for ( l = 0; k[l] != 0; ++l ) (void)0;

      if ( pitems != nullptr && ncount != 0 )
      {
        unsigned  pos = gethash( k, l ) % maplen;
        auto      ptr = &pitems[pos];

        assert( *ptr == nullptr || pos == (*ptr)->pos );

        while ( *ptr != nullptr && !iseq( (const C*)(1 + *ptr), k, l ) )
          ptr = &(*ptr)->lpn;

        if ( *ptr != nullptr )
        {
          keyrec* del = *ptr;
          
          *ptr = del->lpn;
            del->~keyrec();
            allocator.free( del );
          --ncount;
        }
        return 0;
      }
      return EINVAL;
    }

  template <class C, class V, class M>
  void   _base_stringmap_<C, V, M>::DelAll()
    {
      keyrec* del;

      if ( pitems != nullptr )
        for ( auto p = pitems; p < pitems + maplen; ++p )
          while ( (del = *p) != nullptr )
          {
            *p = del->lpn;
              del->~keyrec();
            allocator.free( del );
          }
      ncount = 0;
    }

  template <class C, class V, class M>
  const C*  _base_stringmap_<C, V, M>::KeyStr( const C* k, size_t l ) const
    {
      if ( l == (size_t)-1 )
        for ( l = 0; k[l] != 0; ++l ) (void)0;

      if ( pitems != nullptr )
      {
        unsigned  pos = gethash( k, l ) % maplen;
        auto      ptr = pitems[pos];

        assert( ptr == nullptr || pos == ptr->pos );

        while ( ptr != nullptr && !iseq( (const C*)(ptr + 1), k, l ) )
          ptr = ptr->lpn;

        return ptr != nullptr ? (const C*)(ptr + 1) : nullptr;
      }
      return nullptr;
    }

  template <class C, class V, class M>
  V*    _base_stringmap_<C, V, M>::Insert( const C* k, const V& v )
    {
      return AddKey( k, -1, v );
    }

  template <class C, class V, class M>
  V*    _base_stringmap_<C, V, M>::AddKey( const C* k, size_t l, const V& v )
    {
      size_t    cch = l == (size_t)-1 ? w_strlen( k ) : l;
      unsigned  pos = gethash( k, cch ) % maplen;
      keyrec*   ptr;

      if ( pitems == nullptr && NewMap() != 0 )
        return nullptr;

      if ( (ptr = Create( k, cch, v, pos, pitems[pos] )) != nullptr ) pitems[pos] = ptr;
        else return nullptr;

      return (++ncount, &ptr->val);
    }

  template <class C, class V, class M>
  const V*  _base_stringmap_<C, V, M>::Search( const C* k, size_t l ) const
    {
      if ( pitems == nullptr )
        return nullptr;

      if ( l == (size_t)-1 )
        for ( l = 0; k[l] != 0; ++l ) (void)NULL;

      for ( auto p = pitems[gethash( k, l ) % maplen]; p != nullptr; p = p->lpn )
        if ( iseq( (const C*)(p + 1), k, l ) )
          return &p->val;

      return nullptr;
    }

  template <class C, class V, class M>
  V*        _base_stringmap_<C, V, M>::Search( const C* k, size_t l )
    {
      if ( pitems == nullptr )
        return nullptr;

      if ( l == (size_t)-1 )
        for ( l = 0; k[l] != 0; ++l ) (void)NULL;

      for ( auto p = pitems[gethash( k, l ) % maplen]; p != nullptr; p = p->lpn )
        if ( iseq( (const C*)(p + 1), k, l ) )
          return &p->val;

      return nullptr;
    }

  template <class C, class V, class M>
  const void* _base_stringmap_<C, V, M>::Enum( const void* pvn ) const
    {
      auto  ppktop = pitems;
      auto  ppkend = pitems + maplen;

    // Check pitems initialized
      if ( pitems == nullptr )
        return nullptr;

    // For the first call, make valid object pointer
      if ( pvn != nullptr )
      {
        const keyrec* lpnext;

        if ( (lpnext = ((const keyrec*)pvn)->lpn) != nullptr )
          return lpnext;

        ppktop = pitems + ((const keyrec*)pvn)->pos + 1;
      }
        else
      ppktop = pitems;

      for ( ppkend = pitems + maplen; ppktop < ppkend && *ppktop == nullptr; ++ppktop )
        (void)NULL;

      return ppktop < ppkend ? *ppktop : nullptr;
    }

  template <class C, class V, class M>
  void*       _base_stringmap_<C, V, M>::Enum( void* pvn )
    {
      keyrec**  ppktop;
      keyrec**  ppkend;

    // Check pitems initialized
      if ( pitems == nullptr )
        return nullptr;

    // For the first call, make valid object pointer
      if ( pvn != nullptr )
      {
        keyrec* lpnext;

        if ( (lpnext = ((keyrec*)pvn)->lpn) != nullptr )
          return lpnext;

        ppktop = pitems + ((const keyrec*)pvn)->pos + 1;
      }
        else
      ppktop = pitems;

      for ( ppkend = pitems + maplen; ppktop < ppkend && *ppktop == nullptr; ++ppktop )
        (void)NULL;

      return ppktop < ppkend ? *ppktop : nullptr;
    }

  template <class C, class V, class M>
  template <class action>
  int       _base_stringmap_<C, V, M>::for_each( action _do_ )
    {
      const void* p = nullptr;
      int         e = 0;

      while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
        (void)0;

      return e;
    }

  template <class C, class V, class M>
  template <class action>
  int       _base_stringmap_<C, V, M>::for_each( action _do_ ) const
    {
      const void* p = nullptr;
      int         e = 0;

      while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
        (void)0;

      return e;
    }

  template <class C, class V, class M>
  template <class ifcond>
  void      _base_stringmap_<C, V, M>::DeleteIf( ifcond _if_ )
    {
      for ( auto p = Enum( nullptr ); p != nullptr; )
        if ( _if_( GetKey( p ), GetVal( p ) ) )
        {
          const C*  delkey = GetKey( p );

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
