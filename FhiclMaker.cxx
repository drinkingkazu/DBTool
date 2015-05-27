#ifndef DBTOOL_FHICLMAKER_CXX
#define DBTOOL_FHICLMAKER_CXX

#include "FhiclMaker.h"
#include "DBConstants.h"
#include "DBException.h"
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
  
  void FPSet::append(const FPSet& pset)
  {
    auto const leaf_iter = _leaf.find(pset.name());
    if(leaf_iter != _leaf.end()) {
      Print(msg::kWARNING,__FUNCTION__,
	    Form("Overriding key \"%s\" (\"%s\"=>FPSet)",
		 pset.name().c_str(),(*leaf_iter).second.c_str())
	    );
      _leaf.erase(pset.name());
    }
    auto const node_iter = _node.find(pset.name());
    if(node_iter != _node.end()){
      Print(msg::kWARNING,__FUNCTION__,
	    Form("Overriding key \"%s\" (FPSet=>FPSet\")",
		 pset.name().c_str())
	    );
    }
    _node.insert(std::make_pair(pset.name(),pset));
    return;
  }
  
  void FPSet::append(const std::string& key,
		     const std::map<std::string,std::string>& pset)
  {
    FPSet p(key);
    for(auto const& key_value : pset)
      p.append(key_value.first,key_value.second);
    append(p);
  }

  void FPSet::dump(std::string& content,size_t level) const
  {
    std::string space;
    for(size_t i=0; i<(level*2); ++i) space += " ";
    content += (space + name() + ":{\n");
    for(auto const& key_value : _leaf)
      
      content += (space + "  " + key_value.first + " : " + key_value.second + "\n");
    
    for(auto const& key_pset : _node)

      key_pset.second.dump(content,(level+1));

    content += (space + "}\n");
  }

  FhiclMaker::FhiclMaker() 
  {}

  FPSet& FhiclMaker::CratePSet( FPSet& pset,
				const std::string& name )
  {
    if(pset._node.find(name) == pset._node.end()) {
      FPSet tmp(name);
      pset.append(tmp);
    }
    return pset._node[name];
  }
  
  FPSet& FhiclMaker::SlotPSet ( FPSet& pset,
				const std::string& crate_name,
				const std::string& slot_name )
  {
    auto& crate_pset = CratePSet(pset,crate_name);
    if(crate_pset._node.find(slot_name) == crate_pset._node.end()) {
      FPSet tmp(slot_name);
      crate_pset.append(tmp);
    }
    return crate_pset._node[slot_name];
  }
  
  FPSet& FhiclMaker::ChannelPSet ( FPSet& pset,
				   const std::string& crate_name,
				   const std::string& slot_name,
				   const std::string& channel_name)
  {
    auto& slot_pset = SlotPSet(pset,crate_name,slot_name);
    if(slot_pset._node.find(channel_name) == slot_pset._node.end()) {
      FPSet tmp(channel_name);
      slot_pset.append(tmp);
    }    
    return slot_pset._node[channel_name];
  }

  FPSet& FhiclMaker::PSet( FPSet& pset,
			   const std::string& crate_name,
			   const std::string& slot_name,
			   const std::string& channel_name )
  {
    if( crate_name.empty() ) {
      Print(msg::kERROR,__FUNCTION__,
	    "Cannot find FPSet for empty crate key!");
      throw FhiclError();
    }
    if(slot_name.empty() && !channel_name.empty()) {
      Print(msg::kERROR,__FUNCTION__,
	    "Cannot find FPSet for empty slot & non-empty channel key!");
      throw FhiclError();
    }

    if(slot_name.empty() && channel_name.empty()) return CratePSet(pset,crate_name);
    if(channel_name.empty()) return SlotPSet(pset,crate_name,slot_name);
    return ChannelPSet(pset,crate_name,slot_name,channel_name);
  }

  FPSet FhiclMaker::FhiclParameterSet(const SubConfig& sub_cfg)
  {
    FPSet pset(sub_cfg.Name());

    // Get crate/slot/channel name
    std::map<int,std::string> crate_name_m;
    std::map<int,std::map<int,std::string> > slot_name_m;
    std::map<int,std::map<int,std::map<int,std::string> > > channel_name_m;
    
    for(auto const& key_param : sub_cfg) {
      
      auto const& key    = key_param.first;
      auto const& params = key_param.second;
      
      if(key.IsDefaultCrate() || key.IsDefaultSlot() || key.IsDefaultChannel())
	continue;
      if(key.IsCrate()) {
	crate_name_m[key.Crate()] = params.Name();
	CratePSet(pset,params.Name());
      }
      else if(key.IsSlot())
	slot_name_m[key.Crate()][key.Slot()] = params.Name();
      else if(key.IsChannel())
	channel_name_m[key.Crate()][key.Slot()][key.Channel()] = params.Name();
      else{
	Print(msg::kERROR,__FUNCTION__,
	      Form("Found ill-defined CParamsKey!"));
	throw ConfigError();
      }
    }
    
    for(auto const& crate_slot_pair : slot_name_m) {
      auto const& crate = crate_slot_pair.first;
      auto const& slot_name = crate_slot_pair.second;
      for(auto const& id_name : slot_name) {
	SlotPSet(pset,crate_name_m[crate],
		 id_name.second);
      }
    }
    
    for(auto const& crate_slot_pair : channel_name_m) {
      auto const& crate = crate_slot_pair.first;
      auto const& slot_channel = crate_slot_pair.second;
      for(auto const& slot_ch_pair : slot_channel) {
	auto const& slot = slot_ch_pair.first;
	auto const& ch_name = slot_ch_pair.second;
	for(auto const& id_name : ch_name){
	  ChannelPSet(pset,
		      crate_name_m[crate],
		      slot_name_m[crate][slot],
		      id_name.second);
	}
      }
    }
    
    // Insert a configuration
    for(auto const& key_param : sub_cfg) {
      
      auto const& key    = key_param.first;
      auto const& params = key_param.second;
      
      if(key.IsDefaultCrate() || key.IsDefaultSlot() || key.IsDefaultChannel())
	continue;
      
      auto const& crate_name = crate_name_m[key.Crate()];
      std::string slot_name, channel_name;
      if(!key.IsCrate())
	slot_name = slot_name_m[key.Crate()][key.Slot()];
      if(key.IsChannel())
	channel_name = channel_name_m[key.Crate()][key.Slot()][key.Channel()];
      
      auto& target_pset = this->PSet(pset,crate_name,slot_name,channel_name);
      
      for(auto const& key_value : params){
	if(key_value.first == kPSET_NAME_KEY) continue;
	if(key_value.first == kPSET_NAME_PREFIX_KEY) continue;
	target_pset.append(key_value.first,key_value.second);
      }
    }
    return pset;
  }

  FPSet FhiclMaker::FhiclParameterSet(const RunConfig& cfg)
  {
    FPSet pset(cfg.Name());

    for(auto const& sub_cfg_name : cfg.List()) {

      auto sub_cfg_pset = this->FhiclParameterSet(cfg.Get(sub_cfg_name));

      pset.append(sub_cfg_pset);
      
    }
    return pset;
  }
}

#endif
