# if !defined( __mtc_threadPool_hpp__ )
# define __mtc_threadPool_hpp__
# include <memory>
# include <chrono>
# include <functional>

namespace mtc {

  class ThreadPool
  {
    struct impl;

  public:
    enum: size_t
    {
      MaxQueueLen = 2048,
      ThreadLimit = 8
    };

    ThreadPool() = default;
    ThreadPool( const ThreadPool& ) = default;

    template <class _Rep, class _Period>
    bool  Insert( std::function<void()>, const std::chrono::duration<_Rep, _Period>& timeout );
    bool  Insert( std::function<void()>, unsigned timeout = unsigned(-1) );
    void  Cancel();

  protected:
    std::shared_ptr<impl> pool;

  };

  template <class _Rep, class _Period>
  bool  ThreadPool::Insert( std::function<void()> action, const std::chrono::duration<_Rep, _Period>& timeout )
  {
    return Insert( action, std::chrono::duration_cast<std::chrono::milliseconds>( timeout ).count() );
  }

}

# endif   // !__mtc_threadPool_hpp__
