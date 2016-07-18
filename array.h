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
# if !defined( __mtc_array_h__ )
# define  __mtc_array_h__
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <cassert>
# include "platform.h"
# include <new>

# if defined( _MSC_VER )
#   pragma warning( push )
#   pragma warning( disable: 4291 4514 4786 4710 )
# endif // _MSC_VER

namespace mtc
{

  // Common destruction methods
  template <class A>
  inline  void  __safe_array_destruct( A* lplist, int  lcount )
    {  for ( ; lcount-- > 0; lplist++ ) lplist->~A(); }
  template <>
  inline  void  __safe_array_destruct( char*, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned char*, int ) {}
  template <>
  inline  void  __safe_array_destruct( short*, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned short*, int ) {}
  template <>
  inline  void  __safe_array_destruct( int*, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned int*, int ) {}
  template <>
  inline  void  __safe_array_destruct( long*, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned long*, int ) {}
  template <>
  inline  void  __safe_array_destruct( float*, int ) {}
  template <>
  inline  void  __safe_array_destruct( double*, int ) {}
  template <>
  inline  void  __safe_array_destruct( char**, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned char**, int ) {}
  template <>
  inline  void  __safe_array_destruct( short**, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned short**, int ) {}
  template <>
  inline  void  __safe_array_destruct( int**, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned int**, int ) {}
  template <>
  inline  void  __safe_array_destruct( long**, int ) {}
  template <>
  inline  void  __safe_array_destruct( unsigned long**, int ) {}
  template <>
  inline  void  __safe_array_destruct( float**, int ) {}
  template <>
  inline  void  __safe_array_destruct( double**, int ) {}

  // Common construction methods
  template <class A>
  inline  void  __safe_array_construct_def( A* p, int c )
    {
      memset( p, 0, c * sizeof(A) );
      while ( c-- > 0 )
        new( p++ ) A;
    }
  template <>
  inline  void  __safe_array_construct_def( unsigned long* p, int c )
    {  memset( p, 0, c * sizeof(unsigned long) );  }
  template <>
  inline  void  __safe_array_construct_def( unsigned int* p, int c )
    {  memset( p, 0, c * sizeof(unsigned int) );  }
  template <>
  inline  void  __safe_array_construct_def( unsigned short* p, int c )
    {  memset( p, 0, c * sizeof(unsigned short) );  }
  template <>
  inline  void  __safe_array_construct_def( unsigned char* p, int c )
    {  memset( p, 0, c * sizeof(unsigned char) );  }
  template <>
  inline  void  __safe_array_construct_def( long* p, int c )
    {  memset( p, 0, c * sizeof(long) );  }
  template <>
  inline  void  __safe_array_construct_def( int* p, int c )
    {  memset( p, 0, c * sizeof(int) );  }
  template <>
  inline  void  __safe_array_construct_def( short* p, int c )
    {  memset( p, 0, c * sizeof(short) );  }
  template <>
  inline  void  __safe_array_construct_def( char* p, int c )
    {  memset( p, 0, c * sizeof(char) );  }
  template <>
  inline  void  __safe_array_construct_def( double* p, int c )
    {  memset( p, 0, c * sizeof(double) );  }
  template <>
  inline  void  __safe_array_construct_def( float* p, int c )
    {  memset( p, 0, c * sizeof(float) );  }

  // Copy construction methods
  template <class A>
  inline  A*              __safe_array_construct_cpy( A* p, const A& r )
    {  return new( p )A( r );  }
  inline  unsigned long*  __safe_array_construct_cpy( unsigned long* p, unsigned long r )
    {  *p = r;  return p;  }
  inline  unsigned int*   __safe_array_construct_cpy( unsigned int* p, unsigned int r )
    {  *p = r;  return p;  }
  inline  unsigned short* __safe_array_construct_cpy( unsigned short* p, unsigned short r )
    {  *p = r;  return p;  }
  inline  unsigned char*  __safe_array_construct_cpy( unsigned char* p, unsigned char r )
    {  *p = r;  return p;  }
  inline  long*           __safe_array_construct_cpy( long* p, long r )
    {  *p = r;  return p;  }
  inline  int*            __safe_array_construct_cpy( int* p, int r )
    {  *p = r;  return p;  }
  inline  short*          __safe_array_construct_cpy( short* p, short r )
    {  *p = r;  return p;  }
  inline  char*           __safe_array_construct_cpy( char* p, char r )
    {  *p = r;  return p;  }
  inline  double*         __safe_array_construct_cpy( double* p, double r )
    {  *p = r;  return p;  }
  inline  float*          __safe_array_construct_cpy( float* p, float r )
    {  *p = r;  return p;  }

