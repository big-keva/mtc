# if !defined( __mtc_zipStream_h__ )
# define __mtc_zipStream_h__
# include "iStream.h"

namespace mtc
{
  IFlatStream*  OpenZipStream( const char* sz, unsigned dwmode = 0, unsigned buflen = 0x400 * 0x400 );
  IByteBuffer*  LoadZipBuffer( const char* sz );
}

# endif  // __mtc_zipStream_h__
