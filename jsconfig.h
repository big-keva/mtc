# if !defined( __mtc_jsconfig_h__ )
# define __mtc_jsconfig_h__
# include "zarray.h"
# include "cstr.h"

namespace mtc
{

  class configuration: public zarray<>
  {
    cstr  szpath;

    friend configuration OpenConfig( const char* );

  public:     // construction
    configuration(): zarray<>() {}
    configuration( const configuration& c ): zarray<>( c ), szpath( c.szpath ) {}
    configuration( const zarray<>& z ): zarray<>( z ) {}
    configuration& operator = ( const configuration& c )
      {
        zarray<>::operator = ( c );
        szpath = c.szpath;
        return *this;
      }
    operator bool () const  {  return szpath != nullptr;  }

  public:     // expand path
    const char*   ExpandPath( char* absolutePath, unsigned cchbuf, const char* relativePath ) const;
    cstr          ExpandPath( const char* relativePath ) const;

  protected:  // initialization
    int           SetCfgPath( const char* psz );

  };

  configuration OpenConfig( const char* szpath );

}

# endif  // __mtc_jsconfig_h__
