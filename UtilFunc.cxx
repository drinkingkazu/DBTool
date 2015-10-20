#ifndef DBTOOL_UTILFUNC_CXX
#define DBTOOL_UTILFUNC_CXX

#include "UtilFunc.h"
#include "DBEnv.h"
#include "DBException.h"
#include <iostream>
namespace ubpsql {
  
  int RunTypeFromName(const std::string name) 
  { 
    for(size_t i=0; i<kRunTypeMax; ++i) {

      if( name == kRunTypeName[i] ) return (RunType_t)i;

    }
    std::cout<<"\033[91m<<ERROR>>\033[00m No such run type name: "<<name.c_str()<<std::endl;
    throw DBException();
    return -1;
  }

  const std::string& RunTypeName(const int type)
  {
    if(type >= kRunTypeMax) {
      std::cout << "\033[91m<<ERROR>>\033[00m No such run type" << type << std::endl;      
      throw DBException();
    }
    return kRunTypeName[type];
  }

}
#endif
