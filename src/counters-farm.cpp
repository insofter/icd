#include "counters-farm.hpp"
#include <iostream>

CcountersFarm::CcountersFarm() {
  0; //NOOP
}


CcountersFarm::~CcountersFarm() {
  for( int i=0; i< counters_.size(); ++i ) {
    delete [] counters_[i];
  }
}


int CcountersFarm::addDevice( std::string dev, int id ) {
  return reader_.addDevice( dev, id );
}

void CcountersFarm::addCounter( Ccounter* counter ) {
  counter->setReader( &reader_ );
  counters_.push_back( counter );
}

int CcountersFarm::run() {
  Ctime wait( 5, 0 );
  int r;
  while( 1==1 ) {

    std::cout << "LOOP: " << reader_.pollEvents( wait ) << std::endl;

    for( int i=0; i< counters_.size(); ++i ) {
      std::cout << "for" << std::endl;

      CcounterVal cv=counters_[i]->getCount( Ctime() );
      /*debug*/std::cout << "CV: v=" << cv.val << " d="
        << cv.dark.sec << "." << cv.dark.usec << " w="
        << cv.work.sec << "." << cv.work.usec << std::endl;
    }

    /*debug*/
/*    for( int i=0; i<4; ++i ) {
      Cevent ev=reader_.getEvent( i );
      if( ev!=Cevent::EMPTY() ) {
        std::cout << "event list (" << i <<"): " 
          << ev.time.sec << " " << ev.time.usec
          << " " << ev.value << std::endl;
      }
    }*/

  }// while( 1==1 )
}
