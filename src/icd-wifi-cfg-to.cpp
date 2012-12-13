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

  public:
    void readConfigFile( std::string cfgFileName="/tmp/wpa_suppl.conf" ) {
      Cwifinet net;
      CkeyVal keyval;
      std::string s;
      bool inNetwork=false;
      bool inName=false;

      std::fstream cfgFile( cfgFileName.c_str(), std::ios_base::in );
      cfgFile >> s; //ctrl_interface=/var/run/wpa_supplicant
      cfgFile >> s; //ctrl_interface_group=wheel
      cfgFile >> s; //update_config=1

      cfgFile >> s;
      while( ! cfgFile.eof() ) {
        if( !inNetwork ) {
          if( s.compare( "network={" )==0 ) {
            inNetwork=true;
          } else {
            exit(1);
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

    }
    void readDbConfig() {
    }
    void Update() {
      for( int i=0; i<networks.size(); ++i ) {
        std::cout << "net: " << networks[i].ssid << std::endl;
        for( int j=0; j<networks[i].values.size(); ++j ) {
          std::cout << "  " << networks[i].values[j].key << " : "
            << networks[i].values[j].val << std::endl;
        }
      }
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
