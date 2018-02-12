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
# if !defined( __mtc_patricia_h__ )
# define __mtc_patricia_h__
# include "serialize.h"
# include "platform.h"
# include "autoptr.h"

# if defined( TEST_PATRICIA )
#   include <stdio.h>
# endif  // TEST_PATRICIA

namespace mtc
{

  template <class V, class M = def_alloc>
  class patricia
  {
    class patnode;

  /*
    compare two keys on limited length and return both match length and compare result
  */
    static  size_t  getmatch( int& rescmp, const byte_t* p1, size_t l1, const byte_t* p2, size_t l2 )
      {
        size_t  lmatch;
        size_t  maxlen = min( l1, l2 );

        for ( lmatch = 0; lmatch < maxlen && (rescmp = p1[lmatch] - p2[lmatch]) == 0; ++lmatch )
          (void)NULL;
        if ( rescmp == 0 )
          rescmp = (l1 > l2) - (l1 < l2);
        return lmatch;
      }

    struct patarray
    {
      word16_t  uflags;

    public:     // construction
      patarray()
        {}

    public:     // array immitation
      int   size() const
        {
          return uflags & 0x1ff;
        }
      int   getlimit() const
        {
          return (uflags >> 7) & ~0x03;
        }
      void  setlimit( int n )
        {
          assert( (n & 0x03) == 0 );
          uflags = (uflags & 0x1ff) | (n << 7);
        }
      patnode*& operator [] ( int i )
        {
          assert( i >= 0 && i < size() );
          return begin()[i];
        }
      const patnode**  begin() const  {  return (const patnode**)(this + 1);  }
            patnode**  begin()        {  return (patnode**)(this + 1);  }
      const patnode**  end() const    {  return begin() + size();  }
            patnode**  end()          {  return begin() + size();  }
      bool  setlen( int n )
        {
          assert( n <= 0x100 );
          uflags = (uflags & ~0x1ff) + n;
          return true;
        }
      void  append( patnode* p )
        {
          int   curlen;

          assert( size() < getlimit() );
          setlen( 1 + (curlen = size()) );
            (*this)[curlen] = p;
        }
      bool  insert( int n, patnode* p )
        {
          int   thelen;

          if ( (thelen = size()) >= getlimit() )
            return false;
          memmove( begin() + n + 1, begin() + n, (thelen - n) * sizeof(patricia*) );
            begin()[n] = p;
          return setlen( thelen + 1 );
        }
      bool  move( patarray& r )
        {
          if ( getlimit() >= r.size() )  {  setlen( r.size() );  r.setlen( 0 );  }
            else return false;
          memmove( begin(), r.begin(), size() * sizeof(patricia*) );
            return true;
        }
      template <class action>
      void  for_each( action _ac_ )
        {
          for ( auto p = begin(); p < end(); ++p )
            _ac_( *p );
        }

    public:     // serialization
      size_t  GetBufLen() const
        {
          size_t  l = 0;

          for ( auto p = begin(); p < end(); ++p )
            l += (*p)->GetBufLen();
          return l;
        }
      template <class O> O* Serialize( O* o ) const
        {
          for ( auto p = begin(); p < end() && o != nullptr; ++p )
            o = (*p)->Serialize( o );
          return o;
        }

    private:
      patarray( const patarray& );
     ~patarray();
      patarray& operator = ( const patarray& );

    };

  /*
    patnode 'uflags':
      0x80000000 - the record has inline data value;
      0x40000000 - the record contains the serialized patricia tree for partial compress
      0x3fff0000 - the 4-aligned string length limit >> 2
  */
    class patnode
    {
      word32_t  uflags;   // the base class field
  /*  V         uvalue;
      char      string[];    of defined length
      word16_t  arrlen;      the patnode* tree array len
      patnode*  patarr[];    patnode* array itself */

    public:     // accessor functions - pseudo-fields

