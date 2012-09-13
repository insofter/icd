#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string> 
#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <libgen.h>
#include <queue>
#include <signal.h>

#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include "daemonizer.h"
#include "syslogstream.h"
#include "threads.h"

void print_usage(char *argv0)
{
  std::cerr << 
    "\n"
    "Usage: " << basename(argv0) << " OPTION...\n"
    "\n"
    "A daemon communicating with itd devices. After configuration of\n"
    "the device the tool reads input events from it and stores them\n"
    "in the database.\n"
    "\n"
    "Options:\n"
    "  -d|--db=DB_NAME              Database file path; Mandatory option\n"
    "  -t|--timeout=TIMEOUT_MS      Timeout when waiting for acces to the database in ms\n"
    "  -i|--device=DEVICE           Device name; E.g. itd0, itd1... This is mandatory option\n"
    "  -r|--rand                    Device emulation: generates input events randomly\n"
    "  -k|--kbd                     Device emulation: generates event when based on keyboard\n"
    "  -b|--daemon                  Run as a daemon\n"
    "  -p|--pidfile=FILE            Create pid file (if a daemon)\n"
    "  -h|--help\n"
    "  -v|--version\n"
    << std::endl;
}

void print_version(char *argv0)
{
  std::cerr << basename(argv0) << " " << version << "\n"
    << copyright << std::endl;
}
/*
namespace icd
{

  enum itd_state 
  {
    UNINITIALIZED = -1,
    CLEAR = 0,
    BLOCKED = 1,
  };

  class itd_time
  {
    public:
      itd_time();
      itd_time(long sec, long usec) : _sec(sec), _usec(usec) {}
      itd_time(const itd_time& ref) : _sec(ref._sec), _usec(ref._usec) {}

      ~itd_time() {}

      itd_time& operator =(const itd_time& ref)
      {
        _sec = ref._sec;
        _usec = ref._usec;
        return *this; 
      }

      long sec() { return _sec; }
      long usec() { return _usec; }

      friend std::ostream& operator <<(std::ostream &os, const itd_time &ref);

    private:
      long _sec;
      long _usec;
  };

  itd_time::itd_time()
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    _sec = tv.tv_sec;
    _usec = tv.tv_usec;
  }

  std::ostream& operator <<(std::ostream &os,const itd_time &ref)
  {
    char s[32];
    struct tm result;
    struct tm *m = gmtime_r((const time_t *)&ref._sec, &result);
    strftime(s, sizeof(s), "%Y-%m-%dT%H:%M:%S", m);
    os << s << "." << std::setw(3) << std::setfill('0') << ref._usec / 1000;
    return os;
  }

  class itd_event
  {
    public:
      itd_event() : _state(UNINITIALIZED) {}
      itd_event(const itd_state& state) : _state(state) {}
      itd_event(const itd_state& state, const itd_time& dtm)
        : _state(state), _dtm(dtm) {}
      itd_event(const itd_event& ref)
        : _state(ref._state), _dtm(ref._dtm) {}

      ~itd_event() {}

      itd_event& operator =(const itd_event& ref)
      {
        _state = ref._state;
        _dtm = ref._dtm;
        return *this;
      }

      itd_state state() { return _state; }
      itd_time dtm() { return _dtm; }

    private:
      itd_state _state;
      itd_time _dtm;
  };

  class itd_base_device
  {
    public:
      itd_base_device() {}
      virtual ~itd_base_device() {}

      virtual itd_event next() = 0;
      virtual void cleanup() {}
  };

  class itd_device : public itd_base_device
  {
    public :
      itd_device(const std::string& name);
      virtual ~itd_device() {}

      std::string name() { return name_; }

      void set_active_low(int active_low);
      void set_engage_delay_ms(long engage_delay_ms);
      void set_release_delay_ms(long release_delay_ms);

      virtual itd_event next();
      virtual void cleanup();

    private:
      void throw_ios_reading_failure(const std::string& file);
      void throw_ios_writing_failure(const std::string& file);

      std::string name_;
      std::string dev_path_;
      std::string sysfs_path_;
      std::ifstream is_;
  };

  itd_device::itd_device(const std::string& name)
  {
    name_ = name;
    dev_path_ = std::string("/dev/") + name; 
    sysfs_path_ = std::string("/sys/devices/platform/gpio-itd.");
    sysfs_path_ += name.substr(3);

    is_.open(dev_path_.c_str());
    if (!is_)
      throw_ios_reading_failure(dev_path_);
  }

  void itd_device::set_active_low(int active_low)
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

  void itd_device::set_engage_delay_ms(long engage_delay_ms)
  {
    std::string path = sysfs_path_ + "/engage_delay_usec";
    std::ofstream os;

    os.open(path.c_str());
    if (!os)
      throw_ios_writing_failure(path);

    os << engage_delay_ms * 1000 << std::endl;
    if (!os)
      throw_ios_writing_failure(path);

    os.close();
  }

  void itd_device::set_release_delay_ms(long release_delay_ms)
  {
    std::string path = sysfs_path_ + "/release_delay_usec";
    std::ofstream os;

    os.open(path.c_str());
    if (!os)
      throw_ios_writing_failure(path);

    os << release_delay_ms * 1000 << std::endl;
    if (!os)
      throw_ios_writing_failure(path);

    os.close();
  }

  itd_event itd_device::next()
  {
    std::string line;
    if (!getline(is_, line))
      throw_ios_reading_failure(dev_path_);

    long long sec = 0, usec = 0;
    int state = 0;
    std::istringstream iss(line);
    iss >> sec >> usec >> state >> std::ws;
    if (!iss.eof())
      throw_ios_reading_failure(dev_path_);

    return itd_event((itd_state)state, itd_time(sec, usec));
  }

  void itd_device::cleanup()
  {
    is_.close();
  }

  void itd_device::throw_ios_reading_failure(const std::string& file)
  {
    std::ostringstream oss;
    oss << "Reading file '" << file << "' failed";
    throw std::ios_base::failure(oss.str());
  }

  void itd_device::throw_ios_writing_failure(const std::string& file)
  {
    std::ostringstream oss;
    oss << "Writing file '" << file << "' failed";
    throw std::ios_base::failure(oss.str());
  }

  class itd_rand_emulator : public itd_base_device
  {
    public:
      itd_rand_emulator();

      virtual itd_event next();

    private:
      itd_state _state;

      int msleep(unsigned long msec);
  };

  itd_rand_emulator::itd_rand_emulator() : _state(UNINITIALIZED)
  {
    srand(time(NULL));
  }

  itd_event itd_rand_emulator::next()
  {
    // Wait between 0-30sec when clear
    // and 0-3sec when blocked or uninitialzed
    long mod = (_state == CLEAR) ? 30000 : 3000;
    long period = rand() % mod;

    msleep(period);

    _state = (_state == CLEAR) ? BLOCKED : CLEAR;
    return itd_event(_state);
  }

  int itd_rand_emulator::msleep(unsigned long milisec)
  {
    struct timespec req={0};
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    while(nanosleep(&req,&req)==-1)
         continue;
    return 1;
  }

  class itd_kbd_emulator : public itd_base_device
  {
    public:
      itd_kbd_emulator() : _state(UNINITIALIZED) {}
     
      virtual itd_event next();

    private:
      itd_state _state;
  };

  itd_event itd_kbd_emulator::next()
  {
    std::string s;
    std::getline(std::cin, s);
    _state = (_state == CLEAR) ? BLOCKED : CLEAR;
    return itd_event(_state);
  }

}

void throw_bad_config_entry(const std::string& section, const std::string& key)
{
  std::ostringstream oss;
  oss << "Reading db config option '" << section << ":" << key << "' failed."
    " Missing or invalid value.";
  throw std::runtime_error(oss.str());
}

icd::itd_base_device *create_device(sqlite3cc::conn& db,
  const std::string& dev_name, const std::string& emulator)
{
  icd::itd_base_device *dev = NULL;

  icd::config config(db);
  bool enabled  = config.entry_bool(dev_name, "enabled", false);
  long engage_delay_ms = config.entry_long(dev_name, "engage-delay-ms", false);
  long release_delay_ms = config.entry_long(dev_name, "release-delay-ms", false);
  bool active_low = config.entry_bool(dev_name, "active-low", false);

  if (!enabled)
  {
    std::ostringstream oss;
    oss << "Device '" << dev_name << "' is not enabled.";
    throw std::runtime_error(oss.str());
  }

  if (emulator.empty())
  {
    icd::itd_device *itddev = new icd::itd_device(dev_name);
    itddev->set_engage_delay_ms(engage_delay_ms);
    itddev->set_release_delay_ms(release_delay_ms);
    itddev->set_active_low(active_low);
    dev = itddev;
  }
  else if (emulator == "rand")
    dev = new icd::itd_rand_emulator();
  else
    dev = new icd::itd_kbd_emulator();

  return dev;
}

void config_itddev(sqlite3cc::conn& db, icd::itd_device& itddev)
{
  std::istringstream iss;
  std::string s;
}

int main(int argc, char *argv[])
{
  syslogstream syslog(basename(argv[0]), LOG_PERROR);

  try
  {
    daemonizer daemon;
    std::string db_name;
    int db_timeout = 60000; // default timeout is 60 seconds
    std::string dev_name;
    std::string emulator;
    bool run_as_daemon = false;
    bool exit = false;

    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "device", required_argument, 0, 'i' },
      { "rand", no_argument, 0, 'r' },
      { "kbd", no_argument, 0, 'k' },
      { "daemon", no_argument, 0, 'b' },
      { "pidfile", required_argument, 0, 'p' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:i:rkbphv", long_options, &option_index);
      if (ch == -1)
        break;

      switch(ch)
      {
        case 'd':
          db_name = optarg;
          break;
        case 't':
          db_timeout = strtol(optarg, NULL, 0);
          break;
        case 'i':
          dev_name = optarg;
          break;
        case 'k':
          emulator = "kbd";
          break;
        case 'r':
          emulator = "rand";
          break;
        case 'b':
          run_as_daemon = true;
          break;
        case 'p':
          daemon.pid_file(optarg);
          break;
        case 'h':
          print_usage(argv[0]);
          exit = true;
          break;
        case 'v':
          print_version(argv[0]);
          exit = true;
          break;
        default:
        {
          std::ostringstream oss;
          oss << "Unknown option '" << char(ch) << "'";
          throw std::runtime_error(oss.str());
        }
      }

      if (exit)
        break;
    }

    if (!exit && db_name.empty())
      throw std::runtime_error("Missing '--db' parameter");

    if (!exit && dev_name.empty())
      throw std::runtime_error("Missing '--device' parameter");

    if (!exit && run_as_daemon)
      exit = daemon.fork();

    if (!exit)
    {
      sqlite3cc::conn db;
      db.open(db_name.c_str());
      db.busy_timeout(db_timeout);

      sqlite3cc::stmt stmt(db);
      const char *sql = "INSERT INTO events (itd, dtmms, state) VALUES (?1, ?2, ?3)";
      stmt.prepare(sql);

      icd::itd_base_device *dev = create_device(db, dev_name, emulator);


      while(1)
      {
        icd::itd_event e = dev->next();
        std::ostringstream oss;
        oss << e.dtm();

        syslog << dev_name << " " << e.dtm() << " " << e.state() << std::endl;

        stmt.bind_text(1, dev_name.c_str());
        stmt.bind_int64(2, (long long)e.dtm().sec() * 1000 + (long long)e.dtm().usec() / 1000);
        stmt.bind_int(3, e.state());

        stmt.step();

        stmt.reset();
        stmt.clear_bindings();
      }

      stmt.finalize();

      dev->cleanup();
      delete dev;

      db.close();
    }
  }
  catch(std::exception& e)
  {
    syslog << basename(argv[0]) << " error: " << e.what()  << std::endl;
    return 1;
  }

  return 0;
}
*/

