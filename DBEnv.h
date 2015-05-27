#ifndef DBTOOL_DBENV_H
#define DBTOOL_DBENV_H


#include <string>

namespace ubpsql{

  /// ENUM to define allowed connection user type
  enum DBI_USER_t {    
    kUBDAQ_READER=0, ///< for READER access to DAQ database
    kUBDAQ_WRITER,   ///< for WRITER access to DAQ database
    kUBDAQ_ADMIN,    ///< for ADMIN access to DAQ database
    kDBI_USER_MAX
  };

}

#endif
