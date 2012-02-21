#include <string>
#include <sys/types.h>

#ifndef __DAEMONIZER_H__
#define __DAEMONIZER_H__

class daemonizer
{
public:
  daemonizer() : pid_(0) {}
  ~daemonizer();

  // if pid file specified the it will be created and will
  // contain pid of the daemon process (number in text format)
  // the file will be deleted when daemon exits
  void pid_file(const std::string& pid_file) { pid_file_ = pid_file; }
  const std::string& pid_file() { return pid_file_; }

  // true = parent process, not a daemon - exit
  // fale = child process, daemon - do the job
  bool fork();

private:
  std::string pid_file_;
  pid_t pid_;

  void create_pid_file();
  void prepare_child();
};

#endif // __DAEMONIZER_H__

