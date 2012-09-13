#include <syslog.h>
#include <iostream>
#include <vector>

#ifndef __SYSLOGSTREAM_H__
#define __SYSLOGSTREAM_H__

namespace icd
{
  class syslogbuf : public std::streambuf
  {
    public:
      explicit syslogbuf(const std::string& name, std::size_t size);
      ~syslogbuf();

      void priority(int priority) { priority_ = priority; }
      int priority() { return priority_; }

    protected:
      bool flush();

    private:
      int_type overflow(int_type ch);
      int sync();

      // copy ctor and assignment not implemented;
      // copying not allowed
      syslogbuf(const syslogbuf &);
      syslogbuf &operator= (const syslogbuf &);

    private:
      std::vector<char> buffer_;
      std::string name_;
      int priority_;

      void write_to_syslog(const char * str);
      const char *priority_str(int priority);
  };

  class syslogstream : public std::ostream
  {
  public:
    static void initialize(const char *ident, int option = 0, int facility = LOG_DAEMON);

    syslogstream(const std::string name = "main", std::size_t size = 256)
    : std::ostream(), syslogbuf_(name, size)
    {
      rdbuf(&syslogbuf_);
    }

  private:
    syslogbuf syslogbuf_;
  };

  std::ostream &emerg(std::ostream &os);
  std::ostream &alert(std::ostream &os);
  std::ostream &crit(std::ostream &os);
  std::ostream &err(std::ostream &os);
  std::ostream &warning(std::ostream &os);
  std::ostream &notice(std::ostream &os);
  std::ostream &info(std::ostream &os);
  std::ostream &debug(std::ostream &os);
}

#endif // __SYSLOGSTREAM_H__

