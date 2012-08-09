#include "lcd-menu.hpp"


CdbParam::CdbParam(): _lastPos(0), _lastSize(0) {
}


CmenuItem::CmenuItem(std::string newname): name(newname), refCnt(0) {
}

CmenuItem::~CmenuItem() {
}

CmenuList::CmenuList(std::string newname): CmenuItem(newname) {
  _active=-1;
  _in=false;
}

CmenuList::~CmenuList() {
  unsigned int i;
  for( i=0; i<_list.size(); ++i ) {
    if( _list[i]->refCnt == 1 ) {
      delete _list[i];
    } else {
      --(_list[i]->refCnt);
    }
  }
}

int CmenuList::itemAdd(CmenuItem* item) {
  _list.push_back(item);
  ++(item->refCnt);
  return _list.size()-1;
}

void CmenuList::delItem(int nr) {
}

int CmenuList::up(Clcd *lcd) {
  if( _in ) {
    if( _list[_active]->up(lcd) ) {
      _in=false;
      this->screen(lcd);
    }
  } else {
    if( _active==-1) {
      _active=_list.size()-1;
    } else {
      --_active;
    }
    this->screen(lcd);
  }
  return 0;
}

int CmenuList::down(Clcd *lcd) {
  if( _in ) {
    if( _list[_active]->down(lcd) ) {
      _in=false;
      this->screen(lcd);
    }
  } else {
    if( _active==((int)_list.size())-1 ) {
      _active=-1;
    } else {
      ++_active;
    }
    this->screen(lcd);
  }
  return 0;
}

int CmenuList::left(Clcd *lcd) {
  if( _in ) {
    if( _list[_active]->left(lcd) ) {
      _in=false;
      this->screen(lcd);
    }
  } else {
    return 1;
  }
  return 0;
}

int CmenuList::right(Clcd *lcd) {
  if( _in ) {
    if( _list[_active]->right(lcd) ) {
      _in=false;
      this->screen(lcd);
    }
  } else {
    if( _active==-1 ) {
      return 1;
    } else {
      _in=true;
      _list[_active]->screen(lcd);
    }
  }
  return 0;
}

int CmenuList::enter(Clcd *lcd) {
  if( _in ) {
    if( _list[_active]->enter(lcd) ) {
      _in=false;
      this->screen(lcd);
    }
  } else {
    if( _active==-1 ) {
      return 1;
    } else {
      _in=true;
      _list[_active]->screen(lcd);
    }
  }
  return 0;
}

int CmenuList::esc(Clcd *lcd) {
  if( _in ) {
    if( _list[_active]->esc(lcd) ) {
      _in=false;
      this->screen(lcd);
    }
  } else {
    return 1;
  }
  return 0;
}

void CmenuList::screen(Clcd *lcd) {
  if( _in ) {
    _list[_active]->screen(lcd);
  } else {
    lcd->_lcd[0]=name;
    if( _active==-1 ) {
      lcd->_lcd[1]="..";
    } else {
      lcd->_lcd[1]=">";
      lcd->_lcd[1]+=_list[_active]->name;
    }
    lcd->_refresh=0;
    lcd->_curOn=false;
  }
}

void CmenuList::fullEsc() {
  if( _in ) {
    _list[_active]->fullEsc();
  }
  _in=false;
  _active=-1;
}



CmenuContainer::CmenuContainer(std::string newname): CmenuList(newname) {
}

void CmenuContainer::screen(Clcd *lcd) {
  if( _in ) {
    _list[_active]->screen(lcd);
  } else {
    if( _active==-1 ) {
      lcd->_lcd[0]=name;
      lcd->_lcd[1]="..";
    } else {
      _list[_active]->screen(lcd);
    }
  }
}



CmenuDbParamList::CmenuDbParamList(std::string newname): CmenuItem(newname) {
  _active=-1;
}

CmenuDbParamList::~CmenuDbParamList() {
}

int CmenuDbParamList::itemAdd(std::string name, std::string sect, std::string key) {
  CdbParam param;
  param._key=key;
  param._sect=sect;
  param._name=name;
  _list.push_back(param);
  return _list.size()-1;
}

void CmenuDbParamList::screen(Clcd *lcd) {
  if( _active==-1 ) {
    lcd->_lcd[0]=name;
    lcd->_lcd[1]="..";
  } else {
    std::string val;
    lcd->_lcd[0]=_list[_active]._name;
    val=globalConfig->entry(_list[_active]._sect, _list[_active]._key);
    if( val.size()>16 ) {
      lcd->_refresh=2000;
      if( val.size()!=_list[_active]._lastSize || _list[_active]._lastPos>=val.size() ) {
        _list[_active]._lastPos=0;
        _list[_active]._lastSize=val.size();
      }
      lcd->_lcd[1]=val.substr(_list[_active]._lastPos, 16);
      _list[_active]._lastPos+=16;
    } else {
      lcd->_refresh=0;
      lcd->_lcd[1]=val;
    }
    lcd->_curOn=false;
  }
}

