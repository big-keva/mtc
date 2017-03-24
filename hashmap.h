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
# if !defined( __mtc_hashmap_h__ )
# define  __mtc_hashmap_h__

# include "platform.h"
# include <cassert>
# include <string.h>
# include <stdlib.h>
# include <errno.h>

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 )
#   pragma warning( disable: 4710 )
# endif // _MSC_VER

namespace mtc
{

  template <class T>
  inline  unsigned  hashmap_gethash( const T& /*t*/ )
    {  return 0;  }

  inline  unsigned  hashmap_gethash( char k )
    {  return (unsigned)( k >= 0 ? k : -k );  }
  inline  unsigned  hashmap_gethash( short k )
    {  return (unsigned)( k >= 0 ? k : -k );  }
  inline  unsigned  hashmap_gethash( int k )
    {  return (unsigned)( k >= 0 ? k : -k );  }
  inline  unsigned  hashmap_gethash( long k )
    {  return (unsigned)( k >= 0 ? k : -k );  }
  inline  unsigned  hashmap_gethash( unsigned char k )
    {  return k;  }
  inline  unsigned  hashmap_gethash( unsigned short k )
    {  return k;  }
  inline  unsigned  hashmap_gethash( unsigned int k )
    {  return k;  }
  inline  unsigned  hashmap_gethash( unsigned long k )
    {  return (unsigned)k;  }
  inline  unsigned  hashmap_gethash( const char* psz )
    {
      unsigned  int nHash = 0;
      while ( *psz )
        nHash = (nHash << 5) + nHash + (unsigned char)*psz++;
      return nHash;
    }
  inline  unsigned  hashmap_gethash( const unsigned char* psz )
    {
      unsigned  int nHash = 0;
      while ( *psz )
        nHash = (nHash << 5) + nHash + (unsigned char)*psz++;
      return nHash;
    }

  template <class T>
  inline  bool      hashmap_iseq( const T& t1, const T& t2 )
    {  return t1 == t2;  }
  inline  bool      hashmap_iseq( const char* p1, const char* p2 )
    {  return p1 == p2 || strcmp( p1, p2 ) == 0;  }
  inline  bool      hashmap_iseq( const unsigned char* p1, const unsigned char* p2 )
    {  return p1 == p2 || strcmp( (const char*)p1, (const char*)p2 ) == 0;  }

  template <class K, class V, class M = def_alloc>
  class   hashmap
  {
    M   allocator;

    struct  keyrec
    {
      K         key;
      V         val;
      unsigned  pos;
      keyrec*   lpn;

    public:   // construction
      keyrec( const K& k, const V& v, unsigned p, keyrec* n ): key( k ), val( v ), pos( p ), lpn( n ) {}
      keyrec( const K& k,             unsigned p, keyrec* n ): key( k ), pos( p ), lpn( n ) {}

    };

  protected:  // allocation
    template <class... constructor_args>
    keyrec* Create( const K& k, constructor_args... a )
      {
        keyrec* palloc;

        if ( (palloc = (keyrec*)allocator.alloc( sizeof(keyrec) )) != nullptr )
          new( palloc ) keyrec( k, a... );

        return palloc;
      }

  public:     // construction
    hashmap( unsigned tablen = 69959 ): pitems( nullptr ), maplen( tablen ), ncount( 0 )  {}
   ~hashmap();
    hashmap( const hashmap& ) = delete;
    hashmap& operator = ( const hashmap& ) = delete;

  public:     // methods
    int           Append( const hashmap& );
    int           Delete( const K& );
    void          DelAll();
    unsigned      GetLen() const  {  return ncount;  }
    unsigned      MapLen() const  {  return maplen;  }
    int           Rehash( unsigned  newlen );
          V*      Insert( const K&, const V& v = V() );
    const V*      Search( const K& ) const;
          V*      Search( const K& );

  public:     // stl compat
    int     size() const  {  return GetLen();  }

  // enumerator support methods
    const void*         Enum( const void* ) const;
          void*         Enum( void* );
    static const K&     GetKey( const void* p ) {  assert( pvn != nullptr ); return ((keyrec*)pvn)->key;  }
    static       K&     GetKey(       void* p ) {  assert( pvn != nullptr ); return ((keyrec*)pvn)->key;  }
    static const V&     GetVal( const void* p ) {  assert( pvn != nullptr ); return ((keyrec*)pvn)->val;  }
    static       V&     GetVal(       void* p ) {  assert( pvn != nullptr ); return ((keyrec*)pvn)->val;  }

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

  private:
    keyrec**  pitems;
    unsigned  maplen;
    unsigned  ncount;

  };

  // Map inline implementation

