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
    Ccounter( int master, Ctime beginTime );
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
    virtual CcounterVal getCount( const Ctime time, Ereset reset=NO_RESET )=0;

  protected:
    CdevicesReader * reader_;
    int masterId_;
    int counter_;
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
    CcounterMono( int master, const Ctime beginTime, const Ctime engage, const Ctime release );
    virtual ~CcounterMono();
    /**
     * Returns counter value.
     * Will be runned for every 200ms.
     * @param reset If reset is RESET then atomically get value and set counter to 0.
     */
    virtual CcounterVal getCount( const Ctime time, Ereset reset=NO_RESET );
  protected:
    Ctime engage_;
    Ctime release_;
    Estate state_;
    Cevent last_;
    Ctime dark_;
    Ctime work_;

};







#endif // COUNTERS_COUNTERS_HPP