//------------
#include <map>
#include <pthread.h>

namespace icd
{

#define EVENT_TYPE_TIMER (1)
#define EVENT_TYPE_ITD (2)

class event
{
public:
  event(const std::string& publisher, const time& timestamp)
    : publisher_(publisher), timestamp_(timestamp) {}
  ~event() {}

  virtual int type() const = 0;
  virtual event *clone() const = 0;

  std::string publisher() const { return publisher_; }
  time timestamp() const { return timestamp_; }

  friend std::ostream& operator <<(std::ostream &os, const event &ref);

private:
  std::string publisher_;
  time timestamp_;

  virtual void dbg_print(std::ostream& os) const = 0;
};

std::ostream& operator <<(std::ostream &os, const event &ref)
{
  ref.dbg_print(os);
  return os;
}

class timer_event : public event
{
public:
  timer_event(const std::string& publisher, const time& timestamp)
    : event(publisher, timestamp) {}
  ~timer_event() {}

  virtual int type() const { return EVENT_TYPE_TIMER; } 
  virtual event *clone() const { return new timer_event(*this); }

private:
  virtual void dbg_print(std::ostream& os) const
  {
    os << "timer_event(" << publisher() << "," << timestamp() << ")";
  }
};

enum itd_state 
{
  CLEAR = 0,
  BLOCKED = 1,
};

class itd_event : public event
{
public:
  itd_event(const std::string& publisher, const time& timestamp)
    : event(publisher, timestamp), state_(CLEAR) {}
  ~itd_event() {}