      word16_t        getStrLim() const   {  return (uflags & ~0xc000ffff) >> 14;  }
      word16_t        getStrLen() const   {  return (word16_t)uflags;  }
      const byte_t*   getString() const   {  return sizeof(V) + (const byte_t*)(this + 1);  }
      void            setStrLen( word16_t n )
        {
          assert( n <= (size_t)getStrLim() );
          uflags = (uflags & 0xffff0000) | (unsigned)n;
        }
      template <class chartype>
      void          setString( const chartype* k, size_t l )
        {
          if ( k != nullptr )
            memcpy( (void*)getString(), k, l );
          setStrLen( (word16_t)l );
        }
    
    public:     // accessor functions - array and value
      const patarray& getarray() const
        {  return *(const patarray*)( sizeof(V) + getStrLim() + (char*)(this + 1) );  }
      const V*  getvalue() const
        {  return (uflags & 0x80000000) != 0 ? (const V*)(this + 1) : nullptr;  }
      patarray& getarray()
        {  return *(patarray*)( sizeof(V) + getStrLim() + (char*)(this + 1) );  }
      V*    getvalue()
        {  return (uflags & 0x80000000) != 0 ? (V*)(this + 1) : nullptr;  }
      void  delvalue()
        {
          if ( (uflags & 0x80000000) != 0 )
            getvalue()->~V();
          uflags &= ~0x80000000;
        }
      V*    setvalue()
        {
          if ( (uflags & 0x80000000) != 0 )
            return getvalue();
          uflags |= 0x80000000;
            return new( getvalue() ) V();
        }
      V*    setvalue( const V& v )
        {
          if ( (uflags & 0x80000000) != 0 )
            getvalue()->~V();
          uflags |= 0x80000000;
            return new( getvalue() ) V( v );
        }
      void  movarray( patnode* p )
        {
          assert( getarray().size() == 0 );
          assert( getarray().getlimit() >= p->getarray().size() );

          getarray().setlen( p->getarray().size() );
            memmove( getarray().begin(), p->getarray().begin(), getarray().size() * sizeof(patnode*) );
          p->getarray().setlen( 0 );
        }
      void  movvalue( patnode* p )
        {
          assert( (uflags & 0x80000000) == 0 );   // check object has no value
          if ( (p->uflags & 0x80000000) != 0 )
          {
            uflags |= 0x80000000;
              memmove( getvalue(), p->getvalue(), sizeof(V) );
            p->uflags &= ~0x80000000;
          }
        }

    public:     // construction
      patnode( size_t strlimit ): uflags( (unsigned)(((strlimit + 3) & ~3) << 14) )
        {
        }
     ~patnode()
        {
          getarray().for_each( []( patnode* p )
            {  deallocate_with<M>( p );  } );
          delvalue();
        }

  # if defined( TEST_PATRICIA )
    public:
      void  DumpPatriciaTrie( FILE* output, int before = 0 )
        {
          patarray& aitems = getarray();
          int       maxlen = 0;
          int       nindex;

        // get maximal length
          aitems.for_each( [&]( patnode* p )
            {  maxlen = max( maxlen, p->getStrLen() );  } );

        // check if maxlen is too big
          maxlen = min( maxlen, 0x20 );

        // print the lines
          for ( nindex = 0; nindex < aitems.size(); ++nindex )
          {
            const byte_t* thekey = aitems[nindex]->getString();
            int           cchkey = aitems[nindex]->getStrLen();
            int           nwrite = min( cchkey, 0x20 - 3 );

            for ( auto i = 0; i < before; ++i )
              fputc( ' ', output );
            fwrite( thekey, 1, nwrite, output );
              fprintf( output, "%s\n", (cchkey > 0x20 - 3) ? "..." : "" );

            aitems[nindex]->DumpPatriciaTrie( output, before + 2 );
          }
        }
  # endif  // TEST_PATRICIA

