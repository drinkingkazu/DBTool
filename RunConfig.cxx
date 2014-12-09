#ifndef DBTOOL_RUNCONFIG_CXX
#define DBTOOL_RUNCONFIG_CXX

#include "RunConfig.h"

namespace ubpsql {

  void RunConfig::ls() const
  {
    std::cout
      << "  Main-Config Name: " << fName.c_str() << std::endl
      << "  Listing Sub-Config IDs... " << std::endl;
    for(auto const& d : _data)
      std::cout 
	<< "    Sub-Config: \"" << d.first.c_str() << "\""
	<< " ... ID=" << d.second.ConfigID() 
	<< " ... Mask=" << std::hex << d.second.Mask()
	<< std::dec
	<< std::endl;
    std::cout<<std::endl;

  }

  /// Getter for a Sub-Config
  const ConfigData& RunConfig::Get(const std::string& name) const
  { 
    auto iter = _data.find(name);
    if( iter == _data.end() ) {
      std::ostringstream msg;
      msg << "SubConfig \""
	  << name.c_str()
	  << "\" does not exist!";
      Print(msg::kERROR,__FUNCTION__,msg.str());
      throw ConfigError();
    }
    return (*iter).second;
  }

  /// Sub-Config name list
  std::vector<std::string> RunConfig::List() const
  { 
    std::vector<std::string> res;
    res.reserve(_data.size());
    for(auto const& d : _data) res.push_back(d.first);
    return res;
  }

  
  /// ConfigData inserter
  void RunConfig::AddSubConfig(const ConfigData& cfg) 
  { 
    auto iter = _data.find(cfg.Name());
    if(iter != _data.end()) {
      std::ostringstream msg;
      msg << "SubConfig \""
	  << cfg.Name()
	  << "\" already exists exist!";
      Print(msg::kERROR,__FUNCTION__,msg.str());
      throw ConfigError();
    }
    
    _data.insert(std::make_pair(cfg.Name(),cfg));
  }

}

#endif
