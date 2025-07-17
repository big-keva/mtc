/*

The MIT License (MIT)

Copyright (c) 2000-2016 Андрей Коваленко aka Keva
  keva@meta.ua
  keva@rambler.ru
  skype: big_keva
  phone: +7(495)648-4058, +7(916)015-5592

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

=============================================================================

Данная лицензия разрешает лицам, получившим копию данного программного обеспечения
и сопутствующей документации (в дальнейшем именуемыми «Программное Обеспечение»),
безвозмездно использовать Программное Обеспечение без ограничений, включая неограниченное
право на использование, копирование, изменение, слияние, публикацию, распространение,
сублицензирование и/или продажу копий Программного Обеспечения, а также лицам, которым
предоставляется данное Программное Обеспечение, при соблюдении следующих условий:

Указанное выше уведомление об авторском праве и данные условия должны быть включены во
все копии или значимые части данного Программного Обеспечения.

ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ,
ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ТОВАРНОЙ ПРИГОДНОСТИ,
СООТВЕТСТВИЯ ПО ЕГО КОНКРЕТНОМУ НАЗНАЧЕНИЮ И ОТСУТСТВИЯ НАРУШЕНИЙ, НО НЕ ОГРАНИЧИВАЯСЬ
ИМИ.

НИ В КАКОМ СЛУЧАЕ АВТОРЫ ИЛИ ПРАВООБЛАДАТЕЛИ НЕ НЕСУТ ОТВЕТСТВЕННОСТИ ПО КАКИМ-ЛИБО ИСКАМ,
ЗА УЩЕРБ ИЛИ ПО ИНЫМ ТРЕБОВАНИЯМ, В ТОМ ЧИСЛЕ, ПРИ ДЕЙСТВИИ КОНТРАКТА, ДЕЛИКТЕ ИЛИ ИНОЙ
СИТУАЦИИ, ВОЗНИКШИМ ИЗ-ЗА ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ ИЛИ ИНЫХ ДЕЙСТВИЙ
С ПРОГРАММНЫМ ОБЕСПЕЧЕНИЕМ.

*/
# include "../fileStream.h"
# include "../wcsstr.h"
# include "../utf.hpp"
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <errno.h>
# if defined( _WIN32 )
#   define  NOMINMAX
#   include <Windows.h>
# else
#   if !defined( _LARGEFILE64_SOURCE )
#     define _LARGEFILE64_SOURCE
#   endif
#   include <unistd.h>
#   include <sys/mman.h>
# endif  // _WIN32

# if !defined( win32_decl )
#   if defined( _WIN32 )
#     define  win32_decl( expr )  expr
#     define  posix_decl( expr )
#   else
#     define  win32_decl( expr )
#     define  posix_decl( expr )  expr
#   endif  // _WIN32
# endif // !win32_decl

# if defined( _MSC_VER )
#   pragma warning( disable: 4291 )
# endif  // _MSC_VER

# include <limits>

# define LineId( arg )  "" #arg

namespace mtc
{
  template <class error>
  class FileStream;

  template <class error>
  class FileMemmap final: public IByteBuffer
  {
    implement_lifetime_control

    friend class  FileStream<error>;

  protected:     // construction
    FileMemmap();
   ~FileMemmap();
    FileMemmap( const FileMemmap& ) = delete;
    FileMemmap& operator = ( const FileMemmap& ) = delete;

  public:     // overridables
    const char* GetPtr(                     ) const noexcept override {  return nshift + (char*)ptrmap;  }
    size_t      GetLen(                     ) const noexcept override {  return cchmem;  }
    int         SetBuf( const void*, size_t )       noexcept override {  return EINVAL;  }
    int         SetLen( size_t              )       noexcept override {  return EINVAL;  }

  public:     // helpers
    int         Create( FileStream<error>*, int64_t, uint32_t );

  protected:  // variables
    uint32_t  cchmem;
    unsigned  dwgran;
    unsigned  nshift;
    uint32_t  maplen;
    void*     ptrmap;
# if defined( _WIN32 )
    HANDLE    handle;
# endif   // _WIN32

  };