    public:     // iterator
      template <class action> int for_each( action _do_ )
        {
          patarray& anodes = getarray();
          int       nerror;

          for ( auto p = anodes.begin(); p < anodes.end(); ++p )
            if ( (nerror = (*p)->for_each( _do_ )) != 0 )
              return nerror;

          return getvalue() != nullptr ? _do_( *getvalue() ) : 0;
        }

    public:     // search & insert
      patnode*  Search( const byte_t* k, size_t l ) const
        {
          const patnode*  search = this;

          for ( ; ; )
          {
            const patarray& anodes = search->getarray();
            const patnode** ptrtop = anodes.begin();
            const patnode** ptrend = anodes.end();
            byte_t          chfind = *(byte_t*)k;
            const byte_t*   sznext = nullptr;
            byte_t          chnext;
            unsigned        curlen;

          // найти вложенный элемент с совпадающим первым символом
            while ( ptrtop < ptrend && (chnext = *(sznext = (*ptrtop)->getString())) < chfind )
              ++ptrtop;
            if ( ptrtop >= ptrend || chnext != chfind )
              return nullptr;

          // check complete match for string
            if ( (curlen = (*ptrtop)->getStrLen()) > l )
              return nullptr;
            if ( curlen > 1 && memcmp( sznext, k, curlen ) != 0 )
              return nullptr;

          // check this or nested element
            if ( curlen == l )
              return (patnode*)*ptrtop;

            search = *ptrtop;
              k += curlen;
              l -= curlen;
          }
        }
      patnode*  Insert( patnode*&  placed, const byte_t* k, size_t l )
        {
          patarray&           aitems = getarray();
          patnode**           ptrtop = aitems.begin();
          patnode**           ptrend = aitems.end();
          byte_t              chbyte;
          _auto_<patnode, M>  palloc;
          word16_t            ccount;
          int                 rescmp;
          size_t              lmatch;

        // уже нашли
          if ( l == 0 )
            return this;

        // найти вложенный элемент с совпадающим первым символом
          while ( ptrtop < ptrend && (chbyte = *(*ptrtop)->getString()) < *k )
            ++ptrtop;

        // проверить, что элемент найден; если нет, создать и вставить новый элемент
          if ( ptrtop >= ptrend || chbyte != *k )
          {
            if ( (palloc = allocate( k, l )) == nullptr )
              return nullptr;
            if ( (placed = this->InsertAt( (int)(ptrtop - aitems.begin()), palloc )) == nullptr )
              return nullptr;
            return palloc.detach();
          }

        // элемент с таким начальным символом уже есть; проверить, что точно совпадает
          ccount = (*ptrtop)->getStrLen();

          if ( (lmatch = getmatch( rescmp, (*ptrtop)->getString(), ccount, (const byte_t*)k, l )) == ccount )
            return (*ptrtop)->Insert( *ptrtop, k + ccount, l - ccount );

        // иначе пилить элемент на два:
        //    - элемент с хвостом от себя, вместе с массивом вложений;
        //    - элемент с хвостом от добавляемого,
        // и укорачивать собственную строку
          if ( (palloc = allocate( (*ptrtop)->getString() + lmatch, ccount - lmatch, (*ptrtop)->getarray().getlimit() )) != nullptr )
          {
            _auto_<patnode, M>  aptail;

          // move elements from self array to created array
            palloc->movarray( *ptrtop );
            palloc->movvalue( *ptrtop );

            if ( l > lmatch )
            {
              if ( (aptail = allocate( k + lmatch, l - lmatch )) == nullptr )
                return nullptr;

            // сбросить собственный массив и заполнить двумя элементами
              (*ptrtop)->getarray().setlen( 2 );
                (*ptrtop)->getarray()[rescmp <= 0 ? 1 : 0] = aptail;
                (*ptrtop)->getarray()[rescmp <= 0 ? 0 : 1] = palloc.detach();

            }
              else
            {
              aptail = *ptrtop;
                (*ptrtop)->getarray().setlen( 1 );
                (*ptrtop)->getarray()[0] = palloc.detach();
            }

          // сбросить собственную строку
            (*ptrtop)->setStrLen( (word16_t)lmatch );
              return aptail.detach();
          }

          return nullptr;
        }

