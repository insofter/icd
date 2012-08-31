#include "lcd-driver.hpp"

Cletter::Cletter(std::string utf, std::string ascii, // !! strings with ""
                 char a, char b,
                 char c, char d,
                 char e, char f,
                 char g, char h) {
  _utf[0]=utf[0];
  _utf[1]=utf[1];
  _ascii=ascii[0];
  _bytes[0]=a;
  _bytes[1]=b;
  _bytes[2]=c;
  _bytes[3]=d;
  _bytes[4]=e;
  _bytes[5]=f;
  _bytes[6]=g;
  _bytes[7]=h;
}

ClcdDriver::ClcdDriver(const char * cmd_path, const char * data_path) {
  _cmds.open(cmd_path);
  _data.open(data_path);
  if( !_cmds.is_open() || !_data.is_open() ) {
    std::cerr << "Unable to open device" << std::endl;
    exit(-1);
  }
  _cmds << std::unitbuf;
  _data << std::unitbuf;
  
  _cmd(15); //00001111 start
  _cmd(1); //reset + cur na 0,0
  _cmd(12); //cur off

  _letters.push_back( new Cletter("ł"," ",
                                  0b00001,
                                  0b00001,
                                  0b11111,
                                  0b10001,
                                  0b10001,
                                  0b11111,
                                  0b11111,
                                  0b11111 ));
  _letters.push_back( new Cletter("ą"," ",
                                  0b11100,
                                  0b00010,
                                  0b11001,
                                  0b00101,
                                  0b10101,
                                  0b00101,
                                  0b11001,
                                  0b00010 ));
  _letters.push_back( new Cletter("ć"," ",
                                  0b11111,
                                  0b00100,
                                  0b11111,
                                  0b10000,
                                  0b11111,
                                  0b10000,
                                  0b10101,
                                  0b11111 ));
  _letters.push_back( new Cletter("ą","a",
                                  0b00000,
                                  0b00000,
                                  0b01110,
                                  0b00001,
                                  0b01111,
                                  0b10001,
                                  0b01111,
                                  0b00100 ));
  _letters.push_back( new Cletter("ć","c",
                                  0b00010,
                                  0b00100,
                                  0b01110,
                                  0b10000,
                                  0b10000,
                                  0b10001,
                                  0b01110,
                                  0b00000 ));
  _letters.push_back( new Cletter("ę","e",
                                  0b00000,
                                  0b00000,
                                  0b01110,
                                  0b10001,
                                  0b11111,
                                  0b10000,
                                  0b01110,
                                  0b00100 ));
  _letters.push_back( new Cletter("ł","l",
                                  0b01100,
                                  0b00101,
                                  0b00110,
                                  0b00100,
                                  0b01100,
                                  0b10100,
                                  0b01110,
                                  0b00000 ));
  _letters.push_back( new Cletter("ń","n",
                                  0b00001,
                                  0b00010,
                                  0b10110,
                                  0b11001,
                                  0b10001,
                                  0b10001,
                                  0b10001,
                                  0b00000 ));
  _letters.push_back( new Cletter("ó","o",
                                  0b00010,
                                  0b00100,
                                  0b01110,
                                  0b10001,
                                  0b10001,
                                  0b10001,
                                  0b01110,
                                  0b00000 ));
  _letters.push_back( new Cletter("ś","s",
                                  0b00010,
                                  0b00100,
                                  0b01110,
                                  0b10000,
                                  0b01110,
                                  0b00001,
                                  0b11110,
                                  0b00000 ));
  _letters.push_back( new Cletter("ź","z",
                                  0b00010,
                                  0b00100,
                                  0b11111,
                                  0b00010,
                                  0b00100,
                                  0b01000,
                                  0b11111,
                                  0b00000 ));
  _letters.push_back( new Cletter("ż","z",
                                  0b00100,
                                  0b00000,
                                  0b11111,
                                  0b00010,
                                  0b00100,
                                  0b01000,
                                  0b11111,
                                  0b00000 ));




  _letters.push_back( new Cletter("Ą","A",
                                  0b01110,
                                  0b10001,
                                  0b10001,
                                  0b10001,
                                  0b11111,
                                  0b10001,
                                  0b10001,
                                  0b00010 ));
  _letters.push_back( new Cletter("Ć","C",
                                  0b00100,
                                  0b01110,
                                  0b10001,
                                  0b10000,
                                  0b10000,
                                  0b10001,
                                  0b01110,
                                  0b00000 ));
  _letters.push_back( new Cletter("Ę","E",
                                  0b11111,
                                  0b10000,
                                  0b10000,
                                  0b11111,
                                  0b10000,
                                  0b10000,
                                  0b11111,
                                  0b00100 ));
  _letters.push_back( new Cletter("Ł","L",
                                  0b10000,
                                  0b10000,
                                  0b10010,
                                  0b10100,
                                  0b11000,
                                  0b10000,
                                  0b11111,
                                  0b00000 ));
  _letters.push_back( new Cletter("Ń","N",
                                  0b00010,
                                  0b00100,
                                  0b10001,
                                  0b11001,
                                  0b10101,
                                  0b10011,
                                  0b10001,
                                  0b00000 ));
  _letters.push_back( new Cletter("Ó","O",
                                  0b00100,
                                  0b01110,
                                  0b10001,
                                  0b10001,
                                  0b10001,
                                  0b10001,
                                  0b01110,
                                  0b00000 ));
  _letters.push_back( new Cletter("Ś","S",
                                  0b00100,
                                  0b01111,
                                  0b10000,
                                  0b01110,
                                  0b00001,
                                  0b00001,
                                  0b11110,
                                  0b00000 ));
  _letters.push_back( new Cletter("Ź","Z",
                                  0b00010,
                                  0b00100,
                                  0b11111,
                                  0b00010,
                                  0b00100,
                                  0b01000,
                                  0b11111,
                                  0b00000 ));
  _letters.push_back( new Cletter("Ż","Z",
                                  0b00100,
                                  0b00000,
                                  0b11111,
                                  0b00010,
                                  0b00100,
                                  0b01000,
                                  0b11111,
                                  0b00000 ));
}
ClcdDriver::~ClcdDriver() {
  unsigned int i;
  _cmds.close();
  _data.close();
  for( i=0; i<_letters.size(); ++i ) {
    delete _letters[i];
  }

}

