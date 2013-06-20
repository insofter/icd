#include "counters-counters.hpp"
#include <iostream>

CcounterVal::CcounterVal(): val(0) {
  0; //NOOP
}

CcounterVal::CcounterVal( int val_, Ctime dark_, Ctime work_ ) :val(val_), dark(dark_), work(work_) {
  0; //NOOP
}


Ccounter::Ccounter( int id, int master, Ctime beginTime ): 
  reader_(NULL), led_(NULL), id_(id), counter_(0), masterId_(master)
{ 0; //NOOP
}

Ccounter::~Ccounter() {
  0; //NOOP
}

void Ccounter::setReader( CdevicesReader* reader ) {
  reader_=reader;
}
void Ccounter::setLed( Cled * led ) {
  led_=led;
}

void Ccounter::ledOn_() {
  if( led_!=NULL ) {
    led_->on();
  }
}

void Ccounter::ledOff_() {
  if( led_!=NULL ) {
    led_->off();
  }
}

CcounterMono::CcounterMono( int id, int master, const Ctime beginTime, const Ctime engage, const Ctime release, Econstants reverse ): 
  Ccounter( id, master, beginTime ), engage_(engage), release_(release), state_(LOW), reverse_(reverse),
  last_( beginTime, 0 ), dark_(0, 0), work_(0, 0)
{ 0; //NOOP
}

CcounterMono::~CcounterMono() {
  0; //NOOP
}

CcounterVal CcounterMono::getCount( const Ctime time, Econstants reset ) {
  CcounterVal ret;

  Cevent ev=reader_->getEvent( masterId_ );
  if( ev == Cevent::EMPTY() ) {
    ev=last_;
  }

  if( ev == last_ ) {//no change, only clock is ticking...

    if( state_==CONDHIGH ) {
      if( ev.time + engage_ < time ) {//change to high and ++counter
        state_=HIGH;
        ledOn_();
        ++counter_;
      }
    } else if( state_==CONDLOW ) {
      if( ev.time + release_ < time ) {//change to low and count darktime
        state_=LOW;
        ledOff_();
      }
    }
    // LOW and HIGH with no change == no effect

  } else {//new event
    if( ev.value!=last_.value ) {//duplicated are ignored (ex. high to high)
      if( ev.value==0 ) {//change to CONDLOW

        if( state_==CONDHIGH ) {
          if( last_.time + engage_ < ev.time ) {//time passed and count is correct before event
            state_=HIGH;
            ledOn_();
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
            ledOff_();
          }
        }
        //count times and change state
        work_=work_ + ev.time - last_.time;
        state_=CONDHIGH;
      }
      last_=ev;
    }
  }

  ret.id=id_;
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


CcounterThick::CcounterThick( int id, int master, int slave, const Ctime beginTime, const Ctime engage, const Ctime release, Econstants reverse ) :
  Ccounter( id,  master, beginTime ), engage_( engage ), release_( release ), 
  state_( LOW ), state_master_ ( LOW ) , state_slave_( LOW ),
  reverse_( reverse ), 
  last_( beginTime, 0 ), last_slave_( beginTime, 0 ), last_master_( beginTime, 0 ),
  dark_( 0, 0 ), work_( 0, 0 ) {
    0;
    //NOOP
  }

CcounterThick::~CcounterThick() {
  0; //NOOP
}

CcounterVal CcounterThick:: getCount( const Ctime time, Econstants reset ) {
  CcounterVal ret;

  Cevent ev_master = reader_->getEvent( masterId_ );
  if( ev_master == Cevent::EMPTY() ) {
    ev_master=last_master_;
  }

  Cevent ev_slave = reader_->getEvent( slaveId_ ) ;
  if( ev_slave == Cevent::EMPTY() ) {
    ev_slave=last_slave_;
  }

  if( ev_slave == last_slave_ && ev_master == last_master_ ) {// no change to counter 

    if( state_master_==CONDHIGH ) {
      if( ev_master.time + engage_ < time ) {
        state_master_=HIGH;
      } // if master havent reached -> keep calm and no action
    }
    if( state_slave_==CONDHIGH ) {
      if( ev_slave.time + engage_ < time ) {
        state_slave_=HIGH;
      } // if slave havent reached -> keep calm and no action
    }
    if( state_slave_==state_master_==HIGH ) {//proper signal
      state_=HIGH;
      ledOn_();
      ++counter_;
    }


    if( state_master_==CONDLOW) {
      if( ev_master.time + release_ < time ) {
        state_master_=LOW;
      } // if master havent reached -> keep calm and no action
    }
    if( state_slave_==CONDLOW ) {
      if( ev_slave.time + release_ < time ) {
        state_slave_=LOW;
      } // if slave havent reached -> keep calm and no action
    }
    if( state_slave_==LOW || state_master_==LOW ) {//signal down
      state_=LOW;
      ledOff_();
    }

  } else { // if either SLAVE or MASTER differs from last -> NEW EVENT
    /*NEW EVENT*/

    if( last_master_.value != ev_master.value ) { //check if master has changed duplicete -- ignotre

      if( ev_master.value==0 ){ //change to CONDLOW
        if( state_master_==CONDHIGH ) {
          if( last_master_.time + engage_ < ev_master.time ) {//time passed count correct before event
            state_master_=HIGH;
          }
        }
        //TODO







      }//end CONDLOW

    }//end check master
  }// end NEW EVENT

  //TODO 
  //reset
  //condhigh 
  //condlow


    return ret;
  }

