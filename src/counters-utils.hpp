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

enum Ereset {
  NO_RESET,
  RESET,
};


struct Ctime {
  enum constants {
    NUMBER_USECS=1000*1000,
  };
  int sec;
  int usec;
  /**
   * Constructor.
   * With no parameters gives current time.
   */
  Ctime();
  Ctime(int sec_, int usec_=0);

  void setCurrentTime();
  void tidy();
  int msec();

  Ctime operator-( const Ctime & b ) const;
  Ctime operator+( const Ctime & b ) const;
  bool operator<( const Ctime & b ) const;
  bool operator==( const Ctime & b ) const;

};

struct Cevent {
  Ctime time;
  int value;
  Cevent( int sec_=INT_MIN, int usec_=0, int value_=INT_MIN );
  Cevent( Ctime time_, int value_ );
  Cevent( const char * buf );
  bool operator==( const Cevent & b ) const;
  static Cevent EMPTY();
};

/**
 * Class for writing to db
 */
class CdbWriter {
  /**
   * Constructor.
   * Uses global db access.
   */
  CdbWriter();
  /**
   * Writes record to db.
   */
  void write( int counterId, Ctime dtm, int cnt, Ctime dark,
      Ctime work, int test );
  /**
   * Makes new aggregation period.
   */
  void newAggr();
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
    const Cevent getEvent( int devId );

  private:
    char buf_[32];//10=unixtime + 1=space + 7usecs + 1=space + 1=event + 1=\n ( + 3=multiplexer )
    struct pollfd * pollfd_;
    std::map< /*id*/int, Cevent > events_;
    std::map< /*fd*/int, /*id*/int > devices_;

};

#endif // COUNTERS_UTILS_HPP
