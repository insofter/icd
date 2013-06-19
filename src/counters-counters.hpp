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

struct CcounterVal {
  int val;
  int id;
  Ctime dark;
  Ctime work;
  CcounterVal();
  CcounterVal( int val_, Ctime dark_, Ctime work_ );

};

class Ccounter {
  public:
    enum Estate {
      HIGH,
      LOW,
      CONDHIGH, //before engage time
      CONDLOW //before release time
    };
    /**
     * Constructor.
     * @param master Id of master device.
     * @param beginTime Event with current time.
     */
    Ccounter( int id, int master, Ctime beginTime );
    /**
     * Destructor.
     * Kills object to death.
     */
    virtual ~Ccounter();
    /**
     * Sets reader for reading events.
     */
    void setReader( CdevicesReader * reader );
    /**
     * Returns counter value.
     * Will be runned for every 200ms.
     * @param reset If reset is RESET then atomically get value and set counter to 0.
     */
    virtual CcounterVal getCount( const Ctime time, Econstants reset=NO_RESET )=0;
    /**
     * Sets led for turning it on and off.
     */
    void setLed(Cled * led);

  protected:
    CdevicesReader * reader_;
    Cled * led_;
    int id_;
    int masterId_;
    int counter_;
    void ledOn_();
    void ledOff_();
};

class CcounterMono: public Ccounter {
  public:
    /**
     * Constructor.
     * @param master Id of master device.
     * @param beginTime Event with current time.
     * @param engage Time of engage filter.
     * @param release Time of release filter.
     */
    CcounterMono( int id, int master, const Ctime beginTime, const Ctime engage, const Ctime release, Econstants reverse=NORMAL );
    virtual ~CcounterMono();
    /**
     * Returns counter value.
     * Will be runned for every 200ms.
     * @param reset If reset is RESET then atomically get value and set counter to 0.
     */
    virtual CcounterVal getCount( const Ctime time, Econstants reset=NO_RESET );
  protected:
    Ctime engage_;
    Ctime release_;
    Estate state_;
    Cevent last_;
    Ctime dark_;
    Ctime work_;
    Econstants reverse_;


};







#endif // COUNTERS_COUNTERS_HPP
