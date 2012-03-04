#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <getopt.h>

#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"

class command_parser
{
  public:
    command_parser(sqlite3cc::conn& db, int argc, char* argv[]) 
      : config(db), argc(argc), argv(argv),
        separator(":"), optind(0)  {}
    ~command_parser() {}

    void set_separator(const std::string& s) { separator = s; }
    void run();

  private:
    icd::config config;
    int argc;
    char **argv;
    std::string separator;
    int optind;

    void check_num_of_args(int expected);

    void run_get();
    void run_set();
    void run_rm();
    void run_list();
    void run_list_all();
    void run_add_section();
    void run_rm_section();
    void run_list_sections();
};

void command_parser::run()
{
  while(optind < argc)
  {
    if (std::string(argv[optind]).compare("get") == 0)
      run_get();
    else if (std::string(argv[optind]).compare("set") == 0)
      run_set();
    else if (std::string(argv[optind]).compare("rm") == 0)
      run_rm();
    else if (std::string(argv[optind]).compare("list") == 0)
      run_list();
    else if (std::string(argv[optind]).compare("list-all") == 0)
      run_list_all();
    else if (std::string(argv[optind]).compare("add-section") == 0)
      run_add_section();
    else if (std::string(argv[optind]).compare("rm-section") == 0)
      run_rm_section();
    else if (std::string(argv[optind]).compare("list-sections") == 0)
      run_list_sections();
    else
      throw std::runtime_error(std::string("Unknown command '") + argv[optind] + "'");
  }
}

void command_parser::check_num_of_args(int expected)
{
  if (optind + expected >= argc)
  {
    std::ostringstream oss;
    oss << "Incorect number of arguments for '" << argv[optind] << "'command";
    throw std::runtime_error(oss.str());
  }  
}

void command_parser::run_get()
{
  check_num_of_args(2);
  std::string section = argv[optind + 1];
  std::string key = argv[optind + 2];
  std::string value = config.entry(section, key);
  std::cout << section << separator << key << separator << value << std::endl;
  optind += 3;
}

void command_parser::run_set()
{
  check_num_of_args(3);
  std::string section = argv[optind + 1];
  std::string key = argv[optind + 2];
  std::string value = argv[optind + 3];
  config.set_entry(section, key, value);
  optind += 4;
}

void command_parser::run_rm()
{
  check_num_of_args(2);
  std::string section = argv[optind + 1];
  std::string key = argv[optind + 2];
  config.remove_entry(section, key);
  optind += 3;
}

void command_parser::run_list()
{
  check_num_of_args(1);
  std::string section = argv[optind + 1];
  std::vector<icd::config_entry> list = config.list_entries(section);
  std::vector<icd::config_entry>::iterator i;
  for (i = list.begin() ; i < list.end(); i++)
    std::cout << (*i).section << separator << (*i).key
      << separator << (*i).value << std::endl;
  optind += 2;
}

void command_parser::run_list_all()
{
  std::vector<icd::config_entry> list = config.list_entries();
  std::vector<icd::config_entry>::iterator i;
  for (i = list.begin() ; i < list.end(); i++)
    std::cout << (*i).section << separator << (*i).key
      << separator << (*i).value << std::endl;
  optind += 1;
}

void command_parser::run_add_section()
{
  check_num_of_args(1);
  std::string section = argv[optind + 1];
  config.add_section(section);
  optind += 2;
}

void command_parser::run_rm_section()
{
  check_num_of_args(1);
  std::string section = argv[optind + 1];
  config.remove_section(section);
  optind += 2;
}

void command_parser::run_list_sections()
{
  std::vector<std::string> list = config.list_sections();
  std::vector<std::string>::iterator i;
  for (i = list.begin() ; i < list.end(); i++)
    std::cout << (*i) << std::endl;
  optind += 1;
}

//------------------------------------------------------------------------------

#define APP_NAME "icd-config"

const char *usage =
  "\n"
  "Usage: "APP_NAME" OPTION... CMD [ARG]... [CMD [ARG]...]...\n"
  "\n"
  "A command line tool for managing icd configuration entries.\n"
  "\n"
  "Comands:\n"
  "  get SECTION KEY\n"
  "  set SECTION KEY VALUE\n"
  "  rm SECTION KEY\n"
  "  list [SECTION]\n"
  "  list-all\n"
  "  add-section SECTION\n"
  "  rm-section SECTION\n"
  "  list-sections\n"
  "\n"
  "Options:\n"
  "  -d|--db=DB_NAME              Database file path; Mandatory option\n"
  "  -t|--timeout=TIMEOUT_MS      Timeout when waiting for acces to the database in ms\n"
  "  -s|--separator=SEPARATOR     Output separator; Default value is ':'\n"
  "  -h|--help\n"
  "  -v|--version\n"
  "\n";


int main(int argc, char *argv[])
{
  try
  {
    std::string db_name;
    std::string separator(":");
    int db_timeout = 60000; // default timeout is 60 seconds
    bool exit = false;

    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "separator", required_argument, 0, 's' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while(1)
    {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:s:hv", long_options, &option_index);
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
        case 'h':
          std::cout << usage;
          exit = true;
          break;
        case 'v':
          std::cout << APP_NAME << " " << version << " " << build_date 
            << "\n" << copyright << std::endl;
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
      if (optind >= argc)
        throw std::runtime_error("No command specified");

      if (db_name.empty())
        throw std::runtime_error("Missing '--db' parameter");

      sqlite3cc::conn db;
      db.open(db_name.c_str());
      db.busy_timeout(db_timeout);
      db.exec("BEGIN EXCLUSIVE TRANSACTION");

      command_parser parser(db, argc - optind, argv + optind);
      parser.set_separator(separator);
      parser.run();

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

