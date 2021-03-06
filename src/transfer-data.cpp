#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include "transfer-logger.hpp"
#include <getopt.h>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <fstream>

#include "transfer-gsoap/icdtcp3SoapProxy.h"
#include "transfer-gsoap/icdtcp3Soap.nsmap"

//gsoap/_Stub.h -- klasy parametrów
//gsoap/_Service1SoapProxy.h -- funkcje serwera

icd::config *globalConfig;
sqlite3cc::conn *globalConfigDb;
sqlite3cc::conn *globalDataDb;

#define TRANSFER_PACKAGE_CNT "32" //have to be string, not int

int iloscDanych() {
  sqlite3cc::stmt stmt( *globalDataDb );
  stmt.prepare( "SELECT COUNT(*) FROM flow WHERE flags > 0 "
      "ORDER BY dtm ASC LIMIT " TRANSFER_PACKAGE_CNT );//ilosc rekordow
  stmt.step();
  int x=stmt.column_int(0);
  stmt.finalize();
  return x;
}


int createData( std::string & data ) {
  sqlite3cc::stmt stmt( *globalDataDb );
  std::ostringstream ss;
  stmt.prepare( "SELECT id, counter_id, "
      "datetime(dtm, 'unixepoch', 'localtime'), cnt, dark_time, "
      "work_time ,test FROM flow WHERE flags > 0 ORDER BY dtm ASC LIMIT " TRANSFER_PACKAGE_CNT );//ilosc rekordow
  while( stmt.step() == SQLITE_ROW ) {
    ss << stmt.column_int(0) << ';' //id
      << stmt.column_text(1) << ';' //itd
      << stmt.column_text(2) << ';' //dtm
      << stmt.column_int(3) << ';' //cnt
      << stmt.column_int(4) << ';' //drk
      << stmt.column_int(5) << ';' //wrk
      << stmt.column_int(6) //test
      << std::endl;
  }
  stmt.finalize();
  data = ss.str();
  if( data.empty() ) {
    return 0;
  } else {
    return 1;
  }
}


void commitData( const std::string & data, int ilDanych ) {

  sqlite3cc::stmt stmt( *globalDataDb );
  stmt.prepare( "BEGIN IMMEDIATE TRANSACTION" );
  stmt.step();
  stmt.finalize();

  stmt.prepare( "UPDATE flow SET flags = ?1 WHERE id == ?2" );
  std::istringstream ss( data );
  int i, f;
  char x;
  while( ( ! ss.eof() ) && ilDanych ) {
    ss >> i >> x >> f ;
    // std::cerr << "........ i=" << i << " f=" << f << std::endl;
    stmt.bind_int( 1, f );
    stmt.bind_int( 2, i );
    stmt.step();
    stmt.reset();
    --ilDanych;
  }
  stmt.finalize();

  stmt.prepare( "COMMIT" );
  stmt.step();
  stmt.finalize();
}


void setTime( std::string & data ) {
  std::string s;
  if( data.size() >= 19 ) {
    data[4]='.';
    data[7]='.';
    data[10]='-';
    data[13]=':';
    data[16]=':';
    s="sudo -n date -s ";
    s+=data;
    s+=" &>/dev/null";
    if( system( s.c_str() ) == 0 ) {
      system( "sudo -n hwclock -w" );
    }
  }
}


void print_version(char *argv0) {
  std::cerr << basename(argv0) << " " << version << "\n"
    << copyright << std::endl;
}


void print_usage(char *argv0) {
  std::cerr
    << "\n"
    "Usage: " << basename(argv0) << " OPTION...\n"
    "\n"
    "This program sends data to server.\n"
    "\n"
    "Options:\n"
    " -d|--db=DB_NAME Database file path;\n"
    " Mandatory option\n"
    "\n"
    " -t|--timeout=TIMEOUT Timeout for access to\n"
    " the database in ms\n"
    "\n"
    " -l|--log=LOG_MODE How many information you need:\n"
    " silent - no output, only info in data-\n"
    " base and exitcode (default option)\n"
    " short - prints percents in nonblocking\n"
    " mode and status line \n"
    " long - shows everything: communication\n"
    " with server, responses, data pac- \n"
    " kages etc. \n"
    "\n"
    " -h|--help\n"
    " -v|--version\n"
    ;
}

