#ifndef __VIRTUAL_DEVICE_H__
#define __VURTUAL_DEVICE_H__

#include "syslogstream.h"
#include "threads.h"

#include <string>
#include <time.h>
#include <queue>
#include <map>

namespace icd
{
  class event
  {
  public:
    event(const std::string& publisher, const time& dtm, int id, int priority = 0)
      : publisher_(publisher), dtm_(dtm), id_(id), priority_(priority) {}
    virtual ~event() {}

    virtual event *clone() const = 0;

    int id() const  { return id_; }
    int priority() const { return priority_; }
    std::string publisher() const { return publisher_; }
    time dtm() const { return dtm_; }

    friend std::ostream& operator <<(std::ostream &os, const event &ref);

  private:
    int id_;
    int priority_;
    std::string publisher_;
    time dtm_;

    virtual void dbg_print(std::ostream& os) const = 0;
  };

  class timer_event : public event
  {
  public:
    enum { ID = 1 };

    timer_event(const std::string& publisher, const time& dtm, int priority = 0)
      : event(publisher, dtm, ID, priority) {}
    virtual ~timer_event() {}

    virtual event *clone() const { return new timer_event(*this); }

  private:
    virtual void dbg_print(std::ostream& os) const;
  };


  class virtual_device : public thread
  {
  public:
    virtual_device(const std::string& name)
      : syslog_(name), name_(name), term_lock_(NULL), term_cond_(NULL) {}
    virtual ~virtual_device() {}

    std::string name() const { return name_; }
    std::ostream &syslog() { return syslog_; }

    void set_term_signal(condition& term_cond, mutex& term_lock) 
    { 
      term_cond_ = &term_cond;
      term_lock_ = &term_lock; 
    }

  private:
    icd::syslogstream syslog_;
    std::string name_;
    mutex* term_lock_;
    condition* term_cond_;

    virtual void run();
    virtual void run_vd() = 0;
  };

  class event_subscriber : public virtual_device
  {
  public:
    event_subscriber(const std::string& name) : virtual_device(name), terminate_(false) {}
    virtual ~event_subscriber() {}

    time delay() { mutex_locker ml(lock_); return delay_; }
    void set_delay(const time& delay) { mutex_locker ml(lock_); delay_ = delay; }

    void queue_event(const event& e);
    virtual void stop();

  protected:
    virtual void initialize() {}
    virtual void destroy() {}
    virtual void handle_event(const event& e) = 0;

  private:
    struct prioritize_events
    {
      // return true if e2 is older (=higher priority) that e1
      bool operator()(const event* &e1, const event* &e2) const
      {
        return (e1->dtm() > e2->dtm())
          || (e1->dtm() == e2->dtm() && e1->priority() < e2->priority());
      }
    };

    std::priority_queue<const event *, std::vector<const event *>, prioritize_events> queue_;
    time delay_;
    mutex lock_;
    condition queue_cond_;
    condition delay_cond_;
    bool terminate_;

    virtual void run_vd();
  };

  class event_publisher
  {
  public:
    event_publisher() {}
    virtual ~event_publisher() {}

    void attach_subscriber(event_subscriber& subscriber);
    void detach_subscriber(event_subscriber& subscriber);

  protected:
    void notify_subscribers(const event& e);

  private:
    std::map<std::string, event_subscriber&> subscribers_;
    mutex lock_;
  };

  class timer_vd : public virtual_device, public event_publisher
  {
  public:
    timer_vd(const std::string& name) : virtual_device(name),
      periodic_(true), aligned_(false), priority_(0), terminate_(false)  {}
    virtual ~timer_vd() {}

    bool periodic() { mutex_locker ml(lock_); return periodic_; }
    bool aligned() { mutex_locker ml(lock_); return aligned_; }
    time interval() { mutex_locker ml(lock_); return interval_; }
    int priority() { mutex_locker ml(lock_); return priority_; }

    void set_periodic(bool periodic) { mutex_locker ml(lock_); periodic_ = periodic; }
    void set_aligned(bool aligned) { mutex_locker ml(lock_); aligned_ = aligned; }
    void set_interval(const time& interval) { mutex_locker ml(lock_); interval_ = interval; }
    void set_priority(int priority) { mutex_locker ml(lock_); priority_ = priority; }

    virtual void stop();

  private:
    bool periodic_;
    bool aligned_;
    time interval_;
    int priority_;
    bool terminate_;
    mutex lock_;
    condition cond_;

    virtual void run_vd();
  };

  class terminator_vd : public virtual_device
  {
  public:
    terminator_vd(const std::string& name) : virtual_device(name) {}
    virtual ~terminator_vd() {}

    static void initialize();

  private:
    virtual void run_vd();
  };

  class dbglog_vd : public event_subscriber
  {
  public:
    dbglog_vd(const std::string& name) : event_subscriber(name) {}
    virtual ~dbglog_vd() {}

    void add_publisher(event_publisher& publisher);

  private:
    virtual void handle_event(const event& e);
  };

  class vd_farm
  {
  public:
    vd_farm();
    virtual ~vd_farm();

    void run();

  protected:
    void add(virtual_device* vd);

  private:
    std::vector<virtual_device*> vds_;
    mutex lock_;
    condition cond_;

    virtual void create() = 0;
    void start();
    void stop();
  };
}
#endif // __VURTUAL_DEVICE_H__

