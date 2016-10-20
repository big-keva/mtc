# if !defined( __mtc_istream_h__ )
# define __mtc_istream_h__
# include "platform.h"
# include "iBuffer.h"

namespace mtc
{
  //
  // IStream - public interface for sequental reading/writing
  //
  struct  IByteStream: public Iface
  {
    virtual unsigned  Get(       void*, word32_t ) = 0;
    virtual unsigned  Put( const void*, word32_t ) = 0;
  };

  struct  IFlatStream: public IByteStream
  {
    virtual int       PGet( IByteBuffer**, int64_t, word32_t ) = 0;
    virtual word32_t  PGet(       void*,   int64_t, word32_t ) = 0;
    virtual word32_t  PPut( const void*,   int64_t, word32_t ) = 0;
    virtual int64_t   Seek( int64_t                          ) = 0;
    virtual int64_t   Size(                                  ) = 0;
    virtual int64_t   Tell(                                  ) = 0;
  };

}

# endif  // __mtc_istream_h__