  template <class error>
  class FileStream final: public IFileStream
  {
    implement_lifetime_control

    class filebuffer;

    friend class FileMemmap<error>;

  protected:    // construction
    FileStream( const char* szname, size_t ccname );

    void  operator  delete( void* p ) {  delete[] (char*)p;  }

  public:
   ~FileStream();

   public:      // creation
    static  auto      Create( const char* szname, size_t ccname = (size_t)-1 ) -> FileStream*;

  public:     // overridables from IStream
    uint32_t  Get (       void*,   uint32_t ) noexcept override;
    uint32_t  Put ( const void*,   uint32_t ) noexcept override;

  public:     // overridables from IFlatStream
    auto  PGet(                int64_t, uint32_t ) -> mtc::api<IByteBuffer> override;
    int   PGet( IByteBuffer**, int64_t, uint32_t ) override;
    auto  PGet(       void*,   int64_t, uint32_t ) noexcept -> uint32_t override;
    auto  PPut( const void*,   int64_t, uint32_t ) noexcept -> uint32_t override;
    auto  Seek( int64_t                          ) noexcept -> int64_t  override;
    auto  Size(                                  ) noexcept -> int64_t  override;
    auto  Tell(                                  ) noexcept -> int64_t  override;

  public:     // overridables from IFileStream
    api<IByteBuffer>  MemMap( int64_t, uint32_t ) override;
    bool              SetLen( int64_t ) noexcept override;
    bool              Sync() override;

  public:     // creation
    auto  Open( unsigned ) -> int;
    auto  Load() -> api<IByteBuffer>;

  protected:  // helpers
    void  Close();

  protected:  // name access
    auto  FileName() const -> const char* {  return (const char*)(this + 1);  }
    auto  FileName()        ->      char* {  return (char*)(this + 1);  }

  protected:  // variables
# if defined( _WIN32 )
    HANDLE  handle;
# else
    int     handle;
# endif   // _WIN32

  };

  template <class error>
  class FileStream<error>::filebuffer final: public IByteBuffer
  {
    implement_lifetime_control

    filebuffer( size_t l ): length( l ) {}

  protected:     // construction
    void  operator delete( void* p ) {  delete [] (char*)p;  }

  public:     // overridables
    const char* GetPtr() const noexcept override
      {  return buffer;  }
    size_t      GetLen() const noexcept override
      {  return length;  }
    int         SetBuf( const void*, size_t ) noexcept override
      {  return EINVAL;  }
    int           SetLen( size_t newlen ) noexcept override
      {  return newlen > length ? EINVAL : (length = newlen), 0;  }

  public:
    static  auto  create( size_t length ) -> api<filebuffer>
    {
      try
        {  return new ( new char[sizeof(filebuffer) + length - 1] ) filebuffer( length );  }
      catch ( const std::bad_alloc& xp )
        {  return error()( nullptr, xp );  }
      catch ( ... )
        {  return error()( nullptr, std::current_exception() );  }
    }

  protected:  // variables
    size_t  length;
    char    buffer[1];

  };

  unsigned  GetMemPageSize()
  {
# if defined( _WIN32 )
    SYSTEM_INFO syinfo;

    GetSystemInfo( &syinfo );
    return syinfo.dwAllocationGranularity;
# else
    return getpagesize();
# endif
  }

  // FileMemmap implementation

  template <class error>
  FileMemmap<error>::FileMemmap(): cchmem( 0 ), dwgran( GetMemPageSize() ), nshift( 0 ), maplen( 0 ), ptrmap( 0 )
  {
    win32_decl( handle = INVALID_HANDLE_VALUE );
  }

  template <class error>
  FileMemmap<error>::~FileMemmap()
  {
    if ( ptrmap != nullptr )
    {
      win32_decl( UnmapViewOfFile( ptrmap ) );
      win32_decl( CloseHandle( handle ) );
      posix_decl( munmap( ptrmap, maplen ) );
    }
  }

