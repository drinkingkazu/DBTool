#ifndef DBTOOL_DBBASE_CXX
#define DBTOOL_DBBASE_CXX

#include "DBBase.h"

namespace ubpsql {

  void DBBase::Print(const msg::MSGLevel_t level,
		     const std::string& msg1,
		     const std::string& msg2) const 
  { 
    if(level < _msg_level) return;

    switch(level){
    case msg::kDEBUG:
      std::cout << "\033[1;34;40m"   << "[DEBUG]  " << "\033[0m "; break;
    case msg::kINFO:
      std::cout << "\033[1;35;40m"   << "[INFO]   " << "\033[0m "; break;
    case msg::kNORMAL:
      std::cout << "\033[95m"        << "[NORMAL] " << "\033[0m "; break;
    case msg::kWARNING:
      std::cout << "\033[5;1;33;40m" << "[WARNING]" << "\033[0m "; break;
    case msg::kERROR:
      std::cout << "\033[5;1;33;41m" << "[ERROR]  " << "\033[0m "; break;
    }
    if(msg2.empty())
      
      std::cout << msg1.c_str() << std::endl;

    else
      std::cout << "<< " << msg1.c_str() << " >> " 
		<< msg2.c_str() << std::endl;
    
  }

}

#endif
