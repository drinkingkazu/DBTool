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

  FhiclMaker::FhiclMaker() : _pset("fhicl")
  {}

  FPSet& FhiclMaker::CratePSet( const std::string& name )
  {
    if(_pset._node.find(name) == _pset._node.end()) {
      FPSet tmp(name);
      _pset.append(tmp);
    }
    return _pset._node[name];
  }
  
  FPSet& FhiclMaker::SlotPSet ( const std::string& crate_name,
				const std::string& slot_name )
  {
    auto& crate_pset = CratePSet(crate_name);
    if(crate_pset._node.find(slot_name) == crate_pset._node.end()) {
      FPSet tmp(slot_name);
      crate_pset.append(tmp);
    }
    return crate_pset._node[slot_name];
  }
  
  FPSet& FhiclMaker::ChannelPSet ( const std::string& crate_name,
				   const std::string& slot_name,
				   const std::string& channel_name)
  {
    auto& slot_pset = SlotPSet(crate_name,slot_name);
    if(slot_pset._node.find(channel_name) == slot_pset._node.end()) {
      FPSet tmp(channel_name);
      slot_pset.append(tmp);
    }    
    return slot_pset._node[channel_name];
  }

  FPSet& FhiclMaker::PSet( const std::string& crate_name,
			   std::string slot_name,
			   std::string channel_name )
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

    if(slot_name.empty() && channel_name.empty()) return CratePSet(crate_name);
    if(channel_name.empty()) return SlotPSet(crate_name,slot_name);
    return ChannelPSet(crate_name,slot_name,channel_name);
  }

  void FhiclMaker::set(const RunConfig& cfg)
  {
    _pset.clear(cfg.Name());

    for(auto const& sub_cfg_name : cfg.List()) {

      auto const& sub_cfg = cfg.Get(sub_cfg_name);

      // Get crate/slot/channel name
      std::map<int,std::string> crate_name_m;
      std::map<int,std::map<int,std::string> > slot_name_m;
      std::map<int,std::map<int,std::map<int,std::string> > > channel_name_m;

      for(auto const& key_param : sub_cfg) {

	auto const& key    = key_param.first;
	auto const& params = key_param.second;

	if(key.IsDefaultCrate() || key.IsDefaultSlot() || key.IsDefaultChannel())
	  continue;

	auto const name_iter = params.find("name");

	if(name_iter == params.end()) {
	  Print(msg::kERROR,__FUNCTION__,
		Form("CubConfig \"%s\" contains a parameter w/o name!",
		     sub_cfg.Name().c_str())
		);
	  throw FhiclError();
	}
	std::string name = (*name_iter).second;

	if(key.IsCrate()) {
	  if(name.empty()) name = Form("Crate%02d",key.Crate());
	  crate_name_m[key.Crate()] = name;
	  CratePSet(name);
	}
	else if(key.IsSlot()) {
	  if(name.empty()) name = Form("Slot%02d",key.Slot());
	  slot_name_m[key.Crate()][key.Slot()] = name;
	}
	else if(key.IsChannel()) {
	  if(name.empty()) name = Form("Ch%02d",key.Channel());
	  channel_name_m[key.Crate()][key.Slot()][key.Channel()] = name;
	}else{
	  Print(msg::kERROR,__FUNCTION__,
		Form("Found ill-defined CParamsKey!"));
	  throw ConfigError();
	}
      }
      
      for(auto const& crate_slot_pair : slot_name_m) {
	auto const& crate = crate_slot_pair.first;
	auto const& slot_name = crate_slot_pair.second;
	for(auto const& id_name : slot_name) {
	  SlotPSet(crate_name_m[crate],
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
	    ChannelPSet(crate_name_m[crate],
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

	auto& pset = this->PSet(crate_name,slot_name,channel_name);
	
	for(auto const& key_value : params){
	  if(key_value.first == "name") continue;
	  pset.append(key_value.first,key_value.second);
	}
      }
    }
  }
}

#endif
