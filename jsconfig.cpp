# include "jsconfig.h"
# include "jsonTools.h"
# include "file.h"
# if defined( _WIN32 )
#   if defined( _MSC_VER )
#     include <direct.h>
#   endif
# else
#   include <unistd.h>
# endif  // _MSC_VER

namespace mtc
{

  const char* configuration::ExpandPath( char* psz, unsigned cch, const char* str ) const
  {
    char  orgdir[0x400];
    char  curdir[0x400];
    char  getdir[0x400];
    char  newdir[0x400];
    char* pslash;
  
    if ( str == nullptr )
      return nullptr;

  // get config dir
    if ( szpath != nullptr )
    {
      for ( pslash = strcpy( orgdir, szpath ) + szpath.length(); pslash > orgdir
        && *pslash != '/' && *pslash != '\\'; --pslash ) ;
      if ( pslash == orgdir ) strcpy( orgdir, "." );
        else *pslash = '\0';
    } else strcpy( orgdir, "." );

  // get argument dir
    for ( pslash = strcpy( getdir, str ) + strlen( str ); pslash > getdir
      && *pslash != '/' && *pslash != '\\'; --pslash ) ;
    *pslash = '\0';

  // save current 
    getcwd( curdir, sizeof(curdir) );
    chdir ( orgdir );
    chdir ( getdir );
    getcwd( newdir, sizeof(newdir) );
    chdir ( curdir );

    for ( pslash = (char*)str + strlen( str ); pslash > str
      && *pslash != '/' && *pslash != '\\'; --pslash ) ;
    while ( *pslash == '/' || *pslash == '\\' ) ++pslash;
  
    return strlen( strcat( strcat( newdir, "/" ), pslash ) ) >= cch ? nullptr : strcpy( psz, newdir );
  }

  cstr  configuration::ExpandPath( const char* relativePath ) const
  {
    char  szpath[0x400];
    cstr  thestr;

    return ExpandPath( szpath, sizeof(szpath), relativePath ) != nullptr ?
      w_strcpy( thestr, szpath ) : thestr;
  }

  int         configuration::SetCfgPath( const char* psz )
  {
    char  curdir[0x400];
    char  newdir[0x400];
    char* pslash;
  
    for ( pslash = strcpy( newdir, psz ) + strlen( psz ); pslash > newdir
      && *pslash != '/' && *pslash != '\\'; --pslash ) ;
    *pslash = 0;

    getcwd( curdir, sizeof(curdir) );
    chdir ( newdir );
    getcwd( newdir, sizeof(newdir) );
    chdir ( curdir );

    for ( pslash = (char*)psz + strlen( psz ); pslash > psz
      && pslash[-1] != '/' && pslash[-1] != '\\'; --pslash ) ;
  
    return w_strcat( w_strcat( w_strcpy( szpath, newdir ), "/" ), pslash ) != nullptr ? 0 : ENOMEM;
  }

  configuration OpenConfig( const char* szpath )
  {
    configuration config;
    file          infile;

    if ( (infile = fopen( szpath, "rb" )) == nullptr )
      return config;

    if ( ParseJson( (FILE*)infile, config ) == nullptr )
      return configuration();

    return config.SetCfgPath( szpath ) == 0 ? config : configuration();
  }

}
