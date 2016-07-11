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
# if !defined( __prefixtree_h__ )
# define  __prefixtree_h__
# include "serialize.h"
# include "autoptr.h"
# include "array.h"

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 4514 4786 4710 )
# endif // _MSC_VER

namespace mtc
{
  template <class V, class counter, class M = def_alloc<>>
  class prefixtree: public array<prefixtree<V, counter, M>, M>
  {
    typedef array<prefixtree<V, counter, M>, M> nested;

    unsigned char       chnode;
    unsigned            length;
    bool                hasval;
    V                   avalue;

  protected:  
    static  unsigned    CounterGetBufLen()
      {
        return sizeof(counter);
      }
    template <class O>
    static  O*          CounterSerialize( O* o, int ncount, bool extended )
      {
        counter acount = ncount | (extended ? (1 << (sizeof(counter) * CHAR_BIT - 1)) : 0);
  
        return ::Serialize( o, &acount, sizeof(acount) );
      }
  
  public:     // construction
              prefixtree( unsigned char c = '\0' );
             ~prefixtree();

  public:     // expand
          V*        Insert( const char*, size_t, const V& e = 0 );
          V*        Search( const char*, size_t );
    const V*        Search( const char*, size_t ) const;
          void      DelAll();
  
  public:     // serialization
                        size_t  GetBufLen();
    template <class O>  O*      Serialize( O* ) const;
  
  public:     // enum
    template <class A>  int   Enumerate( const A& action ) const;
    template <class A>  int   Enumerate( A& action );
  
  public:     // real enumerator
    template <class A>  int   enumerate( A& action, array<char, M>& keybuf, int l = 0 );
  };
  
  // prefixtree inline implementation
  
  template <class V, class counter, class M>
  inline  prefixtree<V, counter, M>::prefixtree( unsigned char c ):
    array<prefixtree<V, counter, M>, M>( 0x2 ), chnode( c ), hasval( false )
  {
  }
  
  template <class V, class counter, class M>
  inline  prefixtree<V, counter, M>::~prefixtree()
  {
  }
  
  template <class V, class counter, class M>
  inline  V*  prefixtree<V, counter, M>::Insert( const char* pszstr, size_t cchstr, const V& object )
  {
    auto  expand = this;

    while ( cchstr > 0 )
    {
      auto  ptrtop = expand->begin();
      auto  ptrend = expand->end();
      auto  chfind = (unsigned char)*pszstr;
  
      if ( ptrtop < ptrend )
        while ( ptrtop->chnode > chfind && ++ptrtop < ptrend )
          (void)NULL;

      if ( ptrtop >= ptrend || ptrtop->chnode != chfind )
      {
        int   inspos;
  
        if ( ((nested*)expand)->Insert( inspos = (int)(ptrtop - expand->begin()), prefixtree( chfind ) ) == 0 )
          ptrtop = expand->begin() + inspos;  else return nullptr;
      }
      expand = ptrtop;
        ++pszstr;
        --cchstr;
    }

    if ( !expand->hasval )
      {  expand->avalue = object;  expand->hasval = true;  }
    return &expand->avalue;
  }
  
  template <class V, class counter, class M>
  inline  V*  prefixtree<V, counter, M>::Search( const char* pszstr, size_t cchstr )
  {
    return (V*)((const prefixtree<V, counter, M>*)this)->Search( pszstr, cchstr );
  }
  
  template <class V, class counter, class M>
  inline  const V*  prefixtree<V, counter, M>::Search( const char* pszstr, size_t cchstr ) const
  {
    auto  search = this;

    while ( cchstr > 0 )
    {
      auto  ptrtop = search->begin();
      auto  ptrend = search->end();
      auto  chfind = (unsigned char)*pszstr;
  
      if ( ptrtop < ptrend )
        while ( ptrtop->chnode > chfind && ++ptrtop < ptrend )
          (void)NULL;

      if ( ptrtop >= ptrend || ptrtop->chnode != chfind ) return nullptr;
        else search = ptrtop;

      ++pszstr;
      --cchstr;
    }
  
    return search->hasval ? &search->avalue : nullptr;
  }

  template <class V, class counter, class M>
  inline  void    prefixtree<V, counter, M>::DelAll()
  {
    elmptr = nullptr;
    SetLen( 0 );
  }
  
  template <class V, class counter, class M>
  inline  size_t  prefixtree<V, counter, M>::GetBufLen()
  {
    size_t    buflen = CounterGetBufLen();
    size_t    sublen;
  
    for ( auto p = begin(); p < end(); ++p )
      buflen += ::GetBufLen( sublen = p->GetBufLen() ) + sublen + 1;
  
    if ( hasval )
      buflen += ::GetBufLen( avalue );
  
    return (size_t)(length = (unsigned)buflen);
  }
  
  template <class V, class counter, class M>
  template <class O>
  inline  O*  prefixtree<V, counter, M>::Serialize( O* o ) const
  {
    if ( (o = CounterSerialize( o, (int)size(), elmptr != nullptr )) == nullptr )
      return nullptr;
  
    for ( auto p = begin(); p < end(); ++p )
      o = p->Serialize( ::Serialize( ::Serialize( o, p->chnode ), p->length ) );
  
    if ( hasval )
      o = ::Serialize( o, avalue );
  
    return o;
  }
  
  template <class V, class counter, class M>
  template <class A>
  inline  int       prefixtree<V, counter, M>::Enumerate( const A& action ) const
  {
    array<char, M>  keybuf( 0x40 );

    return ((prefixtree<V, counter, M>*)this)->enumerate( (A&)action, keybuf );
  }

  template <class V, class counter, class M>
  template <class A>
  inline  int       prefixtree<V, counter, M>::enumerate( A& action, array<char, M>& keybuf, int l )
  {
    int   nerror;
  
    for ( auto p = begin(); p < end(); ++p )
    {
      if ( keybuf.size() <= l && keybuf.SetLen( l + 0x10 ) != 0 )
        return ENOMEM;

      keybuf[l] = p->chnode;

      if ( (nerror = p->enumerate( action, keybuf, l + 1 )) != 0 )
        return nerror;
    }
  
    return hasval ? action( keybuf, l, avalue ) : 0;
  }

}  // mtc namespace

# endif  // __prefixtree_h__
