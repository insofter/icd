#include "counters-farm.hpp"
#include <iostream>
#ifdef DESKTOP//different headers on arm and desktop,
//use -DDESKTOP durng compilation on desktop 
#include <unistd.h>
#endif


float randVal() {
  unsigned int rand;
  int rfile = open("/dev/urandom", O_RDONLY);
  read(rfile, (void*) &rand, sizeof( int ) );
  close( rfile );

  return (float)rand/(float)UINT_MAX;
}



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

  std::cout << "New aggr: " << ctime( &current.sec ) << std::endl;

  Ctime wait( 0, 200*1000 );//200 ms == 200 000 us == 0,2 s
  int lazyCounter=0;
  bool dbWrite=false;
  Ctime nextTest;
  int nextTestI=0;


  while( 1==1 ) {
    ++lazyCounter;

    reader_.pollEvents( wait );

    Ctime newtime;
    newtime.usec=0;
    newtime.sec/=period.sec;
    newtime.sec*=period.sec;


    if( lazyCounter > 15 || current < newtime ) {
      if( nextTest < Ctime() ) {
        counters_[nextTestI]->test();
        Ctime now;
        std::cout << "Test (" << nextTestI << "): " << ctime( &now.sec ) << std::endl;
        ++nextTestI;
        if( nextTestI >= counters_.size() ) {
          nextTest.sec=newtime.sec+period.sec*(1.1+randVal()*0.8);
          nextTestI=0;
        }
      }
      if( writer.beginTransaction() ) {
        dbWrite=true;
        lazyCounter=0;
      } else {
        lazyCounter=5;
        std::cout << "Db locked :( " << std::endl;
      }
    }

    for( int i=0; i< counters_.size(); ++i ) {
      CcounterVal cv;
      if( dbWrite ) {
        if( current < newtime ) {//all records should be closed
          cv=counters_[i]->getCount( Ctime(), RESET );
        } else {
          cv=counters_[i]->getCount( Ctime() );
        }
        writer.write( cv.id, current.sec, cv.val, cv.dark, cv.work, cv.test, 3 );
      } else {
        cv=counters_[i]->getCount( Ctime() );
      }
    }

    if( dbWrite && current < newtime ) {//all records should be closed
      writer.closeRecords();
      current=newtime;
      std::cout << "New aggr: " << ctime( &current.sec ) << std::endl;
    }

    if( dbWrite ) {
      writer.commitTransaction();
      dbWrite=false;
    }

    //TODO: check if db is modified and return
    

  } // while( 1==1 )
}



