#include "db-config.h"

namespace icdtcp3
{
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

  std::vector<config_entry> config::list_entries(const std::string& section)
  {
    std::vector<config_entry> list;
    sqlite3cc::stmt stmt(db);
    const char *sql = "SELECT s.name, c.key, c.value "
      "FROM config c JOIN config_section s ON c.section_id == s.id "
      "WHERE s.name == ?1";
    stmt.prepare(sql);
    stmt.bind_text(1, section.c_str());
    while (stmt.step() == SQLITE_ROW)
    {
      config_entry entry;
      entry.section = stmt.column_text(0);
      entry.key = stmt.column_text(1);
      entry.value = stmt.column_text(2);
      list.push_back(entry);
    }
    stmt.finalize();
    return list;
  }

  std::vector<config_entry> config::list_entries()
  {
    std::vector<config_entry> list;
    sqlite3cc::stmt stmt(db);
    const char *sql = "SELECT s.name, c.key, c.value "
      "FROM config c JOIN config_section s ON c.section_id == s.id";
    stmt.prepare(sql);
    while (stmt.step() == SQLITE_ROW)
    {
      config_entry entry;
      entry.section = stmt.column_text(0);
      entry.key = stmt.column_text(1);
      entry.value = stmt.column_text(2);
      list.push_back(entry);
    }
    stmt.finalize();
    return list;
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

  std::vector<std::string> config::list_sections()
  {
    std::vector<std::string> list;
    sqlite3cc::stmt stmt(db);
    const char *sql = "SELECT name FROM config_section";
    stmt.prepare(sql);
    while (stmt.step() == SQLITE_ROW)
      list.push_back(stmt.column_text(0));
    stmt.finalize();
    return list;
  }
}

