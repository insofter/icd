#include "counters-utils.hpp"
#include <iostream>

Ctime::Ctime() {
  this->setCurrentTime();
}

Ctime::Ctime( int sec_, int usec_ ): sec(sec_), usec(usec_) {
  this->tidy();
}

void Ctime::tidy() {

  if( usec >= NUMBER_USECS ) {
    sec+=usec/NUMBER_USECS;
    usec=usec%NUMBER_USECS;
  } else if( usec < 0 ) {
    sec-=1;
    sec+=usec/NUMBER_USECS;

    usec=usec%NUMBER_USECS;
    usec+=NUMBER_USECS;
  }

}

void Ctime::setCurrentTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  sec=tv.tv_sec;
  usec= tv.tv_usec;
}

Ctime Ctime::operator-( const Ctime& b ) const {
  Ctime out( sec-b.sec, usec-b.usec );
  return out;
}

Ctime Ctime::operator+( const Ctime& b ) const {
  Ctime out( sec+b.sec, usec+b.usec );
  return out;
}

bool Ctime::operator<( const Ctime& b ) const {
  if( sec < b.sec ) {
    return true; //a.s<b.s
  } else if( sec > b.sec ) {
    return false; //a.s>b.s
  } else if( usec < b.usec ) {
    return true; //a.s==b.s && a.u<b.u
  } else {
    return false; //a.s==b.s && a.u>=b.u
  }
}

Cevent::Cevent( int sec_, int usec_, int value_, int dev_ ): 
  time( sec_, usec_ ), value(value_), dev(dev_) {
  }

Cevent::Cevent( Ctime time_, int value_, int dev_ ):
  time(time_), value(value_), dev(dev_) {
  }

CdbWriter::CdbWriter() {
//TODO:
}

void CdbWriter::write( int counterId, Ctime dtm, int cnt, Ctime dark, Ctime work, int test ) {
//TODO:
}

void CdbWriter::newAggr() {
//TODO:
}

Cdevice::Cdevice( std::string dev, int id, Ctime engage, Ctime release ) {

}

Ctime Cdevice::getDarktime( Ereset reset ) {

}

int Cdevice::getFd() {

}

Cevent Cdevice::getEvent() {

}
