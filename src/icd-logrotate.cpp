#include "version.h"
#include <cstdlib>
#include <iostream>
#ifdef DESKTOP//different headers on arm and desktop,
//use -DDESKTOP durng compilation on desktop 
#include <unistd.h>
#endif
#include <fcntl.h>
#include <cstring>
#include <cstdio>


void print_version(char *argv0) {
  std::cerr << basename(argv0) << " " << version << "\n"
    << copyright << std::endl;
}

void print_usage(char *argv0) {
  std::cerr 
    << "\n"
    "Usage: " << basename(argv0) << " logfile\n"
    "\n"
    "This  program   reads  log  from  stdin \n"
    "and put it to log files and rotate them.\n"
    "\n"
    "Example:\n"
    "  daemon --daemon params | " << basename(argv0) << " logfile\n"
    "\n"
    "Files:\n"
    "  logfile.0\n"
    "  logfile.1\n"
    "  logfile.2\n"
    "\n"
    "Options:\n"
    "\n"
    "  --help\n"
    "  --version\n"
    ;
}

#define NUMBER_OF_FILES 3
#define BUFSIZE 512
#define LOGSIZE 49995

void rotate( char * name ) {
  char newname[BUFSIZE];
  char oldname[BUFSIZE];
  int i;
  for( i=NUMBER_OF_FILES-1; i>0; --i ) {
    sprintf( newname, "%s.%i", name, i );
    sprintf( oldname, "%s.%i", name, i-1 );
    rename( oldname, newname );
  }
}

int main( int argc, char *argv[] ) {

  char buf[BUFSIZE];
  int logfile;

  int x=1;
  int logsize;

  if( argc < 2 ) {
    std::cerr << "No log file" << std::endl;
    return 1;
  } else if( strcmp( argv[1], "--help" )==0 ) {
    print_usage(argv[0]);
    return 0;
  } else  if( strcmp( argv[1], "--version" )==0 ) {
    print_version(argv[0]);
    return 0;
  } else if( strlen( argv[1] )> BUFSIZE-5 ) {
      std::cerr << "File name too long" << std::endl;
      return 1;
  } else {
    while( x > 0 ) { 
      rotate( argv[1] );
      logsize=0;
      sprintf( buf, "%s.0", argv[1] );
      logfile=open( buf, O_WRONLY|O_CREAT, 0644 );
      if( logfile==-1 ) {
        std::cerr << "Unable to open log file: " << buf << std::endl;
        return 1;
      }

      while( logsize < LOGSIZE && x > 0 ) {
        x=read( 0, buf, BUFSIZE );
        logsize+=x;
        write( logfile, buf, x );
      }
      close( logfile );
    }

  }
  return 0;
}
