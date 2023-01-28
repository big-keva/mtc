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
# include "dir.hpp"
# include "utf.hpp"
# include <atomic>
# include "wcsstr.h"

# if defined( _WIN32 )
#   include <io.h>
# else
#   include <dirent.h>
#   include <fnmatch.h>
# endif

namespace mtc {
namespace fs {

  class directory::string::inner_t
  {
    friend class string;

    std::atomic_int refcnt;
    size_t          cchstr;

  public:     // access
    auto  wstr() const -> const widechar* {  return (const widechar*)(this + 1);  }
    auto  wstr()       ->       widechar* {  return       (widechar*)(this + 1);  }
    auto  cstr() const -> const     char* {  return (const char*)(wstr() + cchstr + 1);  }
    auto  cstr()       ->           char* {  return       (char*)(wstr() + cchstr + 1);  }

  public:     // creation
    template <class chartype>
    static  auto  strlen( const chartype*, size_t ) -> size_t;
    static  auto  create( const widechar*, size_t = (size_t)-1 ) -> inner_t*;
    static  auto  create( const     char*, size_t = (size_t)-1 ) -> inner_t*;

  private:
    inner_t( const widechar* s, size_t l );
    inner_t( const char* s, size_t l );
  };

  class directory::inner_t
  {
    friend class directory;

    std::atomic_int refcnt;
    unsigned        u_attr;       // directory::attr_xxx virtual attribute combine
    string          w_path;       // current scanned folder

# if defined( _WIN32 )
    widechar*             p_name; // pointer to string in system-specific struct
    struct  _wfinddata_t  fidata;
    intptr_t              handle;

  public:     // construction
    inner_t( unsigned attr ): refcnt( 1 ), u_attr( attr ), handle( -1 )
      {
      }
      
  public:     // read
    auto  attr() const -> unsigned  {  return (fidata.attrib & _A_SUBDIR) ? attr_dir : attr_file;  }
    auto  read()       -> widechar* {  return p_name = (_wfindnext( handle, &fidata ) == 0 ? fidata.name : nullptr);  }

# else
    char*           p_name; // pointer to string in system-specific struct
    DIR*            dirptr;
    struct dirent*  pentry;
    std::string     filter;

  public:     // construction
    inner_t( unsigned attr ): refcnt( 1 ), u_attr( attr ), p_name( nullptr ), dirptr( nullptr ), pentry( nullptr )
      {
      }
   ~inner_t()
      {
        if ( dirptr != nullptr )
          closedir( dirptr );
      }

  public:     // read
    unsigned    attr() const
      {
        return pentry != nullptr ? pentry->d_type == DT_DIR ? attr_dir : attr_file : 0;
      }
    char*       read()
      {
        for ( p_name = nullptr; p_name == nullptr && dirptr != nullptr && (pentry = readdir( dirptr )) != nullptr; )
          if ( filter.length() == 0 || fnmatch( filter.c_str(), pentry->d_name, FNM_NOESCAPE | FNM_PATHNAME ) == 0 )
            p_name = pentry->d_name;
        return p_name;
      }
# endif   // WIN32
  };

  // directory::string::inner_t implementation

  inline
  directory::string::inner_t::inner_t( const widechar* s, size_t l ): refcnt( 0 ), cchstr( l )
  {
    auto  pwsstr = wstr();
    auto  pwsend = pwsstr + cchstr + 1;

    if ( s != nullptr )
      while ( l-- != 0 )  *pwsstr++ = *s++;

    while ( pwsstr != pwsend )
      *pwsstr++ = 0;

    utf8::encode( cstr(), cchstr * 6, wstr(), cchstr );
  }

  inline
  directory::string::inner_t::inner_t( const char* s, size_t l ): refcnt( 0 ), cchstr( l )
  {
    utf16::encode( wstr(), cchstr + 1, strcpy( cstr(), s ) );
  }

  template <class chartype> inline
  auto  directory::string::inner_t::strlen( const chartype* str, size_t cch ) -> size_t
  {
    if ( cch == (size_t)-1 )
    {
      if ( str != nullptr ) for ( cch = 0; str[cch] != 0; ++cch ) (void)NULL;
        else cch = 0;
    }

    return cch;
  }

  inline
  auto  directory::string::inner_t::create( const widechar* str, size_t cch ) -> inner_t*
  {
    auto    ccwstr = strlen( str, cch );
    auto    cccstr = ccwstr * 6;
    size_t  nalloc = sizeof(inner_t) + (ccwstr + 1) * sizeof(widechar) + (cccstr + 1) * sizeof(char);

    return new( new char[nalloc] ) inner_t( str, ccwstr );
  }

  inline
  auto  directory::string::inner_t::create( const char* str, size_t cch ) -> inner_t*
  {
    auto    cccstr = strlen( str, cch );
    auto    ccwstr = utf::strlen( utf16(), str, cccstr );
    size_t  nalloc = sizeof(inner_t) + (ccwstr + 1) * sizeof(widechar) + (cccstr + 1) * sizeof(char);

    return new( new char[nalloc] ) inner_t( str, ccwstr );
  }

  // directory::string implementation

  widechar  directory::string::zero = 0;

  directory::string::string( const widechar* pws, size_t cch )
  {
    ++(data = inner_t::create( pws, cch ))->refcnt;
  }

  directory::string::string( const char* psz, size_t cch )
  {
    ++(data = inner_t::create( psz, cch ))->refcnt;
  }

  directory::string::string(): data( nullptr )
  {
  }

  directory::string::string( const string& s )
  {
    if ( (data = s.data) != nullptr )
      ++data->refcnt;
  }

  directory::string::~string()
  {
    if ( data != nullptr && --data->refcnt == 0 )
      delete data;
  }

