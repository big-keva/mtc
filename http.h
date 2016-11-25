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
# if !defined( __mtc_http_h__ )
# define __mtc_http_h__
# include "charstream.h"
# include "stringmap.h"
# include "stdlog.h"
# include "array.h"

namespace mtc
{

  class HTTPHeader
  {
    struct  headkey
    {
      const char*   key;
      _auto_<char>  val;
    };

  protected:  // helper classes
    class   charstr
    {
      array<char>&  buffer;
      int           cchstr;

    public:     // construction
      charstr( array<char>& r ): buffer( r ), cchstr( 0 )
        {
        }

    public:     // filling
      int   length() const noexcept
        {
          return cchstr;
        }
      bool  addchr( char c ) noexcept
        {
          if ( cchstr >= buffer.size() - 1 && buffer.SetLen( cchstr + 0x100 ) != 0 )
            return false;
          buffer[cchstr++] = c;  buffer[cchstr] = 0;
            return true;
        }

    };

  public:     // keys
    int         Insert( const char*, const char* ) noexcept;
    const char* Search( const char* ) const noexcept;

  public:     // serialization
    template <class S>  S*    Fetch( S* ) noexcept;
    template <class O>  O*    Store( O* ) noexcept;

  protected:  // helpers
    template <class S>  S*    Fetch( charstream<S>& ) noexcept;
    template <class S>  char  GetCh( charstream<S>& ) noexcept;

  protected:  // variables
    array<headkey>  keyset;
    stringmap<int>  keymap;

  };

  class HTTPRequest: public HTTPHeader
  {
    array<char> method;
    array<char> urlstr;
    array<char> verstr;

  public:     // access
    const char* GetMethod() const noexcept  {  return method.size() > 0 && method.first() != 0 ? method.begin() : "GET"; }
    const char* GetURI() const noexcept     {  return urlstr.size() > 0 && urlstr.first() != 0 ? urlstr.begin() : "/";   }
    const char* GetVersion() const noexcept {  return verstr.size() > 0 && verstr.first() != 0 ? verstr.begin() : "1.0"; }

    int   SetMethod( const char* s ) noexcept   {  method.SetLen( 0 );  return method.Append( w_strlen( s ) + 1, s );  }
    int   SetURI( const char* s ) noexcept      {  urlstr.SetLen( 0 );  return urlstr.Append( w_strlen( s ) + 1, s );  }
    int   SetVersion( const char* s ) noexcept  {  verstr.SetLen( 0 );  return verstr.Append( w_strlen( s ) + 1, s );  }

  public:     // serialization
    template <class S>  S*  Fetch( S* s ) noexcept;
    template <class O>  O*  Store( O* o ) noexcept;

  protected:  // helpers
    template <class S>  S*  Fetch( charstream<S>& ) noexcept;

  };

  class HTTPResponce: public HTTPHeader
  {
    array<char> version;
    array<char> comment;
    unsigned    hresult;

  public:     // construction
    HTTPResponce(): hresult( 200 )  {}

  public:     // fields
    const char* GetVersion() const noexcept {  return version.size() > 0 && version.first() != 0 ? version.begin() : "1.0"; }
    const char* GetComment() const noexcept {  return comment.size() > 0 ? comment.begin() : "";  }
    unsigned    GetStatus() const noexcept  {  return hresult;  }

    int   SetVersion( const char* s ) noexcept  {  version.SetLen( 0 );  return version.Append( w_strlen( s ) + 1, s );  }
    int   SetComment( const char* s ) noexcept  {  comment.SetLen( 0 );  return comment.Append( w_strlen( s ) + 1, s );  }
    void  SetStatus( unsigned u ) noexcept {  hresult = u;  }

  public:     // serialization
    template <class S>  S*  Fetch( S* ) noexcept;
    template <class O>  O*  Store( O* ) noexcept;

  protected:  // helpers
    template <class S>  S*  Fetch( charstream<S>& ) noexcept;

  };

  // HTTPHeader implementation

