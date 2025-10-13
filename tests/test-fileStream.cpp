# include "../fileStream.h"
# include "../exceptions.h"
# include "../utf.hpp"
# include "../test-it-easy.hpp"
# include <cstdio>
# if defined( _WIN32 ) || defined( _WIN64 )
#   include <fileapi.h>
# else
#   include <unistd.h>
# endif   // _WIN32 || _WIN64

namespace {

  auto  GetTempFile() -> std::pair<int, std::string>
  {
    char  existing_name[1024];

# if defined( _WIN32 ) || defined( _WIN64 )
    char  template_path[1024];

    GetTempPath( sizeof(template_path), template_path );

    if ( GetTempFileName( template_path, "mtc-test-file", 0, existing_name ) )
      return { ::open( existing_name, O_RDWR, 0666 ), existing_name };
    else
      return { -1, "" };
# else
    auto  template_path = "/tmp/mtc-test-fileXXXXX";

    return { mkstemp( strcpy( existing_name, template_path ) ), existing_name };
# endif
  }

  auto  GetTempName() -> std::string
  {
    auto  temp = GetTempFile();

    return temp.first >= 0 ? ::close( temp.first ), temp.second : "";
  }

}

TestItEasy::RegisterFunc  testFileStream( []()
{
  TEST_CASE( "mtc/fileStream" )
  {
    SECTION( "fileStream may be opened in both 'enable_exceptions' and 'disable_exceptions' modes" )
    {
      SECTION( "with 'disable_exceptions'," )
      {
        SECTION( "opening non-existing file returns nullptr" )
        {
          auto  s_name = std::string( "non-existing-file" );
          auto  w_name = mtc::utf::decode( "non-existing-file" );

          REQUIRE_NOTHROW( mtc::OpenFileStream( s_name.c_str(), O_RDONLY, mtc::disable_exceptions ) );
                  REQUIRE( mtc::OpenFileStream( s_name, O_RDONLY, mtc::disable_exceptions ) == nullptr );
          REQUIRE_NOTHROW( mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::disable_exceptions ) );
                  REQUIRE( mtc::OpenFileStream( w_name, O_RDONLY, mtc::disable_exceptions ) == nullptr );
        }

        SECTION( "existing file returns object interface" )
        {
          auto  existing_file_name = GetTempName();

          REQUIRE_NOTHROW( mtc::OpenFileStream( existing_file_name, O_RDONLY, mtc::disable_exceptions ) );
                  REQUIRE( mtc::OpenFileStream( existing_file_name, O_RDONLY, mtc::disable_exceptions ) != nullptr );

          remove( existing_file_name.c_str() );
        }
      }

      SECTION( "with 'enable-exceptions," )
      {
        SECTION( "opening non-existing file throws mtc::file_error" )
        {
          REQUIRE_EXCEPTION( mtc::OpenFileStream( "mtc-non-existing-file", O_RDONLY, mtc::enable_exceptions ), mtc::file_error );
        }
        SECTION( "opening existing file returns object interface" )
        {
          auto  existing_file_name = GetTempName();

          REQUIRE_NOTHROW( mtc::OpenFileStream( existing_file_name, O_RDONLY, mtc::enable_exceptions ) );
                  REQUIRE( mtc::OpenFileStream( existing_file_name, O_RDONLY, mtc::enable_exceptions ) != nullptr );

          remove( existing_file_name.c_str() );
        }
      }
    }

    SECTION( "fileStream created on existing file may be MemMap()'ed" )
    {
      auto  existing_file_name = GetTempName();

      {
        auto  lpfile = fopen( existing_file_name.c_str(), "wb" );
          REQUIRE( lpfile != nullptr );
        fwrite( existing_file_name.c_str(), 1, existing_file_name.length(), lpfile );
          fclose( lpfile );
      }
      auto  stream = mtc::api<mtc::IFileStream>();
      auto  memmap = mtc::api<mtc::IByteBuffer>();

      REQUIRE_NOTHROW( stream = mtc::OpenFileStream( existing_file_name, O_RDONLY, mtc::enable_exceptions ) );
      if ( REQUIRE( stream != nullptr ) )
      {
        if ( REQUIRE_NOTHROW( memmap = stream->MemMap( 0, existing_file_name.length() ) )
          && REQUIRE( memmap != nullptr ) )
             REQUIRE( memcmp( memmap->GetPtr(), existing_file_name.c_str(), existing_file_name.length() ) == 0 );
      }

      remove( existing_file_name.c_str() );
    }

    SECTION( "Long files may be MemMap()'ed" )
    {
      auto  existing_file = GetTempName();

      {
        auto  fd = ::open( existing_file.c_str(), O_CREAT + O_RDWR, 0666 );
          REQUIRE( fd != -1 );
        REQUIRE( ::lseek64( fd, (uint64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 1, SEEK_SET ) == (int64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 1 );
        REQUIRE( ::write( fd, "a", 1 ) == 1 );
        ::close( fd );
      }
      auto  stream = mtc::api<mtc::IFileStream>();
      auto  memmap = mtc::api<mtc::IByteBuffer>();

      if ( REQUIRE_NOTHROW( stream = mtc::OpenFileStream( existing_file, O_RDONLY, mtc::enable_exceptions ) )
        && REQUIRE( stream != nullptr ) )
      {
        if ( REQUIRE_NOTHROW( memmap = stream->MemMap( 10, (mtc::word32_t)-1 ) )
          && REQUIRE( memmap != nullptr ) )
        {
          REQUIRE( memmap->GetLen() == (uint64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 10 );
          REQUIRE( memmap->GetPtr() != nullptr );
          REQUIRE( memmap->GetPtr()[(uint64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 11] == 'a' );
        }
      }

      remove( existing_file.c_str() );
    }
  }
  TEST_CASE( "mtc/fileBuffer" )
  {
    SECTION( "file buffer may be loaded" )
    {
      SECTION( "loading without existing file results error" )
      {
        SECTION( "with enabled exceptions throws file_error" )
        {
          REQUIRE_EXCEPTION( mtc::LoadFileBuffer( "non-existing-file", mtc::enable_exceptions ), mtc::file_error );
        }
        SECTION( "with disables exceptions returns nullptr" )
        {
          mtc::api<mtc::IByteBuffer> buffer;

          if ( REQUIRE_NOTHROW( buffer = mtc::LoadFileBuffer( "non-existing-file", mtc::disable_exceptions ) ) )
            REQUIRE( buffer == nullptr );
        }
      }
      SECTION( "loading of existing file returns byte buffer" )
      {
        auto  tmps = GetTempName();

        {
          auto  file = fopen( tmps.c_str(), "wb" );
            fwrite( "test string", 11, 1, file );
          fclose( file );
        }

        SECTION( "it may be named as string" )
        {
          if ( REQUIRE_NOTHROW( LoadFileBuffer( tmps, mtc::enable_exceptions ) ) )
            REQUIRE( LoadFileBuffer( tmps, mtc::enable_exceptions ) != nullptr );
        }
        SECTION( "and as widestring" )
        {
          if ( REQUIRE_NOTHROW( LoadFileBuffer( mtc::utf::decode( tmps ), mtc::enable_exceptions ) ) )
            REQUIRE( LoadFileBuffer( mtc::utf::decode( tmps ), mtc::enable_exceptions ) != nullptr );
        }

        remove( tmps.c_str() );
      }
    }
  }
} );
