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

void Ctime::setZero() {
  sec=0;
  usec=0;
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
bool Ctime::operator!=( const Ctime & b ) const {
  return ( ! ( *this==b ) );
}

Cevent::Cevent( int sec_, int usec_, EeventVal value_ ): time( sec_, usec_ ), value(value_) {
}

Cevent::Cevent( Ctime time_, EeventVal value_ ): time(time_), value(value_) {
}
Cevent::Cevent( const char * buf ): time(0,0) {
  int tmp;
  if( sscanf( buf, "%d %d %d", &time.sec, &time.usec, &tmp )==3 ) {
    if( tmp==0 ) {
      value=DARK;
    } else {
      value=LIGHT;
    }
  } else {
    //wrong event, set EMPTY
    time.sec=INT_MIN;
    time.usec=0;
    value=ERROR;
  }
}

bool Cevent::operator==( const Cevent & b ) const {
  if( time==b.time && value==b.value ) {
    return true;
  } else {
    return false;
  }
}
bool Cevent::operator!=( const Cevent & b ) const {
  return ( ! ( *this==b ) );
}

Cevent Cevent::EMPTY() {
  Cevent empty(INT_MIN, 0, ERROR);
  return empty;
}

// temp solution
Cevent latest( const Cevent & ev1, const Cevent & ev2 )
{
  if( ev1.time < ev2.time ) {
    return ev2;
  } else {
    return ev1;
  }
}
//temp solution
Cevent earliest( const Cevent & ev1, const Cevent & ev2 )
{
  if( ev2.time < ev1.time ) {
    return ev2;
  } else {
    return ev1;
  }
}

Cled::Cled( std::string dev ) {
  fd_=open( dev.c_str(), O_WRONLY );
  this->off();
}
Cled::~Cled() {
  close( fd_ );
}

void Cled::on() {
  write( fd_, "1\n", 2 );
}
void Cled::off() {
  write( fd_, "0\n", 2 );
}



CdbWriter::CdbWriter() {


  dataInsert_ = new sqlite3cc::stmt( *globalDataDb );
  dataUpdate_ = new sqlite3cc::stmt( *globalDataDb );
  dataSelect_ = new sqlite3cc::stmt( *globalDataDb );
  dataClose_ = new sqlite3cc::stmt( *globalDataDb );
  dataBegin_ = new sqlite3cc::stmt( *globalDataDb );
  dataCommit_ = new sqlite3cc::stmt( *globalDataDb );

  dataInsert_->prepare( "INSERT INTO flow (counter_id, dtm, cnt, dark_time, work_time, test, flags)"
            " VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)" );

  dataUpdate_->prepare( "UPDATE flow SET cnt = ?3, dark_time = ?4,"
      " work_time = ?5, test = ?6, flags = ?7 WHERE counter_id = ?1 AND dtm = ?2" );

  dataClose_->prepare( "UPDATE flow SET flags = 2 WHERE flags = 3" ); 

  dataSelect_->prepare( "SELECT counter_id FROM flow"
      " WHERE counter_id == ?1 AND dtm == ?2 LIMIT 1" ); //usunięte ,,AND flags == 3'', nie potrzebne

  dataBegin_->prepare( "BEGIN IMMEDIATE TRANSACTION" );
  dataCommit_->prepare( "COMMIT TRANSACTION" );

  this->dataCloseRecords();
  //---------------------------------------------------------
  liveInsert_ = new sqlite3cc::stmt( *globalLiveDb );
  liveUpdate_ = new sqlite3cc::stmt( *globalLiveDb );
  liveSelect_ = new sqlite3cc::stmt( *globalLiveDb );
  liveClose_ = new sqlite3cc::stmt( *globalLiveDb );
  liveBegin_ = new sqlite3cc::stmt( *globalLiveDb );
  liveCommit_ = new sqlite3cc::stmt( *globalLiveDb );

  liveInsert_->prepare( "INSERT INTO flow (counter_id, dtm, cnt, dark_time, work_time, test, flags)"
            " VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)" );

  liveUpdate_->prepare( "UPDATE flow SET cnt = ?3, dark_time = ?4,"
      " work_time = ?5, test = ?6, flags = ?7 WHERE counter_id = ?1 AND dtm = ?2" );

  liveClose_->prepare( "UPDATE flow SET flags = 2 WHERE flags = 3" ); //zmienione
  liveDelete_->prepare( "DELETE FROM flow WHERE flags = 2" );

  liveSelect_->prepare( "SELECT counter_id FROM flow"
      " WHERE counter_id == ?1 AND dtm == ?2 AND flags == 3 LIMIT 1" );

  liveBegin_->prepare( "BEGIN IMMEDIATE TRANSACTION" );
  liveCommit_->prepare( "COMMIT TRANSACTION" );

  this->liveCloseRecords();
}

CdbWriter::~CdbWriter() {
  delete [] liveInsert_;
  delete [] liveUpdate_;
  delete [] liveSelect_;
  delete [] liveClose_;
  delete [] liveDelete_;
  delete [] liveBegin_;
  delete [] liveCommit_;

  delete [] dataInsert_;
  delete [] dataUpdate_;
  delete [] dataSelect_;
  delete [] dataClose_;
  delete [] dataBegin_;
  delete [] dataCommit_;
}


void CdbWriter::dataWrite( int counterId, Ctime dtm, int cnt, Ctime dark, Ctime work, int test, int flags ) {
  dataSelect_->bind_int( 1, counterId );
  dataSelect_->bind_int( 2, dtm.sec );
  if( dataSelect_->step() == SQLITE_ROW ) {//update
    dataUpdate_->bind_int( 1, counterId );//counter_id
    dataUpdate_->bind_int( 2, dtm.sec );//dtm
    dataUpdate_->bind_int( 3, cnt );//cnt
    dataUpdate_->bind_int( 4, dark.msec() );//dark_time
    dataUpdate_->bind_int( 5, work.msec() );//work_time
    dataUpdate_->bind_int( 6, test );//test
    dataUpdate_->bind_int( 7, flags );//flags
    dataUpdate_->step();
    dataUpdate_->reset();
  } else { //insert
    dataInsert_->bind_int( 1, counterId );//counter_id
    dataInsert_->bind_int( 2, dtm.sec );//dtm
    dataInsert_->bind_int( 3, cnt );//cnt
    dataInsert_->bind_int( 4, dark.msec() );//dark_time
    dataInsert_->bind_int( 5, work.msec() );//work_time
    dataInsert_->bind_int( 6, test );//test
    dataInsert_->bind_int( 7, flags );//flags
    dataInsert_->step();
    dataInsert_->reset();
  }
  dataSelect_->reset();
}

void CdbWriter::liveWrite( int counterId, Ctime dtm, int cnt, Ctime dark, Ctime work, int test, int flags ) {
  liveSelect_->bind_int( 1, counterId );
  liveSelect_->bind_int( 2, dtm.sec );
  if( liveSelect_->step() == SQLITE_ROW ) {//update
    liveUpdate_->bind_int( 1, counterId );//counter_id
    liveUpdate_->bind_int( 2, dtm.sec );//dtm
    liveUpdate_->bind_int( 3, cnt );//cnt
    liveUpdate_->bind_int( 4, dark.msec() );//dark_time
    liveUpdate_->bind_int( 5, work.msec() );//work_time
    liveUpdate_->bind_int( 6, test );//test
    liveUpdate_->bind_int( 7, flags );//flags
    liveUpdate_->step();
    liveUpdate_->reset();
  } else { //insert
    liveInsert_->bind_int( 1, counterId );//counter_id
    liveInsert_->bind_int( 2, dtm.sec );//dtm
    liveInsert_->bind_int( 3, cnt );//cnt
    liveInsert_->bind_int( 4, dark.msec() );//dark_time
    liveInsert_->bind_int( 5, work.msec() );//work_time
    liveInsert_->bind_int( 6, test );//test
    liveInsert_->bind_int( 7, flags );//flags
    liveInsert_->step();
    liveInsert_->reset();
    liveDelete_->step();
    liveDelete_->reset();
  }
  liveSelect_->reset();
}

void CdbWriter::dataCloseRecords() {
  dataClose_->step();
  dataClose_->reset();
}
void CdbWriter::liveCloseRecords() {
  liveClose_->step();
  liveClose_->reset();
}

bool CdbWriter::dataBeginTransaction() {
  bool ret=true;
  try {
    dataBegin_->step();
    dataBegin_->reset();
  } catch(...) {
    std::cerr << "Nie rozpoczęto transakcji" << std::endl;
    ret=false;
  }
  return ret;
}
bool CdbWriter::liveBeginTransaction() {
  bool ret=true;
  try {
    liveBegin_->step();
    liveBegin_->reset();
  } catch(...) {
    std::cerr << "Nie rozpoczęto transakcji" << std::endl;
    ret=false;
  }
  return ret;
}

void CdbWriter::dataCommitTransaction() {
  dataCommit_->step();
  dataCommit_->reset();
}
void CdbWriter::liveCommitTransaction() {
  liveCommit_->step();
  liveCommit_->reset();
}


CdevicesReader::CdevicesReader(): pollfd_(NULL) {
  0; //NOOP
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
            if( ev.time.sec > 3600 ) {
              events_[ devices_[ pollfd_[i].fd ] ]=ev;
              /*debug*/std::cout << "Event inserted: dev(" << devices_[ pollfd_[i].fd ] <<  "), usec(" 
                << ev.time.usec << "), val(" << ev.value << "),  " << ctime( &ev.time.sec ) << std::endl;
            }

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

const Cevent CdevicesReader::getEvent( int devId, Econstants reverse ) {
  if( events_.count( devId ) > 0 ) {
    Cevent ev=events_[devId];
    if( reverse==REVERSE ) {
      if( ev.value==DARK ) {
        ev.value=LIGHT;
      } else {
        ev.value=DARK;
      }
    }
    return ev;
  } else {
    return Cevent::EMPTY();
  }
}




