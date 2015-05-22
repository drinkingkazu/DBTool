#ifndef DBTOOL_DBENV_H
#define DBTOOL_DBENV_H


#include <string>

namespace ubpsql{

  /// ENUM to define allowed connection user type
  enum DBI_USER_t {    
    kCUSTOM_USER=0,  ///< for custom user connection
    kUBDAQ_READER,   ///< for READER access to DAQ database
    kUBDAQ_WRITER,   ///< for WRITER access to DAQ database
    kDBI_USER_MAX
  };

  /// Default DB Connection Info String
  const std::string DBI_CONFIG[kDBI_USER_MAX] =
    {
      /// kCUSTOM_USER
      "host=localhost dbname=testdb user=yuntse connect_timeout=5",
      /// kUBDAQ_READER
      "host=192.168.144.189 dbname=rctestskazu user=uboonedaq_writer password=argon!uBooNE connect_timeout=5",
      /// kUBDAQ_WRITER
      "host=192.168.144.189 dbname=rctestskazu user=uboonedaq_writer password=argon!uBooNE connect_timeout=5"
    };

  /// Special key string to name a parameter set
  const std::string kPSET_NAME_KEY("pset_name");

  /// Special key string to have a prefix of the name of a parameter set
  const std::string kPSET_PREFIX_KEY("pset_prefix");

  /// Empty string expression
  const std::string kEMPTY_STRING_PARAM("");

}

#endif