  virtual int type() const { return EVENT_TYPE_ITD; }
  virtual event *clone() const { return new itd_event(*this); }

  void set_device(const std::string& device) { device_ = device; }
  void set_dtm(const time& dtm) { dtm_ = dtm; }
  void set_state(const itd_state& state) { state_ = state; }

  std::string device() const { return device_; }
  time dtm() const { return dtm_; }
  itd_state state() const { return state_; }

private:
  std::string device_;
  time dtm_;
  itd_state state_;

  virtual void dbg_print(std::ostream& os) const
  {
    os << "itd_event(" << publisher() << "," << timestamp() << ","
      << ((state_ == CLEAR) ? "CLEAR" : "BLOCKED") << ")";
  }
};



class virtual_device : public thread
{
public:
  virtual_device(const std::string& name) : syslog_(name), name_(name) {}
  ~virtual_device() {}

  std::string name() const { return name_; }
  std::ostream &syslog() { return syslog_; }

private:
  icd::syslogstream syslog_;
  std::string name_;
};




class event_subscriber : public virtual_device
{
public:
  event_subscriber(const std::string& name) : virtual_device(name) {}
  ~event_subscriber() {}

  time delay() { mutex_locker ml(lock_); return delay_; }
  void set_delay(const time& delay) { mutex_locker ml(lock_); delay_ = delay; }

