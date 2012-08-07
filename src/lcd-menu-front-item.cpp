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
  lcd->_lcd[0]="Status wysylania";
  lcd->_lcd[1]="--ERR timeout";
  lcd->_refresh=5000;
  lcd->_curOn=false;
}
