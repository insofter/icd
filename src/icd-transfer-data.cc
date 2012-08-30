#include "gsoap/Service1SoapProxy.h"
#include "gsoap/Service1Soap.nsmap"
/*#ifdef DESKTOP//different headers on arm and desktop,
              //use -DDESKTOP durng compilation on desktop 
#include <unistd.h>
#endif
*/

//gsoap/_Stub.h -- klasy parametrów
//gsoap/_Service1SoapProxy.h -- funkcje serwera
//
//
//
int print_percent(int perc) {
  struct pollfd fds[1];

  fds[0].fd=STDOUT_FILENO;
  fds[0].events=POLLOUT;
  poll( fds, 1, 0 );

  if( fds[0].revents & POLLOUT ) {
    char buf[5];
    int x;
  
    if( perc > 100 ) {
      perc=100;
    } else if( perc < 0 ) {
      perc=0;
    }
    x=sprintf( buf,"%i\n", perc );
    write(STDOUT_FILENO, buf, x );
    return perc;
  }
  return -1;
}


int main() {
  Service1SoapProxy service;

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
