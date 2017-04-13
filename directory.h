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
# if !defined( __mtc_dir_h__ )
# define __mtc_dir_h__
# include "platform.h"
# include "autoptr.h"
# include "wcsstr.h"
# include <atomic>

# if defined( _WIN32 )
#   include <io.h>
# else
#   include <dirent.h>
#   include <fnmatch.h>
# endif

namespace mtc
{

  class directory
  {
    class dir_str
    {
      char*   strptr;

    public:     // implementation
      dir_str( char* s = nullptr );
      dir_str( const dir_str& );
     ~dir_str();
      dir_str& operator = ( const dir_str& );
      dir_str& operator = ( char* );
      operator const char* () const   {  return strptr;  }

    public:
      static char*  strdup( const char*, size_t l = -1 );

    private:
      std::atomic_int*  base();

    };

  public:     // constants
    enum
    {
      attr_dir  = 0x00000001,
      attr_file = 0x00000002,
      attr_any  = 0x00000003
    };

    class direntry
    {
      friend class directory;

      dir_str     szfold;
      dir_str     szname;
      unsigned    uattrs;

    protected:  // internal initialization
      direntry( dir_str& dir, char* str, unsigned att ): szfold( dir ), szname( str ), uattrs( att )
        {}

    public:     // initialization
      direntry(): uattrs( 0 )
        {}
      direntry( const direntry& d ): szfold( d.szfold ), szname( d.szname ), uattrs( d.uattrs )
        {}
      direntry& operator = ( const direntry& d )
        {
          szfold = d.szfold;
          szname = d.szname;
          uattrs = d.uattrs;
          return *this;
        }

    public:     // name access and attributes
      const char* folder() const  {  return szfold;  }
      const char* string() const  {  return szname;  }
      unsigned    attrib() const  {  return uattrs;  }

      operator bool () const          {  return defined();  }
      bool  defined() const           {  return szname != nullptr;  }

    };

  protected:
    struct dir_val
    {
      std::atomic_int refcnt;
      unsigned        dwattr;     // directory::attr_xxx virtual attribute combine
      char*           szname;     // pointer to string in system-specific struct
      dir_str         folder;     // current scanned folder

# if defined( _WIN32 )
      struct  _finddata_t fidata;
      intptr_t            handle;

    public:     // construction
      dir_val( unsigned attr ): refcnt( 1 ), szname( nullptr ), dwattr( attr ), handle( -1 )
        {
        }
      
    public:     // read
      unsigned    attrib() const  {  return (fidata.attrib & _A_SUBDIR) ? attr_dir : attr_file;  }
      char*       doread()        {  return szname = (_findnext( handle, &fidata ) == 0 ? fidata.name : nullptr);  }
# else
      DIR*                dirptr;
      struct dirent*      pentry;
      _auto_<char>        filter;

    public:     // construction
      dir_val( unsigned attr ): refcnt( 1 ), szname( nullptr ), dwattr( attr ), dirptr( nullptr ), pentry( nullptr )
        {
        }
     ~dir_val()
        {
          if ( dirptr != nullptr )
            closedir( dirptr );
        }

    public:     // read
      unsigned    attrib() const
        {
          return pentry != nullptr ? pentry->d_type == DT_DIR ? attr_dir : attr_file : 0;
        }
      char*       doread()
        {
          for ( szname = nullptr; szname == nullptr && dirptr != nullptr && (pentry = readdir( dirptr )) != nullptr; )
            if ( *(char*)filter == '\0' || fnmatch( filter, pentry->d_name, FNM_NOESCAPE | FNM_PATHNAME ) == 0 )
              szname = pentry->d_name;
          return szname;
        }
# endif   // WIN32

    };

    dir_val*  didata;

  public:     // construction
    directory(): didata( nullptr )
      {}
    directory( const directory& d )
      {
        if ( (didata = d.didata) != nullptr )
          ++didata->refcnt;
      }
   ~directory()
      {
        if ( didata != nullptr && --didata->refcnt == 0 )
          delete didata;
      }
    directory& operator = ( const directory& d )
      {
        if ( didata != nullptr && --didata->refcnt == 0 )
          delete didata;
        if ( (didata = d.didata) != nullptr )
          ++didata->refcnt;
        return *this;
      }

    bool  defined() const   {  return didata != nullptr;  }
    operator bool () const  {  return defined();  }

  public:     // recursive directory scanner
    direntry  Get();