char * new_c_str( const std::string & s ) {
  char * mem = new char[ s.size()+1 ];
  strcpy( mem, s.c_str() );
  return mem;
}

class CicdTransfer {
  private:
    icdtcp3SoapProxy *service;
    Clog *log;
    std::string s;
    int ans;
    _icd1__GetDeviceUpdateInfo update;
    _icd1__GetDeviceUpdateInfoResponse rupdate;

  public:
    CicdTransfer( Clog *l, bool GetCfg=true ) {
      log=l;
      std::string s;
      s=globalConfig->entry( "tcpip", "mac");
      if( GetCfg ) {
        if( atoi( (globalConfig->entry( "device", "idd")).c_str() ) == 0
            || s.size() != 17 || s.compare( "00:1C:D3:00:00:00" ) == 0 ) {
          CicdTransfer trGetCfg( log, false );
          if( trGetCfg.login() ) {
            if( trGetCfg.getTime() ) {
              if( trGetCfg.getCfg() ) {
                trGetCfg.logout();
              }
            }
          }
        }
      }
      s="http://";
      s+=globalConfig->entry( "device", "address" );
      s+="/icdtcp3/icdtcp3.asmx";
      service=new icdtcp3SoapProxy( new_c_str( s ) );
      service->userid = new_c_str( globalConfig->entry( "device", "user" ) );
      service->passwd = new_c_str( globalConfig->entry( "device", "pass" ) );
    }

    ~CicdTransfer() {
      delete service->soap_endpoint;
      delete service->userid;
      delete service->passwd;
      delete service;
    }
    void done() {
      log->done();
    }

    int login() {
      _icd1__LoginDevice login;
      _icd1__LoginDeviceResponse rlogin;
      login.idd=atoi( (globalConfig->entry( "device", "idd")).c_str() );
      login.mac=new std::string( globalConfig->entry( "tcpip", "mac" ) ); //
      login.deviceIds=new std::string( globalConfig->entry( "device", "ids" ) ); //
      std::fstream etc_soft( "/etc/software_version" , std::ios_base::in ); //
      etc_soft >> s;
      etc_soft.close();
      login.devInfo=new std::string( s );
      s="idd='";
      s+=globalConfig->entry( "device", "idd");
      s+="', deviceIds='";
      s+=*(login.deviceIds);
      s+="', devInfo='";
      s+=*(login.devInfo);
      s+="'";
      log->okParams( 3, "LoginDevice" );
      ans=service->LoginDevice( &login, &rlogin );
      delete login.deviceIds;
      delete login.devInfo;
      delete login.mac;
      if( ans!=SOAP_OK ) {
        log->errSoap( 7, service->soap_fault_detail(), ans, "Błąd transmisji" ); //
        return 0;
      }
      log->okSoap( 7, s );
      if( rlogin.LoginDeviceResult==0 ) {
        log->okServerAns( 10, *(rlogin.message) );
      } else {
        char bl[9];
        sprintf(bl, "bl. %3i", rlogin.LoginDeviceResult);
        log->errServerAns( 10, *(rlogin.message), bl, "LD", "błąd logowania" ); //
        return 0;
      }
      return 1;
    }

    int getTime() {
      _icd1__GetTime time;
      _icd1__GetTimeResponse rtime;
      log->okParams( 13, "GetTime" );
      ans=service->GetTime(&time, &rtime);
      if( ans!=SOAP_OK ) {
        log->errSoap( 17, service->soap_fault_detail(), ans, "Błąd transmisji" ); //
        return 0;
      }
      log->okSoap( 17, "GetTime" );
      log->okServerAns( 20, *(rtime.GetTimeResult) );
      setTime( *(rtime.GetTimeResult) );
      return 1;
    }

