#include <iostream>
#include <sstream>

#include "sqlite3cc.h"

namespace idctcp3
{
  class aggregation_agent
  {
    public:
      aggregation_agent(const std::string& dbname);
      ~aggregation_agent()
    
      void run();

    private:
      sqlite3cc::conn db;  
      time_t dtm;

      void get_current_time();
      void create_flow_entires();
      void exec_aggregation();
      void update_event_flags();
      void remove_old_events();
    
  };

  aggregation_agent::aggregation_agent(const std::string& dbname)
  {
    db.open(dbname.cstr());
    db.busy_timeout(60 * 1000);
    get_current_time();
  }  

  aggregation_agen::~aggregation_agent()
  {
    db.close();
  }

  void aggregation_agent::run()  
  {
    db.exec("BEGIN EXCLUSIVE TRANSACTION");

    create_flow_entires();
    exec_aggregation();
    update_event_flags();
    remove_old_events();

    db.exec("COMMIT TRANSACTION");
  }

  void aggregation_agent::get_current_time()
  {
    sqlite3cc::stmt stmt(db);
    const char *now_sql = "SELECT strftime('%s','now')";
    stmt.prepare(now_sql);
    stmt.step();
    dtm = stmt.column_int(0);
    stmt.finalize();
  }

  void aggregation_agent::create_flow_entires()
  {
  }

  void aggregation_agent::exec_aggregation()
  {
  }

  void aggregation_agent::update_event_flags()
  {
  }

  void aggregation_agent::remove_old_events()
  {
  }
}

int main(int argc, char *argv[])
{
  std::cout << "icdtcd3-aggr-data: Hello world;\n";

  aggregation_agent aggr("live.db");
  aggr.run();



  //TODO get last aggregation time from current config
  time_t last = 1322390560;
  time_t period = 900; // 15 mins in seconds TODO get this value from current config
  time_t events_retention_period = 43200; //12 hours  //TODO get it form current config

  const char * insert_sql = "INSERT OR IGNORE INTO flow (itd_id, dtm) VALUES (?1, ?2)";
  stmt.prepare(insert_sql);

  time_t dtm = (last / period) * period;
  while (dtm < now)
  {
    std::cout << "dtm = " << dtm << std::endl;

    std::ostringstream oss;
    oss << dtm;

    stmt.bind_int(1, 0);
    stmt.bind_int(2, dtm);
    stmt.step();
    stmt.reset();
    stmt.clear_bindings();

    dtm += period;
  }
  stmt.finalize();

/*
SELECT f.id, f.itd_id, f.dtm,
  f.cnt + (SELECT count(*) FROM events e1 JOIN events e2
      ON e2.dtmms == (SELECT MIN(x.dtmms) FROM events x WHERE x.dtmms > e1.dtmms)
      AND e2.itd_id == e1.itd_id
    WHERE e1.dtmms >= f.dtm * 1000 AND e1.dtmms < (f.dtm + 900) * 1000
      AND e1.itd_id == f.itd_id AND e1.state == 1 AND e1.flags != 0),
  f.dark_time + (SELECT SUM(max(f.dtm * 1000, min((f.dtm + 900) * 1000, e2.dtmms))
      - max(f.dtm * 1000, min((f.dtm + 900) * 1000, e1.dtmms)))
    FROM events e1 JOIN events e2
      ON e2.dtmms == (SELECT MIN(x.dtmms) FROM events x WHERE x.dtmms > e1.dtmms)
      AND e2.itd_id == e1.itd_id
    WHERE e1.itd_id == f.itd_id AND e1.state == 1),
  f.work_time + (max(f.dtm, min(f.dtm + 900, 1322942565))
    - max(f.dtm, min(f.dtm + 900, 1322926200))) * 1000
FROM flow f
WHERE f.dtm >= (1322926200 / 900) * 900;
*/

//set flags of all events to 0;
//Remember - if last event is 1 (has status 1) then it is not considered in the this aggregation
// (the sql does not take it into account)
//and should not be marked as send (flag stays 1)
/*
UPDATE events SET flags = 0
WHERE id IN (SELECT e1.id FROM events e1
    JOIN events e2
      ON e2.dtmms == (SELECT MIN(x.dtmms) FROM events x WHERE x.dtmms > e1.dtmms)
      AND e2.itd_id == e1.itd_id)
  AND flags != 0;
*/

//Remove all events older than 'events retention period'

/*
DELETE FROM events WHERE dtmms < (1323013552 - 43200) * 1000;
*/


  return 0;
}
