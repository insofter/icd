#include <iostream>
#include <fstream>
#include <linux/input.h>
#include <signal.h>

#include "sqlite3cc.h"

int main(int argc, char *argv[])
{
  std::ifstream kbd;
  kbd.open("/dev/input/event0");

  while(1)
  {
    struct input_event ev;
    if (kbd.read((char*)&ev, sizeof(ev)))
    {
      std::cout << "ev:" << "time=" << (long long)ev.time.tv_sec
        << ",type=" << ev.type << ",code=" << ev.code
        << ",value=" << ev.value << std::endl;
    }
  }

  kbd.close();

  return 0;
}

