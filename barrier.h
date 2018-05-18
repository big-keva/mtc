# if !defined( __mtc_barrier_h__ )
# define __mtc_barrier_h__
# include <condition_variable>
# include <atomic>
# include <mutex>

namespace mtc
{

  class barrier
  {
    std::atomic_int         threadCount;
    std::mutex              lockThreads;
    std::condition_variable waitThreads;

  public:     // construction
    barrier( int threads ): threadCount( threads )  {}
    barrier( const barrier& ) = delete;
    barrier& operator = ( const barrier& ) = delete;

    int   SetCount( int n )
      {
        threadCount = n;
        waitThreads.notify_all();
      }

  public:     // waiting
    void  wait()
      {
        if ( --threadCount != 0 )
          {
            std::unique_lock<std::mutex>  brlock( lockThreads );

            waitThreads.wait( brlock, [&](){  return threadCount == 0;  } );
          }
        else
          waitThreads.notify_all();
      }
  };

}

# endif  // __mtc_barrier_h__
