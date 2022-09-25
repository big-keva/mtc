# include "../test-it-easy.hpp"
# include "../fileStream.h"
# include "../utf.hpp"

using namespace mtc;

TestItEasy::RegisterFunc  testFileStream( []()
  {
    TEST_CASE( "mtc/fileStream" )
    {
      SECTION( "FileStream may be opened without exceptions" )
      {
        auto  s_name = "/tmp/non-existing-file";
        auto  w_name = mtc::utf::decode( "/tmp/non-existing-file" );
        auto  stream = mtc::api<IFileStream>();

        REQUIRE_NOTHROW( stream = mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::disable_exceptions ) );
          REQUIRE( stream == nullptr );

        fclose( fopen( s_name, "wb" ) );

        REQUIRE_NOTHROW( stream = mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::disable_exceptions ) );
          REQUIRE( stream != nullptr );
        REQUIRE_NOTHROW( stream = mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::disable_exceptions ) );
          REQUIRE( stream != nullptr );

        remove( s_name );
      }
      SECTION( "FileStream may be opened with exceptions" )
      {
        auto  s_name = "/tmp/non-existing-file";
        auto  w_name = mtc::utf::decode( "/tmp/non-existing-file" );
        auto  stream = mtc::api<IFileStream>();

        REQUIRE_EXCEPTION( stream = mtc::OpenFileStream( s_name, O_RDONLY, mtc::enable_exceptions ), mtc::file_error );
        REQUIRE_EXCEPTION( stream = mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::enable_exceptions ), mtc::file_error );

        fclose( fopen( s_name, "wb" ) );

        REQUIRE_NOTHROW( stream = mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::enable_exceptions ) );
          REQUIRE( stream != nullptr );
        REQUIRE_NOTHROW( stream = mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::enable_exceptions ) );
          REQUIRE( stream != nullptr );

        remove( s_name );
      }
      SECTION( "LoadFileBuffer" )
      {
        auto  s_name = "/tmp/non-existing-buffer";
        auto  w_name = mtc::utf::decode( s_name );
        auto  pibuff = mtc::api<IByteBuffer>();

        REQUIRE_NOTHROW( LoadFileBuffer( s_name, mtc::disable_exceptions ) );
        REQUIRE_EXCEPTION( LoadFileBuffer( s_name, mtc::enable_exceptions ), mtc::file_error );

        {
          auto  lpfile = fopen( s_name, "wb" );
          fwrite( "TEST_BUFFER", 1, 11, lpfile );
          fclose( lpfile );
        }

        REQUIRE_NOTHROW( pibuff = LoadFileBuffer( s_name, mtc::disable_exceptions ) );
        REQUIRE( pibuff != nullptr );
        REQUIRE_NOTHROW( pibuff = LoadFileBuffer( s_name, mtc::enable_exceptions ) );
        REQUIRE( pibuff != nullptr );

        remove( s_name );
      }
    }
  } );
