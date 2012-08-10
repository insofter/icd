/**
 * @file lcd-menu.hpp
 * @author Piotr Mika
 * @date VIII 2012r.
 * Basic classes for menu: main interface, lists etc.
 */

#ifndef LCD_MENU_HPP
#define LCD_MENU_HPP
#include <vector>
#include <string>
#include "db-config.h"
#include "sqlite3cc.h"
#include "lcd-driver.hpp"

extern icd::config *globalConfig;


/**
 * Struct of database parameters.
 * Struct describes values which can be get from database.
 */
struct CdbParam {
  /**
   * Name, which will be shown on 1st linie.
   */
  std::string _name;

  /**
   * Section from config_section table.
   */
  std::string _sect;

  /**
   * Key from config table.
   */
  std::string _key;

  /**
   * For long parameters, more than 16 characters.
   */
  unsigned int _lastPos;

  /**
   * For long parameters, more than 16 characters.
   */
  unsigned int _lastSize;

  /**
   * Constructor, sets 0.
   */
  CdbParam();
};


/**
 * Interface class. It describes virtual menu item. 
 * It should be inherited by every other menu item.
 */
class CmenuItem {
public:
  /**
   * Name of item. Can be used by listings.
   */
  std::string name;
  /**
   * Reference counter. Allows to give item's pointer to many places 
   * in menu and to use destructors.
   */

  int refCnt;
  /* Constructor. Sets name.
   * @param newname Name of item.
   */

  CmenuItem(std::string newname);
  /**
   * Destructor. Have to be virtual.
   */
  virtual ~CmenuItem();

  /**
   * Key up function. Menu will run it while pressing key.
   * @param lcd Pointer to structure which describes LCD screen.
   * @return Exitcode: Non-0 means return from that menu option to higher level.
   */
  virtual int up(Clcd *lcd)=0;

  /**
   * Key down function. Menu will run it while pressing key.
   * @param lcd Pointer to structure which describes LCD screen.
   * @return Exitcode: Non-0 means return from that menu option to higher level.
   */
  virtual int down(Clcd *lcd)=0;

  /**
   * Left key function. Menu will run it while pressing key.
   * @param lcd Pointer to structure which describes LCD screen.
   * @return Exitcode: Non-0 means return from that menu option to higher level.
   */
  virtual int left(Clcd *lcd)=0;

  /**
   * Right key function. Menu will run it while pressing key.
   * @param lcd Pointer to structure which describes LCD screen.
   * @return Exitcode: Non-0 means return from that menu option to higher level.
   */
  virtual int right(Clcd *lcd)=0;

  /**
   * Enter key function. Menu will run it while pressing key.
   * @param lcd Pointer to structure which describes LCD screen.
   * @return Exitcode: Non-0 means return from that menu option to higher level.
   */
  virtual int enter(Clcd *lcd)=0;

  /**
   * Escape key function. Menu will run it while pressing key.
   * @param lcd Pointer to structure which describes LCD screen.
   * @return Exitcode: Non-0 means return from that menu option to higher level.
   */
  virtual int esc(Clcd *lcd)=0;

  /**
   * Global return. Menu will run it after timeout. It means that menu should 
   * run it for lower level item if it is active and set their state like returning 
   * for higher level.
   */
  virtual void fullEsc()=0;

  /**
   * Screen function. Sets values in LCD structure. Should be used in key functions
   * ( this->screen(lcd) ). It have to set correct screen ALWAYS because there is 
   * no other information that this menu item is used now than running screen or
   * key functions.
   * @param lcd Pointer to structure which describes LCD screen
   */
  virtual void screen(Clcd *lcd)=0;
};

/**
 * Class implements list of menu items. It shows List name in first line and item name with ">" in second.
 */
class CmenuList: public CmenuItem {
protected:
  int _active;
  bool _in;
  std::vector< CmenuItem* > _list;
public:
  CmenuList(std::string newname);
  virtual ~CmenuList();

  /**
   * Adds new item to menu. It can be everything compatible with CmenuItem 
   * interface. 
   * @param item Pointer to menu item, for using like itemAdd( new item ) or putting one item in few places.
   * @return Number of item in list.
   */
  int itemAdd(CmenuItem* item);

  /** 
   * Deletes item. NOT IMPLEMENTED.
   * @param nr Number of item returned by itemAdd.
   */
  void delItem(int nr);
  virtual int up(Clcd *lcd);
  virtual int down(Clcd *lcd);
  virtual int left(Clcd *lcd);
  virtual int right(Clcd *lcd);
  virtual int enter(Clcd *lcd);
  virtual int esc(Clcd *lcd);
  virtual void screen(Clcd *lcd);
  virtual void fullEsc();
};

/**
 * List of menu items with different showing. It always run item->screen(). Do not show it's name.
 */
class CmenuContainer: public CmenuList {
public:
  CmenuContainer(std::string newname);
  virtual void screen(Clcd *lcd);
};

/**
 * List of database parameters, read-only.
 */
class CmenuDbParamList: public CmenuItem {
protected:
  std::vector<CdbParam> _list;
  int _active;
public:
  CmenuDbParamList(std::string newname);
  virtual ~CmenuDbParamList();

  /**
   * Adds new item to list.
   * @param name Name of parameter, for screen's first line.
   * @param sect Database section for config_section table.
   * @param key Database parameter key for config table.
   * @return Number of item in list.
   */
  int itemAdd(std::string name, std::string sect, std::string key);
  virtual void screen(Clcd *lcd);
  virtual int up(Clcd *lcd);
  virtual int left(Clcd *lcd);
  virtual int down(Clcd *lcd);
  virtual int right(Clcd *lcd);
  virtual int enter(Clcd *lcd);
  virtual int esc(Clcd *lcd);
  virtual void fullEsc();
};

/**
 * Class implements list of menu items. It always run item->screen(). It is used
 * for main menu so it has no "root" item and can't be empty. It also has list of 
 * "shortcut" items.
 */
class CmenuContainerNoRoot: public CmenuList {
protected:
  int _fastActive;
  std::vector< CmenuItem* > _fast;
  CmenuItem* _menu;
public:
  CmenuContainerNoRoot(CmenuItem* menu, CmenuItem* item, std::string newname="");
  virtual ~CmenuContainerNoRoot();
  /**
   * Add item to shortcuts.
   * @param item Pointer to menu item.
   * @return Number of item in list.
   */
  int fastAdd(CmenuItem* item);

  /**
   * Switches to shortcut nr. It runs fullEsc() before.
   * @param nr Number of item in list.
   */
  int fastGoto(int nr);
  virtual int up(Clcd *lcd);
  virtual int down(Clcd *lcd);
  virtual void fullEsc();
  virtual int left(Clcd *lcd);
  virtual int right(Clcd *lcd);
  virtual int enter(Clcd *lcd);
  virtual int esc(Clcd *lcd);
  virtual void screen(Clcd *lcd);
};




#endif // LCD_MENU_HPP
