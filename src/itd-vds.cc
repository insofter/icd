#include "itd-vds.h"
#include "db-config.h"
#include <cstdlib>

namespace icd
{
  template <class T>
  inline std::string to_string (const T& t)
  {
    std::stringstream ss;
    ss << t;
    return ss.str();
  }

  std::string itd_state_str(const itd_state& state)
  {
    switch(state)
    {
      case ITD_STATE_UNKNOWN:
        return "ITD_STATE_UNKNOWN";
      case ITD_STATE_CLEAR:
        return "ITD_STATE_CLEAR"; 
      case ITD_STATE_BLOCKED:
        return "ITD_STATE_BLOCKED";
      default :
        return "?";
    }
  }

  void itd_event::dbg_print(std::ostream& os) const
  {
    os << "itd_event(" << publisher() << "," << dtm() << "," << priority()
      << "," << itd_state_str(state_) << ")";
  }

  void stats_event::dbg_print(std::ostream& os) const
  {
    os << "stats_event(" << publisher() << "," << dtm() << "," << priority()
      << "," << clear_time_.to_float() << "," << blocked_time_.to_float() 
      << "," << aggr_period_ << ")";
  }

  void filter_event::dbg_print(std::ostream& os) const
  {
    os << "filter_event(" << publisher() << "," << dtm() << "," << priority()
      << "," << queue_empty_ << ")";
  }

  itd_vd::itd_vd(const std::string& name, const std::string& dev_path)
    : virtual_device(name)
  {
    dev_path_ = dev_path.empty() ? "/dev/" + name : dev_path;
    sysfs_path_ = "/sys/devices/platform/gpio-itd.";
    sysfs_path_ += name.substr(3);
  }

  void itd_vd::set_active_low(bool active_low)
  {
    std::string path = sysfs_path_ + "/active_low";
    std::ofstream os;
    os.open(path.c_str());
    if (!os)
      throw_ios_writing_failure(path);

    os << active_low << std::endl;
    if (!os)
      throw_ios_writing_failure(path);

    os.close();
  }

  void itd_vd::restart(void)
  {
    std::string path = sysfs_path_ + "/restart";
    std::ofstream os;
    os.open(path.c_str());
    if (!os)
      throw_ios_writing_failure(path);

    os << "1" << std::endl;
    if (!os)
      throw_ios_writing_failure(path);

    os.close();
  }

  void itd_vd::start()
  {
    is_.open(dev_path_.c_str());
    if (!is_)
      throw_ios_reading_failure(dev_path_);
    thread::start();
  }

  void itd_vd::stop()
  {
    thread::stop();
    is_.close();
  }

  void itd_vd::run_vd()
  {
    restart();

    while(1)
    {
      if (!getline(is_, line_))
        throw_ios_reading_failure(dev_path_);

      long long sec = 0, usec = 0;
      int state = 0;
      std::istringstream iss(line_);
      iss >> sec >> usec >> state >> std::ws;
      if (!iss.eof())
        throw_ios_reading_failure(dev_path_);

      itd_event e(name(), time(sec, usec));
      e.set_device(name());
      e.set_state(static_cast<itd_state>(state));
      notify_subscribers(e);

      syslog() << debug << "itd_vd::run_vd: event sent, " << e << std::endl;
    }
  }

  void itd_vd::throw_ios_reading_failure(const std::string& file)
  {
    std::ostringstream oss;
    oss << "Reading file '" << file << "' failed";
    throw std::ios_base::failure(oss.str());
  }

  void itd_vd::throw_ios_writing_failure(const std::string& file)
  {
    std::ostringstream oss;
    oss << "Writing file '" << file << "' failed";
    throw std::ios_base::failure(oss.str());
  }

  void file_itd_vd::start()
  {
    dtm_start_ = time(0,0);
    dtm_file_ = time(0,0);
    initialized_ = false;
    itd_vd::start();
  }

