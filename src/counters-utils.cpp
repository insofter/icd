#include "counters-utils.hpp"
#include <iostream>
#include <stdio.h>

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
Cevent::Cevent( const char * buf ): time(0,0) {
  if( sscanf( buf, "%d %d %d", &time.sec, &time.usec, &value )!=3 ) {
    //wrong event, set EMPTY
    time.sec=INT_MIN;
    time.usec=0;
    value=INT_MIN;
  }
}

bool Cevent::operator==( const Cevent & b ) const {
  if( time==b.time && value==b.value ) {
    return true;
  } else {
    return false;
  }
}

Cevent Cevent::EMPTY() {
  Cevent empty(INT_MIN, 0, INT_MIN);
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
  int fd = open( dev.c_str(), O_RDONLY | O_NONBLOCK );
  std::cerr << "Opened '" << dev << "' as number " << id << std::endl;
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

  int i=0; std::map< int, int >::iterator J=devices_.begin();
  for( /*init above*/; J!=devices_.end(); ++J, ++i ) {
    pollfd_[i].fd=J->first;
    pollfd_[i].events=POLLIN;
  }
  int ret=poll( pollfd_, devices_.size(), wait.msec() );
  if( ret > 0 ) {//haz events
    int size=devices_.size();
    for( i=0; i<size; ++i ) {
      if( pollfd_[i].revents & POLLIN ) {
        int j=0;
        while( 1==1 ) {
          if( read( pollfd_[i].fd, buf_+j, 1 )==0 ) {// EOF
            break;
          } else if( buf_[j]=='\n' ) {// end of event, create object
            buf_[j]=0;
            Cevent ev( buf_ );
            events_[ devices_[ pollfd_[i].fd ] ]=ev;

            /*debug*/std::cout << "event inserted: " << ev.time.sec << " " << ev.time.usec
              << " " << ev.value << std::endl;

            break;
          }
          ++j;
        }
      } else if( pollfd_[i].revents & (POLLNVAL | POLLERR | POLLHUP) ) {
        //broken fd,  fallback==rtemove
        std::cerr << "Err: counter=" << devices_[ pollfd_[i].fd ]
          << ", fd=" << pollfd_[i].fd << ", poll() revents=";
        if( pollfd_[i].revents & POLLNVAL ) {
          std::cerr << "POLLNVAL, ";
        }
        if( pollfd_[i].revents & POLLERR ) {
          std::cerr << "POLLERR, ";
        }
        if( pollfd_[i].revents & POLLHUP ) {
          std::cerr << "POLLHUP, ";
        }
        std::cerr << std::endl;

        events_.erase( devices_[ pollfd_[i].fd ] );
        devices_.erase( pollfd_[i].fd );
      }
    }
  }
  return ret;
}

const Cevent CdevicesReader::getEvent( int devId ) {
  if( events_.count( devId ) > 0 ) {
    return events_[devId];
  } else {
    return Cevent::EMPTY();
  }
}




