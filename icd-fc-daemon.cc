#include "sqlite3cc.h"
#include "db-config.h"
#include "daemonizer.h"
#include "syslogstream.h"

#include <iostream>
#include <fstream>
#include <linux/input.h>
#include <getopt.h>
#include <cstdlib>

#define APP_NAME "icd-fc-daemon"
#define APP_VERSION "1.0"
#define APP_COPYRIGHT "Copyright (c) 2012 Tomasz Rozensztrauch"

const char *usage =
  "\n"
  "Usage: "APP_NAME" OPTION...\n"
  "\n"
  "A front controller daemon. Provides user interface via lcd and buttons.\n"
  "\n"
  "Options:\n"
  "  -d|--db=DB_NAME              Database file path; Mandatory option\n"
  "  -t|--timeout=TIMEOUT_MS      Timeout when waiting for acces to the database in ms\n"
  "  -b|--daemon                  Run as a daemon\n"
  "  -p|--pidfile=FILE            Create pid file (if a daemon)\n"
  "  -h|--help\n"
  "  -v|--version\n"
  "\n";

const char *version =
  APP_NAME" "APP_VERSION"\n"
  APP_COPYRIGHT"\n";

void write_lcd_cmd(unsigned char cmd)
{
  std::ofstream os;
  os.open("/sys/devices/platform/gpio-hd44780/cmd");
  os << (int)cmd << std::endl;
  os.close();
}

int main(int argc, char *argv[])
{
  syslogstream syslog(APP_NAME, LOG_PERROR);

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
      int ch = getopt_long(argc, argv, "d:t:bphv", long_options, &option_index);
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

    if (!exit && db_name.empty())
      throw std::runtime_error("Missing '--db' parameter");

    if (!exit && run_as_daemon)
      exit = daemon.fork();

    if (!exit)
    {
      sqlite3cc::conn db;
      db.open(db_name.c_str());
      db.busy_timeout(db_timeout);

      std::ofstream lcd;
      lcd.open("/dev/lcd0");
      write_lcd_cmd(0x0F); // enable display
  
      std::ifstream kbd;
      kbd.open("/dev/input/event0");

      while(!exit)
      {
        icd::config config(db);
        std::vector<icd::config_entry> list = config.list_entries();
        std::vector<icd::config_entry>::iterator i;
        for (i = list.begin() ; i < list.end(); i++)
        {
          bool key_down = false;
          while(!key_down && !exit)
          {
            struct input_event ev;
            if (!kbd.read((char*)&ev, sizeof(ev)))
              exit = true;
            else if (ev.code >= 160 && ev.value == 1)
              key_down = true;
          }

          syslog << (*i).section << " " << (*i).key
            << " " << (*i).value << std::endl;

          write_lcd_cmd(0x01); // clear display
          lcd << (*i).section << ":" << (*i).key << std::endl;
          write_lcd_cmd(0xC0); // go to second line
          lcd << (*i).value << std::endl;
        }
      }

      kbd.close();
      lcd.close();
      db.close();
    }
  }
  catch(std::exception& e)
  {
    syslog << APP_NAME" error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

