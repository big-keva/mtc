# include "../threadPool.hpp"
# include "../recursive_shared_mutex.hpp"
# include "../test-it-easy.hpp"
# include <condition_variable>
# include <thread>
# include <memory>
# include <atomic>
# include <list>
# include <mutex>

TestItEasy::RegisterFunc  testThreadPool( []()
{
  TEST_CASE( "ThreadPool" )
  {
    SECTION( "actions are added" )
    {
      mtc::ThreadPool pool;
      std::atomic_int test = 0;

      REQUIRE_NOTHROW( pool.Insert( [&]()
        {  std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );  ++test;  } ) );
      REQUIRE_NOTHROW( pool.Insert( [&]()
        {  std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );  ++test;  } ) );
      REQUIRE_NOTHROW( pool.Insert( [&]()
        {  std::this_thread::sleep_for( std::chrono::milliseconds( 300 ) );  ++test;  } ) );

      REQUIRE( test == 0 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 150 ) );
      REQUIRE( test == 1 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
      REQUIRE( test == 2 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
      REQUIRE( test == 3 );
    }
  }
} );