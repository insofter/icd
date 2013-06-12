/**
 * @file counters.hpp
 * @author Piotr Mika
 * @date V 2013r.
 */

#ifndef COUNTERS_COUNTERS_HPP
#define COUNTERS_COUNTERS_HPP
#include <vector>
#include <string>
#include <sys/time.h>
#include <limits.h>
#include "db-config.h"
#include "sqlite3cc.h"
#include "counters-utils.hpp"

extern icd::config *globalConfig;
extern sqlite3cc::conn *globalConfigDb;
extern sqlite3cc::conn *globalLiveDb;

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
    virtual int getMasterId()=0;
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
    virtual void writeSlaveEvent( Cevent event )=0;
    /**
     * Writes stop event TO counter.
     * @param event Event.
     */
    virtual void writeStopEvent( Cevent event )=0;
    /**
     * Changes worktime and darktime, should be runned approx. for every 200ms.
     * @param time Event with current time.
     */
    virtual void noChangeEvent( Ctime current )=0;
    /**
     * Counts new values from events.
     * Should be runned by write***Event() functions.
     */
    virtual void reCount()=0;

  protected:
    int masterId_;
};

#endif // COUNTERS_COUNTERS_HPP
