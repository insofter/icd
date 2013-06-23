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


class CcounterThick: public Ccounter {
  public:

    /*!
     * constructor
     * \param [in] id -- counter id
     * \param [in] master -- master device id
     * \param [in] slave -- slave device id
     */
    CcounterThick( 
        int id, int master, int slave,
        const Ctime beginTime,
        const Ctime engage_master, const Ctime engage_slave,
        const Ctime release_master,  const Ctime release_slave,
        Econstants reverse_master=NORMAL, Econstants reverse_slave=NORMAL
        );

    /*! 
     * destructor
     */
    virtual ~CcounterThick();

    /*! 
     * \brief returns counter value
     * \param [in] time -- time to check for events?
     * \param [in] reset -- if set, returns value and set counter to 0
     */
    virtual CcounterVal getCount( const Ctime time, Econstants reset=NO_RESET );

  protected:
    int slaveId_;

    Ctime begin_time_;
    Ctime begin_work_;
    Ctime engage_master_;
    Ctime engage_slave_;
    Ctime release_master_;
    Ctime release_slave_;

    Estate state_;
    Estate state_master_;
    Estate state_slave_;

    Cevent last_slave_;
    Cevent last_master_;

    Ctime dark_;
    Ctime work_;

    Econstants reverse_master_;
    Econstants reverse_slave_;
};



#endif // COUNTERS_COUNTERS_HPP
