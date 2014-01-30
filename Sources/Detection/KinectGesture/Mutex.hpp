#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <pthread.h>

class Mutex {
public:
  Mutex();
  void lock();
  void unlock();
  class ScopedLock
  {
    Mutex & _mutex;
  public:
    ScopedLock(Mutex & mutex);
    ~ScopedLock();
  };
private:
  pthread_mutex_t m_mutex;
};

#endif
