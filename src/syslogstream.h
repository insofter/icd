#include <syslog.h>
#include <iostream>
#include <vector>

#ifndef __SYSLOGSTREAM_H__
#define __SYSLOGSTREAM_H__

class syslogbuf : public std::streambuf
{
  public:
    explicit syslogbuf(const char *ident, int option = 0,
      int facility = LOG_DAEMON, std::size_t size = 256);
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
    int priority_;

};

class syslogstream : public std::ostream
{
public:
  syslogstream(const char *ident, int option = 0, int facility = LOG_DAEMON,
    std::size_t size = 256)
  : std::ostream(), syslogbuf_(ident, option, facility, size)
  {
    rdbuf(&syslogbuf_);
  }

private:
  syslogbuf syslogbuf_;

};

#endif // __SYSLOGSTREAM_H__

