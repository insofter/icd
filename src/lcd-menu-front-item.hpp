/**
 * @file lcd-menu-front-item.hpp
 * @author Piotr Mika
 * @date VIII 2012r.
 * Classes for front menu.
 */

#ifndef LCD_MENU_FRONT_ITEM_HPP
#define LCD_MENU_FRONT_ITEM_HPP
#include "lcd-menu.hpp"
#include <cstdio>
#include <ctime>
#include <poll.h>

/**
 * Class describes interface for items in front menu. For example: date and time, Counters stats etc.
 * By default that items don't accept keys and always return 1;
 */
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

/**
 * Default screen with date, time and two counters.
 * TODO param for AB or CD.
 */
class CmenuItemTimeFoto: public CmenuItemFrontMenu {
protected:

public:
  virtual void screen(Clcd *lcd);
};

/**
 * Screen with info about IDD and IDS
 */
class CmenuItemIdds: public CmenuItemFrontMenu {
public:
  virtual void screen(Clcd *lcd);
};

/**
 * Show one parameter from database
 */
class CmenuItemDbParam: public CmenuItemFrontMenu {
private:
  CdbParam _param;
public:
  CmenuItemDbParam(std::string name, std::string sect, std::string key);
  virtual void screen(Clcd *lcd);
};



/**
 * This class is used to run some application and to show output. It accept keys.
 * Application output: number of percent, while working and oneline status (16 characters).
 * Percent writing should check if write will not block. Percent numbers should be separated by 
 * tab, newline or carrige return. Other characters ( space too !! ) means status message. Polish language
 * special characters can be used in UTF-8 encoding (longest correct string with only polish special 
 * characters has 32 bytes).
 */
class CmenuItemRunTestApp: public CmenuItemFrontMenu {
protected:
  FILE * _app;
  int _run;
  int _appfd;
  int _smig;
  int _progress;
  int _tmp;
  char _buf[33];
  std::string _name;
  std::string _info;
  std::string _path;
  std::string _head1;
  std::string _head2;
public:
  /**
   * Constructor.
   * @param name Name of application for screen first line.
   * @param info Screen's second line before start, for example ">run me!".
   * @param path Correct path for applicaton, used by popen() function.
   * @param head1 First line on screen while running, switched with head2 to blinking.
   * @param head2 First line on screen while running, switched with head1 to blinking.
   */
  CmenuItemRunTestApp(std::string name, std::string info,
      std::string path, std::string head1, std::string head2);
  virtual void screen(Clcd *lcd);
  virtual int up(Clcd *lcd);
  virtual int down(Clcd *lcd);
  virtual int left(Clcd *lcd);
  virtual int right(Clcd *lcd);
  virtual int enter(Clcd *lcd);
  virtual int esc(Clcd *lcd);
};
#endif // LCD_MENU_FRONT_ITEM_HPP
