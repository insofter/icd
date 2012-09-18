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
  enum { ID = 2 };

  itd_event(const std::string& publisher, const time& dtm, int priority = 0)
    : event(publisher, dtm, ID, priority), state_(ITD_STATE_UNKNOWN) {}
  ~itd_event() {}

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

    os << "itd_event(" << publisher() << "," << dtm() << "," << priority()
      << "," << itd_state_str(state_) << ")";
  }
};

class itd_vd : public virtual_device, public event_publisher
{
public:
  itd_vd(const std::string& name, const std::string& dev_path = "");
  ~itd_vd();

  void set_active_low(bool active_low);

protected:
  std::string dev_path_;
  std::string sysfs_path_;
  std::string line_;
  std::ifstream is_;

  void throw_ios_reading_failure(const std::string& file);
  void throw_ios_writing_failure(const std::string& file);

private:
  virtual void run();
};

class file_itd_vd : public itd_vd 
{
public:
  file_itd_vd(const std::string& name, const std::string& file_path)
  : itd_vd(name, file_path), initialized_(false) {}
  ~file_itd_vd() {}

  time aggr_period() const { return aggr_period_; }
  void set_aggr_period(const time& aggr_period) { aggr_period_ = aggr_period; }

private:
  time aggr_period_;
  mutex lock_;
  condition cond_;
  time dtm_start_;
  time dtm_file_;
  bool initialized_;

  virtual void run();
};

class rand_itd_vd : public virtual_device, public event_publisher
{
public:
  rand_itd_vd(const std::string& name);
  ~rand_itd_vd() {}

  virtual void stop();

private:
  itd_state state_;
  mutex lock_;
  condition cond_;
  bool terminate_;

  virtual void run();
};

class aggr_timer_vd : public timer_vd
{
public:
  aggr_timer_vd(const std::string& name) : timer_vd(name)
  {
    set_periodic(true);
    set_aligned(true);
    set_priority(10);
  }
  ~aggr_timer_vd() {}

  void set_aggr_period(const time& aggr_period) { set_interval(aggr_period); }

};

class stats_event : public event
{
public:
  enum { ID = 3 };

  stats_event(const std::string& publisher, const time& dtm, int priority = 0)
    : event(publisher, dtm, ID, priority) {}
  ~stats_event() {}

  virtual event *clone() const { return new stats_event(*this); }

  void set_device(const std::string& device) { device_ = device; }
  void set_clear_time(const time& clear_time) { clear_time_ = clear_time; }
  void set_blocked_time(const time& blocked_time) { blocked_time_ = blocked_time; }

  std::string device() const { return device_; }
  time clear_time() const { return clear_time_; }
  time blocked_time() const { return blocked_time_; }

private:
  std::string device_;
  time clear_time_;
  time blocked_time_;

  virtual void dbg_print(std::ostream& os) const
  {
    os << "stats_event(" << publisher() << "," << dtm() << "," << priority()
      << "," << clear_time_.to_float() << "," << blocked_time_.to_float() << ")";
  }
};

class filter_vd : public event_subscriber, public event_publisher
{
public:
  filter_vd(const std::string& name)
    : event_subscriber(name), engage_delay_(time::from_msec(200)),
      release_delay_(time::from_msec(200)), fsm_event_(name, time()),
      fsm_state_(FILTER_FSM_UNINITIALIZED), state_(ITD_STATE_UNKNOWN) {}
  ~filter_vd() {}

  void set_itd_vd(virtual_device& itd_vd);
  void set_aggr_timer_vd(virtual_device& aggr_timer_vd);
  void set_engage_delay(const time& engage_delay) { engage_delay_ = engage_delay; }
  void set_release_delay(const time& release_delay) { release_delay_ = release_delay; }

  time engage_delay() const { return engage_delay_; }
  time release_delay() const { return release_delay_; }

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
  std::string aggr_timer_vd_name_;
  time engage_delay_;
  time release_delay_;

  itd_event fsm_event_;
  std::string device_;
  fsm_state fsm_state_;
  time dtm_;
  time stats_sent_dtm_;
  itd_state state_;
  time clear_time_;
  time blocked_time_;

  virtual void handle_event(const event& e);
  virtual void initialize();

  void handle_itd_event(const itd_event& e);
  void handle_filter_event(const timer_event& e);
  void handle_aggr_event(const timer_event& e);
  std::string fsm_state_str(const fsm_state& state);
  void update_fsm(const fsm_state& fsm_state);
  void update_stats(const time& dtm);
  void send_fsm_event();
  void send_stats_event();
};

class aggr_vd : public event_subscriber
{
public:
  aggr_vd(const std::string& name, const std::string& db_name, int db_timeout)
    : event_subscriber(name), db_name_(db_name), db_timeout_(db_timeout),
      aggr_period_(15,0), insert_stmt_(db_), update_stmt_(db_), 
      initialized_(false), cnt_(0) {}
  ~aggr_vd() {}

  time aggr_period() const { return aggr_period_; }

  void set_filter_vd(virtual_device& filter_vd);
  void set_aggr_period(const time& aggr_period) { aggr_period_ = aggr_period; }

private:
  enum flow_flag
  { 
    FLOW_FLAG_NOT_SENT = 2
   };

  std::string db_name_;
  int db_timeout_;
  time aggr_period_;
  std::string filter_vd_name_;
  sqlite3cc::conn db_;
  sqlite3cc::stmt insert_stmt_;
  sqlite3cc::stmt update_stmt_;
  bool initialized_;
  std::string device_;
  time dtm_;
  int cnt_;
  time clear_time_;
  time blocked_time_;

  virtual void handle_event(const event& e);
  virtual void initialize();
  virtual void destroy();

  void handle_itd_event(const itd_event& e);
  void handle_stats_event(const stats_event& e);
  void insert_flow();
  void update_flow();
};

class event_logger_vd : public event_subscriber
{
public:
  event_logger_vd(const std::string& name, const std::string& db_name, int db_timeout)
    : event_subscriber(name), db_name_(db_name), db_timeout_(db_timeout), stmt_(db_) {}
  ~event_logger_vd() {}

  void add_publisher(virtual_device& vd);

private:
  std::string db_name_;
  int db_timeout_;
  sqlite3cc::conn db_;
  sqlite3cc::stmt stmt_;

  virtual void handle_event(const event& e);
  virtual void initialize();
  virtual void destroy();
};
}
#endif // __ITD_VDS_H__

