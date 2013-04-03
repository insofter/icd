#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include <getopt.h>
#include <time.h>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include <libgen.h>
#ifdef DESKTOP//different headers on arm and desktop,
//use -DDESKTOP durng compilation on desktop 
#include <unistd.h>
#endif
#include <fcntl.h>
#include <limits.h>
#include <cstdio>


icd::config *globalConfig;
sqlite3cc::conn *globalConfigDb;

void print_version(char *argv0) {
  std::cerr << basename(argv0) << " " << version << "\n"
    << copyright << std::endl;
}

void print_usage(char *argv0) {
  std::cerr 
    << "\n"
    "Usage: " << basename(argv0) << " OPTION...\n"
    "\n"
    "This program runs sening data to server periodically.\n"
    "\n"
    "Options:\n"
    "\n"
    "  -h|--help\n"
    "  -v|--version\n"
    ;
}

float randVal() {
  unsigned int rand;
  int rfile = open("/dev/urandom", O_RDONLY);
  read(rfile, (void*) &rand, sizeof( int ) );
  close( rfile );

  return (float)rand/(float)UINT_MAX;
}

#define MIN_SYNC_DELAY 180
int getDelay() {
  int delay;

  delay=60*atoi( (globalConfig->entry( "device",
          "server-sync-period-mins" )).c_str() );   

  if( delay < MIN_SYNC_DELAY ) {
    delay = MIN_SYNC_DELAY;
  }//delay == delay time in seconds
  return delay;
}

int main( int argc, char *argv[] ) {

  int db_timeout = 60000; 
  bool end = false;
  int ans;


  //parametry uruchomienia
  struct option long_options[] = {
    { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
    { 0, 0, 0, 0 }
  };

  while( end==false ) {
    int option_index = 0;
    int ch=getopt_long(argc, argv, "hv", long_options, &option_index);
    if (ch == -1)
      break;
    switch(ch) {
      case 'h':
        print_usage(argv[0]);
        end = true;
        break;
      case 'v':
        print_version(argv[0]);
        end = true;
        break;
MIN:
        std::cerr << "Unknown option ` " << char(ch) << " '" << std::endl;
        exit(1);
        break;
    }
  }
  //parametry uruchomienia -- koniec

  globalConfigDb=new sqlite3cc::conn();
  globalConfigDb->open( std::getenv("ICD_CONFIG_DB") );
  globalConfigDb->busy_timeout( db_timeout );
  globalConfig=new icd::config( *globalConfigDb );


#define FLUSH_DELAY 180

  sleep( randVal()*20.0+10 );
  system( "icd-transfer-data" );//first sync


  int timeOfNextTransfer;
  int timeOfNextFlush;
  int timeNow;
  int delayNow=0;


  while( 1==1 ) {

    sleep( 30 );//sleep to avoid running loop twice in one second
    timeNow=time(NULL);
    timeOfNextFlush=timeNow;
    timeOfNextFlush/=FLUSH_DELAY;
    timeOfNextFlush+=1;
    timeOfNextFlush*=FLUSH_DELAY;
    timeOfNextFlush+=5;//rounded to 3 min, + 5 secs

    if( delayNow!=getDelay() ) {//delay changed by user or transfer 
      delayNow=getDelay();
      timeOfNextTransfer=timeNow;
      timeOfNextTransfer/=delayNow;
      timeOfNextTransfer+=1;
      timeOfNextTransfer*=delayNow;
      if( delayNow > 300 ) {//rand of no more than 5 minutes
        timeOfNextTransfer+=(int)((double)randVal()*(double)0.1*300.0);
      } else {
        timeOfNextTransfer+=(int)((double)randVal()*(double)0.1*(double)delayNow);
      }
      timeOfNextTransfer+=30;
    }

    if( timeOfNextTransfer<=timeOfNextFlush ) {//transfer will be faster than flush
      if( timeOfNextTransfer-timeNow > 0 ) {
        sleep( timeOfNextTransfer-timeNow );
      }
      system( "icd-flush-db" );
      system( "icd-transfer-data" );
      delayNow=0;//force get delay and calculate new time

    } else {//only flush
      if( timeOfNextFlush-timeNow > 0 ) {
        sleep( timeOfNextFlush-timeNow );
      }
      system( "icd-flush-db" );

    }
  }//end while( 1==1 )

  return 0;
}