  void file_itd_vd::run_vd()
  {
    while(1)
    {
      if (!getline(is_, line_))
        throw_ios_reading_failure(dev_path_);

      long long sec = 0, usec = 0;
      int state = 0;
      std::istringstream iss(line_);
      iss >> sec >> usec >> state >> std::ws;
      if (!iss.eof())
        throw_ios_reading_failure(dev_path_);

      if (!initialized_)
      {
        dtm_start_ = time::align(time::now(), aggr_period_);
        dtm_file_ = time(sec, usec);
        initialized_ = true;
      }

      time dtm = dtm_start_ + time(sec, usec) - dtm_file_;
      while(time::now() < dtm)
        cond_.timedwait(lock_, dtm);
      
      itd_event e(name(), dtm);
      e.set_device(name());
      e.set_state(static_cast<itd_state>(state));
      notify_subscribers(e);

      syslog() << debug << "file_itd_vd::run_vd: event sent, " << e << std::endl;
    }
  }


  rand_itd_vd::rand_itd_vd(const std::string& name)
  : virtual_device(name), state_(ITD_STATE_CLEAR), terminate_(false)
  {
    srand(::time(NULL));
  }

  void rand_itd_vd::stop()
  {
    lock_.lock();
    terminate_ = true;
    cond_.signal();
    lock_.unlock();
    join();
    terminate_ = false;
  }

  void rand_itd_vd::run_vd()
  {
    while(!terminate_)
    {
      // Wait between 0-30sec when in clear state
      // and 0-3sec when in blocked state
      // r is expressed in miliseconds
      long r = rand() % ((state_ == ITD_STATE_CLEAR) ? 30000 : 3000);
      time interval(r / 1000, (r % 1000) * 1000);

      time dtm = time::now() + interval;
      while (time::now() < dtm && !terminate_)
        cond_.timedwait(lock_, dtm);

      if (!terminate_)
      {
        state_ = (state_ == ITD_STATE_CLEAR) ? ITD_STATE_BLOCKED : ITD_STATE_CLEAR;
        itd_event e(name(), dtm);
        e.set_device(name());
        e.set_state(state_);
        notify_subscribers(e);
        syslog() << debug << "rand_itd_vd::run_vd: event sent, " << e << std::endl;
      }
    }
  }

  void filter_vd::set_itd_vd(virtual_device& itd_vd)
  {
    event_publisher& publisher = dynamic_cast<event_publisher&>(itd_vd);
    publisher.attach_subscriber(*this);
    itd_vd_name_ = itd_vd.name();
  }

  void filter_vd::set_aggr_timer_vd(virtual_device& aggr_timer_vd)
  {
    event_publisher& publisher = dynamic_cast<event_publisher&>(aggr_timer_vd);
    publisher.attach_subscriber(*this);
    aggr_timer_vd_name_ = aggr_timer_vd.name();
  }

  void filter_vd::set_led_state(bool on)
  {
    led_ctrl_os_ << std::string(on ? "1" : "0") << std::endl;
    if (!led_ctrl_os_)
    {
      std::ostringstream oss;
      oss << "Writting file '" << led_ctrl_os_ << "' failed";
      throw std::ios_base::failure(oss.str());
    }
  }

  void filter_vd::initialize()
  {
    led_ctrl_path_ = "/sys/devices/platform/gpio-itd.";
    led_ctrl_path_ += itd_vd_name_.substr(3);
    led_ctrl_path_ += "/led_ctrl";

    led_ctrl_os_.open(led_ctrl_path_.c_str());
    if (!led_ctrl_os_)
    {
      std::ostringstream oss;
      oss << "Opening file '" << led_ctrl_os_ << "' failed";
      throw std::ios_base::failure(oss.str());
    }

    set_led_state(true);

    fsm_event_ = itd_event(name(), time(0,0));
    device_ = std::string();
    fsm_state_ = FILTER_FSM_UNINITIALIZED;
    dtm_ = time(0,0);
    state_ = ITD_STATE_UNKNOWN;
    clear_time_ = time(0,0);
    blocked_time_ = time(0,0);
  }

  void filter_vd::destroy()
  {
    led_ctrl_os_.close();
  }

