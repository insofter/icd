#include "lcd-menu-front-item.hpp"

CmenuItemFrontMenu::CmenuItemFrontMenu(): CmenuItem("Main screen") {
}

int CmenuItemFrontMenu::up(Clcd *lcd) {
  return 1;
}

int CmenuItemFrontMenu::down(Clcd *lcd) {
  return 1;
}

int CmenuItemFrontMenu::left(Clcd *lcd) {
  return 1;
}

int CmenuItemFrontMenu::right(Clcd *lcd) {
  return 1;
}

int CmenuItemFrontMenu::enter(Clcd *lcd) {
  return 1;
}

int CmenuItemFrontMenu::esc(Clcd *lcd) {
  return 1;
}

void CmenuItemFrontMenu::fullEsc() {
}

CmenuItemTimeFoto::CmenuItemTimeFoto(int a): _a(a) {
  hour = new sqlite3cc::stmt( *globalLiveDb );
  hour->prepare( "SELECT `cnt`, `dtm` FROM flow WHERE counter_id = ?1 ORDER BY `dtm` DESC LIMIT 1" );

  sum = new sqlite3cc::stmt( *globalDataDb );
  sum->prepare( "SELECT sum(cnt) AS sum FROM flow WHERE counter_id = ?1 "
      "AND `dtm` <= ?2 AND `dtm` >= ?3" );
}

CmenuItemTimeFoto::~CmenuItemTimeFoto() {
  hour->finalize();
  delete hour;
  sum->finalize();
  delete sum;
}

