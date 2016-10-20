# include "fileStream.h"
# include "stdlog.h"
# include "autoptr.h"
# include <assert.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# if defined( WIN32 )
#   include <Windows.h>
# else
#   include <unistd.h>
#   include <sys/mman.h>
# endif  // WIN32

# if !defined( win32_decl )
#   if defined( WIN32 )
#     define  win32_decl( expr )  expr
#     define  posix_decl( expr )
#   else
#     define  win32_decl( expr )
#     define  posix_decl( expr )  expr
#   endif  // WIN32
# endif // !win32_decl

# if defined( _MSC_VER )
#   pragma warning( disable: 4291 )
# endif  // _MSC_VER

namespace mtc
{
  class FileStream;

  class FileMemmap: public IByteBuffer
  {
    implement_lifetime_control

    friend class  FileStream;

  public:     // construction
    FileMemmap();
   ~FileMemmap();
    FileMemmap( const FileMemmap& ) = delete;
    FileMemmap& operator = ( const FileMemmap& ) = delete;

  public:     // overridables
    virtual const char* GetPtr(                       ) noexcept override {  return nshift + (char*)ptrmap;  }
    virtual unsigned    GetLen(                       ) noexcept override {  return cchmem;  }
    virtual int         SetBuf( const void*, unsigned ) noexcept override {  return EINVAL;  }
    virtual int         SetLen( unsigned              ) noexcept override {  return EINVAL;  }

  public:     // helpers
    int   Create( FileStream*, int64_t, word32_t );

  protected:  // variables
    word32_t                cchmem;
    unsigned                dwgran;
    unsigned                nshift;
    word32_t                maplen;
    void*                   ptrmap;
    win32_decl( HANDLE      handle );

  };

  class FileStream: public IFileStream
  {
    implement_lifetime_control

    friend class  FileMemmap;

    struct  filebuffer: public IByteBuffer
    {
      implement_lifetime_control

    public:     // construction
      filebuffer( unsigned l ): length( l ) {}
          
    public:     // overridables
      virtual const char* GetPtr() noexcept override {  return buffer;  }
      virtual unsigned    GetLen() noexcept override {  return length;  }
      virtual int         SetBuf( const void*, unsigned ) noexcept override {  return EINVAL;  }
      virtual int         SetLen( unsigned ) noexcept override {  return EINVAL;  }
    
    protected:  // variables
      unsigned  length;
      char      buffer[1];
    };

  public:     // construction
    FileStream();
   ~FileStream();

  public:     // overridables from IStream
    virtual unsigned  Get (       void*,   word32_t ) noexcept override;
    virtual unsigned  Put ( const void*,   word32_t ) noexcept override;

  public:     // overridables from IFlatStream
    virtual int       PGet( IByteBuffer**, int64_t, word32_t ) noexcept override;
    virtual word32_t  PGet(       void*,   int64_t, word32_t ) noexcept override;
    virtual word32_t  PPut( const void*,   int64_t, word32_t ) noexcept override;
    virtual int64_t   Seek( int64_t                          ) noexcept override;
    virtual int64_t   Size(                                  ) noexcept override;
    virtual int64_t   Tell(                                  ) noexcept override;       

  public:     // overridables from IFileStream
    virtual int       Map ( IByteBuffer**, int64_t, word32_t ) noexcept override;

  public:     // creation
    int               Open( const char*, unsigned );
    IByteBuffer*      Load();

  protected:  // helpers
    void              Close();

  protected:  // variables
    win32_decl( HANDLE handle );
    posix_decl( int    handle )

  };

  unsigned  GetMemPageSize()
  {
# if defined( WIN32 )
    SYSTEM_INFO syinfo;

    GetSystemInfo( &syinfo );
    return syinfo.dwAllocationGranularity;
# else
    return getpagesize();
# endif
  }
  // FileMemmap implementation

  FileMemmap::FileMemmap(): cchmem( 0 ), dwgran( GetMemPageSize() ), nshift( 0 ), maplen( 0 ), ptrmap( 0 )
  {
    win32_decl( handle = INVALID_HANDLE_VALUE );
  }

  FileMemmap::~FileMemmap()
  {
    if ( ptrmap != nullptr )
    {
      win32_decl( UnmapViewOfFile( ptrmap ) );
      win32_decl( CloseHandle( handle ) );
      posix_decl( munmap( ptrmap, maplen ) );
    }
  }

