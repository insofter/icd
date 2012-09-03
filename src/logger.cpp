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
}

void Clog::okParams( int percent, const std::string & cmd ) {
  //todb

  switch( _log ) {
    case SHORT:
      print_percent( percent );
      break;

    case LONG:
      std::cout << "[" << percent << "%] " << std::endl
        << "++OK Utworzono parametry dla \"" << cmd << "\""
        << std::endl; 
      break;
  }
}
 
void Clog::okSoap( int percent, const std::string & servQuery ) {
//todb

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
  //todb

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
  //todb

  switch( _log ) {
    case SHORT:
      std::cout << "--ERR CR:" << short_err << std::endl;
      break;

    case LONG:
      std::cout << "[" << percent << "%] " << std::endl
        << "--ERR Błąd przygotowania danych dla polecenia \""
        << cmd << "\":" << std::endl << "    "
        << err
        << std::endl;
      break;
  }
}

 
void Clog::errSoap( int percent, const std::string & soapTxt,
    int soapCode, const std::string & err ) {
  //todb

  switch( _log ) {
    case SHORT:
      printf("--ERR SOAP:%4i\n", soapCode );
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
  //todb

  switch( _log ) {
    case SHORT:
      std::cout << "--ERR " << shortCmd << ":" << shortAns 
        << std::endl;
      break;

    case LONG:
      std::cout << "[" << percent << "%] " << std::endl
        << "--ERR Błąd polecenia: " << errName
        << std::endl << "    " << servAns << std::endl;

      break;
  }
}

void Clog::done( int warn ) {
  //todb
  
  switch( _log ) {
    case SHORT:
      if( warn == 0 ) {
        std::cout << "++OK Transfer OK" << std::endl;
      } else {
        std::cout << "++WARN: " << warn << std::endl;
      }
      break;
    case LONG:
      std::cout << "[100%] " << std::endl;
      if( warn == 0 ) {
        std::cout << "++OK Transfer OK" << std::endl;
      } else {
        std::cout << "++WARN: Ilość ostrzeżeń: " << warn << std::endl;
      }
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

