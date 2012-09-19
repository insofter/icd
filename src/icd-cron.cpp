#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include "daemonizer.h"
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
    "  -d|--db=DB_NAME          Database file path;\n"
    "                               Mandatory option\n"
    "\n"
    "  -t|--timeout=TIMEOUT     Timeout for access to\n"
    "                               the database in ms\n"
    "\n"
    "  -b|--daemon                  Run as a daemon\n"
    "  -p|--pidfile=FILE            Create pid file (if a daemon)\n"
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
 
int getDelay( std::string & s, int db_timeout ) {
  int delay;
  sqlite3cc::conn db;
  db.open( s.c_str() );
  db.busy_timeout(db_timeout);
  icd::config config( db );

  delay=60*atoi( (config.entry( "device",
          "server-sync-period-mins" )).c_str() );   
  db.close();

  if( delay < 60 ) {
    delay = 60;
  }//delay == delay time in seconds
  return delay;
}

int main( int argc, char *argv[] ) {

  int db_timeout = 60000; 
  bool end = false;
  char * mem;
  int ans;
  daemonizer daemon;
  bool run_as_daemon = false;
  std::string db;


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

    while( end==false ) {
      int option_index = 0;
      int ch=getopt_long(argc, argv, "d:t:bphv", long_options, &option_index);
      if (ch == -1)
        break;
      switch(ch) {
      case 'd':
        db = optarg;
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
    if( !end && db.empty() ) {
      std::cerr << "Missing '--db' parameter" << std::endl;
      exit(1);
    }
    if (run_as_daemon && daemon.fork() ) {
      std::cerr << "Forking to background..." << std::endl;
      exit(0);
    }
//parametry uruchomienia -- koniec

#define FLUSH 180

  std::string cmd;
  cmd="icd-transfer-data --db=\"";
  cmd+=db;
  cmd+="\"";

  int delay;//delay time
  int ndelay;
  int next;
  int flush;
  flush=time(NULL)+FLUSH;

  //sleep for first sync to avoid flooding
  sleep( randVal()*20.0+10 );


  while( 1==1 ) {

    system( cmd.c_str() );//run command

    next=time(NULL);//time
    delay=getDelay( db, db_timeout );
    next/=delay;
    next*=delay;//time rounded to last sending time
  
    next+=delay*( randVal()*0.1+1 );//time of next sending
              //time shifted with 0.1*delay*random

    if( time(NULL) > flush ) {
      system( "icd-sync-db" );
      flush=time(NULL)+FLUSH;
    }

    while( time(NULL) < next ) {
      sleep( 30+30*randVal() );
      ndelay=getDelay( db, db_timeout );
      if( delay!=ndelay ) {
        delay=ndelay;
        next=time(NULL);
        next/=delay;
        next*=delay;
        next+=delay*( randVal()*0.1+1 );
      }
      if( time(NULL) > flush ) {
        system( "icd-sync-db" );
        flush=time(NULL)+FLUSH;
      }
    }
  }

  return 0;
}
