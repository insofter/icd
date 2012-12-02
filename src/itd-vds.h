#ifndef __ITD_VDS_H__
#define __ITD_VDS_H__

#include "sqlite3cc.h"
#include "virtual-device.h"
#include "itd-device.h"
#include <fstream>

namespace icd
{
/*  enum itd_state
  {
    ITD_STATE_UNKNOWN = -1,
    ITD_STATE_CLEAR = 0,
    ITD_STATE_BLOCKED = 1,
  };
*/
  class itd_event : public event
  {
  public:
    enum { ID = 101 };

    itd_event(const std::string& publisher, const time& dtm, int priority = 0)
      : event(publisher, dtm, ID, priority), state_(ITD_STATE_UNKNOWN) {}
    virtual ~itd_event() {}

    virtual event *clone() const { return new itd_event(*this); }

    void set_device(const std::string& device) { device_ = device; }
    void set_state(const itd_state& state) { state_ = state; }

    std::string device() const { return device_; }
    itd_state state() const { return state_; }

  private:
    std::string device_;
    itd_state state_;

    virtual void dbg_print(std::ostream& os) const;
  };

  class stats_event : public event
  {
  public:
    enum { ID = 102 };

    stats_event(const std::string& publisher, const time& dtm, int priority = 0)
      : event(publisher, dtm, ID, priority), aggr_period_(false)  {}
    virtual ~stats_event() {}

    virtual event *clone() const { return new stats_event(*this); }

    void set_device(const std::string& device) { device_ = device; }
    void set_clear_time(const time& clear_time) { clear_time_ = clear_time; }
    void set_blocked_time(const time& blocked_time) { blocked_time_ = blocked_time; }
    void set_aggr_period(bool aggr_period) { aggr_period_ = aggr_period; }

    std::string device() const { return device_; }
    time clear_time() const { return clear_time_; }
    time blocked_time() const { return blocked_time_; }
    bool aggr_period() const { return aggr_period_; }

  private:
    std::string device_;
    time clear_time_;
    time blocked_time_;
    bool aggr_period_;

    virtual void dbg_print(std::ostream& os) const;
  };

  class filter_event : public event
  {
  public:
    enum { ID = 103 };

    filter_event(const std::string& publisher, const time& dtm, int priority = 0)
      : event(publisher, dtm, ID, priority), queue_empty_(false) {}
    virtual ~filter_event() {}

    virtual event *clone() const { return new filter_event(*this); }

    void set_queue_empty(bool queue_empty) { queue_empty_ = queue_empty; }
    bool queue_empty() const { return queue_empty_; }

  private:
    bool queue_empty_;

    virtual void dbg_print(std::ostream& os) const;
  };

  class test_event : public event
  {
  public:
    enum { ID = 104 };

    test_event(const std::string& publisher, const time& dtm, int priority = 0)
      : event(publisher, dtm, ID, priority) {}
    virtual ~test_event() {}

    virtual event *clone() const { return new test_event(*this); }

    void set_results(const std::map<std::string,itd_test>& results) { results_ = results; }
    std::map<std::string,itd_test> results() const { return results_; }
    itd_test result(const std::string& device) const;

  private:
    std::map<std::string,itd_test> results_;

    virtual void dbg_print(std::ostream& os) const;
  };

  class itd_vd : public virtual_device, public event_publisher
  {
  public:
    itd_vd(const std::string& name, const std::string& dev_path = "");
    virtual ~itd_vd() {}

    void set_active_low(bool active_low);

  protected:
    std::string dev_path_;
    std::string sysfs_path_;
    std::string line_;
    std::ifstream is_;

    void throw_ios_reading_failure(const std::string& file);
    void throw_ios_writing_failure(const std::string& file);

    virtual void start();
    virtual void stop();

  private:
    virtual void run_vd();
    void restart(void);
  };

  class file_itd_vd : public itd_vd 
  {
  public:
    file_itd_vd(const std::string& name, const std::string& file_path)
    : itd_vd(name, file_path), initialized_(false) {}
    virtual ~file_itd_vd() {}

    time aggr_period() const { return aggr_period_; }
    void set_aggr_period(const time& aggr_period) { aggr_period_ = aggr_period; }

    virtual void start();

  private:
    time aggr_period_;
    mutex lock_;
    condition cond_;
    time dtm_start_;
    time dtm_file_;
    bool initialized_;

