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
# include "../bufStream.h"
# include "../exceptions.h"
# include "../wcsstr.h"
# include "../utf.hpp"
# include "miniBuffer.h"
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
#include <bits/codecvt.h>

#include "../platform.h"

# define LineId( arg )  "" #arg

namespace mtc
{
  template <class error>
  class BufStream final: public IFlatStream
  {
    std::atomic_long  refCount = 0;

  protected:
    BufStream( const char* szname, size_t ccname, size_t ccbuff );
    void  operator  delete( void* p ) {  delete[] (char*)p;  }

  public:      // creation
    static
    auto  Create( const char* szname, size_t ccname, size_t buflen ) -> BufStream*;

  // overridables from Iface
    long  Attach() override;
    long  Detach() override;

  // overridables from IStream
    uint32_t  Get (       void*,   uint32_t ) noexcept override;
    uint32_t  Put ( const void*,   uint32_t ) noexcept override;

  // overridables from IFlatStream
    auto  PGet(                int64_t, uint32_t ) -> mtc::api<IByteBuffer> override;
    int   PGet( IByteBuffer**, int64_t, uint32_t ) override;
    auto  PGet(       void*,   int64_t, uint32_t ) noexcept -> uint32_t override;
    auto  PPut( const void*,   int64_t, uint32_t ) noexcept -> uint32_t override;
    auto  Seek( int64_t                          ) noexcept -> int64_t  override;
    auto  Size(                                  ) noexcept -> int64_t  override;
    auto  Tell(                                  ) noexcept -> int64_t  override;

    int   Open( unsigned dwmode );

  protected:  // variables
# if defined( _WIN32 )
    HANDLE  handle = INVALID_HANDLE_VALUE;
# else
    int     handle = -1;
# endif   // _WIN32

    int64_t curpos = 0;

    bool    modify = false;

    char*   szname;
    char*   buforg;
    char*   buflim;
    char*   bufptr;
    char*   bufend;

  };

  // BufStream implementation

  template <class error>
  BufStream<error>::BufStream( const char* sz, size_t cc, size_t buflen ):
    szname( (char*)(this + 1) ),
    buforg( (char*)(szname + ((cc + 0x10) & ~0x0f)) ),
    buflim( buforg + buflen ),
    bufptr( buforg ),
    bufend( bufptr )
  {
    strncpy( szname, sz, cc )[cc] = 0;
  }

  template <class error>
  auto  BufStream<error>::Create( const char* sz, size_t cc, size_t buflen ) -> BufStream*
  {
    size_t  cchstr = cc != (size_t)-1 ? cc : strlen( sz );
    size_t  nalign = (buflen + 0x0f) & ~0x0f;
    size_t  nalloc = sizeof(BufStream<error>) + ((cchstr + 0x10) & ~0x0f) + nalign;

    try
      {  return new ( new char[nalloc] ) BufStream<error>( sz, cchstr, nalign );  }
    catch ( const std::bad_alloc& xp )
      {  return error()( nullptr, xp );  }
    catch ( ... )
      {  return error()( nullptr, std::current_exception() );  }
  }

  template <class error>
  long BufStream<error>::Attach()
  {
    return ++refCount;
  }

  template <class error>
  long  BufStream<error>::Detach()
  {
    long  refcount = --refCount;

    if ( refcount == 0 )
    {
      if ( posix_decl( handle != -1 ) win32_decl( handle != INVALID_HANDLE_VALUE ) )
      {
        if ( modify && ::pwrite64( handle, buforg, bufend - buforg, curpos ) < 0 )
        {
          error()( EFAULT, file_error( mtc::strprintf(
            "could not flush file buffers for '%s' @" __FILE__ ":" LineId( __LINE__ ), szname ) ) );
        }
      }
      posix_decl( ::close( handle ) );
      delete this;
    }
    return refcount;
  }

