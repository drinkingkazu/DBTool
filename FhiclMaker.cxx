#ifndef DBTOOL_FHICLMAKER_CXX
#define DBTOOL_FHICLMAKER_CXX

#include "FhiclMaker.h"

namespace ubpsql{

  FPSet::FPSet(std::string name)
    : _name(name)
  {
    if(_name.empty()) {
      Print(msg::kERROR,__FUNCTION__,"Cannot create FPSet w/ empty name..");
      throw FhiclError();
    }
  }
  
  void FPSet::append(const std::string& key, 
		     const std::string& param)
  { 
    auto const leaf_iter = _leaf.find(key);
    if(leaf_iter != _leaf.end()) {
      Print(msg::kWARNING,__FUNCTION__,
	    Form("Overriding key \"%s\" (\"%s\"=>\"%s\")",
		 key.c_str(),param.c_str(),(*leaf_iter).second.c_str())
	    );
    }    
    auto const node_iter = _node.find(key);
    if(node_iter != _node.end()){
      Print(msg::kWARNING,__FUNCTION__,
	    Form("Overriding key \"%s\" (FPSet=>\"%s\")",
		 key.c_str(),param.c_str())
	    );
      _node.erase(key);
    }
    _leaf[key]=param;
    return;
  }
  
  void FPSet::append(const std::string& key,
		     const FPSet& pset)
  {
    auto const leaf_iter = _leaf.find(key);
    if(leaf_iter != _leaf.end()) {
      Print(msg::kWARNING,__FUNCTION__,
	    Form("Overriding key \"%s\" (\"%s\"=>FPSet)",
		 key.c_str(),(*leaf_iter).second.c_str())
	    );
      _leaf.erase(key);
    }
    auto const node_iter = _node.find(key);
    if(node_iter != _node.end()){
      Print(msg::kWARNING,__FUNCTION__,
	    Form("Overriding key \"%s\" (FPSet=>FPSet\")",
		 key.c_str())
	    );
    }
    _node[key]=pset;
    return;
  }
  
  void FPSet::append(const std::string& key,
		     const std::map<std::string,std::string>& pset)
  {
    FPSet p(key);
    for(auto const& key_value : pset)
      p.append(key_value.first,key_value.second);
    append(key,p);
  }

  FhiclMaker::FhiclMaker() : _pset("fhicl")
  {}

  void FhiclMaker::set(const RunConfig& cfg)
  {
    _pset.clear(cfg.Name());
  }
}

#endif
