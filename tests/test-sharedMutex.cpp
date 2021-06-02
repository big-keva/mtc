# include "../recursive_shared_mutex.hpp"
# include <vector>
# include <assert.h>

mtc::recursive_shared_mutex   mx;
std::mutex                    _w;

void  TestLockIsRecursive()
{
  fprintf( stderr, ""
    "*****  unique_lock() is recursive()  *****\n" );

  auto  l1 = mtc::make_unique_lock( mx );
    fprintf( stderr, "#1\tgot unique_lock\n" );
  auto  l2 = mtc::make_unique_lock( mx );
    fprintf( stderr, "#1\tgot one more unique_lock\n" );
  auto  l3 = mtc::make_shared_lock( mx );
    fprintf( stderr, "#1\tgot shared_lock\n" );
  auto  l4 = mtc::make_unique_lock( mx );
    fprintf( stderr, "#1\tgot one more unique_lock\n" );
}

void  TestMultipleSharedLocks()
{
  std::vector<std::thread>  ts;

  fprintf( stderr, "*****  multiple threads can lock_shared  *****\n" );

  for ( auto i = 0; i != 10; ++i )
    ts.push_back( std::thread( [&, i]()
    {
      auto  ls = mtc::make_shared_lock( mx );
        fprintf( stderr, "#%d\tgot shared_lock()\n", i );
      std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    } ) );

  for ( auto& t: ts ) t.join();
}

void  TestUniqueLockBlocksAllLocks()
{
  fprintf( stderr, "*****  unique_lock() blocks other threads from unique_lock() and shared_lock()  *****\n" );
  fprintf( stderr, "#1\tafter unlock() in 100 ms first will be #2 with unique_lock, than #3 with shared_lock\n" );

  auto  ex = mtc::make_unique_lock( mx );

  auto  t1 = std::thread( [&]()
  {
    fprintf( stderr, "#2\ttry receive unique_lock()\n" );
      auto  sh = mtc::make_unique_lock( mx );
    fprintf( stderr, "#2\tgot unique_lock()\n" );
  } );
  auto  t2 = std::thread( [&]()
  {
    fprintf( stderr, "#3\ttry receive shared_lock()\n" );
      auto  sh = mtc::make_shared_lock( mx );
    fprintf( stderr, "#3\tgot shared_lock()\n" );
  } );

    std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    ex.unlock();

  t1.join();
  t2.join();
}

void  TestLockSharedLocksAttemptToUniqueLock()
{
  fprintf( stderr, "*****  shared_lock() blocks other threads from unique_lock()  *****\n" );
  fprintf( stderr, "#1\tafter unlock_shared() in 100 ms the unique lock will unblock\n" );

  auto  sh = mtc::make_shared_lock( mx );

  auto  tx = std::thread( [&]()
  {
    fprintf( stderr, "#2\ttry receive unique_lock()\n" );
      auto  sh = mtc::make_unique_lock( mx );
    fprintf( stderr, "#2\tgot unique_lock()\n" );
  } );

  std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
  sh.unlock();

  tx.join();
}

void  TestUniqueLockOverSharedLockBlocksSharedLocks()
{
  fprintf( stderr, "*****  unique_lock() request over shared_locks() blocks following attempts to lock_shared()  *****\n" );

  auto  sh = mtc::make_shared_lock( mx );
    fprintf( stderr, "#1\tcreated shared lock\n" );
  auto  th = std::thread( [&]()
    {
      fprintf( stderr, "#2\tkeeping unique_lock() request...\n" );
      auto  exlock = mtc::make_unique_lock( mx );

      fprintf( stderr, "#2\tgot unique_lock() request for 500ms...\n" );
      std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

      fprintf( stderr, "#2\tunloging unique_lock...\n" );
    } );

  std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

  fprintf( stderr, "#1\tunlocking shared_lock()\n" );

  sh.unlock();

  auto  tstart= std::chrono::steady_clock::now();
  std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
  fprintf( stderr, "#1\ttry get shared_lock again...\n" );

  sh.lock();

  fprintf( stderr, "#1\tgot shared_lock in %d ms...\n", std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::steady_clock::now() - tstart ).count() );

  th.join();
}

int   main()
{
  TestLockIsRecursive();
  TestMultipleSharedLocks();
  TestUniqueLockBlocksAllLocks();
  TestLockSharedLocksAttemptToUniqueLock();
  TestUniqueLockOverSharedLockBlocksSharedLocks();

  return 0;
}
