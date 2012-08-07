#include <linux/input.h>
#include <getopt.h>
#include <cstdlib>
#include <libgen.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

#include "lcd-menu.hpp"
#include "lcd-menu-front-item.hpp"
#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include "daemonizer.h"
#include "syslogstream.h"

//#include <unistd.h>
#include <poll.h>
#include <fcntl.h>


icd::config *globalConfig;

void print_usage(char *argv0) {
  std::cerr <<
            "\n"
            "Usage: " << basename(argv0) << " OPTION...\n"
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
            << std::endl;
}

void print_version(char *argv0) {
  std::cerr << basename(argv0) << " " << version << "\n"
            << copyright << std::endl;
}

int main(int argc, char *argv[]) {
  syslogstream syslog(basename(argv[0]), LOG_PERROR);

  try {
    daemonizer daemon;
    std::string db_name;
    int db_timeout = 60000; // default timeout is 60 seconds
    bool run_as_daemon = false;
    bool exit = false;

//parametry uruchomienia
    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "daemon", no_argument, 0, 'b' },
      { "pidfile", required_argument, 0, 'p' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while( 1==1 ) {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:bphv", long_options, &option_index);
      if (ch == -1)
        break;
      switch(ch) {
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
        std::ostringstream oss;
        oss << "Unknown option '" << char(ch) << "'";
        throw std::runtime_error(oss.str());
        break;
      }
      if(exit) {
        break;
      }
    }
    if( !exit && db_name.empty() ) {
      throw std::runtime_error("Missing '--db' parameter");
    }

    if (!exit && run_as_daemon) {
      exit = daemon.fork();
    }
//parametry uruchomienia -- koniec

//budowa menu
    ClcdDriver lcdDrv;
    Clcd lcd;
    CmenuList *menu = new CmenuList("Menu");
    CmenuList *item;
    CmenuDbParamList *pl;

    item = new CmenuList("TCPIP");

    pl=new CmenuDbParamList("Aktualna");
    pl->itemAdd("DHCP", "current", "dhcp");
    pl->itemAdd("Adres IP", "current", "ip");
    pl->itemAdd("Maska", "current", "mask");
    pl->itemAdd("Brama", "current", "gate");
    pl->itemAdd("DNS 1", "current", "dns1");
    pl->itemAdd("DNS 2", "current", "dns2");
    item->itemAdd(pl);

    pl=new CmenuDbParamList("Statyczna");
    pl->itemAdd("DHCP", "tcpip", "dhcp");
    pl->itemAdd("Adres IP", "tcpip", "ip");
    pl->itemAdd("Maska", "tcpip", "mask");
    pl->itemAdd("Brama", "tcpip", "gate");
    pl->itemAdd("DNS 1", "tcpip", "dns1");
    pl->itemAdd("DNS 2", "tcpip", "dns2");
    item->itemAdd(pl);

    pl=new CmenuDbParamList("WiFi");
    item->itemAdd(pl);

    pl=new CmenuDbParamList("GSM");
    item->itemAdd(pl);

    menu->itemAdd( item );

    pl=new CmenuDbParamList("Wysyłanie");
    pl->itemAdd("IDS - id salonu", "device", "ids");
    pl->itemAdd("IDD - id urządz.", "device", "idd");
    pl->itemAdd("Adres wysyłania", "device", "address");
    pl->itemAdd("Użytkownik", "device", "user");

    menu->itemAdd( pl );

    pl=new CmenuDbParamList("Foto");
    pl->itemAdd("Nazwa", "itd0", "name");
    pl->itemAdd("Nazwa", "itd1", "name");
    pl->itemAdd("Nazwa", "itd2", "name");
    pl->itemAdd("Nazwa", "itd3", "name");

    menu->itemAdd( pl );

    CmenuContainerNoRoot *mainMenu = new CmenuContainerNoRoot(menu, new CmenuItemTimeFoto);
    mainMenu->itemAdd(new CmenuItemIdds);
    mainMenu->itemAdd(new CmenuItemSendStat);
//budowa menu -- koniec


    if (!exit) {
      //glowna petla
      sqlite3cc::conn db;
      db.open(db_name.c_str());
      db.busy_timeout(db_timeout);
      globalConfig=new icd::config(db);
      int wait=333;

      mainMenu->screen(&lcd);
      lcdDrv.print(lcd);

      int kbd;
      kbd=open("/dev/input/event0", O_RDONLY);

      struct input_event ev;
      struct pollfd fds[1];



      while(!exit) {

        fds[0].fd=kbd;
        fds[0].events=POLLIN;

        poll( fds, 1, wait );

        if( fds[0].revents & POLLIN ) {
          read( kbd, (char*)&ev, sizeof(ev) );
          if( ev.value == 1 ) {
            switch(ev.code) {//TODO kody klawiszy
            case 259:
              mainMenu->up(&lcd);
              break;
            case 258:
              mainMenu->down(&lcd);
              break;
            case 1259:// !!
              mainMenu->left(&lcd);
              break;
            case 1258:// !!
              mainMenu->right(&lcd);
              break;
            case 257:
              mainMenu->enter(&lcd);
              break;
            case 256:
              mainMenu->esc(&lcd);
              break;
            }
            lcdDrv.print(lcd);

            read( kbd, (char*)&ev, sizeof(ev) );//4 eventy na jedno przycisniecie
            read( kbd, (char*)&ev, sizeof(ev) );
            read( kbd, (char*)&ev, sizeof(ev) );
          }
        } else {
          mainMenu->screen(&lcd);
          lcdDrv.print(lcd);
        }
        if( lcd._refresh==0 ) {
          wait=-1;
        } else if( lcd._refresh<300 ) {
          wait=333;
        } else {
          wait=lcd._refresh;
        }
      }

      /*mainMenu->fullEsc();
      mainMenu->screen(&lcd);
      lcdDrv.print(lcd);
      usleep(333*1000);*/

      /*  if( !kbd.read( (char*)&ev, sizeof(ev) ) ) {
          exit = true;
        }
        else { //} if( ev.code >= 160 && ev.value == 1 ) {
          sprintf(ss,"code:%i ", ev.code);
          printf("%s\n",ss);
          lcd._lcd[0]=ss;
          sprintf(ss, "value:%i  cnt:%i ", ev.value, i);
          printf("%s\n",ss);
          lcd._lcd[1]=ss;
          ++i;
          lcdDrv.print(lcd);
        }*/

      /*while( c!='q' ) {
        cout << endl;
        cout << "cur: ";
        if( lcd._curOn ) {
          cout << "on";
        } else {
          cout << "off";
        }
        cout << endl;
        cout << "----------------" << endl;
        cout << lcd._lcd[0] << endl;
        cout << lcd._lcd[1] << endl;
        if( lcd._static ) {
          cout << "----------------" << endl;
        } else {
          cout << "-------------->>" << endl;
        }
        cin >> c;
        if( c=='w' ) {
          mainMenu->up(&lcd);
        } else if( c=='s' ) {
          mainMenu->down(&lcd);
        } else if( c=='e' ) {
          mainMenu->enter(&lcd);
        } else if( c=='x' ) {
          mainMenu->esc(&lcd);
        } else if( c=='a' ) {
          mainMenu->left(&lcd);
        } else if( c=='d' ) {
          mainMenu->right(&lcd);
        } else if( c=='z' ) {
          mainMenu->fullEsc();
          mainMenu->screen(&lcd);
        } else {
          mainMenu->screen(&lcd);
        }*/

      /*for (i = list.begin() ; i < list.end(); i++)
      {
        syslog << (*i).section << " " << (*i).key
        << " " << (*i).value << std::endl;

        write_lcd_cmd(0x01); // clear display
        lcd << (*i).section << ":" << (*i).key << std::endl;
        write_lcd_cmd(0xC0); // go to second line
        lcd << (*i).value << std::endl;

        bool key_down = false;
        while(!key_down && !exit)
        {
          struct input_event ev;
          if (!kbd.read((char*)&ev, sizeof(ev)))
            exit = true;
          else if (ev.code >= 160 && ev.value == 1)
            key_down = true;
        }
      }*/

//glowna petla -- koniec

      db.close();
    }
  }
  catch(std::exception& e)
  {
    syslog << basename(argv[0]) << " error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}