    public:     // serialization
      size_t  GetBufLen() const
        {
          const patarray& patarr = getarray();
          const V*        pvalue = getvalue();
          word16_t        arsize = patarr.size() << 1;
          size_t          ccharr = patarr.GetBufLen();

          if ( pvalue != nullptr )
            {  ccharr += ::GetBufLen( *pvalue );  arsize |= 1;  }

        // serialize byte count of nested branches
          return ::GetBufLen( getStrLen() ) + getStrLen() + ccharr + ::GetBufLen( ccharr ) + ::GetBufLen( arsize );
        }
      template <class O>  O*  Serialize( O* o ) const
        {
          const patarray& patarr = getarray();
          const V*        pvalue = getvalue();
          word16_t        arsize = (patarr.size() << 1) | (pvalue != nullptr ? 0x01 : 0x00);
          size_t          nodlen;

        // store key size, array size and key
          o = ::Serialize( ::Serialize( ::Serialize( o, getStrLen() ), arsize ),
            getString(), getStrLen() );

        // write the array and value size in bytes
          nodlen = patarr.GetBufLen() + (pvalue != nullptr ? ::GetBufLen( *pvalue ) : 0);

          if ( (o = patarr.Serialize( ::Serialize( o, nodlen ) )) == nullptr )
            return nullptr;

        // serialize value
          return o != nullptr && pvalue != nullptr ? ::Serialize( o, *pvalue ) : o;
        }
      template <class S>  S*  FetchFrom( S* s, int arsize, bool bvalue )
        {
          patarray& a = getarray();
          unsigned  l;

          assert( getStrLen() <= getStrLim() );
          assert( arsize <= a.limit() );

        // get string itself
          if ( (s = ::FetchFrom( s, (void*)getString(), getStrLen() )) == nullptr )
            return nullptr;

        // check for data
          if ( bvalue && (s = ::FetchFrom( s, *setvalue() )) == nullptr )
            return nullptr;

          if ( arsize > 0 && (s = ::FetchFrom( s, l )) == nullptr )
            return nullptr;

        // load the array
          while ( arsize-- > 0 && s != nullptr )
          {
            int       cchstr;
            word16_t  arsets;

            if ( (s = ::FetchFrom( ::FetchFrom( s, cchstr ), arsets )) != nullptr )
            {
              _auto_<patnode, M>  palloc;

              if ( (palloc = allocate( (char*)nullptr, cchstr, ((arsets >> 1) + 3) & ~(4 - 1) )) == nullptr ) return nullptr;
                else palloc->setStrLen( cchstr );

              if ( (s = palloc->FetchFrom( s, arsets >> 1, (arsets & 0x01) != 0 )) != nullptr )
                a.append( palloc.detach() );
            }
          }
          
          return s;
        }

    protected:  // allocs
      patnode*  InsertAt( int n, patnode* p )
        {
          patarray& rnodes = getarray();
          V*        pvalue;

          if ( !rnodes.insert( n, p ) )
          {
            _auto_<patnode, M>  palloc;

            if ( (palloc = allocate( (char*)getString(), getStrLen(), rnodes.getlimit() + 4 )) != nullptr )
            {
              if ( (pvalue = getvalue()) != nullptr )
                palloc->setvalue( *getvalue() );
              palloc->getarray().move( rnodes );
                palloc->getarray().insert( n, p );
            }
            deallocate_with<M>( this );

            return palloc.detach();
          }
          return this;
        }

