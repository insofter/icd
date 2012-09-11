#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include "daemonizer.h"
#include "logger.hpp"
#include <getopt.h>
#include <sstream>

#include "gsoap/icdtcp3SoapProxy.h"
#include "gsoap/icdtcp3Soap.nsmap"

//gsoap/_Stub.h -- klasy parametrów
//gsoap/_Service1SoapProxy.h -- funkcje serwera

icd::config *globalConfig;
sqlite3cc::conn *globalDb;


int createData( std::string & data ) {
  sqlite3cc::stmt stmt( *globalDb );

  std::ostringstream ss;

  stmt.prepare( "SELECT id, itd, "
      "datetime(dtm, 'unixepoch', 'localtime'), cnt, dark_time, "
      "work_time FROM flow WHERE flags > 0 ORDER BY dtm ASC LIMIT 16" );

  while( stmt.step() == SQLITE_ROW ) {
    ss << stmt.column_int(0) << ';' //id
      << stmt.column_text(1) << ';' //itd
      << stmt.column_text(2) << ';' //dtm
      << stmt.column_int(3) << ';' //cnt
      << stmt.column_int(4) << ';' //drk
      << stmt.column_int(5) << ';' //wrk
      << std::endl;
  }
  stmt.finalize();

  data = ss.str();

  if( data.empty() ) {
    return 0;
  } else {
    return 1;
  }
}

void commitData( const std::string & data ) {


}


void print_version(char *argv0) {
    std::cerr << basename(argv0) << " " << version << "\n"
      << copyright << std::endl;
}



void print_usage(char *argv0) {
  std::cerr 
    << "\n"
    "Usage: " << basename(argv0) << " OPTION...\n"
    "\n"
    "This program sends data to server.\n"
    "\n"
    "Options:\n"
    "  -d|--db=DB_NAME          Database file path;\n"
    "                               Mandatory option\n"
    "\n"
    "  -t|--timeout=TIMEOUT     Timeout for access to\n"
    "                               the database in ms\n"
    "\n"
    "  -l|--log=LOG_MODE        How many information you need:\n"
    "                           silent - no output, only info in data-\n"
    "                               base and exitcode (default option)\n"
    "                           short - prints percents in nonblocking\n"
    "                               mode and status line              \n"
    "                           long - shows everything: communication\n"
    "                               with server, responses, data pac- \n"
    "                               kages etc.                        \n"
    "\n"
    "  -h|--help\n"
    "  -v|--version\n"
    ;
}





