//#include <iostream>
//#include <vector>
//#include <list>

#include "transfer-agent.h"
#include "curlcc.h"
#include "sqlite3cc.h"

int main(int argc, char* argv[])
{
  std::cout << "Hello world!!!\n";

  sqlite3cc::conn db;
  const char* db_name = "/home/tomasz/projects/icdtcp3/icdtcp3/live.db";
  db.open(db_name);

  std::string base_url("http://www.insofter.pl/");
  idctcp3::transfer_agent ta(base_url);

  ta.login();
  ta.get_time();

//  ta.test_session();
//  ta.test_session();
//  ta.test_session();

  ta.send_data();

  sqlite3cc::rowset entries(db);
  entries.db_select("SELECT id, itd_id, dtm, cnt, dark_time, work_time, test, flags FROM flow");

  std::vector<sqlite3cc::row *>::iterator i;  
  for (i = entries.get_rows().begin(); i < entries.get_rows().end(); i++)
  {
    sqlite3cc::row *r = *i;
    std::cerr << "id=" << r->id << "|"
     << "itd_id=" << r->itd_id << "|"
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
