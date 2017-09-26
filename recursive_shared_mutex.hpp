# pragma once
# if !defined( __mtc_mutex__ )
# define __mtc_mutex__
# include <mutex>
# include <condition_variable>
# include <thread>

namespace mtc
{

  class recursive_shared_mutex
  {
    std::recursive_mutex        thrdLock;
    std::condition_variable_any w_Reader;
    std::condition_variable_any w_Writer;
    int                         nReaders = 0;
    int                         nWriters = 0;
    std::thread::id             writerId;

  public:     // constructors
    recursive_shared_mutex()  {}
    recursive_shared_mutex( const recursive_shared_mutex& ) = delete;

  public:     // exclusive locking
    void    lock();
//    bool    try_lock();
    void    unlock();

  public:     // shared locking
    void    lock_shared();
//    bool    try_lock_shared();
    void    unlock_shared();

  };

// recursive_shared_mutex implementation

  inline
  void  recursive_shared_mutex::lock()
    {
      std::unique_lock<std::recursive_mutex>  u_lock( thrdLock );
      auto                                    thrdId = std::this_thread::get_id();

      if ( thrdId != writerId )
        w_Writer.wait( u_lock, [&](){  return nWriters == 0;  } );

      writerId = thrdId;
        ++nWriters;

      w_Reader.wait( u_lock, [&](){  return nReaders == 0;  } );
    }

  inline
  void  recursive_shared_mutex::unlock()
    {
      std::lock_guard<std::recursive_mutex> aulock( thrdLock );

      if ( --nWriters == 0 )
        w_Writer.notify_all();
    }

  inline
  void  recursive_shared_mutex::lock_shared()
    {
      std::unique_lock<std::recursive_mutex>  u_lock( thrdLock );

      w_Writer.wait( u_lock, [&](){  return nReaders < INT_MAX && (nWriters == 0 || nReaders > 0);  } );
        ++nReaders;
    }

  inline
  void  recursive_shared_mutex::unlock_shared()
    {
      std::lock_guard<std::recursive_mutex> aulock( thrdLock );

      --nReaders;

      if ( nWriters != 0 )
      {
      // writer is waiting, and this thread is a last reader
        if ( nReaders == 0 )
          w_Reader.notify_one();
      }
        else
      {
      // Nobody is waiting for shared locks to clear, if we were at the max
      // capacity, release one thread waiting to obtain a shared lock in lock_shared().
        if ( nReaders == INT_MAX - 1 )
          w_Writer.notify_one();
      }
    }

}

# endif   // __priority_mutex_h__
