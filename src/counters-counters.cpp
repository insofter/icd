#include "counters-counters.hpp"
#include <iostream>

CcounterVal::CcounterVal(): val(0) {
  0; //NOOP
}

CcounterVal::CcounterVal( int val_, Ctime dark_, Ctime work_ ) :val(val_), dark(dark_), work(work_) {
  0; //NOOP
}


Ccounter::Ccounter( int id, int master, Ctime beginTime ): 
  reader_(NULL), id_(id), counter_(0), masterId_(master)
{ 0; //NOOP
}

Ccounter::~Ccounter() {
  0; //NOOP
}

void Ccounter::setReader( CdevicesReader* reader ) {
  reader_=reader;
}
void Ccounter::addLed( Cled * led ) {
  leds_.push_back(led);
}

void Ccounter::clearLeds() {
  leds_.clear();
}

void Ccounter::ledOn_() {
  for( int i=0; i<leds_.size(); ++i ) {
    leds_[i]->on();
  }
}

void Ccounter::ledOff_() {
  for( int i=0; i<leds_.size(); ++i ) {
    leds_[i]->off();
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

  Cevent ev=reader_->getEvent( masterId_, reverse_ );
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
  last_slave_( beginTime, 0 ), last_master_( beginTime, 0 ),
  dark_( 0, 0 ), work_( 0, 0 ) 
{
  0;
  //NOOP
}

CcounterThick::~CcounterThick() {
  0; //NOOP
}

CcounterVal CcounterThick:: getCount( const Ctime time, Econstants reset ) {
  CcounterVal ret;

  std::cout << "================================" <<std::endl;

  Cevent ev_master = reader_->getEvent( masterId_ );
  if( ev_master == Cevent::EMPTY() ) {
    ev_master=last_master_;
  }

  Cevent ev_slave = reader_->getEvent( slaveId_ ) ;
  if( ev_slave == Cevent::EMPTY() ) {
    ev_slave=last_slave_;
  }

  if( ev_slave == last_slave_ && ev_master == last_master_ ) {// no change to counter 

    /*high*/
    if( state_master_==CONDHIGH ) {
      if( ev_master.time + engage_ < time ) {
        state_master_=HIGH;
        std::cout << "<<< state master high" <<std::endl;

      } // if master havent reached -> keep calm and no action
    }
    if( state_slave_==CONDHIGH ) {
      if( ev_slave.time + engage_ < time ) {
        state_slave_=HIGH;
        std::cout << "<<< state slave high" <<std::endl;
      } // if slave havent reached -> keep calm and no action
    }
    if( state_slave_==HIGH && state_master_==HIGH ) {//proper signal

      if( state_!=HIGH ) {
        state_=HIGH;
        ledOn_();
        ++counter_;
        std::cout << "....state high: counter++" <<std::endl;
      }
    }


    /*low*/
    if( state_master_==CONDLOW) {
      if( ev_master.time + release_ < time ) {
        state_master_=LOW;
        std::cout << "<<< state master low" <<std::endl;

      } // if master havent reached -> keep calm and no action
    }
    if( state_slave_==CONDLOW ) {
      if( ev_slave.time + release_ < time ) {
        state_slave_=LOW;
        std::cout << "<<< state slave low" <<std::endl;

      } // if slave havent reached -> keep calm and no action
    }
    if( state_slave_==LOW || state_master_==LOW ) {//signal down
      state_=LOW;
      ledOff_();

      std::cout << "....state low" <<std::endl;

    }

  } else { // if either SLAVE or MASTER differs from last -> NEW EVENT
    /*NEW EVENT*/

    /*master*/
    if( last_master_.value != ev_master.value ) { //check if master has changed; duplicete -- ignore

      if( ev_master.value==0 ){ //change master to CONDLOW
        if( state_master_==CONDHIGH ) {
          if( last_master_.time + engage_ < ev_master.time ) {//time passed count correct before event
            state_master_=HIGH;
            std::cout << "<< state master high" <<std::endl;
          }
        }
        //count times and change state
        dark_=dark_ + ev_master.time - last_master_.time;
        work_=work_ + ev_master.time - last_master_.time;
        state_master_=CONDLOW;
        std::cout << "<< state master condlow" <<std::endl;
      } else { //change master to CONDHIGH
        if( state_master_==CONDLOW ) {
          if( last_master_.time + release_ < ev_master.time ) {//time passed and count is correct before event
            state_master_=LOW;
            std::cout << "<< state master low" <<std::endl;

            state_ = LOW;
            std::cout << "....state low" <<std::endl;
          }
        }
        work_=work_ + ev_master.time - last_master_.time;
        state_master_=CONDHIGH;
        std::cout << "<< state master condhigh" <<std::endl;

      }
      last_master_=ev_master;
    }

    /*slave*/

    if( last_slave_.value != ev_slave.value ) { //check if slave has changed; duplicete -- ignore

      if( ev_slave.value==0 ){ //change slave to CONDLOW
        if( state_slave_==CONDHIGH ) {
          if( last_slave_.time + engage_ < ev_slave.time ) {//time passed count correct before event
            state_slave_=HIGH;
            std::cout << "<< state slave high" <<std::endl;


          }
        }
        //count times and change state
        state_slave_=CONDLOW;
        std::cout << "<< state slave condlow" <<std::endl;

      } else { //change slave to CONDHIGH
        if( state_slave_==CONDLOW ) {
          if( last_slave_.time + release_ < ev_slave.time ) {//time passed and count is correct before event
            state_slave_=LOW;
            std::cout << "<< state slave low" <<std::endl;

            state_ = LOW;
            std::cout << "....state low" <<std::endl;

          }
        }
        // work_=work_ + ev_master.time - last_master_.time;
        state_slave_=CONDHIGH;
        std::cout << "<< state slave condhigh" <<std::endl;

      }
      last_slave_=ev_slave;
    }
  }//end new event - counter doest change - wait for it

  ret.id=id_;
  ret.val=counter_;
  ret.work=work_ + time - ev_master.time;
  if( state_==HIGH ) {//darktime is ticking
    ret.dark=dark_ + time - ev_master.time;
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







