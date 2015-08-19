#ifndef DBTOOL_UTILFUNC_H
#define DBTOOL_UTILFUNC_H

#include <string>
namespace ubpsql {
  
  int RunTypeFromName(const std::string name);

  const std::string& RunTypeName(const int type);

}
#endif
