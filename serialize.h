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
# if !defined( __mtc_serialize__ )
# define  __mtc_serialize__
# include <cstdlib>
# include <cstring>
# include <string>
# include <vector>
# include <map>

namespace mtc
{
  class sourcebuf
  {
    const char* p;
    const char* e;

  public:     // construction
    sourcebuf( const void* t = nullptr, size_t l = 0 ) noexcept: p( (char*)t ), e( l + (char*)t ) {}
    sourcebuf( const sourcebuf& s ): p( s.p ), e( s.e ) {}
    sourcebuf&  operator = ( const sourcebuf& s )
    {
      p = s.p;
      e = s.e;
      return *this;
    }
    sourcebuf* ptr() const {  return (sourcebuf*)this;  }
    operator sourcebuf* () const    {  return ptr();  }
    const char* getptr() const      {  return p < e ? p : nullptr;  }
    sourcebuf*  skipto( size_t l )  {  return (p = l + p) <= e ? this : nullptr;  }

  public:     // fetch
    sourcebuf*  FetchFrom( void* o, size_t l )  {  return p + l <= e ? (memcpy( o, p, l ), p += l, this) : (p = e, nullptr);  }
  };

  class serialbuf
  {
    const void* data;
    size_t      size;

  public:
    serialbuf( const void* p, size_t l ): data( p ), size( l ) {}
    serialbuf( const serialbuf& s ): data( s.data ), size( s.size ) {}

  public:
    template <class O> O*  Serialize( O* o ) const;
  };

}

/*
 * common serialization templates declaration for base get/put operations   []
 */

template <class O>  O*  Serialize( O*, const void*, size_t );
template <class S>  S*  FetchFrom( S*,       void*, size_t );

/*
 * base i/o specializations declarations
 */

template <> inline  auto  Serialize( char* o, const void* p, size_t l ) -> char*
  {  return o != nullptr ? l + (char*)memcpy( o, p, l ) : nullptr;  }
template <> inline  auto  Serialize( unsigned char* o, const void* p, size_t l ) -> unsigned char*
  {  return o != nullptr ? l + (unsigned char*)memcpy( o, p, l ) : nullptr;  }
template <> inline  auto  Serialize( FILE* o, const void* p, size_t l ) -> FILE*
  {  return o != nullptr && fwrite( p, sizeof(char), l, o ) == l ? o : nullptr;  }

template <> inline  auto  FetchFrom( const char* s, void* p, size_t l ) -> const char*
  {  return s != nullptr ? (memcpy( p, s, l ), l + s) : nullptr;  }
template <> inline  auto  FetchFrom( const unsigned char* s, void* p, size_t l ) -> const unsigned char*
  {  return s != nullptr ? (memcpy( p, s, l ), l + s) : nullptr;  }
template <> inline  auto  FetchFrom( FILE* s, void* p, size_t l ) -> FILE*
  {  return s != nullptr && fread( p, sizeof(char), l, s ) == l ? s : nullptr;  }

/*
 * values serialization/deserialization
 */

template <class O, class T> O*  Serialize( O*, const T& );
template <class S, class T> S*  FetchFrom( S*, T& );

struct integers final
{
  template <class O, class T>
  static inline O*  put( O*  o, T t )
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
      o = Serialize( o, &bstore, sizeof(bstore) );
    } while ( o != NULL && (bstore & 0x80) != 0 );

    return o;
  }
  template <class S, class T>
  static inline S*  get( S* s, T& t )
  {
    int   nshift = 0;
    char  bfetch;

    t = 0;
    do  {
      if ( (s = FetchFrom( s, &bfetch, sizeof(bfetch) )) == nullptr ) return nullptr;
      else  t |= (((T)bfetch & 0x7f)) << (nshift++ * 7);
    } while ( bfetch & 0x80 );

    return s;
  }
};

/*
 * values serialization/deserialization for standard types
 */
template <class O>  inline  O*  Serialize( O* o, const char& c )          {  return Serialize( o, &c, sizeof(c) );  }
template <class O>  inline  O*  Serialize( O* o, const unsigned char& c ) {  return Serialize( o, &c, sizeof(c) );  }
template <class O>  inline  O*  Serialize( O* o, const float&  f )        {  return Serialize( o, &f, sizeof(f) );  }
template <class O>  inline  O*  Serialize( O* o, const double& d )        {  return Serialize( o, &d, sizeof(d) );  }
template <class O>  inline  O*  Serialize( O* o, const bool& b )          {  return Serialize( o, (char)(b ? 1 : 0) );  }