    public:
      template <class chartype=char> static
      patnode*   allocate( const chartype* k = nullptr, size_t l = 0, unsigned a = 0x04 /* array size */ )
        {
          size_t    lalign = (l + 3) & ~3;          // align length to 4 bytes
          size_t    nalloc = sizeof(patnode)        // for runtime usage
                           + sizeof(V)              // value place
                           + lalign                 // key value
                           + sizeof(word16_t)       // array size and value flag
                           + a * sizeof(patricia*); // array
          patnode*  palloc;

        // allocate the tree
          if ( (palloc = (patnode*)M().alloc( nalloc )) == nullptr ) return nullptr;
            else new( (patnode*)memset( palloc, 0, nalloc ) ) patnode( lalign );

        // set the flags
          palloc->setString( k, l );
            assert( palloc->getStrLim() == lalign );
            assert( palloc->getStrLen() == l );

        // set array length
          palloc->getarray().setlimit( a );

          return palloc;
        }

    };

  public:     // construction
    patricia()  {}
    patricia( patricia&& p )
      {
        inplace_swap( patree, p.patree );
      }
    patricia& operator = ( patricia&& p )
      {
        patree = nullptr;
        inplace_swap( patree, p.patree );
        return *this;
      }
    patricia( const patricia& ) = delete;
    patricia& operator = ( const patricia& ) = delete;

  public:     // helpers
    void    DelAll()
      {
        patree = nullptr;
      }
  public:     // API
    template <class chartype = char> const V*  Search( const chartype* k, size_t l ) const;
    template <class chartype = char>       V*  Search( const chartype* k, size_t l );
    template <class chartype = char>       V*  Insert( const chartype* k, size_t l, const V& v = V() );

  public:     // iterator
    template <class action> int for_each( action _do_ )
      {
        return patree != nullptr ? patree->for_each( _do_ ) : 0;
      }

  public:     // serialization
    size_t  GetBufLen() const
      {
        return patree != nullptr ? patree->GetBufLen() : 1;
      }
    template <class O>  O*  Serialize( O* o ) const
      {
        return patree != nullptr ? patree->Serialize( o ) : ::Serialize( ::Serialize( o, 0 ), 0 );
      }
    template <class S>  S*  FetchFrom( S* s )
      {
        int       cchstr;
        word16_t  arsets;

        if ( patree != nullptr )
          patree = nullptr;

        if ( (s = ::FetchFrom( ::FetchFrom( s, cchstr ), arsets )) == nullptr )
          return nullptr;

        if ( cchstr == 0 && arsets == 0 )
          return s;

        if ( (patree = patnode::allocate( (char*)nullptr, cchstr, ((arsets >> 1) + 3) & ~(4 - 1) )) == nullptr )
          return nullptr;  else patree->setStrLen( cchstr );

        if ( (s = patree->FetchFrom( s, arsets >> 1, (arsets & 0x01) != 0 )) == nullptr )
          patree = nullptr;

        return s;
      }

  # if defined( TEST_PATRICIA )
  public:
    void  DumpPatriciaTrie( FILE* output )
      {
        if ( patree != nullptr )
          patree->DumpPatriciaTrie( output );
      }
  # endif  // TEST_PATRICIA

  protected:  // variables
    _auto_<patnode, M>  patree;

  };

  class patriciaSearch
  {
    enum
    {
      asterisk = 0x0100 + '*',
      question = 0x0100 + '?'
    };

  public:     // iterator
    class iterator;

    class plainkey
    {
      friend class iterator;

    public:
      plainkey( const char* ke = nullptr, size_t le = 0 ): key( ke ), len( le ) {}
      plainkey( const plainkey& k ): key( k.key ), len( k.len ) {}
      plainkey& operator = ( const plainkey& k )
        {  key = k.key;  len = k.len;  return *this;  }

    public:
      const char* Key() const {  return key;  }
      size_t      Len() const {  return len;  }

    protected:
      const char* key;
      size_t      len;

    };

    class iterator
    {
      friend class patriciaSearch;

      struct  patpos
      {
        const byte_t* dicptr;
        int           keylen;
        int           nnodes;
        bool          bvalue;
        const byte_t* endptr;