    int getCfg() {
      _icd1__GetMacIdd gmi;
      _icd1__GetMacIddResponse rgmi;
      log->okParams( 43, "GetMacIdd" );
      ans=service->GetMacIdd(&gmi, &rgmi);
      if( ans!=SOAP_OK ) {
        log->errSoap( 47, service->soap_fault_detail(), ans, "Błąd transmisji" ); //
        return 0;
      }
      log->okSoap( 47, "GetMacIdd" );
      if( rgmi.GetMacIddResult==0 ) {
        char ch[10];
        sprintf(ch,"%i",rgmi.response->Idd);
        globalConfig->set_entry( "device", "idd", ch);
        globalConfig->set_entry( "tcpip", "mac", *(rgmi.response->Mac) ); //
        log->okServerAns( 49, *(rgmi.response->Message) );
      } else {
        log->errServerAns( 49, *(rgmi.response->Message), "GetIdd", "GI",
            "Bład pobierania podstawowej konfiguracji" );
        return 0;
      }
      return 1;
    }

    int transfer() {
#define PROCENT_NA_TRANSFER 50
      _icd1__SendData3 data;
      _icd1__SendData3Response rdata;
      data.data = &s;
      int ilDanych=iloscDanych();
      int ilPaczek=std::ceil( ((float)ilDanych)/4 );
      int aktPaczka=0;
      while( createData( s ) && aktPaczka<ilPaczek ) {
        log->okParams( 20+PROCENT_NA_TRANSFER*(aktPaczka*3+1)/(ilPaczek*3), //
            "SendData3" );
        ans=service->SendData3(&data, &rdata);
        if( ans!=SOAP_OK ) {
          log->errSoap( 20+PROCENT_NA_TRANSFER*(aktPaczka*3+2)/(ilPaczek*3), //
              service->soap_fault_detail(), ans, "Błąd transmisji" );
          return 0;
        }
        log->okSoap( 20+PROCENT_NA_TRANSFER*(aktPaczka*3+2)/(ilPaczek*3), s, true ); //
        commitData( *rdata.message, ilDanych );
        if( rdata.SendData3Result>0 ) {
          log->errServerAns( 20+PROCENT_NA_TRANSFER*(aktPaczka*3+3)/(ilPaczek*3), //
              *(rdata.message),
              "too old", "SD", "błąd wysyłania" );
          break;
        } else {
          log->okServerAns( 20+PROCENT_NA_TRANSFER*(aktPaczka*3+3)/(ilPaczek*3), //
              *(rdata.message), true );
        }
        ++aktPaczka;
      }
      return 1;
    }

    int chkUpdate() {
      /*_icd1__GetDeviceUpdateInfo update; *** używane w 2 metodach*/
      /*_icd1__GetDeviceUpdateInfoResponse rupdate;*/
      std::fstream etc_type( "/etc/software_type" , std::ios_base::in );
      etc_type >> s;
      etc_type.close();
      update.softVersion=&s;
      log->okParams( 83, "Update" );
      ans=service->GetDeviceUpdateInfo( &update, &rupdate );
      if( ans!=SOAP_OK ) {
        log->errSoap( 87, service->soap_fault_detail(), ans, "Błąd transmisji" ); //
        return 0;
      }
      log->okSoap( 87, s/*"Update"*/ );//parametry SoftVersion=xxxx
      if( rupdate.GetDeviceUpdateInfoResult==0 ) {
        log->okServerAns( 89, "Dostępne nowe oprogramowanie" );
      } else {
        log->okServerAns( 89, "System aktualny" );
      }
      return 1;
    }

    int logout() {
      _icd1__LogoutDevice out;
      _icd1__LogoutDeviceResponse rout;
      log->okParams( 93, "LogoutDevice" );
      ans=service->LogoutDevice(&out, &rout);
      if( ans!=SOAP_OK ) {
        log->errSoap( 97, service->soap_fault_detail(), ans, "Błąd transmisji" ); //
        return 0;
      }
      log->okSoap( 97, "LogoutDevice" );
      if( rout.LogoutDeviceResult==0 ) {
        log->okServerAns( 99, *(rout.message) );
      } else {
        log->errServerAns( 99, *(rout.message), "sesserr", "LO",
            "błąd zamykania sesji" );
        return 0;
      }
      return 1;
    }

