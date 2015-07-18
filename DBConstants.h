
#ifndef DBTOOL_DBCONSTANTS_H
#define DBTOOL_DBCONSTANTS_H

#include <limits>
#include <climits>
#include <TTimeStamp.h>
namespace ubpsql {

  static const unsigned int kINVALID_UINT = std::numeric_limits<unsigned int>::max();

  static const unsigned int kINVALID_USHORT = std::numeric_limits<unsigned short>::max();

  static const TTimeStamp   kINVALID_TIMESTAMP((time_t)0,(int)0);

  /// Special key string to name a parameter set
  const std::string kPSET_NAME_KEY("pset_name");

  /// Special key string to name a parameter set
  const std::string kPSET_NAME_PREFIX_KEY("pset_prefix");

  /// Empty string expression
  const std::string kEMPTY_STRING_PARAM("");

}

#endif
