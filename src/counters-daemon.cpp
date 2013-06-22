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
        return 1;
        break;
    }
    if( exit ) {
      break;
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

/*  farm.addDevice( "/dev/itd0", 0 );
  farm.addDevice( "/dev/itd1", 1 );
  farm.addDevice( "/dev/itd2", 2 );
  farm.addDevice( "/dev/itd3", 3 );*/

  farm.addDevice( "/tmp/itd0", 0 );
  farm.addDevice( "/tmp/itd1", 1 );

  farm.addCounter( new CcounterThick( 2, 0, 1, Ctime(), Ctime( 2, 0 ), Ctime( 2, 0 ), Ctime( 2, 0 ), Ctime( 2, 0 ) ) );

  farm.run();

  return 0;
}
