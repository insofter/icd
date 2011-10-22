#include <stdio.h>
#include <signal.h>
#include <sqlite3.h>

volatile sig_atomic_t sigint = 0;

void sigint_handler(int signum)
{
  printf("sigint invoked\n");
  sigint = 1;
}

int main(int argc, char *argv[])
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

