/**
 * @file logger.hpp
 * @author Piotr Mika
 * @date IX 2012r.
 * Class for logs.
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "db-config.h"
#include <poll.h>
#include <fcntl.h>
#include <string>
#include <cstdio>
#include <iostream>
#ifdef DESKTOP//different headers on arm and desktop,
              //use -DDESKTOP durng compilation on desktop 
#include <unistd.h>
#endif

extern icd::config *globalConfig;

/**
 * Interface for printing logs on standard output.
 */
class Clog {
protected:
  char progress[17];
  bool destrLock;

public:
  enum log_type { SILENT, SHORT, LONG };

//protected:
  log_type _log;

//public:
  /**
   * Constructor, creates log interface of log_type.
   * @param log_type Type of logs.
   */
  Clog( log_type log=SILENT );

  /**
   * Destructor, sets end status in db if there is progress status.
   */
  ~Clog();
  
  /**
   * Succesfull creating of parameters.
   * @param percent Precent.
   * @param cmd Name of command which need that parameters.
   */
  void okParams( int percent, const std::string & cmd );
  /**
   * Succesfull SOAP connection.
   * @param percent Precent.
   * @param servQuery Data send to server.
   */
  void okSoap( int percent, const std::string & servQuery );

  /**
   * Positive server answer.
   * @param percent Precent.
   * @param servAns Server answer.
   */
  void okServerAns( int percent, const std::string & servAns );

  /**
   * Error with creating parameters.
   * @param percent Precent.
   * @param cmd Name of command which need that parameters.
   * @param err Error info.
   * @param short_err Short info (7 characters).
   */
  void errParams( int percent, const std::string & cmd, 
      const std::string & err, const std::string & short_err );

  /**
   * Error with SOAP communication.
   * @param percent Precent.
   * @param soapTxt SOAP answer.
   * @param soapCode SOAP code.
   * @param err Error info.
   */
  void errSoap( int percent, const std::string & soapTxt, 
      int soapCode, const std::string & err );

  /**
   * Error with server answer.
   * @param percent Precent.
   * @param servAns String returned by server.
   * @param shortAns Short info (7 characters).
   * @param shortCmd Short command (2 characters).
   * @param errName Error name.
   */
  void errServerAns( int percent, const std::string & servAns, 
      const std::string & shortAns, const std::string & shortCmd,
      const std::string & errName );

  /**
   * Transfer succesfull
   * @param warn Number of warnings (warnings are not critical)
   */
  void done( int warn=0 );
  //0123456789012345//
  //--ERR CR: PARAM.//
  //--ERR SOAP: 123 //
  //--ERR LD: no dev//
  //--ERR GT:0123456//
  //--ERR SD:too old//
  //--ERR AF:       //
  //--ERR LO: I.S.E //
  //--ERR           //
  //--ERR           //
  //++OK Transfer OK//
  
  /**
   * Transfer without success
   * @param err Number of errors
   */
  void error( int err );

  /**
   * Transfer in progress
   */
  void errTrInProgress();


};



#endif
