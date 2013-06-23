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
  _typingText=false;
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
  if( _typingText ) {
    _typeText.screen( lcd );
  } else {
    if( _active==-1 ) {
      lcd->_lcd[0]=name;
      lcd->_lcd[1]="..";
      lcd->_cur._car=Ccur::none;
    } else if( _editing ) {//reparse tmp values to string with cursor
      lcd->_lcd[0]=_list[_active]._name;
      lcd->_cur._y=1;
      char x[16];
      switch( _list[_active]._editMode ) {
        case CdbParam::editBool:
          lcd->_cur._x=0;
          if( _tmpi[0]==1 ) {
            lcd->_lcd[1]="yes";
          } else {
            lcd->_lcd[1]="no";
          }
          break;
        case CdbParam::editInt:
          sprintf(x,"%09i", _tmpi[0]);
          lcd->_lcd[1]=x;
          lcd->_cur._x=_tmpp;
          break;
        case CdbParam::editIp:
          sprintf(x,"%03i.%03i.%03i.%03i", _tmpi[0],  _tmpi[1], _tmpi[2], _tmpi[3] );
          lcd->_lcd[1]=x;
          lcd->_cur._x=_tmpp+(_tmpp/3);
          break;
        case CdbParam::editMac:
          sprintf(x,"%02X%02X%02X%02X%02X%02X",
              _tmpi[0],  _tmpi[1], _tmpi[2], _tmpi[3], _tmpi[4], _tmpi[5] );
          lcd->_lcd[1]=x;
          lcd->_cur._x=_tmpp;
          break;
        case CdbParam::editText:
          if( _tmpp>15 ) {
            lcd->_lcd[1]=_tmps.substr( _tmpp-15, 15);
            lcd->_cur._x=15;
          } else {
            lcd->_lcd[1]=_tmps;
            lcd->_cur._x=_tmpp;
          }
          lcd->_lcd[1]+="¶";
          break;
      }
      lcd->_refresh=0;
      lcd->_cur._car=Ccur::blinkLine;
    } else {
      std::string val;
      lcd->_lcd[0]=_list[_active]._name;
      val=globalConfig->entry(_list[_active]._sect, _list[_active]._key);
      if( val.size()>16 ) {
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
        lcd->_lcd[1]=val;
        lcd->_cur._car=Ccur::none;
      }
      lcd->_refresh=2000;
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
        _tmpi[0]+=std::pow(10, (8-_tmpp) );
        if( _tmpi[0] > 999999999 ) {
          _tmpi[0]=0;
        }
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
      case CdbParam::editMac:
        //_tmpp/2 == which number is edited
        //_tmpp%2 == which digit  is edited
        if( _tmpp%2 == 0 ) {
          _tmpi[ _tmpp/2 ]+=16;
        } else {
          _tmpi[ _tmpp/2 ]+=1;
        }
        if( _tmpi[ _tmpp/2 ] > 255 ) {//overflow 
          _tmpi[ _tmpp/2 ]=0;
        }
        break;
      case CdbParam::editText:
        if( _typingText ) {
          _typeText.up( lcd );
        } else {
          if( _tmps.size()==_tmpp )
          {
            _typeText.gotoChar( 13 );
          } else {
            _typeText.gotoChar( _tmps[ _tmpp ] );
          }
          _typingText=true;
        }
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
        if( _tmpp == 0 ) {
          _tmpp=8;
        } else {
          --_tmpp;
        }
        break;
      case CdbParam::editIp:
        if( _tmpp == 0 ) {
          _tmpp=11;
        } else {
          --_tmpp;
        }
        break;
      case CdbParam::editMac:
        if( _tmpp == 0 ) {
          _tmpp=11;
        } else {
          --_tmpp;
        }
        break;
      case CdbParam::editText:
        if( _typingText ) {
          _typeText.left( lcd );
        } else {

          if( _tmpp == 0 ) {
            _tmpp=_tmps.size();
          } else {
            --_tmpp;
          }
        }
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
        _tmpi[0]-=std::pow(10, (8-_tmpp) );
        if( _tmpi[0] < 0 ) {
          _tmpi[0]=999999999;
        }
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
      case CdbParam::editMac:
        //_tmpp/2 == which number is edited
        //_tmpp%2 == which digit  is edited
        if( _tmpp%2 == 0 ) {
          _tmpi[ _tmpp/2 ]-=16;
        } else {
          _tmpi[ _tmpp/2 ]-=1;
        }
        if( _tmpi[ _tmpp/2 ] < 0 ) {//overflow 
          _tmpi[ _tmpp/2 ]=255;
        }
        break;
      case CdbParam::editText:
        if( _typingText ) {
          _typeText.down( lcd );
        } else {
          if( _tmps.size()==_tmpp )
          {
            _typeText.gotoChar( 13 );
          } else {
            _typeText.gotoChar( _tmps[ _tmpp ] );
          }
          _typingText=true;
        }
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
        if( _tmpp == 8 ) {
          _tmpp=0;
        } else {
          ++_tmpp;
        }
        break;
      case CdbParam::editIp:
        if( _tmpp == 11 ) {
          _tmpp=0;
        } else {
          ++_tmpp;
        }
        break;
      case CdbParam::editMac:
        if( _tmpp == 11 ) {
          _tmpp=0;
        } else {
          ++_tmpp;
        }
        break;
      case CdbParam::editText:
        if( _typingText ) {
          _typeText.right( lcd );
        } else {
          if( _tmpp == _tmps.size() ) {
            _tmpp=0;
          } else {
            ++_tmpp;
          }
        }
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
      if( _typingText ) {
        char x = _typeText.enter( lcd );
        if( x == 13 ) {
          if( _tmpp<_tmps.size() ) {
            _tmps.erase( _tmpp );
          }
        } else {
          if( _tmpp==_tmps.size() ) {
            _tmps+=x;
          } else {
            _tmps[ _tmpp ] = x;
          }
          ++_tmpp;
        }
        _typingText=false;
      } else {
        char c[32];
        switch( _list[_active]._editMode ) {
          case CdbParam::editBool:
            if( _tmpi[0]==1 ) {
              globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, "yes" );
            } else {
              globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, "no" );
            }
            break;
          case CdbParam::editInt:
            sprintf(c,"%i", _tmpi[0]);
            globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, c );
            break;
          case CdbParam::editIp:
            sprintf(c,"%i.%i.%i.%i", _tmpi[0],  _tmpi[1], _tmpi[2], _tmpi[3] );
            globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, c );
            break;
          case CdbParam::editMac:
            sprintf(c,"%02X:%02X:%02X:%02X:%02X:%02X",
                _tmpi[0],  _tmpi[1], _tmpi[2], _tmpi[3], _tmpi[4], _tmpi[5] );
            globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, c );
            break;
          case CdbParam::editText:
            globalConfig->set_entry( _list[_active]._sect, _list[_active]._key, _tmps );
            break;
        }
        _editing=false;
      }
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
          _tmpi[0]=atoi( (globalConfig->entry( _list[_active]._sect,
                  _list[_active]._key)).c_str() );
          _tmpp=8;
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
        case CdbParam::editMac:
          _tmpi[0]=0;
          _tmpi[1]=0;
          _tmpi[2]=0;
          _tmpi[3]=0;
          _tmpi[4]=0;
          _tmpi[5]=0;
          sscanf( ( globalConfig->entry( _list[_active]._sect, 
                  _list[_active]._key) ).c_str(), "%X:%X:%X:%X:%X:%X", 
              _tmpi, _tmpi+1, _tmpi+2, _tmpi+3, _tmpi+4, _tmpi+5 );
          _tmpp=0; //string from db is parsed to 6 hexs, editpos=0;
          break;
        case CdbParam::editText:
          _tmps=globalConfig->entry( _list[_active]._sect, _list[_active]._key);
          _tmpp=0;
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
  if( _editing ) {
    if( _typingText ) {
      if( _typeText.esc( lcd )!=0 ) {
        _typingText=false;
        this->screen(lcd);
      }
    } else {
      _editing=false;
      this->screen(lcd);
    }
    return 0;
  } else {
    _active=-1;
    return 1;
  }
}

