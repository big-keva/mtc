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
# if !defined( __mtc_dir_h__ )
# define __mtc_dir_h__
# include "platform.h"

# if defined( _MSC_VER )
#   include <io.h>
# endif

namespace mtc
{

  template <class M = def_alloc<>>
  class directory
  {
    struct directory_data
    {
      int   refcnt;
      int*  dirstr;

    public:     // construction
      directory_data():
        refcnt( 1 ), dirstr( nullptr ) {}
     ~directory_data()
        {
          if ( dirstr != nullptr && --*dirstr )
            M().free( dirstr );
        }

    public:     // real
# if defined( _MSC_VER )
      struct  _finddata_t fidata;
      intptr_t            handle;
      unsigned            dwattr;

    public:     // reading
      bool  Next()
        {
        // free allocated string
          if ( dirstr != nullptr && --*dirstr == 0 )
            M().free( dirstr );
    
          for ( dirstr = nullptr; _findnext( handle, &fidata ) == 0; )
            if ( fidata.attrib & dwattr )
              if ( (dirstr = (int*)M().alloc( sizeof(int) + strlen( fidata.name ) + 1 )) != nullptr )
              {
                strcpy( (char*)(dirstr + 1), fidata.name );  *dirstr = 1;
                return true;
              }

          return false;
        }
# else
#   error Undefined directory<> class implementation!
# endif  // _MSC_VER
    };

    directory_data* didata;

  public:     // interface class
    class direntry
    {
      friend class directory;

      int*        szname;
      unsigned    attrib;

    protected:  // internal initialization
      direntry( int* s, unsigned a ): attrib( a )
        {
          if ( (szname = s) != nullptr )
            ++*szname;
        }

    public:     // initialization
      direntry(): szname( nullptr ), attrib( 0 )
        {
        }
      direntry( const direntry& d ): attrib( d.attrib )
        {
          if ( (szname = d.szname) != nullptr )
            ++*szname;
        }
     ~direntry()
        {
          if ( szname != nullptr && --*szname == 0 )
            M().free( szname );
        }
      direntry& operator = ( const direntry& d )
        {
          if ( szname != nullptr && --*szname == 0 )
            M().free( szname );
          if ( (szname = d.szname) != nullptr )
            ++*szname;
          attrib = d.attrib;
            return *this;
        }

    public:     // name access and attributes
      const char*     getname() const
        {  return szname != nullptr ? (char*)(szname + 1) : nullptr;  }
      const unsigned  getattr() const
        {  return attrib;  }

      operator const char* () const
        {  return getname();  }
      operator bool () const
        {  return szname != nullptr;  }
    };

  public:     // constants
    enum
    {
      attr_dir  = 0x00000001,
      attr_file = 0x00000002,
      attr_any  = 0x00000003
    };

  public:     // construction
    directory(): didata( nullptr )
      {
      }
    directory( const directory& d )
      {
        if ( (didata = d.didata) != nullptr )
          ++didata->refcnt;
      }
   ~directory()
      {
        if ( didata != nullptr && --didata->refcnt == 0 )
          M().deallocate( didata );
      }
    directory& operator = ( const directory& d )
      {
        if ( didata != nullptr && --didata->refcnt == 0 )
          M().deallocate( didata );
        if ( (didata = d.didata) != nullptr )
          ++didata->refcnt;
        return *this;
      }

    operator bool () const
      {
        return didata != nullptr;
      }

  public:     // recursive directory scanner
    direntry  Get();

  public:     // lambda
    template <class _do_> int   for_each( _do_ action )
      {
        direntry  d;
        int       e;

        while ( (en = Get()) != false && (e = action( d )) == 0 )
          (void)NULL;
        return e;
      }

  public:     // create class
    static  directory Open( const char* pszdir, unsigned uflags = attr_any );

  };

  template <class M>
  typename directory<M>::direntry  directory<M>::Get()
  {
    direntry  output;

  // check if nothing to get now
    if ( didata != nullptr && didata->dirstr == nullptr && --didata->refcnt == 0 )
      {  M().deallocate( didata );  didata = nullptr;  }

  // check if deallocated
    if ( didata == nullptr )
      return direntry();

# if defined( _MSC_VER )
    output = direntry( didata->dirstr, didata->fidata.attrib & _A_SUBDIR ? attr_dir : attr_file );
      didata->Next();
    return output;
# else
#   error Undefined directory<> class implementation!
# endif  // _MSC_VER
  }

  template <class M>
  directory<M>  directory<M>::Open( const char* pszdir, unsigned uflags )
  {
    directory<M>  thedir;

  // allocate directory object
    if ( (thedir.didata = M().allocate<directory<M>::directory_data>()) == nullptr )
      return thedir;

# if defined( _MSC_VER )
    char      altdir[1024];

    if ( uflags & attr_dir )  thedir.didata->dwattr = _A_SUBDIR;
      else thedir.didata->dwattr = 0;
    if ( uflags & attr_file )
      thedir.didata->dwattr |= ~_A_SUBDIR;
    
  // check if a direct name is provided, else if the template has the wild card
    if ( (thedir.didata->handle = _findfirst( pszdir, &thedir.didata->fidata )) == -1 )
    {
      if ( strchr( pszdir, '*' ) == nullptr && strchr( pszdir, '?' ) == nullptr )
      {
        if ( *pszdir == '\0' )  pszdir = "./*";
          else  pszdir = strcat( strcpy( altdir, pszdir ), "/*" );
      }
      if ( (thedir.didata->handle = _findfirst( pszdir, &thedir.didata->fidata )) == -1 )
        return directory<M>();
    }

    return thedir.didata->Next() ? thedir : directory<M>();
# else
#   error Undefined directory<> class implementation!
# endif  // _MSC_VER
  }

}

# endif  // __mtc_dir_h__
