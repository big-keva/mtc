# include "../recursive_shared_mutex.hpp"
# include <vector>
# include <assert.h>

int   main()
{
  mtc::recursive_shared_mutex   mx;
  std::mutex                    _m;
  std::condition_variable       cv;

// test lock() in different threads
  {
    auto  xc = mtc::make_unique_lock( mx );
    auto  x2 = mtc::make_unique_lock( mx );   // recursive
    auto  x3 = mtc::make_unique_lock( mx );   // recursive
    auto  xs = std::string( "start value" );
    auto  _l = mtc::make_unique_lock( _m );

    fprintf( stderr, "set start string value = '%s'\n", xs.c_str() );

    auto  th = std::thread( [&]()
    {
      auto  lc = mtc::make_unique_lock( mx );

      fprintf( stderr, "patch string value to '%s'\n", (xs = "changed value").c_str() );

      cv.notify_one();
    } );

    assert( xs == "start value" );

    fprintf( stderr, "wait 1 second\n" );
      std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

    fprintf( stderr, "unlock first lock...\n" );
      xc.unlock();
    fprintf( stderr, "unlock second lock...\n" );
      x2.unlock();
    fprintf( stderr, "unlock third lock...\n" );
      x3.unlock();

    fprintf( stderr, "wait for patch...\n" );

    cv.wait( _l );

    assert( xs == "changed value" );

    th.join();
  }

// test lock_shared()
  {
    fprintf( stderr, "multiple threads can lock_shared...\n" );
    {
      std::vector<std::thread>  ts;

      for ( auto i = 0; i != 10; ++i )
        ts.push_back( std::thread( [&, i]()
        {
          auto  ls = mtc::make_shared_lock( mx );
          fprintf( stderr, "\tgot lock %u\n", i );
        } ) );

      while ( ts.size() != 0 )
      {
        ts.back().join();
        ts.pop_back();
      }
    }
    fprintf( stderr, "OK\n" );
  }

// while lock()ed, lock_shared() blocks
  {
    auto  ex = mtc::make_unique_lock( mx );
    auto  _l = mtc::make_unique_lock( _m );
    auto  th = std::thread( [&]()
    {
      auto  lc = mtc::make_shared_lock( mx );
      auto  _w = mtc::make_unique_lock( _m );

      fprintf( stderr, "2) received shared lock, notify main thread\n" );
        cv.notify_one();
    } );

    fprintf( stderr, "1) unique-locked, unlock\n" );
      ex.unlock();
      cv.wait( _l );
    fprintf( stderr, "3) waiting for thread finish\n" );

    th.join();
  }
  /*
  auto  l1 = mtc::make_shared_lock(  mx );

  auto  th = std::thread( [&]()
  {
    auto  lx = mtc::make_unique_lock( mx );

    fprintf( stderr, "unique\n" );
  } );

  auto  l2 = mtc::make_shared_lock(  mx );

  l1.unlock();
  l2.unlock();

  fprintf( stderr, "unlocked\n" );

  th.join();
  */
  return 0;
}