  template <class error>
  int   FileMemmap<error>::Create( FileStream<error>* stm, int64_t offset, uint32_t length )
  {
# if defined( _WIN32 )
    uint32_t  offshi = (uint32_t)(off >> 32);
    uint32_t  offslo = (uint32_t)(off);
    uint32_t  oalign = offslo / dwgran * dwgran;

  // create mapping view
    if ( (handle = CreateFileMapping( stm->handle, nullptr, PAGE_READONLY | SEC_COMMIT, 0, 0, nullptr )) == nullptr )
      return error()( file_error( strprintf( "Could not CreateFileMapping( '%s' ), error code 0x%08x",
        stm->FileName(), GetLastError() ) ) ), EFAULT;

    cchmem = len;
    nshift = offslo - oalign;
    maplen = cchmem + nshift;

  // create mapping pointer
    if ( (ptrmap = MapViewOfFile( handle, FILE_MAP_READ, offshi, oalign, (unsigned)maplen )) == nullptr )
    {
      CloseHandle( handle );
        handle = INVALID_HANDLE_VALUE;
      return error()( file_error( strprintf( "Could not MapViewOfFile( '%s' ) for the requested block, error code 0x%08x!",
        stm->FileName(), GetLastError() ) ) ), EFAULT;
    }

    return 0;
# else
    int64_t   stsize = stm->Size();
    int64_t   oalign = (offset / dwgran) * dwgran;  // смещение в файле как адрес отображения
      nshift = offset - oalign;                     // смещение в памяти до нужных байтов
    word64_t  blklen = std::min( word64_t(stsize - oalign), word64_t(length) + nshift );
                                                    // длина от базы до конца файла или нужная с выравниванием

    for ( blklen = ((blklen + dwgran - 1) / dwgran) * dwgran; blklen > (word64_t)(uint32_t)-1; )
      blklen -= dwgran;

    maplen = (uint32_t)blklen;

    if ( (cchmem = std::min( maplen - nshift, length )) != length && length != (uint32_t)-1 )
      return error()( EOVERFLOW, file_error( strprintf( "buffer too long to be mmap()'ed" ) ) );

    if ( (ptrmap = mmap( NULL, maplen, PROT_READ, MAP_SHARED | MAP_NORESERVE, stm->handle, oalign )) == MAP_FAILED )
    {
      int   nerror = errno;

      return error()( nerror, file_error( strprintf( "Could not MapViewOfFile( '%s' ) for the requested block, error code %u!",
        stm->FileName(), nerror ) ) );
    }
    return 0;
# endif
  }

  // CFileStream implementation

  template <class error>
  FileStream<error>::FileStream( const char* szname, size_t ccname )
  {
    strncpy( FileName(), szname, ccname + 1 )[ccname] = '\0';
      win32_decl( handle = INVALID_HANDLE_VALUE );
      posix_decl( handle = -1 );
  }

  template <class error>
  FileStream<error>::~FileStream()
  {
    Close();
  }

  template <class error>
  auto  FileStream<error>::Create( const char* szname, size_t ccname ) -> FileStream<error>*
  {
    size_t  cchstr = ccname != (size_t)-1 ? ccname : strlen( szname );
    size_t  nalloc = sizeof(FileStream<error>) + cchstr + 1;

    try
      {  return new ( new char[nalloc] ) FileStream<error>( szname, cchstr );  }
    catch ( const std::bad_alloc& xp )
      {  return error()( nullptr, xp );  }
    catch ( ... )
      {  return error()( nullptr, std::current_exception() );  }
  }

  template <class error>
  api<IByteBuffer>  FileStream<error>::MemMap( int64_t offset, uint32_t length )
  {
    auto  memmap = api<FileMemmap<error>>();

    try
      {  memmap = new FileMemmap<error>();  }
    catch ( const std::bad_alloc& xp )
      {  return error()( nullptr, xp );  }
    catch ( ... )
      {  return error()( nullptr, std::current_exception() );  }

    return memmap->Create( this, offset, length ) == 0 ? memmap.ptr() : nullptr;
  }

