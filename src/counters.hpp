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

struct Cevent {
  int sec;
  int usec;
  int value;
  Cevent(int sec_, int usec_, int value_);
};

class CdbWriter {
  /**
   * Constructor.
   * Uses global db access.
   */
  CdbWriter();
  /**
   * Writes record to db.
   */
  void write( int counterId, int dtm, int cnt, int dark
      int work, int test );
  /**
   * Makes new aggregation period.
   */
  void newAggr();
};

class Ccounter {
  public:
    /** 
     * Constructor.
     * @param master Id of master device.
     * @param db Database writer for setting values in db.
     * @param time Event with current time.
     */
    Ccounter( int master, CdbWriter * db, Cevent currentTime );
    /**
     * Returns id of master device.
     */
    int getMaster()=0;
    /**
     * Returns id of slave device or -1 if unnecessary.
     */
    int getSlave();
    /**
     * Returns id of stoping device or -1 if unnecessary.
     */
    int getStop();
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
    void noChangeEvent( Cevent event )=0;
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
 * Class for reading devices and running counters.
 */
class CcountersFarm {
  public:
    /**
     * Add device to be readed.
     * @param dev Device file.
     * @param id Unique identifier of device.
     * @return 1 for correct, 0 for error, ex. Non-unique identifier.
     */
    int addDevice( std::string dev, int id );
    /**
     * Add counter to farm.
     * @param counter Valid pointer to counter.
     */
    void addCounter( Ccounter * counter );
    int run();

  private:
    int poll();


};


#endif // COUNTERS_HPP
