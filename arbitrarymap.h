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
# if !defined( __mtc_arbitrarymap_h__ )
# define  __mtc_arbitrarymap_h__
# include "platform.h"
# include <assert.h>
# include <stdlib.h>
# include <errno.h>

namespace mtc
{

  template <class V, class M = def_alloc>
  class   arbitrarymap
  {
    M   allocator;

    struct  keyrec
    {
      V         val;
      unsigned  pos;
      keyrec*   lpn;
      unsigned  len;
      char      key[1];

    public:   // construction
      keyrec( const V& v, unsigned p, keyrec* n ): val( v ), pos( p ), lpn( n ) {}
      keyrec(             unsigned p, keyrec* n ):           pos( p ), lpn( n ) {}

    };

  protected:  // allocation
    template <class... constructor_args>
    keyrec* Create( const void* k, unsigned l, constructor_args... a )
      {
        keyrec* palloc;

        if ( (palloc = (keyrec*)allocator.alloc( l - 1 + sizeof(keyrec) )) != nullptr )
        {
          new( palloc ) keyrec( a... );
            memcpy( palloc->key, k, palloc->len = l );
        }
        return palloc;
      }

  public:     // construction
    arbitrarymap( unsigned tablen = 69959 );
   ~arbitrarymap();
    arbitrarymap( const arbitrarymap& ) = delete;
    arbitrarymap& operator = ( const arbitrarymap& ) = delete;
    
