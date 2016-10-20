# include "dirtools.hpp"

# if defined( WIN32 )
#   include <windows.h>
# else
# endif

namespace mtc
{

  int64_t GetFileSize( const char* szpath )
  {
# if defined( WIN32 )
    WIN32_FILE_ATTRIBUTE_DATA fiData;

    return GetFileAttributesEx( szpath, GetFileExInfoStandard, &fiData ) != 0 ?
      (((int64_t)fiData.nFileSizeHigh) << 32) | fiData.nFileSizeLow : -1;
# else
#   error Implementation not defined!
# endif
  }

}
