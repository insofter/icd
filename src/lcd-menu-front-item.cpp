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

void CmenuItemTimeFoto::screen(Clcd *lcd) {
  int i=23;
  char buf[17];
  char timebuf[9];
  time_t rawtime;
  struct tm * timeinfo;

  time( &rawtime );
  timeinfo = localtime ( &rawtime );

  strftime(timebuf,8,"%y.%m.%d",timeinfo);
  timebuf[8]=0;
  sprintf(buf, "%s %c:%5i", timebuf, 'A', i);
  lcd->_lcd[0]=buf;

  strftime(timebuf,8,"%H:%M:%S",timeinfo);
  timebuf[8]=0;
  sprintf(buf, "%s %c:%5i", timebuf, 'B', i+3423);
  lcd->_lcd[1]=buf;

  lcd->_refresh=300;
  lcd->_curOn=false;
}



void CmenuItemIdds::screen(Clcd *lcd) {
  lcd->_lcd[0]="ids:        PL15";
  lcd->_lcd[1]="idd:     0123456";
  lcd->_refresh=0;
  lcd->_curOn=false;
}



void CmenuItemSendStat::screen(Clcd *lcd) {
  lcd->_lcd[0]="Status wysyłania";
  lcd->_lcd[1]="--ERR timeout";
  lcd->_refresh=5000;
  lcd->_curOn=false;
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
  if( _run==0 ) {
    return 1;
  } else {
    this->up(lcd);
  }
}

CmenuItemRunTestApp::CmenuItemRunTestApp(std::string name, 
    std::string info, std::string path, std::string head1, std::string head2):
  _name(name), _info(info), _path(path), _head1(head1), _head2(head2) {
  _app=NULL;
  _smig=0;
  _run=0;
}

int CmenuItemRunTestApp::up(Clcd *lcd) {
  if( _run==0 ) {
    _progress=0;
    _app=popen( _path.c_str(), "r" );
    if( _app==NULL ) {
      return 0;
    }
    _appfd=fileno(_app);
    _tmp=0;
    _run=1;
  } else if( _run==2 )
  {
    _run=0;
  }
  this->screen(lcd);
  return 0;
}

void CmenuItemRunTestApp::screen(Clcd *lcd) {
  if( _run==0 ) {
    lcd->_lcd[0]=_name;
    lcd->_lcd[1]=_info;
    lcd->_refresh=0;
    lcd->_curOn=false;
  } else if( _run==2 ) {
    lcd->_lcd[0]=_name;
    lcd->_lcd[1]=_buf;
    lcd->_refresh=0;
    lcd->_curOn=false;
  } else {
    struct pollfd fds[1];
    char b;
    int pos=0;
    int i;

    if( _smig ) {
      lcd->_lcd[0]=_head1;
      _smig=0;
    } else {
      lcd->_lcd[0]=_head2;
      _smig=1;
    }

    fds[0].fd=_appfd;
    fds[0].events=POLLIN;
    
    poll( fds, 1, 0 );
    if( fds[0].revents & ( POLLERR | POLLRDHUP | POLLNVAL) ) {
      sprintf(_buf, "--ERR błąd prog.");
      pclose( _app );
      _app=NULL;
      _run=2;
    } else {
      while( fds[0].revents & POLLIN ) {
        read( _appfd, &b, 1 );
        if( b!='\t' && b!='\n' &&  b!='\r'/* && b!=' '*/ ) {
          if( b>='0' && b<='9' ) {
            _tmp*=10;
            _tmp+=(b-'0');
          } else {
            if( pos<32 ) {
              _buf[pos]=b;
              ++pos;
            }
          }
        } else {
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
        if( fds[0].revents & ( POLLERR | POLLRDHUP | POLLNVAL) ) {
          sprintf(_buf, "--ERR błąd prog.");
          pclose( _app );
          _app=NULL;
          _run=2;
          break;
        }
      }  

      if( pos==0 ) {
        for( i=0; i<(_progress*16)/100; ++i ) {
          _buf[i]='#';
        }
        for( ; i<16 ; ++i) {
          _buf[i]='-';
        }
        i=sprintf(_buf+7, "%i", _progress);
        _buf[7+i]='%';
        _buf[16]=0;
      } else {
        _buf[pos]=0;
        pclose( _app );
        _app=NULL;
        _run=2;
      }
    }
    lcd->_lcd[1]=_buf;
    lcd->_refresh=333;
    lcd->_curOn=false;
  }
}
