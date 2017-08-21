# if !defined( __mtc_barrier_h__ )
# define __mtc_barrier_h__
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

    int   SetCount( int n ) {  threadCount = n;  }

  public:     // waiting
    void  wait()
      {
        if ( --threadCount == 0 )
          waitThreads.notify_all();
        else
          waitThreads.wait( std::unique_lock<std::mutex>( lockThreads ), [&](){  return threadCount == 0;  } );
      }
  };

}

# endif  // __mtc_barrier_h__