  template <class error>
  auto  BufStream<error>::PGet( int64_t off, uint32_t len ) -> mtc::api<IByteBuffer>
  {
    api<MiniBuffer<error>>  buffer;
    uint32_t                cbread;

    if ( (buffer = MiniBuffer<error>::Create( len )) == nullptr )
      return nullptr;

    if ( (cbread = PGet( (char*)buffer->GetPtr(), off, len )) == (uint32_t)-1 )
    {
      return error()( nullptr, file_error( strprintf( "error reading file '%s' @" __FILE__ ":%u, error code %d",
        szname, LineId( __LINE__ ), errno ) ) );
    }

    return buffer->SetLen( cbread ), buffer.ptr();
  }

  template <class error>
  int   BufStream<error>::PGet( IByteBuffer** ppi, int64_t off, uint32_t len )
  {
    api<MiniBuffer<error>>  buffer;
    uint32_t                cbread;

    if ( ppi == nullptr )
      return error()( EINVAL, std::invalid_argument( "invalid (null) outptr" ) );

    if ( (buffer = MiniBuffer<error>::Create( len )) == nullptr )
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
/*
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
*/

  template <class error>
  uint32_t  BufStream<error>::Get( void* lpdata, uint32_t cbdata ) noexcept
  {
    char* outorg = (char*)lpdata;
    char* outptr( outorg );
    char* outend( outorg + cbdata );

    while ( outptr < outend )
    {
      ssize_t cbcopy = std::min( bufend - bufptr, outend - outptr );

      // copy existing data
      while ( cbcopy-- > 0 )
        *outptr++ = *bufptr++;

      // check if no more data
      if ( bufptr != bufend )
        continue;

      // check if file is modified; flush current buffer
      if ( modify && ::pwrite64( handle, buforg, bufend - buforg, curpos ) < 0 )
        return error()( -1, file_error( "error flushing data" ) );

      // correct the internal file pointer
      curpos += bufend - buforg,
        bufptr = bufend = buforg,
          modify = false;

      // check if read big portion
      if ( outend - outptr > buflim - buforg )
      {
        if ( (cbcopy = ::pread64( handle, outptr, outend - outptr, curpos )) < 0 )
          return error()( -1, file_error( "error reading file" ) );
        curpos += cbcopy;
          return cbcopy;
      }

      // read subbuffer
      if ( (cbcopy = ::pread64( handle, buforg, buflim - buforg, curpos )) < 0 )
        return error()( -1, file_error( "error reading file" ) );
      if ( cbcopy == 0 )
        break;
      bufend = buforg + cbcopy;
    }
    return outptr - outorg;
  }

  template <class error>
  uint32_t  BufStream<error>::Put( const void* lpdata, uint32_t cbdata ) noexcept
  {
    auto  srcorg = static_cast<const char*>( lpdata );
    auto  srcptr( srcorg );
    auto  srcend( srcorg + cbdata );

    while ( srcptr < srcend )
    {
      ssize_t cbcopy = std::min( buflim - bufptr, srcend - srcptr );

      // copy to existing buffer space
      if ( cbcopy != 0 )
        for ( modify = true; cbcopy-- > 0; )
          *bufptr++ = *srcptr++;

      // check buffer end
      bufend = std::max( bufend, bufptr );

      // check if finished
      if ( bufptr != buflim )
        continue;

      // flush current buffer
      // check if file is modified; flush current buffer
      if ( modify && ::pwrite64( handle, buforg, buflim - buforg, curpos ) < 0 )
        return error()( -1, file_error( "error flushing data" ) );

      curpos += buflim - buforg,
        modify = false;

      // check if much to write
      if ( srcend - srcptr >= buflim - buforg )
      {
        if ( (cbcopy = ::pwrite64( handle, srcptr, srcend - srcptr, curpos )) < 0 )
          return error()( -1, file_error( "error flushing data" ) );
        curpos += cbcopy;
      }

      bufptr = bufend = buforg;
    }
    return srcptr - srcorg;
  }

  template <class error>
  uint32_t  BufStream<error>::PGet( void* lpdata, int64_t offset, uint32_t length ) noexcept
  {
    return ::pread64( handle, lpdata, length, offset );
  }

  template <class error>
  uint32_t  BufStream<error>::PPut( const void* pvdata, int64_t offset, uint32_t length ) noexcept
  {
    return ::pwrite64( handle, pvdata, length, offset );
  }

  template <class error>
  int64_t   BufStream<error>::Seek( int64_t  offset ) noexcept
  {
    if ( offset >= curpos && offset < curpos + bufend - buforg )
      return bufptr = buforg + (offset - curpos), offset;

    if ( modify && ::pwrite64( handle, buforg, bufend - buforg, curpos ) < 0 )
    {
      return error()( -1, file_error( mtc::strprintf(
        "could not flush file buffers for '%s' @" __FILE__ ":" LineId( __LINE__ ), szname ) ) );
    }
    if ( (curpos = ::lseek64( handle, offset, SEEK_SET )) < 0 )
    {
      return error()( -1, file_error( mtc::strprintf(
        "could not lseek64 for '%s' @" __FILE__ ":" LineId( __LINE__ ), szname ) ) );
    }
    bufptr = bufend = buforg;
    return curpos;
  }

  template <class error>
  int64_t   BufStream<error>::Size() noexcept
  {
    int64_t getpos = ::lseek64( handle, 0, SEEK_CUR );
    int64_t getlen = ::lseek64( handle, 0, SEEK_END );

    ::lseek64( handle, getpos, SEEK_SET );
    return getlen;
  }

  template <class error>
  int64_t   BufStream<error>::Tell() noexcept
  {
    return curpos + bufptr - buforg;
  }

  template <class error>
  int   BufStream<error>::Open( unsigned dwmode )
  {
    return (handle = ::open( szname, dwmode, 0666 )) != -1 ? 0 : errno;
  }

# endif

  // Реакция на ошибку: да или нет
  template <class Err>
  struct reactor;

  template <>
  struct reactor<enable_exceptions_t>
  {
    template <class result, class except>
    result  operator ()( const result&, const except& x )  {  throw x;  }
  };

  template <>
  struct reactor<disable_exceptions_t>
  {
    template <class result, class except>
    result  operator ()( result res, const except& )  {   return res;  }
  };

  template <class except>
  auto  OpenStream( const char* lpname, unsigned dwmode, size_t buflen, const except& ) -> api<BufStream<reactor<except>>>
  {
    api<BufStream<reactor<except>>> stream;
    int                             nerror;

    if ( lpname == nullptr || *lpname == '\0' )
      return reactor<except>()( nullptr, std::invalid_argument( strprintf(
        "invalid argument: empty file name @" __FILE__ ":%u", __LINE__ ) ) );

    if ( (stream = BufStream<reactor<except>>::Create( lpname, -1, buflen )) == nullptr )
      return reactor<except>()( nullptr, std::bad_alloc() );

    if ( (nerror = stream->Open( dwmode )) != 0 )
      return reactor<except>()( nullptr, file_error( strprintf(
        "could not open file '%s' @" __FILE__ ":%u, error code %d", lpname, __LINE__, nerror ) ) );

    return stream;
  }

  api<IFlatStream>  OpenBufStream( const char* szname, unsigned dwmode, size_t buflen, const disable_exceptions_t& )
  {
    return OpenStream( szname, dwmode, buflen, disable_exceptions ).ptr();
  }

  api<IFlatStream>  OpenBufStream( const char* szname, unsigned dwmode, size_t buflen, const enable_exceptions_t& )
  {
    return OpenStream( szname, dwmode, buflen, enable_exceptions ).ptr();
  }

  api<IFlatStream>  OpenBufStream( const widechar* szname, unsigned dwmode, size_t buflen, const disable_exceptions_t& )
  {
    return OpenStream( utf::encode( szname ).c_str(), dwmode, buflen, disable_exceptions ).ptr();
  }

  api<IFlatStream>  OpenFileStream( const widechar* szname, unsigned dwmode, size_t buflen, const enable_exceptions_t& )
    {  return OpenStream( utf::encode( szname ).c_str(), dwmode, buflen, enable_exceptions ).ptr();  }

  // wrappers

}  // mtc namespace