  template <class error>
  api<IByteBuffer>  FileStream<error>::Load()
  {
    api<filebuffer> buf;
    int64_t         len = Size();

    if ( len > std::numeric_limits<int64_t>::max() )
      return nullptr;

    if ( (buf = filebuffer::create( len )) == nullptr )
      return nullptr;

    return PGet( (char*)buf->GetPtr(), 0, (uint32_t)len ) == len ? buf.ptr() : nullptr;
  }

  template <class error>
  auto  FileStream<error>::PGet( int64_t off, uint32_t len ) -> mtc::api<IByteBuffer>
  {
    api<filebuffer> buffer;
    uint32_t        cbread;

    if ( (buffer = filebuffer::create( len )) == nullptr )
      return error()( nullptr, std::bad_alloc() );

    if ( (cbread = PGet( (char*)buffer->GetPtr(), off, len )) == (uint32_t)-1 )
    {
      return error()( nullptr, file_error( strprintf( "error reading file '%s' @" __FILE__ ":%u, error code %d",
        FileName(), LineId( __LINE__ ), errno ) ) );
    }

    return buffer->SetLen( cbread ), buffer.ptr();
  }

  template <class error>
  int       FileStream<error>::PGet( IByteBuffer** ppi, int64_t off, uint32_t len )
  {
    api<filebuffer> buffer;
    uint32_t        cbread;

    if ( (buffer = filebuffer::create( len )) == nullptr )
      return ENOMEM;

    if ( (cbread = PGet( (char*)buffer->GetPtr(), off, len )) == (uint32_t)-1 )
      return EACCES;

    buffer->SetLen( cbread );
      return (*ppi = buffer.ptr())->Attach(), 0;
  }

# if defined( _WIN32 )

  template <class error>
  uint32_t  FileStream<error>::Get( void* out, uint32_t len ) noexcept
  {
    DWORD     cbread;
    uint32_t  dwread = ReadFile( handle, out, len, &cbread, nullptr ) ? cbread : 0;

    debug_decl( DWORD uError = GetLastError() );

    return dwread;
  }

  template <class error>
  uint32_t  FileStream<error>::Put( const void* src, uint32_t len ) noexcept
  {
    DWORD nwrite;

    return WriteFile( handle, src, len, &nwrite, nullptr ) ? nwrite : 0;
  }

  template <class error>
  unsigned  FileStream<error>::PosGet( void* out, int64_t off, uint32_t len ) noexcept
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

  template <class error>
  unsigned  FileStream<error>::PosPut( const void* src, int64_t off, uint32_t len ) noexcept
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

  template <class error>
  int64_t   FileStream<error>::Seek( int64_t off ) noexcept
  {
    LONG    hioffs = (LONG)(off >> 32);
    LONG    looffs = (LONG)(off);
    DWORD   dwmove;

    return (dwmove = SetFilePointer( handle, looffs, &hioffs, FILE_BEGIN )) == INVALID_SET_FILE_POINTER ? (int64_t)-1 : off;
  }

  template <class error>
  int64_t   FileStream<error>::Size() noexcept
  {
    DWORD   looffs;
    DWORD   hioffs;

    return (looffs = GetFileSize( handle, &hioffs )) == INVALID_FILE_SIZE ? (int64_t)-1 : looffs | (((int64_t)hioffs) << 32);
  }

  template <class error>
  int64_t FileStream<error>::Tell() noexcept
  {
    LONG    hioffs = 0;
    DWORD   dwmove;

    return (dwmove = SetFilePointer( handle, 0, &hioffs, FILE_CURRENT )) == INVALID_SET_FILE_POINTER ? (int64_t)-1 : dwmove | (((int64_t)hioffs) << 32);
  }

  template <class error>
  bool  FileStream<error>::SetLen( int64_t len ) noexcept
  {
    LONG    hioffs = (LONG)(len >> 32);
    LONG    looffs = (LONG)(len);
    DWORD   dwmove;

    return (dwmove = SetFilePointer( handle, looffs, &hioffs, FILE_BEGIN )) != INVALID_SET_FILE_POINTER ?
      SetEndOfFile( handle ) : false;
  }

