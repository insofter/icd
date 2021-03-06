#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include <cstdlib>
#include <string.h>
#include <cstdio>
#include <time.h>

icd::config *globalConfig;
sqlite3cc::conn *globalConfigDb;
sqlite3cc::conn *globalDataDb;
sqlite3cc::conn *globalLiveDb;


void print_version(char *argv0) {
  std::cerr << basename(argv0) << " " << version << "\n"
    << copyright << std::endl;
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


  sqlite3cc::stmt dataDbSql( *globalDataDb );
  dataDbSql.prepare( "BEGIN IMMEDIATE TRANSACTION" );
  dataDbSql.step();
  dataDbSql.finalize();

  sqlite3cc::stmt dataDbSqlSelect( *globalDataDb );
  dataDbSqlSelect.prepare( "SELECT id FROM flow WHERE counter_id == ?1 AND dtm == ?2 LIMIT 1" );

  sqlite3cc::stmt dataDbSqlUpdate( *globalDataDb );
  dataDbSqlUpdate.prepare( "UPDATE flow SET cnt = ?3, dark_time = ?4, work_time = ?5, test = ?6, flags = ?7 WHERE counter_id == ?1 AND dtm ==?2" );

  sqlite3cc::stmt dataDbSqlInsert( *globalDataDb );
  dataDbSqlInsert.prepare( "INSERT INTO flow (counter_id, dtm, cnt, dark_time, work_time, test, flags) VALUES( ?1, ?2, ?3, ?4, ?5, ?6, ?7 )" );

  sqlite3cc::stmt liveDbSqlSelect( *globalLiveDb );
  liveDbSqlSelect.prepare( "SELECT counter_id, dtm, cnt, dark_time, work_time, test, flags FROM flow" );

  while( liveDbSqlSelect.step() == SQLITE_ROW ) {
    // "SELECT id FROM flow WHERE counter_id == ?1 AND dtm ==?2"
    dataDbSqlSelect.bind_int( 1, liveDbSqlSelect.column_int(0) );
    dataDbSqlSelect.bind_int( 2, liveDbSqlSelect.column_int(1) );
    if( dataDbSqlSelect.step() == SQLITE_ROW ) {
      //"UPDATE flow SET cnt = ?3, dark_time = ?4, work_time = ?5, flags = ?6 WHERE counter_id == ?1 AND dtm ==?2"
      dataDbSqlUpdate.bind_int( 1, liveDbSqlSelect.column_int(0) );//counter_id
      dataDbSqlUpdate.bind_int( 2, liveDbSqlSelect.column_int(1) );//dtm
      dataDbSqlUpdate.bind_int( 3, liveDbSqlSelect.column_int(2) );//cnt
      dataDbSqlUpdate.bind_int( 4, liveDbSqlSelect.column_int(3) );//dark_time
      dataDbSqlUpdate.bind_int( 5, liveDbSqlSelect.column_int(4) );//work_time
      dataDbSqlUpdate.bind_int( 6, liveDbSqlSelect.column_int(5) );//test
      dataDbSqlUpdate.bind_int( 7, liveDbSqlSelect.column_int(6) );//flags
      dataDbSqlUpdate.step();
      dataDbSqlUpdate.reset();
    } else {
      //"INSERT INTO flow (counter_id, dtm, cnt, dark_time, work_time, flags) VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )"
      dataDbSqlInsert.bind_int( 1, liveDbSqlSelect.column_int(0) );//counter_id
      dataDbSqlInsert.bind_int( 2, liveDbSqlSelect.column_int(1) );//dtm
      dataDbSqlInsert.bind_int( 3, liveDbSqlSelect.column_int(2) );//cnt
      dataDbSqlInsert.bind_int( 4, liveDbSqlSelect.column_int(3) );//dark_time
      dataDbSqlInsert.bind_int( 5, liveDbSqlSelect.column_int(4) );//work_time
      dataDbSqlInsert.bind_int( 6, liveDbSqlSelect.column_int(5) );//test
      dataDbSqlInsert.bind_int( 7, liveDbSqlSelect.column_int(6) );//flags
      dataDbSqlInsert.step();
      dataDbSqlInsert.reset();
    }
    dataDbSqlSelect.reset();
  }
  liveDbSqlSelect.finalize();
  dataDbSqlUpdate.finalize();
  dataDbSqlInsert.finalize();

  //remove leftover records
  int oldestRecord=0;
  int cutTime=time(NULL)-(globalConfig->entry_long( "device", "flow-entry-retention-period-days" )*24*3600);
  dataDbSql.prepare( "SELECT min(dtm) FROM flow" );
  if( dataDbSql.step() == SQLITE_ROW ) {
    oldestRecord=dataDbSql.column_int(0);
    if( cutTime-oldestRecord > 24*3600 ) {
      dataDbSql.finalize();
      dataDbSql.prepare( "DELETE FROM flow WHERE dtm < ?1" );
      dataDbSql.bind_int( 1, cutTime );
      dataDbSql.step();
    }
  }
  dataDbSql.finalize();

  dataDbSql.prepare( "COMMIT" );
  dataDbSql.step();
  dataDbSql.finalize();

  //live.db cleanup
  sqlite3cc::stmt liveDbSqlDelete( *globalLiveDb );
  liveDbSqlDelete.prepare( "DELETE FROM flow WHERE counter_id == ?1 AND dtm == ?2" );

  liveDbSqlSelect.prepare( "SELECT counter_id, dtm FROM flow WHERE flags = 2" );
  while( liveDbSqlSelect.step() == SQLITE_ROW ) {
    dataDbSqlSelect.bind_int( 1, liveDbSqlSelect.column_int(0) );
    dataDbSqlSelect.bind_int( 2, liveDbSqlSelect.column_int(1) );
    if( dataDbSqlSelect.step() == SQLITE_ROW ) {
      liveDbSqlDelete.bind_int( 1, liveDbSqlSelect.column_int(0) );//counter_id
      liveDbSqlDelete.bind_int( 2, liveDbSqlSelect.column_int(1) );//dtm
      liveDbSqlDelete.step();
      liveDbSqlDelete.reset();
    }
    dataDbSqlSelect.reset();
  }
  dataDbSqlSelect.finalize();
  liveDbSqlDelete.finalize();



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
*/

  return 0;
}