  template <class T, class M = def_alloc<>>
  class array
  {
    M     malloc;

  public:     // construction
          array( int adelta = 0x10 );
          array( const array<T, M>& );
         ~array();
    array<T, M>& operator =( const array<T, M>& );

  public:     // set allocator
    M&    GetAllocator()              {  return malloc;     }
    M&    SetAllocator( const M& m )  {  return malloc = m; }

  public:     // members
    int   Append( const T& t )                    {  return Insert( size(), t );      }
    int   Append( T& t )                          {  return Insert( size(), t );      }
    int   Append( int c, const T* p )             {  return Insert( size(), c, p );   }
    int   Append( const array<T, M>& r )          {  return Insert( size(), r );      }
    int   Append( array<T, M>& r )                {  return Insert( size(), r );      }
    int   Insert( int p, const T& t )             {  return insert( p, t );           }
    int   Insert( int p, T& t )                   {  return insert( p, t );           }
    int   Insert( int p, int c, const T* t )      {  return insert( p, c, t );        }
    int   Insert( int p, int c, T* t )            {  return insert( p, c, t );        }
    int   Insert( int p, const array<T, M>& t )   {  return insert( p, t.size(), (const T*)t ); }
    int   Insert( int p, array<T, M>& t )         {  return insert( p, t.size(), (T*)t );       }
    int   Delete( int );
    int   GetLen() const;
    int   SetLen( int );
    void  DelAll();

  public:     // searchers
    int   Lookup( const T& ) const;
    template <class _pred_>
    int   Lookup( _pred_ ) const;
    bool  Search( const T&, int& ) const;
    template <class _key_, class _cmp_>
    bool  Search( const _key_&, int&, _cmp_ ) const;

  public:     // operators
    operator        T* ()         {  return pitems;   }
    operator const  T* () const   {  return pitems;   }
    T&    operator [] ( int );
    const T&  operator [] ( int ) const;

  public:     // customizing
    int   GetLimit() const  {  return nlimit;   }
    int   GetDelta() const  {  return ndelta;   }
    int   SetLimit( int );
    void  SetDelta( int n ) {  ndelta = n;      }

  public:     // iterators
    template <class _func_>
    int       for_each( _func_ ) const;
    template <class _func_>
    int       for_each( _func_ );
    template <class _func_>
    int       DeleteIf( _func_ );
    int       DeleteIf( const T& );

  public:     // stl compatibility
    T*        begin()       {  return pitems;  }
    T*        end()         {  return pitems + ncount;  }
    const T*  begin() const {  return pitems;  }
    const T*  end() const   {  return pitems + ncount;  }
    int       size() const  {  return ncount;  }

  private:
    template <class _type_>
    int  insert( int, _type_ );
    template <class _type_>
    int  insert( int, int, _type_* );

  protected:
    T*    pitems;
    int   ncount;
    int   nlimit;
    int   ndelta;

  };

  template <class T, class M = def_alloc<>>
  class shared_array
  {
    struct array_data: public array<T, M>
    {
      long  refcount;

    public:
    
      array_data( int d ): array<T, M>( d ) {  refcount = 0;  }
    };

    M           malloc;
    array_data* parray;
    int         ndelta;

  protected:  // helper
    bool  Ensure()
      {
        if ( parray == nullptr && (parray = malloc.template allocate<array_data>( ndelta )) != nullptr )
          {  ++parray->refcount;  parray->SetAllocator( malloc );  }
        return parray != nullptr;
      }
  public:     // construction
    shared_array( int adelta = 0x10 ): parray( nullptr ), ndelta( adelta )
      {
      }
    shared_array( const shared_array& a ):
                          malloc( a.malloc ),
                          ndelta( a.ndelta )
      {
        if ( (parray = a.parray) != nullptr )
          ++parray->refcount;
      }
   ~shared_array()
      {
        if ( parray != nullptr && --parray->refcount == 0 )
          malloc.deallocate( parray );
      }
    shared_array& operator = ( const shared_array& a )
      {
        if ( parray != nullptr && --parray->refcount == 0 )
          malloc.deallocate( parray );
        malloc = a.malloc;
        if ( (parray = a.parray) != nullptr )
          ++parray->refcount;
        return *this;
      }