  template <class K, class V, class M>
  hashmap<K, V, M>::~hashmap()
    {
      if ( pitems != nullptr )
      {
        DelAll();
        allocator.free( pitems );
      }
    }

  template <class K, class V, class M>
  int   hashmap<K, V, M>::Append( const hashmap<K, V, M>& s )
    {
      for ( const void* p = nullptr; (p = e.Enum( p )) != nullptr; )
        if ( Insert( GetKey( p ), GetVal( p ) ) == nullptr )
          return ENOMEM;

      return 0;
    }

  template <class K, class V, class M>
  inline  int   hashmap<K, V, M>::Delete( const K& k )
    {
      if ( pitems != nullptr && ncount != 0 )
      {
        unsigned  pos = hashmap_gethash( k ) % maplen;
        keyrec**  ptr = &pitems[pos];

        assert( *ptr == nullptr || pos == (*ptr)->pos );

        while ( *ptr != nullptr && !hashmap_iseq( (*ptr)->key, k ) )
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

  template <class K, class V, class M>
  inline  void  hashmap<K, V, M>::DelAll()
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

  template <class K, class V, class M>
  inline  int   hashmap<K, V, M>::Rehash( unsigned newlen )
  {
    if ( newlen == 0 )
      return EINVAL;

    if ( pitems != nullptr )
    {
      keyrec**  newitems;
      unsigned  oldindex;

      if ( (newitems = (keyrec**)allocator.alloc( newlen * sizeof(keyrec*) )) == NULL )
        return ENOMEM;
      else memset( newitems, 0, newlen * sizeof(keyrec*) );

      for ( oldindex = 0; oldindex < maplen; ++oldindex )
      {
        keyrec*   rehash = pitems[oldindex];
      
        while ( rehash != nullptr )
        {
          unsigned  newindex = hashmap_gethash( rehash->key ) % newlen;
          keyrec*   nextitem = rehash->lpn;

          rehash->lpn = newitems[rehash->pos = newindex];
            newitems[newindex] = rehash;
          rehash = nextitem;
        }
      }

      free( pitems );  pitems = newitems;
    }

    maplen = newlen;
    return 0;
  }

  template <class K, class V, class M>
  V*    hashmap<K, V, M>::Insert( const K& k, const V& v )
    {
      unsigned  pos = hashmap_gethash( k ) % maplen;
      keyrec*   ptr;

      if ( pitems == nullptr && NewMap() != 0 )
        return nullptr;

      if ( (ptr = Create( k, v, pos, pitems[pos] )) != nullptr ) pitems[pos] = ptr;
        else return nullptr;

      return (++ncount, &ptr->val);
    }

  template <class K, class V, class M>
  const V*  hashmap<K, V, M>::Search( const K& k ) const
    {
      if ( pitems != NULL )
      {
        unsigned      pos = hashmap_gethash( k ) % maplen;
        const keyrec* ptr = pitems[pos];

        assert( ptr == NULL || pos == ptr->pos );

        while ( ptr != nullptr && !hashmap_iseq( ptr->key, k ) )
          ptr = ptr->lpn;

        return ptr != nullptr ? &ptr->val : nullptr;
      }
      return nullptr;
    }

  template <class K, class V, class M>
  V*      hashmap<K, V, M>::Search( const K& k )
    {
      if ( pitems != NULL )
      {
        unsigned  pos = hashmap_gethash( k ) % maplen;
        keyrec*   ptr = pitems[pos];

        assert( ptr == NULL || pos == ptr->pos );

        while ( ptr != nullptr && !hashmap_iseq( ptr->key, k ) )
          ptr = ptr->lpn;

        return ptr != nullptr ? &ptr->val : nullptr;
      }
      return nullptr;
    }

  template <class K, class V, class M>
  const void* hashmap<K, V, M>::Enum( const void* pvn ) const
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

  template <class K, class V, class M>
  void* hashmap<K, V, M>::Enum( void* pvn )
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

  template <class K, class V, class M>
  template <class action>
  int       hashmap<K, V, M>::for_each( action _do_ ) const
    {
      const void* p = nullptr;
      int         e = 0;

      while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
        (void)0;

      return e;
    }

  template <class K, class V, class M>
  template <class action>
  inline  int       hashmap<K, V, M>::for_each( action _do_ )
    {
      void* p = nullptr;
      int   e = 0;

      while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
        (void)0;

      return e;
    }

  template <class K, class V, class M>
  template <class ifcond>
  void      hashmap<K, V, M>::DeleteIf( ifcond _if_ )
  {
    for ( auto p = Enum( nullptr ); p != nullptr; )
      if ( _if_( GetKey( p ), GetVal( p ) ) )
      {
        const K&  delkey = GetKey( p );

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

# endif  // __mtc_hashmap_h__