void CmenuDbParamList::fullEsc() {
  _active=-1;
  _editing=false;
  _typingText=false;
  _typeText.fullEsc();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CmenuAppParamList::CmenuAppParamList(std::string newname): CmenuItem(newname) {
  _active=-1;
}

CmenuAppParamList::~CmenuAppParamList() {
}

int CmenuAppParamList::itemAdd(std::string name, std::string cmd) {
  CappParam item;
  item._name=name;
  item._cmd=cmd;
  _list.push_back(item);
  return _list.size()-1;
}

void CmenuAppParamList::screen(Clcd *lcd) {
  if( _active==-1 ) {
    lcd->_lcd[0]=name;
    lcd->_lcd[1]="..";
  } else {
    char buf[128];
    if( _check <= 0 || val.size() == 0 ) {
      FILE * cmd_out=popen( _list[_active]._cmd.c_str() , "r" );

      if( cmd_out && fgets( buf, 128, cmd_out ) ) {
        int i=0;
        while( buf[i]!=0 && buf[i]!='\n' ) {//find end and remove \n
          ++i;
        }
        buf[i]=0;
        val=buf;
      } else {
        val="--Err";
      }
      pclose( cmd_out );
      _check=5;
    } else {
      --_check;
    }

    lcd->_lcd[0]=_list[_active]._name;
    if( val.size()>16 ) {
      if( val.size()!=_lastSize || _lastPos>=val.size() ) {
        _lastPos=0;
        _lastSize=val.size();
      }
      lcd->_lcd[1]=val.substr(_lastPos, 16);

      int ilCzesci=std::ceil( ((float)_lastSize)/16 );
      int ktora=std::floor( _lastPos/16 )+1;

      lcd->_cur._x=(( ktora*16 ) / ilCzesci)-1;

      lcd->_cur._y=1;
      lcd->_cur._car=Ccur::line;

      _lastPos+=16;

    } else {
      lcd->_lcd[1]=val;
      lcd->_cur._car=Ccur::none;
    }
    lcd->_refresh=2000;
  }
}

int CmenuAppParamList::up(Clcd *lcd) {
  if( _active==-1) {
    _active=_list.size()-1;
  } else {
    --_active;
  }
  if( _active!=-1 ) {
    _lastSize=0;
    _check=0;
  }
  this->screen(lcd);
  return 0;
}

int CmenuAppParamList::left(Clcd *lcd) {
  return this->up(lcd);
}

int CmenuAppParamList::down(Clcd *lcd) {
  if( _active==((int)_list.size())-1 ) {
    _active=-1;
  } else {
    ++_active;
  }
  if( _active!=-1 ) {
    _lastSize=0;
    _check=0;
  }
  this->screen(lcd);
  return 0;
}

int CmenuAppParamList::right(Clcd *lcd) {
  return this->down(lcd);
}

int CmenuAppParamList::enter(Clcd *lcd) {
  if( _active==-1 ) {
    return 1;
  } else {
    this->screen(lcd);
    return 0;
  }
}

int CmenuAppParamList::esc(Clcd *lcd) {
  return 1;
}

void CmenuAppParamList::fullEsc() {
  _active=-1;
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
  for( i=0; i<_fast.size(); ++i ) {
    if( _fast[i]->refCnt == 1 ) {
      delete _fast[i];
    } else {
      --(_fast[i]->refCnt);
    }
  }
  clearCounters();
}


int CmenuContainerNoRoot::fastAdd(CmenuItem* item) {
  _fast.push_back(item);
  ++(item->refCnt);
  return _fast.size()-1;
}

int CmenuContainerNoRoot::counterAdd(CmenuItem* item) {
  _counters.push_back(item);
  ++(item->refCnt);
  _active=-1;
  _counterActive=0;
  return _counters.size()-1;
}

void CmenuContainerNoRoot::clearCounters() {
  unsigned int i;
  for( i=0; i<_counters.size(); ++i ) {
    if( _counters[i]->refCnt == 1 ) {
      delete _counters[i];
    } else {
      --(_counters[i]->refCnt);
    }
  }
  _counters.clear();
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
  } else if( _in ) {//in menu
    if( _menu->up(lcd) ) {
      _in=false;
      if( _counters.size()>0 ) {//return to counters list
        _active=-1;
        _counterActive=0;
      } else {//return to normal list
        _active=0;
      }
      this->screen(lcd);
    }
  } else {//in lists
    if( _active==0) {//up end of list
      if( _counters.size()>0 ) {//goto counters last
        _active=-1;
        _counterActive=_counters.size()-1;
      } else {// no counters, goto list last
        _active=_list.size()-1;
      }
    } else if( _active==-1 ) {//in counters
      if( _counterActive==0 ) {//end of counters, goto last list 
        _active=_list.size()-1;
      } else {//--counters
        --_counterActive;
      }
    } else {// --list
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
  } else if( _in ) {//in menu
    if( _menu->up(lcd) ) {
      _in=false;
      if( _counters.size()>0 ) {//return to counters list
        _active=-1;
        _counterActive=0;
      } else {//return to normal list
        _active=0;
      }
      this->screen(lcd);
    }
  } else {//in lists
    if( _active==((int)_list.size())-1 ) {//down end of list
      if( _counters.size()>0 ) {//goto counters last
        _active=-1;
        _counterActive=0;//_counters.size()-1;
      } else {// no counters, goto list first
        _active=0;//_list.size()-1;
      }
    } else if( _active==-1 ) {//in counters
      if( _counterActive==((int)_counters.size())-1 ) {//end of counters, goto last list 
        _active=_list.size()-1;
      } else {//++counters
        ++_counterActive;
      }
    } else {// ++list
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
  if( _counters.size()>0 ) {
    _counterActive=0;
    _active=-1;
  } else {
    _active=0;
  }
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

      if( _counters.size()>0 ) {
        _counterActive=0;
        _active=-1;
      } else {
        _active=0;
      }
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
      if( _counters.size()>0 ) {
        _counterActive=0;
        _active=-1;
      } else {
        _active=0;
      }
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
      if( _counters.size()>0 ) {
        _counterActive=0;
        _active=-1;
      } else {
        _active=0;
      }
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
      if( _counters.size()>0 ) {
        _counterActive=0;
        _active=-1;
      } else {
        _active=0;
      }
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
    if( _active==-1 ) {
      _counters[_counterActive]->screen(lcd);
    } else {
      _list[_active]->screen(lcd);
    }
  }
}

//////////////////////////////////////////////////////
CmenuTypeText::CmenuTypeText(): CmenuItem("") {
  //0123456789012
  _tab[0]="ABCDEFGHIJKLM";
  _tab[1]="NOPQRSTUVWXYZ";
  _tab[2]="abcdefghijklm";
  _tab[3]="nopqrstuvwxyz";
  _tab[4]=" 0123456789?¶";
  _tab[5]=".,/-_+=:;#!@&";
  _tx=0;
  _ty=2;
  _ln=0;
}

CmenuTypeText::~CmenuTypeText() {
}

void CmenuTypeText::gotoChar( char c ){
  int y;
  int x;
  _tx=-1;
  if( c==13 ) {
    _tx=12;
    _ty=4;
    _old=13;
  } else {
    for( y=0; y<6; ++y ) {
      for( x=0; x<13; ++x ) {
        if(  _tab[y][x]==c ) {
          _ty=y;
          _tx=x;
          _old=_tab[_ty][_tx];
          y=6;
          break;
        }
      }
    }
  }
  if( _tx==-1 ) {
    _tx=0;
    _ty=2;
    _old='a';
  }
  if( _ty==5 ) {
    _ln=1;
  } else {
    _ln=0;
  }
}

int CmenuTypeText::up(Clcd *lcd) {
  if( _ty==0 ) {
    _ty=5;
    _ln=1;
  } else {
    --_ty;
    if( _ln==1 ) {
      _ln=0;
    }
  }
  this->screen(lcd);
  return 0;
}
int CmenuTypeText::down(Clcd *lcd) {
  if( _ty==5 ) {
    _ty=0;
    _ln=0;
  } else {
    ++_ty;
    if( _ln==0 ) {
      _ln=1;
    }
  }
  this->screen(lcd);
  return 0;
}
int CmenuTypeText::left(Clcd *lcd) {
  if( _tx==0 ) {
    _tx=12;
  } else {
    --_tx;
  }
  this->screen(lcd);
  return 0;
}
int CmenuTypeText::right(Clcd *lcd) {
  if( _tx==12 ) {
    _tx=0;
  } else {
    ++_tx;
  }
  this->screen(lcd);
  return 0;
}
int CmenuTypeText::enter(Clcd *lcd) {
  if( _tx==12 && _ty==4 ) {
    return 13;
  } else {
    return _tab[_ty][_tx];
  }
}
int CmenuTypeText::esc(Clcd *lcd) {
  return 1;
}
void CmenuTypeText::screen(Clcd *lcd) {
  if( _ln==0 ) {
    lcd->_lcd[0]=_tab[_ty];
    lcd->_lcd[1]=_tab[_ty+1];
  } else {
    lcd->_lcd[0]=_tab[_ty-1];
    lcd->_lcd[1]=_tab[_ty];
  }
  lcd->_lcd[0]+=" >";
  if( _old==13 ) {
    lcd->_lcd[0]+="¶";
  } else {
    lcd->_lcd[0]+=_old;
  }
  lcd->_lcd[1]+=" <";
  if( _tx==12 && _ty==4 ) {
    lcd->_lcd[1]+="¶";
  } else {
    lcd->_lcd[1]+=_tab[_ty][_tx];
  }
  lcd->_cur._x=_tx;
  lcd->_cur._y=_ln;
  lcd->_cur._car=Ccur::blinkLine;
}
void CmenuTypeText::fullEsc() {
}