  public:     // set allocator
    M&    GetAllocator()              {  return malloc;     }
    M&    SetAllocator( const M& m )  {  return malloc = m; }

  public:     // API
    int   Append( const T& t )
      {  return Ensure() ? parray->Insert( size(), t ) : ENOMEM;  }
    int   Append( T& t )
      {  return Ensure() ? parray->Insert( size(), t ) : ENOMEM;  }
    int   Append( int c, const T* p )
      {  return Ensure() ? parray->Insert( size(), c, p ) : ENOMEM;   }
    int   Append( const array<T, M>& r )
      {  return Ensure() ? parray->Insert( size(), r ) : ENOMEM;  }
    int   Append( array<T, M>& r )
      {  return Ensure() ? parray->Insert( size(), r ) : ENOMEM;  }
    int   Insert( int i, const T& t )
      {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Insert( int i, T& t )
      {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Insert( int i, int c, const T* t )
      {  return Ensure() ? parray->Insert( i, c, t ) : ENOMEM;  }
    int   Insert( int i, int c, T* t )
      {  return Ensure() ? parray->Insert( i, c, t ) : ENOMEM;  }
    int   Insert( int i, const array<T, M>& t )
       {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Insert( int i, array<T, M>& t )
       {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Delete( int n )
      {  return parray == nullptr ? 0 : parray->Delete( n );  }
    int   GetLen() const
      {  return parray != nullptr ? parray->GetLen() : 0;  }
    int   SetLen( int l )
      {
        if ( l == 0 )
        {
          if ( parray != nullptr && --parray->refcount == 0 )
            malloc.deallocate( parray );
          parray = nullptr;
            return 0;
        }
          else
        return Ensure() ? parray->SetLen( l ) : ENOMEM;
      }
    void  DelAll()
      {
        if ( parray != nullptr && --parray->refcount == 0 )
          malloc.deallocate( parray );
        parray = nullptr;
      }
    int   Privatize()
      {
        if ( parray != nullptr && parray->refcount != 1 )
        {
          array_data* palloc;

          if ( (palloc = malloc.template allocate<array_data>( parray->GetDelta() )) == nullptr )
            return ENOMEM;  else parray->SetAllocator( malloc );

          if ( palloc->Append( *parray ) != 0 )
          {
            malloc.deallocate( palloc );
            return ENOMEM;
          }
          if ( --parray->refcount == 0 )
            parray->GetAllocator().deallocate( p );

          ++(parray = palloc)->refcount;
        }
        return 0;
      }

  public:     // searchers
    int   Lookup( const T& t ) const
      {  return parray != nullptr ? parray->Lookup( t ) : -1;  }
    template <class _pred_>
    int   Lookup( _pred_ test ) const
      {  return parray != nullptr ? parray->Lookup( test ) : -1;  }
    bool  Search( const T& t, int& s ) const
      {  return parray != nullptr ? parray->Search( t, s ) : (s = 0) != 0;  }
    template <class _key_, class _cmp_>
    bool  Search( const _key_& k, int& s, _cmp_ cmp ) const
      {  return parray != nullptr ? parray->Search( k, s, cmp ) : (s = 0) != 0;  }

  public:     // operators
    operator        T* ()
      {  return parray != nullptr ? (T*)*parray : (T*)nullptr;  }
    operator const  T* () const
      {  return parray != nullptr ? (const T*)*parray : (T*)nullptr;  }
    T&    operator [] ( int n )
      {  assert( parray != nullptr );  return (*parray)[n];  }
    const T&  operator [] ( int n ) const
      {  assert( parray != nullptr );  return (*parray)[n];  }

  public:     // customizing
    int   GetLimit() const
      {  return parray != nullptr ? parray->GetLimit() : 0;  }
    int   GetDelta() const
      {  return parray != nullptr ? parray->GetDelta() : ndelta;  }
    int   SetLimit( int l )
      {  return Ensure() ? parray->SetLimit( l ) : ENOMEM;  }
    void  SetDelta( int n )
      {  ndelta = n;  if ( parray != nullptr )  parray->SetDelta( ndelta );  }

  public:     // iterators
    template <class _func_>
    int       for_each( _func_ func ) const
      {  return parray != nullptr ? parray->for_each( func ) : 0;  }
    template <class _func_>
    int       for_each( _func_ func )
      {  return parray != nullptr ? parray->for_each( func ) : 0;  }
    template <class _func_>
    int       DeleteIf( _func_ func )
      {  return parray != nullptr ? parray->DeleteIf( func ) : 0;  }
    int       DeleteIf( const T& t )
      {  return parray != nullptr ? parray->DeleteIf( t ) : 0;  }

  public:     // stl compatibility
    T*        begin()       {  return *this;  }
    T*        end()         {  return size() + *this;  }
    const T*  begin() const {  return *this;  }
    const T*  end() const   {  return size() + *this;  }
    int       size() const  {  return parray != nullptr ? parray->size() : 0;  }
  };

// array inline implementation

  template <class T, class M>
  inline  array<T, M>::array( int adelta ):
                      pitems( 0 ),
                      ncount( 0 ),
                      nlimit( 0 ),
                      ndelta( adelta <= 0 ? 0x10 : adelta )
  {
  }

  template <class T, class M>
  inline array<T, M>::array( const array<T, M>& r ):
                      malloc( r.malloc ),
                      pitems( r.pitems ),
                      ncount( r.ncount ), 
                      nlimit( r.nlimit ),
                      ndelta( r.ndelta )
  {
    ((array<T, M>&)r).pitems = nullptr;
    ((array<T, M>&)r).ncount = 0;
  }

  template <class T, class M>
  inline array<T, M>& array<T, M>::operator = ( const array<T, M>& r )
  {
    this->~array<T, M>();
      malloc = r.malloc;
      pitems = r.pitems;
      ncount = r.ncount;
      nlimit = r.nlimit;
      ndelta = r.ndelta;
    ((array<T, M>&)r).pitems = nullptr;
    ((array<T, M>&)r).ncount = 0;
      return *this;
  }


  template <class T, class M>
  inline  array<T, M>::~array()
  {
    if ( pitems )
    {
      if ( ncount )
        __safe_array_destruct( pitems, ncount );
      malloc.free( pitems );
    }
  }

  template <class T, class M>
  inline  int   array<T, M>::Delete( int nindex )
  {
    if ( nindex < 0 || nindex >= ncount )
      return EINVAL;
    else __safe_array_destruct( pitems + nindex, 1 );
    if ( nindex < --ncount )
      memmove( pitems + nindex, pitems + nindex + 1, (ncount - nindex)
        * sizeof(T) );
    return 0;
  }

  template <class T, class M>
  inline  int   array<T, M>::GetLen() const
  {
    return ncount;
  }

  template <class T, class M>
  inline  int   array<T, M>::SetLen( int length )
  {
    if ( length < 0 )
      return EINVAL;
    if ( length < ncount )
      __safe_array_destruct( pitems + length, ncount - length );
    if ( length > ncount )
    {
      if ( length > nlimit )
      {
        int   newlimit = nlimit + ndelta;
        T*    newitems;

        if ( newlimit < length )
          newlimit = length;

      // Allocate new space
        if ( (newitems = (T*)malloc.alloc( newlimit * sizeof(T) )) == NULL )
          return ENOMEM;

      // Copy the data
        if ( ncount > 0 )
          memcpy( newitems, pitems, ncount * sizeof(T) );

      // Set new buffer
        if ( pitems != NULL )
          malloc.free( pitems );
        pitems = newitems;
        nlimit = newlimit;
      }
      __safe_array_construct_def( pitems + ncount, length - ncount );
    }
      else
    if ( length == 0 )
    {
      malloc.free( pitems );
      pitems = 0;
      nlimit = 0;
    }
    ncount = length;
    return 0;
  }

  template <class T, class M>
  inline  void   array<T, M>::DelAll()
  {
    __safe_array_destruct( pitems, ncount );
    malloc.free( pitems );
    pitems = 0;
    nlimit = 0;
    ncount = 0;
  }

  template <class T, class M>
  inline  int   array<T, M>::Lookup( const T& t ) const
  {
    for ( auto p = begin(); p < end(); ++p )
      if ( *p == t )  return p - begin();
    return -1;
  }

  template <class T, class M>
  template <class _pred_>
  inline  int   array<T, M>::Lookup( _pred_ pred ) const
  {
    for ( auto p = begin(); p < end(); ++p )
      if ( pred( *p ) ) return p - begin();
    return -1;
  }

  template <class T, class M>
  inline  bool  array<T, M>::Search( const T& t, int& p ) const
  {
    int   l = 0;
    int   h = ncount - 1;
    int   m;
    bool  s = false;

    while ( l <= h )
    {
      m = ( l + h ) >> 1;
      if ( pitems[m] < t ) l = m + 1;
        else
      {
        h = m - 1;
        s |= (pitems[m] == t);
      }
    }
    p = (int)l;
    return s;
  }

  template <class T, class M>
  template <class _key_, class _cmp_>
  inline  bool  array<T, M>::Search( const _key_& k, int& p, _cmp_ comp ) const
  {
    int   l = 0;
    int   h = ncount - 1;
    int   m;
    bool  s = false;

    while ( l <= h )
    {
      int   r;

      m = ( l + h ) >> 1;
      r = comp( k, pitems[m] );

      if ( r > 0 ) l = m + 1;
        else
      {
        h = m - 1;
        s |= (r == 0);
      }
    }
    p = (int)l;
    return s;
  }

  template <class T, class M>
  inline  T&    array<T, M>::operator [] ( int nindex )
  {
    assert( nindex < ncount && nindex >= 0 );
    return pitems[nindex];
  }

  template <class T, class M>
  inline  const T&  array<T, M>::operator [] ( int nindex ) const
  {
    assert( nindex < ncount && nindex >= 0 );
    return pitems[nindex];
  }

  template <class T, class M>
  inline  int   array<T, M>::SetLimit( int newlimit )
  {
    if ( newlimit < ncount && SetLen( newlimit ) != 0 )
      return ENOMEM;
    if ( newlimit != nlimit )
    {
      T*  newitems;

      if ( newlimit != 0 )
      {
        if ( (newitems = (T*)malloc.alloc( newlimit * sizeof(T) )) == NULL )
          return ENOMEM;
        memmove( newitems, pitems, ncount * sizeof(T) );
          malloc.free( pitems );
        pitems = newitems;
      }
        else
      this->~array<T, M>();
    }
    nlimit = newlimit;
    return 0;
  }

  template <class T, class M>
  template <class _func_>
  inline  int   array<T, M>::for_each( _func_ func ) const
  {
    int   nerror;

    for ( auto p = begin(); p < end(); ++p )
      if ( (nerror = func( *p )) != 0 )
        return nerror;

    return 0;
  }

  template <class T, class M>
  template <class _func_>
  inline  int   array<T, M>::for_each( _func_ func )
  {
    int   nerror;

    for ( auto p = begin(); p < end(); ++p )
      if ( (nerror = func( *p )) != 0 )
        return nerror;

    return 0;
  }

  template <class T, class M>
  template <class _func_>
  inline  int   array<T, M>::DeleteIf( _func_ _if_ )
  {
    for ( int i = 0; i < size(); ++i )
      if ( _if_( (*this)[i] ) )
        Delete( i-- );
    return 0;
  }

  template <class T, class M>
  inline  int   array<T, M>::DeleteIf( const T& t )
  {
    for ( int i = 0; i < size(); ++i )
      if ( t == (*this)[i] )
        Delete( i-- );
    return 0;
  }

  template <class T, class M>
  template <class _type_>
  inline  int   array<T, M>::insert( int nindex, _type_ t )
  {
    return insert( nindex, 1, &t );
  }

  template <class T, class M>
  template <class _type_>
  inline  int   array<T, M>::insert( int nindex, int c, _type_* p )
  {
  // Check if valid arguments passed
    if ( nindex < 0 || nindex > ncount )
      return EINVAL;

    assert( ncount <= nlimit );

  // Ensure enough space
    if ( ncount + c > nlimit )
    {
      int   newlimit = ((nlimit + c + ndelta - 1) / ndelta) * ndelta;
        assert( newlimit >= ncount + c );
      T*    newitems;

      assert( newlimit > nindex );

    // Allocate new space
      if ( (newitems = (T*)malloc.alloc( newlimit * sizeof(T) )) == NULL )
        return ENOMEM;

    // Copy the data
      if ( ncount > 0 )
        memcpy( newitems, pitems, ncount * sizeof(T) );

    // Set new buffer
      if ( pitems != NULL )
        malloc.free( pitems );

      pitems = newitems;
      nlimit = newlimit;
    }

  // Check if the space would be prepared
    if ( nindex < ncount )
      memmove( pitems + nindex + c, pitems + nindex, (ncount - nindex) * sizeof(T) );

  // Create the element with the copy constructor
    for ( ncount += c; c-- > 0; )
      __safe_array_construct_cpy( pitems + nindex++, *p++ );

    return 0;
  }

}  // mtc namespace

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif // _MSC_VER

# endif  // __mtc_array_h__
