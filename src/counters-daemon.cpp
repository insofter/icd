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
  Ctime start;
  Ctime period( (globalConfig->entry_long( "device", "aggr-period-mins" )*60), 0 );//time of aggregation
  std::vector< std::string > devices;

  std::vector< std::string > counters=globalConfig->list_like( "counter" );
  for( int i=0; i< counters.size(); ++i ) {
    Ccounter * counter=NULL;
    if( globalConfig->entry_bool( counters[i], "enabled" ) ) {

      int counter_id=globalConfig->entry_long( counters[i], "counter_id" );

      std::string master=globalConfig->entry( counters[i], "master" );

      int master_id=-1;//management of devices, and setting ids for them
      for( int j=0; j<devices.size(); ++j ) {
        if( devices[j].compare( master )==0 ) {
          master_id=j;
          break;
        }
      }
      if( master_id==-1 ) {
        master_id=devices.size();
        devices.push_back( master );
      }//management of devices, and setting ids for them

      Ctime master_engage( 0, (globalConfig->entry_long( counters[i], "master-engage" )*1000) );
      Ctime master_release( 0, (globalConfig->entry_long( counters[i], "master-release" )*1000) );

      Econstants master_reversed=NORMAL;
      if( globalConfig->entry_bool( counters[i], "master-reversed" ) ) {
        master_reversed=REVERSE;
      }

      std::string slave_mode=globalConfig->entry( counters[i], "slave-mode" );

      if( slave_mode.compare( "single" )==0 ) {
        counter=new CcounterMono( counter_id, master_id , start, master_engage, master_release, master_reversed );
        std::cout << "CcounterMono id=" << counter_id << ", master=" <<  master_id << std::endl;
      } else if( slave_mode.compare( "thickness" )==0 ) {

        std::cout << "CcounterThick id=" << counter_id << ", master=" <<  master_id << std::endl;
        //farm.addCounter( new CcounterThick( 2, 0, 1, Ctime(), Ctime( 2, 0 ), Ctime( 2, 0 ), Ctime( 2, 0 ), Ctime( 2, 0 ) ) );


      }

      std::string stop=globalConfig->entry( counters[i], "stop" );
      //TODO:

      farm.addCounter( counter );
    }// if enabled
  }
  for( int i=0; i< devices.size(); ++i ) {
    farm.addDevice( devices[i], i );
  }

  /*
     slave slave
     slave-engage slave-engage
     slave-mode slave-mode
     slave-release slave-release
     slave-reversed slave-reversed
     stop stop
     stop-engage stop-engage
     stop-release stop-release
     stop-reversed stop-reversed
     */

  farm.run( period );

  return 0;
}
