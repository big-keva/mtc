# include "../byteBuffer.h"
# include "../test-it-easy.hpp"
# include <cstdio>

using namespace mtc;

# if !defined( _WIN32 ) && !defined( _WIN64 )
# include <dlfcn.h>

/*
 * disabling memory allocator for tests
 */
void* (*rtl_malloc)( size_t ) = nullptr;
void* (*rtl_calloc)( size_t, size_t ) = nullptr;
volatile  size_t  limit_size = (size_t)-1;
volatile  bool    disable_mm = false;

extern "C" void* malloc( size_t size )
{
  if ( !disable_mm )
  {
    if ( size >= limit_size )
      return (errno = ENOMEM), nullptr;

    if ( rtl_malloc == nullptr)
    {
      disable_mm = true;
      rtl_malloc = (dlerror(), (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc" ));
      disable_mm = false;
    }
    return rtl_malloc( size );
  }
  return nullptr;
}

extern "C" void* calloc( size_t num, size_t size )
{
  if ( !disable_mm )
  {
    if ( size >= limit_size )
      return (errno = ENOMEM), nullptr;

    if ( rtl_calloc == nullptr)
    {
      disable_mm = true;
      rtl_calloc = (dlerror(), (void*(*)(size_t, size_t))dlsym(RTLD_NEXT, "calloc" ));
      disable_mm = false;
    }
    return rtl_calloc( num, size );
  }
  return nullptr;
}

# endif   // !_WIN32 && !_WIN64

TestItEasy::RegisterFunc  testBuffer( []()
  {
    TEST_CASE( "mtc/byteBuffer" )
    {
      api<IByteBuffer>  buffer;

      SECTION( "buffers may be created in old-style" )
      {
        SECTION( "if enougn memory, buffers are created" )
        {
          REQUIRE_NOTHROW( CreateByteBuffer( buffer = nullptr ) );  // as IByteBuffer**
            REQUIRE( buffer != nullptr );
          REQUIRE( CreateByteBuffer( buffer = nullptr ) == 0 );  // as IByteBuffer**
            REQUIRE( buffer != nullptr );
        }
# if !defined( _WIN32 ) && !defined( _WIN64 )
        SECTION( "if not enough memory, it returns errors" )
        {
          int   nerror;

          limit_size = 0;
            REQUIRE_NOTHROW( nerror = CreateByteBuffer( buffer = nullptr ) );  // as IByteBuffer**
          limit_size = 512 * 1024;

          REQUIRE( nerror == ENOMEM );
          REQUIRE_NOTHROW( nerror = CreateByteBuffer( (IByteBuffer**)(buffer = nullptr), 1024 * 1024 * 1024 ) );  // as IByteBuffer**
          REQUIRE( nerror == ENOMEM );

          limit_size = (size_t)-1;
        }
# endif   // !_WIN32 && !_WIN64
      }
      SECTION( "buffers may be created in new style with exceptions" )
      {
        SECTION( "if enougn memory, buffers are created" )
        {
          REQUIRE_NOTHROW( buffer = CreateByteBuffer( enable_exceptions ) );  // as IByteBuffer**
            REQUIRE( buffer != nullptr );
          REQUIRE_NOTHROW( buffer = CreateByteBuffer( 1024 * 1024, enable_exceptions ) );  // as IByteBuffer**
            REQUIRE( buffer != nullptr );
        }
# if !defined( _WIN32 ) && !defined( _WIN64 )
        SECTION( "if not enough memory, it throws exceptions" )
        {
          limit_size = 0;
          REQUIRE_EXCEPTION( buffer = CreateByteBuffer( enable_exceptions ), std::bad_alloc );  // as IByteBuffer**
          limit_size = 512 * 1024;
          REQUIRE_EXCEPTION( buffer = CreateByteBuffer( 1024 * 1024 * 1024, enable_exceptions ), std::bad_alloc );  // as IByteBuffer**
          limit_size = (size_t)-1;
        }
# endif   // !_WIN32 && !_WIN64
      }
      SECTION( "buffers may be created in new style without exceptions" )
      {
        SECTION( "if enougn memory, buffers are created" )
        {
          REQUIRE_NOTHROW( buffer = CreateByteBuffer( disable_exceptions ) );  // as IByteBuffer**
            REQUIRE( buffer != nullptr );
          REQUIRE_NOTHROW( buffer = CreateByteBuffer( 1024 * 1024, disable_exceptions ) );  // as IByteBuffer**
            REQUIRE( buffer != nullptr );
        }
# if !defined( _WIN32 ) && !defined( _WIN64 )
        SECTION( "if not enough memory, it returns nullptr" )
        {
          limit_size = 0;
          REQUIRE_NOTHROW( buffer = CreateByteBuffer( disable_exceptions ) );  // as IByteBuffer**
          limit_size = 512 * 1024;
          REQUIRE( buffer == nullptr );

          REQUIRE_NOTHROW( buffer = CreateByteBuffer( 1024 * 1024 * 1024, disable_exceptions ) );  // as IByteBuffer**
          REQUIRE( buffer == nullptr );

          limit_size = (size_t)-1;
        }
# endif   // !_WIN32 && !_WIN64
      }
    }
  } );