  inline
  int     HTTPHeader::Insert( const char* k, const char* v ) noexcept
  {
    headkey insert;
    int*    keypos;

    if ( (insert.val = w_strdup( v )) == nullptr )
      return ENOMEM;
    if ( keyset.Append( insert ) != 0 )
      return ENOMEM;
    if ( (keypos = keymap.AddKey( k )) == nullptr )
    {
      keyset.SetLen( keyset.size() - 1 );
      return ENOMEM;
    }
    keyset.last().key = keymap.KeyStr( k );
      *keypos = keyset.size() - 1;
    return 0;
  }

  inline
  const char* HTTPHeader::Search( const char* k ) const noexcept
  {
    const int*  keypos;

    if ( (keypos = keymap.Search( k )) == nullptr )
      return nullptr;
    return *keypos >= 0 && *keypos < keyset.size() ? keyset[*keypos].val : nullptr;
  }

  template <class S>
  S*    HTTPHeader::Fetch( S* s ) noexcept
  {
    charstream<S> stream( s );

    return Fetch( stream );
  }

  template <class O>
  O*    HTTPHeader::Store( O* o ) noexcept
  {
    return keyset.for_each( [&o]( const headkey& l )
      {
        o = ::Serialize(
            ::Serialize(
            ::Serialize(
            ::Serialize( o, l.key, w_strlen( l.key ) ), ": ", 2 ), (const char*)l.val, w_strlen( l.val ) ), "\r\n", 2 );

        return o != nullptr ? 0 : EFAULT;
      } ) == 0 ? ::Serialize( o, "\r\n", 2 ) : nullptr;
  }

  template <class S>
  S*    HTTPHeader::Fetch( charstream<S>& s ) noexcept
  {
    array<char> keystr;
    array<char> valstr;
    char        chnext;

    while ( (chnext = GetCh( s )) != '\0' && chnext != '\n' )
    {
      charstr thekey( keystr );
      charstr theval( valstr );

    // get next key
      while ( chnext != ':' && (byte_t)chnext > 0x20 )
        if ( thekey.addchr( chnext ) ) chnext = GetCh( s );
          else return nullptr;

    // check ':' delimiter; on mismatch, skip until '\n'
      if ( chnext != ':' || thekey.length() == 0 )
      {
        log_warning( EINVAL, "key-string followed by ':' expected white parsing the HTTP headers @" __FILE__ ":%u", __LINE__ );

        while ( chnext != '\0' && chnext != '\n' )
          chnext = GetCh( s );
        if ( chnext == '\0' ) return nullptr;
          else continue;
      }

    // begin get value after ' '
      while ( (chnext = GetCh( s )) == ' ' )
        (void)NULL;

      while ( chnext != '\0' && chnext != '\n' )
        if ( theval.addchr( chnext ) ) chnext = GetCh( s );
          else return nullptr;

      if ( Insert( keystr, valstr ) != 0 )
        return nullptr;
    }

    return chnext == '\n' ? s : nullptr;
  }

  template <class S>
  char  HTTPHeader::GetCh( charstream<S>& s ) noexcept
  {
    char  c;

    return (c = s.getnext()) == '\r' ? s.getnext() : c;
  }

  // HTTPRequest implementation

  template <class S>
  S*    HTTPRequest::Fetch( S* s ) noexcept
  {
    charstream<S> stream( s );

    return Fetch( stream );
  }

  template <class O>
  O*    HTTPRequest::Store( O* o ) noexcept
  {
    const char* metstr = method.size() > 0 && method.first() != 0 ? method : "GET";
    const char* pszurl = urlstr.size() > 0 && urlstr.first() != 0 ? urlstr : "/";
    const char* pszver = verstr.size() > 0 && verstr.first() != 0 ? verstr : "1.0";

    o = ::Serialize( ::Serialize( ::Serialize( ::Serialize( ::Serialize( o,
                    metstr, w_strlen( metstr ) ), ' ' ),
                    pszurl, w_strlen( pszurl ) ), " HTTP/", 6 ),
                    pszver, w_strlen( pszver ) );

    return (o = ::Serialize( o, "\r\n", 2 )) != nullptr ? HTTPHeader::Store( o ) : nullptr;
  }

