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




int CmenuItemConnectionTest::down(Clcd *lcd) {
    return this->up(lcd);
}

int CmenuItemConnectionTest::left(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuItemConnectionTest::right(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuItemConnectionTest::enter(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuItemConnectionTest::esc(Clcd *lcd) {
  if( _run==0 ) {
    return 1;
  } else {
    this->up(lcd);
  }
}

CmenuItemConnectionTest::CmenuItemConnectionTest() {
  _app=NULL;
  _smig=0;
  _run=0;
}

int CmenuItemConnectionTest::up(Clcd *lcd) {
  if( _run==0 ) {
    _progress=0;
    _app=popen("/root/icd-conn-test --short", "r");
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

void CmenuItemConnectionTest::screen(Clcd *lcd) {
  if( _run==0 ) {
    lcd->_lcd[0]="Test połączenia";
    lcd->_lcd[1]=">Uruchom";
    lcd->_refresh=0;
    lcd->_curOn=false;
  } else if( _run==2 ) {
    lcd->_lcd[0]="Test połączenia";
    lcd->_lcd[1]=_buf;
    lcd->_refresh=0;
    lcd->_curOn=false;
  } else {
    struct pollfd fds[1];
    char b;
    int pos=0;
    int i;

    if( _smig ) {
      lcd->_lcd[0]="Test połączenia>";
      _smig=0;
    } else {
      lcd->_lcd[0]="Test połączenia-";
      _smig=1;
    }

    fds[0].fd=_appfd;
    fds[0].events=POLLIN;
    
    poll( fds, 1, 0 );
    while( fds[0].revents & POLLIN ) {
      read( _appfd, &b, 1 );
      if( b!=' ' && b!='\t' && b!='\n' &&  b!='\r' ) {
        if( b>='0' && b<'9' ) {
          _tmp*=10;
          _tmp+=(b-'0');
        } else {
          if( pos<16 ) {
            _buf[pos]=b;
            ++pos;
          }
        }
      } else {
        if( _tmp>_progress ) {
          _progress=_tmp;
        }
        _tmp=0;
      }
      poll( fds, 1, 0 );
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
    } else {
      _buf[pos]=0;
      fclose( _app );
      _app=NULL;
      _run=2;
    }
    lcd->_lcd[1]=_buf;
    lcd->_refresh=333;
    lcd->_curOn=false;
  }
}