  int   FileMemmap::Create( FileStream* stm, int64_t off, word32_t len )
  {
# if defined( WIN32 )
    word32_t  offshi = (word32_t)(off >> 32);
    word32_t  offslo = (word32_t)(off);
    word32_t  oalign = offslo / dwgran * dwgran;

  // create mapping view
    if ( (handle = CreateFileMapping( stm->handle, nullptr, PAGE_READONLY | SEC_COMMIT, 0, 0, nullptr )) == nullptr )
      return log_error( EFAULT, "Could not create the file mapping handle @" __FILE__ ":%u", __LINE__ );

    cchmem = len;
    nshift = offslo - oalign;
    maplen = cchmem + nshift;

  // create mapping pointer
    if ( (ptrmap = MapViewOfFile( handle, FILE_MAP_READ, offshi, oalign, (unsigned)maplen )) == nullptr )
    {
      CloseHandle( handle );  handle = INVALID_HANDLE_VALUE;
      return log_error( EFAULT, "Could not MapViewOfFile() for the requested block, error code %u!", GetLastError() );
    }

    return 0;
# else
    int64_t   oalign = (off / dwgran) * dwgran;

    cchmem = len;
    nshift = off - oalign;
    maplen = ((len + nshift + dwgran) / dwgran) * dwgran;

    if ( (ptrmap = mmap( NULL, maplen, PROT_READ, MAP_SHARED, stm->handle, oalign )) == MAP_FAILED )
      return errno;
    return 0;
# endif
  }

  // CFileStream implementation

  FileStream::FileStream()
  {
    win32_decl( handle = INVALID_HANDLE_VALUE );
    posix_decl( handle = -1 );
  }

  FileStream::~FileStream()
  {
    Close();
  }

  int   FileStream::Map( IByteBuffer** ppi, int64_t off, word32_t len ) noexcept
  {
    _auto_<FileMemmap>  memmap;
    int                 nerror;

    if ( ppi == nullptr )
      return EINVAL;

    if ( (memmap = allocate<FileMemmap>()) == nullptr )
      return ENOMEM;

    if ( (nerror = memmap->Create( this, off, len )) != 0 )
      return nerror;

    (*ppi = memmap.detach())->Attach();
      return 0;
  }

  IByteBuffer*  FileStream::Load()
  {
    _auto_<filebuffer>  buf;
    int64_t             len = (long)Size();

    if ( len > SIZE_MAX )
      return nullptr;

    if ( (buf = (filebuffer*)malloc( (size_t)(sizeof(filebuffer) + len - 1) )) == nullptr )
      return nullptr;

    return PGet( (char*)(new( buf.ptr() ) filebuffer( (unsigned)len ))->GetPtr(), 0, (unsigned)len ) == len ? buf.detach() : nullptr;
  }

# if defined( WIN32 )

  unsigned  FileStream::Get( void* out, unsigned len ) noexcept
  {
    DWORD cbread;

    return ReadFile( handle, out, len, &cbread, nullptr ) ? cbread : 0;
  }

  unsigned  FileStream::Put( const void* src, unsigned len ) noexcept
  {
    DWORD nwrite;

    return WriteFile( handle, src, len, &nwrite, nullptr ) ? nwrite : 0;
  }

  int       FileStream::PGet( IByteBuffer** ppi, int64_t off, word32_t len ) noexcept
  {
    _auto_<filebuffer>  buffer;

    if ( (buffer = (filebuffer*)malloc( sizeof(filebuffer) + len - 1 )) == nullptr )
      return ENOMEM;
    if ( PGet( (char*)(new( buffer.ptr() ) filebuffer( len ))->GetPtr(), off, len ) != len )
      return EACCES;
    (*ppi = buffer.detach())->Attach();
      return 0;
  }

  unsigned  FileStream::PGet( void* out, int64_t off, word32_t len ) noexcept
  {
    DWORD       cbread;
    OVERLAPPED  reinfo;

    reinfo.hEvent = NULL;
    reinfo.Internal = 0;
    reinfo.InternalHigh = 0;

    reinfo.OffsetHigh = (DWORD)(off >> 32);
    reinfo.Offset = (DWORD)off;

    return ReadFile( handle, out, len, &cbread, &reinfo ) ? cbread : 0;
  }

  unsigned  FileStream::PPut( const void* src, int64_t off, word32_t len ) noexcept
  {
    DWORD       nwrite;
    OVERLAPPED  reinfo;

    reinfo.hEvent = NULL;
    reinfo.Internal = 0;
    reinfo.InternalHigh = 0;
    reinfo.OffsetHigh = (DWORD)(off >> 32);
    reinfo.Offset = (DWORD)off;

    return WriteFile( handle, src, len, &nwrite, &reinfo ) ? nwrite : 0;
  }

  int64_t   FileStream::Seek( int64_t off ) noexcept
  {
    LONG    hioffs = (LONG)(off >> 32);
    LONG    looffs = (LONG)(off);
    DWORD   dwmove;

    return (dwmove = SetFilePointer( handle, looffs, &hioffs, FILE_BEGIN )) == INVALID_SET_FILE_POINTER ? (int64_t)-1 : off;
  }

  int64_t   FileStream::Size() noexcept
  {
    DWORD   looffs;
    DWORD   hioffs;

    return (looffs = GetFileSize( handle, &hioffs )) == INVALID_FILE_SIZE ? (int64_t)-1 : looffs | (((int64_t)hioffs) << 32);
  }

