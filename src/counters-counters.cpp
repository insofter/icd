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


CcounterThick::CcounterThick( 
    int id, int master, int slave, 
    const Ctime beginTime, 
    const Ctime engage_master, const Ctime engage_slave, 
    const Ctime release_master, const Ctime release_slave, 
    Econstants reverse_master, Econstants reverse_slave
    ) 
:
  Ccounter( id,  master, beginTime ), 
  slaveId_( slave ), 
  begin_time_( beginTime ), begin_work_(0 ,0 ),
  engage_master_( engage_master ), engage_slave_( engage_slave), 
  release_master_( release_master ), release_slave_( release_slave ),
  state_( LOW ),  state_master_ ( LOW ) , state_slave_( LOW ),
  reverse_master_( reverse_master ), reverse_slave_( reverse_slave ),
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
  if( ev_master == Cevent::EMPTY() ) {// if no event -- take last value
    ev_master=last_master_;
  }
  Cevent ev_slave = reader_->getEvent( slaveId_ ) ;
  if( ev_slave == Cevent::EMPTY() ) {
    ev_slave=last_slave_;
  }

  /*check if meanwhile state has changed*/
  /*master*/
  if( state_master_==CONDHIGH ) {
    if( last_master_.time + engage_master_ < time ) {
      state_master_=HIGH;
      std::cout << "<<< state master high" <<std::endl;
      dark_ = dark_ + ev_master.time - last_master_.time;
    } // if master havent reached -> keep calm and no action
  }
  if( state_master_==CONDLOW) {
    if( last_master_.time + release_master_ < time ) {
      state_master_=LOW;
      std::cout << ">>state  master low" << std::endl;
    } // if master havent reached -> keep calm and no action
  }

  /*slave*/
  if( state_slave_==CONDHIGH ) {
    if( last_slave_.time + engage_slave_ < time ) {
      state_slave_=HIGH;
      std::cout << "<<< state slave high" <<std::endl;
    } // if slave havent reached -> keep calm and no action
  }
  if( state_slave_==CONDLOW ) {
    if( last_slave_.time + release_slave_ < time ) {
      state_slave_=LOW;
      std::cout << "<<< state slave low" <<std::endl;
    } // if slave havent reached -> keep calm and no action
  }

  if( state_slave_==HIGH && state_master_==HIGH ) {//proper signal for counter
    if( state_!=HIGH ) {
      state_=HIGH;
      ledOn_();
      ++counter_;
      std::cout << "....state high: counter++" <<std::endl;
    }
  }
  if( state_slave_==LOW || state_master_==LOW ) {//counter state down
    state_=LOW;
    ledOff_();
    std::cout << "....state low" <<std::endl;
  }


  if( ev_slave == last_slave_ && ev_master == last_master_ ) {// check for new events 
    std::cout<< "no change" << std::endl;

  } else { // if either SLAVE or MASTER differs from last -> NEW EVENT

    /*NEW EVENT*/
    std::cout << "new event" << std::endl;

    /*master*/
    if( last_master_.value != ev_master.value ) { //check if master has changed; duplicete -- ignore
      std::cout << "new event: change master" << std::endl;
    
      if( ev_master.value==0 ){// event to change master to CONDLOW
       
        if( state_master_ == CONDLOW ) {//cond & new evernt - unproper
          std::cout << "== state master high" << std::endl;
          state_master_ = HIGH; //last proper state
          dark_ = dark_ + ev_master.time - last_master_.time;
          //smth with dark?
        } else if( state_master_ == CONDHIGH ) {//cond & new event - unproper 
          dark_ = dark_ + ev_master.time - last_master_.time;
          std::cout << "== state master low" << std::endl;
          state_master_=LOW;
        
        } else { // proper change
          //count times and change state
          dark_ = dark_ + ev_master.time - last_master_.time;
          state_master_=CONDLOW;
          std::cout << "<< state master condlow" <<std::endl;
        }

      } else {// event to change master to CONDHIGH
        if( state_master_ == CONDLOW ) {//cond & new event - unproper
          std::cout << "== state master high" << std::endl;
          state_master_ = HIGH; //last proper state
          //dark stil ticking
        } else if( state_master_ == CONDHIGH ) {// cond & new even - unproper
          std::cout << "== state master low" << std::endl;
          state_master_=LOW;
        
        } else {// proper change
          state_master_=CONDHIGH;
          std::cout << "<< state master condhigh" <<std::endl;
        }
      }
      last_master_ = ev_master;
    } // end check master

    /*slave*/
    if( last_slave_.value != ev_slave.value ) { //check if slave has changed; duplicete -- ignore
      std::cout << "new event: change slave" << std::endl;
     
      if( ev_slave.value==0 ){//event to change slave to CONDLOW
     
        if( state_slave_ == CONDLOW ) {//cond & new evernt - unproper
          std::cout << "== state slave high" << std::endl;
          state_slave_ = HIGH; //last proper state
        } else if( state_slave_ == CONDHIGH ) {//cond & new event - unproper 
          std::cout << "== state slave low" << std::endl;
          state_slave_=LOW;
        
        } else { // proper change
        state_slave_=CONDLOW;
        std::cout << "<< state slave condlow" <<std::endl;
        }

      } else { //event to change slave to CONDHIGH
        if( state_slave_ == CONDLOW ) {//cond & new event - unproper
          std::cout << "== state slave high" << std::endl;
          state_slave_ = HIGH; //last proper state
        } else if( state_slave_ == CONDHIGH ) {// cond & new even - unproper
          std::cout << "== state slave low" << std::endl;
          state_slave_=LOW;
        
        } else {// proper change
        state_slave_=CONDHIGH;
        std::cout << "<< state slave condhigh" <<std::endl;
        }
      }
      last_slave_=ev_slave;
    }// end check slave

  }//end new event


  ret.id=id_;
  ret.val=counter_;
  work_ = latest( ev_master, ev_slave).time - begin_time_; //not used
  ret.work = time - begin_time_;

  if( state_master_==HIGH || state_master_==CONDHIGH ) {//darktime is ticking
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



