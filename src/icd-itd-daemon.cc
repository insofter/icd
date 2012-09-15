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

      icd::filter_vd filter("filter0");
      filter.set_delay(icd::time(1,0));
      filter.set_itd_vd_name("itd0");
      filter.set_aggr_vd_name("timer0");
      itd0.subscribe(filter);
      tm.subscribe(filter);

      icd::dbglog_vd log("log0");
      log.set_delay(icd::time(1,0));
      filter.subscribe(log);
      itd0.subscribe(log);
      tm.subscribe(log);

//      icd::event_writer_vd evwr("evwr0", db_name, db_timeout);
//      itd0.subscribe(evwr);
  
      tm.start();
      itd0.start();
      filter.start();
      log.start();
//      evwr.start();

      syslog << icd::info << "Waiting for termination signal" << std::endl;
      int sig_caught;
      res = sigwait (&signal_mask, &sig_caught);
      if (res != 0)
        throw std::runtime_error("sigwait failed");
      syslog << icd::info << "Signal received, terminating" << std::endl;

      tm.stop();
      itd0.stop();
      filter.stop();
      log.stop();
//      evwr.stop();
    }
  }
  catch(std::exception& e)
  {
    syslog << basename(argv[0]) << " error: " << e.what()  << std::endl;
    return 1;
  }

  return 0;
}

