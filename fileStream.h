# if !defined( __mtc_fileStream_h__ )
# define  __mtc_fileStream_h__
# include "iStream.h"

namespace mtc
{

  struct  IFileStream: public IFlatStream
  {
    virtual int   Map( IByteBuffer**, int64_t, word32_t ) = 0;
  };

  IFileStream*  OpenFileStream( const char* sz, unsigned dwmode = 0 );
  IByteBuffer*  LoadFileBuffer( const char* sz );

}

# endif  // __mtc_fileStream_h__
