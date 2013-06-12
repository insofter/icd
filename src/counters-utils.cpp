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
int Ctime::msec() {
  return sec*1000+usec/1000;
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

bool Ctime::operator==( const Ctime& b ) const {
  if( sec==b.sec && usec==b.usec ) {
    return true;
  } else {
    return false;
  }
}

Cevent::Cevent( int sec_, int usec_, int value_ ): time( sec_, usec_ ), value(value_) {
}

Cevent::Cevent( Ctime time_, int value_ ): time(time_), value(value_) {
}

bool Cevent::operator==( const Cevent & b ) const {
  if( time==b.time && value==b.value ) {
    return true;
  } else {
    return false;
  }
}

Cevent Cevent::EMPTY() {
  Cevent empty(INT_MIN, INT_MIN, INT_MIN);
  return empty;
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




CdevicesReader::CdevicesReader(): pollfd_(NULL) {

}
CdevicesReader::~CdevicesReader() {
  delete [] pollfd_;

  for( std::map< int, int >::iterator i=devices_.begin(); i!=devices_.end(); ++i ) {
    close( i->first );
  }
}

int CdevicesReader::addDevice( std::string dev, int id ) {
  int fd = open( dev.c_str(), O_RDONLY );
  if( fd!=-1 ) {
    devices_[fd]=id;
    if( pollfd_!=NULL ) {
      delete [] pollfd_;
    }
    pollfd_= new struct pollfd [ devices_.size() ];

    return fd;
  } else {
    return -1;
  }
}

int CdevicesReader::pollEvents( Ctime wait ) {

    int j=0; std::map< int, int >::iterator i=devices_.begin();
    for( /*init above*/; i!=devices_.end(); ++i, ++j ) {
      pollfd_[j].fd=i->first;
      pollfd_[j].events=POLLIN;
    }
    poll( pollfd_, devices_.size(), 00000000 );



}

const Cevent CdevicesReader::getEvent( int devId ) {
  if( events_.count( devId ) > 0 ) {
    return events_[devId];
  } else {
    return Cevent::EMPTY();
  }
}




