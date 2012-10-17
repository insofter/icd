#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include <cstdlib>
#include <string.h>




icd::config *globalConfig;
sqlite3cc::conn *globalConfigDb;
sqlite3cc::conn *globalDataDb;
sqlite3cc::conn *globalLiveDb;

void print_version(char *argv0) {
  std::cerr << basename(argv0) << " " << version << "\n"
    << copyright << std::endl;
}


char * new_c_str( const std::string & s ) {
  char * mem = new char[ s.size()+1 ];
  strcpy( mem, s.c_str() );
  return mem;
}

int main( int argc, char *argv[] ) {

  int db_timeout = 60000; 

  globalConfigDb=new sqlite3cc::conn();
  globalConfigDb->open( std::getenv("ICD_CONFIG_DB") );
  globalConfigDb->busy_timeout( db_timeout );
  globalConfig=new icd::config( *globalConfigDb );

  globalDataDb=new sqlite3cc::conn();
  globalDataDb->open( std::getenv("ICD_DATA_DB") );
  globalDataDb->busy_timeout( db_timeout );

  globalLiveDb=new sqlite3cc::conn();
  globalLiveDb->open( std::getenv("ICD_LIVE_DB") );
  globalLiveDb->busy_timeout( db_timeout );

  /*
   * begin transaction datadb
   * select * from livedb
   * for every:
   *    select from datadb
   *    if id!=null 
   *      then update
   *      else insert
   * commit datadb
   * select from livedb where flags = 2
   * for every:
   *    select from data
   *    ifexist
   *      delete from livedb
   *
   *
   * http://www.sqlite.org/lang_transaction.html
*/

  return 0;
}
