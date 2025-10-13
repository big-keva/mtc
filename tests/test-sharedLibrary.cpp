# include "../sharedLibrary.hpp"
# include "../test-it-easy.hpp"
# include <cstdio>

# define __Q__(x) #x
# define QUOTE(x) __Q__(x)

TestItEasy::RegisterFunc  testSharedLibrary( []()
{
  TEST_CASE( "mtc/SharedLibrary" )
  {
    auto  lib = mtc::SharedLibrary();

    SECTION( "shared library may be loaded" )
    {
      SECTION( "if it is absent," )
      {
        SECTION( "either exception is thrown," )
        {
          REQUIRE_EXCEPTION( lib = mtc::SharedLibrary::Load( "undefined-shared-library-for-mtc-text.so" ),
            mtc::SharedLibrary::error );
        }
        SECTION( "or null library is returned" )
        {
          REQUIRE_NOTHROW( lib = mtc::SharedLibrary::Load( "undefined-shared-library-for-mtc-text.so",
            mtc::disable_exceptions ) );
          REQUIRE( lib == nullptr );
        }
      }

      auto  libpath = QUOTE(CMAKE_BINARY_DIR) "/libmtc-test-sharedLibrary-lib" QUOTE(CMAKE_DLL_SUFFIX);

      SECTION( "loading library " QUOTE(CMAKE_BINARY_DIR) "/libmtc-test-sharedLibrary-lib" QUOTE(CMAKE_DLL_SUFFIX) )
      {
        REQUIRE_NOTHROW( lib = mtc::SharedLibrary::Load( libpath ) );
      }
    }
    if ( lib != nullptr )
    {
      std::function<std::string(const std::string&)> func;

      SECTION( "absent functions are not found" )
      {
        REQUIRE_EXCEPTION( lib.Find( "absentFunc" ), mtc::SharedLibrary::error );
        REQUIRE_NOTHROW( lib.Find( "absentFunc", mtc::disable_exceptions ) );
        REQUIRE( lib.Find( "absentFunc", mtc::disable_exceptions ) == nullptr );
      }
      SECTION( "library function may be found by name" )
      {
        REQUIRE_NOTHROW( func = (std::string(*)(const std::string&))lib.Find( "ExportedStringFunc" ) );
        REQUIRE_NOTHROW( func != nullptr );
      }
      if ( func != nullptr )
      {
        SECTION( "library found function may be called" )
        {
          if ( REQUIRE_NOTHROW( func( "tester string" ) ) )
            REQUIRE( func( "tester string" ) == "string 'tester string' from library" );
        }
      }
    }
  }
} );