int main( int argc, char *argv[] ) {

  Clog log;
  std::string db_name;
  int db_timeout = 60000; 
  bool end = false;

//parametry uruchomienia
    struct option long_options[] = {
      { "db", required_argument, 0, 'd' },
      { "timeout", required_argument, 0, 't' },
      { "log", optional_argument, 0, 'l' },
      { "help", no_argument, 0, 'h' },
      { "version", no_argument, 0, 'v' },
      { 0, 0, 0, 0 }
    };

    while( 1==1 ) {
      int option_index = 0;
      int ch = getopt_long(argc, argv, "d:t:l::hv", long_options, &option_index);
      if (ch == -1)
        break;
      switch(ch) {
      case 'd':
        db_name = optarg;
        break;
      case 't':
        db_timeout = strtol(optarg, NULL, 0);
        break;
      case 'l':
        //printf("%s", optarg);
        if( strcmp(optarg, "short")==0 ) {
          log=Clog(Clog::SHORT);
        } else if( strcmp(optarg, "long")==0 ) {
          log=Clog(Clog::LONG);
        } 
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
      if(end) {
        break;
      }
    }
    if( !end && db_name.empty() ) {
      std::cerr << "Missing '--db' parameter" << std::endl;
      exit(1);
    }
//parametry uruchomienia -- koniec

  globalDb=new sqlite3cc::conn();

  globalDb->open(db_name.c_str());
  globalDb->busy_timeout(db_timeout);

 // globalConfig=new icd::config( *globalDb );





  icdtcp3SoapProxy service;//("http://www.insofter.pl/pawo/icdtcp3webservice/Service1.asmx");
  int ans;
  std::string s;


  _icd1__LoginDevice login;
  _icd1__LoginDeviceResponse rlogin;

  login.idd=0;
  login.name=new std::string("Piotr");
  login.devInfo=new std::string("icdtcp3");

  log.okParams( 3, "LoginDevice" );

  ans=service.LoginDevice( &login, &rlogin );


  if( ans!=SOAP_OK ) {
    log.errSoap( 7, service.soap_fault_detail(), ans, "Błąd transmisji" );
    exit(1);
  }
  log.okSoap( 7, *(login.name) );

  delete login.name;
  delete login.devInfo;

  if( rlogin.LoginDeviceResult==0 ) {
    log.okServerAns( 10, *(rlogin.message) );
  } else {
    log.errServerAns( 10, *(rlogin.message), "błąd", "LD", "błąd logowania" );
    exit(1);
  }



  _icd1__GetTime time;
  _icd1__GetTimeResponse rtime;

  log.okParams( 13, "GetTime" );

  ans=service.GetTime(&time, &rtime);

  if( ans!=SOAP_OK ) {
    log.errSoap( 17, service.soap_fault_detail(), ans, "Błąd transmisji" );
    exit(1);
  }
  log.okSoap( 17, "GetTime" );

  log.okServerAns( 20, *(rtime.GetTimeResult) );
  if( rtime.GetTimeResult->size() >= 19 ) {
    rtime.GetTimeResult->at(4)='.';
    rtime.GetTimeResult->at(7)='.';
    rtime.GetTimeResult->at(10)='-';
    rtime.GetTimeResult->at(13)=':';
    rtime.GetTimeResult->at(16)=':';
    s="date -s ";
    s+=*(rtime.GetTimeResult);
    s+=" &>/dev/null";
    if( system( s.c_str() ) == 0 ) {
      system( "hwclock -w" );
    }
  }

//set time01234567890123456789
//date -s 2012.09.06-15:15:00
//Thu Sep  6 15:15:00 UTC 2012
///etc # hwclock -w
//
//system


//////////////////////////////////////////////////////
  _icd1__SendData data;
  _icd1__SendDataResponse rdata;
  data.data = &s;

  while( createData( s ) ) {
    log.okParams( 23, "SendData" );

    ans=service.SendData(&data, &rdata);

    if( ans!=SOAP_OK ) {
      log.errSoap( 27, service.soap_fault_detail(), ans, "Błąd transmisji" );
      exit(1);
    }
    log.okSoap( 27, s );

    commitData( *rdata.message );

    if( rdata.SendDataResult==0 ) {
      log.okServerAns( 30, *(rdata.message) );
    } else {
      log.errServerAns( 30, *(rdata.message), "too old", "SD", "błąd wysyłania" );
      exit(1);
    }
  }

////////////////////////////////////////////////////



  _icd1__LogoutDevice out;
  _icd1__LogoutDeviceResponse rout;

  log.okParams( 93, "OutDevice" );

  ans=service.LogoutDevice(&out, &rout);

  if( ans!=SOAP_OK ) {
    log.errSoap( 97, service.soap_fault_detail(), ans, "Błąd transmisji" );
    exit(1);
  }
  log.okSoap( 97, "OutDevice" );


  if( rout.LogoutDeviceResult==0 ) {
    log.okServerAns( 99, *(rout.message) );
  } else {
    log.errServerAns( 99, *(rout.message), "sesserr", "LO", 
        "błąd zamykania sesji" );
    exit(1);
  }

  log.done();
}



/*
//#include <iostream>
//#include <vector>
//#include <list>

#include "transfer-agent.h"
#include "curlcc.h"
#include "sqlite3cc.h"

class flow_entry_db : public idctcp3::flow_entry
{
  public:
    flow_entry_db(sqlite3cc::stmt& st)
    {
      id = st.column_int(0);
      itd = st.column_text(1);
      dtm = st.column_text(2);
      cnt = st.column_int(3);
      dark_time = st.column_int(4);
      work_time = st.column_int(5);
      test = st.column_text(6);
      flags = st.column_int(7);
    }
};

int main(int argc, char* argv[])
{
  std::cout << "Hello world!!!\n";

  sqlite3cc::conn db;
  const char* db_name = "/home/icdtcp3/projects/icd/live.db";
  db.open(db_name);

  std::string base_url("http://www.insofter.pl/");
  idctcp3::transfer_agent ta(base_url);

  ta.login();
  ta.get_time();

//  ta.test_session();
//  ta.test_session();
//  ta.test_session();

  ta.send_data();

  sqlite3cc::rowset<flow_entry_db> entries(db);
  entries.db_select("SELECT id, itd, dtm, cnt, dark_time, work_time, test, flags FROM flow");

  std::vector<flow_entry_db *>::iterator i;
  for (i = entries.get_rows().begin(); i < entries.get_rows().end(); i++)
  {
    flow_entry_db *r = *i;
    std::cerr << "id=" << r->id << "|"
     << "itd=" << r->itd << "|"
     << "dtm=" << r->dtm << "|"
     << "cnt=" << r->cnt << "|"
     << "dark_time=" << r->dark_time << "|"
     << "work_time=" << r->work_time << "|"
     << "test=" << r->test << "|"
     << "flags=" << r->flags << std::endl;
  }

  ta.logout();
  db.close();

  return 0;
}
*/
