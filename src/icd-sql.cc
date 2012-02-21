#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <getopt.h>

#include "sqlite3cc.h"
#define APP_NAME "icd-sql"
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
  "  -s|--separator=SEPARATOR     Output separator; Default value is ':'\n"
  "  -e|--extern-db=DB_NAME       Second (external) database for commands that\n"
  "                               performs operations between two databases\n"
  "  -h|--help\n"
  "  -v|--version\n"
  "\n";

const char *version =
  APP_NAME" "APP_VERSION"\n"
  APP_COPYRIGHT"\n";

int db_exec_cbf(void* data, int cnt, char **values, char** names)
{
  std::string *separator = reinterpret_cast<std::string*>(data);
  for (int i = 0; i < cnt; i++)
  {
    std::cout << ((i != 0) ? *separator : "");
    std::cout << (values[i] != NULL ? values[i] : "NULL");
  }
  std::cout << std::endl;
  return 0;
}

int main(int argc, char *argv[])
{
  try
  {
    std::string db_name;
    std::string separator(":");
    std::string db_attach;
    int db_timeout = 60000; // default timeout is 60 seconds
    bool exit = false;

    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "separator", required_argument, 0, 's' },
      { "extern-db", required_argument, 0, 'e' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:s:e:hv", long_options, &option_index);
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
        case 's':
          separator = optarg;
          break;
        case 'e':
        {
          std::ostringstream oss;
          oss << "ATTACH DATABASE '" << optarg << "' AS externdb";
          db_attach = oss.str();
          break;
        }
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
           if (!is.eof())
           {
             std::ostringstream oss;
             oss << "Reading file '" << argv[optind] << "' failed";
             throw std::ios_base::failure(oss.str());
           }
           is.close();
           optind++;
         }
      }

      sqlite3cc::conn db;
      db.open(db_name.c_str());
      db.busy_timeout(db_timeout);
      db.set_exec_cbf(db_exec_cbf, reinterpret_cast<void*>(&separator));
      db.exec("PRAGMA recursive_triggers = ON");
      db.exec("PRAGMA foreign_keys = ON");
      if (!db_attach.empty())
        db.exec(db_attach.c_str());
      db.exec("BEGIN EXCLUSIVE TRANSACTION");
      db.exec(oss.str().c_str());
      db.exec("COMMIT TRANSACTION");
      if (!db_attach.empty())
        db.exec("DETACH DATABASE externdb");
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

