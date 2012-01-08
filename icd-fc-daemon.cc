#include <iostream>
#include <fstream>
#include <linux/input.h>
#include <signal.h>

#include "sqlite3cc.h"

volatile sig_atomic_t sigint = 0;

void sigint_handler(int signum)
{
  printf("sigint invoked\n");
  sigint = 1;
}

int main(int argc, char *argv[])
{
  struct sigaction action;
  action.sa_handler = sigint_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  int err = sigaction(SIGINT, &action, NULL);
  if (err != 0)
  {
    printf("sigaction failed\n");
    return 1;
  }

  std::ifstream kbd;
  kbd.open("/dev/input/event0");

  while(!sigint)
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

