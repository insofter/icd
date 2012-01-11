#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string> 
#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>

#include "sqlite3cc.h"
#include "db-config.h"

#define APP_NAME "icd-itd-daemon"
#define APP_VERSION "1.0"
#define APP_COPYRIGHT "Copyright (c) 2012 Tomasz Rozensztrauch"

const char *usage =
  "\n"
  "Usage: "APP_NAME" OPTION...\n"
  "\n"
  "A daemon communicating with itd devices. After configuration of\n"
  "the device the tool reads input events from it and stores them\n"
  "in the database.\n"
  "\n"
  "Options:\n"
  "  -d|--db=DB_NAME              Database file path; Mandatory option\n"
  "  -t|--timeout=TIMEOUT_MS      Timeout when waiting for acces to the database in ms\n"
  "  -i|--device=DEVICE           Device name; E.g. itd0, itd1... This is mandatory option\n"
  "                               This tool provides also 2 'emulator' devices:\n"
  "                               rand - generates input events randomly\n"
  "                               kbd - generates event when user enters characters\n"
  "                               on standard input\n"
  "  -c|--cmdline                 Do not daemonize; Run as a command line tool\n"
  "  -h|--help\n"
  "  -v|--version\n"
  "\n";

const char *version =
  APP_NAME" "APP_VERSION"\n"
  APP_COPYRIGHT"\n";

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
  try
  {
    std::string db_name;
    int db_timeout = 60000; // default timeout is 60 seconds
    std::string dev_name;
    std::string emulator;
    bool daemonize = true;
    bool exit = false;

    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "device", required_argument, 0, 'i' },
      { "rand", no_argument, 0, 'r' },
      { "kbd", no_argument, 0, 'k' },
      { "cmdline", no_argument, 0, 'c' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:i:rkchv", long_options, &option_index);
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
        case 'c':
          daemonize = false;
          break;
        case 'h':
          std::cout << usage;
          exit = true;
          break;
        case 'v':
          std::cout << version;
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

    if (!exit)
    {
      if (db_name.empty())
        throw std::runtime_error("Missing '--db' parameter");

      if (dev_name.empty())
        throw std::runtime_error("Missing '--device' parameter");

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

        std::cout << dev_name << " " << e.dtm() << " " << e.state() << std::endl;

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
    std::cout << APP_NAME" error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

