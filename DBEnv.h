#ifndef DBTOOL_DBENV_H
#define DBTOOL_DBENV_H

#include <string>
#include <vector>
namespace ubpsql{

  /// ENUM to define allowed connection user type
  enum DBI_USER_t {    
    kUBDAQ_READER=0, ///< for READER access to DAQ database
    kUBDAQ_WRITER,   ///< for WRITER access to DAQ database
    kUBDAQ_ADMIN,    ///< for ADMIN access to DAQ database
    kDBI_USER_MAX
  };

  /// ENUM to define a run type
  enum RunType_t {
    kPHYSICS=0,   ///< PHYSICS dude
    kCALIBRATION, ///< CALIBRATION ma'am
    kTEST,        ///< TEST sucker
    kRunTypeMax   ///< max
  };

  /// RunType names
  const std::string kRunTypeName[kRunTypeMax] = {"PHYSICS","CALIBRATION","TEST"};
}

#endif
