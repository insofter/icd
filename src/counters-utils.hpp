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
#include "db-config.h"
#include "sqlite3cc.h"

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

  Ctime operator-( const Ctime & b ) const;
  Ctime operator+( const Ctime & b ) const;
  bool operator<( const Ctime & b ) const;
};

struct Cevent {
  Ctime time;
  int value;
  int dev;
  Cevent( int sec_, int usec_, int value_, int dev_=-1);
  Cevent( Ctime time_, int value_, int dev_=-1);
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
class Cdevice {
  public:
    /**
     * Constructor.
     * @param dev Path to file.
     * @param id Id.
     * @param engage Engage time.
     * @param engage Release time.
     */
    Cdevice( std::string dev, int id, Ctime engage, Ctime release );
    /**
     * Returns value of darktime.
     * @param reset If reset is RESET then atomically get value and set time to 0.
     */
    Ctime getDarktime( Ereset reset=NO_RESET );
    /**
     * Returns file descriptor connected with device, to check it with poll().
     */
    int getFd();
    /**
     * Returns event. It's BLOCKING.
     */
    Cevent getEvent();
};

#endif // COUNTERS_UTILS_HPP