    virtual void run_vd();
  };

  class rand_itd_vd : public virtual_device, public event_publisher
  {
  public:
    rand_itd_vd(const std::string& name);
    virtual ~rand_itd_vd() {}

    virtual void stop();

  private:
    itd_state state_;
    mutex lock_;
    condition cond_;
    bool terminate_;

    virtual void run_vd();
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
    virtual ~aggr_timer_vd() {}

    void set_aggr_period(const time& aggr_period) { set_interval(aggr_period); }

  };

  class filter_vd : public event_subscriber, public event_publisher
  {
  public:
    filter_vd(const std::string& name)
      : event_subscriber(name), engage_delay_(time::from_msec(200)),
        release_delay_(time::from_msec(200)), fsm_event_(name, time()),
        fsm_state_(FILTER_FSM_UNINITIALIZED), state_(ITD_STATE_UNKNOWN) {}
    virtual ~filter_vd() {}

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
    std::string led_ctrl_path_;
    std::ofstream led_ctrl_os_;

    itd_event fsm_event_;
    std::string device_;
    fsm_state fsm_state_;
    time dtm_;
    itd_state state_;
    time clear_time_;
    time blocked_time_;

    virtual void initialize();
    virtual void destroy();
    virtual void handle_event(const event& e);
    virtual void handle_queue_empty();

    void handle_itd_event(const itd_event& e);
    void handle_filter_event(const filter_event& e);
    void handle_aggr_event(const timer_event& e);
    std::string fsm_state_str(const fsm_state& state);
    void update_fsm(const fsm_state& fsm_state);
    void update_stats(const time& dtm);
    void send_fsm_event();
    void send_stats_event(bool aggr_period = false);

    void set_led_state(bool on);
  };

  class test_vd : public event_subscriber, public event_publisher
  {
  public:
    test_vd(const std::string& name)
      : event_subscriber(name) {}
    virtual ~test_vd() {}

    void set_timer_vd(virtual_device& timer_vd);

  private:
    std::string timer_vd_name_;

    virtual void handle_event(const event& e);

    void handle_timer_event(const timer_event& e);
  };

  class aggr_vd : public event_subscriber
  {
  public:
    aggr_vd(const std::string& name, const std::string& db_name, int db_timeout)
      : event_subscriber(name), db_name_(db_name), db_timeout_(db_timeout),
        insert_stmt_(db_), update_stmt_(db_), counter_id_(0), cnt_(0) {}
    virtual ~aggr_vd() {}

    void set_filter_vd(virtual_device& filter_vd);
    void set_test_vd(virtual_device& test_vd, const std::string& device);
    void set_counter_id(long counter_id) { counter_id_ = counter_id; }

  private:
    enum flow_flag
    { 
      FLOW_FLAG_OPEN = 3,
      FLOW_FLAG_COMPLETE = 2
    };

    std::string filter_vd_name_;
    std::string test_vd_name_;
    std::string device_;

    std::string db_name_;
    int db_timeout_;
    sqlite3cc::conn db_;
    sqlite3cc::stmt insert_stmt_;
    sqlite3cc::stmt update_stmt_;

    long counter_id_;
    time dtm_;
    int cnt_;
    time clear_time_;
    time blocked_time_;
    itd_test test_result_;

    virtual void handle_event(const event& e);
    virtual void initialize();
    virtual void destroy();

    void handle_itd_event(const itd_event& e);
    void handle_stats_event(const stats_event& e);
    void handle_test_event(const test_event& e);
    void insert_flow();
    void update_flow(const flow_flag& flag);
  };

  class event_logger_vd : public event_subscriber
  {
  public:
    event_logger_vd(const std::string& name, const std::string& db_name, int db_timeout)
      : event_subscriber(name), db_name_(db_name), db_timeout_(db_timeout), stmt_(db_) {}
    virtual ~event_logger_vd() {}

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

  class itd_farm : public vd_farm
  {
  public:
    itd_farm(const std::string& config_db_name, const std::string& data_db_name,
      int db_timeout)
      : config_db_name_(config_db_name), data_db_name_(data_db_name),
        db_timeout_(db_timeout) {}
    virtual ~itd_farm() {}

  private:
    std::string config_db_name_;
    std::string data_db_name_;
    int db_timeout_;

    virtual void create();
  };
}
#endif // __ITD_VDS_H__