    int doUpdate() {
      if( rupdate.GetDeviceUpdateInfoResult==0 ) {
        s="wget -q -O /tmp/update.img http://";
        s+=globalConfig->entry( "device", "address" );
        s+="/icdtcp3";
        s+=*rupdate.response->Link;
        if( system( s.c_str() ) == 0 ) {
          char md5sum[33];
          FILE *md5Chk = popen("md5sum /tmp/update.img", "r");
          fscanf( md5Chk, "%32s", md5sum );
          pclose( md5Chk );
          if( (*(rupdate.response->Md5)).compare( md5sum ) == 0 ) {
            if( (*(rupdate.response->UpdateForce)).compare( "FORCE" ) == 0 ) { //
              system( "sudo -n icdtcp3-update-sw --force /tmp/update.img " //
                  "&& sleep 3 && ict-shutdown --reboot &" );
              std::cerr << ":: FORCE update with file: "
                << "/tmp/update.img" << std::endl;
            } else {
              s="sudo -n icdtcp3-update-sw --type=";
              s+=(*(rupdate.response->NewVersion));
              s+=" /tmp/update.img && sleep 3 && ict-shutdown --reboot &"; //
              system( s.c_str() );
              std::cerr << ":: update with file: "
                << "/tmp/update.img, cmd: " << s << std::endl;
            }
          }
        }
      }
      return 1;
    }
};

int main( int argc, char *argv[] ) {

  Clog log;
  std::string s;
  int db_timeout = 60000;
  bool end = false;
  int ans;

  //parametry uruchomienia
  struct option long_options[] = {
    { "db", required_argument, 0, 'd' },
    { "timeout", required_argument, 0, 't' },
    { "log", optional_argument, 0, 'l' },
    { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
    { 0, 0, 0, 0 }
  };

  while( end==false ) {
    int option_index = 0;
    // int ch=getopt_long(argc, argv, "d:t:l::hv", long_options, &option_index);
    int ch=getopt_long(argc, argv, "t:l::hv", long_options, &option_index);
    if (ch == -1)
      break;
    switch(ch) {
      // case 'd':
      // s = optarg;
      // break;
      case 't':
        db_timeout = strtol(optarg, NULL, 0);
        break;
      case 'l':
        //printf("%s", optarg);
        if( strcmp(optarg, "short")==0 ) {
          log._log=Clog::SHORT;
        } else if( strcmp(optarg, "www")==0 ) {
          log._log=Clog::WWW;
        } else if( strcmp(optarg, "long")==0 ) {
          log._log=Clog::LONG;
        }
        break;
      case 'h':
        print_usage(argv[0]);
        end = true;
        break;
      case 'v':
        print_version(argv[0]);
        end = true;
        break;
MIN:
        std::cerr << "Unknown option ` " << char(ch) << " '" << std::endl;
        exit(1);
        break;
    }
  }
  globalConfigDb=new sqlite3cc::conn();
  globalConfigDb->open( std::getenv("ICD_CONFIG_DB") );
  globalConfigDb->busy_timeout( db_timeout );
  globalConfig=new icd::config( *globalConfigDb );

  globalDataDb=new sqlite3cc::conn();
  globalDataDb->open( std::getenv("ICD_DATA_DB") );
  globalDataDb->busy_timeout( db_timeout );


  std::fstream sendstat("/tmp/last-send-status", std::ios_base::in);
  if( sendstat.is_open() ) {//background communication
    char x;
    sendstat >> x;
    if( x=='?' ) {
      log.errTrInProgress();
      exit(1);
    }
    sendstat.close();
  }


  CicdTransfer tr( &log );
  if( !tr.login() ) {
    exit(1);
  }
  if( !tr.getTime() ) {
    exit(1);
  }
  if( !tr.transfer() ) {
    exit(1);
  }
  if( !tr.chkUpdate() ) {
    exit(1);
  }
  if( !tr.logout() ) {
    exit(1);
  }
  if( !tr.doUpdate() ) {
    exit(1);
  }
  tr.done();

  return 0;
}
