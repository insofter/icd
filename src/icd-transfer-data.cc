#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include "daemonizer.h"
#include "logger.hpp"
#include <getopt.h>

#include "gsoap/Service1SoapProxy.h"
#include "gsoap/Service1Soap.nsmap"

//gsoap/_Stub.h -- klasy parametrów
//gsoap/_Service1SoapProxy.h -- funkcje serwera




void print_version(char *argv0) {
    std::cerr << basename(argv0) << " " << version << "\n"
      << copyright << std::endl;
}


icd::config *globalConfig;

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


/*
  log.okParams( 3, "LoginDevice" );

  log.okSoap( 7, "Hej Zenek, jak się masz?" );

  log.okServerAns( 10, "Żyję" );

  srand( time(NULL) );

  switch( rand()%5 ) {
    case 0:
      log.errParams( 13, "GetTime", "Pusta baza danych", "emptyDb" );
      break;

    case 1:
      log.errSoap( 17, "tcp_connect() error", 404, "Brak połączenia z internetem" );
      break;
    case 2:
      log.errServerAns( 20, "Server ans: NO BEER", "NO BEER", "GT",
      "Błąd, skończyło się piwo" );
      break;

    case 3:
      log.done();
      break;

    case 4:
      log.done(11);
      break;
  }


    exit(2);
*/
  Service1SoapProxy service("http://www.insofter.pl/pawo/icdtcp3webservice/Service1.asmx");
  int ans;

  _icd1__LoginDevice login;
  _icd1__LoginDeviceResponse rlogin;

  login.idd=2;
  login.name=new std::string("Piotr");
  login.DevInfo=new std::string("icdtcp3");

  ans=service.LoginDevice( &login, &rlogin );
  delete  login.name;

  if( ans!=SOAP_OK ) {
//    service.soap_stream_fault(std::cerr); 

    std::cout << "Błąd połączenia: " << service.soap_fault_detail() << std::endl;
  }


  std::cout << "Odp serwera: " << *(rlogin.LoginDeviceResult) << std::endl;

  exit(0);
  _icd1__HelloWorld a; 
  _icd1__HelloWorldResponse b;


  if( service.HelloWorld(&a, &b) == SOAP_OK ) {
    std::cerr << "SOAP_OK" << std::endl;
    std::cerr << *(b.HelloWorldResult) << std::endl;
  } else {
    service.soap_stream_fault(std::cerr); 
  }

  _icd1__TestSession c;
  _icd1__TestSessionResponse d;
  if( service.TestSession(&c, &d) == SOAP_OK ) {
    std::cerr << "SOAP_OK" << std::endl;
    std::cerr << d.TestSessionResult << std::endl;
  } else {
    service.soap_stream_fault(std::cerr);
  }


  _icd1__GetTime e;
  _icd1__GetTimeResponse f;

  if( service.GetTime(&e, &f) == SOAP_OK ) {
    std::cerr << "SOAP_OK" << std::endl;
    std::cerr << *(f.GetTimeResult) << std::endl;
  } else {
    service.soap_stream_fault(std::cerr);
  }




  if( service.TestSession(&c, &d) == SOAP_OK ) {
    std::cerr << "SOAP_OK" << std::endl;
    std::cerr << d.TestSessionResult << std::endl;
  } else {
    service.soap_stream_fault(std::cerr);
  }




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
