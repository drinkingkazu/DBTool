#ifndef DBTOOL_DBCONINFO_H
#define DBTOOL_DBCONINFO_H

#include <vector>
#include <string>
#include "DBEnv.h"
namespace ubpsql {

  class DBConnInfo {
    
  private:
    DBConnInfo();
    static DBConnInfo* _me;
    std::vector<std::string> _conn_str_v;

  public:
    static const DBConnInfo& Get()
    {
      if(!_me) _me = new DBConnInfo;
      return (*_me);
    }

    const std::string& GetConnString(const DBI_USER_t type) const
    { return _conn_str_v[type]; }
  
  };
}

#endif
