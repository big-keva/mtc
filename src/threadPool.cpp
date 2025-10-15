# include "../threadPool.hpp"
# include "../recursive_shared_mutex.hpp"
# include <condition_variable>
# include <thread>
# include <atomic>
# include <mutex>
# include <list>

# if defined( _MSC_VER )
#   define pthread_self( ... )
#   define pthread_setname_np( ... )
# endif   // _MSC_VER

namespace mtc {

  struct ThreadPool::impl
  {
    size_t                            maxQueueLen = MaxQueueLen;
    size_t                            threadLimit = ThreadLimit;
    std::list<std::function<void()>>  actionQueue;
    std::mutex                        actionMutex;
    std::atomic_ulong                 threadAvail;
    std::atomic_ulong                 threadCount;

    volatile bool                     canContinue = true;
    std::condition_variable           syncronizer;

   ~impl();

    bool  Insert( std::function<void()>, unsigned ms );
    void  Cancel();
    void  Thread();

  };

  auto  ThreadPool::SetQueueSize( size_t size ) -> ThreadPool&
  {
    if ( pool == nullptr )
      pool = std::make_shared<impl>();
    pool->maxQueueLen = size;
      return *this;
  }

  auto  ThreadPool::SetThreadLimit( size_t limit ) -> ThreadPool&
  {
    if ( pool == nullptr )
      pool = std::make_shared<impl>();
    pool->threadLimit = limit;
      return *this;
  }

  bool  ThreadPool::Insert( std::function<void()> fn, unsigned tm  )
  {
    if ( pool == nullptr )
      pool = std::make_shared<impl>();
    return pool->Insert( fn, tm );
  }

  void  ThreadPool::Cancel()
  {
    if ( pool != nullptr )
      pool->Cancel();
  }

  ThreadPool::impl::~impl()
  {
    try {  Cancel();  }
    catch ( ... ) {}
    actionQueue.clear();
  }

  bool  ThreadPool::impl::Insert( std::function<void()> fn, unsigned tm )
  {
    if ( canContinue )
    {
      auto  canInsert = [&](){  return !canContinue || actionQueue.size() < maxQueueLen;  };
      auto  timeLimit = std::chrono::steady_clock::now() + std::chrono::milliseconds( tm );
      auto  waitMutex = mtc::make_unique_lock( actionMutex, std::defer_lock );

      if ( threadAvail == 0 && threadCount < threadLimit )
        std::thread( &impl::Thread, this ).detach();

      waitMutex.lock();

      if ( syncronizer.wait_until( waitMutex, timeLimit, canInsert ) && canContinue )
      {
        actionQueue.push_back( std::move( fn ) );
        syncronizer.notify_one();
        return true;
      }
    }
    return false;
  }

  void  ThreadPool::impl::Cancel()
  {
    auto  waitMutex = mtc::make_unique_lock( actionMutex );

    canContinue = false;
    syncronizer.notify_all();

    syncronizer.wait( waitMutex, [this](){  return threadCount == 0;  } );
  }

  void  ThreadPool::impl::Thread()
  {
    auto  waitMutex = mtc::make_unique_lock( actionMutex, std::defer_lock );
    auto  waitLimit = std::chrono::seconds( 30 );

    pthread_setname_np( pthread_self(), "mtc::poolthread" );

    ++threadAvail;
    ++threadCount;

    for ( waitMutex.lock(); canContinue; )
    {
      syncronizer.wait_for( waitMutex, waitLimit, [this]()
        {  return !canContinue || actionQueue.size() != 0;  } );

      if ( canContinue && !actionQueue.empty() )
      {
        auto  action = actionQueue.front();

        actionQueue.pop_front();
        waitMutex.unlock();
        syncronizer.notify_one();
          --threadAvail;
            action();
          ++threadAvail;
        waitMutex.lock();
      }
    }
    --threadAvail;
    --threadCount;
      syncronizer.notify_all();
  }

}
