#include "db-config.h"

namespace icd
{
	std::string config::entry(const std::string& section, const std::string& key)
	{
		std::string value;
		sqlite3cc::stmt stmt(db);
		const char *sql = "SELECT c.value "
			"FROM config c JOIN config_section s ON c.section_id == s.id "
			"WHERE s.name == ?1 AND c.key == ?2";
		stmt.prepare(sql);
		stmt.bind_text(1, section.c_str());
		stmt.bind_text(2, key.c_str());
		stmt.step();
		if (stmt.column_type(0) != SQLITE_NULL)
			value = stmt.column_text(0);
		stmt.finalize();
		return value;
	}

	bool config::entry_bool(const std::string& section, const std::string& key,
		bool empty_ok, bool def_val)
	{
		std::string value = entry(section, key);
		if (value.empty())
		{
			if (empty_ok)
				return def_val;
			else
			{
				std::ostringstream oss;
				oss << "Reading db config (" << section << ":" << key << ") failed."
				 " Empty entry is not allowed.";
				throw std::runtime_error(oss.str());
			}
		}
		 
		if (value == "1" || value == "yes" || value == "true")
			return true;
		else if (value == "0" || value == "no" || value == "false") 
			return false;
		else
		{
			std::ostringstream oss;
			oss << "Reading db config boolean '" << value << "' (" << section << ":" << key << ") failed."
				" Expected one of the values: 1, yes, true, 0, no, false.";
			throw std::runtime_error(oss.str());
		}
	}

	long config::entry_long(const std::string& section, const std::string& key,
		bool empty_ok, long def_val)
	{
		std::string value = entry(section, key);
		if (value.empty())
		{
			if (empty_ok)
				return def_val;
			else
			{
				std::ostringstream oss;
				oss << "Reading db config (" << section << ":" << key << ") failed."
					" Empty entry is not allowed.";
				throw std::runtime_error(oss.str());
			}
		}

		std::istringstream iss(value);
		long value_long;
		iss >> value_long;
		if (!iss.eof())
		{
			std::ostringstream oss;
			oss << "Reading db config long '" << value << "' (" << section << ":" << key << ") failed."
				" Invalud long integer value.";
			throw std::runtime_error(oss.str());
		}

		return value_long;
	}

	void config::set_entry(const std::string& section, const std::string& key, const std::string& value)
	{
		std::string dbvalue;
		sqlite3cc::stmt stmt(db);
		const char *sql = "SELECT c.value "
			"FROM config c JOIN config_section s ON c.section_id == s.id "
			"WHERE s.name == ?1 AND c.key == ?2";
		stmt.prepare(sql);
		stmt.bind_text(1, section.c_str());
		stmt.bind_text(2, key.c_str());
		stmt.step();
		if (stmt.column_type(0) != SQLITE_NULL) {
			dbvalue = stmt.column_text(0);
			if( dbvalue==value )
			{
				stmt.finalize();
				return;
			}
		}
		stmt.finalize();

		const char *sql2 = "INSERT OR REPLACE INTO config (section_id, key, value) "
			"VALUES ( (SELECT s.id FROM config_section s WHERE s.name = ?1), ?2, ?3)";
		stmt.prepare(sql2);
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

	std::vector<std::string> config::list_like(const std::string &name)
	{
		std::vector<std::string> list;
		sqlite3cc::stmt stmt(db);
    std::string like="%";
    like+=name;
    like+="%";
		const char *sql = "SELECT name FROM config_section WHERE name LIKE ?1";
		stmt.prepare(sql);
    stmt.bind_text(1, like.c_str());
		while (stmt.step() == SQLITE_ROW)
			list.push_back(stmt.column_text(0));
		stmt.finalize();
		return list;
	}
}

