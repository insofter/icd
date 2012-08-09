#ifndef LCD_MENU_FRONT_ITEM_HPP
#define LCD_MENU_FRONT_ITEM_HPP
#include "lcd-menu.hpp"
#include <cstdio>
#include <ctime>
#include <poll.h>


class CmenuItemFrontMenu: public CmenuItem {
protected:

public:
  CmenuItemFrontMenu();
  virtual int up(Clcd *lcd);
  virtual int down(Clcd *lcd);
  virtual int left(Clcd *lcd);
  virtual int right(Clcd *lcd);
  virtual int enter(Clcd *lcd);
  virtual int esc(Clcd *lcd);
  virtual void fullEsc();
};

class CmenuItemTimeFoto: public CmenuItemFrontMenu {
protected:

public:
  virtual void screen(Clcd *lcd);
};

class CmenuItemIdds: public CmenuItemFrontMenu {
public:
  virtual void screen(Clcd *lcd);
};

class CmenuItemSendStat: public CmenuItemFrontMenu {
public:
  virtual void screen(Clcd *lcd);
};

class CmenuItemConnectionTest: public CmenuItemFrontMenu {
protected:
  FILE * _app;
  int _run;
  int _appfd;
  int _smig;
  int _progress;
  int _tmp;
  char _buf[17];
public:
  CmenuItemConnectionTest();
  virtual void screen(Clcd *lcd);
  virtual int up(Clcd *lcd);
  virtual int down(Clcd *lcd);
  virtual int left(Clcd *lcd);
  virtual int right(Clcd *lcd);
  virtual int enter(Clcd *lcd);
  virtual int esc(Clcd *lcd);
};
#endif // LCD_MENU_FRONT_ITEM_HPP
