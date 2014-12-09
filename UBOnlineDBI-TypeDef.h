//
// type-def file for ROOT
//

#ifndef DBTOOL_UBONLINEDBI_TYPEDEF_H
#define DBTOOL_UBONLINEDBI_TYPEDEF_H

// Header files included here
#include "DBEnv.h"
#include "PQMessenger.h"
#include "DBException.h"
#include "DBConn.h"
#include "ConfigParams.h"
#include "ConfigData.h"
#include "ConfigReader.h"
#include "ConfigWriter.h"

// Class forward declaration here
namespace ubpsql{

  // Basic classes
  class DBException;     // Exception base
  class InvalidUser;     // Exception implementation for invalid user
  class ConnectionError; // Exception implementation for connection error
  class DBBase;
  class DBAppBase;

  // Connection handler
  class DBConn;

  // Application classes
  class PQMessenger;
  class ConfigReader;    // Run configuration database read interface
  class ConfigWriter;    // Run configuration database write interface

  class ConfigData;
  class ConfigParams;
  class RunConfig;
}
//ADD_EMPTY_CLASS ... do not change this comment line
#endif







