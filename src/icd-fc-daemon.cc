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
#include "syslogstream.h"

#include <poll.h>
#include <fcntl.h>

icd::config *globalConfig;
sqlite3cc::conn *globalConfigDb;
sqlite3cc::conn *globalDataDb;
sqlite3cc::conn *globalLiveDb;

void print_usage( char *argv0 ) {
  std::cerr <<
    "\n"
    "Usage: " << basename( argv0 ) << " OPTION...\n"
    "\n"
    "A front controller daemon. Provides user interface via lcd and buttons.\n"
    "\n"
    "Options:\n"
//    "  -d|--db=DB_NAME              Database file path; Mandatory option\n"
    "  -t|--timeout=TIMEOUT_MS      Timeout for access to the database in ms\n"
    "  -h|--help\n"
    "  -v|--version\n"
    << std::endl;
}

void print_version( char *argv0 ) {
  std::cerr << basename( argv0 ) << " " << version << "\n"
    << copyright << std::endl;
}



int main( int argc, char *argv[] ) {
  icd::syslogstream::initialize( basename( argv[0] ), LOG_PERROR );
  icd::syslogstream syslog;

  try {
    int db_timeout = 60000; // MIN timeout is 60 seconds
    bool exit = false;

    //parametry uruchomienia
    struct option long_options[] = {
      { "timeout", required_argument, 0, 't' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while( 1==1 ) {
      int option_index = 0;
      int ch = getopt_long( argc, argv, "t:hv", long_options, &option_index );
      if ( ch == -1 )
        break;
      switch( ch ) {
        case 't':
          db_timeout = strtol( optarg, NULL, 0 );
          break;
        case 'h':
          print_usage( argv[0] );
          exit = true;
          break;
        case 'v':
          print_version( argv[0] );
          exit = true;
          break;
MIN:
          std::ostringstream oss;
          oss << "Unknown option '" << char( ch ) << "'";
          throw std::runtime_error( oss.str() );
          break;
      }
      if( exit ) {
        break;
      }
    }

    //parametry uruchomienia -- koniec

    //menu
    /* Menu visualisation
     * * F - item in shortcuts
     * * M - menu in main screen
     *
     * mainMenu
     * |
     * +--CmenuItemTimeFoto A
     * +--CmenuItemTimeFoto B
     * +--CmenuItemTimeFoto C
     * +--CmenuItemTimeFoto D
     * +--CmenuItemIdds
     * +--CmenuItemDbParam: Adres IP 
     * +--CmenuItemDbParam: IP (WiFi) i SSID
     * +--CmenuItemDbParam: Status wysyłania
     * |
     * F--Test połączenia
     * F--Test fotokomórek
     * F--Wyłączanie
     * |
     * M--Menu
     *    |
     *    +--TCPIP
     *    |  |
     *    |  +--Aktualna
     *    |  |  |
     *    |  |  +--DHCP
     *    |  |  +--Adres IP
     *    |  |  +--Maska
     *    |  |  +--Brama
     *    |  |  +--DNS 1
     *    |  |  +--DNS 2
     *    |  |
     *    |  +--Statyczna
     *    |  |  |
     *    |  |  +--DHCP ( edit: bool )
     *    |  |  +--Adres IP ( edit: ip )
     *    |  |  +--Maska ( edit: ip )
     *    |  |  +--Brama ( edit: ip )
     *    |  |  +--DNS 1 ( edit: ip )
     *    |  |  +--DNS 2 ( edit: ip )
     *    |  |
     *    |  +--WiFi
     *    |  |  +--Włącz wifi ( edit: bool )
     *    |  |  +--Adres IP && SSID
     *    |  |
     *    |  +--GSM
     *    |
     *    +--Wysyłanie
     *    |  |
     *    |  +--IDS ( edit: text )
     *    |  +--IDD ( edit: num )
     *    |  +--Adres wysyłania ( edit: text )
     *    |  +--Uzytkownik ( edit: text )
     *    |
     *    +--Fotokomórki
     *       |
     *       +--Nazwa
     *       +--Nazwa
     *       +--Nazwa
     *       +--Nazwa
     */


    globalConfigDb=new sqlite3cc::conn();
    globalConfigDb->open( std::getenv("ICD_CONFIG_DB") );
    globalConfigDb->busy_timeout( db_timeout );
    globalConfig=new icd::config( *globalConfigDb );

    globalLiveDb=new sqlite3cc::conn();
    globalLiveDb->open( std::getenv("ICD_LIVE_DB") );
    globalLiveDb->busy_timeout( db_timeout );

    globalDataDb=new sqlite3cc::conn();
    globalDataDb->open( std::getenv("ICD_DATA_DB") );
    globalDataDb->busy_timeout( db_timeout );

    ClcdDriver lcdDrv;
    Clcd lcd;
    CmenuList *menu = new CmenuList( "Menu" );
    CmenuList *item;
    CmenuDbParamList *dbpl;
    CmenuAppParamList *appl;

    item = new CmenuList( "TCPIP" );

    appl=new CmenuAppParamList( "Aktualna" );
    appl->itemAdd( "Adres IP (Eth)", "icd-current eth ip" );
    appl->itemAdd( "Maska", "icd-current eth mask" );
    appl->itemAdd( "Brama", "icd-current eth gate" );
    appl->itemAdd( "DNS 1", "icd-current eth dns1" );
    appl->itemAdd( "DNS 2", "icd-current eth dns2" );
    item->itemAdd( appl );

    dbpl=new CmenuDbParamList( "Statyczna" );
    dbpl->itemAdd( "DHCP", "tcpip", "dhcp", CdbParam::editBool );
    dbpl->itemAdd( "Adres IP (Eth)", "tcpip", "ip", CdbParam::editIp );
    dbpl->itemAdd( "Maska", "tcpip", "mask", CdbParam::editIp );
    dbpl->itemAdd( "Brama", "tcpip", "gate", CdbParam::editIp );
    dbpl->itemAdd( "DNS 1", "tcpip", "dns1", CdbParam::editIp );
    dbpl->itemAdd( "DNS 2", "tcpip", "dns2", CdbParam::editIp );
    dbpl->itemAdd( "MAC", "tcpip", "mac", CdbParam::editMac );
    dbpl->itemAdd( "Dnsmasq", "tcpip", "dnsmasq", CdbParam::editBool );
    item->itemAdd( dbpl );

    dbpl=new CmenuDbParamList( "WiFi" );
    dbpl->itemAdd( "Włącz wifi", "wifi", "enabled", CdbParam::editBool );
    dbpl->itemAdd( "Adres IP (WiFi)", "wifi", "ip-ssid" );
    item->itemAdd( dbpl );

    dbpl=new CmenuDbParamList( "GSM" );
    item->itemAdd( dbpl );

    menu->itemAdd( item );

    dbpl=new CmenuDbParamList( "Wysyłanie" );
    dbpl->itemAdd( "IDS - id salonu", "device", "ids", CdbParam::editText );
    dbpl->itemAdd( "IDD - id urządz.", "device", "idd", CdbParam::editInt );
    dbpl->itemAdd( "Adres wysyłania", "device", "address", CdbParam::editText );
    dbpl->itemAdd( "Użytkownik", "device", "user", CdbParam::editText );

    menu->itemAdd( dbpl );

    dbpl=new CmenuDbParamList( "Fotokomórki" );
    dbpl->itemAdd( "Nazwa", "counter0", "name" );
    dbpl->itemAdd( "Nazwa", "counter1", "name" );
    dbpl->itemAdd( "Nazwa", "counter2", "name" );
    dbpl->itemAdd( "Nazwa", "counter3", "name" );

    menu->itemAdd( dbpl );

    CmenuContainerNoRoot *mainMenu = new CmenuContainerNoRoot( menu, new CmenuItemTimeFoto( 1 ) );
    mainMenu->itemAdd( new CmenuItemTimeFoto( 2 ) );
    mainMenu->itemAdd( new CmenuItemTimeFoto( 3 ) );
    mainMenu->itemAdd( new CmenuItemTimeFoto( 4 ) );
    mainMenu->itemAdd( new CmenuItemIdds );
    mainMenu->itemAdd( new CmenuItemAppParam( "Adres IP (Eth)", "icd-current eth ip" ) );
    mainMenu->itemAdd( new CmenuItemAppParam( "IP (WiFi) i SSID", "icd-current wifi ip-ssid" ) );
    mainMenu->itemAdd( new CmenuItemFileParam( "Status wysyłania", "/tmp/last-send-status" ) );
    std::string cmd;
    cmd="icd-flush-db && icd-transfer-data --log=short";
    int conntestid=mainMenu->fastAdd( 
        new CmenuItemRunTestApp( "Test połączenia", "> Uruchomić?",
          cmd, "Test połączenia*", "Test połączenia" ) );

    cmd="icd-test --format=lcd";
    int fototestid=mainMenu->fastAdd( 
        new CmenuItemRunTestApp( "Test fotokomórek","> Testuj",
          cmd, "Test fotokomórek", "", true ) );

    int poweroff=mainMenu->fastAdd( 
        new CmenuItemRunTestApp( "Wyłącz ICDTCP3", "> Wyłączyć?",
          "icd-shutdown", "Wyłączanie  - -", "Wyłączanie   - -" ) );
    //end menu

#define MIN_WAIT 333
#define DEFAULT_WAIT 1000
#define RETURN_TIME 600000

#define KBD_UP 260
#define KBD_DOWN 261
#define KBD_LEFT 259 //TODO: poprawne kody klawiszy
#define KBD_RIGHT 258
#define KBD_ENTER 256
#define KBD_ESC 257

    if ( !exit ) {

      int wait=MIN_WAIT;
      int toReturn;

      mainMenu->screen( &lcd );
      lcdDrv.print( lcd );

      int kbd;
      kbd=open( "/dev/input/event0", O_RDONLY );

      struct input_event ev;
      struct pollfd fds[1];

      int esc=0;

      while( !exit ) {
        fds[0].fd=kbd;
        fds[0].events=POLLIN;

        if( toReturn >= RETURN_TIME ) {
          mainMenu->fullEsc();
          mainMenu->screen( &lcd );
          lcdDrv.backlight(0); //wyłącz podswietlenie
          toReturn=0;
        }
        poll( fds, 1, wait );
        toReturn+=wait;


        if( fds[0].revents & POLLIN ) {
          read( kbd, ( char* )&ev, sizeof( ev ) );
          if( ev.code!=0 ) {//rzucany pusty event niewiadomo czemu
            toReturn=0;
            lcdDrv.backlight(1); //włącz podswietlenia ( po kliknieciu )
            if( ev.value==1 && ev.code==KBD_ESC ) {//wciśniecie esc
              esc=1;
            } else if( ev.value==0 && ev.code==KBD_ESC  ) {//puszczenie esc
              if( esc==1 ) {//esc nie uzyty jako shift
                mainMenu->esc( &lcd );
              }
              esc=0;
            } else if( ev.value==0 ) {//puszczenie innego klawisza
              if( esc!=0 ) {//esc wcisniety i puszczono inny
                esc=2;
                switch( ev.code ) {//esc wcisniety i puszczono inny
                  case KBD_UP:    mainMenu->fastGoto( conntestid );
                                  mainMenu->screen( &lcd ); break;
                  case KBD_DOWN:  mainMenu->fastGoto( fototestid );
                                  mainMenu->screen( &lcd ); break;
                  case KBD_LEFT:  break;
                  case KBD_RIGHT: break;
                  case KBD_ENTER: mainMenu->fastGoto( poweroff );
                                  mainMenu->screen( &lcd ); break;
                }
              } else {//puszczono inny bez esc
                switch( ev.code ) {
                  case KBD_UP:    mainMenu->up( &lcd ); break;
                  case KBD_DOWN:  mainMenu->down( &lcd ); break;
                  case KBD_LEFT:  mainMenu->left( &lcd ); break;//off
                  case KBD_RIGHT: mainMenu->right( &lcd ); break;//off
                  case KBD_ENTER: mainMenu->enter( &lcd ); break;
                }
              }
            }
            lcdDrv.print( lcd );
          }
        } else if( lcd._refresh!=0 ) {//skonczony czas bez klawisza
          mainMenu->screen( &lcd );
          lcdDrv.print( lcd );
        }

        if( lcd._refresh==0 ) {
          wait=DEFAULT_WAIT;
        } else if( lcd._refresh<MIN_WAIT ) {
          wait=MIN_WAIT;
        } else {
          wait=lcd._refresh;
        }

      }
      globalConfigDb->close();
    }
  } catch( std::exception& e ) {
    syslog << basename( argv[0] ) << " error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}




