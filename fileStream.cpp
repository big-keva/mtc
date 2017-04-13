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
# include "fileStream.h"
# include "stdlog.h"
# include "autoptr.h"
# include <assert.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <errno.h>
# if defined( _WIN32 )
#   include <Windows.h>
# else
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
    virtual word32_t    GetLen(                       ) noexcept override {  return cchmem;  }
    virtual int         SetBuf( const void*, word32_t ) noexcept override {  return EINVAL;  }
    virtual int         SetLen( word32_t              ) noexcept override {  return EINVAL;  }

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
      filebuffer( word32_t l ): length( l ) {}
          
    public:     // overridables
      virtual const char* GetPtr() noexcept override {  return buffer;  }
      virtual word32_t    GetLen() noexcept override {  return length;  }
      virtual int         SetBuf( const void*, word32_t ) noexcept override {  return EINVAL;  }
      virtual int         SetLen( word32_t ) noexcept override {  return EINVAL;  }
    
    protected:  // variables
      word32_t  length;
      char      buffer[1];
    };

  public:     // construction
    FileStream();
   ~FileStream();

  public:     // overridables from IStream
    virtual word32_t  Get (       void*,   word32_t ) noexcept override;
    virtual word32_t  Put ( const void*,   word32_t ) noexcept override;

  public:     // overridables from IFlatStream
    virtual int       GetBuf( IByteBuffer**, int64_t, word32_t ) noexcept override;
    virtual word32_t  PosGet(       void*,   int64_t, word32_t ) noexcept override;
    virtual word32_t  PosPut( const void*,   int64_t, word32_t ) noexcept override;
    virtual int64_t   Seek  ( int64_t                          ) noexcept override;
    virtual int64_t   Size  (                                  ) noexcept override;
    virtual int64_t   Tell  (                                  ) noexcept override;       

  public:     // overridables from IFileStream
    virtual int       MemMap( IByteBuffer**, int64_t, word32_t ) noexcept override;

  public:     // creation
    int               Open( const char*, unsigned );
    IByteBuffer*      Load();

  protected:  // helpers
    void              Close();

  protected:  // variables
    win32_decl( HANDLE handle );
    posix_decl( int    handle );

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
# if defined( _WIN32 )
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

  int   FileStream::MemMap( IByteBuffer** ppi, int64_t off, word32_t len ) noexcept
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

    return PosGet( (char*)(new( buf.ptr() ) filebuffer( (word32_t)len ))->GetPtr(), 0, (word32_t)len ) == len ? buf.detach() : nullptr;
  }

  int       FileStream::GetBuf( IByteBuffer** ppi, int64_t off, word32_t len ) noexcept
  {
    _auto_<filebuffer>  buffer;

    if ( (buffer = (filebuffer*)malloc( sizeof(filebuffer) + len - 1 )) == nullptr )
      return ENOMEM;
    if ( PosGet( (char*)(new( buffer.ptr() ) filebuffer( len ))->GetPtr(), off, len ) != len )
      return EACCES;
    (*ppi = buffer.detach())->Attach();
      return 0;
  }

# if defined( _WIN32 )

  word32_t  FileStream::Get( void* out, word32_t len ) noexcept
  {
    DWORD     cbread;
    word32_t  dwread = ReadFile( handle, out, len, &cbread, nullptr ) ? cbread : 0;

    debug_decl( DWORD uError = GetLastError() );

    return dwread;
  }

  word32_t  FileStream::Put( const void* src, word32_t len ) noexcept
  {
    DWORD nwrite;

    return WriteFile( handle, src, len, &nwrite, nullptr ) ? nwrite : 0;
  }

  unsigned  FileStream::PosGet( void* out, int64_t off, word32_t len ) noexcept
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

  unsigned  FileStream::PosPut( const void* src, int64_t off, word32_t len ) noexcept
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

  void  FileStream::Close()
  {
    if ( handle != -1 )
      ::close( handle );
    handle = -1;
  }

  int   FileStream::Open( const char* szname, unsigned dwmode )
  {
  // check if stream is open; close it
    Close();

    return (handle = ::open( szname, dwmode, 0666 )) != -1 ? 0 : errno;
  }

  word32_t  FileStream::Get( void* lpdata, word32_t cbdata ) noexcept
  {
    return ::read( handle, lpdata, cbdata );
  }

  word32_t  FileStream::Put( const void* lpdata, word32_t cbdata ) noexcept
  {
    return ::write( handle, lpdata, cbdata );
  }

  word32_t  FileStream::PosGet( void* lpdata, int64_t offset, word32_t length ) noexcept
  {
    return ::pread( handle, lpdata, length, offset );
  }

  word32_t  FileStream::PosPut( const void* pvdata, int64_t offset, word32_t length ) noexcept
  {
    return ::pwrite( handle, pvdata, length, offset );
  }

  int64_t   FileStream::Seek( int64_t     offset ) noexcept
  {
    return ::lseek( handle, offset, SEEK_SET );
  }

  int64_t   FileStream::Size() noexcept
  {
    int64_t curpos = ::lseek( handle, 0, SEEK_CUR );
    int64_t curlen = ::lseek( handle, 0, SEEK_END );

    ::lseek( handle, curpos, SEEK_SET );
    return curlen;
  }

  int64_t   FileStream::Tell() noexcept
  {
    return ::lseek( handle, 0, SEEK_CUR );
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