  template <class error>
  int     FileStream<error>::Open( unsigned dwmode )
  {
    auto      filename = FileName();
    DWORD     dwAccess;
    DWORD     dwDispos;
    DWORD     dwFlAttr = FILE_ATTRIBUTE_NORMAL;

    Close();

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
    handle = CreateFile( filename, dwAccess, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, dwDispos, dwFlAttr, NULL );
    return handle == INVALID_HANDLE_VALUE ? ENOENT : 0;
  }

  template <class error>
  void  FileStream<error>::Close()
  {
    if ( handle != INVALID_HANDLE_VALUE )
      CloseHandle( handle );
    handle = INVALID_HANDLE_VALUE;
  }

  template <class error>
  bool  FileStream<error>::Sync()
  {
    return true;
  }

# else

  template <class error>
  void  FileStream<error>::Close()
  {
    if ( handle != -1 )
      ::close( handle );
    handle = -1;
  }

  template <class error>
  int   FileStream<error>::Open( unsigned dwmode )
  {
  // check if stream is open; close it
    Close();

    return (handle = ::open( FileName(), dwmode, 0666 )) != -1 ? 0 : errno;
  }

  template <class error>
  uint32_t  FileStream<error>::Get( void* lpdata, uint32_t cbdata ) noexcept
  {
    return ::read( handle, lpdata, cbdata );
  }

  template <class error>
  uint32_t  FileStream<error>::Put( const void* lpdata, uint32_t cbdata ) noexcept
  {
    return ::write( handle, lpdata, cbdata );
  }

  template <class error>
  uint32_t  FileStream<error>::PGet( void* lpdata, int64_t offset, uint32_t length ) noexcept
  {
    return ::pread64( handle, lpdata, length, offset );
  }

  template <class error>
  uint32_t  FileStream<error>::PPut( const void* pvdata, int64_t offset, uint32_t length ) noexcept
  {
    return ::pwrite64( handle, pvdata, length, offset );
  }

  template <class error>
  int64_t   FileStream<error>::Seek( int64_t  offset ) noexcept
  {
    return ::lseek64( handle, offset, SEEK_SET );
  }

  template <class error>
  int64_t   FileStream<error>::Size() noexcept
  {
    int64_t curpos = ::lseek64( handle, 0, SEEK_CUR );
    int64_t curlen = ::lseek64( handle, 0, SEEK_END );

    ::lseek64( handle, curpos, SEEK_SET );
    return curlen;
  }

  template <class error>
  int64_t   FileStream<error>::Tell() noexcept
  {
    return ::lseek64( handle, 0, SEEK_CUR );
  }

  template <class error>
  bool  FileStream<error>::SetLen( int64_t len ) noexcept
  {
# ifdef __NR_truncate64
    int   nerror = ::ftruncate64( handle, len );
# else
    int   nerror = ::ftruncate( handle, len );
# endif

    if ( nerror != 0 )
    {
      return error()( false, file_error( strprintf( "could not ftruncate64( '%s', len ) @" __FILE__ ":%u, "
        "error code %d (%s)", FileName(), __LINE__, nerror, strerror( nerror ) ) ) );
    }
    return true;
  }

  template <class error>
  bool  FileStream<error>::Sync()
  {
#ifdef F_FULLFSYNC
    int   nerror = fcntl( handle, F_FULLFSYNC );
#else
    int   nerror = fdatasync( handle );
#endif

    if ( nerror != 0 )
    {
      return error()( false, file_error( strprintf( "could not fdatasync( '%s' ) @" __FILE__ ":%u, "
        "error code %d (%s)", FileName(), __LINE__, nerror, strerror( nerror ) ) ) );
    }
    return true;
  }

# endif

  /*
    Два варианта реакции на ошибку
  */
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

