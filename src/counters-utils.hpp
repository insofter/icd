/**
 * @file counters.hpp
 * @author Piotr Mika
 * @date V 2013r.
 */

#ifndef COUNTERS_UTILS_HPP
#define COUNTERS_UTILS_HPP
#include <vector>
#include <string>
#include <sys/time.h>
#include <limits.h>
#ifdef DESKTOP//different headers on arm and desktop,
//use -DDESKTOP durng compilation on desktop 
#include <unistd.h>
#endif
#include <fcntl.h>
#include <poll.h>
#include "db-config.h"
#include "sqlite3cc.h"
#include <map>

extern icd::config *globalConfig;
extern sqlite3cc::conn *globalConfigDb;
extern sqlite3cc::conn *globalLiveDb;
extern sqlite3cc::conn *globalDataDb;

enum Econstants {
  NO_RESET,
  RESET,
  NORMAL,
  REVERSE,
};

enum EeventVal {
  ERROR=INT_MIN,
  LIGHT=1,
  DARK=0,
};

struct Ctime {
  enum constants {
    NUMBER_USECS=1000*1000,
  };
  long sec;
  int usec;
  /**
   * Constructor.
   * With no parameters gives current time.
   */
  Ctime();
  Ctime( int sec_, int usec_=0 );

  void setCurrentTime();
  void setZero();
  void tidy();
  int msec();

  Ctime operator-( const Ctime & b ) const;
  Ctime operator+( const Ctime & b ) const;
  bool operator<( const Ctime & b ) const;
  bool operator==( const Ctime & b ) const;
  bool operator!=( const Ctime & b ) const;

};

struct Cevent {
  Ctime time;
  EeventVal value;
//  int value_old;
  Cevent( int sec_=INT_MIN, int usec_=0, EeventVal value_=ERROR );
  Cevent( Ctime time_, EeventVal value_ );
  Cevent( const char * buf );
  bool operator==( const Cevent & b ) const;
  bool operator!=( const Cevent & b ) const;
  static Cevent EMPTY();

  //TODO temporary solution!
  friend Cevent latest( const Cevent & ev1, const Cevent & ev2 );
  friend Cevent earliest( const Cevent & ev1, const Cevent & ev2 );
};

class Cled {
  public:
    Cled( std::string dev );
    ~Cled();
    void on();
    void off();
  protected:
    int fd_;
};

/**
 * Class for writing to db
 */
class CdbWriter {
  public:
    /**
     * Constructor.
     * Uses global db access.
     */
    CdbWriter();
    ~CdbWriter();

    /**
     * Begins transaction.
     * Used for faster adding events.
     */
    bool liveBeginTransaction();
    bool dataBeginTransaction();
    /**
     * Commits transaction.
     */
    void liveCommitTransaction();
    void dataCommitTransaction();

    /**
     * Writes record to db.
     */
    void liveWrite( int counterId, Ctime dtm, int cnt, Ctime dark,
        Ctime work, int test, int flags );
    void dataWrite( int counterId, Ctime dtm, int cnt, Ctime dark,
        Ctime work, int test, int flags );
    /**
     * Makes new aggregation period.
     */
    void liveCloseRecords();
    void dataCloseRecords();
  protected:
    sqlite3cc::stmt * dataInsert_;
    sqlite3cc::stmt * dataUpdate_;
    sqlite3cc::stmt * dataSelect_;
    sqlite3cc::stmt * dataClose_;
    sqlite3cc::stmt * dataBegin_;
    sqlite3cc::stmt * dataCommit_;

    sqlite3cc::stmt * liveInsert_;
    sqlite3cc::stmt * liveUpdate_;
    sqlite3cc::stmt * liveSelect_;
    sqlite3cc::stmt * liveClose_;
    sqlite3cc::stmt * liveDelete_;
    sqlite3cc::stmt * liveBegin_;
    sqlite3cc::stmt * liveCommit_;
};

/**
 * Class for reading device and filtering events.
 */
class CdevicesReader {
  public:
    CdevicesReader();
    ~CdevicesReader();
    /**
     * Returns -1 for error.
     * @param dev iPath to file to be readed.
     * @param id Unique id of device.
     */
    int addDevice( std::string dev, int id );
    /**
     * Poll for events.
     * @param wait Waiting time.
     * @return Number of readed events.
     */
    int pollEvents( Ctime wait );
    /**
     * @param devId Id of device.
     * Returns event. 
     */
    const Cevent getEvent( int devId, Econstants reverse=NORMAL );

  private:
    char buf_[32];//10=unixtime + 1=space + 7usecs + 1=space + 1=event + 1=\n ( + 3=multiplexer )
    struct pollfd * pollfd_;
    std::map< /*id*/int, Cevent > events_;
    std::map< /*fd*/int, /*id*/int > devices_;

};

#endif // COUNTERS_UTILS_HPP
