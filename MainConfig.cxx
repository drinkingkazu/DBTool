#ifndef DBTOOL_MAINCONFIG_CXX
#define DBTOOL_MAINCONFIG_CXX

#include "MainConfig.h"
#include "DBException.h"
namespace ubpsql {

  void MainConfig::ls() const
  {
    std::cout
      << "  Main-Config Name: " << fName.c_str() << std::endl
      << "  Listing Sub-Config IDs... " << std::endl;
    for(auto const& d : _data)
      std::cout 
	<< "    Sub-Config: \"" << d.second.ID().Name().c_str() << "\""
	<< " ... ID=" << d.second.ID().ID()
	<< std::dec
	<< std::endl;
    std::cout<<std::endl;

  }

  const SubConfig& MainConfig::Get(const std::string& name) const
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

  std::string MainConfig::Dump(const bool stack_main_cfg_name) const
  {
    std::string res;

    for(auto const& name_cfg : _data)
      res += name_cfg.second.Dump();

    if(stack_main_cfg_name)
      res = this->Name() + " : {\n" + res + "\n}\n";
    return res;
  }

  std::vector<std::string> MainConfig::ListSubConfigs() const
  { 
    std::vector<std::string> res;
    res.reserve(_data.size());
    for(auto const& d : _data) res.push_back(d.first);
    return res;
  }

  std::vector<ubpsql::SubConfigID> MainConfig::ListSubConfigIDs() const
  {
    std::vector<ubpsql::SubConfigID> res;
    res.reserve(_data.size());
    for(auto const& d : _data) res.push_back(d.second.ID());
    return res;
  }

  void MainConfig::AddSubConfig(const SubConfig& cfg) 
  { 
    auto iter = _data.find(cfg.ID().Name());
    if(iter != _data.end()) {
      std::ostringstream msg;
      msg << "SubConfig \""
	  << cfg.ID().Name()
	  << "\" already exists exist!";
      Print(msg::kERROR,__FUNCTION__,msg.str());
      throw ConfigError();
    }
    
    _data.insert(std::make_pair(cfg.ID().Name(),cfg));
  }

}

#endif
