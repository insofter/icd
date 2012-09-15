#include "virtual-device.h"

#include <iostream>

namespace icd
{
std::ostream& operator <<(std::ostream &os, const event &ref)
{
  ref.dbg_print(os);
  return os;
}

void event_subscriber::queue_event(const event& e)
{
  mutex_locker ml(lock_);
  syslog() << "event queued, " << e << std::endl;
  queue_.push(e.clone());
  queue_cond_.signal();
}

void event_subscriber::run()
{
  mutex_locker ml(lock_);

  while(1)
  {
    queue_cond_.wait(lock_);

    while (!queue_.empty())
    {
      // apply delay
      time dtm = queue_.top()->dtm() + delay_;
      while (time::now() < dtm)
        delay_cond_.timedwait(lock_, dtm);

      // process event
      // note that the queue top element here may be defferent to the one
      // that was taken to calculate delay (a late event may have arrived
      // to the queue)
      const event *e = queue_.top();
      queue_.pop();
      lock_.unlock();
      syslog() << "event handled, " << *e << std::endl;
      handle_event(*e);
      lock_.lock();
      delete e;
    }
  }
}

void event_publisher::subscribe(event_subscriber& subscriber)
{
  mutex_locker ml(lock_);
  subscribers_.insert(std::pair<std::string, event_subscriber&>(
    subscriber.name(), subscriber));
}

void event_publisher::unsubscribe(event_subscriber& subscriber)
{
  mutex_locker ml(lock_);
  subscribers_.erase(subscriber.name());
}

void event_publisher::notify_subscribers(const event& e)
{
  mutex_locker ml(lock_);
  std::map<std::string, event_subscriber&>::iterator i;
  for (i = subscribers_.begin(); i != subscribers_.end(); i++)
    (*i).second.queue_event(e);
}

void timer_vd::run()
{
  mutex_locker ml(lock_);

  while(periodic_)
  {
    time dtm = time::now();
    if (aligned_)
      dtm = time::align(dtm, interval_);
    dtm = dtm + interval_;

    cond_.timedwait(lock_, dtm);

    if (time::now() >= dtm)
    {
      timer_event e(name(), dtm);
      notify_subscribers(e);
    }
  }
}

void dbglog_vd::handle_event(const event& e)
{
  syslog() << "dbglog_vd::handle_event, " << e << std::endl;
}
}