  void filter_vd::handle_event(const event& e)
  {
    switch(e.id())
    {
      case itd_event::ID:
      {
        const itd_event& ie = static_cast<const itd_event&>(e);
        if (e.publisher() == itd_vd_name_)
          handle_itd_event(ie);
        else
          syslog() << warning << "filter_vd: unknown event publisher, "
            << e << std::endl;
        break;
      }
      case timer_event::ID:
      {
        const timer_event& te = static_cast<const timer_event&>(e);
        if (e.publisher() == aggr_timer_vd_name_)
          handle_aggr_event(te);
        else
          syslog() << warning << "filter_vd: unknown event publisher, "
            << e << std::endl;
        break;
      }
      case filter_event::ID:
      {
        const filter_event& fe = static_cast<const filter_event&>(e);
        if (e.publisher() == name())
          handle_filter_event(fe);
        else
          syslog() << warning << "filter_vd: unknown event publisher, "
            << e << std::endl;
        break;
      }

      default:
        syslog() << warning << "filter_vd: unknown event id, "
          << e << std::endl;
    }
  }

  void filter_vd::handle_queue_empty()
  {
    filter_event ev(name(), time::now() + time(10,0), 10);
    ev.set_queue_empty(true);
    queue_event(ev);
  }

  void filter_vd::handle_itd_event(const itd_event& e)
  {
    if ((e.state() == ITD_STATE_CLEAR || e.state() == ITD_STATE_BLOCKED)
      && e.state() != state_)
    {
      syslog() << debug << "filter_vd::handle_itd_event: processing event, "
        << e << std::endl;


      syslog() << debug << "engage_delay_=" << engage_delay_.to_float() << std::endl;
      syslog() << debug << "release_delay_=" << release_delay_.to_float() << std::endl;

      switch(fsm_state_)
      {
        case FILTER_FSM_UNINITIALIZED:
          device_ = e.device();
          if (e.state() == ITD_STATE_CLEAR)
            update_fsm(FILTER_FSM_CLEAR);
          else if (e.state() == ITD_STATE_BLOCKED)
            update_fsm(FILTER_FSM_BLOCKED);
          break;

        case FILTER_FSM_CLEAR:
          if (e.state() == ITD_STATE_BLOCKED)
          {
            fsm_event_ = e;
            queue_event(filter_event(name(), e.dtm() + engage_delay_, 10));
            update_fsm(FILTER_FSM_ENGAGING);
          }
          break;

        case FILTER_FSM_ENGAGING:
          if (e.state() == ITD_STATE_CLEAR)
            update_fsm(FILTER_FSM_CLEAR);
          break;
        
        case FILTER_FSM_BLOCKED:
          if (e.state() == ITD_STATE_CLEAR)
          {
            fsm_event_ = e;
            queue_event(filter_event(name(), e.dtm() + release_delay_, 10));
            update_fsm(FILTER_FSM_RELEASING);
          }
          break;

        case FILTER_FSM_RELEASING:
          if (e.state() == ITD_STATE_BLOCKED)
            update_fsm(FILTER_FSM_BLOCKED);
          break;
      }

      update_stats(e.dtm());
      state_ = e.state();
    }
    else 
      syslog() << debug << "filter_vd::handle_itd_event: event discarded, "
        << e << std::endl;
  }

  void filter_vd::handle_filter_event(const filter_event& e)
  {
    syslog() << debug << "filter_vd::handle_filter_event: processing event, "
      << e << std::endl;

    update_stats(e.dtm());

    switch(fsm_state_)
    {
      case FILTER_FSM_ENGAGING:
        if (e.dtm() >= fsm_event_.dtm() + engage_delay_)
        { 
          send_stats_event(); 
          send_fsm_event();
          update_fsm(FILTER_FSM_BLOCKED);
        }
        break;

      case FILTER_FSM_RELEASING:
        if (e.dtm() >= fsm_event_.dtm() + release_delay_)
        {
          send_stats_event();
          send_fsm_event();
          update_fsm(FILTER_FSM_CLEAR);
        }
        break;

      default:
        if (e.queue_empty())
          send_stats_event();
    }
  }

