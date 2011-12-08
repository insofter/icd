#include <string>
#include "sqlite3cc.h"

namespace icdtcp3
{
  class config
  {
    public:
      config(sqlite3cc::conn& db) : db(db) {}
      ~config() {}

      std::string entry(const std::string& section, const std::string& key);
      void set_entry(const std::string& section, const std::string& key, const std::string& value);
      void remove_entry(const std::string& section, const std::string& key);

      void add_section(const std::string& name);
      void remove_section(const std::string& name);

    private:
      sqlite3cc::conn& db;
  };

  std::string config::entry(const std::string& section, const std::string& key)
  {
    sqlite3cc::stmt stmt(db);
    const char *sql = "SELECT c.value "
      "FROM config c JOIN config_section s ON c.section_id == s.id "
      "WHERE s.name == ?1 AND c.key == ?2";
    stmt.prepare(sql);
    stmt.bind_text(1, section.c_str());
    stmt.bind_text(2, key.c_str());
    stmt.step();
    std::string value = stmt.column_text(0);
    stmt.finalize();
    return value;
  }

  void config::set_entry(const std::string& section, const std::string& key, const std::string& value)
  {
    sqlite3cc::stmt stmt(db);
    const char *sql = "INSERT OR REPLACE INTO config (section_id, key, value) "
      "VALUES ( (SELECT s.id FROM config_section s WHERE s.name = ?1), ?2, ?3)";
    stmt.prepare(sql);
    stmt.bind_text(1, section.c_str());
    stmt.bind_text(2, key.c_str());
    stmt.bind_text(3, value.c_str());
    stmt.step();
    stmt.finalize();
  }

  void config::remove_entry(const std::string& section, const std::string& key)
  {
    sqlite3cc::stmt stmt(db);
    const char *sql = "DELETE FROM config WHERE "
      "section_id == (SELECT s.id FROM config_section s WHERE s.name == ?1) "
      "AND key == ?2";
    stmt.prepare(sql);
    stmt.bind_text(1, section.c_str());
    stmt.bind_text(2, key.c_str());
    stmt.step();
    stmt.finalize();
  }

  void config::add_section(const std::string& name)
  {
    sqlite3cc::stmt stmt(db);
    const char *sql = "INSERT INTO config_section (name) VALUES (?1)";
    stmt.prepare(sql);
    stmt.bind_text(1, name.c_str());
    stmt.step();
    stmt.finalize();
  }

  void config::remove_section(const std::string& name)
  {
    sqlite3cc::stmt stmt(db);
    const char *sql = "DELETE FROM config_section WHERE name == ?1";
    stmt.prepare(sql);
    stmt.bind_text(1, name.c_str());
    stmt.step();
    stmt.finalize();
  }

  struct config_entry
  {  
    int id;
    int section_id;
    std::string section;
    std::string key;
    std::string value;
  };

/*  class config
  {
    public:
      config(db);
      ~config();

      void load(); // loads data from db
      void save(); // stores data into db

      std::string entry(const std::string& section, const std::string& key) const;
      void set_entry(const std::string& section, const std::string& key, const std::string& value);
      void remove_entry(const std::string& section, const std::string& key) const;
      void remove_section(const std::string& name);

      vector<std::string> keys(const std::sting& section) const;
      vector<std::string> sections() const;

    private:
      enum state
      {
        UNCHANGED,
        NEW,
        MODIFIED,
        DELETED
      };

      struct entry
      {
        int id;
        int section_id;
        std::string key;
        std::string name;
        state st;
        section *section;
      };

      struct section
      {
        int id;
        std::string name;
        state st;
        std::map<std::string, entry> entries;
      };

      conn *db;
      std::map<std::string, section> sections;
  };*/
}

int main(int argc, char* argv[])
{
  sqlite3cc::conn db;
  db.open("live.db");
  db.busy_timeout(60 * 1000);

  icdtcp3::config config(db);

  std::string ip = config.entry("tcpip","ip");
  std::cout << "tcpip:ip=" << ip << std::endl;

  config.set_entry("tcpip","ipa","192.168.2.200");

  db.close();

  return 0;
}

