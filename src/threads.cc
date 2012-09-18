#include <cstdio>
#include <stdexcept>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <errno.h>
#include "threads.h"

namespace icd
{
  time time::now()
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return time(tv.tv_sec, tv.tv_usec);
  }

  time time::align(const time& dtm, const time& interval)
  {
    long long a = dtm.sec() * 1000000 + dtm.usec();
    long long b = interval.sec() * 1000000 + interval.usec();
    long long c = (a / b) * b;
    return time(c / 1000000, c % 1000000);
  }

  std::ostream& operator <<(std::ostream &os,const time &ref)
  {
    char s[32];
    struct tm result;
    struct tm *m = gmtime_r((const time_t *)&ref.sec_, &result);
    strftime(s, sizeof(s), "%Y-%m-%dT%H:%M:%S", m);
    os << s << "." << std::setw(6) << std::setfill('0') << ref.usec_;
    return os;
  }

  void time::operator+=(const time &ref)
  {
    long usec = usec_ + ref.usec_;
    sec_ += ref.sec_ + usec / 1000000;
    usec_ = usec % 1000000;
  }

  time operator+(const time &ref1, const time &ref2)
  {
    long usec = ref1.usec_ + ref2.usec_;
    long sec = ref1.sec_ + ref2.sec_ + usec / 1000000;
    usec = usec % 1000000;
    return time(sec, usec);
  }

  time operator-(const time &ref1, const time &ref2)
  {
    long usec = ref1.usec_ - ref2.usec_;
    long sec = ref1.sec_ - ref2.sec_;
    if (usec < 0)
    {
      usec += 1000000;
      sec -= 1;
    }

    return time(sec, usec);
  }

  bool operator== (const time &ref1, const time &ref2)
  {
    return (ref1.sec_ == ref2.sec_ && ref1.usec_ == ref2.usec_);
  }

  bool operator!= (const time &ref1, const time &ref2)
  {
    return (ref1.sec_ != ref2.sec_ || ref1.usec_ != ref2.usec_);
  }

  bool operator> (const time &ref1, const time &ref2)
  {
    return (ref1.sec_ > ref2.sec_ || (ref1.sec_ == ref2.sec_ && ref1.usec_ > ref2.usec_));
  }

  bool operator>= (const time &ref1, const time &ref2)
  {
    return (ref1.sec_ > ref2.sec_ || (ref1.sec_ == ref2.sec_ && ref1.usec_ >= ref2.usec_));
  }

  bool operator< (const time &ref1, const time &ref2)
  {
    return (ref1.sec_ < ref2.sec_ || (ref1.sec_ == ref2.sec_ && ref1.usec_ < ref2.usec_));
  }

  bool operator<= (const time &ref1, const time &ref2)
  {
    return (ref1.sec_ < ref2.sec_ || (ref1.sec_ == ref2.sec_ && ref1.usec_ <= ref2.usec_));
  }

  void thread::start()
  {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int res = pthread_create(&raw_thread_, &attr, start_routine, (void *)this); 
    if (res != 0)
      throw std::runtime_error("pthread_create failed");

    pthread_attr_destroy(&attr);
  }

  void thread::stop()
  {
    cancel();
    join();
  }

  void thread::cancel()
  {
    int res = pthread_cancel(raw_thread_);
    if (res != 0)
      throw std::runtime_error("pthread_cancel failed");
  }

  void thread::join()
  {
    int res = pthread_join(raw_thread_, NULL);
    if (res != 0)
      throw std::runtime_error("pthread_join failed");
  }

  void *thread::start_routine(void* data)
  {
    int state = 0;
    int type = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &state);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &type);

    static_cast<thread*>(data)->run();

    pthread_exit(NULL);
  }



  mutex::mutex()
  {
    int res = pthread_mutex_init(&raw_mutex_, NULL);
    if (res != 0)
      throw std::runtime_error("mutex_init failed");
  }

  mutex::~mutex()
  {
    pthread_mutex_destroy(&raw_mutex_);
  }

  void mutex::lock()
  {
    int res = pthread_mutex_lock(&raw_mutex_);
    if (res != 0)
      throw std::runtime_error("mutex_lock failed");
  }

  void mutex::trylock()
  {
    int res = pthread_mutex_trylock(&raw_mutex_);
    if (res != 0)
      throw std::runtime_error("mutex_try failed");
  }

  void mutex::unlock()
  {
    int res = pthread_mutex_unlock(&raw_mutex_);
    if (res != 0)
      throw std::runtime_error("mutex_unlock failed");
  }



  condition::condition()
  {
    int res = pthread_cond_init(&raw_cond_, NULL);
    if (res != 0)
      throw std::runtime_error("cond_init failed");
  }

  condition::~condition()
  {
    pthread_cond_destroy(&raw_cond_);
  }

  void condition::wait(mutex& m)
  {
    int res = pthread_cond_wait(&raw_cond_, m.raw_mutex());
    if (res != 0)
      throw std::runtime_error("cond_wait failed");
  }

  void condition::timedwait(mutex& m, const time& dtm)
  {
    timespec abstime;
    abstime.tv_sec = static_cast<time_t>(dtm.sec());
    abstime.tv_nsec = dtm.usec() * 1000;
    int res = pthread_cond_timedwait(&raw_cond_, m.raw_mutex(), &abstime);
    
    if (res != 0 && res != ETIMEDOUT)
      throw std::runtime_error("cond_timedwait failed");
  }

  void condition::signal()
  {
    int res = pthread_cond_signal(&raw_cond_);
    if (res != 0)
      throw std::runtime_error("cond_signal failed");
  }

  void condition::broadcast()
  {
    int res = pthread_cond_broadcast(&raw_cond_);
    if (res != 0)
      throw std::runtime_error("cond_broadcast failed");
  }

}

