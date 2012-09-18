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

void print_usage(char *argv0)
{
  std::cerr << 
    "\n"
    "Usage: " << basename(argv0) << " OPTION...\n"
    "\n"
    "A daemon that reads events from itd devices, aggregates them\n"
    "and stores in the flow table entries.\n"
    "\n"
    "Options:\n"
    "  -d|--db=DB_NAME           Database file path; Mandatory option\n"
    "  -t|--timeout=TIMEOUT_MS   Timeout for waiting for acces to\n"
    "                            the database in ms. The default is 60sec\n"
    "  -b|--daemon               Run as a daemon\n"
    "  -p|--pidfile=FILE         Create pid file (if a daemon)\n"
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
    bool run_as_daemon = false;
    bool exit = false;

    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "daemon", no_argument, 0, 'b' },
      { "pidfile", required_argument, 0, 'p' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:bp:hv", long_options, &option_index);
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

    if (!exit && run_as_daemon)
      exit = daemon.fork();

    if (!exit)
    {
      icd::thread::mask_signal(true, SIGINT);

      icd::itd_farm farm(db_name, db_timeout);
      farm.run();
    }
  }
  catch(std::exception& e)
  {
    syslog << icd::err << basename(argv[0]) << " error: " << e.what()  << std::endl;
    return 1;
  }

  return 0;
}

