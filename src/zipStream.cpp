# include "../zipStream.h"
# include "../fileStream.h"
# include "../exceptions.h"
# include "../recursive_shared_mutex.hpp"
# include <vector>
# include <mutex>
# include <fcntl.h>
# include <stdexcept>
# define ZLIB_WINAPI
# include <zlib.h>

namespace mtc {
namespace zip {

  class ZipBuffer final: public IByteBuffer, public std::vector<char>
  {
    implement_lifetime_control

  public:
    ZipBuffer( size_t size ): std::vector<char>( size ) {}

  public:     // IByteBuffer overridables
    const char* GetPtr(                     ) const noexcept override {  return data();  }
    size_t      GetLen(                     ) const noexcept override {  return size();  }
    int         SetBuf( const void*, size_t ) noexcept override {  return EINVAL;   }
    int         SetLen( size_t              ) noexcept override {  return EINVAL;   }

  };

  template <class error>
  class ZipStream final: public IFlatStream
  {
    implement_lifetime_control

  public:     // construction
    ZipStream( gzFile p = nullptr ): gzfile( p ) {}
   ~ZipStream();

  public:     // from IByteStream
    uint32_t  Get(       void*, uint32_t ) override;
    uint32_t  Put( const void*, uint32_t ) override;

  public:     // from IFlatStream
    auto  PGet(                int64_t, uint32_t ) -> mtc::api<IByteBuffer> override;
    int   PGet( IByteBuffer**, int64_t, uint32_t ) override;
    auto  PGet(       void*,   int64_t, uint32_t ) -> int32_t override;
    auto  PPut( const void*,   int64_t, uint32_t ) -> int32_t override;
    auto  Seek( int64_t                          ) -> int64_t override;
    auto  Size(                                  ) -> int64_t override {  return -1;  }
    auto  Tell(                                  ) -> int64_t override;

  protected:  // variables
    std::mutex  gzlock;
    gzFile      gzfile;

  };

  // ZipStream implementation

  template <class error>
  ZipStream<error>::~ZipStream()
  {
    if ( gzfile != nullptr )
      gzclose( gzfile );
  }

  template <class error>
  uint32_t  ZipStream<error>::Get( void* p, uint32_t l )
  {
    return interlocked( make_unique_lock( gzlock ),[&]()
      {  return (uint32_t)gzread( gzfile, p, l );  } );
  }

  template <class error>
  uint32_t  ZipStream<error>::Put( const void* p, uint32_t l )
  {
    return interlocked( make_unique_lock( gzlock ),[&]()
      {  return (uint32_t)gzwrite( gzfile, p, l );  } );
  }

  template <class error>
  int   ZipStream<error>::PGet( IByteBuffer** buf, int64_t pos, uint32_t len )
  {
    api<ZipBuffer>  palloc;

    if ( buf == nullptr )
      return error()( EINVAL, std::invalid_argument( "invalid (NULL) output address" ) );

    try
    {
      palloc = new ZipBuffer( len );

      if ( PGet( (char*)palloc->data(), pos, palloc->size() ) != int32_t(palloc->size()) )
        return EACCES;

      return ((*buf = palloc.ptr())->Attach(), 0);
    }
    catch ( const std::bad_alloc& xp  ) {  return error()( ENOMEM, xp );  }
    catch ( const std::exception& xp )  {  return error()( EFAULT, xp );  }
  }

  template <class error>
  int32_t ZipStream<error>::PGet( void* ptr, int64_t pos, uint32_t len )
  {
    auto    locked = make_unique_lock( gzlock );
    auto    curpos = gztell( gzfile );
    int32_t cbread;

    if ( gzseek( gzfile, (z_off_t)pos, SEEK_SET ) != pos )
    {
      gzseek( gzfile, curpos, SEEK_SET );
      return error()( -1, "invalid gzfile offset" );
    }

    cbread = gzread( gzfile, ptr, len );

    return gzseek( gzfile, curpos, SEEK_SET ), cbread;
  }

  template <class error>
  int32_t ZipStream<error>::PPut( const void* ptr, int64_t pos, uint32_t len )
  {
    auto    locked = make_unique_lock( gzlock );
    auto    curpos = gztell( gzfile );
    int32_t nwrite;

    if ( gzseek( gzfile, (z_off_t)pos, SEEK_SET ) != pos )
    {
      gzseek( gzfile, curpos, SEEK_SET );
      return error()( -1, "invalid gzfile offset" );
    }

    nwrite = gzwrite( gzfile, ptr, len );

    return gzseek( gzfile, curpos, SEEK_SET ), nwrite;
  }