  void process_notification(const event& e);

protected:
  virtual void handle_event(const event& e) = 0;

private:
  struct prioritize_events
  {
    // return true if e2 is older (=higher priority) that e1
    bool operator()(const event* &e1, const event* &e2) const
    {
      return e1->timestamp() > e2->timestamp();
    }
  };

  std::priority_queue<const event *, std::vector<const event *>, prioritize_events> queue_;
  time delay_;
  mutex lock_;
  condition queue_cond_;
  condition delay_cond_;

  virtual void run();
};

class event_publisher : public virtual_device
{
public:
  event_publisher(const std::string& name) : virtual_device(name) {}
  ~event_publisher() {}

  void subscribe(event_subscriber& subscriber);
  void unsubscribe(event_subscriber& subscriber);

protected:
  void notify_subscribers(const event& e);

private:
  std::map<std::string, event_subscriber&> subscribers_;
  mutex lock_;
};

void event_subscriber::process_notification(const event& e)
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
      time timestamp = queue_.top()->timestamp() + delay_;
      while (time::now() < timestamp)
        delay_cond_.timedwait(lock_, timestamp);

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
  syslog() << "event sent, " << e << std::endl;
  std::map<std::string, event_subscriber&>::iterator i;
  for (i = subscribers_.begin(); i != subscribers_.end(); i++)
    (*i).second.process_notification(e);
}

// timer; sends a message to its subscribers on time elapse
class timer_vd : public event_publisher
{
public:
  timer_vd(const std::string& name) : event_publisher(name),
    periodic_(true), aligned_(false)  {}
  ~timer_vd() {}

  bool periodic() { mutex_locker ml(lock_); return periodic_; }
  bool aligned() { mutex_locker ml(lock_); return aligned_; } 
  time interval() { mutex_locker ml(lock_); return interval_; } 

  void set_periodic(bool periodic) { mutex_locker ml(lock_); periodic_ = periodic; }
  void set_aligned(bool aligned) { mutex_locker ml(lock_); aligned_ = aligned; }
  void set_interval(const time& interval) { mutex_locker ml(lock_); interval_ = interval; }

private:
  bool periodic_;
  bool aligned_;
  time interval_;
  mutex lock_;
  condition cond_;

  virtual void run();
};


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
      notify_subscribers(timer_event(name(), dtm));
    }
  }
}

//-----------
class itd_vd : public event_publisher
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

itd_vd::itd_vd(const std::string& name)
: event_publisher(name), dev_path_("/dev/" + name)
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

    time dtm(sec, usec);

    itd_event e(name(), dtm);
    e.set_device(name());
    e.set_dtm(dtm);
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

class rand_itd_vd: public event_publisher
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