void CmenuItemTimeFoto::screen(Clcd *lcd) {
  char buf[18];
  char timebuf[9];
  time_t rawtime;
  time_t daystart;
  struct tm * timeinfo;
  char sql[8];

  time( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime(timebuf,8,"%y-%m-%d",timeinfo);
  timebuf[8]=0;


  hour->bind_int( 1, _a );
  if( hour->step() == SQLITE_ROW ) {
    sprintf(buf, "%s %c:%5i", timebuf, 'A'-1+_a, hour->column_int(0));
  } else {
    sprintf(buf, "%s %c: xxx ", timebuf, 'A'-1+_a);
  }

  lcd->_lcd[0]=buf;

  strftime(timebuf,8,"%H:%M:%S",timeinfo);
  timebuf[8]=0;

  daystart=rawtime/24/3600;
  daystart*=(24*3600);
  sum->bind_int( 1, _a );
  sum->bind_int( 2, hour->column_int(1) );
  sum->bind_int( 3, daystart );
  if( sum->step() == SQLITE_ROW ) {
    sprintf(buf, "%s Σ:%5i", timebuf, sum->column_int(0)+hour->column_int(0) );
  } else {
    sprintf(buf, "%s Σ: xxx ", timebuf );
  }
  sum->reset();
  hour->reset();

  lcd->_lcd[1]=buf;

  lcd->_refresh=300;
  lcd->_cur._car=Ccur::none;
  //sprintf(buf, "%s%c%c%c:%5i", timebuf, Cletter::byte1, Cletter::byte2Eth, 'A'+_a, i);
}

CmenuItemDoubleTimeFoto::CmenuItemDoubleTimeFoto(int a, int b): _a(a), _b(b) {
  stmt = new sqlite3cc::stmt( *globalLiveDb );
  stmt->prepare( "SELECT `cnt` FROM flow WHERE counter_id = ?1 ORDER BY `dtm` DESC LIMIT 1" );

}

CmenuItemDoubleTimeFoto::~CmenuItemDoubleTimeFoto() {
  stmt->finalize();
  delete stmt;
}

void CmenuItemDoubleTimeFoto::screen(Clcd *lcd) {
  char buf[17];
  char timebuf[9];
  time_t rawtime;
  struct tm * timeinfo;
  char sql[8];

  time( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime(timebuf,8,"%y-%m-%d",timeinfo);
  timebuf[8]=0;


//  sprintf(sql, "itd%i", _a);
  stmt->bind_int( 1, _a );
  if( stmt->step() == SQLITE_ROW ) {
//    i=stmt->column_int(0);
    sprintf(buf, "%s %c:%5i", timebuf, 'A'-1+_a, stmt->column_int(0));
  } else {
    sprintf(buf, "%s %c: xxx ", timebuf, 'A'-1+_a);
    //i=0;
  }
  stmt->reset();

  lcd->_lcd[0]=buf;

  strftime(timebuf,8,"%H:%M:%S",timeinfo);
  timebuf[8]=0;


//  sprintf(sql, "itd%i", _b);
  stmt->bind_int( 1, _b );
  if( stmt->step() == SQLITE_ROW ) {
//    i=stmt->column_int(0);
    sprintf(buf, "%s %c:%5i", timebuf, 'A'-1+_b, stmt->column_int(0));
  } else {
//    i=0;
    sprintf(buf, "%s %c: xxx ", timebuf, 'A'-1+_b);
  }
  stmt->reset();

  lcd->_lcd[1]=buf;

  lcd->_refresh=300;
  lcd->_cur._car=Ccur::none;
  //sprintf(buf, "%s%c%c%c:%5i", timebuf, Cletter::byte1, Cletter::byte2Eth, 'A'+_a, i);
}



void CmenuItemIdds::screen(Clcd *lcd) {
  std::string val;

  val=globalConfig->entry( "device", "ids" );
  lcd->_lcd[0]="ids:";
  if( val.size()<=12 ) {
    lcd->_lcd[0].append( 12-val.size(), ' ' );
  }
  lcd->_lcd[0]+=val;

  val=globalConfig->entry( "device", "idd" );
  lcd->_lcd[1]="idd:";
  if( val.size()<=12 ) {
    lcd->_lcd[1].append( 12-val.size(), ' ' );
  }
  lcd->_lcd[1]+=val;

  lcd->_refresh=0;
  lcd->_cur._car=Ccur::none;
}


CmenuItemDbParam::CmenuItemDbParam(std::string name, 
    std::string sect, std::string key) {
  _param._key=key;
  _param._sect=sect;
  _param._name=name;
}

void CmenuItemDbParam::screen(Clcd *lcd) {
  std::string val;
  lcd->_lcd[0]=_param._name;
  val=globalConfig->entry(_param._sect, _param._key);
  if( val.size()>16 ) {
    lcd->_refresh=2000;
    if( val.size()!=_param._lastSize || _param._lastPos>=val.size() ) {
      _param._lastPos=0;
      _param._lastSize=val.size();
    }
    lcd->_lcd[1]=val.substr(_param._lastPos, 16);
    
    int ilCzesci=std::ceil( ((float)_param._lastSize)/16 );
    int ktora=std::floor( _param._lastPos/16 )+1;

    lcd->_cur._x=(( ktora*16 ) / ilCzesci)-1;

    lcd->_cur._y=1;
    lcd->_cur._car=Ccur::line;

    _param._lastPos+=16;
  } else {
    lcd->_refresh=10000;
    lcd->_lcd[1]=val;
    lcd->_cur._car=Ccur::none;
  }
}


CmenuItemAppParam::CmenuItemAppParam(std::string name, std::string cmd) {
  _param._cmd=cmd;
  _param._name=name;
  _check=0;
}

void CmenuItemAppParam::screen(Clcd *lcd) {
  char buf[128];
  if( _check <= 0 || _val.size() == 0 ) {
    FILE * cmd_out=popen( _param._cmd.c_str() , "r" );

    if( cmd_out && fgets( buf, 128, cmd_out ) ) {
      int i=0;
      while( buf[i]!=0 && buf[i]!='\n' ) {//find end and remove \n
        ++i;
      }
      buf[i]=0;
      _val=buf;
    } else {
      _val="--Err";
    }
    pclose( cmd_out );
    _check=2;
  } else {
    --_check;
  }

  lcd->_lcd[0]=_param._name;
  if( _val.size()>16 ) {
    if( _val.size()!=_lastSize || _lastPos>=_val.size() ) {
      _lastPos=0;
      _lastSize=_val.size();
    }
    lcd->_lcd[1]=_val.substr(_lastPos, 16);

    int ilCzesci=std::ceil( ((float)_lastSize)/16 );
    int ktora=std::floor( _lastPos/16 )+1;

    lcd->_cur._x=(( ktora*16 ) / ilCzesci)-1;

    lcd->_cur._y=1;
    lcd->_cur._car=Ccur::line;

    _lastPos+=16;

  } else {
    lcd->_lcd[1]=_val;
    lcd->_cur._car=Ccur::none;
  }
  lcd->_refresh=2000;
}




CmenuItemFileParam::CmenuItemFileParam(std::string fpname, 
    std::string file) {
  _file=file;
  name=fpname;
  _lastSize=0;
  _lastPos=0;
}

void CmenuItemFileParam::screen(Clcd *lcd) {
  std::string val;
  lcd->_lcd[0]=name;
//  val=globalConfig->entry(_param._sect, _param._key);
  std::fstream sendstat( _file.c_str() );
  if( sendstat.is_open() ) {
    getline( sendstat, val );
    sendstat.close();  
  }  

  if( val.size()>16 ) {
    lcd->_refresh=2000;
    if( val.size()!=_lastSize || _lastPos>=val.size() ) {
      _lastPos=0;
      _lastSize=val.size();
    }
    lcd->_lcd[1]=val.substr(_lastPos, 16);
    
    int ilCzesci=std::ceil( ((float)_lastSize)/16 );
    int ktora=std::floor( _lastPos/16 )+1;

    lcd->_cur._x=(( ktora*16 ) / ilCzesci)-1;

    lcd->_cur._y=1;
    lcd->_cur._car=Ccur::line;

    _lastPos+=16;
  } else {
    lcd->_refresh=10000;
    lcd->_lcd[1]=val;
    lcd->_cur._car=Ccur::none;
  }
}






int CmenuItemRunTestApp::down(Clcd *lcd) {
    return this->up(lcd);
}

int CmenuItemRunTestApp::left(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuItemRunTestApp::right(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuItemRunTestApp::enter(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuItemRunTestApp::esc(Clcd *lcd) {
  if( _run==CmenuItemRunTestApp::done ) {
    _run=CmenuItemRunTestApp::idle;
  }
  return 1;
}

CmenuItemRunTestApp::CmenuItemRunTestApp(const std::string & name, 
    const std::string & info, const std::string & path, 
    const std::string & head1, const std::string & head2,
    bool autoStart ): _autoStart(autoStart), _name(name), _info(info), 
    _path(path), _head1(head1), _head2(head2) {
  _app=NULL;
  _smig=0;
  _run=CmenuItemRunTestApp::idle;
}

int CmenuItemRunTestApp::up(Clcd *lcd) {
  if( _run==CmenuItemRunTestApp::idle ) {//state: 'not working', key == start
    _progress=0;
    _app=popen( _path.c_str(), "r" );
    if( _app==NULL ) {
      return 0;
    }
    _appfd=fileno(_app);
    _tmp=0;
    _run=CmenuItemRunTestApp::running;
  } else if( _run==CmenuItemRunTestApp::done ) {//state: 'after work', showing status line
    _run=CmenuItemRunTestApp::idle;
  }
  this->screen(lcd);
  return 0;
}

void CmenuItemRunTestApp::screen(Clcd *lcd) {
  if( _autoStart && _run==CmenuItemRunTestApp::idle ) {//for autostart we emulate keypress
    this->up(lcd);
    return;
  }

  if( _run==CmenuItemRunTestApp::idle ) {//state: 'not working', show start screen
    lcd->_lcd[0]=_name;
    lcd->_lcd[1]=_info;
    lcd->_refresh=0;
    lcd->_cur._car=Ccur::none;
  } else if( _run==CmenuItemRunTestApp::done ) {//state: 'after work', show status line
    lcd->_lcd[0]=_name;
    lcd->_lcd[1]=_buf;
    lcd->_refresh=0;
    lcd->_cur._car=Ccur::none;
  } else {//state: 'working'
    if( _smig ) {//blinking 
      lcd->_lcd[0]=_head1;
      _smig=0;
    } else {
      lcd->_lcd[0]=_head2;
      _smig=1;
    }//end blinking

    struct pollfd fds[1];
    char b;
    int pos=0;
    int i;

    fds[0].fd=_appfd;//prepare to poll
    fds[0].events=POLLIN;
    
    poll( fds, 1, 0 );

    if( (!( fds[0].revents & POLLIN)) && fds[0].revents & POLLHUP ) {
      sprintf(_buf, "--ERR błąd prog.");//HUP without IN means no running app
      pclose( _app );                   //App ended too fast (ex: without state)
      _app=NULL;                        //or can't be started (wrong path)
      _run=CmenuItemRunTestApp::done;
    } else {

      while( fds[0].revents & POLLIN ) {//main loop, read number or status line
        read( _appfd, &b, 1 );          //without blocking
        if( b!='\t' && b!='\n' &&  b!='\r') {
          if( pos==0 && b>='0' && b<='9' ) {//parse number
            _tmp*=10;
            _tmp+=(b-'0');
          } else {//end number, read state line
            if( pos<32 ) {
              _buf[pos]=b;
              ++pos;
            }
          }//end read state line
        } else {//set progress after read
          if( _tmp>_progress ) {
            if( _tmp > 100 ) {
              _progress=100;
            } else {
              _progress=_tmp;
            }
          }
          _tmp=0;
          break;
        }
        poll( fds, 1, 0 );
      }// end main loop  

      if( pos==0 ) {//there was no status line reading, print progressbar
        for( i=0; i<(_progress*16)/100; ++i ) {
          _buf[i]='#';
        }
        for( ; i<16 ; ++i) {
          _buf[i]='-';
        }
        i=sprintf(_buf+7, "%i", _progress);
        _buf[7+i]='%';
        _buf[16]=0;
      } else {//there was status line, end of application
        _buf[pos]=0;
        pclose( _app );
        _app=NULL;
        _run=CmenuItemRunTestApp::done;
      }

    }
    //put buffer to screen structure
    lcd->_lcd[1]=_buf;
    lcd->_refresh=333;
    lcd->_cur._car=Ccur::none;
  }
}
