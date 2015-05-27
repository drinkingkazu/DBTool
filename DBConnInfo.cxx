#ifndef DBTOOL_DBCONNINFO_CXX
#define DBTOOL_DBCONNINFO_CXX

#include "DBConnInfo.h"
#include <sstream>
namespace ubpsql {
  
  DBConnInfo* DBConnInfo::_me = nullptr;

  DBConnInfo::DBConnInfo()
  {
    _conn_str_v.clear();
    _conn_str_v.resize(kDBI_USER_MAX);

    std::ostringstream ostr;
    if(getenv("DBTOOL_CUSTOM_HOST"))
      ostr << "host=" << getenv("DBTOOL_CUSTOM_HOST") << " ";
    if(getenv("DBTOOL_CUSTOM_DB"))
      ostr << "dbname=" << getenv("DBTOOL_CUSTOM_DB") << " ";
    if(getenv("DBTOOL_CUSTOM_USER"))
      ostr << "user=" << getenv("DBTOOL_CUSTOM_USER") << " ";
    if(getenv("DBTOOL_CUSTOM_PASS"))
      ostr << "password=" << getenv("DBTOOL_CUSTOM_PASS") << " ";
    if(getenv("DBTOOL_CUSTOM_TIMEOUT"))
      ostr << "connect_timeout=" << getenv("DBTOOL_CUSTOM_TIMEOUT") << " ";

    std::string str(ostr.str());

    if(!str.empty()) {
      for(size_t i=0; i<_conn_str_v.size(); ++i)
	_conn_str_v[i] = str;
      return;
    }

    ostr.str("");
    if(getenv("DBTOOL_READER_HOST"))
      ostr << "host=" << getenv("DBTOOL_READER_HOST") << " ";
    if(getenv("DBTOOL_READER_DB"))
      ostr << "dbname=" << getenv("DBTOOL_READER_DB") << " ";
    if(getenv("DBTOOL_READER_USER"))
      ostr << "user=" << getenv("DBTOOL_READER_USER") << " ";
    if(getenv("DBTOOL_READER_PASS"))
      ostr << "password=" << getenv("DBTOOL_READER_PASS") << " ";
    if(getenv("DBTOOL_READER_TIMEOUT"))
      ostr << "connect_timeout=" << getenv("DBTOOL_READER_TIMEOUT") << " ";
    _conn_str_v[kUBDAQ_READER] = ostr.str();

    ostr.str("");
    if(getenv("DBTOOL_WRITER_HOST"))
      ostr << "host=" << getenv("DBTOOL_WRITER_HOST") << " ";
    if(getenv("DBTOOL_WRITER_DB"))
      ostr << "dbname=" << getenv("DBTOOL_WRITER_DB") << " ";
    if(getenv("DBTOOL_WRITER_USER"))
      ostr << "user=" << getenv("DBTOOL_WRITER_USER") << " ";
    if(getenv("DBTOOL_WRITER_PASS"))
      ostr << "password=" << getenv("DBTOOL_WRITER_PASS") << " ";
    if(getenv("DBTOOL_WRITER_TIMEOUT"))
      ostr << "connect_timeout=" << getenv("DBTOOL_WRITER_TIMEOUT") << " ";
    _conn_str_v[kUBDAQ_WRITER] = ostr.str();

    ostr.str("");
    if(getenv("DBTOOL_ADMIN_HOST"))
      ostr << "host=" << getenv("DBTOOL_HOST") << " ";
    if(getenv("DBTOOL_ADMIN_DB"))
      ostr << "dbname=" << getenv("DBTOOL_ADMIN_DB") << " ";
    if(getenv("DBTOOL_ADMIN_USER"))
      ostr << "user=" << getenv("DBTOOL_ADMIN_USER") << " ";
    if(getenv("DBTOOL_ADMIN_PASS"))
      ostr << "password=" << getenv("DBTOOL_ADMIN_PASS") << " ";
    if(getenv("DBTOOL_ADMIN_TIMEOUT"))
      ostr << "connect_timeout=" << getenv("DBTOOL_ADMIN_TIMEOUT") << " ";
    _conn_str_v[kUBDAQ_ADMIN] = ostr.str();

  }
}
#endif
