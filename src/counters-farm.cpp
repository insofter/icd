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
  std::cout << "~CcountersFarm" << std::endl;
  for( int i=0; i< counters_.size(); ++i ) {
    delete counters_[i];
  }
  std::cout << "~CcountersFarm ok" << std::endl;
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
  std::vector< CcounterVal > cv( counters_.size() );

  std::cout << "New aggr: " << ctime( &current.sec ) << std::endl;

  Ctime wait( 0, 200*1000 );//200 ms == 200 000 us == 0,2 s
  Ctime nextTest;
  Ctime nextData;
  nextData.sec+=90;
  Ctime nextLive;

  while( 1==1 ) {// main loop

    reader_.pollEvents( wait );

    Ctime newtime;
    newtime.usec=0;
    newtime.sec/=period.sec;
    newtime.sec*=period.sec;

    if( nextTest < Ctime() ) {//testing loop 
      reader_.testRun();

      for( int i=0; i<counters_.size(); ++i ) {
        counters_[ i ]->test();
      }
      Ctime now;
      std::cout << "Test: " << ctime( &now.sec ) << std::endl;
      nextTest.sec=newtime.sec+period.sec*(1.1+randVal()*0.8);
    }//testing 

    for( int i=0; i< counters_.size(); ++i ) {//reading loop
      if( current < newtime ) {//all records should be closed
        cv[i]=counters_[i]->getCount( Ctime(), RESET );
      } else {
        cv[i]=counters_[i]->getCount( Ctime() );
      }
    }//reading loop

    if( current < newtime ) {//new aggr
//      std::cout << "try newaggr" << std::endl;
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
//      std::cout << "try data write" << std::endl;
      if( writer.dataBeginTransaction() ) {
        for( int i=0; i< counters_.size(); ++i ) {
          writer.dataWrite( cv[i].id, current.sec, cv[i].val, cv[i].dark, cv[i].work, cv[i].test, 3 );
        }
        writer.dataCommitTransaction();
      }
      nextData.sec+=180;//3 min between syncs
    } else if( nextLive < Ctime() ) {//end data sync -- live sync
//      std::cout << "try live write" << std::endl;
      if( writer.liveBeginTransaction() ) {
        for( int i=0; i< counters_.size(); ++i ) {
          writer.liveWrite( cv[i].id, current.sec, cv[i].val, cv[i].dark, cv[i].work, cv[i].test, 3 );
        }
        writer.liveCommitTransaction();
      }
    }//end live sync

    if( nextLive < Ctime() ) {//restart module
      if( unlink( "/tmp/counters-reset" ) == 0 ) {
        if( writer.dataBeginTransaction() ) {
          for( int i=0; i< counters_.size(); ++i ) {
            writer.dataWrite( cv[i].id, current.sec, cv[i].val, cv[i].dark, cv[i].work, cv[i].test, 3 );
          }
          writer.dataCommitTransaction();
        }
        std::cout << "counters-daemon restart" << std::endl;
        break;
      }
      nextLive.sec+=2;
    }

  } // while( 1==1 )
}



