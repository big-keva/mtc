# include "tmppath.h"

# if defined( _WIN32 ) || defined( _WIN64 )
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <stdlib.h>
#   include <io.h>

auto  GetTmpPath() -> std::string
{
  char  tmp_path[1024];

  GetTempPath( sizeof(tmp_path), tmp_path );

  return tmp_path;
}

auto  GetTmpName( const char* tmpl ) -> std::string
{
  auto  output = GetTmpPath() + tmpl;

  return _mktemp( (char*)output.c_str() );
}

# else
# include <unistd.h>

auto  GetTmpPath() -> std::string {  return "/tmp/";  }

auto  GetTmpName( const char* tmpl ) -> std::string
{
  auto  output = GetTmpPath() + tmpl;
  auto  get_fd = mkstemp( (char*)output.c_str() );

  return get_fd >= 0 ? close( get_fd ), output : "";
}

# endif