  void filter_vd::handle_aggr_event(const timer_event& e)
  {

    if (fsm_state_ != FILTER_FSM_UNINITIALIZED)
    {
      syslog() << debug << "filter_vd::handle_aggr_event: processing event, "
        << e << std::endl;
      update_stats(e.dtm());
      send_stats_event(true);
    }
    else
      syslog() << debug << "filter_vd::handle_aggr_event: event discarded, "
        << e << std::endl;
  }

  std::string filter_vd::fsm_state_str(const fsm_state& state)
  {
    switch(state)
    {
      case FILTER_FSM_UNINITIALIZED:
        return "FILTER_FSM_UNINITIALIZED";
      case FILTER_FSM_CLEAR:
        return "FILTER_FSM_CLEAR";
      case FILTER_FSM_ENGAGING:
        return "FILTER_FSM_ENGAGING";
      case FILTER_FSM_BLOCKED:
        return "FILTER_FSM_BLOCKED";
      case FILTER_FSM_RELEASING:
        return "FILTER_FSM_RELEASING";
      default:
        return "?";
    }
  }

  void filter_vd::update_fsm(const fsm_state& fsm_state)
  {
    syslog() << debug << "filter_vd:update_fsm: " << fsm_state_str(fsm_state_)
      << " --> " << fsm_state_str(fsm_state) << std::endl;

    fsm_state_ = fsm_state;

    if (fsm_state_ ==  FILTER_FSM_CLEAR || fsm_state_ == FILTER_FSM_ENGAGING)
      set_led_state(false);
    else 
      set_led_state(true);
  }

  void filter_vd::update_stats(const time& dtm)
  {
    clear_time_ += (state_ == ITD_STATE_CLEAR) ? (dtm - dtm_) : time(0,0);
    blocked_time_ += (state_ == ITD_STATE_BLOCKED) ? (dtm - dtm_) : time(0,0);
    dtm_ = dtm;
    syslog() << debug << "filter_vd::update_stats: "
      << ",clear_time_=" << clear_time_.to_float()
      << ",blocked_time_=" << blocked_time_.to_float() << std::endl;
  }

  void filter_vd::send_fsm_event()
  {
    itd_event e(name(), fsm_event_.dtm());
    e.set_device(fsm_event_.device());
    e.set_state(fsm_event_.state());
    syslog() << debug << "filter_vd::send_fsm_event: " << e << std::endl;
    notify_subscribers(e);
  }

  void filter_vd::send_stats_event(bool aggr_period)
  {
    stats_event e(name(), dtm_, aggr_period ? 10 : 20);
    e.set_device(device_);
    e.set_clear_time(clear_time_);
    e.set_blocked_time(blocked_time_);
    e.set_aggr_period(aggr_period);
    syslog() << debug << "filter_vd::send_stats_event: " << e << std::endl;
    notify_subscribers(e);
    clear_time_ = time(0,0);
    blocked_time_ = time(0,0);
  }

  void aggr_vd::set_filter_vd(virtual_device& filter)
  {
    event_publisher& publisher = dynamic_cast<event_publisher&>(filter);
    publisher.attach_subscriber(*this);
    filter_vd_name_ = filter.name();
  }

  void aggr_vd::initialize()
  {
    db_.open(db_name_.c_str());
    db_.busy_timeout(db_timeout_);

    const char *insert_sql = 
      "INSERT INTO flow (counter_id, dtm, cnt, dark_time, work_time, flags)"
      " VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
    insert_stmt_.prepare(insert_sql);

    const char *update_sql = 
      "UPDATE flow SET cnt = ?3, dark_time = ?4,"
      " work_time = ?5, flags = ?6 WHERE counter_id = ?1 AND dtm = ?2";
    update_stmt_.prepare(update_sql);

    dtm_ = time::now();
    cnt_ = 0;
    clear_time_ = time(0,0);
    blocked_time_ = time(0,0);
    insert_flow();
  }