void ClcdDriver::_cmd(int cmd) {
  _cmds << cmd;
}

void ClcdDriver::addChr(int nr, char a[8], int newCurPos) {
  if( nr>=0 && nr<8 ) {
    int i;
    _cmd(64+nr*8); //ram addres for char
    for( i=0 ; i<8; ++i ) {
      _data << a[i];
    }
    _cmd(newCurPos); //cur 0,0 to write on scren not to memory
  }
}

void ClcdDriver::print(const Clcd & lcd) {
  unsigned int lpos, spos, i, line;
  int rpos=0;
  for( i=0; i<_letters.size(); ++i ) {
    _letters[i]->_ramPos=-1;
  }
  for( line=0; line<2; ++line ) {
    _cmd(128+line*64);
    lpos=0;
    spos=0;
    while( spos<lcd._lcd[line].size() && lpos<16 ) {
      if( (lcd._lcd[line][spos] & 0b11100000) == 0b11000000 ) {
        for( i=0; i<_letters.size(); ++i ) {
          if( lcd._lcd[line][spos]==_letters[i]->_utf[0]
              && lcd._lcd[line][spos+1]==_letters[i]->_utf[1] ) {
            //znaleziono znak utfa i trzeba go przetworzyc
            if( _letters[i]->_ramPos!=-1 ) {
              _data << (unsigned char)_letters[i]->_ramPos; 
            } else {
              if( rpos>7 ) {
                _data << _letters[i]->_ascii;
              } else {
                addChr( rpos, _letters[i]->_bytes, 128+line*64+lpos );
                _letters[i]->_ramPos=rpos;
                _data << (unsigned char)rpos;
                ++rpos;
              }
            }
            spos+=2;
            ++lpos;
            break;
          }
        }
      } else {
        _data << lcd._lcd[line][spos];
        ++spos;
        ++lpos;
      }
    }
    for( ; lpos<16; ++lpos ) {
      _data << " ";
    }
  }
  if( lcd._cur._car!=Ccur::none && ( lcd._cur._y==0 || lcd._cur._y==1 )
      && ( lcd._cur._x>=0 && lcd._cur._x<16 ) ) {
    
    _cmd(128 + lcd._cur._y*64 + lcd._cur._x);

    if( lcd._cur._car==Ccur::line ) {
      _cmd(14);
    } else if( lcd._cur._car==Ccur::blink ) {
      _cmd(13);
    } else {
      _cmd(15); 
    }

  } else {
    _cmd(12); //cur off
  }
}


