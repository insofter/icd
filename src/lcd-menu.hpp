#ifndef LCD_MENU_HPP
#define LCD_MENU_HPP

#include <vector>
#include <string>
#include "db-config.h"
#include "sqlite3cc.h"
#include "lcd-driver.hpp"

extern icd::config *globalConfig;



struct CdbParam {
  std::string _name;
  std::string _sect;
  std::string _key;
  unsigned int _lastPos;
  unsigned int _lastSize;
  CdbParam();
};



class CmenuItem {
public:
  std::string name;
  int refCnt;
  CmenuItem(std::string newname);
  virtual ~CmenuItem();
  virtual int up(Clcd *lcd)=0;
  virtual int down(Clcd *lcd)=0;
  virtual int left(Clcd *lcd)=0;
  virtual int right(Clcd *lcd)=0;
  virtual int enter(Clcd *lcd)=0;
  virtual int esc(Clcd *lcd)=0;
  virtual void fullEsc()=0;
  virtual void screen(Clcd *lcd)=0;
};

class CmenuList: public CmenuItem {
protected:
  int _active;
  bool _in;
  std::vector< CmenuItem* > _list;
public:
  CmenuList(std::string newname);
  virtual ~CmenuList();
  int itemAdd(CmenuItem* item);
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

class CmenuContainer: public CmenuList {
public:
  CmenuContainer(std::string newname);
  virtual void screen(Clcd *lcd);
};

class CmenuDbParamList: public CmenuItem {
protected:
  std::vector<CdbParam> _list;
  int _active;
public:
  CmenuDbParamList(std::string newname);
  virtual ~CmenuDbParamList();
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

class CmenuContainerNoRoot: public CmenuList {
protected:
  int _fastActive;
  std::vector< CmenuItem* > _fast;
  CmenuItem* _menu;
public:
  CmenuContainerNoRoot(CmenuItem* menu, CmenuItem* item, std::string newname="");
  virtual ~CmenuContainerNoRoot();
  int fastAdd(CmenuItem* item);
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