  void aggr_vd::destroy()
  {
    insert_stmt_.finalize();
    update_stmt_.finalize();
    db_.close();
  }

  void aggr_vd::handle_event(const event& e)
  {
    switch(e.id())
    {
      case itd_event::ID:
      {
        const itd_event& ie = static_cast<const itd_event&>(e);
        if (e.publisher() == filter_vd_name_)
          handle_itd_event(ie);
        else
          syslog() << warning << "aggr_vd: unknown event publisher, "
            << e << std::endl;
        break;
      }
      case stats_event::ID:
      {
        const stats_event& se = static_cast<const stats_event&>(e);
        if (e.publisher() == filter_vd_name_)
          handle_stats_event(se);
        else
          syslog() << warning << "aggr_vd: unknown event publisher, "
            << e << std::endl;
        break;
      }
      default:
        syslog() << debug << "aggr_vd: unknown event id, "
          << e << std::endl;
    }
  }

  void aggr_vd::handle_itd_event(const itd_event& e)
  {
    if (e.state() == ITD_STATE_CLEAR)
    {
      cnt_ += 1;
      syslog() << debug << "aggr_vd::handle_itd_event: transition 1->0 cnt_++"
        << ",dtm_=" << dtm_ << ",cnt_=" << cnt_
        << ",clear_time_=" << clear_time_.to_float()
        << ",blocked_time_=" << blocked_time_.to_float() << std::endl;
    }
    else
      syslog() << debug << "aggr_vd::handle_itd_event: transition 0->1"
        << ",dtm_=" << dtm_ << ",cnt_=" << cnt_
        << ",clear_time_=" << clear_time_.to_float()
        << ",blocked_time_=" << blocked_time_.to_float() << std::endl;
  }

  void aggr_vd::handle_stats_event(const stats_event& e)
  {
    clear_time_ += e.clear_time();
    blocked_time_ += e.blocked_time();
    update_flow(e.aggr_period() ? FLOW_FLAG_COMPLETE : FLOW_FLAG_OPEN);
    syslog() << debug << "aggr_vd::handle_stats_event: updating"
      << ",dtm_=" << dtm_ << ",cnt_=" << cnt_
      << ",clear_time_=" << clear_time_.to_float()
      << ",blocked_time_=" << blocked_time_.to_float() << std::endl;

    if (e.aggr_period())
    {
      dtm_ = e.dtm();
      cnt_ = 0;
      clear_time_ = time(0,0);
      blocked_time_ = time(0,0);
      insert_flow();
      syslog() << debug << "aggr_vd::handle_stats_event: new aggregation period"
        << ",dtm_=" << dtm_ << std::endl;
    }
  }

  void aggr_vd::update_flow(const flow_flag& flag)
  {
    update_stmt_.reset();
    update_stmt_.clear_bindings();
    update_stmt_.bind_int(1, counter_id_);
    update_stmt_.bind_int64(2, dtm_.sec());
    update_stmt_.bind_int(3, cnt_);
    update_stmt_.bind_int64(4, blocked_time_.to_msec());
    update_stmt_.bind_int64(5, (clear_time_ + blocked_time_).to_msec());
    update_stmt_.bind_int(6, flag);
    update_stmt_.step();
  }

  void aggr_vd::insert_flow()
  {
    try
    {
      insert_stmt_.reset();
      insert_stmt_.clear_bindings();
      insert_stmt_.bind_int(1, counter_id_);
      insert_stmt_.bind_int64(2, dtm_.sec());
      insert_stmt_.bind_int(3, cnt_);
      insert_stmt_.bind_int64(4, blocked_time_.to_msec());
      insert_stmt_.bind_int64(5, (clear_time_ + blocked_time_).to_msec());
      insert_stmt_.bind_int(6, FLOW_FLAG_OPEN);
      insert_stmt_.step();
    }
    catch(std::exception& e)
    {
      syslog() << info << "aggr_vd::insert_flow failed" << std::endl;
    }
  }

