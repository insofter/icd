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
  CcounterVal cv[ counters_.size() ];

  std::cout << "New aggr: " << ctime( &current.sec ) << std::endl;

  Ctime wait( 0, 200*1000 );//200 ms == 200 000 us == 0,2 s
  Ctime nextTest;
  int nextTestI=0;


  while( 1==1 ) {

    reader_.pollEvents( wait );

    Ctime newtime;
    Ctime nextData;
    nextData.sec+=90;
    Ctime nextLive;
    newtime.usec=0;
    newtime.sec/=period.sec;
    newtime.sec*=period.sec;


    if( nextTest < Ctime() ) {//testing loop TODO: zmienic całkiem testy
      counters_[nextTestI]->test();
      Ctime now;
      std::cout << "Test (" << nextTestI << "): " << ctime( &now.sec ) << std::endl;
      ++nextTestI;
      if( nextTestI >= counters_.size() ) {
        nextTest.sec=newtime.sec+period.sec*(1.1+randVal()*0.8);
        nextTestI=0;
      }
    }//testing loop

    for( int i=0; i< counters_.size(); ++i ) {//reading loop
      if( current < newtime ) {//all records should be closed
        cv[i]=counters_[i]->getCount( Ctime(), RESET );
      } else {
        cv[i]=counters_[i]->getCount( Ctime() );
      }
    }//reading loop

    if( current < newtime ) {//new aggr
      if( writer.liveBeginTransaction() ) {

        if( writer.dataBeginTransaction() ) {
          for( int i=0; i< counters_.size(); ++i ) {
            writer.dataWrite( cv[i].id, current.sec, cv[i].val, cv[i].dark, cv[i].work, cv[i].test, 3 );
            writer.liveWrite( cv[i].id, current.sec, cv[i].val, cv[i].dark, cv[i].work, cv[i].test, 3 );
          }
          writer.dataCloseRecords();
          writer.dataCommitTransaction();
          current=newtime;
        }

        writer.liveCloseRecords();
        writer.liveCommitTransaction();
      }
    } else if( nextData < Ctime() ) {//end new aggr -- data sync
      if( writer.dataBeginTransaction() ) {
        for( int i=0; i< counters_.size(); ++i ) {
          writer.dataWrite( cv[i].id, current.sec, cv[i].val, cv[i].dark, cv[i].work, cv[i].test, 3 );
        }
        writer.dataCommitTransaction();
      }
      nextData.setCurrentTime();
      nextData.sec+=180;//3 min between syncs
    } else if( nextLive < Ctime() ) {//end data sync -- live sync
      if( writer.liveBeginTransaction() ) {
        for( int i=0; i< counters_.size(); ++i ) {
          writer.liveWrite( cv[i].id, current.sec, cv[i].val, cv[i].dark, cv[i].work, cv[i].test, 3 );
        }
        writer.liveCommitTransaction();
      }
      nextLive.sec+=2;
    }//end live sync

  } // while( 1==1 )
}



