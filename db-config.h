#include <string>
#include "sqlite3cc.h"

#ifndef __DB_CONFIG_H__
#define __DB_CONFIG_H__

namespace icd
{
  struct config_entry
  {  
    std::string section;
    std::string key;
    std::string value;
  };

  class config
  {
    public:

      config(sqlite3cc::conn& db) : db(db) {}
      ~config() {}

      std::string entry(const std::string& section, const std::string& key);
      void set_entry(const std::string& section, const std::string& key, const std::string& value);
      void remove_entry(const std::string& section, const std::string& key);
      std::vector<config_entry> list_entries(const std::string& section);
      std::vector<config_entry> list_entries();

      void add_section(const std::string& name);
      void remove_section(const std::string& name);
      std::vector<std::string> list_sections();

    private:
      sqlite3cc::conn& db;
  };

}

#endif // __DB_CONFIG_H__
