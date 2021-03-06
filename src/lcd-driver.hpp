/**
 * @file lcd-driver.hpp
 * @author Piotr Mika
 * @date VIII 2012r.
 * Classes for Hitachi HD44780 LCD controller on linux.
 */

#ifndef LCD_DRIVER_HPP
#define LCD_DRIVER_HPP
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>

/**
 * Cursor position
 */
struct Ccur {
  enum cursor {
    none = 0, // cmd 12
    blink = 1, // cmd 13
    line = 2, // cmd 14
    blinkLine = 3 //cmd 15
  };
  int _x;
  int _y;
  /**
   * Visibility of cursor.
   */
  cursor _car;
};

/**
 * Polish language special character
 */
struct Cletter {
  /**
   * First byte of special sign.
   */
  static const char byte1 = 0b11000010;
  /*
   * Second byte of special sign - ethernet.
   */
  static const char byte2Eth = 0b10000000;
  /*
   * Second byte of special sign - wifi.
   */
  static const char byte2Wifi = 0b10000001;
  /*
   * Second byte of special sign - gsm.
   */
  static const char byte2Gsm = 0b10000010;
  /**
   * Letter in two-byte UTF-8
   */
  char _utf[2];//2-byte UTF-8 character

  /**
   * ASCII 7-bit eqivalent ( ó -> o ), used in case of lack of LCD RAM (8 positions only).
   */
  char _ascii;

  /**
   * Eight bytes of sign, important is only 5 lower bits of every byte.
   */
  char _bytes[8];
  
  /**
   * Position in LCD's memory ( 0 to 7 ).
   */
  int _ramPos;

  /**
   * Constructor, build character from itd data.
   * Multiple parameters allow to constructing characters in easy way. Use editor to highlight
   * "1" and write letter
   * @code{.cpp}
   * new Cletter("ó","o",
   *    0b00010,
   *    0b00100,
   *    0b01110,
   *    0b10001,
   *    0b10001,
   *    0b10001,
   *    0b01110,
   *    0b00000 );
   * @endcode
   * @param utf Polish language special character (3-byte C style 0-ended string or 2-byte std::string).
   * @param ascii ASCII 7-bit eqivalent. 
   * @param a First line of character. 
   * @param b Second line.
   * @param c Third line.
   * @param d Fourth line.
   * @param e Fifth line.
   * @param f Sixth line.
   * @param g Seventh line.
   * @param h Eighth line. 
   */
  Cletter(std::string utf, std::string ascii,
          char a, char b,
          char c, char d,
          char e, char f,
          char g, char h);
};

/**
 * Lcd display
 */
struct Clcd {
  /**
   * Two lines of display, can have polish UTF-8 letters, despite the fact that std::string 
   * don't understand 2-byte characters. But it can contain them.
   */
  std::string _lcd[2];

  /**
   * Refresh counter delay in miliseconds, 0 means no refresh
   */
  int _refresh;
  /**
   * Cursor.
   */
  Ccur _cur;
};

class ClcdDriver {
protected:
  std::ofstream _cmds;
  std::ofstream _data;
  std::ofstream _backlight;
  void _cmd(int cmd);
  std::vector< Cletter* > _letters;

public:
  /**
   * Constructor.
   * @param cmd_path Path to command device.
   * @param data_pathPath to data device.
   */
  ClcdDriver(const char * cmd_path="/sys/devices/platform/gpio-hd44780/cmd",
      const char * data_path="/dev/lcd0",
      const char * backlight_path="/sys/devices/platform/gpio-hd44780/backlight");
  virtual ~ClcdDriver();

  /**
   * Adds character to LCD's RAM
   * @param nr Position in RAM (0 to 7)
   * @param a Pointer to table of character's data.
   * @param newCurPos Cursor position after programming new character.
   */
  void addChr(int nr, char a[8], int newCurPos=128);
  /**
   * Prints Clcd structure on LCD display.
   * @param lcd Screen data.
   */
  void print(const Clcd & lcd);
  /**
   * Turns on or off LcdBacklight
   * @param newState 1 for on, 0 for off
   */
  void backlight( int newState );
};

#endif // LCD_DRIVER_HPP
