#ifndef DBTOOL_SUBCONFIG_CXX
#define DBTOOL_SUBCONFIG_CXX

#include "SubConfig.h"

namespace ubpsql {
  
  SubConfig::SubConfig(std::string name,
		       int    config_id,
		       size_t mask)
  {
    if( name.empty() || config_id < 0 )

      throw TableDataError();

    fName     = name;
    fConfigID = config_id;
    fMask     = mask;
    /*
      this->insert(std::make_pair(CParamsKey(-999,-1,-1),CParams(0)));
      this->insert(std::make_pair(CParamsKey(-1,-999,-1),CParams(0)));
      this->insert(std::make_pair(CParamsKey(-1,-1,-999),CParams(0)));
    */
  }
  
  const CParams& SubConfig::GetParams( const int crate,
				       const int slot,
				       const int channel) const
  {
    CParamsKey p(crate,slot,channel);
    auto iter = this->find(p);
    if(iter == this->end()) {
      std::ostringstream msg;
      msg << "No ConfigParams for"
	  << " crate=" << crate
	  << " slot="  << slot
	  << " ch="    << channel;
      Print(msg::kERROR,__FUNCTION__,msg.str());
      throw TableDataError();
    }
    return (*iter).second;
  }

  void SubConfig::append(const CParamsKey& key,const CParams& value)
  {
    if(this->find(key) != this->end()) {
      std::ostringstream msg;
      msg << "Configuration for"
	  << " crate = " << key.Crate()
	  << " slot = " << key.Slot()
	  << " ch = " << key.Channel()
	  << " already registered! (duplication not allowed)";
      Print(msg::kERROR,__FUNCTION__,msg.str());
      throw ConfigError();
    }
    (*this)[key] = value;
  }
  
  void SubConfig::ls() const
  {
    std::cout
      << std::endl
      << "  Sub-Config \"" << fName.c_str() << "\"" << std::endl
      << "  Config ID  : " << fConfigID << std::endl
      << "  Mask       : ";
    for(size_t i=0; i < 8*sizeof(size_t); ++i) 
      std::cout << bool((fMask >> i) & 0x1);

    std::cout 
      << std::endl
      << "  ... with following configurations." << std::endl;
    
    for(auto const& key_params : *this) {
      auto const& key    = key_params.first;
      auto const& params = key_params.second;
      key.ls();
      params.ls();
    }
    
    std::cout<<std::endl;

  }

}

#endif
