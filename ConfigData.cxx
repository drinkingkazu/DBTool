#ifndef DBTOOL_CONFIGDATA_CXX
#define DBTOOL_CONFIGDATA_CXX

#include "ConfigData.h"

namespace ubpsql {
  
  ConfigData::ConfigData(std::string name,
			 int    config_id,
			 size_t mask)
  {
    if( name.empty() || config_id < 0 )

      throw TableDataError();
    
    fName     = name;
    fConfigID = config_id;
    fMask     = mask;
  }
  
  const ConfigParams& ConfigData::CrateDefault() const
  {
    for(auto const& cfg : *this)
      if( cfg.IsDefaultCrate() ) return cfg;
    Print(msg::kERROR,__FUNCTION__,"Default crate config not found!");
    throw DBException();
  }

  const ConfigParams& ConfigData::SlotDefault() const
  {
    for(auto const& cfg : *this)
      if(cfg.IsDefaultSlot()) return cfg;
    Print(msg::kERROR,__FUNCTION__,"Default slot config not found!");
    throw DBException();
  }

  const ConfigParams& ConfigData::ChannelDefault() const
  {
    for(auto const& cfg : *this)
      if(cfg.IsDefaultChannel()) return cfg;
    Print(msg::kERROR,__FUNCTION__,"Default channel config not found!");
    throw DBException();
  }

  const ConfigParams& ConfigData::GetParams( const std::string name,
					     const int crate,
					     const int slot,
					     const int channel)
  {
    ConfigParams p(name,crate,slot,channel);
    if(!contains(p)) {
      std::ostringstream msg;
      msg << "No ConfigParams for"
	  << " name="  << name.c_str() << "\""
	  << " crate=" << crate
	  << " slot="  << slot
	  << " ch="    << channel;
      Print(msg::kERROR,__FUNCTION__,msg.str());
      throw TableDataError();
    }
    return (*(this->find(p)));
  }

  void ConfigData::ls() const
  {
    std::cout
      << std::endl
      << "  Configuration \"" << fName.c_str() << "\"" << std::endl
      << "  Config ID     : " << fConfigID << std::endl
      << "  Mask          : ";
    for(size_t i=0; i < 8*sizeof(size_t); ++i) 
      std::cout << bool((fMask >> i) & 0x1);

    std::cout 
      << std::endl
      << "  ... with following configurations." << std::endl;
    
    for(auto const& v : *this)
      
      v.ls();
    
    std::cout<<std::endl;

  }

  std::string ConfigData::FhiclDump() const
  {
    std::string fcl("");

    /*
    fcl += fName + ":\n{\n";

    for(auto const& param : *this)

      fcl += param.first + ": " + param.second + "\n";

    fcl += "}\n";
    */    
    return fcl;

  }


}

#endif
