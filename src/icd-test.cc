#include "itd-device.h"
#include "version.h"
//#include "sqlite3cc.h"
//#include "db-config.h"
#include "syslogstream.h"

#include <getopt.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <libgen.h>

#define CONFIG_DB_ENV_VARIABLE "ICD_CONFIG_DB"
#define DATA_DB_ENV_VARIABLE "ICD_LIVE_DB"

void print_usage(char *argv0)
{
  std::cerr << 
    "\n"
    "Usage: " << basename(argv0) << " OPTION...\n"
    "\n"
    "A tool executing itd device test procedure.\n"
    "The test result of each itdN device is printed on standard output.\n"
    "Possible values:\n"
    "  0, OK\n"
    "  1, FAIL_01\n"
    "  2, FAIL_10\n"
    "  3, FAIL_11\n"
    "\n"
    "Options:\n"
    "  -c|--config_db=NAME       Config database file path; Mandatory option\n"
    "                            Can be also provided as ICD_CONFIG_DB\n"
    "                            environment variable\n"
    "  -d|--data_db=NAME         Data database file path; Mandatory option\n"
    "                            Can be also provided as ICD_LIVE_DB\n"
    "                            environment variable\n"
    "  -t|--timeout=TIMEOUT_MS   Timeout for waiting for acces to\n"
    "                            the database in ms. The default is 60sec\n"
    "  -f|--format=FORMAT        Output format:\n"
    "                            raw, e.g: 0 0 0 0\n"
    "                            short, e.g: itd0:0 itd1:0 ...\n"
    "                            long (default) e.g. itd0:OK ...\n"
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
    std::string config_db_name;
    std::string data_db_name;
    int db_timeout = 60000; // default timeout is 60 seconds
    std::string format = "long";
    bool exit = false;

    struct option long_options[] = {
      { "config_db", required_argument, 0, 'c' },
      { "data_db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "format", required_argument, 0, 'f' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "c:d:t:f:hv", long_options, &option_index);
      if (ch == -1)
        break;

      switch(ch)
      {
        case 'c':
          config_db_name = optarg;
          break;
        case 'd':
          data_db_name = optarg;
          break;
        case 't':
          db_timeout = strtol(optarg, NULL, 0);
          break;
        case 'f':
          format = optarg;
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

    if (!exit && format != "long" && format != "short" && format != "raw")
    {
      throw std::runtime_error("Invalid '--format' parameter."
        " Expected 'long', 'short' or 'raw'");
    }

    if (!exit && config_db_name.empty())
    {
      char *env = std::getenv(CONFIG_DB_ENV_VARIABLE);
      if (env != NULL)
        config_db_name = std::string(env);
      else
        throw std::runtime_error("Missing '--config_db' parameter");
    }

    if (!exit && data_db_name.empty())
    {
      char *env = std::getenv(DATA_DB_ENV_VARIABLE);
      if (env != NULL)
        data_db_name = std::string(env);
      else
        throw std::runtime_error("Missing '--data_db' parameter");
    }

    if (!exit)
    {
      std::map<std::string,icd::itd_test> results;
      results = icd::itd_device::test_all();

      std::map<std::string,icd::itd_test>::const_iterator it;
      if (format == "raw")
      {
        for (it = results.begin(); it != results.end(); it++)
           std::cout << it->second << " ";
         std::cout << std::endl;
      }
      else if (format == "short")
      {
        for (it = results.begin(); it != results.end(); it++)
          std::cout << it->first << ":" << it->second << std::endl;
      }
      else // if (format == "long")
      {
        for (it = results.begin(); it != results.end(); it++)
          std::cout << it->first << ":" << it->second.to_string() << std::endl;
      }
    }
  }
  catch(std::exception& e)
  {
    syslog << icd::err << basename(argv[0]) << " error: " << e.what()  << std::endl;
    return 1;
  }

  return 0;
}