int CmenuDbParamList::up(Clcd *lcd) {
  if( _active==-1) {
    _active=_list.size()-1;
  } else {
    --_active;
  }
  if( _active!=-1 ) {
    _list[_active]._lastSize=0;
  }
  this->screen(lcd);
  return 0;
}

int CmenuDbParamList::left(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuDbParamList::down(Clcd *lcd) {
  if( _active==((int)_list.size())-1 ) {
    _active=-1;
  } else {
    ++_active;
  }
  if( _active!=-1 ) {
    _list[_active]._lastSize=0;
  }
  this->screen(lcd);
  return 0;
}

int CmenuDbParamList::right(Clcd *lcd) {
  return this->down(lcd);
}

int CmenuDbParamList::enter(Clcd *lcd) {
  if( _active==-1 ) {
    return 1;
  } else {
    this->screen(lcd);
    return 0;
  }
}

int CmenuDbParamList::esc(Clcd *lcd) {
  return 1;
}

void CmenuDbParamList::fullEsc() {
  _active=-1;
}



CmenuContainerNoRoot::CmenuContainerNoRoot(CmenuItem* menu, CmenuItem* item, std::string newname):
  CmenuList(newname), _menu(menu) {
  _list.push_back(item);
  _active=0;
  _fastActive=-1;
}
CmenuContainerNoRoot::~CmenuContainerNoRoot() {
  delete _menu;
}

int CmenuContainerNoRoot::fastAdd(CmenuItem* item) {
  _fast.push_back(item);
  ++(item->refCnt);
  return _fast.size()-1;
}
int CmenuContainerNoRoot::fastGoto(int nr) {
  this->fullEsc();
  if( nr>=0 && nr<_fast.size() ) {
    _fastActive=nr;
  }
}
int CmenuContainerNoRoot::up(Clcd *lcd) {
  if( _fastActive!=-1 ) {
    if( _fast[_fastActive]->up(lcd) ) {
      _fastActive=-1;
      this->screen(lcd);
    }
  } else if( _in ) {
    if( _menu->up(lcd) ) {
      _in=false;
      _active=0;
      this->screen(lcd);
    }
  } else {
    if( _active==0) {
      _active=_list.size()-1;
    } else {
      --_active;
    }
    this->screen(lcd);
  }
  return 0;
}

int CmenuContainerNoRoot::down(Clcd *lcd) {
  if( _fastActive!=-1 ) {
    if( _fast[_fastActive]->down(lcd) ) {
      _fastActive=-1;
      this->screen(lcd);
    }
  } else if( _in ) {
    if( _menu->down(lcd) ) {
      _in=false;
      _active=0;
      this->screen(lcd);
    }
  } else {
    if( _active==((int)_list.size())-1 ) {
      _active=0;
    } else {
      ++_active;
    }
    this->screen(lcd);
  }
  return 0;
}

void CmenuContainerNoRoot::fullEsc() {
  if( _fastActive!=-1 ) {
    _fast[_fastActive]->fullEsc();
    _fastActive=-1;
  } else if( _in ) {
    _menu->fullEsc();
  }
  _in=false;
  _active=0;
}

int CmenuContainerNoRoot::left(Clcd *lcd) {
  if( _fastActive!=-1 ) {
    if( _fast[_fastActive]->left(lcd) ) {
      _fastActive=-1;
      this->screen(lcd);
    }
  } else if( _in ) {
    if( _menu->left(lcd) ) {
      _in=false;
      _active=0;
      this->screen(lcd);
    }
  } else {
    return 1;
  }
  return 0;
}

int CmenuContainerNoRoot::right(Clcd *lcd) {
  if( _fastActive!=-1 ) {
    if( _fast[_fastActive]->right(lcd) ) {
      _fastActive=-1;
      this->screen(lcd);
    }
  } else if( _in ) {
    if( _menu->right(lcd) ) {
      _in=false;
      _active=0;
      this->screen(lcd);
    }
  } else {
    _in=true;
    _menu->screen(lcd);
  }
  return 0;
}

int CmenuContainerNoRoot::enter(Clcd *lcd) {
  if( _fastActive!=-1 ) {
    if( _fast[_fastActive]->enter(lcd) ) {
      _fastActive=-1;
      this->screen(lcd);
    }
  } else if( _in ) {
    if( _menu->enter(lcd) ) {
      _in=false;
      _active=0;
      this->screen(lcd);
    }
  } else {
    _in=true;
    _menu->screen(lcd);
  }
  return 0;
}

int CmenuContainerNoRoot::esc(Clcd *lcd) {
  if( _fastActive!=-1 ) {
    if( _fast[_fastActive]->esc(lcd) ) {
      _fastActive=-1;
      this->screen(lcd);
    }
  } else if( _in ) {
    if( _menu->esc(lcd) ) {
      _in=false;
      _active=0;
      this->screen(lcd);
    }
  } else {
    return 1;
  }
  return 0;
}

void CmenuContainerNoRoot::screen(Clcd *lcd) {
  if( _fastActive!=-1 ) {
    _fast[_fastActive]->screen(lcd);
  } else if( _in ) {
    _menu->screen(lcd);
  } else {
    _list[_active]->screen(lcd);
  }
}



