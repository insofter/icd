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

itd_vd::itd_vd(const std::string& name)
: virtual_device(name), dev_path_("/dev/" + name)
{
  is_.open(dev_path_.c_str());
  if (!is_)
    throw_ios_reading_failure(dev_path_);
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

rand_itd_vd::rand_itd_vd(const std::string& name)
: virtual_device(name), state_(ITD_STATE_CLEAR)
{
  srand(::time(NULL));
}

void rand_itd_vd::run()
{
  while(1)
  {
    // Wait between 0-30sec when in clear state
    // and 0-3sec when in blocked state
    // r is expressed in miliseconds
    long r = rand() % ((state_ == ITD_STATE_CLEAR) ? 30000 : 3000);
    time interval(r / 1000, (r % 1000) * 1000);

    time dtm = time::now() + interval;
    while (time::now() < dtm)
      cond_.timedwait(lock_, dtm);

    state_ = (state_ == ITD_STATE_CLEAR) ? ITD_STATE_BLOCKED : ITD_STATE_CLEAR;

    itd_event e(name(), dtm);
    e.set_device(name());
    e.set_state(state_);

    notify_subscribers(e);
  }
}

/*
file_itd_vd

simple_aggr_vd

flow_writer_vd*/

void filter_vd::handle_event(const event& e)
{
  if ((e.id() == itd_event::ID && e.publisher() == itd_vd_name_)
    || (e.id() == timer_event::ID 
      && (e.publisher() == aggr_vd_name_ || e.publisher() == name())))
  {
    update_stats(e);
    update_fsm(e);
  }
  else
    syslog() << "filter_vd: unknown event or publisher, " << e << std::endl;
}


void filter_vd::update_stats(const event& e)
{
  if (state_ != ITD_STATE_UNKNOWN)
  {
    statistics_event se(name(), e.dtm());
    se.set_work_time(e.dtm() - dtm_);
    se.set_dark_time((state_ == ITD_STATE_BLOCKED) ? (e.dtm() - dtm_) : time(0,0));
    notify_subscribers(se);
  }

  if (e.id() == itd_event::ID)
    state_ = static_cast<const itd_event&>(e).state();

  dtm_ = e.dtm();
}

void filter_vd::update_fsm(const event& e)
{
  itd_state state = ITD_STATE_UNKNOWN;
  if (e.id() == itd_event::ID)
    state = static_cast<const itd_event&>(e).state();
  
  switch(fsm_state_)
  {
    case FILTER_FSM_UNINITIALIZED:
      if (state == ITD_STATE_CLEAR)
        fsm_state_ = FILTER_FSM_CLEAR;
      else if (state == ITD_STATE_BLOCKED)
        fsm_state_ = FILTER_FSM_BLOCKED;
      break;

    case FILTER_FSM_CLEAR:
      if (state == ITD_STATE_BLOCKED)
      {
        fsm_event_ = static_cast<const itd_event&>(e);
        fsm_state_ = FILTER_FSM_ENGAGING;
        queue_event(timer_event(name(), e.dtm() + engage_time_));
      }
      break;

    case FILTER_FSM_ENGAGING:
      if (fsm_event_.dtm() > e.dtm() + engage_time_)
      {  
        send_fsm_event();
        fsm_state_ = FILTER_FSM_BLOCKED;
      }
      else if (state == ITD_STATE_CLEAR)
        fsm_state_ = FILTER_FSM_CLEAR;
      break;
      
    case FILTER_FSM_BLOCKED:
      if (state == ITD_STATE_CLEAR)
      {
        fsm_event_ = static_cast<const itd_event&>(e);
        fsm_state_ = FILTER_FSM_RELEASING;
        queue_event(timer_event(name(), e.dtm() + release_time_));
      }
      break;

    case FILTER_FSM_RELEASING:
      if (fsm_event_.dtm() > e.dtm() + release_time_)
      {
        send_fsm_event();
        fsm_state_ = FILTER_FSM_CLEAR;
      }
      else if (state == ITD_STATE_BLOCKED)
        fsm_state_ = FILTER_FSM_BLOCKED;
      break;

    default:
      throw std::runtime_error("filter_vd: Invalid fsm_state");
  }
}

void filter_vd::send_fsm_event()
{
  itd_event e(name(), fsm_event_.dtm());
  e.set_device(fsm_event_.device());
  e.set_state(fsm_event_.state());
  notify_subscribers(e);
}

void aggr_vd::handle_event(const event& e)
{
}



event_writer_vd::event_writer_vd(const std::string& name, const std::string& db_name, int db_timeout)
: event_subscriber(name), stmt_(db_) 
{
  db_.open(db_name.c_str());
  db_.busy_timeout(db_timeout);
  const char *sql = "INSERT INTO events (itd, dtmms, state) VALUES (?1, ?2, ?3)";
  stmt_.prepare(sql);
}

event_writer_vd::~event_writer_vd()
{
  stmt_.finalize();
  db_.close();
}

void event_writer_vd::handle_event(const event& e)
{
  switch(e.id())
  {
    case itd_event::ID:
    {
      syslog() << "event_writer_vd: processing event, " << e << std::endl;
      const itd_event& ie = dynamic_cast<const itd_event&>(e);
      stmt_.reset();
      stmt_.clear_bindings();
      stmt_.bind_text(1, ie.device());
      stmt_.bind_int64(2, (long long)ie.dtm().sec() * 1000 + (long long)ie.dtm().usec() / 1000);
      stmt_.bind_int(3, ie.state());
      stmt_.step();
      break;
    }
  default:
    syslog() << "event_writer_vd: unhandled event, " << e << std::endl;
  }
}
}

