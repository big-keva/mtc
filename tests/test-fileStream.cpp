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
    }
  } );
      /*

void  TestLoadFileBuffer()
{
  auto  s_name = "non-existing-buffer";
  auto  w_name = mtc::utf::decode( s_name );

  assert( LoadFileBuffer( s_name, mtc::disable_exceptions ) == nullptr );

  try
  {
    LoadFileBuffer( s_name, mtc::enable_exceptions );
    assert( "exception is not thrown" == nullptr );
  }
  catch ( mtc::file_error& )  {}

  {
    auto  file = fopen( s_name, "wb" );
      fwrite( "test string", 11, 1, file );
    fclose( file );
  }

  assert( LoadFileBuffer( s_name, mtc::disable_exceptions ) != nullptr );
  assert( LoadFileBuffer( s_name, mtc::enable_exceptions ) != nullptr );

  assert( LoadFileBuffer( w_name.c_str(), mtc::enable_exceptions )->GetLen() == 11 );

  remove( s_name );
}
*/