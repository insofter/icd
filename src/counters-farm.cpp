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
  if( counter!=NULL ) {
    counter->setReader( &reader_ );
    counters_.push_back( counter );
  }
}

int CcountersFarm::run( Ctime period ) {
  CdbWriter writer;
  Ctime current;

  Ctime wait( 0, 200 );
  int r;
  while( 1==1 ) {

//    std::cout << "LOOP: " << reader_.pollEvents( wait ) << std::endl;
    reader_.pollEvents( wait );

    Ctime newtime;
    newtime.usec=0;
    newtime.sec/=period.sec;
    newtime.sec*=period.sec;


    writer.beginTransaction();

    for( int i=0; i< counters_.size(); ++i ) {
      CcounterVal cv=counters_[i]->getCount( Ctime() );
      writer.write( cv.id, current.sec, cv.val, cv.dark, cv.work, 3 );
    }
    if( current < newtime ) {//all records were closed
      writer.closeRecords();
      current=newtime;
    }

    writer.commitTransaction();
  } // while( 1==1 )



      /*debug*/
    /*std::cout << "CV: v=" << cv.val << " d="
        << cv.dark.sec << "." << cv.dark.usec << " w="
        << cv.work.sec << "." << cv.work.usec << std::endl;

      Ctime per;
      per.sec/=3600;
      per.sec*=3600;

     // writer.write( cv.id, per.sec, cv.val, cv.dark, cv.work, 888 );
    }*/

    /*debug*/
/*    for( int i=0; i<4; ++i ) {
      Cevent ev=reader_.getEvent( i );
      if( ev!=Cevent::EMPTY() ) {
        std::cout << "event list (" << i <<"): " 
          << ev.time.sec << " " << ev.time.usec
          << " " << ev.value << std::endl;
      }
    }*/


    //TODO: check if db is modified and return
}
