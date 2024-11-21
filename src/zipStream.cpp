# include "../zipStream.h"
# include <vector>
# include <mutex>
# include <fcntl.h>
# include <stdexcept>
# define ZLIB_WINAPI
# include <zlib.h>

namespace mtc
{
  class ZipBuffer;
  class ZipStream;

  class ZipBuffer final: public IByteBuffer, protected std::vector<char>
  {
    friend IByteBuffer*  LoadZipBuffer( const char* sz );
    friend class ZipStream;

    implement_lifetime_control

  public:
    ZipBuffer( size_t size ): std::vector<char>( size ) {}

  public:     // IByteBuffer overridables
    const char* GetPtr(                       ) const noexcept override {  return data();  }
    word32_t    GetLen(                       ) const noexcept override {  return size();  }
    int         SetBuf( const void*, word32_t ) noexcept override {  return EINVAL;   }
    int         SetLen( word32_t              ) noexcept override {  return EINVAL;   }

  };

  class ZipStream final: public IFlatStream
  {
    friend IFlatStream*  OpenZipStream( const char* sz, unsigned dwmode, unsigned buflen );
    friend IByteBuffer*  LoadZipBuffer( const char* sz );

    implement_lifetime_control

  public:     // construction
    ZipStream( gzFile p = nullptr ): gzfile( p ) {}
   ~ZipStream()
      {
        if ( gzfile != nullptr )
          gzclose( gzfile );
      }

  public:     // from IByteStream
    word32_t  Get(       void*, word32_t ) override;
    word32_t  Put( const void*, word32_t ) override;

  public:     // from IFlatStream
    int       GetBuf( IByteBuffer**, int64_t, word32_t ) override;
    word32_t  PosGet(       void*,   int64_t, word32_t ) override;
    word32_t  PosPut( const void*,   int64_t, word32_t ) override;
    int64_t   Seek  ( int64_t                          ) override;
    int64_t   Size  (                                  ) override;
    int64_t   Tell  (                                  ) override;

  protected:  // variables
    std::mutex  gzlock;
    gzFile      gzfile;

  };

  // ZipStream implementation

  word32_t  ZipStream::Get( void* p, word32_t l )
  {
    std::lock_guard<std::mutex> locker( gzlock );

    if ( gzfile == nullptr )
      throw std::logic_error( "uninitialized zip stream" );
    return (word32_t)gzread( gzfile, p, l );
  }

  word32_t  ZipStream::Put( const void* p, word32_t l )
  {
    std::lock_guard<std::mutex> locker( gzlock );

    if ( gzfile == nullptr )
      throw std::logic_error( "uninitialized zip stream" );
    return (word32_t)gzwrite( gzfile, p, l );
  }

  int       ZipStream::GetBuf( IByteBuffer** buf, int64_t pos, word32_t len )
  {
    api<ZipBuffer>  palloc;

    if ( gzfile == nullptr )
      throw std::logic_error( "uninitialized zip stream" );
    if ( buf == nullptr )
      return EINVAL;

    if ( (palloc = allocate<ZipBuffer>( len )) == nullptr )
      return ENOMEM;

    if ( PosGet( (char*)palloc->data(), pos, palloc->size() ) != (word32_t)palloc->size() )
      return EACCES;

    return ((*buf = palloc.ptr())->Attach(), 0);
  }

  word32_t  ZipStream::PosGet( void* ptr, int64_t pos, word32_t len )
  {
    std::lock_guard<std::mutex> locker( gzlock );

    if ( gzfile == nullptr )
      throw std::logic_error( "uninitialized zip stream" );
    return gzseek( gzfile, (z_off_t)pos, SEEK_SET ) == pos ? (word32_t)gzread( gzfile, ptr, len ) : (word32_t)-1;
  }

  word32_t  ZipStream::PosPut( const void* ptr, int64_t pos, word32_t len )
  {
    std::lock_guard<std::mutex> locker( gzlock );

    if ( gzfile == nullptr )
      throw std::logic_error( "uninitialized zip stream" );
    return gzseek( gzfile, (z_off_t)pos, SEEK_SET ) == pos ? (word32_t)gzwrite( gzfile, ptr, len ) : (word32_t)-1;
  }

  int64_t   ZipStream::Seek( int64_t pos )
  {
    std::lock_guard<std::mutex> locker( gzlock );

    if ( gzfile == nullptr )
      throw std::logic_error( "uninitialized zip stream" );
    return gzseek( gzfile, (z_off_t)pos, SEEK_SET );
  }

  int64_t   ZipStream::Size()
  {
    return -1;
  }

  int64_t   ZipStream::Tell()
  {
    std::lock_guard<std::mutex> locker( gzlock );

    if ( gzfile == nullptr )
      throw std::logic_error( "uninitialized zip stream" );
    return gztell( gzfile );
  }

  IFlatStream*  OpenZipStream( const char* szpath, unsigned dwmode, unsigned buflen )
  {
    api<ZipStream>  palloc;
    gzFile          thezip;
    char            szmode[8];
    char*           pszmod = szmode;

    switch ( dwmode & 0x000f )
    {
      case O_RDONLY:  *pszmod++ = 'r';  break;
      case O_WRONLY:  *pszmod++ = 'w';  break;
      case O_APPEND:  *pszmod++ = 'a';  break;
      default:        return nullptr;
    }

    *pszmod++ = 'b';  *pszmod++ = '\0';

    if ( (thezip = gzopen( szpath, szmode )) == nullptr )
      return nullptr;

    if ( (palloc = allocate<ZipStream>( thezip )) == nullptr )  gzclose( thezip );
      else  gzbuffer( thezip, buflen <= 0x400 ? 0x400 : buflen );

    return palloc.ptr();
  }

  IByteBuffer*  LoadZipBuffer( const char* sz )
  {
    api<ZipBuffer>    buffer;
    api<IFlatStream>  infile;
    char              zipbuf[0x400 * 0x40];
    word32_t          cbread;

    if ( (infile = OpenZipStream( sz, O_RDONLY )) == nullptr )
      return nullptr;

    if ( (buffer = allocate<ZipBuffer>( 0 )) == nullptr )
      return nullptr;

    while ( (cbread = infile->Get( zipbuf, sizeof(zipbuf) )) != (word32_t)-1 )
    {
      buffer->insert( buffer->end(), zipbuf, zipbuf + cbread );

      if ( cbread < sizeof(zipbuf) )
        return buffer.ptr();
    }

    return nullptr;
  }

}
