#include <linux/input.h>
#include <getopt.h>
#include <cstdlib>
#include <libgen.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

#include "counters-utils.hpp"
#include "counters-counters.hpp"
#include "counters-farm.hpp"
#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"

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
    "A cunters daemon.\n"
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
  CcountersFarm xxx;

/*  try {
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
  }
  //parametry uruchomienia -- koniec


  globalConfigDb=new sqlite3cc::conn();
  globalConfigDb->open( std::getenv("ICD_CONFIG_DB") );
  globalConfigDb->busy_timeout( db_timeout );
  globalConfig=new icd::config( *globalConfigDb );

  globalLiveDb=new sqlite3cc::conn();
  globalLiveDb->open( std::getenv("ICD_LIVE_DB") );
  globalLiveDb->busy_timeout( db_timeout );


  CcountersFarm farm;

  farm.addDevice( "/dev/itd0", 0 );

  return farm.run();
*/
}
/*
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
//TODO: wyłącz podswietlenie
toReturn=0;
}
poll( fds, 1, wait );
toReturn+=wait;


if( fds[0].revents & POLLIN ) {
read( kbd, ( char* )&ev, sizeof( ev ) );
if( ev.code!=0 ) {//rzucany pusty event niewiadomo czemu
toReturn=0;
//TODO: włącz podswietlenia ( po kliknieciu )
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
}*/
