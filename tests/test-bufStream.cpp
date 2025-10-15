# include "../bufStream.h"
# include "../exceptions.h"
# include "tmppath.h"
# include "../test-it-easy.hpp"
# include <cstdio>

TestItEasy::RegisterFunc  testBufStream( []()
{
  TEST_CASE( "mtc/bufStream" )
  {
    SECTION( "bufStream may be opened in both 'enable_exceptions' and 'disable_exceptions' modes" )
    {
      SECTION( "with 'disable_exceptions'," )
      {
        SECTION( "opening non-existing file returns nullptr" )
        {
          auto  s_name = std::string( "non-existing-file" );
          auto  w_name = mtc::utf::decode( "non-existing-file" );

          REQUIRE_NOTHROW( mtc::OpenBufStream( s_name.c_str(), O_RDONLY, 0x16, mtc::disable_exceptions ) );
                  REQUIRE( mtc::OpenBufStream( s_name,         O_RDONLY, 0x16, mtc::disable_exceptions ) == nullptr );
          REQUIRE_NOTHROW( mtc::OpenBufStream( w_name.c_str(), O_RDONLY, 0x16, mtc::disable_exceptions ) );
                  REQUIRE( mtc::OpenBufStream( w_name,         O_RDONLY, 0x16, mtc::disable_exceptions ) == nullptr );
        }

        SECTION( "existing file returns object interface" )
        {
          auto  tempname = GetTmpName();
            fclose( fopen( tempname.c_str(), "wb" ) );

          if ( REQUIRE( !tempname.empty() ) )
          {
            REQUIRE_NOTHROW( mtc::OpenBufStream( tempname, O_RDONLY, 0x16, mtc::disable_exceptions ) );
                    REQUIRE( mtc::OpenBufStream( tempname, O_RDONLY, 0x16, mtc::disable_exceptions ) != nullptr );

            remove( tempname.c_str() );
          }
        }
      }

      SECTION( "with 'enable-exceptions," )
      {
        SECTION( "opening non-existing file throws mtc::file_error" )
        {
          REQUIRE_EXCEPTION( mtc::OpenBufStream( "mtc-non-existing-file", O_RDONLY, 0x16, mtc::enable_exceptions ),
            mtc::file_error );
        }
        SECTION( "opening existing file returns object interface" )
        {
          auto  tempname = GetTmpName();
            fclose( fopen( tempname.c_str(), "wb" ) );

          if ( REQUIRE( !tempname.empty() ) )
          {
            if ( REQUIRE_NOTHROW( mtc::OpenBufStream( tempname, O_RDONLY, 0x16, mtc::enable_exceptions ) ) )
              REQUIRE( mtc::OpenBufStream( tempname, O_RDONLY, 0x16, mtc::enable_exceptions ) != nullptr );

            remove( tempname.c_str() );
          }
        }
      }
    }
    SECTION( "data may be sequentally written and read to-from the file insite the buffer" )
    {
      auto  fs = mtc::api<mtc::IFlatStream>();
      auto  tf = GetTmpName();

      if ( REQUIRE( !tf.empty() ) )
      {
        if ( REQUIRE_NOTHROW( fs = mtc::OpenBufStream( tf, O_CREAT + O_RDWR, 0x20, mtc::enable_exceptions ) )
          && REQUIRE( fs != nullptr ) )
        {
          int64_t nwrite;

          SECTION( "small data pieces may be written to the buffer" )
          {
            if ( REQUIRE_NOTHROW( nwrite = fs->Put( "some small data string", 22 ) ) )
              REQUIRE( nwrite == 22 );
            if ( REQUIRE_NOTHROW( nwrite = fs->Put( " list", 5 ) ) )
              REQUIRE( nwrite == 5 );
            SECTION( "overlapped buffer is flushed" )
            {
              if ( REQUIRE_NOTHROW( nwrite = fs->Put( " may be flushed to the disk", 27 ) ) && REQUIRE( nwrite == 27 ) )
              {
                SECTION( "stream may be repositioned and flushed" )
                {
                  if ( REQUIRE_NOTHROW( nwrite = fs->Seek( 5 ) ) && nwrite == 5 )
                  {
                    if ( REQUIRE_NOTHROW( nwrite = fs->Put( "other", 5 ) ) )
                      REQUIRE( nwrite == 5 );
                  }
                }
              }
            }
          }
        }
        fs = nullptr;
        remove( tf.c_str() );
      }
    }
  }
} );
