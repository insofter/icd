#include "syslogstream.h"

syslogbuf::syslogbuf(const char *ident, int option, int facility,
  std::size_t size)
: buffer_(size + 1), priority_(LOG_DEBUG)
{
  char *base = &buffer_.front();
  setp(base, base + buffer_.size() - 1); // -1 to make overflow() easier

  openlog(ident, option, facility);
}

syslogbuf::~syslogbuf()
{
  closelog();
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
      syslog(priority_, "%s", b);
      b = p + 1;
    }
  }
  if (b < e)
    syslog(priority_, "%s", b);

  std::ptrdiff_t n = pptr() - pbase();
  pbump(-n);

  return true;
}

syslogstream &info(syslogstream &os)
{
  syslogbuf *buf = dynamic_cast<syslogbuf *>(os.rdbuf());
  if (buf)
    buf->priority(LOG_INFO);

  return os;
}

