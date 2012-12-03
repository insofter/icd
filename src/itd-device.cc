#include "itd-device.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

/*#include <fstream>
#include <sstream>
#include <iomanip>
#include <string> 
#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <libgen.h>

#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include "daemonizer.h"
#include "syslogstream.h"
*/

namespace icd
{
  itd_test::itd_test(int value)
  {
    if (value < 0 || value > 3)
      throw std::runtime_error("Invalid itd_test value");
     value_ = static_cast<type>(value);
  }

  std::string itd_test::to_string() const
  {
    std::string str;
    switch(value_)
    {
    case OK:
      str = "OK";
      break;
    case FAIL_01:
      str = "FAILED_01";
      break;
    case FAIL_10:
      str = "FAILED_10";
      break; 
    case FAIL_11:
      str = "FAILED_11";
      break; 
    default: 
      throw std::runtime_error("Invalid itd_test value");
    }
    return str;
  }

  const std::string itd_device::sysfs_drv_path_ = std::string("/sys/bus/platform/drivers/gpio-itd");

  itd_device::itd_device(const std::string& name)
  {
    name_ = name;
    dev_path_ = std::string("/dev/") + name;
    sysfs_path_ = std::string("/sys/devices/platform/gpio-itd.") + name.substr(3);
  }

  void itd_device::open()
  {
    is_.open(dev_path_.c_str());
    if (!is_)
      throw_ios_reading_failure(dev_path_);
  }

  void itd_device::close()
  {
    is_.close();
  }

  itd_raw_event itd_device::read()
  {
    std::string line;
    if (!getline(is_, line))
      throw_ios_reading_failure(dev_path_);

    long long sec = 0, usec = 0;
    int state = 0;
    std::istringstream iss(line);
    iss >> sec >> usec >> state >> std::ws;
    if (!iss.eof())
      throw_ios_reading_failure(dev_path_);

    itd_raw_event ev;
    ev.state = static_cast<itd_state>(state);
    ev.sec = sec;
    ev.usec = usec;
    return ev;
  }

  void itd_device::set_active_low(int active_low)
  {
    write_sysfs_attr("active_low", active_low);
  }

  void itd_device::set_engage_delay_ms(long engage_delay_ms)
  {
    write_sysfs_attr("engage_delay_usec", engage_delay_ms * 1000);
  }

  void itd_device::set_release_delay_ms(long release_delay_ms)
  {
    write_sysfs_attr("release_delay_usec", release_delay_ms * 1000);
  }

  /*void itd_device::set_led_ctrl(itd_led_ctrl led_ctrl)
  {
    write_sysfs_attr("led_ctrl",led_ctrl.to_string())
  }*/

  void itd_device::set_queue_len(int queue_len)
  {
    write_sysfs_attr("queue_len", queue_len);
  }

  void itd_device::restart()
  {
    write_sysfs_attr("restart", 1);
  }

  int itd_device::active_low()
  {
    int value;
    read_sysfs_attr("active_low", value);
    return value;
  }

  long itd_device::engage_delay_ms()
  {
    long value;
    read_sysfs_attr("engage_delay_usec", value);
    return value / 1000;
  }

  long itd_device::release_delay_ms()
  {
    long value;
    read_sysfs_attr("release_delay_usec", value);
    return value / 1000;
  }

  /*itd_led_ctrl itd_device::led_ctrl()
  {
    std::string value;
    read_sysfs_attr("led_ctrl", value)
    return itd_led_ctrl::from_string(value);
  }*/

  int itd_device::queue_len()
  {
    int value;
    read_sysfs_attr("queue_len", value);
    return value;
  }

  itd_test itd_device::test()
  {
    int value;
    read_sysfs_attr("test", value);
    return itd_test(value);
  }

  void itd_device::set_test_time_usec(long test_time_usec)
  {
    std::string path = sysfs_drv_path_ + "/test_time_usec";
    std::ofstream os;

    os.open(path.c_str());
    if (!os)
      throw_ios_writing_failure(path);

    os << test_time_usec << std::endl;
    if (!os)
      throw_ios_writing_failure(path);

    os.close();
  }

  long itd_device::test_time_usec()
  {
    std::string path = sysfs_drv_path_ + "/test_time_usec";
    std::ifstream is;

    is.open(path.c_str());
    if (!is)
      throw_ios_reading_failure(path);

    long test_time_usec;
    is >> test_time_usec  >> std::ws;
    if (!is)
      throw_ios_reading_failure(path);

    is.close();

    return test_time_usec;
  }

  std::map<std::string,itd_test> itd_device::test_all()
  {
    std::map<std::string,itd_test> results;
    std::string path = sysfs_drv_path_ + "/test";
    std::ifstream is;

    is.open(path.c_str());
    if (!is)
      throw_ios_reading_failure(path);

    std::string line;
    if (!getline(is, line))
      throw_ios_reading_failure(path);

    int dev_id = 0;
    std::istringstream iss(line);
    iss >> std::ws;
    while (!iss.eof())
    {
      std::ostringstream oss;
      oss << "itd" << dev_id;
      std::string key = oss.str();

      int value;
      iss >> value >> std::ws;
      if (!iss)
         throw_ios_reading_failure(path);

      results.insert(std::pair<std::string,itd_test>(key, value));
      ++dev_id;
    }

    return results;
  }

  void itd_device::throw_ios_reading_failure(const std::string& file)
  {
    std::ostringstream oss;
    oss << "Reading file '" << file << "' failed";
    throw std::ios_base::failure(oss.str());
  }

  void itd_device::throw_ios_writing_failure(const std::string& file)
  {
    std::ostringstream oss;
    oss << "Writing file '" << file << "' failed";
    throw std::ios_base::failure(oss.str());
  }
}