  template <class error>
  int64_t   ZipStream<error>::Seek( int64_t pos )
  {
    return interlocked( make_unique_lock( gzlock ), [&]()
      {  return gzseek( gzfile, (z_off_t)pos, SEEK_SET );  } );
  }

  template <class error>
  int64_t   ZipStream<error>::Tell()
  {
    return interlocked( make_unique_lock( gzlock ), [&]()
      {  return gztell( gzfile );  } );
  }

  // exception processors

  struct report_error_no_except
  {
    template <class result, class except>
    result  operator ()( result res, const except& )  {   return res;  }
  };

  struct report_error_exception
  {
    template <class result, class except>
    result  operator ()( const result&, const except& x )  {  throw x;  }
  };

  // creators

  template <class error>
  auto  openStream( const char* szpath, unsigned dwmode, unsigned buflen ) -> api<ZipStream<error>>
  {
    gzFile          thezip;
    char            szmode[3] = { 0, 'b', 0 };

    switch ( dwmode & 0x000f )
    {
      case O_RDONLY:  szmode[0] = 'r';  break;
      case O_WRONLY:  szmode[0] = 'w';  break;
      case O_APPEND:  szmode[0] = 'a';  break;
      default:        return error()( nullptr, std::invalid_argument( "unsupported access mode" ) );
    }

    if ( (thezip = gzopen( szpath, szmode )) == nullptr )
      return error()( nullptr, file_error( "file not found or invalid zip file" ) );

    try
    {
      gzbuffer( thezip, buflen <= 0x400 ? 0x400 : buflen );
      return new ZipStream<error>( thezip );
    }
    catch ( const std::bad_alloc& xp )
      {  gzclose( thezip );  return error()( nullptr, xp );  }
    catch ( ... )
      {  gzclose( thezip );  return error()( nullptr, std::current_exception() );  }
  }

  template <class error>
  auto  loadBuffer( const char* sz ) -> api<IByteBuffer>
  {
    api<IFlatStream>  infile;
    api<ZipBuffer>    buffer;
    char              zipbuf[0x400 * 0x40];
    uint32_t          cbread;

    if ( (infile = openStream<error>( sz, O_RDONLY, 0x400 * 0x400 )) == nullptr )
      return nullptr;

    try
    {
      buffer = new ZipBuffer( 0 );

      while ( (cbread = infile->Get( zipbuf, sizeof(zipbuf) )) != (uint32_t)-1 )
      {
        buffer->insert( buffer->end(), zipbuf, zipbuf + cbread );

        if ( cbread < sizeof(zipbuf) )
          return buffer.ptr();
      }

      return buffer.ptr();
    }
    catch ( const std::bad_alloc& xp )
      {  return error()( nullptr, xp );  }
    catch ( ... )
      {  return error()( nullptr, std::current_exception() );  }
  }

  auto  OpenStream( const char* sz, unsigned mode, const enable_exceptions_t&, unsigned buflen ) -> api<IFlatStream>
    {  return openStream<report_error_exception>( sz, mode, buflen ).ptr();  }
  auto  OpenStream( const char* sz, unsigned mode, const disable_exceptions_t&, unsigned buflen ) -> api<IFlatStream>
    {  return openStream<report_error_no_except>( sz, mode, buflen ).ptr();  }

  auto  OpenStream( const std::string& sz, unsigned mode, const enable_exceptions_t&, unsigned buflen ) -> api<IFlatStream>
    {  return openStream<report_error_exception>( sz.c_str(), mode, buflen ).ptr();  }
  auto  OpenStream( const std::string& sz, unsigned mode, const disable_exceptions_t&, unsigned buflen ) -> api<IFlatStream>
    {  return openStream<report_error_no_except>( sz.c_str(), mode, buflen ).ptr();  }

  auto  LoadBuffer( const char* sz, const enable_exceptions_t& ) -> api<IByteBuffer>
    {  return loadBuffer<report_error_exception>( sz );  }
  auto  LoadBuffer( const char* sz, const disable_exceptions_t& ) -> api<IByteBuffer>
    {  return loadBuffer<report_error_no_except>( sz );  }

  auto  LoadBuffer( const std::string& sz, const enable_exceptions_t& ) -> api<IByteBuffer>
    {  return loadBuffer<report_error_exception>( sz.c_str() );  }
  auto  LoadBuffer( const std::string& sz, const disable_exceptions_t& ) -> api<IByteBuffer>
    {  return loadBuffer<report_error_no_except>( sz.c_str() );  }

}}
