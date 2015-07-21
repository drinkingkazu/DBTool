
#ifndef DBTOOL_DBCONSTANTS_H
#define DBTOOL_DBCONSTANTS_H

#include <limits>
#include <climits>
#include <TTimeStamp.h>
namespace ubpsql {

  static const unsigned int   kINVALID_UINT   = std::numeric_limits<unsigned int>::max();

  static const int            kINVALID_INT    = std::numeric_limits<int>::max();

  static const unsigned long  kINVALID_ULONG  = std::numeric_limits<unsigned long>::max();

  static const unsigned short kINVALID_USHORT = std::numeric_limits<unsigned short>::max();

  static const double kINVALID_DOUBLE = std::numeric_limits<double>::max();
  
  static const short  kINVALID_SHORT  = std::numeric_limits<short>::max();

  static const time_t kINVALID_TIME   = std::numeric_limits<time_t>::max();

  static const TTimeStamp   kINVALID_TIMESTAMP((time_t)0,(int)0);

  /// Special key string to name a parameter set
  const std::string kPSET_NAME_KEY("pset_name");

  /// Special key string to name a parameter set
  const std::string kPSET_NAME_PREFIX_KEY("pset_prefix");

  /// Empty string expression
  const std::string kEMPTY_STRING_PARAM("");

}

#endif
