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
#include <cmath>
#include <poll.h>
#include <fstream>
#ifdef DESKTOP//different headers on arm and desktop,
              //use -DDESKTOP durng compilation on desktop 
  #include <unistd.h>
#endif


extern sqlite3cc::conn *globalLiveDb;
extern sqlite3cc::conn *globalDataDb;


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
 * Default screen with date, time and counter and daily Σ.
 */
class CmenuItemTimeFoto: public CmenuItemFrontMenu {
protected:
  int _a;
  sqlite3cc::stmt *hour;
  sqlite3cc::stmt *sum;

public:
  /**
   * Constructor
   * @param a Id of first detector
   * @param b Id of second detector
   */
  CmenuItemTimeFoto(int a);
  virtual ~CmenuItemTimeFoto();

  virtual void screen(Clcd *lcd);
};

/**
 * Default screen with date, time and two counters.
 */
class CmenuItemDoubleTimeFoto: public CmenuItemFrontMenu {
protected:
  int _a;
  int _b;
  sqlite3cc::stmt *stmt;

public:
  /**
   * Constructor
   * @param a Id of first detector
   * @param b Id of second detector
   */
  CmenuItemDoubleTimeFoto(int a, int b);
  virtual ~CmenuItemDoubleTimeFoto();

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
 * Show first line from file
 */
class CmenuItemFileParam: public CmenuItemFrontMenu {
private:
  std::string _file;
  int _lastPos;
  int _lastSize; 
public:
  CmenuItemFileParam(std::string name, std::string file);
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
  enum appState {
    idle,
    running,
    done
  };
  FILE * _app;
  appState _run;
  bool _autoStart;
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
   * @param autoStart Don't go to idle state, start immediately.
   */
  CmenuItemRunTestApp(const std::string & name, const std::string & info,
      const std::string & path, const std::string & head1, 
      const std::string & head2, bool autoStart=false );
  virtual void screen(Clcd *lcd);
  virtual int up(Clcd *lcd);
  virtual int down(Clcd *lcd);
  virtual int left(Clcd *lcd);
  virtual int right(Clcd *lcd);
  virtual int enter(Clcd *lcd);
  virtual int esc(Clcd *lcd);
};
#endif // LCD_MENU_FRONT_ITEM_HPP
