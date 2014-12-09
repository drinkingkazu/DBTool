#ifndef DBTOOL_DBAPPBASE_CXX
#define DBTOOL_DBAPPBASE_CXX

#include "DBAppBase.h"

namespace ubpsql {

  bool DBAppBase::Connect(){
    
    if(!_conn) {
      _conn = (DBConn*)(DBConn::GetME(_user_type));
      _conn->SetVerbosity(_msg_level);
    }

    if(_conn->Connect()) return true;

    throw ConnectionError();
    
    return false;
  }

}

#endif
