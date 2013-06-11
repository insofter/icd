/**
 * @file counters.hpp
 * @author Piotr Mika
 * @date V 2013r.
 */

#ifndef COUNTERS_HPP
#define COUNTERS_HPP
#include <vector>
#include <string>
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
  Ctime(int sec_=0, int usec_=0);

  Cevent operator-( const Cevent & b ) const;
  Cevent operator+( const Cevent & b ) const;
  Cevent operator<( const Cevent & b ) const;
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
  void write( int counterId, Ctime dtm, int cnt, Ctime dark
      Ctime work, int test );
  /**
   * Makes new aggregation period.
   */
  void newAggr();
};
/**
 * Class for counting and parsing mono and multi-fotodetector counters.
 */
class Ccounter {
  public:
    /** 
     * Constructor.
     * @param master Id of master device.
     * @param db Database writer for setting values in db.
     * @param time Event with current time.
     */
    Ccounter( int master, Ctime current );
    /**
     * Returns id of master device.
     */
    int getMasterId()=0;
    /**
     * Returns id of slave device or -1 if unnecessary.
     */
    int getSlaveId();
    /**
     * Returns id of stoping device or -1 if unnecessary.
     */
    int getStopId();
    /**
     * Returns counter value.
     * @param reset If reset is RESET then atomically get value and set counter to 0.
     */
    int getCount( Ereset reset=NO_RESET );
    /**
     * Returns time of work since last reset.
     */
    Ctime getWorkTime();
    /**
     * Writes master event TO counter.
     * @param event Event.
     */
    void writeMasterEvent( Cevent event );
    /**
     * Writes slave event TO counter.
     * @param event Event.
     */
    void writeSlaveEvent( Cevent event )=0;
    /**
     * Writes stop event TO counter.
     * @param event Event.
     */
    void writeStopEvent( Cevent event )=0;
    /**
     * Changes worktime and darktime, should be runned approx. for every 200ms.
     * @param time Event with current time.
     */
    void noChangeEvent( Ctime current )=0;
    /**
     * Counts new values from events.
     * Should be runned by write***Event() functions.
     */
    void reCount()=0;

  protected:
    int masterId_;
    Cevent masterA;
    Cevent masterB;
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

/**
 * Class for reading devices and running counters.
 */
class CcountersFarm {
  public:
    /**
     * Add device to be readed.
     * @param dev Device.
     * @return 1 for correct, 0 for error, ex. Non-unique identifier.
     */
    int addDevice( Cdevice * dev );
    /**
     * Add counter to farm.
     * @param counter Valid pointer to counter.
     */
    void addCounter( Ccounter * counter );
    int run();

};


#endif // COUNTERS_HPP
