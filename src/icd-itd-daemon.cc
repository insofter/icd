#include "itd-vds.h"
#include "threads.h"
#include "daemonizer.h"
#include "syslogstream.h"
#include "version.h"

#include <iostream>
#include <sstream>
#include <string> 
#include <cstdlib>
#include <stdexcept>
#include <getopt.h>
#include <libgen.h>
#include <signal.h> //TODO

//#include "db-config.h"

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

template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

#include "db-config.h"
#include <memory>

namespace icd
{
class vd_collection
{
public:
  vd_collection(const std::string& db_name, int db_timeout)
  : db_name_(db_name), db_timeout_(db_timeout) {}
  virtual ~vd_collection();

  void create();

  void start();
  void stop();

protected:
  void add(virtual_device* vd);

private:
  std::vector<virtual_device*> vds_;
  std::string db_name_;
  int db_timeout_;
};

void vd_collection::create()
{
  sqlite3cc::conn db;
  db.open(db_name_.c_str());
  db.busy_timeout(db_timeout_);
  config config(db);

  long aggr_period_mins = config.entry_long("device","aggr-period-mins", 15);

  aggr_timer_vd* aggr_timer = new aggr_timer_vd("timer0");
  aggr_timer->set_aggr_period(time::from_mins(aggr_period_mins));
  add(aggr_timer);

  event_logger_vd* event_logger = NULL;

  for (int i = 0; i < 4; i++)
  {
    std::string dev_name = "itd" + to_string(i);
    bool enabled  = config.entry_bool(dev_name, "enabled", false);
    long engage_delay_ms = config.entry_long(dev_name, "engage-delay-ms", false);
    long release_delay_ms = config.entry_long(dev_name, "release-delay-ms", false);
    bool active_low = config.entry_bool(dev_name, "active-low", false);
    std::string log_events = config.entry(dev_name, "log-events");
    std::string test_mode = config.entry(dev_name, "test-mode");
    std::string test_file = config.entry(dev_name, "test-file");


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
    filter->set_itd_vd(*itd);
    filter->set_aggr_timer_vd(*aggr_timer);
    filter->set_engage_delay(time::from_msec(engage_delay_ms));
    filter->set_release_delay(time::from_msec(release_delay_ms));
    add(filter);

    aggr_vd* aggr = new aggr_vd("aggr" + to_string(i), db_name_, db_timeout_);
    aggr->set_filter_vd(*filter);
    aggr->set_aggr_period(time::from_mins(aggr_period_mins));
    add(aggr);

    if (log_events == "raw" || log_events == "filered" )
    {
      if (event_logger == NULL)
      {
        event_logger = new event_logger_vd("log0", db_name_, db_timeout_);
        add(event_logger);
      }
      event_logger->add_publisher((log_events == "raw") ? *itd : *filter);
    }
  }

  db.close();
}

void vd_collection::add(virtual_device* vd)
{
  vds_.push_back(vd);
}

void vd_collection::start()
{
  std::vector<virtual_device*>::iterator i;
  for (i = vds_.begin(); i != vds_.end(); i++)
    (*i)->start();
}

void vd_collection::stop()
{
  std::vector<virtual_device*>::iterator i;
  for (i = vds_.begin(); i != vds_.end(); i++)
    (*i)->stop();
}

vd_collection::~vd_collection()
{
  std::vector<virtual_device*>::iterator i;
  for (i = vds_.begin(); i != vds_.end(); i++)
    delete (*i);
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

      icd::vd_collection collection(db_name, db_timeout);
      collection.create();
      collection.start();



/*      icd::timer_vd tm("timer0");
      tm.set_periodic(true);
      tm.set_aligned(true);
      tm.set_interval(icd::time(10,0));
      tm.set_priority(10);

      icd::rand_itd_vd itd0("itd0");

      icd::filter_vd filter("filter0");
      filter.set_delay(icd::time(1,0));
      filter.set_itd_vd(itd0);
      filter.set_aggr_timer_vd(tm);

      icd::aggr_vd aggr0("aggr0", db_name, db_timeout);
      aggr0.set_aggr_period(icd::time(10,0));
      aggr0.set_delay(icd::time(1,0));
      aggr0.set_filter_vd(filter);

      icd::dbglog_vd log("log0");
      log.set_delay(icd::time(1,0));
      log.add_publisher(filter);
      log.add_publisher(itd0);
      log.add_publisher(tm);

//      icd::event_writer_vd evwr("evwr0", db_name, db_timeout);
//      itd0.subscribe(evwr);
  
      tm.start();
      itd0.start();
      filter.start();
      aggr0.start();
      log.start();
//      evwr.start();*/

      syslog << icd::info << "Waiting for termination signal" << std::endl;
      int sig_caught;
      res = sigwait (&signal_mask, &sig_caught);
      if (res != 0)
        throw std::runtime_error("sigwait failed");
      syslog << icd::info << "Signal received, terminating" << std::endl;

/*      tm.stop();
      itd0.stop();
      filter.stop();
      aggr0.stop();
      log.stop();*/
//      evwr.stop();

      collection.stop();
    }
  }
  catch(std::exception& e)
  {
    syslog << basename(argv[0]) << " error: " << e.what()  << std::endl;
    return 1;
  }

  return 0;
}

