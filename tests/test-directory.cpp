# include "../test-it-easy.hpp"
# include "../directory.h"

using namespace mtc;

TestItEasy::RegisterFunc  testDirectory( []()
  {
    TEST_CASE( "mtc/directory" )
    {
      directory         dir;
      directory::entry  ent;

      SECTION( "current directory may be opened anyway" )
      {
        REQUIRE_NOTHROW( dir = directory::Open( "./*", directory::attr_any ) );
        REQUIRE( dir != false );

        SECTION( "it contains at least one directory entry" )
        {
          REQUIRE( (ent = dir.Get()) != false );
          REQUIRE( ent.folder() == std::string( "./" ) );
        }
      }
    }
  } );