  public:     // methods
    int           Append( const arbitrarymap& );
    int           Delete( const void*, unsigned );
    int           Delete( const widechar* s ) {  return Delete( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    int           Delete( const char* s )     {  return Delete( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    void          DelAll();
    unsigned      GetLen() const  {  return ncount;  }
    unsigned      MapLen() const  {  return maplen;  }
          V*      Insert( const void*, unsigned, const V& v = V() );
          V*      Insert( const widechar* s, const V& v = V() ) {  return Insert( s, sizeof(*s) * (w_strlen( s ) + 1), v );  }
          V*      Insert( const char*     s, const V& v = V() ) {  return Insert( s, sizeof(*s) * (w_strlen( s ) + 1), v );  }
    const V*      Search( const void*, unsigned ) const;
    const V*      Search( const widechar* s ) const {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
    const V*      Search( const char*     s ) const {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
          V*      Search( const void*, unsigned );
          V*      Search( const widechar* s ) {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }
          V*      Search( const char*     s ) {  return Search( s, sizeof(*s) * (w_strlen( s ) + 1) );  }

  public:     // stl compat
    int     size() const  {  return GetLen();  }

  public:     // template wrappers
    template <class K>  int   Delete( const K& k )  {  return Delete( k.begin(), k.size() );  }
    template <class K>  V*    Insert( const K& k, const V& v = V() )  {  return Insert( k.begin(), k.size(), v );  }

  // enumerator support methods
    const void*         Enum( const void* ) const;
          void*         Enum( void* );
    static  const void* GetKey( const void* );
    static  unsigned    KeyLen( const void* );
    static  const V&    GetVal( const void* );
    static  V&          GetVal(       void* );

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
    unsigned  gethash( const unsigned char* p, unsigned c ) const
      {
        unsigned  int nHash = 0;
        while ( c-- > 0 )
          nHash = (nHash << 5) + nHash + *p++;
        return nHash;
      }
    bool      isequal( const void*  p1, unsigned l1,
                       const void*  p2, unsigned l2 ) const
      {
        return l1 == l2 && memcmp( p1, p2, l1 ) == 0;
      }

  private:
    keyrec**  pitems;
    unsigned  maplen;
    unsigned  ncount;

  };

  // Map inline implementation

  template <class V, class M>
  arbitrarymap<V, M>::arbitrarymap( unsigned tablen ): pitems( nullptr ), maplen( tablen ), ncount( 0 )
    {
    }

  template <class V, class M>
  arbitrarymap<V, M>::~arbitrarymap()
    {
      if ( pitems != nullptr )
      {
        DelAll();
        allocator.free( pitems );
      }
    }

  template <class V, class M>
  int   arbitrarymap<V, M>::Append( const arbitrarymap<V, M>& s )
    {
      for ( const void* p = nullptr; (p = e.Enum( p )) != nullptr; )
        if ( Insert( GetKey( p ), KeyLen( p ), GetVal( p ) ) == nullptr )
          return ENOMEM;

      return 0;
    }

  template <class V, class M>
  int   arbitrarymap<V, M>::Delete( const void* k, unsigned l )
    {
      if ( pitems != nullptr && ncount != 0 )
      {
        unsigned  pos = gethash( (const byte_t*)k, l ) % maplen;
        keyrec**  ptr = &pitems[pos];

        assert( *ptr == nullptr || pos == (*ptr)->pos );

        while ( *ptr != nullptr && !isequal( (*ptr)->key, (*ptr)->len, k, l ) )
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

  template <class V, class M>
  void  arbitrarymap<V, M>::DelAll()
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

  template <class V, class M>
  V*    arbitrarymap<V, M>::Insert( const void* k, unsigned l, const V& v )
    {
      unsigned  pos = gethash( (const byte_t*)k, l ) % maplen;
      keyrec*   ptr;

      if ( pitems == nullptr && NewMap() != 0 )
        return nullptr;

      if ( (ptr = Create( k, l, v, pos, pitems[pos] )) != nullptr ) pitems[pos] = ptr;
        else return nullptr;

      ++ncount;
        return 0;
    }

  template <class V, class M>
  const V*  arbitrarymap<V, M>::Search( const void* k, unsigned l ) const
    {
      if ( pitems != NULL )
      {
        unsigned      pos = gethash( (const unsigned char*)k, l ) % maplen;
        const keyrec* ptr = pitems[nhcode];

        assert( ptr == NULL || pos == lpitem->pos );

        while ( ptr != nullptr && !isequal( ptr->key, ptr->len, k, l ) )
          ptr = ptr->lpn;

        return ptr != nullptr ? &ptr->val : nullptr;
      }
      return nullptr;
    }

  template <class V, class M>
  V*      arbitrarymap<V, M>::Search( const void* k, unsigned l )
    {
      if ( pitems != NULL )
      {
        unsigned  pos = gethash( (const unsigned char*)k, l ) % maplen;
        keyrec*   ptr = pitems[nhcode];

        assert( ptr == NULL || pos == lpitem->pos );

        while ( ptr != nullptr && !isequal( ptr->key, ptr->len, k, l ) )
          ptr = ptr->lpn;

        return ptr != nullptr ? &ptr->val : nullptr;
      }
      return nullptr;
    }

  template <class V, class M>
  const void* arbitrarymap<V, M>::Enum( const void* pvn ) const
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

  template <class V, class M>
  void* arbitrarymap<V, M>::Enum( void* pvn )
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

  template <class V, class M>
  const void* arbitrarymap<V, M>::GetKey( const void*  pvn )
    {
      assert( pvn != nullptr );

      return ((keyrec*)pvn)->key;
    }

  template <class V, class M>
  unsigned    arbitrarymap<V, M>::KeyLen( const void*  pvn )
  {
    assert( pvn != nullptr );

    return ((keyrec*)pvn)->len;
  }

  template <class V, class M>
  inline  const V&  arbitrarymap<V, M>::GetVal( const void*  pvn )
  {
    assert( pvn != nullptr );

    return ((const keyrec*)pvn)->val;
  }

  template <class V, class M>
  inline  V&  arbitrarymap<V, M>::GetVal( void*  pvn )
  {
    assert( pvn != nullptr );

    return ((const keyrec*)pvn)->val;
  }

}

# endif  // __mtc_arbitrarymap_h__
