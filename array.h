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
# if !defined( __mtc_array_h__ )
# define  __mtc_array_h__
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <cassert>
# include "platform.h"
# include <atomic>
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

  template <class T, class this_type>
  class _base_array
  {
    this_type&  _this() {  return *(this_type*)this;  }
    const this_type&  _this() const {  return *(const this_type*)this;  }

  public:     // searchers
                        int   Lookup( const T& t ) const          {  return mtc::Lookup( _this().begin(), _this().end(), t );  }
    template <class P>  int   Lookup( P p ) const                 {  return mtc::Lookup( _this().begin(), _this().end(), p );  }
                        bool  Search( const T& t, int& p ) const  {  return mtc::Search( _this().begin(), _this().end(), t, p );  }
    template <class C>  bool  Search( C c, int& p ) const         {  return mtc::Search( _this().begin(), _this().end(), c, p );  }

  public:     // for_*
    template <class _func> int    for_each( _func func ) const  {  return mtc::for_each( _this().begin(), _this().end(), func );  }
    template <class _func> int    for_each( _func func )        {  return mtc::for_each( _this().begin(), _this().end(), func );  }

    template <class _func> void   for_all( _func  func ) const  {  return mtc::for_all( _this().begin(), _this().end(), func );  }
    template <class _func> void   for_all( _func  func )        {  return mtc::for_all( _this().begin(), _this().end(), func );  }

  public:     // operators
    operator        T* ()       {  return _this().begin();  }
    operator const  T* () const {  return _this().begin();  }
          T&  operator [] ( int i )       {  assert( i >= 0 && i < _this().size() );  return _this().begin()[i];  }
    const T&  operator [] ( int i ) const {  assert( i >= 0 && i < _this().size() );  return _this().begin()[i];  }

  public:     // stl compatibility
    T&        at( int i )       {  assert( _this().size() > 0 && i < _this().size() );  return _this().begin()[i];  }
    const T&  at( int i ) const {  assert( _this().size() > 0 && i < _this().size() );  return _this().begin()[i];  }
    T&        first()           {  assert( _this().size() > 0 );  return _this().begin()[0];  }
    const T&  first() const     {  assert( _this().size() > 0 );  return _this().begin()[0];  }
    T&        last()            {  assert( _this().size() > 0 );  return _this().begin()[_this().size() - 1];  }
    const T&  last() const      {  assert( _this().size() > 0 );  return _this().begin()[_this().size() - 1];  }

  };

  template <class T, class M = def_alloc>
  class array: public _base_array<T, array<T, M>>
  {
    M     allocatorObject;

  public:     // construction
          array( int adelta = 0x10 );
          array( M& m, int adelta = 0x10 );
          array( const array<T, M>& );
         ~array();
    array<T, M>& operator =( const array<T, M>& );

  public:     // set allocator
    M&    GetAllocator()              {  return allocatorObject;     }
    M&    SetAllocator( const M& m )  {  return allocatorObject = m; }

  public:     // members
    int   Append( const T& t )                    {  return Insert( size(), t );      }
    int   Append( T&& t )                         {  return Insert( size(), static_cast<T&&>( t ) );  }
    int   Append( int c, const T* p )             {  return Insert( size(), c, p );   }
    int   Append( const array<T, M>& r )          {  return Insert( size(), r );      }
    int   Append( array<T, M>& r )                {  return Insert( size(), r );      }
    int   Insert( int p, const T& t );
    int   Insert( int p, T&& t );
    int   Insert( int p, int c, const T* t )      {  return Insert( p, c, (T*)t );    }
    int   Insert( int p, int c, T* t );
    int   Insert( int p, const array<T, M>& t )   {  return Insert( p, t.size(), (const T*)t ); }
    int   Insert( int p, array<T, M>& t )         {  return Insert( p, t.size(), (T*)t );       }
    int   Delete( int );
    int   GetLen() const;
    int   SetLen( int );
    void  DelAll();

  public:     // stl compat
    int       size() const  {  return ncount;  }
    T*        begin()       {  return pitems;  }
    const T*  begin() const {  return pitems;  }
    T*        end()         {  return pitems + ncount;  }
    const T*  end() const   {  return pitems + ncount;  }

  public:     // customizing
    int   GetLimit() const  {  return nlimit;   }
    int   GetDelta() const  {  return ndelta;   }
    int   SetLimit( int );
    void  SetDelta( int n ) {  ndelta = n;      }

  public:     // delete
    template <class _test>  void  DeleteIf( _test );
                            void  DeleteIf( const T& );

  protected:  // helpers
    int       GrowTo( int newlen );

  protected:
    T*    pitems;
    int   ncount;
    int   nlimit;
    int   ndelta;

  };

  template <class T>
  class inline_array: public _base_array<T, inline_array<T>>
  {
    const T*  pitems;
    int       ncount;

  public:     // construction
    inline_array(): pitems( nullptr ), ncount( 0 )  {}
    inline_array( const T* p, int l ): pitems( p ), ncount( l ) {}
    template <class A>
    inline_array( const A& a ): pitems( a.begin() ), ncount( a.size() ) {}
    template <class A>
    inline_array& operator = ( const A& a )
      {
        pitems = a.begin();
        ncount = a.size();
        return *this;
      }

  public:     // stl compatibility
    int       size() const  {  return ncount;  }
    const T*  begin() const {  return pitems;  }
    const T*  end() const   {  return ncount + begin();  }

  };

  template <class T, int L>
  class static_array: public _base_array<T, static_array<T, L>>
  {
    int   ncount;
    char  aitems[L * sizeof(T)];

  public:     // construction
    static_array();
    static_array( const static_array& );
   ~static_array();
    static_array& operator = ( const static_array& );

    enum
    {
      limit = L
    };

  public:     // operations
    int   Append( const T& t )                    {  return Insert( size(), t );      }
    int   Append( T&& t )                         {  return Insert( size(), static_cast<T&&>( t ) );  }
    int   Append( int c, const T* p )             {  return Insert( size(), c, p );   }
    int   Append( const static_array& r )         {  return Insert( size(), r );      }
    int   Append( static_array& r )               {  return Insert( size(), r );      }
    int   Insert( int p, const T& t );
    int   Insert( int p, T&& t );
    int   Insert( int p, int c, const T* t )      {  return Insert( p, c, (T*)t );    }
    int   Insert( int p, int c, T* t );
    int   Insert( int p, const static_array& t )  {  return Insert( p, t.size(), (const T*)t ); }
    int   Insert( int p, static_array& t )        {  return Insert( p, t.size(), (T*)t );       }
    int   Delete( int );
    int   GetLen() const                          {  return ncount;  }
    int   SetLen( int );
    void  DelAll();

  public:     // delete
    template <class _test> void   DeleteIf( _test );
                            void  DeleteIf( const T& );

  public:     // stl compatibility
    int       size() const  {  return ncount;  }
    const T*  begin() const {  return (const T*)aitems;  }
    T*        begin()       {  return (T*)aitems;  }
    const T*  end() const   {  return ncount + begin();  }
    T*        end()         {  return ncount + begin();  }

  };

  template <class T, class M = def_alloc>
  class shared_array: public _base_array<T, shared_array<T, M>>
  {
    M allocatorObject;

    struct array_data: public array<T, M>
    {
      std::atomic_int refcount;

    public:
    
      array_data( int d ): array<T, M>( d ), refcount( 0 )  {}
    };

    array_data* parray;
    int         ndelta;

  protected:  // helper
    bool  Ensure()
      {
        if ( parray == nullptr && (parray = allocate_with<array_data>( GetAllocator(), ndelta )) != nullptr )
          {  ++parray->refcount;  parray->SetAllocator( GetAllocator() );  }
        return parray != nullptr;
      }
  public:     // construction
    shared_array( int adelta = 0x10 ): parray( nullptr ), ndelta( adelta )  {}
    shared_array( const M& ralloc, int adelta = 0x10 ): malloc( ralloc ), parray( nullptr ), ndelta( adelta ) {}
    shared_array( const shared_array& );
   ~shared_array();
    shared_array& operator = ( const shared_array& );

  public:     // set allocator
    M&    GetAllocator()              {  return allocatorObject;     }
    M&    SetAllocator( const M& m )  {  return allocatorObject = m; }

  public:     // API
    int   Append( const T& t )  {  return Ensure() ? parray->Insert( size(), t ) : ENOMEM;  }
    int   Append( T&& t )       {  return Ensure() ? parray->Insert( size(), t ) : ENOMEM;  }
    int   Append( int c, const T* p )     {  return Ensure() ? parray->Insert( size(), c, p ) : ENOMEM;   }
    int   Append( const array<T, M>& r )  {  return Ensure() ? parray->Insert( size(), r ) : ENOMEM;  }
    int   Append( array<T, M>& r )    {  return Ensure() ? parray->Insert( size(), r ) : ENOMEM;  }
    int   Insert( int i, const T& t ) {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Insert( int i, T&& t )      {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Insert( int i, int c, const T* t )  {  return Ensure() ? parray->Insert( i, c, t ) : ENOMEM;  }
    int   Insert( int i, int c, T* t )        {  return Ensure() ? parray->Insert( i, c, t ) : ENOMEM;  }
    int   Insert( int i, const array<T, M>& t ) {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Insert( int i,       array<T, M>& t ) {  return Ensure() ? parray->Insert( i, t ) : ENOMEM;  }
    int   Delete( int n ) {  return parray == nullptr ? 0 : parray->Delete( n );  }
    int   GetLen() const  {  return parray != nullptr ? parray->GetLen() : 0;  }
    int   SetLen( int l )
      {
        if ( l == 0 )
        {
          if ( parray != nullptr && --parray->refcount == 0 )
            deallocate_with( GetAllocator(), parray );
          parray = nullptr;
            return 0;
        }
        return Ensure() ? parray->SetLen( l ) : ENOMEM;
      }
    void  DelAll()
      {
        if ( parray != nullptr && --parray->refcount == 0 )
          deallocate_with( GetAllocator(), parray );
        parray = nullptr;
      }
    int   Privatize()
      {
        if ( parray != nullptr && parray->refcount != 1 )
        {
          array_data* palloc;

          if ( (palloc = allocate_with<array_data>( GetAllocator(), parray->GetDelta() )) == nullptr )
            return ENOMEM;  else parray->SetAllocator( GetAllocator() );

          if ( palloc->Append( *parray ) != 0 )
          {
            deallocate_with( GetAllocator(), palloc );
            return ENOMEM;
          }

          if ( --parray->refcount == 0 )
            deallocate_with( parray->GetAllocator(), parray );

          ++(parray = palloc)->refcount;
        }
        return 0;
      }

  public:     // customizing
    int   GetLimit() const  {  return parray != nullptr ? parray->GetLimit() : 0;  }
    int   GetDelta() const  {  return parray != nullptr ? parray->GetDelta() : ndelta;  }
    int   SetLimit( int l ) {  return Ensure() ? parray->SetLimit( l ) : ENOMEM;  }
    void  SetDelta( int n ) {  ndelta = n;  if ( parray != nullptr )  parray->SetDelta( ndelta );  }

  public:     // delete
    template <class _func_> void  DeleteIf( _func_ func ) {  if ( parray != nullptr ) parray->DeleteIf( func );  }
                            void  DeleteIf( const T& t )  {  if ( parray != nullptr ) parray->DeleteIf( t );  }

  public:     // stl compatibility
    int       size() const  {  return parray != nullptr ? parray->size() : 0;  }
    T*        begin()       {  return parray != nullptr ? parray->begin() : nullptr;  }
    T*        end()         {  return parray != nullptr ? parray->end() : nullptr;  }
    const T*  begin() const {  return parray != nullptr ? parray->begin() : nullptr;  }
    const T*  end() const   {  return parray != nullptr ? parray->end() : nullptr;  }

  };

// array inline implementation

  template <class T, class M>
  array<T, M>::array( int adelta ):
    pitems( nullptr ), ncount( 0 ), nlimit( 0 ), ndelta( adelta <= 0 ? 0x10 : adelta )
  {
  }

  template <class T, class M>
  array<T, M>::array( M& m, int adelta ):
    allocatorObject( m ), pitems( nullptr ), ncount( 0 ), nlimit( 0 ), ndelta( adelta <= 0 ? 0x10 : adelta )
  {
  }

  template <class T, class M>
  array<T, M>::array( const array<T, M>& r ):
    allocatorObject( r.allocatorObject ), pitems( r.pitems ), ncount( r.ncount ),  nlimit( r.nlimit ), ndelta( r.ndelta )
  {
    ((array<T, M>&)r).pitems = nullptr;
    ((array<T, M>&)r).ncount = 0;
  }

  template <class T, class M>
  array<T, M>& array<T, M>::operator = ( const array<T, M>& r )
  {
    this->~array<T, M>();
      SetAllocator( r.allocatorObject );
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
      GetAllocator().free( pitems );
    }
  }

  template <class T, class M>
  inline  int   array<T, M>::Insert( int nindex, const T& t )
  {
    if ( nindex < 0 || nindex > ncount )
      return EINVAL;

    assert( ncount <= nlimit );

  // Ensure enough space
    if ( ncount >= nlimit && GrowTo( ncount + 1 ) != 0 )
      return ENOMEM;

  // Check if the space would be prepared
    if ( nindex < ncount )
      memmove( pitems + nindex + 1, pitems + nindex, (ncount - nindex) * sizeof(T) );

  // Create the element with the copy constructor
    new( pitems + nindex ) T( t );  ++ncount;
      return 0;
  }

  template <class T, class M>
  inline  int   array<T, M>::Insert( int nindex, T&& t )
  {
    if ( nindex < 0 || nindex > ncount )
      return EINVAL;

    assert( ncount <= nlimit );

  // Ensure enough space
    if ( ncount >= nlimit && GrowTo( ncount + 1 ) != 0 )
      return ENOMEM;

  // Check if the space would be prepared
    if ( nindex < ncount )
      memmove( pitems + nindex + 1, pitems + nindex, (ncount - nindex) * sizeof(T) );

  // Create the element with the copy constructor
    new( pitems + nindex ) T( static_cast<T&&>( t ) );  ++ncount;
      return 0;
  }

  template <class T, class M>
  inline  int   array<T, M>::Insert( int nindex, int c, T* p )
  {
    if ( nindex < 0 || nindex > ncount )
      return EINVAL;

    assert( ncount <= nlimit );

  // Ensure enough space
    if ( ncount + c > nlimit && GrowTo( ncount + c ) != 0 )
      return ENOMEM;

  // Check if the space would be prepared
    if ( nindex < ncount )
      memmove( pitems + nindex + c, pitems + nindex, (ncount - nindex) * sizeof(T) );

  // Create the element with the copy constructor
    for ( ncount += c; c-- > 0; )
      new( pitems + nindex++ ) T( *p++ );

    return 0;
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
        if ( (newitems = (T*)GetAllocator().alloc( newlimit * sizeof(T) )) == NULL )
          return ENOMEM;

      // Copy the data
        if ( ncount > 0 )
          memcpy( newitems, pitems, ncount * sizeof(T) );

      // Set new buffer
        if ( pitems != NULL )
          GetAllocator().free( pitems );
        pitems = newitems;
        nlimit = newlimit;
      }
      __safe_array_construct_def( pitems + ncount, length - ncount );
    }
      else
    if ( length == 0 )
    {
      GetAllocator().free( pitems );
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
    GetAllocator().free( pitems );
    pitems = 0;
    nlimit = 0;
    ncount = 0;
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
        if ( (newitems = (T*)GetAllocator().alloc( newlimit * sizeof(T) )) == NULL )
          return ENOMEM;
        memmove( newitems, pitems, ncount * sizeof(T) );
          GetAllocator().free( pitems );
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
  inline  void  array<T, M>::DeleteIf( _func_ _if_ )
  {
    for ( int i = 0; i < size(); ++i )
      if ( _if_( (*this)[i] ) )
        Delete( i-- );
  }

  template <class T, class M>
  inline  void  array<T, M>::DeleteIf( const T& t )
  {
    for ( int i = 0; i < size(); ++i )
      if ( t == (*this)[i] )
        Delete( i-- );
  }

  template <class T, class M>
  inline  int   array<T, M>::GrowTo( int newlen )
  {
    int   newlimit = ((newlen + ndelta - 1) / ndelta) * ndelta;
    T*    newitems;

    assert( newlimit >= newlen && newlimit >= ncount );

  // Allocate new space
    if ( (newitems = (T*)GetAllocator().alloc( newlimit * sizeof(T) )) == NULL )
      return ENOMEM;

  // Copy the data
    if ( ncount > 0 )
      memcpy( newitems, pitems, ncount * sizeof(T) );

  // Set new buffer
    if ( pitems != NULL )
      GetAllocator().free( pitems );

    pitems = newitems;
    nlimit = newlimit;
      return 0;
  }

  // static_array template implementation

  template <class T, int L>
  static_array<T, L>::static_array(): ncount( 0 )
  {
  }

  template <class T, int L>
  static_array<T, L>::static_array( const static_array& a ): ncount( a.ncount )
  {
    const T*  s;
          T*  d;

    for ( s = a.begin(), d = begin(); s < a.end(); )
      new( d++ ) T( *s++ );
  }

  template <class T, int L>
  static_array<T, L>::~static_array()
  {
    __safe_array_destruct( begin(), size() );
  }

  template <class T, int L>
  static_array<T, L>& static_array<T, L>::operator = ( const static_array<T, L>& a )
  {
    const T*  s;
          T*  d;

    if ( size() > a.size() )
      __safe_array_destruct( begin() + a.size(), a.size() - size() );

    for ( s = a.begin(), d = begin(); s < a.end() && d < end(); )
      *d++ = *s++;

    while ( s < a.end() )
      new( d++ ) T( *s++ );

    ncount = a.ncount;
      return *this;
  }

  template <class T, int L>
  int   static_array<T, L>::Insert( int p, const T& t )
  {
    if ( p < 0 || p > ncount || ncount >= (int)L )
      return EINVAL;

    if ( p < ncount )
      memmove( begin() + p + 1, begin() + p, (ncount - p) * sizeof(T) );

    new( begin() + p ) T( t );  ++ncount;
      return 0;
  }

  template <class T, int L>
  int   static_array<T, L>::Insert( int p, T&& t )
  {
    if ( p < 0 || p > ncount || ncount >= (int)L )
      return EINVAL;

    if ( p < ncount )
      memmove( begin() + p + 1, begin() + p, (ncount - p) * sizeof(T) );

    new( begin() + p ) T( static_cast<T&&>( t ) );  ++ncount;
      return 0;
  }

  template <class T, int L>
  int   static_array<T, L>::Insert( int p, int c, T* t )
  {
    if ( p < 0 || p > ncount || (ncount + c) > (int)L )
      return EINVAL;

    if ( p < ncount )
      memmove( begin() + p + c, begin() + p, (ncount - p) * sizeof(T) );

    for ( ncount += c; c-- > 0; )
      new( begin() + p++ ) T( *t++ );

    return 0;
  }

  template <class T, int L>
  int   static_array<T, L>::Delete( int i )
  {
    if ( i < 0 || i >= ncount )
      return EINVAL;

    __safe_array_destruct( begin() + i, 1 );
      memmove( begin() + i, begin() + i + 1, (ncount - i - 1) * sizeof(T) );

    --ncount;
      return 0;
  }

  template <class T, int L>
  int   static_array<T, L>::SetLen( int newlen )
  {
    if ( newlen > (int)L )
      return ENOMEM;
    if ( newlen < ncount )
      __safe_array_destruct( begin() + newlen, ncount - newlen );
    if ( newlen > ncount )
      __safe_array_construct_def( begin() + ncount, newlen - ncount );
    ncount = newlen;
      return 0;
  }

  template <class T, int L>
  void  static_array<T, L>::DelAll()
  {
    __safe_array_destruct( begin(), size() );
      ncount = 0;
  }

  template <class T, int L>
  template <class _func>
  void  static_array<T, L>::DeleteIf( _func test )
  {
    T*  p;

    for ( p = begin(); p < end(); ++p )
      if ( test( *p ) ) Delete( (p--) - begin() );
  }

  template <class T, int L>
  void  static_array<T, L>::DeleteIf( const T& t )
  {
    T*  p;

    for ( p = begin(); p < end(); ++p )
      if ( *p == t ) Delete( (p--) - begin() );
  }

// shared_array implementation

  template <class T, class M>
  shared_array<T, M>::shared_array( const shared_array& a ): allocatorObject( a.allocatorObject ), ndelta( a.ndelta )
  {
    if ( (parray = a.parray) != nullptr )
      ++parray->refcount;
  }

  template <class T, class M>
  shared_array<T, M>::~shared_array()
  {
    if ( parray != nullptr && --parray->refcount == 0 )
      deallocate_with( GetAllocator(), parray );
  }

  template <class T, class M>
  shared_array<T, M>& shared_array<T, M>::operator = ( const shared_array& a )
  {
    if ( parray != nullptr && --parray->refcount == 0 )
      deallocate_with( GetAllocator(), parray );
    if ( (parray = a.parray) != nullptr )
      ++parray->refcount;
    SetAllocator( a.allocatorObject );
      return *this;
  }

}  // mtc namespace

# if defined( _MSC_VER )
#   pragma warning( pop )
# endif // _MSC_VER

# endif  // __mtc_array_h__
