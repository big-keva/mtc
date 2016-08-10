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

*/
# if !defined( __mtc_hashmap_h__ )
# define  __mtc_hashmap_h__

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

  template <class Key, class Val>
  class   hashmap
  {
    struct  keyrec
    {
      Key       key;
      Val       val;
      unsigned  pos;
      keyrec*   lpn;
    protected:
      template <class thekey, class theval>
      keyrec( thekey&  k, theval& t,
              unsigned p, keyrec* n ): key( k ), val( t ), pos( p ), lpn( n )
        {}
      template <class thekey>
      keyrec( thekey&  k,
              unsigned p, keyrec* n ): key( k ), pos( p ), lpn( n )
        {}
     ~keyrec()
        {}

    public:     // creation
      void    DelAll()
        {
          if ( this != nullptr )
          {
            if ( lpn != nullptr )
              lpn->DelAll();
            delete this;
          }
        }
      template <class thekey, class theval>
      static  keyrec* Create( thekey&   k, theval& t,
                              unsigned  p, keyrec* n )
                {
                  keyrec* newrec = (keyrec*)malloc( sizeof(keyrec) );

                  if ( newrec != nullptr )
                    new ( newrec ) keyrec( k, t, p, n );
                  return newrec;
                }
      template <class thekey>
      static  keyrec* Create( thekey    k,
                              unsigned  p, keyrec* n )
                {
                  keyrec* newrec = (keyrec*)malloc( sizeof(keyrec) );

                  if ( newrec != nullptr )
                    new ( newrec ) keyrec( k, p, n );
                  return newrec;
                }
    };

  private:    // copy prevent section
    hashmap( const hashmap<Key, Val>& );
    hashmap<Key, Val>& operator = ( const hashmap<Key, Val>& );

  public:     // construction
    hashmap( unsigned tablen = 69959 );
   ~hashmap();

  // Map work methods
    int           Delete( const Key& );
    void          DelAll();
    unsigned      GetLen() const;
    unsigned      MapLen() const;
    int           Rehash( unsigned  newlen );
    const Val*    Search( const Key& ) const;
    Val*          Search( const Key& );
    Val*          AddKey( const Key& );
    int           Insert( const Key&, const Val& );
    int           Insert( const Key&, Val& );

  // Enumerator support methods
    const void*   Enum( const void* ) const;
    void*         Enum( void* );
    static  Key&  GetKey( const void* );
    static  Val&  GetVal( const void* );
    template <class action>
    int           for_each( action ) const;
    template <class action>
    int           for_each( action );
    template <class ifcond>
    void          DeleteIf( ifcond _if_ );

  protected:  // helpers
    int       Alloc()
      {
        assert( pitems == nullptr );
        assert( maplen != 0 );

        if ( pitems != nullptr || maplen == 0 )
          return EINVAL;
        if ( (pitems = (keyrec**)malloc( maplen * sizeof(keyrec*) )) == nullptr )
          return ENOMEM;
        else memset( pitems, 0, maplen * sizeof(keyrec*) );
          return 0;
      }

  private:
    keyrec**  pitems;
    unsigned  maplen;
    unsigned  ncount;

  };

  // Map inline implementation

  template <class Key, class Val>
  inline  hashmap<Key, Val>::hashmap( unsigned tablen ):
    pitems( nullptr ), maplen( tablen ), ncount( 0 )
  {
  }

  template <class Key, class Val>
  inline  hashmap<Key, Val>::~hashmap()
  {
    DelAll();
  }

  template <class Key, class Val>
  inline  int   hashmap<Key, Val>::Delete( const Key& k )
  {
    if ( pitems != nullptr && ncount != 0 )
    {
      unsigned  nhcode = hashmap_gethash( k ) % maplen;
      keyrec**  ppitem = &pitems[nhcode];
      keyrec*   lpfree;

      assert( *ppitem == nullptr || nhcode == (*ppitem)->pos );

      while ( *ppitem != nullptr && !hashmap_iseq( (*ppitem)->key, k ) )
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

  template <class Key, class Val>
  inline  void  hashmap<Key, Val>::DelAll()
  {
    if ( pitems != nullptr )
    {
      for ( auto p = pitems; p < pitems + maplen; )
        (*p++)->DelAll();
      free( pitems );
        pitems = nullptr;
    }
    ncount = 0;
  }

  template <class Key, class Val>
  inline  unsigned  hashmap<Key, Val>::GetLen() const
  {
    return ncount;
  }

  template <class Key, class Val>
  inline  unsigned  hashmap<Key, Val>::MapLen() const
  {
    return maplen;
  }

  template <class Key, class Val>
  inline  int       hashmap<Key, Val>::Rehash( unsigned newlen )
  {
    if ( newlen == 0 )
      return EINVAL;

    if ( pitems != nullptr )
    {
      keyrec**  newitems;
      unsigned  oldindex;

      if ( (newitems = (keyrec**)malloc( newlen * sizeof(keyrec*) )) == NULL )
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

  template <class Key, class Val>
  inline  const Val*  hashmap<Key, Val>::Search( const Key& k ) const
  {
    if ( pitems != nullptr )
    {
      unsigned  nhcode = hashmap_gethash( k ) % maplen;
      keyrec*   lpitem = pitems[nhcode];

      assert( lpitem == nullptr || nhcode == lpitem->pos );

      while ( lpitem != nullptr && !hashmap_iseq( lpitem->key, k ) )
        lpitem = lpitem->lpn;
      return lpitem != nullptr ? &lpitem->val : nullptr;
    }
    return nullptr;
  }

  template <class Key, class Val>
  inline  Val*    hashmap<Key, Val>::Search( const Key& k )
  {
    if ( pitems != nullptr )
    {
      unsigned  nhcode = hashmap_gethash( k ) % maplen;
      keyrec*   lpitem = pitems[nhcode];

      assert( lpitem == nullptr || nhcode == lpitem->pos );

      while ( lpitem != nullptr && !hashmap_iseq( lpitem->key, k ) )
        lpitem = lpitem->lpn;
      return lpitem != nullptr ? &lpitem->val : nullptr;
    }
    return nullptr;
  }

  template <class Key, class Val>
  inline  Val*      hashmap<Key, Val>::AddKey( const Key& k )
  {
    unsigned  nindex = hashmap_gethash( k ) % maplen;
    keyrec*   newrec;
    int       nerror;

  // Ensure the map is allocated
    if ( pitems == nullptr && (nerror = Alloc()) != 0 )
      return nullptr;

  // Allocate the item
    if ( (newrec = keyrec::Create( k, nindex, pitems[nindex] )) == NULL )
      return nullptr;
    pitems[nindex] = newrec;
      ++ncount;

    return &newrec->val;
  }

  template <class Key, class Val>
  inline  int       hashmap<Key, Val>::Insert( const Key& k, const Val& t )
  {
    unsigned  nindex = hashmap_gethash( k ) % maplen;
    keyrec*   newrec;
    int       nerror;

  // Ensure the map is allocated
    if ( pitems == nullptr && (nerror = Alloc()) != 0 )
      return nerror;

  // Allocate the item
    if ( (newrec = keyrec::Create( k, t, nindex, pitems[nindex] )) == nullptr )
      return ENOMEM;
    pitems[nindex] = newrec;
      ++ncount;

    return 0;
  }

  template <class Key, class Val>
  inline  int       hashmap<Key, Val>::Insert( const Key& k, Val& t )
  {
    unsigned  nindex = hashmap_gethash( k ) % maplen;
    keyrec*   newrec;
    int       nerror;

  // Ensure the map is allocated
    if ( pitems == nullptr && (nerror = Alloc()) != 0 )
      return nerror;

  // Allocate the item
    if ( (newrec = keyrec::Create( k, t, nindex, pitems[nindex] )) == nullptr )
      return ENOMEM;
    pitems[nindex] = newrec;
      ++ncount;

    return 0;
  }

  template <class Key, class Val>
  inline  const void* hashmap<Key, Val>::Enum( const void* pvn ) const
  {
    keyrec**  ppktop;
    keyrec**  ppkend;

  // Check pitems initialized
    if ( pitems == nullptr )
      return nullptr;

  // For the first call, make valid object pointer
    if ( pvn != nullptr )
    {
      keyrec*   lpnext;

      if ( (lpnext = ((keyrec*)pvn)->lpn) != nullptr )
        return lpnext;

      ppktop = pitems + ((keyrec*)pvn)->pos + 1;
    }
      else
    ppktop = pitems;

    for ( ppkend = pitems + maplen; ppktop < ppkend && *ppktop == nullptr; ++ppktop )
      (void)nullptr;

    return ppktop < ppkend ? *ppktop : nullptr;
  }

  template <class Key, class Val>
  inline  void*     hashmap<Key, Val>::Enum( void* pvn )
  {
    keyrec**  ppktop;
    keyrec**  ppkend;

  // Check pitems initialized
    if ( pitems == nullptr )
      return nullptr;

  // For the first call, make valid object pointer
    if ( pvn != nullptr )
    {
      keyrec*   lpnext;

      if ( (lpnext = ((keyrec*)pvn)->lpn) != nullptr )
        return lpnext;

      ppktop = pitems + ((keyrec*)pvn)->pos + 1;
    }
      else
    ppktop = pitems;

    for ( ppkend = pitems + maplen; ppktop < ppkend && *ppktop == nullptr; ++ppktop )
      (void)nullptr;

    return ppktop < ppkend ? *ppktop : nullptr;
  }

  template <class Key, class Val>
  inline  Key&      hashmap<Key, Val>::GetKey( const void*  pvn )
  {
    assert( pvn != NULL );

    return ((keyrec*)pvn)->key;
  }

  template <class Key, class Val>
  inline  Val&      hashmap<Key, Val>::GetVal( const void*  pvn )
  {
    assert( pvn != NULL );

    return ((keyrec*)pvn)->val;
  }

  template <class Key, class Val>
  template <class action>
  inline  int       hashmap<Key, Val>::for_each( action _do_ ) const
  {
    const void* p = nullptr;
    int         e = 0;

    while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
      (void)0;

    return e;
  }

  template <class Key, class Val>
  template <class action>
  inline  int       hashmap<Key, Val>::for_each( action _do_ )
  {
    const void* p = nullptr;
    int         e = 0;

    while ( (p = Enum( p )) != nullptr && (e = _do_( GetKey( p ), GetVal( p ) )) == 0 )
      (void)0;

    return e;
  }

  template <class Key, class Val>
  template <class ifcond>
  inline  void      hashmap<Key, Val>::DeleteIf( ifcond _if_ )
  {
    for ( auto p = Enum( nullptr ); p != nullptr; )
      if ( _if_( GetKey( p ), GetVal( p ) ) )
      {
        const Key&  delkey = GetKey( p );

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