  auto  directory::string::operator = ( const string& s ) -> string&
  {
    if ( this != &s )
    {
      if ( data != nullptr && --data->refcnt == 0 )
        delete data;
      if ( (data = s.data) != nullptr )
        ++data->refcnt;
    }
    return *this;
  }

  auto  directory::string::charstr() const -> const char*
  {
    return data != nullptr ? data->cstr() : "";
  }

  auto  directory::string::widestr() const -> const widechar*
  {
    return data != nullptr ? data->wstr() : &zero;
  }

  // directory::entry implementation

  directory::entry::entry(): u_attr( 0 )
  {
  }

  directory::entry::entry( const entry& d ):
    w_path( d.w_path ),
    w_name( d.w_name ),
    u_attr( d.u_attr )
  {
  }

  directory::entry::entry( const string& _path, const string& _name, unsigned _attr ):
    w_path( _path ),
    w_name( _name ),
    u_attr( _attr )
  {
  }

  // directory::directory implementation

  directory::directory(): data( nullptr )
  {
  }

  directory::directory( directory&& dir )
  {
    if ( (data = dir.data) != nullptr )
      dir.data = nullptr;
  }

  directory::directory( const directory& dir )
  {
    if ( (data = dir.data) != nullptr )
      ++data->refcnt;
  }

  directory::~directory()
  {
    if ( data != nullptr && --data->refcnt == 0 )
      delete data;
  }

  auto  directory::operator = ( directory&& dir ) -> directory&
  {
    if ( this != &dir )
    {
      if ( data != nullptr && --data->refcnt == 0 )
        delete data;
      data = dir.data;  dir.data = nullptr;
    }
    return *this;
  }
    
  auto  directory::operator = ( const directory& dir ) -> directory&
  {
    if ( data != nullptr && --data->refcnt == 0 )
      delete data;
    if ( (data = dir.data) != nullptr )
      ++data->refcnt;
    return *this;
  }

  auto  directory::get() -> entry
  {
  // check if deallocated
    if ( data == nullptr )
      return entry();

    if ( data->p_name == nullptr )
      data->read();

    while ( data->p_name != nullptr && (data->attr() & data->u_attr) == 0 )
      data->read();
        
    if ( data->p_name != nullptr )
    {
      entry   out( data->w_path, string( data->p_name ), data->attr() );
        data->p_name = nullptr;
      return out;
    }
    return entry();
  }

# if defined( _WIN32 )

  auto  directory::open( const char* dir, unsigned uflags ) -> directory
  {
    widechar  wcs[1024];

    if ( utf16::encode( wcs, sizeof(wcs) / sizeof(wcs[0]), dir ) == (size_t)-1 )
      return directory();
    return std::move( open( wcs, uflags ) );
  }

  auto  directory::open( const widechar* pwsdir, unsigned uflags ) -> directory
  {
    directory   thedir;
    widechar    altdir[1024];
    widechar*   folder;
    widechar*   endptr;

    if ( pwsdir == nullptr )
      return directory();

  // allocate directory object
    thedir.data = new inner_t( uflags );

  // check if a direct name is provided, else if the template has the wild card
    if ( (thedir.data->handle = _wfindfirst( pwsdir = w_strcpy( altdir, pwsdir ), &thedir.data->fidata )) == -1 )
    {
      const widechar  wszany[] = { '.', '/', '*', 0 };

      if ( w_strchr( pwsdir, '*' ) != nullptr || w_strchr( pwsdir, '?' ) != nullptr )
        return directory();

      if ( *pwsdir == 0 ) pwsdir = wszany;
        else
      if ( pwsdir[w_strlen( pwsdir ) - 1] != '/' && pwsdir[w_strlen( pwsdir ) - 1] != '\\' )
        pwsdir = w_strcat( altdir, wszany + 1 );
      else
        pwsdir = w_strcat( altdir, wszany + 2 );

      if ( (thedir.data->handle = _wfindfirst( pwsdir, &thedir.data->fidata )) == -1 ) return directory();
        else thedir.data->p_name = thedir.data->fidata.name;
    } else thedir.data->p_name = thedir.data->fidata.name;

    while ( thedir.data->p_name != nullptr && (thedir.data->attr() & uflags) == 0 )
      thedir.data->read();

    if ( thedir.data->p_name == nullptr )
      return directory();

  // create folder string
    for ( endptr = folder = altdir; *endptr != 0; ++endptr )
      (void)NULL;

    while ( endptr >= folder && *endptr != '/' && *endptr != '\\' )
      *endptr-- = 0;

    thedir.data->w_path = string( altdir );

    return std::move( thedir );
  }

# else

  auto  directory::open( const char* dir, unsigned uflags ) -> directory
  {
    directory   thedir;
    const char* endptr;

    if ( dir == nullptr )
      return directory();

  // allocate directory object
    thedir.data = new inner_t( uflags );

  // check if has directory delimiter; if null, create alternate base folder name as ./
    if ( (endptr = (char*)strrchr( dir, '/' )) == nullptr )
    {
      thedir.data->w_path = string( "./" );
      endptr = dir;
    }
      else
    thedir.data->w_path = string( dir, ++endptr - dir );

    thedir.data->filter = string( endptr );

  // parse the search entry to directory and the mask
    if ( (thedir.data->dirptr = opendir( thedir.data->w_path.charstr() )) == nullptr )
      return directory();

    return thedir;
  }

  auto  directory::open( const widechar* dir, unsigned uflags ) -> directory
  {
    char  mbs[1024];

    if ( utf8::encode( mbs, sizeof(mbs) / sizeof(mbs[0]), dir ) == (size_t)-1 )
      return directory();
    return open( dir, uflags );
  }

# endif   // !_MSC_VER

}}
