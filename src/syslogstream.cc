#include <time.h>
#include <sys/time.h>

#include "syslogstream.h"

namespace icd
{
  syslogbuf::syslogbuf(const std::string& name, std::size_t size)
  : name_(name), buffer_(size + 1), priority_(LOG_DEBUG)
  {
    char *base = &buffer_.front();
    setp(base, base + buffer_.size() - 1); // -1 to make overflow() easier
  }

  syslogbuf::~syslogbuf()
  {
//    closelog();
  }

  syslogbuf::int_type syslogbuf::overflow(int_type ch)
  {
    if (ch != traits_type::eof())
    {
      *pptr() = ch;
      pbump(1);
      if (flush())
        return ch;
    }

    return traits_type::eof();
  }

  int syslogbuf::sync()
  {
    return flush() ? 0 : -1;
  }

  bool syslogbuf::flush()
  {
    char *b = pbase(), *e = pptr();
    for (char *p = pbase(); p != e; ++p)
    {
      if (*p == '\n')
      {
        *p = 0;
        write_to_syslog(b);
        b = p + 1;
      }
    }
    if (b < e)
      write_to_syslog(b);

    std::ptrdiff_t n = pptr() - pbase();
    pbump(-n);

    return true;
  }

  void syslogbuf::write_to_syslog(const char * str)
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    char s[32];
    struct tm result;
    struct tm *m = gmtime_r((const time_t *)&tv.tv_sec, &result);
    strftime(s, sizeof(s), "%Y-%m-%dT%H:%M:%S", m);

    syslog(priority_, "[%s %s %s.%06li] %s", name_.c_str(),
      priority_str(priority_), s, tv.tv_usec, str);
  }

  const char *syslogbuf::priority_str(int priority)
  {
    const char *ret = NULL;
    switch(priority)
    {
    case LOG_EMERG:
      ret = "EMERG";
      break;
    case LOG_ALERT:
      ret = "ALERT";
      break;
    case LOG_CRIT:
      ret = "CRIT";
      break;
    case LOG_ERR:
      ret = "ERR";
      break;
    case LOG_WARNING:
      ret = "WARNING";
      break;
    case LOG_NOTICE:
      ret = "NOTICE";
      break;
    case LOG_INFO:
      ret = "INFO";
      break;
    case LOG_DEBUG:
      ret = "DEBUG";
      break;
    default:
      ret = "?";
      break;
    }
    return ret;
  }

  void syslogstream::initialize(const char *ident, int option, int facility)
  {
    openlog(ident, option, facility);
  }

  std::ostream &emerg(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_EMERG);
    return os;
  }
  std::ostream &alert(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_ALERT);
    return os;
  }
  std::ostream &crit(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_CRIT);
    return os;
  }
  std::ostream &err(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_ERR);
    return os;
  }
  std::ostream &warning(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_WARNING);
    return os;
  }
  std::ostream &notice(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_NOTICE);
    return os;
  }

  std::ostream &info(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_INFO);
    return os;
  }

  std::ostream &debug(std::ostream &os)
  {
    syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
    if (buf)
      buf->priority(LOG_DEBUG);
    return os;
  }
}

