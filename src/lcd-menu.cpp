#include "lcd-menu.hpp"
#include <cmath>


CdbParam::CdbParam(): _lastPos(0), _lastSize(0), _editMode(readOnly) {
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
    lcd->_cur._car=Ccur::none;
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
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CmenuDbParamList::CmenuDbParamList(std::string newname): CmenuItem(newname) {
  _active=-1;
  _editing=false;
}

CmenuDbParamList::~CmenuDbParamList() {
}

int CmenuDbParamList::itemAdd(std::string name, std::string sect, 
    std::string key, CdbParam::editMode _editMode) {
  CdbParam param;
  param._key=key;
  param._sect=sect;
  param._name=name;
  param._editMode=_editMode;
  _list.push_back(param);
  return _list.size()-1;
}

void CmenuDbParamList::screen(Clcd *lcd) {
  if( _active==-1 ) {
    lcd->_lcd[0]=name;
    lcd->_lcd[1]="..";
  } else if( _editing ) {//reparse tmp values to string with cursor
    lcd->_lcd[0]=_list[_active]._name;
    switch( _list[_active]._editMode ) {
      case CdbParam::editBool:
        if( _tmpi[0]==1 ) {
          lcd->_lcd[1]="yes";
        } else {
          lcd->_lcd[1]="no";
        }
        break;
      case CdbParam::editInt:
        break;
      case CdbParam::editIp:
        char x[16];
        sprintf(x,"%03i.%03i.%03i.%03i", _tmpi[0],  _tmpi[1], _tmpi[2], _tmpi[3] );
        lcd->_lcd[1]=x;
        lcd->_refresh=0;
        lcd->_cur._y=1;
        lcd->_cur._x=_tmpp+(_tmpp/3);
        break;
      case CdbParam::editText:
        break;
    }
    lcd->_cur._car=Ccur::blinkLine;
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

      int ilCzesci=std::ceil( ((float)_list[_active]._lastSize)/16 );
      int ktora=std::floor( _list[_active]._lastPos/16 )+1;

      lcd->_cur._x=(( ktora*16 ) / ilCzesci)-1;

      lcd->_cur._y=1;
      lcd->_cur._car=Ccur::line;

      _list[_active]._lastPos+=16;

    } else {
      lcd->_refresh=0;
      lcd->_lcd[1]=val;
      lcd->_cur._car=Ccur::none;
    }
  }
}

int CmenuDbParamList::up(Clcd *lcd) {
  if( _editing ) {
    switch( _list[_active]._editMode ) {
      case CdbParam::editBool:
        if( _tmpi[0]==1 ) {
          _tmpi[0]=0;
        } else {
          _tmpi[0]=1;
        }
        break;
      case CdbParam::editInt:
        break;
      case CdbParam::editIp:
        //_tmpp/3 == which number is edited
        //_tmpp%3 == which digit  is edited
        if( _tmpp%3 == 0 ) {
          _tmpi[ _tmpp/3 ]+=100;
        } else if( _tmpp%3 == 1 ) {
          _tmpi[ _tmpp/3 ]+=10;
        } else {
          _tmpi[ _tmpp/3 ]+=1;
        }
        if( _tmpi[ _tmpp/3 ] > 255 ) {//overflow 
          _tmpi[ _tmpp/3 ]=0;
        }
        break;
      case CdbParam::editText:
        break;
    }
  } else {
    if( _active==-1) {
      _active=_list.size()-1;
    } else {
      --_active;
    }
    if( _active!=-1 ) {
      _list[_active]._lastSize=0;
    }
  }
  this->screen(lcd);
  return 0;
}

int CmenuDbParamList::left(Clcd *lcd) {
  if( _editing ) {
    switch( _list[_active]._editMode ) {
      case CdbParam::editBool:
        break;
      case CdbParam::editInt:
        break;
      case CdbParam::editIp:
        if( _tmpp == 0 ) {
          _tmpp=11;
        } else {
          --_tmpp;
        }
        break;
      case CdbParam::editText:
        break;
    }
    this->screen(lcd);
    return 0;
  } else {
    return this->up(lcd);
  }
}

