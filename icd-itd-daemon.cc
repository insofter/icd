//#include <stdio.h>
//#include <signal.h>


#include <iostream>
#include <sstream>
#include <iomanip>
#include <string> 
#include <cstdlib>
#include <time.h>
#include <sys/time.h>

#include "sqlite3cc.h"

namespace icd
{

  enum itd_state 
  {
    UNINITIALIZED = -1,
    CLEAR = 0,
    BLOCKED = 1,
  };

  class itd_time
  {
    public:
      itd_time();
      itd_time(long sec, long usec) : _sec(sec), _usec(usec) {}
      itd_time(const itd_time& ref) : _sec(ref._sec), _usec(ref._usec) {}

      ~itd_time() {}

      itd_time& operator =(const itd_time& ref)
      {
        _sec = ref._sec;
        _usec = ref._usec;
        return *this; 
      }

      long sec() { return _sec; }
      long usec() { return _usec; }

      friend std::ostream& operator <<(std::ostream &os, const itd_time &ref);

    private:
      long _sec;
      long _usec;
  };

  itd_time::itd_time()
  {
    struct timeval tv;
    /*int res =*/ gettimeofday(&tv, NULL);
    //if (res != 0) trow exception;

    _sec = tv.tv_sec;
    _usec = tv.tv_usec;
  }

  std::ostream& operator <<(std::ostream &os,const itd_time &ref)
  {
    char s[32];
    struct tm result;
    struct tm *m = gmtime_r((const time_t *)&ref._sec, &result);
    //if (m == NULL) throw
  
    strftime(s, sizeof(s), "%Y-%m-%dT%H:%M:%S", m);

    os << s << "." << std::setw(3) << std::setfill('0') << ref._usec / 1000;

//    os << "[idt_time:_sec=" << ref._sec << ";_usec=" << ref._usec << "]";
    return os;
  }

  class itd_event
  {
    public:
      itd_event() : _state(UNINITIALIZED) {}
      itd_event(const itd_state& state) : _state(state) {}
      itd_event(const itd_state& state, const itd_time& dtm)
        : _state(state), _dtm(dtm) {}
      itd_event(const itd_event& ref)
        : _state(ref._state), _dtm(ref._dtm) {}

      ~itd_event() {}

      itd_event& operator =(const itd_event& ref)
      {
        _state = ref._state;
        _dtm = ref._dtm;
        return *this;
      }

      itd_state state() { return _state; }
      itd_time dtm() { return _dtm; }

    private:
      itd_state _state;
      itd_time _dtm;
  };

  class itd_base_device
  {
    public:
      virtual itd_event next() = 0;
      virtual void cleanup() {}
  };

/*  class itd_device : public itd_base_device
  {
  };
*/

  class itd_rand_emulator : public itd_base_device
  {
    public:
      itd_rand_emulator();

      virtual itd_event next();

    private:
      itd_state _state;

      int msleep(unsigned long msec);
  };

  itd_rand_emulator::itd_rand_emulator() : _state(UNINITIALIZED)
  {
    srand(time(NULL));
  }

  itd_event itd_rand_emulator::next()
  {
    // Wait between 0-30sec when clear
    // and 0-3sec when blocked or uninitialzed
    long mod = (_state == CLEAR) ? 30000 : 3000;
    long period = rand() % mod;

    msleep(period);

    _state = (_state == CLEAR) ? BLOCKED : CLEAR;
    return itd_event(_state);
  }

  int itd_rand_emulator::msleep(unsigned long milisec)
  {
    struct timespec req={0};
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    while(nanosleep(&req,&req)==-1)
         continue;
    return 1;
  }

  class itd_kbd_emulator : public itd_base_device
  {
    public:
      itd_kbd_emulator() : _state(UNINITIALIZED) {}
     
      virtual itd_event next();

    private:
      itd_state _state;
  };

  itd_event itd_kbd_emulator::next()
  {
    std::string s;
    std::getline(std::cin, s);
    _state = (_state == CLEAR) ? BLOCKED : CLEAR;
    return itd_event(_state);
  }

}

/*volatile sig_atomic_t sigint = 0;

void sigint_handler(int signum)
{
  printf("sigint invoked\n");
  sigint = 1;
}*/

int main(int argc, char *argv[])
{
//  icd::itd_kbd_emulator dev;
  icd::itd_rand_emulator dev;

  sqlite3cc::conn db;
  db.open("live.db");

  sqlite3cc::stmt stmt(db);
  const char *sql = "INSERT INTO events (itd, dtmms, state) VALUES (?1, ?2, ?3)";
  stmt.prepare(sql);

  while(1)
  {
    icd::itd_event e = dev.next();
    std::ostringstream oss;
    oss << e.dtm();

    std::cout << e.state() << ";" <<  e.dtm() << std::endl;

    stmt.bind_text(1, "itd0");
    stmt.bind_int64(2, (long long)e.dtm().sec() * 1000 + (long long)e.dtm().usec() / 1000);
    stmt.bind_int(3, e.state());

    stmt.step();

    stmt.reset();
    stmt.clear_bindings();
  }

  stmt.finalize();

  db.close();

  dev.cleanup();

  return 0;
}

/*int main_a(int argc, char *argv[])
{
  int err = 0;
  FILE *f = NULL;
  char *buf = NULL;
  size_t buf_len = 0;
  int len = 0;
  struct sigaction action;
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  const char *sql = "insert into tbl values ( ?1, ?2 )"; 

  printf("Hello world!\n");

  action.sa_handler = sigint_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  err = sigaction(SIGINT, &action, NULL);
  if (err != 0)
  {
    printf("sigaction failed\n");
    return 1;
  }

  // open /dev/photocell0 device
  f = fopen("/dev/photocell0", "r");
  if (f == NULL)
  {
    // check err properly
    printf("Failed to open /dev/photocell0\n");
    return 1;
  }

  // open db
  err = sqlite3_open("test.db", &db);
  if (err != SQLITE_OK)
  {
    printf("Failed to open test.db database\n");
    return 1;
  }

  err = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (err != SQLITE_OK)
    printf("sqlite_prepare_v2 failed\n");

  while(!sigint)
  {
     // read line
    len = getline(&buf, &buf_len, f);
      printf("getline returned len=%i\n", len);
    if (len <= 0)
    {
      printf("getline error\n");
      continue;
    }

    // parse the line -> event date and time plus duration

    printf("%s", buf);

    // insert the event into sqlite database table
    err = sqlite3_bind_text(stmt, 1, buf, -1, SQLITE_STATIC);
    if (err != SQLITE_OK)
      printf("sqlite_bind_text failed\n");

    err = sqlite3_bind_text(stmt, 2, "TestTest", -1, SQLITE_STATIC);
    if (err != SQLITE_OK)
      printf("sqlite_bind_text failed\n");

    err = sqlite3_step(stmt);
    if (err != SQLITE_DONE)
      printf("sqlite_step failed (err=%i)\n", err);

    err = sqlite3_reset(stmt);
    if (err != SQLITE_OK)
      printf("sqlite_reset failed\n");
  
    err = sqlite3_clear_bindings(stmt);
    if (err != SQLITE_OK)
      printf("sqlite_clear_bindings failed\n");
  }

  err = sqlite3_finalize(stmt);
  if (err != SQLITE_OK)
    printf("sqlite_finalize failed\n");

  sqlite3_close(db);
  // check err

  // close /dev/photocell0 device
  fclose(f);
  // check err of fclose

  return err;
}
*/