  int64_t FileStream::Tell() noexcept
  {
    LONG    hioffs = 0;
    DWORD   dwmove;

    return (dwmove = SetFilePointer( handle, 0, &hioffs, FILE_CURRENT )) == INVALID_SET_FILE_POINTER ? (int64_t)-1 : dwmove | (((int64_t)hioffs) << 32);
  }

  int     FileStream::Open( const char* szname, unsigned dwmode )
  {
    DWORD     dwAccess;
    DWORD     dwDispos;
    DWORD     dwFlAttr = FILE_ATTRIBUTE_NORMAL;

  // detect access mode
    switch( dwmode & 0x000f )
    {
      case O_RDONLY:
        dwAccess = GENERIC_READ;
        break;
      case O_WRONLY:
        dwAccess = GENERIC_WRITE;
        break;
      case O_RDWR:
        dwAccess = GENERIC_WRITE | GENERIC_READ;
        break;
      default:
        dwAccess = 0;
    }

  // detect the disposition
    dwDispos = 0;
    switch ( dwmode & 0x0300 )
    {
      case 0:
        dwDispos = OPEN_EXISTING;
        break;
      case O_CREAT:
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        if ( (dwmode & O_EXCL ) != 0 )
          dwDispos = CREATE_NEW;
        else
          dwDispos = OPEN_ALWAYS;
        break;
      case O_TRUNC:
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        dwDispos = TRUNCATE_EXISTING;
        break;
      default:  // O_CREAT | O_TRUNC
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        dwDispos = CREATE_ALWAYS;
        break;
    }

    if ( (dwmode & 0x00F0) & O_RANDOM )
      dwFlAttr |= FILE_FLAG_RANDOM_ACCESS;
    if ( (dwmode & 0x00F0) & O_SEQUENTIAL )
      dwFlAttr |= FILE_FLAG_SEQUENTIAL_SCAN;
    if ( (dwmode & 0x00F0) & O_TEMPORARY )
      dwFlAttr |= FILE_FLAG_DELETE_ON_CLOSE;

  // create file handle
    handle = CreateFile( szname, dwAccess, FILE_SHARE_READ, NULL, dwDispos, dwFlAttr, NULL );
    return handle == INVALID_HANDLE_VALUE ? ENOENT : 0;
  }

  void  FileStream::Close()
  {
    if ( handle != INVALID_HANDLE_VALUE )
      CloseHandle( handle );
    handle = INVALID_HANDLE_VALUE;
  }

# else

  _u_err_ CFileStream::Close()
  {
    if ( handle != -1 )
      ::close( handle );
    handle = -1;
      return _u_err_ok;
  }

  _u_err_ CFileStream::Create( const char*  szname, unsigned short dwmode )
  {
    _u_err_ uerror;

  // check if stream is open; close it
    if ( (uerror = Close()) != _u_err_ok )
      return uerror;

  // create new name
    if ( (lppath = strdup( szname )) == NULL )
      return _u_err_nomem;

    if ( (handle = ::open( lppath, dwmode, 0666 )) == -1 )
      return _u_err_no_file;
    return _u_err_ok;
  }

  unsigned  CFileStream::Get( void*        lpdata,
                              unsigned     ccdata )
  {
    return ::read( handle, lpdata, ccdata );
  }

  unsigned  CFileStream::Put( const void*  lpdata,
                              unsigned     ccdata )
  {
    return ::write( handle, lpdata, ccdata );
  }

  unsigned  CFileStream::PGet( void*       lpdata,
                               unsigned    ccdata,
                               int64       offset )
  {
    return ::pread( handle, lpdata, ccdata, offset );
  }

  unsigned  CFileStream::PPut( const void* lpdata,
                               unsigned    ccdata,
                               int64       offset )
  {
    return ::pwrite( handle, lpdata, ccdata, offset );
  }

  int64     CFileStream::Seek( int64       offset )
  {
    return ::lseek( handle, offset, SEEK_SET );
  }

  int64     CFileStream::Size()
  {
    int64   curpos = ::lseek( handle, 0, SEEK_CUR );
    int64   curlen = ::lseek( handle, 0, SEEK_END );

    ::lseek( handle, curpos, SEEK_SET );
    return curlen;
  }

# endif

  FileStream*   openFileStream( const char* lpname, unsigned dwmode )
  {
    _auto_<FileStream>  stream;

    if ( lpname == nullptr || *lpname == '\0' )
      return nullptr;

    if ( (stream = allocate<FileStream>()) == nullptr )
      return nullptr;

    if ( stream->Open( lpname, dwmode ) != 0 )
      return nullptr;

    return stream.detach();
  }

  IFileStream*  OpenFileStream( const char* lpname, unsigned dwmode )
  {
    return openFileStream( lpname, dwmode );
  }

  IByteBuffer*  LoadFileBuffer( const char* szname )
  {
    API<FileStream> lpfile;

    if ( (lpfile = openFileStream( szname, O_RDONLY )) == nullptr )
      return nullptr;
    return lpfile->Load();
  }

}  // mtc namespace
