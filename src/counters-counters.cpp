#include "counters-counters.hpp"
#include <iostream>

CcounterVal::CcounterVal(): val(0) {
  0; //NOOP
}

CcounterVal::CcounterVal( int val_, Ctime dark_, Ctime work_ ) :val(val_), dark(dark_), work(work_) {
  0; //NOOP
}


Ccounter::Ccounter( int master, Ctime beginTime ): 
  reader_(NULL), counter_(0), masterId_(master)
{ 0; //NOOP
}

Ccounter::~Ccounter() {
  0; //NOOP
}

void Ccounter::setReader( CdevicesReader* reader ) {
  reader_=reader;
}


CcounterMono::CcounterMono( int master, const Ctime beginTime, const Ctime engage, const Ctime release ): 
  Ccounter( master, beginTime ), engage_(engage), release_(release), state_(LOW), 
  last_( beginTime, 0 ), dark_(0, 0), work_(0, 0)
{ 0; //NOOP
}

CcounterMono::~CcounterMono() {
}

CcounterVal CcounterMono::getCount( const Ctime time, Ereset reset ) {
  CcounterVal ret;

  Cevent ev=reader_->getEvent( masterId_ );
  if( ev == Cevent::EMPTY() ) {
    ev=last_;
  }

  if( ev == last_ ) {//no change, only clock is ticking...

    if( state_==CONDHIGH ) {
      if( ev.time + engage_ < time ) {//change to high and ++counter
        state_=HIGH;
        ++counter_;
      }
    } else if( state_==CONDLOW ) {
      if( ev.time + release_ < time ) {//change to low and count darktime
        state_=LOW;
      }
    }
    // LOW and HIGH with no change == no effect

  } else {//new event
    if( ev.value!=last_.value ) {//duplicated are ignored (ex. high to high)
      if( ev.value==0 ) {//change to CONDLOW

        if( state_==CONDHIGH ) {
          if( last_.time + engage_ < ev.time ) {//time passed and count is correct before event
            state_=HIGH;
            ++counter_;
          }
        } 
        //count times and change state
        dark_=dark_ + ev.time - last_.time;
        work_=work_ + ev.time - last_.time;
        state_=CONDLOW;

      } else {//change to CONDHIGH

        if( state_==CONDLOW ) {
          if( last_.time + release_ < ev.time ) {//time passed and count is correct before event
            state_=LOW;
          }
        }
        //count times and change state
        work_=work_ + ev.time - last_.time;
        state_=CONDHIGH;
      }
      last_=ev;
    }
  }

  ret.val=counter_;
  ret.work=work_ + time - ev.time;
  if( state_==CONDHIGH || state_==HIGH ) {// darktime is ticking
    ret.dark=dark_ + time - ev.time;
  } else {
    ret.dark=dark_;
  }

  if( reset==RESET ) {
    dark_.setZero();
    work_.setZero();
    counter_=0;
  }
  return ret;
}

