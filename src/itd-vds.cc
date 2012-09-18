#include "itd-vds.h"
#include <cstdlib>

namespace icd
{
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

itd_vd::itd_vd(const std::string& name, const std::string& dev_path)
: virtual_device(name)
{
  dev_path_ = dev_path.empty() ? "/dev/" + name : dev_path;

  sysfs_path_ = "/sys/devices/platform/gpio-itd.";
  sysfs_path_ += name.substr(3);

  is_.open(dev_path_.c_str());
  if (!is_)
    throw_ios_reading_failure(dev_path_);
}

itd_vd::~itd_vd()
{
  is_.close();
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

void itd_vd::run()
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

    itd_event e(name(), time(sec, usec));
    e.set_device(name());
    e.set_state(static_cast<itd_state>(state));
    notify_subscribers(e);
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

void file_itd_vd::run()
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

void rand_itd_vd::run()
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

void filter_vd::initialize()
{
  fsm_event_ = itd_event(name(), time(0,0));
  device_ = std::string();
  fsm_state_ = FILTER_FSM_UNINITIALIZED;
  dtm_ = time(0,0);
  stats_sent_dtm_ = time(0,0);
  state_ = ITD_STATE_UNKNOWN;
  clear_time_ = time(0,0);
  blocked_time_ = time(0,0);
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
        syslog() << "filter_vd: unknown event publisher, " << e << std::endl;
      break;
    }
    case timer_event::ID:
    {
      const timer_event& te = static_cast<const timer_event&>(e);
      if (e.publisher() == name())
        handle_filter_event(te);
      else if (e.publisher() == aggr_timer_vd_name_)
        handle_aggr_event(te);
      else
        syslog() << "filter_vd: unknown event publisher, " << e << std::endl;
      break;
    }
    default:
      syslog() << "filter_vd: unknown event id, " << e << std::endl;
  }
}

void filter_vd::handle_itd_event(const itd_event& e)
{
  if ((e.state() == ITD_STATE_CLEAR || e.state() == ITD_STATE_BLOCKED)
    && e.state() != state_)
  {
    syslog() << "filter_vd::handle_itd_event: processing event, "
      << e << std::endl;

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
          queue_event(timer_event(name(), e.dtm() + engage_delay_, 10));
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
          queue_event(timer_event(name(), e.dtm() + release_delay_, 10));
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
    syslog() << "filter_vd::handle_itd_event: event discarded, "
      << e << std::endl;
}

void filter_vd::handle_filter_event(const timer_event& e)
{
  syslog() << "filter_vd::handle_filter_event: processing event, "
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
  }
}

void filter_vd::handle_aggr_event(const timer_event& e)
{

  if (fsm_state_ != FILTER_FSM_UNINITIALIZED)
  {
    syslog() << "filter_vd::handle_aggr_event: processing event, "
      << e << std::endl;
    update_stats(e.dtm());
    send_stats_event();
  }
  else
    syslog() << "filter_vd::handle_aggr_event: event discarded, "
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
  syslog() << "filter_vd:update_fsm: " << fsm_state_str(fsm_state_)
    << " --> " << fsm_state_str(fsm_state) << std::endl;
  fsm_state_ = fsm_state;
}

void filter_vd::update_stats(const time& dtm)
{
  clear_time_ += (state_ == ITD_STATE_CLEAR) ? (dtm - dtm_) : time(0,0);
  blocked_time_ += (state_ == ITD_STATE_BLOCKED) ? (dtm - dtm_) : time(0,0);
  dtm_ = dtm;
  syslog() << "filter_vd::update_stats: "
    << ",clear_time_=" << clear_time_.to_float()
    << ",blocked_time_=" << blocked_time_.to_float() << std::endl;
}

void filter_vd::send_fsm_event()
{
  itd_event e(name(), fsm_event_.dtm());
  e.set_device(fsm_event_.device());
  e.set_state(fsm_event_.state());
  syslog() << "filter_vd::send_fsm_event: " << e << std::endl;
  notify_subscribers(e);
}

void filter_vd::send_stats_event()
{
  if (stats_sent_dtm_ < dtm_)
  {
    stats_event e(name(), dtm_, 10);
    e.set_device(device_);
    e.set_clear_time(clear_time_);
    e.set_blocked_time(blocked_time_);
    syslog() << "filter_vd::send_stats_event: " << e << std::endl;
    notify_subscribers(e);
    clear_time_ = time(0,0);
    blocked_time_ = time(0,0);
  }
  stats_sent_dtm_ = dtm_;
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
    "INSERT INTO flow (itd, dtm, cnt, dark_time, work_time, flags)"
    " VALUES (?1, ?2, ?3, ?4, ?5, ?6)";
  insert_stmt_.prepare(insert_sql);

  const char *update_sql = 
    "UPDATE flow SET cnt = ?3, dark_time = ?4,"
    " work_time = ?5, flags = ?6 WHERE itd = ?1 AND dtm = ?2";
  update_stmt_.prepare(update_sql);

  device_ = std::string();
  dtm_ = time(0,0);
  cnt_ = 0;
  clear_time_ = time(0,0);
  blocked_time_ = time(0,0);
  initialized_ = false;
}