template <class O>  inline  O*  Serialize( O* o, const int16_t& i )   {  return integers::put( o, i );  }
template <class O>  inline  O*  Serialize( O* o, const int32_t& i )   {  return integers::put( o, i );  }
template <class O>  inline  O*  Serialize( O* o, const int64_t& i )   {  return integers::put( o, i );  }

template <class O>  inline  O*  Serialize( O* o, const uint16_t& i )  {  return integers::put( o, i );  }
template <class O>  inline  O*  Serialize( O* o, const uint32_t& i )  {  return integers::put( o, i );  }
template <class O>  inline  O*  Serialize( O* o, const uint64_t& i )  {  return integers::put( o, i );  }

template <class S>  inline  S*  FetchFrom( S* s, char& c )          {  return FetchFrom( s, &c, sizeof(c) );  }
template <class S>  inline  S*  FetchFrom( S* s, unsigned char& c ) {  return FetchFrom( s, &c, sizeof(c) );  }
template <class S>  inline  S*  FetchFrom( S* s, float&  f )        {  return FetchFrom( s, &f, sizeof(f) );  }
template <class S>  inline  S*  FetchFrom( S* s, double& d )        {  return FetchFrom( s, &d, sizeof(d) );  }
template <class S>  inline  S*  FetchFrom( S* s, bool& b )
{
  char  c;

  return b = (s = FetchFrom( s, c )) != nullptr && c != 0, s;
}

template <class S>  inline  S*  FetchFrom( S* s, int16_t& i )   {  return integers::get( s, i );  }
template <class S>  inline  S*  FetchFrom( S* s, int32_t& i )   {  return integers::get( s, i );  }
template <class S>  inline  S*  FetchFrom( S* s, int64_t& i )   {  return integers::get( s, i );  }

template <class S>  inline  S*  FetchFrom( S* s, uint16_t& i )  {  return integers::get( s, i );  }
template <class S>  inline  S*  FetchFrom( S* s, uint32_t& i )  {  return integers::get( s, i );  }
template <class S>  inline  S*  FetchFrom( S* s, uint64_t& i )  {  return integers::get( s, i );  }

/*
 * C strings serialization/deserialization specializations
 */
template <class O>  inline  O*  Serialize( O* o, const char* const& s )
{
  auto  length = strlen( s );

  return Serialize( Serialize( o, length ), (const void*)s, sizeof(*s) * length );
}

template <class S>  inline  S*  FetchFrom( S* s, char*&  r )
{
  unsigned  length;

  if ( (s = FetchFrom( s, length )) != nullptr )
  {
    if ( (r = (char*)malloc( length + 1 )) == nullptr )
      return nullptr;
    if ( (s = FetchFrom( s, r, length )) == nullptr ) free( r );
      else  r[length] = '\0';
  }
  return s;
}

template <class O>  inline  O*  Serialize( O* o, char* s )  {  return Serialize( o, (const char*)s );  }
template <class S>  inline  S*  FetchFrom( S* s, const char*& r ) {  return FetchFrom( s, (char*&)r );  }

/*
 * std:: types serialization/deserialization specializations
 */

template <class O,
class C>  O*  Serialize( O* o, const std::basic_string<C>& s )
{
  return ::Serialize( ::Serialize( o, s.length() ), s.c_str(), sizeof(C) * s.length() );
}

template <class T>
struct class_is_string
{
  static const bool value = false;
};

template <class T>
struct class_is_string<std::basic_string<T>>
{
  static const bool value = true;
};

struct store_as_scalar
{
  template <class O, class T>
  O*  operator()( O* o, const T& t )  {  return ::Serialize( o, t );  }
};

struct store_as_serial
{
  template <class O, class T>
  O*  operator()( O* o, const T& t )  {  return t.Serialize( o );  }
};

template <class O,
class T>  O*  Serialize( O* o, const std::vector<T>& a )
{
  using serialize = typename std::conditional<std::is_fundamental<T>::value || class_is_string<T>::value,
    store_as_scalar,
    store_as_serial>::type;

  o = ::Serialize( o, a.size() );

  for ( auto& element: a )
    o = serialize()( o, element );

