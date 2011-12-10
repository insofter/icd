#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <getopt.h>

#include "sqlite3cc.h"
#define APP_NAME "icdtcp3-sql"
#define APP_VERSION "1.0"
#define APP_COPYRIGHT "Copyright (c) 2011 Tomasz Rozensztrauch"

const char *usage =
  "\n"
  "Usage: "APP_NAME" OPTION... [SCRIPT_FILE]...\n"
  "\n"
  "A command line tool for running sqlite SQL scripts. Is reads and executes\n"
  "provided script file(s). If no script file has been provided the tool\n"
  "reads standard input. All commands within all files are processes\n"
  "as a part of a single transaction.\n"
  "\n"
  "Options:\n"
  "  -d|--db=DB_NAME              Database file path; Mandatory option\n"
  "  -t|--timeout=TIMEOUT_MS      Timeout when waiting for acces to the database in ms\n"
  "  -h|--help\n"
  "  -v|--version\n"
  "\n";

const char *version =
  APP_NAME" "APP_VERSION"\n"
  APP_COPYRIGHT"\n";

int main(int argc, char *argv[])
{
  try
  {
    std::string db_name;
    int db_timeout = 60000; // default timeout is 60 seconds
    bool exit = false;

    struct option long_options[] = {
    { "db", required_argument, 0, 'd' },
    { "timeout", required_argument, 0, 't' },
    { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
    { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:hv", long_options, &option_index);
      if (ch == -1)
        break;

      switch(ch)
      {
        case 'd':
          db_name = optarg;
          break;
        case 't':
          db_timeout = strtol(optarg, NULL, 0);
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

      // TODO make sure io operatins throw exceptions
      std::ostringstream oss;
      std::string line;
      if (optind >= argc)
      {
        while(getline(std::cin, line))
          oss << line << std::endl;
      }
      else
      {
         while(optind < argc)
         {
           std::ifstream is;
           is.open(argv[optind]);
           while(getline(is, line))
             oss << line << std::endl;
           is.close();
           optind++;
         }
      }

      sqlite3cc::conn db;
      db.open(db_name.c_str());
      db.busy_timeout(db_timeout);
      db.exec("BEGIN EXCLUSIVE TRANSACTION");
      db.exec(oss.str().c_str());
      db.exec("COMMIT TRANSACTION");
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

