#include "virtual-device.h"

#include <iostream>
#include <stdexcept>
#include <signal.h>

namespace icd
{
  std::ostream& operator <<(std::ostream &os, const event &ref)
  {
    ref.dbg_print(os);
    return os;
  }

  void timer_event::dbg_print(std::ostream& os) const
  {
    os << "timer_event(" << publisher() << "," << dtm() << "," << priority() << ")";
  }

  void virtual_device::run()
  {
    try
    {
      syslog() << info << "vd::run: device started" << std::endl;

      run_vd();

      syslog() << info << "vd::run: device terminated" << std::endl;
    }
    catch(std::exception& e)
    {
      syslog() << err << "vd::run: exception handled: " << e.what() << std::endl;
    }

    if (term_lock_ && term_cond_)
    {
      mutex_locker ml(*term_lock_);
      term_cond_->signal();
    }
  }

  void event_subscriber::queue_event(const event& e)
  {
    mutex_locker ml(lock_);
    queue_.push(e.clone());
    queue_cond_.signal();
  }

  void event_subscriber::stop()
  {
    {
      mutex_locker ml(lock_);
      terminate_ = true;
      queue_cond_.signal();
    }
    join();
    terminate_ = false;
  }

  void event_subscriber::run_vd()
  {
    mutex_locker ml(lock_);

    initialize();

    while(!terminate_)
    {

      while (!queue_.empty() && !terminate_)
      {
        // apply delay;
        // note that  this delay gets interrupted when new element is inserted
        // into the queue, after that the time to wait is reevaluated (it may
        // happen that newly inserted item is older and the wait will be shortened)
        time dtm = queue_.top()->dtm() + delay_;
        while (time::now() < dtm && !terminate_)
        {
          queue_cond_.timedwait(lock_, dtm);
          dtm = queue_.top()->dtm() + delay_;
        }

        // process event
        // note that the queue top element here may be defferent to the one
        // that was taken to calculate delay (a late event may have arrived
        // to the queue)
        if (!terminate_)
        {
          const event *e = queue_.top();
          queue_.pop();
          lock_.unlock();
          handle_event(*e);
          lock_.lock();
          delete e;
        }
      }

      lock_.unlock();
      handle_queue_empty();
      lock_.lock();

      if (queue_.empty() && !terminate_)
        queue_cond_.wait(lock_);
    }

    destroy();
  }

  void event_publisher::attach_subscriber(event_subscriber& subscriber)
  {
    mutex_locker ml(lock_);
    subscribers_.insert(std::pair<std::string, event_subscriber&>(
      subscriber.name(), subscriber));
  }

  void event_publisher::detach_subscriber(event_subscriber& subscriber)
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

  void timer_vd::stop()
  {
    {
      mutex_locker ml(lock_);
      terminate_ = true;
      cond_.signal();
    }
    join();
    terminate_ = false;
  }

  void timer_vd::run_vd()
  {
    mutex_locker ml(lock_);

    while(periodic_ && !terminate_)
    {
      time dtm = time::now();
      if (aligned_)
        dtm = time::align(dtm, interval_);
      dtm = dtm + interval_;

      cond_.timedwait(lock_, dtm);

      if (time::now() >= dtm && !terminate_)
      {
        syslog() << debug << "timer_vd::run_vd: timer fired, " << dtm << std::endl;
        timer_event e(name(), dtm, priority_);
        notify_subscribers(e);
      }
    }
  }

  void terminator_vd::run_vd()
  {
    sigset_t signal_mask;
    sigemptyset (&signal_mask);
    sigaddset (&signal_mask, SIGINT);
    int sig_caught;

    syslog() << debug << "terminator_vd::run_vd: wait for termination signal started" << std::endl;

    int res = sigwait (&signal_mask, &sig_caught);
    if (res != 0)
      throw std::runtime_error("sigwait failed");

    syslog() << debug << "terminator_vd::run_vd: terminatin signal received" << std::endl;
  }

  void dbglog_vd::add_publisher(event_publisher& publisher)
  {
    publisher.attach_subscriber(*this);
  }

  void dbglog_vd::handle_event(const event& e)
  {
    syslog() << info << "dbglog_vd::handle_event, " << e << std::endl;
  }

  vd_farm::vd_farm()
  {
    add(new terminator_vd("term"));
  }

  void vd_farm::run()
  {
    try
    {
       mutex_locker ml(lock_);
       create();
       start();
       cond_.wait(lock_);
    }
    catch(std::exception& e)
    {
      stop();
      throw;
    }

    stop();
  }

  void vd_farm::add(virtual_device* vd)
  {
    vd->set_term_signal(cond_, lock_);
    vds_.push_back(vd);
  }

  void vd_farm::start()
  {
    std::vector<virtual_device*>::iterator i;
    for (i = vds_.begin(); i != vds_.end(); i++)
      (*i)->start();
  }

  void vd_farm::stop()
  {
    std::vector<virtual_device*>::iterator i;
    for (i = vds_.begin(); i != vds_.end(); i++)
    {
      try
      {
        (*i)->stop();
      }
      catch(std::exception& e)
      {
      }
    }
  }

  vd_farm::~vd_farm()
  {
    std::vector<virtual_device*>::iterator i;
    for (i = vds_.begin(); i != vds_.end(); i++)
      delete (*i);
  }
}

