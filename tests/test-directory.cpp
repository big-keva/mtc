# include "../utf.hpp"
# include <cstddef>

namespace mtc {
namespace fs {

  class directory
  {
    class inner_t;
    class string;

  public:     // constants
    enum
    {
      attr_dir  = 0x00000001,
      attr_file = 0x00000002,
      attr_any  = 0x00000003
    };

    class entry;

    inner_t*  data;

  public:     // construction
    directory();
    directory( directory&& );
    directory( const directory& );
   ~directory();
    auto  operator = ( directory&& ) -> directory&;
    auto  operator = ( const directory& ) -> directory&;

  public:     // open
    static  auto  open( const char*     dir, unsigned attrib = attr_any ) -> directory;
    static  auto  open( const widechar* dir, unsigned attrib = attr_any ) -> directory;

  public:     // get
    auto  get() -> entry;

  public:     // initialized
    operator bool() const  {  return defined();  }
    bool  defined() const  {  return data != nullptr;  }

  };

  class directory::string
  {
    friend class directory;
    class inner_t;

    static  widechar  zero;
    inner_t*          data;

  protected:  // construction
    string( const widechar*, size_t = (size_t)-1 );
    string( const char*, size_t = (size_t)-1 );
    string();

  public:     // construction
    string( const string& );
   ~string();
    auto  operator = ( const string& ) -> string&;
    auto  operator == ( const nullptr_t ) const {  return data == nullptr;  }
    auto  operator != ( const nullptr_t ) const {  return data != nullptr;  }

  public:     // access
    operator const char*     () const {  return charstr();  }
    operator const widechar* () const {  return widestr();  }

    auto  charstr() const -> const char*;
    auto  widestr() const -> const widechar*;

  };

  class directory::entry
  {
    friend class directory;

  protected:
    entry( const string& _path, const string& _name, unsigned _attr );

  public:     // initialization
    entry();
    entry( const entry& );

    operator bool () const  {  return defined();  }
    bool   defined() const  {  return w_name != nullptr;  }

  public:
    auto  path() const -> const string& {  return w_path;  }
    auto  name() const -> const string& {  return w_name;  }
    auto  attr() const -> unsigned      {  return u_attr;  }

  protected:
    string    w_path;
    string    w_name;
    unsigned  u_attr;

  };

}}

# include <atomic>
# include "../wcsstr.h"

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
    static  auto  strlen( const widechar* wcsstr, size_t cchstr ) -> size_t;
    static  auto  create( const widechar* wcsstr, size_t cchstr = (size_t)-1 ) -> inner_t*;

  private:
    inner_t( const widechar* s, size_t l );
  };

  class directory::inner_t
  {
    friend class directory;

    std::atomic_int refcnt;
    unsigned        u_attr;     // directory::attr_xxx virtual attribute combine
    widechar*       p_name;     // pointer to string in system-specific struct
    string          w_path;     // current scanned folder

# if defined( _WIN32 )
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
      DIR*                dirptr;
      struct dirent*      pentry;
      _auto_<char>        filter;

    public:     // construction
      dir_val( unsigned attr ): refcnt( 1 ), dwattr( attr ), szname( nullptr ), dirptr( nullptr ), pentry( nullptr )
        {
        }
     ~dir_val()
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
          for ( szname = nullptr; szname == nullptr && dirptr != nullptr && (pentry = readdir( dirptr )) != nullptr; )
            if ( *(char*)filter == '\0' || fnmatch( filter, pentry->d_name, FNM_NOESCAPE | FNM_PATHNAME ) == 0 )
              szname = pentry->d_name;
          return szname;
        }
# endif   // WIN32
  };

  // directory::string::inner_t implementation

  inline
  auto  directory::string::inner_t::strlen( const widechar* wcsstr, size_t cchstr ) -> size_t
  {
    if ( cchstr == (size_t)-1 )
    {
      if ( wcsstr == nullptr )
        return 0;

      for ( cchstr = 0; wcsstr[cchstr] != 0; ++cchstr )
        (void)NULL;
    }

    return cchstr;
  }

  inline
  auto  directory::string::inner_t::create( const widechar* wcsstr, size_t cchstr ) -> inner_t*
  {
    auto    ccwstr = strlen( wcsstr, cchstr );
    auto    cccstr = utf::cbchar( wcsstr, ccwstr );
    size_t  nalloc = sizeof(inner_t) + (ccwstr + 1) * sizeof(widechar) + (cccstr + 1) * sizeof(char);

    return new( new char[nalloc] ) inner_t( wcsstr, ccwstr );
  }

  inline
  directory::string::inner_t::inner_t( const widechar* s, size_t l ): refcnt( 0 ), cchstr( l )
  {
    auto  pwsstr = wstr();
    auto  pwsend = pwsstr + cchstr + 1;
    auto  pchstr = cstr();

    if ( s != nullptr )
      while ( l-- != 0 )  *pwsstr++ = *s++;

    while ( pwsstr != pwsend )
      *pwsstr++ = 0;

    utf::encode( cstr(), cchstr + 1, wstr(), cchstr );
  }

  // directory::string implementation

  widechar  directory::string::zero = 0;

  directory::string::string( const widechar* pws, size_t cch )
  {
    ++(data = inner_t::create( pws, cch ))->refcnt;
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
    if ( data != nullptr && --data->refcnt == 0 )
      delete data;
    if ( (data = s.data) != nullptr )
      ++data->refcnt;
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
    if ( data != nullptr && --data->refcnt == 0 )
      delete data;
    data = dir.data;  dir.data = nullptr;
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
      return std::move( out );
    }
    return entry();
  }

  auto  directory::open( const char* dir, unsigned uflags ) -> directory
  {
    widechar  wcs[1024];

    if ( utf::decode( wcs, sizeof(wcs) / sizeof(wcs[0]), dir ) == (size_t)-1 )
      return directory();
    return std::move( open( wcs, uflags ) );
  }

  auto  directory::open( const widechar* pwsdir, unsigned uflags ) -> directory
  {
    directory   thedir;

    if ( pwsdir == nullptr )
      return directory();

  // allocate directory object
    thedir.data = new inner_t( uflags );

# if defined( _WIN32 )
    widechar  altdir[1024];
    widechar* folder;
    widechar* endptr;

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
# else
  // check if has directory delimiter
    if ( (endptr = (char*)strrchr( pszdir, '/' )) == nullptr )
    {
      if ( (thedir.didata->folder = folder = dir_str::strdup( "./" )) == nullptr )  return directory();
        else endptr = (char*)pszdir;
    }
      else
  // create directory name and mask
    if ( (thedir.didata->folder = folder = dir_str::strdup( pszdir )) == nullptr )  return directory();
      else folder[endptr++ - pszdir + 1] = '\0';

  // create the mask
    if ( (thedir.didata->filter = w_strdup( endptr )) == nullptr )
      return directory();

  // parse the search entry to directory and the mask
    if ( (thedir.didata->dirptr = opendir( folder )) == nullptr )
      return directory();
# endif  // _MSC_VER

    return std::move( thedir );
  }

}}

int   main()
{
  auto  dir = mtc::fs::directory::open( "../", mtc::fs::directory::attr_any );

  for ( auto ent = dir.get(); ent; ent = dir.get() )
    fprintf( stdout, "%s%s\n", ent.path().charstr(), ent.name().charstr() );
  {
  }
  return 0;
}
