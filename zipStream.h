# if !defined( __mtc_zipStream_h__ )
# define __mtc_zipStream_h__
# include "iStream.h"
# include "stdlog.h"

namespace mtc
{
  IFlatStream*  OpenZipStream( const char* sz, unsigned dwmode = 0 );
  IByteBuffer*  LoadZipBuffer( const char* sz );
}

# endif  // __mtc_zipStream_h__