      public:     // init
        patpos( const byte_t* serial, array<char>& keybuf, int curlen )
          {
            unsigned  sublen;

            assert( serial != nullptr );

            dicptr = ::FetchFrom( ::FetchFrom( serial, keylen ), nnodes );

            if ( keybuf.size() < curlen + keylen )
              keybuf.SetLen( curlen + keylen );
            dicptr = ::FetchFrom( dicptr, keybuf.begin() + curlen, keylen );
              keylen += curlen;
            bvalue = (nnodes & 0x01) != 0;
              nnodes >>= 1;
            endptr = (dicptr = ::FetchFrom( dicptr, sublen )) + sublen;
          }
      };

      array<char>   keybuf;
      array<patpos> patbuf;
      int           patlen;

    protected:  // first initialization constructor
      iterator( const byte_t* serial )
        {
          if ( serial != nullptr )
            patbuf.Append( patpos( serial, keybuf, 0 ) );
          patlen = patbuf.size();
        }

    public:     // construction
      iterator()  {}
      iterator( iterator&& it ):
          keybuf( static_cast<decltype(keybuf)&&>( it.keybuf ) ),
          patbuf( static_cast<decltype(patbuf)&&>( it.patbuf ) ), patlen( it.patlen )  {  it.patlen = 0;  }
      iterator& operator = ( iterator&& it )
        {
          keybuf.operator = ( static_cast<decltype(keybuf)&&>( it.keybuf ) );
          patbuf.operator = ( static_cast<decltype(patbuf)&&>( it.patbuf ) );  patlen = it.patlen;  it.patlen = 0;
          return *this;
        }
      iterator( const iterator& ) = delete;
      iterator& operator = ( const iterator& ) = delete;

    public:     // API
      const plainkey& Curr()
        {
          return patlen == 0 || patbuf[patlen - 1].bvalue ? thekey : Next();
        }
      const plainkey& Move( const char* key, size_t len );
      const plainkey& Next()
        {
          while ( patlen > 0 )
          {
            patpos& thepos = patbuf[patlen - 1];

            if ( thepos.nnodes != 0 )
            {
              --thepos.nnodes;

              PatAdd( patpos( thepos.dicptr, keybuf, thepos.keylen ) );
                thepos.dicptr = patbuf[patlen - 1].endptr;

              if ( patbuf[patlen - 1].bvalue )
                return thekey = plainkey( keybuf, patbuf[patlen - 1].keylen );
            }
              else
            --patlen;
          }
          return thekey = plainkey();
        }
      const plainkey& Prev();

    protected:  // helpers
      void    PatAdd( const patpos& p )
        {
          if ( patlen == patbuf.size() )  {  patbuf.Append( p );  ++patlen;  }
            else patbuf[patlen++] = p;
        }
    protected:  // key
      plainkey    thekey;

    };

  public:     // construction
    patriciaSearch( const void* p ): serial( (const byte_t*)p ) {}
      
  public:     // search
    const byte_t* Search( const void*, size_t ) const;
    template <class _func_>
    int           Select( const void*, size_t, _func_ ) const;

  public:     // iterator
    iterator  NewIterator() const {  return iterator( serial );  }

  protected:  // helpers
    const byte_t* Search( const byte_t* keystr,
                          size_t        keylen,
                          const byte_t* thedic,
                          size_t        nnodes ) const;
    template <class _func_>
    int           Select( const byte_t* dicstr, int           diclen,
                          const byte_t* keystr, const byte_t* keyend,
                          const byte_t* thedic, int           nnodes,
                          _func_        insert,
                          byte_t*       buftop,
                          byte_t*       bufend,
                          byte_t*       bufptr ) const;

  protected:  // internals
    const byte_t* JumpOver( int nnodes, const byte_t* dicptr ) const;

  protected:  // variables
    const byte_t* serial;

  };

  // patricia implementation

