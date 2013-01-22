#include "version.h"
#include "sqlite3cc.h"
#include "db-config.h"
#include <cstdlib>
#include <string.h>
#include <cstdio>
#include <fstream>


icd::config *globalConfig;
sqlite3cc::conn *globalConfigDb;


void print_version(char *argv0) {
  std::cerr << basename(argv0) << " " << version << "\n"
    << copyright << std::endl;
}

enum job_t { ADD, UPDATE, REMOVE, NONE };


struct CkeyVal {
  std::string key;
  std::string val;
  job_t job;

};

struct Cwifinet {//one network item from config
  std::string ssid;
  std::vector< CkeyVal > values;
  job_t job;

};

class CwifiCfgToDb {
  private:
    std::vector< Cwifinet > networks;
    std::string currentIpSsid;
    job_t ipSsidJob;


  public:
    void readConfigFile( std::string cfgFileName="/tmp/wpa_suppl.conf" ) {
      Cwifinet net;
      CkeyVal keyval;
      std::string s;
      bool inNetwork=false;
      bool inName=false;
      FILE * ipSsid;
      char buf[64];
      int i;

      std::fstream cfgFile( cfgFileName.c_str(), std::ios_base::in );
      cfgFile >> s;
      if( s.compare( "ctrl_interface=/var/run/wpa_supplicant" )!=0 ) {
        exit(1);
      }
      cfgFile >> s;
      if( s.compare( "ctrl_interface_group=wheel" )!=0 ) {
        exit(2);
      }
      cfgFile >> s;
      if( s.compare( "update_config=1" )!=0 ) {
        exit(3);
      }

      cfgFile >> s;
      while( ! cfgFile.eof() ) {
        if( !inNetwork ) {
          if( s.compare( "network={" )==0 ) {
            inNetwork=true;
          } else {
            exit(4);
          }  
        } else {
          if( s.compare( "}" )==0 ) {//end of network
            inNetwork=false;
            net.job=ADD;
            networks.push_back( net );
            net.values.clear();
            net.ssid.clear();
          } else {//in network
            int div=s.find( '=' ); //search of '='
            if( s[div+1] == '"' ) {
              std::string x;
              char c;
              while( s[ s.size()-1 ] != '"' && ( ! cfgFile.eof() ) ) {//spaces in quoted string
                cfgFile >> std::noskipws;
                while( cfgFile.peek()==' ' ) { //ex: >>"My network"<<
                  cfgFile >> c;
                  s+=' ';
                }
                cfgFile >> std::skipws;
                cfgFile >> x;
                s+=x;
              }
            }
            if( (s.substr( 0, div )).compare( "ssid" )==0 ) { //ssid
              net.ssid=s.substr( div+1 );
            } else { //keyval
              keyval.key=s.substr( 0, div );
              keyval.val=s.substr( div+1 );
              keyval.job=ADD;
              net.values.push_back( keyval );
            }
          }
        }
        cfgFile >> s;
      }

      ipSsidJob=UPDATE;
      ipSsid=popen( "wpa_cli -i wlan0 status 2>/dev/null | awk -F '=' ''/'ip_address'/' {print $2}'" , "r" );
      if( ipSsid && fgets( buf, 128, ipSsid ) ) {
        i=0;
        while( buf[i]!=0 && buf[i]!='\n' ) {//find end and remove \n
          ++i;
        }
        buf[i]=0;
        currentIpSsid=buf;
      }
      pclose( ipSsid );

      currentIpSsid+=" -- ";

      ipSsid=popen( "wpa_cli -i wlan0 status 2>/dev/null | grep -v bssid | awk -F '=' ''/'ssid'/' {print $2}'" , "r" );
      if( ipSsid && fgets( buf, 128, ipSsid ) ) {
        i=0;
        while( buf[i]!=0 && buf[i]!='\n' ) {//find end and remove \n
          ++i;
        }
        buf[i]=0;
        currentIpSsid+=buf;
      }
      pclose( ipSsid );

    }
    void readDbConfig() {

      int i;
      int j;
      int k;
      int l;
      bool exists;
      CkeyVal keyval;
      Cwifinet net;
      std::string ssid;                                             //012345678
      std::vector<std::string> dbNetworks = globalConfig->list_like( "wifinet" );

      for( i=0; i<dbNetworks.size(); ++i ) {
        ssid=dbNetworks[i].substr( 8 );
        exists=false;
        for( j=0; j<networks.size(); ++j ) {
          if( ssid.compare( networks[j].ssid )==0 ) {//network exists: update or nothing
            networks[j].job=NONE;
            std::vector<icd::config_entry> entries = globalConfig->list_entries( dbNetworks[i] );

            for( k=0; k<networks[j].values.size(); ++k ) {//for every value
              for( l=0; l<entries.size(); ++l ) {//find connected value from db
                if( networks[j].values[k].key.compare( entries[l].key )==0 ) {
                  if( networks[j].values[k].val.compare( entries[l].value )==0 ) {//items equal
                    networks[j].values[k].job=NONE;
                  } else {//update db value
                    networks[j].values[k].job=UPDATE;
                  }
                  entries[l]=entries[ entries.size()-1 ];//remove used value
                  entries.pop_back();
                  break;
                }
              }
            }
            for( l=0; l<entries.size(); ++l ) {
              keyval.key=entries[l].key;
              keyval.job=REMOVE;//items only in db should be removed
              networks[j].values.push_back( keyval );
            }
            exists=true;
            for( k=0; k<networks[j].values.size(); ++k ) {//check if update is needed
              if( networks[j].values[k].job!=NONE ) {
                networks[j].job=UPDATE;
                break;
              }
            }
            break;
          }
        }
        if( exists!=true ) {//remove from db 
          net.ssid=ssid;
          net.job=REMOVE;
          networks.push_back( net );
        }
      }
      if( currentIpSsid.compare( globalConfig->entry( "wifi", "ip-ssid" ) )==0 ) {
        ipSsidJob=NONE;
      }
    }
    void Update() {
      std::string s;
      int i;
      int j;

      globalConfig->begin_transaction();

      for(i=0; i<networks.size(); ++i ) {
        s="wifinet-";
        s+=networks[i].ssid;
        switch( networks[i].job ) {
          case ADD:
            globalConfig->add_section( s );
            for( j=0; j<networks[i].values.size(); ++j ) {
              globalConfig->set_entry( s, networks[i].values[j].key, networks[i].values[j].val );
            }
            break;
          case UPDATE:
            for( j=0; j<networks[i].values.size(); ++j ) {
              switch( networks[i].values[j].job ) {
                case ADD:
                case UPDATE:
                  globalConfig->set_entry( s, networks[i].values[j].key, networks[i].values[j].val );
                  break;
                case REMOVE:
                  globalConfig->remove_entry( s, networks[i].values[j].key );
                  break;
              }
            }
            break;
          case REMOVE:
            globalConfig->remove_section( s );
            break;
        }
      }

      if( ipSsidJob==UPDATE ) {
        globalConfig->set_entry( "wifi", "ip-ssid", currentIpSsid );
      }

      globalConfig->commit_transaction();
    }

};

int main( int argc, char *argv[] ) {

  int db_timeout = 60000; 

  globalConfigDb=new sqlite3cc::conn();
  globalConfigDb->open( std::getenv("ICD_CONFIG_DB") );
  globalConfigDb->busy_timeout( db_timeout );
  globalConfig=new icd::config( *globalConfigDb );

  CwifiCfgToDb cfg;

  cfg.readConfigFile();
  cfg.readDbConfig();
  cfg.Update();

  return 0;
}