  template <class S>
  S*    HTTPRequest::Fetch( charstream<S>& s ) noexcept
  {
    charstr     themth( method );
    charstr     theurl( urlstr );
    charstr     thever( verstr );
    char        chnext;

  // get method
    while ( (byte_t)(chnext = GetCh( s )) > 0x20 )
      if ( !themth.addchr( chnext ) )
        return nullptr;

    if ( method.size() == 0 || method.first() == '\0' || chnext != ' ' )
      return nullptr;

  // skip space
    do chnext = GetCh( s );
      while ( chnext == ' ' );

  // get url
    if ( (byte_t)chnext > 0x20 ) do {  if ( !theurl.addchr( chnext ) ) return nullptr;  }
      while ( (byte_t)(chnext = GetCh( s )) > 0x20 );

    if ( urlstr.size() == 0 || urlstr.first() == '\0' || chnext != ' ' )
      return nullptr;

  // skip space
    do chnext = GetCh( s );
      while ( chnext == ' ' );

  // check 'HTTP/'
    if ( chnext != 'H' || GetCh( s ) != 'T' || GetCh( s ) != 'T' || GetCh( s ) != 'P' || GetCh( s ) != '/' )
      return nullptr;

  // get version
    while ( (byte_t)(chnext = GetCh( s )) > 0x20 )
      if ( !thever.addchr( chnext ) )
        return nullptr;

  // check next char
    while ( chnext == ' ' )
      chnext = GetCh( s );

    return chnext == '\n' ? HTTPHeader::Fetch( s ) : nullptr;
  }

  // HTTPResponce implementation

  template <class S>
  S*    HTTPResponce::Fetch( S* s ) noexcept
  {
    charstream<S> stream( s );

    return Fetch( stream );
  }

  template <class O>
  O*    HTTPResponce::Store( O* o ) noexcept
  {
    const char* verstr = GetVersion();
    char        resstr[0x20];

    sprintf( resstr, "%d", hresult );

    o = ::Serialize( ::Serialize( ::Serialize( ::Serialize( o,
      "HTTP/", 5 ), verstr, w_strlen( verstr ) ), ' ' ),
                    resstr, w_strlen( resstr ) );

    if ( comment.size() > 0 && comment.first() != '\0' )
      o = ::Serialize( o, comment.begin(), w_strlen( comment ) );

    return (o = ::Serialize( o, "\r\n", 2 )) != nullptr ? HTTPHeader::Store( o ) : nullptr;
  }

  template <class S>
  S*    HTTPResponce::Fetch( charstream<S>& s ) noexcept
  {
    charstr     verstr( version );
    charstr     remstr( comment );

    char  chnext;

    if ( GetCh( s ) != 'H'
      || GetCh( s ) != 'T'
      || GetCh( s ) != 'T'
      || GetCh( s ) != 'P'
      || GetCh( s ) != '/' ) return nullptr;

    while ( (byte_t)(chnext = GetCh( s )) > 0x20 )
      if ( !verstr.addchr( chnext ) )
        return nullptr;

    if ( chnext == ' ' )  while ( (chnext = GetCh( s )) == ' ') (void)NULL;
      else return nullptr;

    if ( chnext < '0' || chnext > '9' )
      return nullptr;

  // get http result
    for ( hresult = chnext - '0'; (chnext = GetCh( s )) >= '0' && chnext <= '9'; hresult = hresult * 10 + chnext - '0' )
      (void)NULL;

  // skip until comment
    while ( chnext != '\0' && chnext != '\n' && (byte_t)chnext <= 0x20 )
      chnext = GetCh( s );

  // get comment string
    if ( chnext > 0x20 )
      do { if ( !remstr.addchr( chnext ) ) return nullptr; }
        while ( (chnext = GetCh( s )) != '\0' && chnext != '\n' );
       
    return chnext != '\0' ? HTTPHeader::Fetch( s ) : nullptr;
  }

}

# endif // __mtc_http_h__
