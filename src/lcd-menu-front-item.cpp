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


CmenuItemTimeFoto::CmenuItemTimeFoto(int a, int b): _a(a), _b(b) {
  stmt = new sqlite3cc::stmt( *globalLiveDb );
  stmt->prepare( "SELECT `cnt` FROM flow WHERE counter_id = ?1 ORDER BY `dtm` DESC LIMIT 1" );

}

CmenuItemTimeFoto::~CmenuItemTimeFoto() {
  stmt->finalize();
  delete stmt;
}

void CmenuItemTimeFoto::screen(Clcd *lcd) {
  /*TODO pobranie danych z bazy*/
  int i;
  char buf[17];
  char timebuf[9];
  time_t rawtime;
  struct tm * timeinfo;
  char sql[8];

  time( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime(timebuf,8,"%y.%m.%d",timeinfo);
  timebuf[8]=0;


//  sprintf(sql, "itd%i", _a);
  stmt->bind_int( 1, _a );
  if( stmt->step() == SQLITE_ROW ) {
    i=stmt->column_int(0);
  } else {
    i=0;
  }

  stmt->reset();

  sprintf(buf, "%s %c:%5i", timebuf, 'A'+_a, i);
  lcd->_lcd[0]=buf;

  strftime(timebuf,8,"%H:%M:%S",timeinfo);
  timebuf[8]=0;


//  sprintf(sql, "itd%i", _b);
  stmt->bind_int( 1, _b );
  if( stmt->step() == SQLITE_ROW ) {
    i=stmt->column_int(0);
  } else {
    i=0;
  }
  stmt->reset();

  sprintf(buf, "%s %c:%5i", timebuf, 'A'+_b, i);
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
