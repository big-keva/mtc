# if !defined( __memmapFile_h__ )
# define __memmapFile_h__
# include <fcntl.h>

# if defined( _MSC_VER )
#   include <Windows.h>
# else
#   include <unistd.h>
#   include <sys/mman.h>
# endif  // _MSC_VER

# if defined( _MSC_VER )
#   define  win32_decl( expr )  expr
#   define  posix_decl( expr )
# else
#   define  win32_decl( expr )
#   define  posix_decl( expr )  expr
# endif  // _MSC_VER

namespace mtc
{

  class SystemFile
  {
    friend class MappedFile;

    win32_decl( HANDLE  fileno );
    posix_decl( int     fileno );
                int     nerror;

  public:     // create
    static  SystemFile  Open( const char* szpath, unsigned dwmode );

  public:     // open/reopen/close
    int         Close();
    int64_t     Size() const;

  public:     // information
    int         GetError() const  {  return nerror;  }
    const char* FilePath() const  {  return "unknown-file-path";  }
    operator bool       () const  {  return fileno != win32_decl( INVALID_HANDLE_VALUE )  posix_decl( -1 );  }

  protected:
    SystemFile& SetError( int e ) {  nerror = e;  return *this;  }

  };

  class MappedFile
  {
    word64_t    cchMem;
    unsigned    nShift;
    word64_t    mapLen;
    void*       ptrmap;
    int         nerror;

    win32_decl( HANDLE  handle );

  public:     // construction
    MappedFile();
    MappedFile( const MappedFile& );
   ~MappedFile();
    MappedFile& operator = ( const MappedFile& );

  private:    // utility constructors
    MappedFile& SetError( int e ) {  nerror = e;  return *this;  }
    void        UnmapMem();

  public:     // create
    static  MappedFile  Open( const SystemFile& onfile, int64_t offset = 0, word64_t length = (word64_t)-1 );

  public:     // information
    int         GetError() const  {  return nerror;               }
    operator bool       () const  {  return ptrmap != nullptr;    }
    operator const void*() const  {  return ptrmap;               }
    operator const char*() const  {  return (const char*)ptrmap;  }

  public:     // area access
    word64_t    GetLen() const  {  return cchMem;  }
    const void* GetPtr() const  {  return ptrmap;  }
          void* GetPtr()        {  return ptrmap;  }

  protected:  // helpers
    static unsigned  GetMemGran();

  };

