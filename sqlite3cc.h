#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <sqlite3.h>

#ifndef __SQLITE3CC_H__
#define __SQLITE3CC_H__

namespace sqlite3cc
{
  class sqlite3cc_error : public std::runtime_error
  {
    public:
      explicit sqlite3cc_error(sqlite3 *handle, const std::string& what)
        : std::runtime_error(std::string())
      {
        std::ostringstream oss;
        oss << what << " (errcode " << sqlite3_errcode(handle) << ", '"
          << sqlite3_errmsg(handle) << "')";
        what_ = oss.str();
      }
      virtual ~sqlite3cc_error() throw() {}

      virtual const char* what() const throw()
      {
        return what_.c_str();
      }

    private:
      std::string what_;
  };

  typedef int (*exec_cbf_t)(void *, int, char **, char **);

  class conn
  {
    public:
      conn() : handle_(NULL), exec_cbf(NULL), exec_cbf_data(NULL) {}

      ~conn()
      {
        if (handle_ != NULL)
          close();
      }

      void open(const char *file_name)
      {
        int err = sqlite3_open(file_name, &handle_);
        if (err != SQLITE_OK)
        {
          std::ostringstream oss;
          oss <<  "Opening db file '" << file_name << "' failed";
          throw sqlite3cc_error(handle_, oss.str());
        }
      }

      void close(void)
      {
        if (handle_ != NULL)
        {
          int err = sqlite3_close(handle_);
          if (err != SQLITE_OK)
          {
             std::ostringstream oss;
             oss <<  "Closing db failed"
               << " (errcode " << sqlite3_errcode(handle_) << ", '"
               << sqlite3_errmsg(handle_) << "')";
             std::cerr << oss.str();
          }
          handle_ = NULL;
        }
      }

      void set_exec_cbf(exec_cbf_t cbf, void* data) 
      {
        exec_cbf = cbf;
        exec_cbf_data = data;
      }

      void exec(const char *sql)
      {
        int err = sqlite3_exec(handle_, sql, exec_cbf, exec_cbf_data, NULL);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(handle_, "Executing SQL failed");
      }

      void busy_timeout(int ms)
      {
        int err = sqlite3_busy_timeout(handle_, ms);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(handle_, "Setting busy_timeout failed");
      }

      sqlite3 *handle(void) const { return handle_; }

    private:
      sqlite3 *handle_;
      exec_cbf_t exec_cbf;
      void *exec_cbf_data;
  };

  class stmt
  {
    public:
      stmt(conn& db) : db(db), handle_(NULL)  {}

      ~stmt()
      {
        if (handle_ != NULL)
          finalize();
      }

      void prepare(const char *sql)
      {
        int err = sqlite3_prepare_v2(db.handle(), sql, -1, &handle_, NULL);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(db.handle(), "Preparing SQL failed");
      }

      int step()
      {
        int err = sqlite3_step(handle_);
        if (err != SQLITE_ROW && err != SQLITE_DONE)
          throw sqlite3cc_error(db.handle(), "Stmt step failed");
        return err;
      }

      void reset()
      {
        int err = sqlite3_reset(handle_);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(db.handle(), "Stmt reset failed");
      }

      int column_type(int col)
      {
        return sqlite3_column_type(handle_, col);
      }

      double column_double(int col)
      {
        return sqlite3_column_double(handle_, col);
      }

      int column_int(int col)
      { 
        return sqlite3_column_int(handle_, col);
      }

      const char *column_text(int col) 
      { 
        return (const char *)sqlite3_column_text(handle_, col);
      }

      void bind_null(int index)
      {
        int err = sqlite3_bind_null(handle_, index);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(db.handle(), "Stmt bind_null failed");
      }

      void bind_int(int index, int value)
      {
        int err = sqlite3_bind_int(handle_, index, value);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(db.handle(), "Stmt bind_int failed");
      }

      void bind_int64(int index, long long int value)
      {
        int err = sqlite3_bind_int64(handle_, index, value);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(db.handle(), "Stmt bind_int64 failed");
      }

      void bind_text(int index, const std::string& value)
      {
        int err = sqlite3_bind_text(handle_, index, value.c_str(), -1, SQLITE_TRANSIENT);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(db.handle(), "Stmt bind_text failed");
      } 

      void clear_bindings()
      {
        int err = sqlite3_clear_bindings(handle_);
        if (err != SQLITE_OK)
          throw sqlite3cc_error(db.handle(), "Stmt clear_bindings failed");
      }

      void finalize(void)
      {
        if (handle_ != NULL)
        {
          int err = sqlite3_finalize(handle_);
          if (err != SQLITE_OK)
          {
            std::ostringstream oss;
            oss <<  "Stmt finalize failed"
               << " (errcode " << sqlite3_errcode(db.handle()) << ", '"
               << sqlite3_errmsg(db.handle()) << "')";
             std::cerr << oss.str();
          }
          handle_ = NULL;
        }
      }

    private:
      conn& db;
      sqlite3_stmt *handle_;
  };

  template <class T>
  class rowset
  {
  public:
    rowset(conn& db) : db(db) {}
    ~rowset();

    std::vector<T *>& get_rows(void) { return rows; }
    conn& get_db(void);

    void db_select(const char *sql);

  protected:
  
    conn& db;
    std::vector<T *> rows;
  };

  template <class T>
  rowset<T>::~rowset()
  {
    while (!rows.empty())
    {
      delete rows.back();
      rows.pop_back();
    }
  }

  template <class T>
  void rowset<T>::db_select(const char *sql)
  {
    stmt st(db);
    st.prepare(sql);

    while (st.step() == SQLITE_ROW)
    {
      T *r = new T(st);
      rows.push_back(r);
    }

    st.finalize();
  }

}

#endif // __SQLITE3CC_H__

