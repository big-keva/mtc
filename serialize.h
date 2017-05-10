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
# if !defined( __mtc_serialize_h__ )
# define  __mtc_serialize_h__
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <errno.h>

class sourcebuf;

sourcebuf*  FetchFrom( sourcebuf*, char& );
sourcebuf*  FetchFrom( sourcebuf*, void*, unsigned );

class sourcebuf
{
  const void* p;
  const void* e;

  friend sourcebuf* ::FetchFrom( sourcebuf*, char& );
  friend sourcebuf* ::FetchFrom( sourcebuf*, void*, unsigned );

public:     // construction
  template <class T>
  sourcebuf( const T* t = nullptr, unsigned l = 0 ): p( t ), e( t + l ) {}
  sourcebuf( const sourcebuf& s ): p( s.p ), e( s.e ) {}
  sourcebuf&  operator = ( const sourcebuf& s )
    {
      p = s.p;
      e = s.e;
      return *this;
    }
  const char* getptr() const
    {
      return (const char*)p < (const char*)e ? (const char*)p : nullptr;
    }
  sourcebuf*  skipto( unsigned l )
    {
      return (p = l + (char*)p) < e ? this : nullptr;
    }
};

inline  sourcebuf*  FetchFrom( sourcebuf* s, char& c )
  {
    if ( s == nullptr || s->p >= s->e )
      return nullptr;
    c = *(char*)s->p;  s->p = 1 + (char*)s->p;
      return s;
  }

inline  sourcebuf*  FetchFrom( sourcebuf* s, void* p, unsigned l )
  {
    if ( s == nullptr || l + (char*)s->p >= s->e )
      return nullptr;
    memcpy( p, s->p, l );
      s->p = l + (char*)s->p;
    return s;
  }

//[]=========================================================================[]

template <class T>
inline  size_t  GetBufLen( T dwdata )
  {
    T       bitest = 0x007f;
    size_t  ncount = 1;

    while ( (dwdata & ~bitest) != 0 )
    {
      bitest = (T)((bitest << 7) | 0x7f);
        ++ncount;
    }
    return ncount;
  }

inline  size_t  GetBufLen(  char  )
  {
    return 1;
  }

inline  size_t  GetBufLen( unsigned char )
  {
    return 1;
  }

inline  size_t  GetBufLen(  bool  )
  {
    return 1;
  }

inline  size_t  GetBufLen( float )
  {
    return sizeof(float);
  }

inline  size_t  GetBufLen( double )
  {
    return sizeof(double);
  }

inline  size_t  GetBufLen( const char*  string )
  {
    auto length = strlen( string );

    return sizeof(*string) * length + GetBufLen( length );
  }

inline  size_t  GetBufLen( char*        string )
  {
    return GetBufLen( (const char*)string );
  }

//[]=========================================================================[]

inline  char* Serialize( char* o, char c )
  {
    if ( o != NULL )
      *o++ = c;
    return o;
  }

inline  char* Serialize( char* o, const void* p, unsigned l )
  {
    return o != NULL ? l + (char*)memcpy( o, p, l ) : NULL;
  }

inline  FILE* Serialize( FILE* o, char c )
  {
    return o != NULL && fwrite( &c, sizeof(char), 1, o ) == 1 ? o : NULL;
  }

inline  FILE* Serialize( FILE* o, const void* p, unsigned l )
  {
    return o != NULL && fwrite( p, sizeof(char), l, o ) == l ? o : NULL;
  }

template <class O>  inline  O* Serialize( O* o, unsigned char b )
  {
    return Serialize( o, (char)b );
  }

template <class O>  inline  O* Serialize( O* o, bool    b )
  {
    return Serialize( o, (char)(b ? 1 : 0) );
  }

template <class O>  inline  O* Serialize( O* o, float   f )
  {
    return Serialize( o, &f, sizeof(f) );
  }

template <class O>  inline  O* Serialize( O* o, double  d )
  {
    return Serialize( o, &d, sizeof(d) );
  }

template <class O, class T>
inline  O*  Serialize( O*  o, T t )
  {
    int   nshift = 0;
    char  bstore;
  
    do
    {
      unsigned  ushift = nshift++ * 7;

      bstore = (char)(((t & (((T)0x7f) << ushift)) >> ushift) & 0x7f);
        t &= ~(((T)0x7f) << ushift);
      if ( t != 0 )
        bstore |= 0x80;
      o = Serialize( o, bstore );
    } while ( o != NULL && (bstore & 0x80) != 0 );

    return o;
  }

template <class O>  inline  O* Serialize( O* o, const char* s )
  {
    auto  length = strlen( s );

    return Serialize( Serialize( o, (unsigned)length ), (const void*)s, (unsigned)(sizeof(*s) * length) );
  }

