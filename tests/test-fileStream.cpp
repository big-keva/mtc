# include "../fileStream.h"
# include "../utf.hpp"
# include "../test-it-easy.hpp"
# include <cstdio>

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

      SECTION( "fileStream created on existing file may be memmapped" )
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
      }
    }
  } );

int   main()
{
  return TestItEasy::Conclusion();
}
# if 0

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
# endif