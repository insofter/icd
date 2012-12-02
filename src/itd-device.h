#ifndef __ITD_DEVICE_H__
#define __ITD_DEVICE_H__

#include <string>
#include <map>
#include <fstream>

namespace icd
{
  enum itd_state
  {
    ITD_STATE_UNKNOWN = -1,
    ITD_STATE_CLEAR = 0,
    ITD_STATE_BLOCKED = 1,
  };

  enum itd_led_ctrl
  {
    ITD_LED_CTRL_AUTOMATIC = 0,
    ITD_LED_CTRL_MANUAL = 3,
    ITD_LED_CTRL_ON = 1,
    ITD_LED_CTRL_OFF = 2,
  };

  struct itd_raw_event
  {
    long long sec;
    long long usec;
    itd_state state;
  };

  class itd_test
  {
  public:
    enum type
    {
      OK = 0,
      FAIL_01 = 1,
      FAIL_10 = 2,
      FAIL_11 = 3,
    };

    itd_test() : value_(OK) {}
    itd_test(int value);
    operator int() const { return static_cast<int>(value_); }
    std::string to_string() const;

  private:
    type value_;
  };

  class itd_device
  {
    public :
      itd_device(const std::string& name);
      ~itd_device() {}

      std::string name() const { return name_; }

      void open();
      void close();
      itd_raw_event read();

      void set_active_low(int active_low);
      void set_engage_delay_ms(long engage_delay_ms);
      void set_release_delay_ms(long release_delay_ms);
      void set_led_ctrl(itd_led_ctrl led_ctrl);
      void set_queue_len(int queue_len);
      void restart();

      int active_low();
      long engage_delay_ms();
      long release_delay_ms();
      itd_led_ctrl led_ctrl();
      int queue_len();
      itd_test test();

      static std::map<std::string,itd_test> test_all();

    private:
      template<class T>
      void write_sysfs_attr(const std::string& attr_name, const T& value)
      {
        std::string path = sysfs_path_ + "/" + attr_name;
        std::ofstream os;

        os.open(path.c_str());
        if (!os)
          throw_ios_writing_failure(path);

        os << value << std::endl;
        if (!os)
          throw_ios_writing_failure(path);

        os.close();
      }

      template<class T>
      void read_sysfs_attr(const std::string& attr_name, T& value)
      {
        std::string path = sysfs_path_ + "/" + attr_name;
        std::ifstream is;

        is.open(path.c_str());
        if (!is)
          throw_ios_reading_failure(path);

        is >> value >> std::ws;
        if (!is)
          throw_ios_reading_failure(path);

        is.close();
      }

      static void throw_ios_reading_failure(const std::string& file);
      static void throw_ios_writing_failure(const std::string& file);

      static const std::string sysfs_drv_path_;

      std::string name_;
      std::string dev_path_;
      std::string sysfs_path_;
      std::ifstream is_;
  };
}

#endif // __ITD_DEVICE_H__

