# include "dirtools.hpp"

# if defined( _WIN32 )
#   include <windows.h>
# else
#   include <sys/stat.h>
#   include <unistd.h>
# endif

namespace mtc
{

  int64_t GetFileSize( const char* szpath )
  {
# if defined( _WIN32 )
    WIN32_FILE_ATTRIBUTE_DATA fiData;

    return GetFileAttributesEx( szpath, GetFileExInfoStandard, &fiData ) != 0 ?
      (((int64_t)fiData.nFileSizeHigh) << 32) | fiData.nFileSizeLow : -1;
# elif defined( __ARCH_WANT_STAT64 )
    struct stat64 fistat;

    return stat64( szpath, &fistat ) == 0 ? fistat.st_size : -1;
# else
    struct stat fistat;

    return stat( szpath, &fistat ) == 0 ? fistat.st_size : -1;
# endif
  }

}