rand_itd_vd::rand_itd_vd(const std::string& name)
: event_publisher(name), state_(CLEAR)
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
    long r = rand() % ((state_ == CLEAR) ? 30000 : 3000);
    time interval(r / 1000, (r % 1000) * 1000);

    time dtm = time::now() + interval;
    while (time::now() < dtm)
      cond_.timedwait(lock_, dtm);

    state_ = (state_ == CLEAR) ? BLOCKED : CLEAR;

    itd_event e(name(), dtm);
    e.set_device(name());
    e.set_dtm(dtm);
    e.set_state(static_cast<itd_state>(state_));

    notify_subscribers(e);
  }
}

/*
file_itd_vd

filter_vd

simple_aggr_vd

flow_writer_vd*/

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
  switch(e.type())
  {
    case EVENT_TYPE_ITD:
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

class dbglog_vd : public event_subscriber
{
public:
  dbglog_vd(const std::string& name) : event_subscriber(name) {}
  ~dbglog_vd() {}

private:
  sqlite3cc::conn db_;

  virtual void handle_event(const event& e);
  
};

void dbglog_vd::handle_event(const event& e)
{
  syslog() << "dbglog_vd::handle_event, " << e << std::endl;
}

}


int main(int argc, char *argv[])
{
  icd::syslogstream::initialize(basename(argv[0]), LOG_PERROR);
  icd::syslogstream syslog;

  try
  {
    daemonizer daemon;
    std::string db_name;
    int db_timeout = 60000; // default timeout is 60 seconds
    std::string dev_name;
    std::string emulator;
    bool run_as_daemon = false;
    bool exit = false;

    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "device", required_argument, 0, 'i' },
      { "rand", no_argument, 0, 'r' },
      { "kbd", no_argument, 0, 'k' },
      { "daemon", no_argument, 0, 'b' },
      { "pidfile", required_argument, 0, 'p' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:i:rkbphv", long_options, &option_index);
      if (ch == -1)
        break;

      switch(ch)
      {
        case 'd':
          db_name = optarg;
          break;
        case 't':
          db_timeout = strtol(optarg, NULL, 0);
          break;
        case 'i':
          dev_name = optarg;
          break;
        case 'k':
          emulator = "kbd";
          break;
        case 'r':
          emulator = "rand";
          break;
        case 'b':
          run_as_daemon = true;
          break;
        case 'p':
          daemon.pid_file(optarg);
          break;
        case 'h':
          print_usage(argv[0]);
          exit = true;
          break;
        case 'v':
          print_version(argv[0]);
          exit = true;
          break;
        default:
        {
          std::ostringstream oss;
          oss << "Unknown option '" << char(ch) << "'";
          throw std::runtime_error(oss.str());
        }
      }

      if (exit)
        break;
    }

    if (!exit && db_name.empty())
      throw std::runtime_error("Missing '--db' parameter");

    if (!exit && dev_name.empty())
      throw std::runtime_error("Missing '--device' parameter");

    if (!exit && run_as_daemon)
      exit = daemon.fork();

    if (!exit)
    {
      sigset_t signal_mask;
      sigemptyset (&signal_mask);
      sigaddset (&signal_mask, SIGINT);
      sigaddset (&signal_mask, SIGTERM);
      int res = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
      if (res != 0)
        throw std::runtime_error("sigmask failed"); 

      icd::timer_vd tm("timer0");
      tm.set_periodic(true);
      tm.set_aligned(true);
      tm.set_interval(icd::time(10,0));

      icd::rand_itd_vd itd0("itd0");

      icd::dbglog_vd log("log0");
      log.set_delay(icd::time(1,0));
      itd0.subscribe(log);
      tm.subscribe(log);

      icd::event_writer_vd evwr("evwr0", db_name, db_timeout);
      itd0.subscribe(evwr);
  
      tm.start();
      itd0.start();
      log.start();
      evwr.start();

      syslog << icd::info << "Waiting for termination signal" << std::endl;
      int sig_caught;
      res = sigwait (&signal_mask, &sig_caught);
      if (res != 0)
        throw std::runtime_error("sigwait failed");
      syslog << icd::info << "Signal received, terminating" << std::endl;

      tm.stop();
      log.stop();
      evwr.stop();
    }
  }
  catch(std::exception& e)
  {
    syslog << basename(argv[0]) << " error: " << e.what()  << std::endl;
    return 1;
  }

  return 0;
}

