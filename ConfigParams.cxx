#ifndef DBTOOL_CONFIGPARAMS_CXX
#define DBTOOL_CONFIGPARAMS_CXX

#include "ConfigParams.h"

namespace ubpsql {
  
  ConfigParams::ConfigParams(std::string name,
			     int    crate,
			     int    slot,
			     int    channel,
			     size_t mask)
  {
    fName     = name;
    fCrate    = crate;
    fSlot     = slot;
    fChannel  = channel;
    fMask     = mask;
  }

  void ConfigParams::ls() const
  {
    std::cout
      << std::endl
      << "    Name \"" << fName.c_str() << "\"" << std::endl
      << "    Crate         : " << fCrate    << std::endl
      << "    Slot          : " << fSlot     << std::endl
      << "    Channel       : " << fChannel  << std::endl
      << "    Mask          : ";
    for(size_t i=0; i < 8*sizeof(size_t); ++i) 
      std::cout<< bool((fMask >> i) & 0x1);
    std::cout<<std::endl;
    std::cout
      << std::endl
      << "    ... with following parameter values:" << std::endl;

      for(auto const& v : *this)

	std::cout << "    " << v.first.c_str() << " => " << v.second.c_str() << std::endl;
    
      std::cout<<std::endl;
  }

  std::string ConfigParams::FhiclDump() const
  {
    std::string fcl("");

    fcl += fName + ":\n{\n";

    for(auto const& param : *this)

      fcl += param.first + ": " + param.second + "\n";

    fcl += "}\n";
    
    return fcl;

  }


}

#endif
