#ifndef __ITD_VDS_H__
#define __ITD_VDS_H__

#include "sqlite3cc.h"
#include "virtual-device.h"
#include <fstream>

namespace icd
{
enum itd_state
{
  ITD_STATE_UNKNOWN = -1,
  ITD_STATE_CLEAR = 0,
  ITD_STATE_BLOCKED = 1,
};

std::string itd_state_str(const itd_state& state);

class itd_event : public event
{
public:
  itd_event(const std::string& publisher, const time& dtm)
    : event(publisher, dtm), state_(ITD_STATE_UNKNOWN) {}
  ~itd_event() {}

  enum { ID = 2 };

  virtual int id() const { return ID; }
  virtual event *clone() const { return new itd_event(*this); }

  void set_device(const std::string& device) { device_ = device; }
  void set_state(const itd_state& state) { state_ = state; }

  std::string device() const { return device_; }
  itd_state state() const { return state_; }

private:
  std::string device_;
  itd_state state_;

  virtual void dbg_print(std::ostream& os) const
  {

    os << "itd_event(" << publisher() << "," << dtm() << ","
      << "," << itd_state_str(state_) << ")";
  }
};

class itd_vd : public virtual_device, public event_publisher
{
public:
  itd_vd(const std::string& name);
  ~itd_vd() {}

private:
  std::string dev_path_;
  std::string line_;
  std::ifstream is_;

  virtual void run();

  void throw_ios_reading_failure(const std::string& file);
};

class rand_itd_vd : public virtual_device, public event_publisher
{
public:
  rand_itd_vd(const std::string& name);
  ~rand_itd_vd() {}

private:
  itd_state state_;
  mutex lock_;
  condition cond_;

  virtual void run();
};


class statistics_event : public event
{
public:
  statistics_event(const std::string& publisher, const time& dtm)
    : event(publisher, dtm) {}
  ~statistics_event() {}

  enum { ID = 3 };

  virtual int id() const { return ID; }
  virtual event *clone() const { return new statistics_event(*this); }

  void set_work_time(const time& work_time) { work_time_ = work_time; }
  void set_dark_time(const time& dark_time) { dark_time_ = dark_time; }

  time work_time() const { return work_time_; }
  time dark_time() const { return dark_time_; }

private:
  time work_time_;
  time dark_time_;

  virtual void dbg_print(std::ostream& os) const
  {
    os << "statistics_event(" << publisher() << "," << dtm() << ","
      << float(work_time_) << "," << float(dark_time_) << ")";
  }
};

class filter_vd : public event_subscriber, public event_publisher
{
public:
  filter_vd(const std::string& name)
    : event_subscriber(name), engage_time_(time(0,200000)),
      release_time_(time(0,200000)), fsm_event_(name, time()),
      fsm_state_(FILTER_FSM_UNINITIALIZED), state_(ITD_STATE_UNKNOWN) {}
  ~filter_vd() {}

  void set_itd_vd_name(const std::string& itd_vd_name) { itd_vd_name_ = itd_vd_name; }
  void set_aggr_vd_name(const std::string& aggr_vd_name) { aggr_vd_name_ = aggr_vd_name; }
  void set_engage_time(const time& engage_time) { engage_time_ = engage_time; }
  void set_release_time(const time& release_time) { release_time_ = release_time; }

  std::string itd_vd_name() const { return itd_vd_name_; }
  std::string aggr_vd_name() const { return aggr_vd_name_; }
  time engage_time() const { return engage_time_; }
  time release_time() const { return release_time_; }

private:
  enum fsm_state
  {
    FILTER_FSM_UNINITIALIZED = 0,
    FILTER_FSM_CLEAR = 1,
    FILTER_FSM_ENGAGING = 2,
    FILTER_FSM_BLOCKED = 3,
    FILTER_FSM_RELEASING = 4
  };

  std::string itd_vd_name_;
  std::string aggr_vd_name_;
  time engage_time_;
  time release_time_;

  itd_event fsm_event_;
  fsm_state fsm_state_;
  time dtm_;
  itd_state state_;

  virtual void handle_event(const event& e);
  void update_stats(const event& e);
  void update_fsm(const event& e);
  void send_fsm_event();
};

class aggr_vd : public event_subscriber
{
public:
  aggr_vd(const std::string& name)
    : event_subscriber(name) {}
  ~aggr_vd() {}

private:
  virtual void handle_event(const event& e);

};

class event_writer_vd : public event_subscriber
{
public:
  event_writer_vd(const std::string& name, const std::string& db_name, int db_timeout);
  ~event_writer_vd();

private:
  sqlite3cc::conn db_;
  sqlite3cc::stmt stmt_;

  virtual void handle_event(const event& e);
};
}
#endif // __ITD_VDS_H__

