#include <iostream>

#include <pthread.h>

#ifndef __THREADS_H__
#define __THREADS_H__

namespace icd
{
  class time
  {
    public:
      time() : sec_(0), usec_(0) {}
      time(long sec, long usec) : sec_(sec), usec_(usec) {}
      time(const time& ref) : sec_(ref.sec_), usec_(ref.usec_) {}

      static time now();
      static time align(const time& dtm, const time& interval);

      ~time() {}

      time& operator =(const time& ref)
      {
        sec_ = ref.sec_;
        usec_ = ref.usec_;
        return *this;
      }

      long sec() const { return sec_; }
      long usec() const { return usec_; }

      operator float() const;

      friend std::ostream& operator <<(std::ostream &os, const time &ref);

      friend time operator+(const time &ref1, const time &ref2);
      friend time operator-(const time &ref1, const time &ref2);

      friend bool operator== (const time &ref1, const time &ref2);
      friend bool operator!= (const time &ref1, const time &ref2);
      friend bool operator> (const time &ref1, const time &ref2);
      friend bool operator>= (const time &ref1, const time &ref2);
      friend bool operator< (const time &ref1, const time &ref2);
      friend bool operator<= (const time &ref1, const time &ref2);

    private:
      long sec_;
      long usec_;
  };

  class thread
  {
  public:
    thread() {}
    ~thread() {}

    void start();
    virtual void stop();

  protected:
    void cancel();
    void join();

    virtual void run() = 0;

  private:
    pthread_t raw_thread_;

    static void *start_routine(void* data);
  };

  class mutex
  {
    friend class condition;
  public:
    mutex();
    ~mutex();

    void lock();
    void trylock();
    void unlock();

  protected:
    pthread_mutex_t *raw_mutex() { return &raw_mutex_; }

  private:
    pthread_mutex_t raw_mutex_;
  };

  class mutex_locker
  {
  public:
    mutex_locker(mutex& m) : m_(m) { m_.lock(); }
    ~mutex_locker() { m_.unlock(); }

  private:
    mutex& m_;
  };

  class condition
  {
  public:
    condition();
    ~condition();

    void wait(mutex& m);
    void timedwait(mutex& m, const time& dtm);
    void signal();
    void broadcast();

  private:
    pthread_cond_t raw_cond_;
  };
}

#endif // __THREADS_H__
