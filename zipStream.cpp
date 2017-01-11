# include "zipStream.h"
# include "autoptr.h"
# include "array.h"
# include <zlib.h>
# include <fcntl.h>
# include <mutex>

namespace mtc
{

  class ZipBuffer;
  class ZipStream;

  class ZipBuffer: public IByteBuffer, protected array<char>
  {
    friend IByteBuffer*  LoadZipBuffer( const char* sz );
    friend class ZipStream;

    implement_lifetime_control

  public:     // IByteBuffer overridables
    virtual const char* GetPtr(                       ) {  return begin();  }
    virtual word32_t    GetLen(                       ) {  return size();   }
    virtual int         SetBuf( const void*, word32_t ) {  return EINVAL;   }
    virtual int         SetLen( word32_t              ) {  return EINVAL;   }

  protected:
            int         setlen( int newlen )  {  return array<char>::SetLen( newlen );  }
  };

  class ZipStream: public IFlatStream
  {
    friend IFlatStream*  OpenZipStream( const char* sz, unsigned dwmode );
    friend IByteBuffer*  LoadZipBuffer( const char* sz );

    implement_lifetime_control

  public:     // construction
    ZipStream( gzFile p = nullptr ): gzfile( p )
      {
      }
   ~ZipStream()
      {
        if ( gzfile != nullptr )
          gzclose( gzfile );
      }

  public:     // from IByteStream
    virtual word32_t  Get(       void*, word32_t );
    virtual word32_t  Put( const void*, word32_t );

  public:     // from IFlatStream
    virtual int       GetBuf( IByteBuffer**, int64_t, word32_t );
    virtual word32_t  PosGet(       void*,   int64_t, word32_t );
    virtual word32_t  PosPut( const void*,   int64_t, word32_t );
    virtual int64_t   Seek  ( int64_t                          );
    virtual int64_t   Size  (                                  );
    virtual int64_t   Tell  (                                  );

  protected:  // variables
    std::mutex  gzlock;
    gzFile      gzfile;

  };

  // ZipStream implementation

  word32_t  ZipStream::Get( void* p, word32_t l )
  {
    return (word32_t)gzread( gzfile, p, l );
  }

  word32_t  ZipStream::Put( const void* p, word32_t l )
  {
    return (word32_t)gzwrite( gzfile, p, l );
  }

  int       ZipStream::GetBuf( IByteBuffer** buf, int64_t pos, word32_t len )
  {
    _auto_<ZipBuffer> palloc;

    if ( buf == nullptr )
      return EINVAL;

    if ( (palloc = allocate<ZipBuffer>()) == nullptr || palloc->setlen( len ) != 0 )
      return ENOMEM;

    if ( PosGet( palloc->begin(), pos, palloc->size() ) != palloc->size() )
      return EACCES;

    return 0 * (*buf = palloc.detach())->Attach();
  }

  word32_t  ZipStream::PosGet( void* ptr, int64_t pos, word32_t len )
  {
    std::lock_guard<std::mutex> aulock( gzlock );

    return gzseek( gzfile, (off_t)pos, SEEK_SET ) == pos ? (word32_t)gzread( gzfile, ptr, len ) : (word32_t)-1;
  }

  word32_t  ZipStream::PosPut( const void* ptr, int64_t pos, word32_t len )
  {
    return gzseek( gzfile, (off_t)pos, SEEK_SET ) == pos ? (word32_t)gzwrite( gzfile, ptr, len ) : (word32_t)-1;
  }

  int64_t   ZipStream::Seek( int64_t pos )
  {
    return gzseek( gzfile, (off_t)pos, SEEK_SET );
  }

  int64_t   ZipStream::Size()
  {
    return -1;
  }

  int64_t   ZipStream::Tell()
  {
    return gztell( gzfile );
  }

  IFlatStream*  OpenZipStream( const char* szpath, unsigned dwmode )
  {
    _auto_<ZipStream> palloc;
    gzFile            thezip;
    char              szmode[8];
    char*             pszmod = szmode;

    switch ( dwmode & 0x000f )
    {
      case O_RDONLY:  *pszmod++ = 'r';  break;
      case O_WRONLY:  *pszmod++ = 'w';  break;
      case O_APPEND:  *pszmod++ = 'a';  break;
      default:        return nullptr;
    }

    *pszmod = '\0';

    if ( (thezip = gzopen( szpath, szmode )) == nullptr )
      return nullptr;

    if ( (palloc = allocate<ZipStream>( thezip )) == nullptr )
      gzclose( thezip );

    return palloc.detach();
  }

  IByteBuffer*  LoadZipBuffer( const char* sz )
  {
    _auto_<ZipBuffer> buffer;
    API<IFlatStream>  infile;
    char              zipbuf[0x400 * 0x40];
    word32_t          cbread;

    if ( (infile = OpenZipStream( sz, O_RDONLY )) == nullptr )
      return nullptr;

    if ( (buffer = allocate<ZipBuffer>()) == nullptr )
      return nullptr;

    while ( (cbread = infile->Get( zipbuf, sizeof(zipbuf) )) != (word32_t)-1 )
    {
      if ( buffer->Append( cbread, zipbuf ) != 0 )
        return nullptr;
      if ( cbread < sizeof(zipbuf) )
        return buffer.detach();
    }

    return nullptr;
  }

}
