#include "logger.hpp"

int print_percent(int perc) {
  struct pollfd fds[1];

  fds[0].fd=STDOUT_FILENO;
  fds[0].events=POLLOUT;
  poll( fds, 1, 0 );

  if( fds[0].revents & POLLOUT ) {
    char buf[5];
    int x;
  
    if( perc > 100 ) {
      perc=100;
    } else if( perc < 0 ) {
      perc=0;
    }
    x=sprintf( buf,"%i\n", perc );
    write(STDOUT_FILENO, buf, x );
    return perc;
  }
  return -1;
}


Clog::Clog( log_type log ): _log(log) {
                  //0123456789012345
  sprintf(progress,"??Progress: xxx%");
  destrLock=false;
}


Clog::~Clog() {
  if( ! destrLock ) {
    std::string x=globalConfig->entry("current", "last-send-status"); 
    if( x.size()>=1 && x[0]=='?' ) {
      globalConfig->set_entry("current", 
          "last-send-status", "--ERR TR. BROKEN");
    }
  }
}
void Clog::okParams( int percent, const std::string & cmd ) {
  sprintf(progress+12,"%3i%%", percent);
  globalConfig->set_entry("current","last-send-status", progress);

  switch( _log ) {
    case SHORT:
      print_percent( percent );
      break;

    case LONG:
      std::cout << std::endl << "[" << percent << "%] " << std::endl
        << "++OK Utworzono parametry dla \"" << cmd << "\""
        << std::endl; 
      break;
  }
}
 
void Clog::okSoap( int percent, const std::string & servQuery ) {
  sprintf(progress+12,"%3i%%", percent);
  globalConfig->set_entry("current","last-send-status", progress);

  switch( _log ) {
    case SHORT:
      print_percent( percent );
      break;

    case LONG:
      std::cout << "[" << percent << "%] " << std::endl
        << "++OK Wykonano zapytanie: " << std::endl << "    "
        << servQuery 
        << std::endl;
      break;
  }
}

 
void Clog::okServerAns( int percent, const std::string & servAns ) {
  sprintf(progress+12,"%3i%%", percent);
  globalConfig->set_entry("current","last-send-status", progress);

  switch( _log ) {
    case SHORT:
      print_percent( percent );
      break;

    case LONG:
      std::cout << "[" << percent << "%] " << std::endl
        << "++OK Odpowiedź serwera: "<< std::endl << "    "
        << servAns
        << std::endl;
      break;
  }
}

 
void Clog::errParams( int percent, const std::string & cmd, 
    const std::string & err, const std::string & short_err ) {
  std::ostringstream ss;

  ss << "--ERR CR:" << short_err;
  globalConfig->set_entry( "current","last-send-status", ss.str() );

  switch( _log ) {
    case SHORT:
      std::cout << ss.str() << std::endl;
      break;

    case LONG:
      std::cout << std::endl << "[" << percent << "%] " << std::endl
        << "--ERR Błąd przygotowania danych dla polecenia \""
        << cmd << "\":" << std::endl << "    "
        << err
        << std::endl;
      break;
  }
}

 
void Clog::errSoap( int percent, const std::string & soapTxt,
    int soapCode, const std::string & err ) {

  char ss[17];
  sprintf(ss,"--ERR SOAP:%4i", soapCode );
  globalConfig->set_entry("current","last-send-status", ss);

  switch( _log ) {
    case SHORT:
      printf("%s\n", ss );
      break;

    case LONG:
      std::cout << "[" << percent << "%] " << std::endl
        << "--ERR Błąd połączenia: "<< err 
        << std::endl << "    " << "SOAP: " << soapTxt
        << std::endl;
      break;
  }
}

void Clog::errServerAns( int percent, const std::string & servAns,
  const std::string & shortAns, const std::string & shortCmd,
  const std::string & errName ) {

  std::ostringstream ss;

  ss << "--ERR " << shortCmd << ":" << shortAns;
  globalConfig->set_entry( "current","last-send-status", ss.str() );

  switch( _log ) {
    case SHORT:
      std::cout << ss.str() << std::endl;
      break;

    case LONG:
      std::cout << "[" << percent << "%] " << std::endl
        << "--ERR Błąd polecenia: " << errName
        << std::endl << "    " << servAns << std::endl;

      break;
  }
}

void Clog::done( int warn ) {
  

  std::ostringstream ss;

  if( warn == 0 ) {
    char buf[21];
    time_t rawtime;
    struct tm * timeinfo;
    time( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime(buf,20,"++OK %m.%d-%H:%M",timeinfo);
    buf[16]=0;       
    ss << buf; 
  } else {
    ss << "++WARNINGS: " << warn;
  }
  globalConfig->set_entry( "current","last-send-status", ss.str() );
  
  switch( _log ) {
    case SHORT:
      std::cout << ss.str() << std::endl;
      break;
    case LONG:
      std::cout << std::endl << "[100%] " << std::endl;
      if( warn == 0 ) {
        std::cout << "++OK Transfer OK" << std::endl;
      } else {
        std::cout << "++WARN: Ilość ostrzeżeń: " << warn << std::endl;
      }
      break;
  }
}

void Clog::error( int err ) {
  std::ostringstream ss;
  ss << "--ERRORS: " << err;
  globalConfig->set_entry( "current","last-send-status", ss.str() );

  
  switch( _log ) {
    case SHORT:
      std::cout << ss.str() << std::endl;
      break;
    case LONG:
      std::cout << std::endl << "[100%] " << std::endl;
      std::cout << "--ERRORS: Ilość błedów: " << err << std::endl;
      break;
  }
}

void Clog::errTrInProgress() {
  destrLock=true;
  switch( _log ) {
    case SHORT:   //0123456789012345
      std::cout << "--TR IN PROGRESS" << std::endl;
      break;
    case LONG:
      std::cout << "--ERR Komunikacja w tle." << std::endl;
      break;
  }
}
  //0123456789012345//
  //--ERR CR. PARAM.//
  //--ERR SOAP: 123 //
  //--ERR LD: no dev//
  //--ERR GT:       //
  //--ERR SD:too old//
  //--ERR AF:       //
  //--ERR LO: I.S.E //
  //--ERR           //
  //--ERR           //
  //--ERR           //

