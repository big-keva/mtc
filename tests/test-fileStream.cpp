# include "../fileStream.h"
# include "../utf.hpp"
# include "../test-it-easy.hpp"
# include <cstdio>
# include <unistd.h>

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
          const char  existing_file_name[] = "/tmp/new-existing-file";

          fclose( fopen( existing_file_name, "wb" ) );

          REQUIRE_NOTHROW( mtc::OpenFileStream( existing_file_name, O_RDONLY, mtc::disable_exceptions ) );
                  REQUIRE( mtc::OpenFileStream( existing_file_name, O_RDONLY, mtc::disable_exceptions ) != nullptr );

          remove( existing_file_name );
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
          fclose( fopen( "/tmp/mtc-existing-file", "wb" ) );

          REQUIRE_NOTHROW( mtc::OpenFileStream( "/tmp/mtc-existing-file", O_RDONLY, mtc::enable_exceptions ) );
                  REQUIRE( mtc::OpenFileStream( "/tmp/mtc-existing-file", O_RDONLY, mtc::enable_exceptions ) != nullptr );

          remove( "/tmp/mtc-existing-file" );
        }
      }
    }

    SECTION( "fileStream created on existing file may be MemMap()'ed" )
    {
      const char  existing_file[] = "/tmp/mtc-existing-file-for-memmap";

      {
        auto  lpfile = fopen( existing_file, "wb" );
          REQUIRE( lpfile != nullptr );
        fwrite( existing_file, 1, strlen( existing_file ), lpfile );
          fclose( lpfile );
      }
      auto  stream = mtc::api<mtc::IFileStream>();
      auto  memmap = mtc::api<mtc::IByteBuffer>();

      REQUIRE_NOTHROW( stream = mtc::OpenFileStream( existing_file, O_RDONLY, mtc::enable_exceptions ) );
              REQUIRE( stream != nullptr );

      REQUIRE_NOTHROW( memmap = stream->MemMap( 0, strlen( existing_file ) ) );
              REQUIRE( memmap != nullptr );

      remove( existing_file );
    }

    SECTION( "Long files may be MemMap()'ed" )
    {
      const char  existing_file[] = "/tmp/mtc-existing-file-for-memmap64";

      {
        auto  fd = ::open( existing_file, O_CREAT + O_RDWR, 0666 );
          REQUIRE( fd != -1 );
        REQUIRE( ::lseek( fd, (uint64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 1, SEEK_SET ) == (int64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 1 );
        REQUIRE( ::write( fd, "a", 1 ) == 1 );
        ::close( fd );
      }
      auto  stream = mtc::api<mtc::IFileStream>();
      auto  memmap = mtc::api<mtc::IByteBuffer>();

      REQUIRE_NOTHROW( stream = mtc::OpenFileStream( existing_file, O_RDONLY, mtc::enable_exceptions ) );
              REQUIRE( stream != nullptr );

      REQUIRE_NOTHROW( memmap = stream->MemMap( 10, (mtc::word32_t)-1 ) );
              REQUIRE( memmap != nullptr );
              REQUIRE( memmap->GetLen() >= (uint64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 1 );
              REQUIRE( memmap->GetPtr() != nullptr );
              REQUIRE( memmap->GetPtr()[(uint64_t)4 * 1024 * 1024 * 1024 - 55 * 1024 - 11] == 'a' );

      remove( existing_file );
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
        auto  tmps = "/tmp/mtc-test-buffer-temporary-filr";

        {
          auto  file = fopen( tmps, "wb" );
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

        remove( tmps );
      }
    }
  }
} );
