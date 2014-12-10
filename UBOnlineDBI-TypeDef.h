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
#include "CParams.h"
#include "SubConfig.h"
#include "RunConfig.h"
#include "ConfigReader.h"
#include "ConfigWriter.h"
#include "FhiclMaker.h"
// Class forward declaration here
namespace ubpsql{

  // Basic classes
  class DBException;    
  class InvalidUser;    
  class ConnectionError;
  class TableDataError; 
  class ConfigError;
  class FhiclError;
  class DBBase;
  class DBAppBase;

  // Connection handler
  class DBConn;

  // Application classes
  class PQMessenger;
  class ConfigReader;    // Run configuration database read interface
  class ConfigWriter;    // Run configuration database write interface

  class CParams;
  class CParamsKey;
  class SubConfig;
  class RunConfig;
  class FhiclMaker;
  class FPSet;
}
//ADD_EMPTY_CLASS ... do not change this comment line
#endif








