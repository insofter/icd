#ifndef LCD_DRIVER_HPP
#define LCD_DRIVER_HPP
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>

struct Ccur {
  int _x;
  int _y;
};

struct Cletter {
  char _utf[2];//2-byte utf8 char
  char _ascii;
  char _bytes[8];
  int _ramPos;
  Cletter(std::string utf, std::string ascii, // !! strings with ""
          char a, char b,
          char c, char d,
          char e, char f,
          char g, char h);
};

struct Clcd {
  std::string _lcd[2];
  int _refresh;
  bool _curOn;
  Ccur _cur;
};

class ClcdDriver {
protected:
  std::ofstream _cmds;
  std::ofstream _data;
  void _cmd(int cmd);
  std::vector< Cletter* > _letters;

public:
  ClcdDriver(const char * cmd_path="/sys/devices/platform/gpio-hd44780/cmd",
             const char * data_path="/dev/lcd0");
  virtual ~ClcdDriver();

  void addChr(int nr, char * a, int newCurPos=128);
  void print(const Clcd & lcd);
};

#endif // LCD_DRIVER_HPP
