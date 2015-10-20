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
    kPhysics=0,        ///< PHYSICS dude
    kBeamOff,          ///< When the beam is off
    kTPCCalibration,   ///< TPC CALIBRATION ma'am
    kPMTCalibration,   ///< PMT CALIBRATION ma'am
    kLaserCalibration, ///< Laser CALIBRATION ma'am
    kNoise,            ///< Noise sir
    kTest,             ///< TEST sucker
    kUnknown,          ///< who cares?
    kRunTypeMax        ///< max
  };

  /// RunType names
  const std::string kRunTypeName[kRunTypeMax] = {"PHYSICS","CALIBRATION","TEST"};
}

#endif
