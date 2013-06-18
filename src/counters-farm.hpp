/**
 * @file counters.hpp
 * @author Piotr Mika
 * @date V 2013r.
 */

#ifndef COUNTERS_FARM_HPP
#define COUNTERS_FARM_HPP
#include <vector>
#include <string>
#include <sys/time.h>
#include <limits.h>
#include "db-config.h"
#include "sqlite3cc.h"
#include "counters-utils.hpp"
#include "counters-counters.hpp"

extern icd::config *globalConfig;
extern sqlite3cc::conn *globalConfigDb;
extern sqlite3cc::conn *globalLiveDb;


/**
 * Class for reading devices and running counters.
 */
class CcountersFarm {
  public:
    CcountersFarm();
    ~CcountersFarm();
    /**
     * Add device to be readed.
     * @param dev Device.
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
    CdevicesReader reader_;
    std::vector< Ccounter * > counters_;


};


#endif // COUNTERS_FARM_HPP