int CmenuDbParamList::down(Clcd *lcd) {
  if( _editing ) {
    switch( _list[_active]._editMode ) {
      case CdbParam::editBool:
        if( _tmpi[0]==1 ) {
          _tmpi[0]=0;
        } else {
          _tmpi[0]=1;
        }
        break;
      case CdbParam::editInt:
        break;
      case CdbParam::editIp:
        //_tmpp/3 == which number is edited
        //_tmpp%3 == which digit  is edited
        if( _tmpp%3 == 0 ) {
          _tmpi[ _tmpp/3 ]-=100;
        } else if( _tmpp%3 == 1 ) {
          _tmpi[ _tmpp/3 ]-=10;
        } else {
          _tmpi[ _tmpp/3 ]-=1;
        }
        if( _tmpi[ _tmpp/3 ] < 0 ) {//overflow 
          _tmpi[ _tmpp/3 ]=255;
        }
        break;
      case CdbParam::editText:
        break;
    }
  } else {
    if( _active==((int)_list.size())-1 ) {
      _active=-1;
    } else {
      ++_active;
    }
    if( _active!=-1 ) {
      _list[_active]._lastSize=0;
    }
  }
  this->screen(lcd);
  return 0;
}

int CmenuDbParamList::right(Clcd *lcd) {
  if( _editing ) {
    switch( _list[_active]._editMode ) {
      case CdbParam::editBool:
        break;
      case CdbParam::editInt:
        break;
      case CdbParam::editIp:
        if( _tmpp == 11 ) {
          _tmpp=0;
        } else {
          ++_tmpp;
        }
        break;
      case CdbParam::editText:
        break;
    }
    this->screen(lcd);
    return 0;
  } else {
    return this->down(lcd);
  }
}

int CmenuDbParamList::enter(Clcd *lcd) {
  if( _active==-1 ) {
    return 1;
  } else {
    if( _editing ) {
      switch( _list[_active]._editMode ) {
        case CdbParam::editBool:
          if( _tmpi[0]==1 ) {
            globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, "yes" );
          } else {
            globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, "no" );
          }
          break;
        case CdbParam::editInt:
          break;
        case CdbParam::editIp:
          char c[16];
          sprintf(c,"%i.%i.%i.%i", _tmpi[0],  _tmpi[1], _tmpi[2], _tmpi[3] );
          globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, c );
          break;
        case CdbParam::editText:
          break;
      }
      _editing=false;
    } else if( _list[_active]._editMode!=CdbParam::readOnly ) {
      switch( _list[_active]._editMode ) {
        case CdbParam::editBool:
          if( ( globalConfig->entry( _list[_active]._sect,
                  _list[_active]._key) ).compare( "yes" ) == 0 ) {
            _tmpi[0]=1;
          } else {
            _tmpi[0]=0;
          }
          break;
        case CdbParam::editInt:
          break;
        case CdbParam::editIp:
          _tmpi[0]=0;
          _tmpi[1]=0;
          _tmpi[2]=0;
          _tmpi[3]=0;
          sscanf( ( globalConfig->entry( _list[_active]._sect, 
                  _list[_active]._key) ).c_str(), "%i.%i.%i.%i", 
              _tmpi, _tmpi+1, _tmpi+2, _tmpi+3 );
          _tmpp=0; //string from db is parsed to 4 integers, editpos=0;
          break;
        case CdbParam::editText:
          break;
        default:
          std::cerr << "bug w CmenuDbParamList::enter" << std::endl;
          exit(1);
          break;
      }
      _editing=true;
    }
    this->screen(lcd);
    return 0;
  }
}

int CmenuDbParamList::esc(Clcd *lcd) {
  _editing=false;
  _active=-1;
  return 1;
}

void CmenuDbParamList::fullEsc() {
  _active=-1;
  _editing=false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CmenuContainerNoRoot::CmenuContainerNoRoot(CmenuItem* menu, CmenuItem* item, std::string newname):
  CmenuList(newname), _menu(menu) {
    ++(_menu->refCnt);
    ++(item->refCnt);
    _list.push_back(item);
    _active=0;
    _fastActive=-1;
  }
CmenuContainerNoRoot::~CmenuContainerNoRoot() {
  if( _menu->refCnt == 1 ) {
    delete _menu;
  } else {
    --(_menu->refCnt);
  }
  unsigned int i;
  for( i=0; i<_list.size(); ++i ) {
    if( _fast[i]->refCnt == 1 ) {
      delete _fast[i];
    } else {
      --(_fast[i]->refCnt);
    }
  }
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