void aggr_vd::destroy()
{
  insert_stmt_.finalize();
  update_stmt_.finalize();
  db_.close();
}

void aggr_vd::handle_event(const event& e)
{
  if (e.id() == itd_event::ID && e.publisher() == filter_vd_name_)
  {
    const itd_event& ie = static_cast<const itd_event&>(e);
    handle_itd_event(ie);
  }
  else if (e.id() == stats_event::ID && e.publisher() == filter_vd_name_)
  {
    const stats_event& se = static_cast<const stats_event&>(e);
    handle_stats_event(se);
  }
  else
    syslog() << "aggr_vd: unknown event or publisher, " << e << std::endl;
}

void aggr_vd::handle_itd_event(const itd_event& e)
{
  if (e.state() == ITD_STATE_CLEAR)
  {
    cnt_ += 1;
    syslog() << "aggr_vd::handle_itd_event: transition 1->0 cnt_++"
      << ",dtm_=" << dtm_ << ",cnt_=" << cnt_
      << ",clear_time_=" << clear_time_.to_float()
      << ",blocked_time_=" << blocked_time_.to_float() << std::endl;
  }
  else
    syslog() << "aggr_vd::handle_itd_event: transition 0->1"
      << ",dtm_=" << dtm_ << ",cnt_=" << cnt_
      << ",clear_time_=" << clear_time_.to_float()
      << ",blocked_time_=" << blocked_time_.to_float() << std::endl;
}

void aggr_vd::handle_stats_event(const stats_event& e)
{
  if (initialized_ && (e.dtm() < dtm_ || e.dtm() > dtm_ + aggr_period_))
    syslog() << "aggr_vd::handle_stats_event: event outside aggr period"
      << ",dtm_=" << dtm_ << std::endl;

  clear_time_ += e.clear_time();
  blocked_time_ += e.blocked_time();

  if (!initialized_)
  {
    device_ = e.device();
    dtm_ = time::align(e.dtm() - time(0,1), aggr_period_);
    initialized_ = true;
    insert_flow();
    syslog() << "aggr_vd::handle_stats_event: initializing"
      << ",dtm_=" << dtm_ << std::endl;
  }
  else
  {
     update_flow();
     syslog() << "aggr_vd::handle_stats_event: updating"
      << ",dtm_=" << dtm_ << ",cnt_=" << cnt_
      << ",clear_time_=" << clear_time_.to_float()
      << ",blocked_time_=" << blocked_time_.to_float() << std::endl;
  }

  if (e.dtm() == dtm_ + aggr_period_)
  {
    dtm_ = dtm_ + aggr_period_;
    cnt_ = 0;
    clear_time_ = time(0,0);
    blocked_time_ = time(0,0);; 
    insert_flow();
    syslog() << "aggr_vd::handle_stats_event: new aggregation period"
      << ",dtm_=" << dtm_ << std::endl;
  }
}

void aggr_vd::update_flow()
{
  update_stmt_.reset();
  update_stmt_.clear_bindings();
  update_stmt_.bind_text(1, device_);
  update_stmt_.bind_int64(2, dtm_.to_msec());
  update_stmt_.bind_int(3, cnt_);
  update_stmt_.bind_int64(4, blocked_time_.to_msec());
  update_stmt_.bind_int64(5, (clear_time_ + blocked_time_).to_msec());
  update_stmt_.bind_int(6, FLOW_FLAG_NOT_SENT);
  update_stmt_.step();
}

void aggr_vd::insert_flow()
{
  insert_stmt_.reset();
  insert_stmt_.clear_bindings();
  insert_stmt_.bind_text(1, device_);
  insert_stmt_.bind_int64(2, dtm_.to_msec());
  insert_stmt_.bind_int(3, cnt_);
  insert_stmt_.bind_int64(4, blocked_time_.to_msec());
  insert_stmt_.bind_int64(5, (clear_time_ + blocked_time_).to_msec());
  insert_stmt_.bind_int(6, FLOW_FLAG_NOT_SENT);
  insert_stmt_.step();
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
      syslog() << "event_loger_vd: processing event, " << e << std::endl;
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
    syslog() << "event_logger_vd: unhandled event, " << e << std::endl;
  }
}
}