  return o;
}

template <class O,
class K,
class V>  O*  Serialize( O* o, const std::map<K, V>& m )
{
  o = ::Serialize( o, m.size() );

  for ( auto ptr = m.begin(); o != nullptr && ptr != m.end(); ++ptr )
    o = ::Serialize( ::Serialize( o, ptr->first ), ptr->second );

  return o;
}

template <class S,
class C>  S*  FetchFrom( S* s, std::basic_string<C>& o )
{
  int   l;

  o.clear();

  if ( (s = ::FetchFrom( s, l )) == nullptr )
    return nullptr;

  o.reserve( (l + 0x10) & ~0x0f );
  o.resize( l );
  o[l] = (C)0;

  if ( (s = ::FetchFrom( s, (C*)o.c_str(), l * sizeof(C) )) == nullptr )
    o.clear();

  return s;
}

struct fetch_as_scalar
{
  template <class S, class T>
  S*  operator()( S* s, T& t )  {  return ::FetchFrom( s, t );  }
};

struct fetch_as_serial
{
  template <class S, class T>
  S*  operator()( S* s, T& t )  {  return t.FetchFrom( s );  }
};

template <class S,
class T>  inline  S*  FetchFrom( S* s, std::vector<T>& a )
{
  using fetchfrom = typename std::conditional<std::is_fundamental<T>::value || class_is_string<T>::value,
    fetch_as_scalar,
    fetch_as_serial>::type;

  int   length;

  a.clear();

  if ( (s = ::FetchFrom( s, length )) == nullptr )
    return s;

  a.reserve( (length + 0x0f) & ~0x0f );
  a.resize( length );

  for ( auto i = 0; i < length && s != nullptr; ++i )
    s = fetchfrom()( s, a.at( i ) );

  return s;
}

template <class S,
class K,
class V>  inline  S*  FetchFrom( S* s, std::map<K, V>& m )
{
  size_t  len;

  s = ::FetchFrom( s, len );

  for ( m.clear(); s != nullptr && len-- > 0; )
  {
    std::pair<K, V> pair;

    if ( (s = ::FetchFrom( ::FetchFrom( s, pair.first ), pair.second )) != nullptr )
      m.insert( m.end(), std::move( pair ) );
  }

  return s;
}

/*
 * helpers for buffers
 */
template <> inline  auto  FetchFrom( mtc::sourcebuf* s, void* p, size_t l ) -> mtc::sourcebuf*
  {  return s != nullptr ? s->FetchFrom( p, l ) : nullptr;  }

/*
 * GetBufLen() family
 */

template <class T>
size_t  GetBufLen( const T& );

template <class T>  inline
size_t  GetBufLen( const T& dwdata )
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

template <> inline  size_t  GetBufLen( const char& )          {  return 1;  }
template <> inline  size_t  GetBufLen( const unsigned char& ) {  return 1;  }
template <> inline  size_t  GetBufLen( const bool& )          {  return 1;  }
template <> inline  size_t  GetBufLen( const float& )         {  return sizeof(float);  }
template <> inline  size_t  GetBufLen( const double& )        {  return sizeof(double);  }

template <>
inline  size_t  GetBufLen( const char* const& string )
{
  auto length = strlen( string );

  return sizeof(*string) * length + GetBufLen( length );
}

template <> inline
size_t  GetBufLen( char* const& string )
  {  return GetBufLen( (char * const&)string );  }

template <class C> inline
size_t  GetBufLen( const std::basic_string<C>& s )  {  return ::GetBufLen( s.length() ) + sizeof(C) * s.length();  }

template <class T> inline
size_t  GetBufLen( const std::vector<T>& a )
{
  size_t  cc = ::GetBufLen( a.size() );

  for ( auto& t: a )
    cc += ::GetBufLen( t );

  return cc;
}

template <class K,
          class V> inline
size_t  GetBufLen( const std::map<K, V>& m )
{
  size_t  cch = ::GetBufLen( m.size() );

  for ( auto ptr = m.begin(); ptr != m.end(); ++ptr )
    cch += ::GetBufLen( ptr->first ) + ::GetBufLen( ptr->second );

  return cch;
}

namespace mtc
{
  template <class O> O*  serialbuf::Serialize( O* o ) const  {  return ::Serialize( o, data, size );  }
}

# endif  // __mtc_serialize__