template <class O>  inline  O*  Serialize( O* o, char* s )
  {
    return Serialize( o, (const char*)s );
  }

//[]=========================================================================[]

template <class T>
inline  int       FetchData( const char*& buffer, T& rfitem )
  {
    int   nshift = 0;
    char  bfetch;

    rfitem = 0;
    do  rfitem |= (((T)(bfetch = *buffer++) & 0x7f)) << (nshift++ * 7);
      while ( bfetch & 0x80 );
    return 0;
  }

inline  int       FetchData( const char*& buffer, char&   rfitem )
  {
    rfitem = *buffer++;
      return 0;
  }

inline  int       FetchData( const char*& buffer, unsigned char& rfitem )
  {
    rfitem = (unsigned char)*buffer++;
      return 0;
  }

inline  int       FetchData( const char*& buffer, bool&   rfitem )
  {
    rfitem = *buffer++ != 0;
      return 0;
  }

inline  int       FetchData( const char*& buffer, float&  rvalue )
  {
    memcpy( &rvalue, buffer, sizeof(rvalue) );
      buffer += sizeof(rvalue);
    return 0;
  }

inline  int       FetchData( const char*& buffer, double& rvalue )
  {
    memcpy( &rvalue, buffer, sizeof(rvalue) );
      buffer += sizeof(rvalue);
    return 0;
  }

inline  int       FetchData( const char*& buffer, const char*&  string )
  {
    unsigned  length;

    if ( (length = (unsigned)*buffer++) & 0x80 )
      length = (length & 0x7f) | ((unsigned)*buffer++ << 7);
    if ( (string = (char*)malloc( length + 1 )) == NULL )
      return ENOMEM;
    memcpy( (char*)string, buffer, length );
      buffer += length;
    ((char*)string)[length] = '\0';
      return 0;
  }

inline  int       FetchData( const char*& buffer, char*&  string )
  {
    return FetchData( buffer, (const char*&)string );
  }

//[]=========================================================================[]

inline  const char* FetchFrom( const char* s, char& c )
  {
    if ( s != NULL )
      c = *s++;
    return s;
  }

inline  const char* FetchFrom( const char* s, void* p, unsigned l )
  {
    if ( s == NULL )
      return NULL;
    memcpy( p, s, l );
      return s + l;
  }

inline  const unsigned char* FetchFrom( const unsigned char* s, char& c )
  {
    return (const unsigned char*)FetchFrom( (const char*)s, c );
  }

inline  const unsigned char* FetchFrom( const unsigned char* s, void* p, unsigned l )
  {
    return (const unsigned char*)FetchFrom( (const char*)s, p, l );
  }

inline  FILE*       FetchFrom( FILE* s, char& c )
  {
    return s != NULL && fread( &c, sizeof(char), 1, s ) == 1 ? s : NULL;
  }

inline  FILE* FetchFrom( FILE* s, void* p, unsigned l )
  {
    return s != NULL && fread( p, sizeof(char), l, s ) == l ? s : NULL;
  }

template <class S> inline  S* FetchFrom( S* s, unsigned char& b )
  {
    return FetchFrom( s, (char&)b );
  }

template <class S> inline  S* FetchFrom( S* s, bool& b )
  {
    char  c;

    if ( (s = FetchFrom( s, c )) != NULL )
      b = c != 0;
    return s;
  }

template <class S> inline  S* FetchFrom( S* s, float& f )
  {
    return FetchFrom( s, &f, sizeof(f) );
  }

template <class S> inline  S* FetchFrom( S* s, double& d )
  {
    return FetchFrom( s, &d, sizeof(d) );
  }

template <class S, class T>
inline  S*  FetchFrom( S* s, T& t )
  {
    int   nshift = 0;
    char  bfetch;

    t = 0;
    do  {
      if ( (s = FetchFrom( s, bfetch )) == NULL ) return NULL;
        else  t |= (((T)bfetch & 0x7f)) << (nshift++ * 7);
    } while ( bfetch & 0x80 );

    return s;
  }

template <class S> inline  S* FetchFrom( S* s, const char*& r )
  {
    unsigned  length;

    if ( (s = FetchFrom( s, length )) == NULL )
      return NULL;
    if ( (r = (char*)malloc( length + 1 )) == NULL )
      return NULL;
    if ( (s = FetchFrom( s, r, length )) == NULL ) free( r );
      else  r[length] = '\0';
    return s;
  }

template <class S> inline  S* FetchFrom( S* s, char*&  r )
  {
    return FetchFrom( s, (const char*&)r );
  }

# endif  // __mtc_serialize_h__