  template <class V, class M> template <class chartype>
  V*  patricia<V, M>::Search( const chartype* k, size_t l )
  {
    const patnode*  pfound;

    return patree != nullptr && (pfound = patree->Search( (const byte_t*)k, l )) != nullptr ?
      (V*)pfound->getvalue() : nullptr;
  }

  template <class V, class M> template <class chartype>
  const V*  patricia<V, M>::Search( const chartype* k, size_t l ) const
  {
    return ((patricia<V, M>*)this)->Search( k, l );
  }

  template <class V, class M> template <class chartype>
  V*  patricia<V, M>::Insert( const chartype* k, size_t l, const V& v )
  {
    patnode*  pfound;

    if ( patree == nullptr && (patree = patnode::allocate( (char*)nullptr, 0, 0x40 )) == nullptr )
      return nullptr;

    if ( k == nullptr || l == 0 || (pfound = patree->Insert( *patree.pptr(), (const byte_t*)k, l )) == nullptr )
      return nullptr;

    return pfound->getvalue() != nullptr ? pfound->getvalue() : pfound->setvalue( v );
  }

  // patriciaSearch implementation

  template <class _func_>
  int           patriciaSearch::Select( const void* k, size_t l, _func_ f ) const
  {
    byte_t        thekey[0x100];
    const byte_t* thedic;
    int           nchars;
    int           nnodes;
    int           sublen;

    if ( (thedic = ::FetchFrom( ::FetchFrom( ::FetchFrom( serial, nchars ), nnodes ), sublen )) == nullptr )
      return 0;

    assert( nnodes <= 513 );
    assert( nchars <= 256 * 4 );

    return Select( thedic, nchars, (const byte_t*)k, l + (const byte_t*)k, thedic + nchars, nnodes, f, thekey, array_end( thekey ), thekey );
  }

  inline
  const byte_t* patriciaSearch::Search( const void* k, size_t cchkey ) const
  {
    const byte_t* thedic;
    size_t        nchars;
    size_t        nnodes;
    size_t        sublen;
    const byte_t* thekey = (const byte_t*)k;

    if ( (thedic = ::FetchFrom( ::FetchFrom( serial, nchars ), nnodes )) == nullptr )
      return nullptr;

    assert( nnodes <= 513 );
    assert( nchars <= 256 * 4 );

    if ( cchkey < nchars )
      return nullptr;

    while ( nchars-- > 0 )
      if ( *thekey++ == *thedic++ ) --cchkey;
        else return nullptr;

    if ( (thedic = ::FetchFrom( thedic, sublen )) == nullptr )
      return nullptr;

    return Search( thekey, (int)cchkey, thedic, nnodes );
  }

