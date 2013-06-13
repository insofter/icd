#include "counters-farm.hpp"
#include <iostream>


int CcountersFarm::addDevice( std::string dev, int id ) {
  return reader_.addDevice( dev, id );
}

void CcountersFarm::addCounter( Ccounter* counter ) {

}

int CcountersFarm::run() {
  Ctime wait( 5, 0 );
  int r;
  while( 1==1 ) {

    std::cout << "LOOP: " << reader_.pollEvents( wait ) << std::endl;

    for( int i=0; i<4; ++i ) {
      Cevent ev=reader_.getEvent( i );

      /*debug*/std::cout << "event list (" << i <<"): " << ev.time.sec << " " << ev.time.usec
        << " " << ev.value << std::endl;
    }
  }
}
