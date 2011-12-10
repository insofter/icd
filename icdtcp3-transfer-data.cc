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