  // SystemFile implementation

# if defined( _MSC_VER )
  inline  SystemFile  SystemFile::Open( const char* szpath, unsigned dwmode )
  {
    SystemFile  openfile;
    DWORD       dwAccess;
    DWORD       dwDispos;
    DWORD       dwFlAttr = FILE_ATTRIBUTE_NORMAL;

  /* create new name
    if ( (lppath = strdup( szname )) == NULL )
      return _u_err_nomem;
  */
  // detect access mode
    switch( dwmode & 0x000f )
    {
      case O_RDONLY:  dwAccess = GENERIC_READ;  break;
      case O_WRONLY:  dwAccess = GENERIC_WRITE; break;
      case O_RDWR:    dwAccess = GENERIC_WRITE | GENERIC_READ;  break;
      default:        dwAccess = 0;
    }

  // detect the disposition
    switch ( dwmode & 0x0300 )
    {
      case 0:
        dwDispos = OPEN_EXISTING;
        break;
      case O_CREAT:
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        dwDispos = (dwmode & O_EXCL ) != 0 ? CREATE_NEW : OPEN_ALWAYS;
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
    if ( (openfile.fileno = CreateFileA( szpath, dwAccess, FILE_SHARE_READ, NULL, dwDispos, dwFlAttr, NULL )) == INVALID_HANDLE_VALUE )
      return SystemFile().SetError( log_error( ENOENT, "Could not open file %s @%s:%u!", szpath, __FILE__, __LINE__ ) );
    return openfile;
  }

  inline  int64_t     SystemFile::Size() const
  {
    DWORD   looffs;
    DWORD   hioffs;

    if ( (looffs = GetFileSize( fileno, &hioffs )) == 0xFFFFFFFF )
      return (int64_t)-1;
    return looffs | (((int64_t)hioffs) << 32);
  }
# else
  inline  SystemFile  SystemFile::Open( const char* szpath, unsigned dwmode )
  {
    SystemFile  openfile;

    if ( (openfile.fileno = ::open( szpath, dwmode, 0666 )) == -1 )
      return SystemFile().SetError( log_error( ENOENT, "Could not open file %s @%s:%u, error code %d!",
        szpath, __FILE__, __LINE__, errno ) );
    return openfile;
  }

  inline  int64_t     SystemFile::Size() const
  {
    int64_t curpos = ::lseek( fileno, 0, SEEK_CUR );
    int64_t curlen = ::lseek( fileno, 0, SEEK_END );

    ::lseek( fileno, curpos, SEEK_SET );
    return curlen;
  }
# endif

  // MappedFile implementation

  inline  MappedFile::MappedFile(): cchMem( 0 ),
                                    nShift( 0 ),
                                    mapLen( 0 ),
                                    ptrmap( nullptr ),
                                    nerror( 0 )
  {
    win32_decl( handle = INVALID_HANDLE_VALUE );
  }

  inline  MappedFile::MappedFile( const MappedFile& m ): cchMem( m.cchMem ),
                                                         nShift( m.nShift ),
                                                         mapLen( m.mapLen ),
                                                         ptrmap( m.ptrmap ),
                                                         nerror( m.nerror )
  {
    win32_decl( handle = m.handle );
    win32_decl( ((MappedFile&)m).handle = INVALID_HANDLE_VALUE );
      ((MappedFile&)m).ptrmap = nullptr;
  }

  inline  MappedFile::~MappedFile()
  {
    UnmapMem();
  }

  inline  MappedFile& MappedFile::operator = ( const MappedFile& m )
  {
    UnmapMem();

    cchMem = m.cchMem;
    nShift = m.nShift;
    mapLen = m.mapLen;
    ptrmap = m.ptrmap;  ((MappedFile&)m).ptrmap = nullptr;
    nerror = m.nerror;
    
    win32_decl( handle = m.handle );
    win32_decl( ((MappedFile&)m).handle = INVALID_HANDLE_VALUE );

    return *this;
  }

  inline  void  MappedFile::UnmapMem()
  {
    if ( ptrmap != NULL )
      win32_decl( UnmapViewOfFile( ptrmap ) ) posix_decl( munmap( ptrmap, mapLen ) );

    win32_decl( if ( handle != INVALID_HANDLE_VALUE ) )
    win32_decl(   CloseHandle( handle ) );
  }

  inline  MappedFile  MappedFile::Open( const SystemFile& onfile, int64_t offset, word64_t length )
  {
    MappedFile  memmap;
    unsigned    dwGran = GetMemGran();

  // check if length is default; resize to real size of file
    if ( length == (word64_t)-1 )
      length = onfile.Size();

    if ( sizeof(size_t) < sizeof(length) && (length >> 32) != 0 )
      return memmap.SetError( log_error( EINVAL, "Mapping region too long @ %s:%u!", __FILE__, __LINE__ ) );

# if defined( WIN32 )
    word32_t  offshi = (word32_t)(offset >> 32);
    word32_t  offslo = (word32_t)(offset);
    word32_t  oalign = (offslo / dwGran) * dwGran;

  // create mapping view
    if ( (memmap.handle = CreateFileMapping( onfile.fileno, NULL, PAGE_READONLY | SEC_COMMIT, 0, 0, NULL )) == NULL )
      return memmap.SetError( log_error( EFAULT, "Could not CreateFileMapping() for \'%s\'!", onfile.FilePath() ) );

    memmap.cchMem = length;
    memmap.nShift = offslo - oalign;
    memmap.mapLen = memmap.cchMem + memmap.nShift;

  // create mapping pointer
    if ( (memmap.ptrmap = MapViewOfFile( memmap.handle, FILE_MAP_READ, offshi, oalign, (unsigned)memmap.mapLen )) == NULL )
      return MappedFile().SetError( log_error( ENOMEM, "Could not MapViewOfFile(), error code %u!", GetLastError() ) );
# else
    int64_t oalign = (offset / dwGran) * dwGran;

    memmap.cchMem = length;
    memmap.nShift = offset - oalign;
    memmap.mapLen = ((length + memmap.nShift + dwGran) / dwGran) * dwGran;

    if ( (memmap.ptrmap = mmap( NULL, memmap.mapLen, PROT_READ, MAP_SHARED, onfile.fileno, oalign )) == MAP_FAILED )
      return MappedFile().SetError( errno );
# endif

    return memmap;
  }
    
  inline  unsigned  MappedFile::GetMemGran()
  {
    win32_decl( SYSTEM_INFO syinfo );
    win32_decl( GetSystemInfo( &syinfo ) );
    win32_decl( return syinfo.dwAllocationGranularity );
    posix_decl( return getpagesize() );
  }

}

# endif  // __memmapFile_h__