  template <class _func_>
  int           patriciaSearch::Select( const byte_t* dicstr, int           diclen,
                                        const byte_t* keystr, const byte_t* keyend,
                                        const byte_t* thedic, int           nnodes,
                                        _func_        addptr,
                                        byte_t*       buftop,
                                        byte_t*       bufend,
                                        byte_t*       bufptr ) const
  {
    bool    bvalue = (nnodes & 1) != 0;
    int     nerror;

    assert( diclen >= 0 );
    assert( keyend >= keystr );

  // сравнить соответствующие друг другу фрагменты строки и шаблона
    while ( diclen > 0 && keystr < keyend && (*keystr == *dicstr || *keystr == '?') )
    {
      if ( buftop != nullptr )
      {
        if ( bufptr < bufend )  *bufptr++ = *dicstr;
          else return E2BIG;
      }
      ++dicstr;
      ++keystr;
      --diclen;
    }

  // если строка запроса отсканирована полностью:
  //  - если строка узла словаря исчерпана и есть данные, зарегистрировать их;
  //  - иначе оборвать сканирование
    if ( keystr == keyend )
      return diclen == 0 && bvalue ? addptr( buftop, bufptr - buftop, JumpOver( nnodes >> 1, thedic ) ) : 0;

    assert( keystr < keyend );

  // если в строке словаря остались символы, а шаблон - не звёздочка, оборвать сканирование
    if ( diclen > 0 )
    {
      if ( *keystr != '*' )
        return 0;

    // если символ шаблона '*', зайти рекурсивно, предположив соответствие 0..N символов фрагмента
      for ( auto nmatch = 0; nmatch <= diclen; ++nmatch )
      {
        if ( buftop != nullptr && nmatch > 0 )
        {
          if ( bufend > bufptr )  *bufptr++ = dicstr[nmatch - 1];
            else return E2BIG;
        }
        if ( (nerror = Select( dicstr + nmatch, diclen - nmatch, keystr + 1, keyend, thedic, nnodes, addptr, buftop, bufend, bufptr )) != 0 )
          return nerror;
      }
      return nnodes > 1 ? Select( dicstr + diclen, 0, keystr, keyend, thedic, nnodes, addptr, buftop, bufend, bufptr ) : 0;
    }
      else
  // если строка в узле словаря исчерпана, а у узла нету вложенных узлов, то соответствие
  // может быть только в случае '*', во всех остальных - обрыв сканирования
    {
      if ( nnodes <= 1 )
        return keyend - keystr == 1 && *keystr == '*' ? addptr( buftop, bufptr - buftop, thedic ) : 0;

      for ( nnodes >>= 1; nnodes-- > 0; )
      {
        const byte_t* subdic;
        int           cchars;
        int           cnodes;
        int           curlen;

      // извлечь характеристики очередного узла
        thedic = ::FetchFrom( ::FetchFrom( thedic, cchars ), cnodes );
        subdic = ::FetchFrom( thedic + cchars, curlen );

        if ( (nerror = Select( thedic, cchars, keystr, keyend, subdic, cnodes, addptr, buftop, bufend, bufptr )) != 0 )
          return nerror;

        thedic = subdic + curlen;
      }
    }

    return 0;
  }

  inline
  const byte_t* patriciaSearch::Search( const byte_t* thekey,
                                        size_t        cchkey,
                                        const byte_t* thedic,
                                        size_t        nnodes ) const
  {
    bool    bvalue = (nnodes & 1) != 0;
    byte_t  chfind;

  // если строка кончилась, то узел должен иметь значение
    if ( cchkey == 0 )
      return bvalue ? JumpOver( nnodes >> 1, thedic ) : nullptr;

    assert( cchkey > (size_t)0 );

    for ( chfind = *thekey, nnodes >>= 1; nnodes-- > 0; )
    {
      size_t  cchars;
      size_t  cnodes;
      size_t  curlen;
      int     rescmp;

    // извлечь характеристики очередного узла
      thedic = ::FetchFrom( ::FetchFrom( thedic, cchars ), cnodes );

    // проверить на совпадение
      if ( (rescmp = chfind - *thedic) < 0 )
        return nullptr;

    // сравнить строку с текущим элементом; если не совпадает, завершить поиск
      if ( rescmp == 0 )
      {
        if ( cchkey < cchars )
          return nullptr;
        while ( cchars > 0 && *thedic++ == *thekey++ )
          {  --cchars;  --cchkey;  }

      // есть совпадение, извлечь размер вложенного массива и значения
        return cchars == 0 ? Search( thekey, cchkey, ::FetchFrom( thedic, curlen ), cnodes ) : nullptr;
      }

    // извлечь вложенный размер
      thedic = ::FetchFrom( thedic + cchars, curlen ) + curlen;
    }

    return nullptr;
  }

  inline
  const byte_t* patriciaSearch::JumpOver( int nnodes, const byte_t* thedic ) const
  {
    while ( nnodes-- > 0 )
    {
      int   cchars;
      int   cnodes;
      int   curlen;

      thedic = ::FetchFrom( ::FetchFrom( thedic, cchars ), cnodes );
      thedic = ::FetchFrom( thedic + cchars, curlen );
      thedic = thedic + curlen;
    }
    return thedic;
  }

}

# endif  // __mtc_patricia_h__
