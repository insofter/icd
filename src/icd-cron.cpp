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

 
int main( int argc, char *argv[] ) {

  int db_timeout = 60000; 
  bool end = false;
  char * mem;
  int ans;
  daemonizer daemon;
  bool run_as_daemon = false;
  std::string s;


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
        s = optarg;
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
    if( !end && s.empty() ) {
      std::cerr << "Missing '--db' parameter" << std::endl;
      exit(1);
    }
    if (run_as_daemon && daemon.fork() ) {
      std::cerr << "Forking to background..." << std::endl;
      exit(0);
    }
//parametry uruchomienia -- koniec

  sqlite3cc::conn db;

  std::string cmd;
  cmd="icd-transfer-data --db=\"";
  cmd+=s;
  cmd+="\"";


  while( 1==1 ) {

    system( cmd.c_str() );
    system( "date" );
    system( "date >> /tmp/last" );

    db.open( s.c_str() );
    db.busy_timeout(db_timeout);
    icd::config config( db );

    int delay;//delay time
    delay=60*atoi( (config.entry( "device",
            "server-sync-period-mins" )).c_str() );   
    db.close();

    if( delay < 60 ) {
      delay = 60;
    }//delay == delay time in seconds

    unsigned int rand;//random value
    int rfile = open("/dev/urandom", O_RDONLY);
    read(rfile, (void*) &rand, sizeof( int ) );
    close( rfile );

    int t=time(NULL);//time
//    std::cerr << " time=" << t << std::endl;

    t/=delay;
    t*=delay;//time rounded to last sending time
//    std::cerr << " last=" << t << std::endl;
  
    t+=delay;//time of next sending
//    std::cerr << " next=" << t << std::endl;

    t+=( (float)delay*(float)rand/(float)UINT_MAX*0.1 );
              //time shifted with 0.1*delay*random

//    std::cerr << "shift=" << t << std::endl;
//    std::cerr << "sleep=" << t-time(NULL) << std::endl;

    sleep( t-time(NULL) );
//    sleep( 1 );
  }

  return 0;
}
