#include "daemonizer.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

daemonizer::~daemonizer()
{
  // If we are executing as daemon and pid file was created
  // then delete it
  if (pid_ == 0 && !pid_file_.empty())
    unlink(pid_file_.c_str());
}

void daemonizer::create_pid_file()
{
  // Create pid file if required by the user
  if (pid_ > 0 && !pid_file_.empty())
  {
    std::ofstream os;
    os.open(pid_file_.c_str(), std::ios_base::out | std::ios_base::trunc);
    os << pid_;
    if (!os)
    {
      std::ostringstream oss;
      oss << "Writing pid file '" << pid_file_ << "' failed";
      throw std::ios_base::failure(oss.str());
    }
    os.close();
  }
}

void daemonizer::prepare_child()
{
  // Change the file mode mask
  umask(0);

  // Create a new SID for the child process
  pid_t sid = setsid();
  if (sid < 0)
  {
    std::ostringstream oss;
    oss << "setsid failed (" << sid << ")";
    throw std::runtime_error(oss.str());
  }

  // Change the current working directory. This prevents the current
  // directory from being locked; hence somebody not being able to remove it.
  if ((chdir("/")) < 0)
    throw std::runtime_error("Changing working directory to '/' failed");

  // Redirect standard files to /dev/null
  freopen( "/dev/null", "r", stdin);
  freopen( "/dev/null", "w", stdout);
  freopen( "/dev/null", "w", stderr);
}

bool daemonizer::fork()
{
  // Fork off the parent process
  pid_ = ::fork();
  if (pid_ < 0)
  {
    std::ostringstream oss;
    oss << "fork failed (" << pid_ << ")";
    throw std::runtime_error(oss.str());
  }

  // If we got non-zero pid, then we are executing as the parent process.
  // otherwise we are executing as the child process
  if (pid_ > 0)
    create_pid_file();
  else
    prepare_child();

  return (pid_ > 0);
}

