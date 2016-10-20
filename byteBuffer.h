# if !defined( __byteBuffer_h__ )
# define __byteBuffer_h__
# include "iBuffer.h"

namespace mtc
{
  int   CreateByteBuffer( IByteBuffer** );
  int   CreateByteBuffer( IByteBuffer**, const void*, unsigned );
}

# endif  // __byteBuffer_h__
