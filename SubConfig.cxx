#ifndef DBTOOL_SUBCONFIG_CXX
#define DBTOOL_SUBCONFIG_CXX

#include "SubConfig.h"
#include "DBConstants.h"
#include "DBException.h"
namespace ubpsql{

  SubConfigID::SubConfigID(const std::string& name,
			   const unsigned int id)
    : std::pair<std::string,unsigned int>(name,id)
  {
    if(name.empty()) {
      std::cerr<<"\033[91m[ERROR]\033[00m Cannot have an empty SubConfig name..." << std::endl;
      throw ConfigError();
    }
  }
  
  SubConfig::SubConfig(const std::string& name,
		       const unsigned int id)
    : _id(name,id)
  {}
  
  void SubConfig::Append(const std::string& key, 
			 const std::string& param)
  {
    auto const leaf_iter = _leaf.find(key);
    if(leaf_iter != _leaf.end()) {
      Print(msg::kERROR,__FUNCTION__,
	    Form("Cannot override key \"%s\" (Already a key for a value \"%s\")",
		 key.c_str(),(*leaf_iter).second.c_str())
	    );
      throw ConfigError();
    }

    auto const node_iter = _node.find(key);
    if(node_iter != _node.end()){
      Print(msg::kERROR,__FUNCTION__,
	    Form("Cannot override key \"%s\" (Already a SubConfig w/ ID = %d)",
		 key.c_str(),
		 (*node_iter).second.ID().ID())
	    );
      throw ConfigError();
    }

    _leaf.emplace(key,param);
    return;
  }
  
  void SubConfig::Append(const SubConfig& scfg)
  {
    auto const leaf_iter = _leaf.find(scfg.ID().Name());
    if(leaf_iter != _leaf.end()) {
      Print(msg::kERROR,__FUNCTION__,
	    Form("Cannot override key \"%s\" (Already a key for a value \"%s\")",
		 scfg.ID().Name().c_str(),(*leaf_iter).second.c_str())
	    );
      throw ConfigError();
    }

    auto const node_iter = _node.find(scfg.ID().Name());
    if(node_iter != _node.end()){
      Print(msg::kERROR,__FUNCTION__,
	    Form("Cannot override key \"%s\" (Already a SubConfig w/ ID = %d)",
		 scfg.ID().Name().c_str(),
		 (*node_iter).second.ID().ID())
	    );
      throw ConfigError();
    }

    // Xi cannot depend on Xj
    if(scfg.ID().Name() == this->ID().Name()) {
      Print(msg::kERROR,__FUNCTION__,
	    "Cannot depend on the same sub-configuration type!");
      throw ConfigError();
    }

    auto dep_b = scfg.Dependencies();      
    for(auto const& id : dep_b) {
      if(id.Name() == this->ID().Name()) {
	Print(msg::kERROR,__FUNCTION__,
	      "Cannot depend on the same sub-configuration type!");
	throw ConfigError();
      }
    }

    // Yj should not appear twice
    auto dep_a = this->Dependencies();

    for(auto const& id : dep_a) {
      if(dep_b.find(id) != dep_b.end()) {
	Print(msg::kERROR,__FUNCTION__,
	      Form("Duplicate dependency found (Name=\"%s\",ID=%d)!",id.Name().c_str(),id.ID()));
	throw ConfigError();
      }
    }

    _node.insert(std::make_pair(scfg.ID().Name(),scfg));
    return;
  }
  
  void SubConfig::Dump(std::string& content,size_t level) const
  {
    std::string space;
    for(size_t i=0; i<(level*2); ++i) space += " ";
    if ( level == 0 )
      content += (space + this->ID().Name() + ": [  {\n");
    else
      content += (space + this->ID().Name() + ":{\n");

    for(auto const& key_value : _leaf)
      
      content += (space + "  " + key_value.first + " : " + key_value.second + "\n");
    
    for(auto const& key_scfg : _node)

      key_scfg.second.Dump(content,(level+1));

    if ( level == 0 )
      content += (space + "}  ]\n");
    else
      content += (space + "}\n");
  }

  std::vector<std::string> SubConfig::ListParameters() const
  {
    std::vector<std::string> res;
    res.reserve(_leaf.size());
    for(auto const& name_value : _leaf)
      res.push_back(name_value.first);
    return res;
  }
  
  const std::map<std::string,std::string>& SubConfig::Parameters() const
  {
    return _leaf;
  }

  std::vector<std::string> SubConfig::ListSubConfigs() const
  {
    std::vector<std::string> res;
    res.reserve(_node.size());

    for(auto const& name_scfg : _node)
      res.push_back(name_scfg.first);
    return res;
  }

  std::vector< ubpsql::SubConfigID > SubConfig::ListSubConfigIDs() const
  {
    std::vector< ubpsql::SubConfigID > res;
    res.reserve(_node.size());

    for(auto const& name_scfg : _node)

      res.push_back(name_scfg.second.ID());

    return res;
  }
  
  std::set< ubpsql::SubConfigID > SubConfig::Dependencies() const
  {
    std::set< ubpsql::SubConfigID > res;
    for(auto const& name_scfg : _node) {
      res.insert(name_scfg.second.ID());
      for(auto const& child : name_scfg.second.Dependencies())
	res.insert(child);
    }
    return res;
  }

  const SubConfig& SubConfig::GetSubConfig(const std::string node_name) const
  {
    auto iter = _node.find(node_name);
    if(iter == _node.end()){
      Print(msg::kERROR,__FUNCTION__,Form("Node: \"%s\" does not exist...",node_name.c_str()));
      throw ConfigError();
    }
    return (*iter).second;
  }

}

#endif
