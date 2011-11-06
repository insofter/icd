#include <iostream>
#include <vector>
#include <sqlite3.h>

#ifndef __SQLITE3CC_H__
#define __SQLITE3CC_H__

namespace sqlite3cc
{
  class conn
  {
    public:
      conn() : handle(NULL) {}

      ~conn()
      {
        if (handle != NULL)
          close();
      }

      void open(const char *file_name)
      {
        std::cerr << "conn::open: Open db '" << file_name << "'" << std::endl;

        int err = sqlite3_open(file_name, &handle);
        if (err != SQLITE_OK)
          std::cerr << "conn::open: Error! sqlite3_open returned "
            "an error code (" << err << ")" << std::endl;
        else
          std::cerr << "conn::open: handle " << handle << std::endl;
      }

      void close(void)
      {
        std::cerr << "conn::close: handle " << handle << std::endl;

        if (handle == NULL)
          return;

        int err = sqlite3_close(handle);
        if (err != SQLITE_OK)
          std::cerr << "conn::close: Error! sqlite3_close returned "
            "an error code (" << err << ")" << std::endl;

        handle = NULL;
      }

      sqlite3 *get_handle(void) { return handle; }

    private:
      sqlite3 *handle;
  };

  class stmt
  {
    public:
      stmt(conn& db) : db(db), handle(NULL)  {}

      ~stmt()
      {
        if (handle != NULL)
          finalize();
      }

      void prepare(const char *sql)
      {
        int err = sqlite3_prepare_v2(db.get_handle(), sql, -1, &handle, NULL);
        if (err != SQLITE_OK)
          std::cerr << "stmt::prepare: Error! sqlite3_prepare_v2 returned "
            "an error code (" << err << ")" << std::endl;
        else
          std::cerr << "stmt::prepare: handle " << handle << std::endl;
      }

      int step(void)
      {
        int err = sqlite3_step(handle);
        if (err != SQLITE_ROW && err != SQLITE_DONE)
          std::cerr << "stmt::step: Error! sqlite3_step returned "
            "an error code (" << err << ")" << std::endl;

        return err;
      }

      int column_type(int col)
      {
        return sqlite3_column_type(handle, col);
      }

      double column_double(int col)
      {
        return sqlite3_column_double(handle, col);
      }

      int column_int(int col)
      { 
        return sqlite3_column_int(handle, col);
      }

      const char *column_text(int col) 
      { 
        return (const char *)sqlite3_column_text(handle, col);
      }

      void finalize(void)
      {
        std::cerr << "stmt::finalize: handle " << handle << std::endl;

        if (handle == NULL)
          return;

        int err = sqlite3_finalize(handle);
        if (err != SQLITE_OK)
          std::cerr << "stmt::finalize: Error! sqlite3_finalize returned "
            "an error code (" << err << ")" << std::endl;

        handle = NULL;
      }

    private:
      conn& db;
      sqlite3_stmt *handle;
  };

  class row
  {
    public:
      row();
      ~row() {}

      int id;
      int itd_id;
      std::string dtm;
      int cnt;
      int dark_time;
      int work_time;
      std::string test;
      int flags;

      void init(stmt& st);
  };

  row::row() : id(0), itd_id(0), cnt(0),
    dark_time(0), work_time(0), flags(0)
  {
  }

  void row::init(stmt& st)
  {
    id = st.column_int(0);
    itd_id = st.column_int(1);
    dtm = st.column_text(2);
    cnt = st.column_int(3);
    dark_time = st.column_int(4);
    work_time = st.column_int(5);
    test = st.column_text(6);
    flags = st.column_int(7);
  } 

  //template <class T>
  class rowset
  {
  public:
    rowset(conn& db) : db(db) {}
    ~rowset();

    std::vector<row *>& get_rows(void) { return rows; }
    conn& get_db(void);

    void db_select(const char *sql);

  protected:
  
    conn& db;
    std::vector<row *> rows;
  };

  rowset::~rowset()
  {
    while (!rows.empty())
    {
      delete rows.back();
      rows.pop_back();
    }
  }

  void rowset::db_select(const char *sql)
  {
    stmt st(db);
    st.prepare(sql);

    while (st.step() == SQLITE_ROW)
    {
      row *r = new row();
      r->init(st);
      rows.push_back(r);
    }

    st.finalize();
  }

}

#endif // __SQLITE3CC_H__