  void event_logger_vd::initialize()
  {
    db_.open(db_name_.c_str());
    db_.busy_timeout(db_timeout_);
    const char *sql = "INSERT INTO events (itd, dtmms, state) VALUES (?1, ?2, ?3)";
    stmt_.prepare(sql);
  }

  void event_logger_vd::destroy()
  {
    stmt_.finalize();
    db_.close();
  }

  void event_logger_vd::add_publisher(virtual_device& vd)
  {
    event_publisher& publisher = dynamic_cast<event_publisher&>(vd);
    publisher.attach_subscriber(*this);
  }

  void event_logger_vd::handle_event(const event& e)
  {
    switch(e.id())
    {
      case itd_event::ID:
      {
        syslog() << debug << "event_loger_vd: processing event, " << e << std::endl;
        const itd_event& ie = dynamic_cast<const itd_event&>(e);
        stmt_.reset();
        stmt_.clear_bindings();
        stmt_.bind_text(1, ie.device());
        stmt_.bind_int64(2, ie.dtm().to_msec());
        stmt_.bind_int(3, ie.state());
        stmt_.step();
        break;
      }
    default:
      syslog() << warning << "event_logger_vd: unhandled event, " << e << std::endl;
    }
  }

  void itd_farm::create()
  {
    sqlite3cc::conn db;
    db.open(config_db_name_.c_str());
    db.busy_timeout(db_timeout_);
    config config(db);

    long aggr_period_mins = config.entry_long("device","aggr-period-mins", 15);

    aggr_timer_vd* aggr_timer = new aggr_timer_vd("timer0");
    aggr_timer->set_aggr_period(time::from_mins(aggr_period_mins));
    add(aggr_timer);

    event_logger_vd* event_logger = NULL;

    for (int i = 0; i < 4; i++)
    {
      std::string counter_name = "counter" + to_string(i);

      bool enabled  = config.entry_bool(counter_name, "enabled", false);
      long counter_id = config.entry_long(counter_name, "counter_id", 0);
      std::string dev_name = config.entry(counter_name, "dev");
      long engage_delay_ms = config.entry_long(counter_name, "dev-engage", 200);
      long release_delay_ms = config.entry_long(counter_name, "dev-release", 200);
      bool active_low = config.entry_bool(counter_name, "dev-active-low", false);
      std::string log_events = config.entry(counter_name, "log-events");
      std::string test_mode = config.entry(counter_name, "test-mode");
      std::string test_file = config.entry(counter_name, "test-file");

      if (!enabled)
        continue;

      virtual_device* itd = NULL;
      if (test_mode == "file")
      {
        itd = new file_itd_vd(dev_name, test_file);
        static_cast<file_itd_vd*>(itd)->set_aggr_period(time::from_mins(aggr_period_mins));
      }
      else if (test_mode == "rand")
        itd = new rand_itd_vd(dev_name);
      else
      {
        itd = new itd_vd(dev_name);
        static_cast<itd_vd*>(itd)->set_active_low(active_low);
      }
      add(itd);

      filter_vd* filter = new filter_vd("filter" + to_string(i));
      filter->set_delay(time::from_msec(0));
      filter->set_itd_vd(*itd);
      filter->set_aggr_timer_vd(*aggr_timer);
      filter->set_engage_delay(time::from_msec(engage_delay_ms));
      filter->set_release_delay(time::from_msec(release_delay_ms));
      add(filter);

      aggr_vd* aggr = new aggr_vd("aggr" + to_string(i), data_db_name_, db_timeout_);
      aggr->set_delay(time::from_msec(500));
      aggr->set_filter_vd(*filter);
      aggr->set_aggr_period(time::from_mins(aggr_period_mins));
      aggr->set_counter_id(counter_id);
      add(aggr);

      if (log_events == "raw" || log_events == "filered" )
      {
        if (event_logger == NULL)
        {
          event_logger = new event_logger_vd("log0", data_db_name_, db_timeout_);
          add(event_logger);
        }
        event_logger->set_delay(time::from_msec(500));
        event_logger->add_publisher((log_events == "raw") ? *itd : *filter);
      }
    }

    db.close();
  }
}

