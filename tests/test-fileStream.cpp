# include "../fileStream.h"
# include "../utf.hpp"

void  TestOpenFileStreamNoExcept()
{
  auto  s_name = "non-existing-file";
  auto  w_name = mtc::utf::decode( "non-existing-file" );

  assert( mtc::OpenFileStream( s_name, O_RDONLY, mtc::disable_exceptions ) == nullptr );
  assert( mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::disable_exceptions ) == nullptr );

  fclose( fopen( s_name, "wb" ) );

  assert( mtc::OpenFileStream( s_name, O_RDONLY, mtc::disable_exceptions ) != nullptr );
  assert( mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::disable_exceptions ) != nullptr );

  remove( s_name );
}

void  TestOpenFileStreamWithExcept()
{
  auto  s_name = "non-existing-file";
  auto  w_name = mtc::utf::decode( s_name );

  try
  {
    mtc::OpenFileStream( s_name, O_RDONLY, mtc::enable_exceptions );
    assert( "exception is not thrown" == nullptr );
  }
  catch ( mtc::file_error& )  {}

  try
  {
    mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::enable_exceptions );
    assert( "exception is not thrown" == nullptr );
  }
  catch ( mtc::file_error& )  {}

  fclose( fopen( s_name, "wb" ) );

  assert( mtc::OpenFileStream( s_name, O_RDONLY, mtc::enable_exceptions ) != nullptr );
  assert( mtc::OpenFileStream( w_name.c_str(), O_RDONLY, mtc::enable_exceptions ) != nullptr );

  remove( s_name );
}

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

int   main( int argc, char* argv[] )
{
  TestOpenFileStreamNoExcept();
  TestOpenFileStreamWithExcept();
  TestLoadFileBuffer();
  return 0;
}