  template <class error>
  auto  openFileObject( const char* lpname, unsigned dwmode ) -> api<FileStream<error>>
  {
    api<FileStream<error>>  stream;
    int                     nerror;

    if ( lpname == nullptr || *lpname == '\0' )
      return (FileStream<error>*)error()( nullptr, std::invalid_argument( strprintf( "invalid argument: empty file name @" __FILE__ ":%u", __LINE__ ) ) );

    if ( (stream = FileStream<error>::Create( lpname )) == nullptr )
      return (FileStream<error>*)error()( nullptr, std::bad_alloc() );

    if ( (nerror = stream->Open( dwmode )) != 0 )
      return (FileStream<error>*)error()( nullptr, file_error( strprintf( "could not open file '%s' @" __FILE__ ":%u, error code %d", lpname, __LINE__, nerror ) ) );

    return stream;
  }

  api<IFileStream>  OpenFileStream( const char* szname, unsigned dwmode, const enable_exceptions_t& )
    {  return openFileObject<report_error_exception>( szname, dwmode ).ptr();  }

  api<IFileStream>  OpenFileStream( const char* szname, unsigned dwmode, const disable_exceptions_t& )
    {  return openFileObject<report_error_no_except>( szname, dwmode ).ptr();  }

  api<IFileStream>  OpenFileStream( const widechar* szname, unsigned dwmode, const disable_exceptions_t& )
    {  return OpenFileStream( utf::encode( szname ).c_str(), dwmode, disable_exceptions );  }

  api<IFileStream>  OpenFileStream( const widechar* szname, unsigned dwmode, const enable_exceptions_t& )
    {  return OpenFileStream( utf::encode( szname ).c_str(), dwmode, enable_exceptions );  }

  api<IByteBuffer>  LoadFileBuffer( const char* szname, const disable_exceptions_t& )
    {
      auto  lpfile = openFileObject<report_error_no_except>( szname, O_RDONLY );

      return lpfile != nullptr ? lpfile->Load() : nullptr;
    }

  api<IByteBuffer>  LoadFileBuffer( const widechar* szname, const disable_exceptions_t& )
    {
      auto  lpfile = openFileObject<report_error_no_except>( utf::encode( szname ).c_str(), O_RDONLY );

      return lpfile != nullptr ? lpfile->Load() : nullptr;
    }

  api<IByteBuffer>  LoadFileBuffer( const char* szname, const enable_exceptions_t& )
    {  return openFileObject<report_error_exception>( szname, O_RDONLY )->Load();  }

  api<IByteBuffer>  LoadFileBuffer( const widechar* szname, const enable_exceptions_t& )
    {  return LoadFileBuffer( utf::encode( szname ).c_str(), enable_exceptions );  }

  // wrappers

  api<IFileStream>  OpenFileStream( const std::string& sz, unsigned dwmode, const enable_exceptions_t& xp )
    {  return OpenFileStream( sz.c_str(), dwmode, xp );  }
  api<IFileStream>  OpenFileStream( const std::string& sz, unsigned dwmode, const disable_exceptions_t& xp )
    {  return OpenFileStream( sz.c_str(), dwmode, xp );  }

  api<IFileStream>  OpenFileStream( const std::basic_string<widechar>& sz, unsigned dwmode, const enable_exceptions_t& xp )
    {  return OpenFileStream( sz.c_str(), dwmode, xp );  }
  api<IFileStream>  OpenFileStream( const std::basic_string<widechar>& sz, unsigned dwmode, const disable_exceptions_t& xp )
    {  return OpenFileStream( sz.c_str(), dwmode, xp );  }

  api<IByteBuffer>  LoadFileBuffer( const std::string& sz, const enable_exceptions_t& xp )
    {  return LoadFileBuffer( sz.c_str(), xp );  }
  api<IByteBuffer>  LoadFileBuffer( const std::string& sz, const disable_exceptions_t& xp )
    {  return LoadFileBuffer( sz.c_str(), xp );  }

  api<IByteBuffer>  LoadFileBuffer( const std::basic_string<widechar>& sz, const enable_exceptions_t& xp )
    {  return LoadFileBuffer( sz.c_str(), xp );  }
  api<IByteBuffer>  LoadFileBuffer( const std::basic_string<widechar>& sz, const disable_exceptions_t& xp )
    {  return LoadFileBuffer( sz.c_str(), xp );  }

}  // mtc namespace