  public:     // lambda
    template <class _do_>
    int   for_each( _do_ action )
      {
        direntry  d;
        int       e;

        while ( (d = Get()).defined() && (e = action( d )) == 0 )
          (void)NULL;
        return e;
      }

  public:     // create class
    static  directory Open( const char* pszdir, unsigned uflags = attr_any );

  };

  // directory::dir_str implementation

  inline
  directory::dir_str::dir_str( char* s )
    {
      if ( (strptr = s) != nullptr )
        ++*base();
    }

  inline
  directory::dir_str::dir_str( const dir_str& s )
    {
      if ( (strptr = s.strptr) != nullptr )
        ++*base();
    }

  inline
  directory::dir_str::~dir_str()
    {
      if ( strptr != nullptr && --*base() <= 0 )
        delete base();
    }

  inline
  directory::dir_str& directory::dir_str::operator = ( const dir_str& s )
    {
      if ( strptr != nullptr && --*base() <= 0 )
        delete base();
      if ( (strptr = s.strptr) != nullptr )
        ++*base();
      return *this;
    }

  inline
  directory::dir_str& directory::dir_str::operator = ( char* s )
    {
      if ( strptr != nullptr && --*base() <= 0 )
        delete base();
      if ( (strptr = s) != nullptr )
        ++*base();
      return *this;
    }

  inline
  std::atomic_int*  directory::dir_str::base()
    {
      return strptr != nullptr ? -1 + (std::atomic_int*)strptr : nullptr;
    }

  inline
  char*   directory::dir_str::strdup( const char* s, size_t l )
    {
      std::atomic_int*  palloc;

      if ( l == (size_t)-1 )
        l = w_strlen( s );

      if ( (palloc = (std::atomic_int*)malloc( sizeof(std::atomic_int) + l + 1 )) != nullptr )
        new( palloc++ ) std::atomic_int( 0 );
      else return nullptr;

      *(l + (char*)memcpy( palloc, s, l )) = '\0';
        return (char*)palloc;
    }

  // directory implementation

  inline
  directory::direntry directory::Get()
  {
  // check if deallocated
    if ( didata == nullptr )
      return direntry();

    if ( didata->szname == nullptr )
      while ( didata->doread() != nullptr && (didata->attrib() & didata->dwattr) == 0 )
        (void)NULL;
        
    if ( didata->szname != nullptr )
    {
      direntry  output( didata->folder, dir_str::strdup( didata->szname ), didata->attrib() );
        didata->szname = nullptr;
      return output;
    }
    return direntry();
  }

  inline
  directory directory::Open( const char* pszdir, unsigned uflags )
  {
    directory   thedir;
    char*       folder;
    char*       endptr;

  // allocate directory object
    if ( (thedir.didata = allocate<directory::dir_val>( uflags )) == nullptr )
      return directory();

# if defined( _WIN32 )
    char      altdir[1024];

  // check if a direct name is provided, else if the template has the wild card
    if ( (thedir.didata->handle = _findfirst( pszdir, &thedir.didata->fidata )) == -1 )
    {
      if ( strchr( pszdir, '*' ) != nullptr || strchr( pszdir, '?' ) != nullptr )
        return directory();

      if ( *pszdir == '\0' )  pszdir = "./*";
        else
      if ( pszdir[strlen( pszdir ) - 1] != '/' && pszdir[strlen( pszdir ) - 1] != '\\' )
        pszdir = strcat( strcpy( altdir, pszdir ), "/*" );
      else
        pszdir = strcat( strcpy( altdir, pszdir ), "*" );

      if ( (thedir.didata->handle = _findfirst( pszdir, &thedir.didata->fidata )) == -1 ) return directory();
        else thedir.didata->szname = thedir.didata->fidata.name;
    } else thedir.didata->szname = thedir.didata->fidata.name;

    while ( (thedir.didata->attrib() & uflags) == 0 )
      if ( thedir.didata->doread() == nullptr )
        return directory();

  // create folder string
    if ( (thedir.didata->folder = folder = dir_str::strdup( pszdir )) == nullptr )
      return directory();

    for ( endptr = folder; *endptr != 0; ++endptr )
      (void)NULL;

    while ( endptr >= folder && *endptr != '/' && *endptr != '\\' )
      *endptr-- = 0;
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

    return thedir;
  }

}

# endif  // __mtc_dir_h__
