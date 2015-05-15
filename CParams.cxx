#ifndef DBTOOL_CPARAMS_CXX
#define DBTOOL_CPARAMS_CXX

#include "CParams.h"
#include <sstream>
namespace ubpsql {
  
  CParamsKey::CParamsKey(int    crate,
			 int    slot,
			 int    channel)
  {
    fCrate    = crate;
    fSlot     = slot;
    fChannel  = channel;
  }

  bool CParamsKey::valid() const
  {
    // not yet a complete check...
    if( fCrate < 0 && fSlot < 0 && fChannel < 0 && (fCrate+fSlot+fChannel) != -1001 ) 
      return false;
    if( fCrate < 0 && fCrate != -1 && fCrate !=-999)
      return false;
    if( fSlot < 0 && fSlot != -1 && fSlot !=-999)
      return false;
    if( fChannel < 0 && fChannel != -1 && fChannel !=-999)
      return false;
    return true;
  }

  bool CParamsKey::IsDefaultCrate() const
  { return (fCrate==-999 && fSlot==-1 && fChannel==-1); }
  
  bool CParamsKey::IsDefaultSlot() const
  { return (fCrate==-1 && fSlot==-999 && fChannel==-1); }
  
  bool CParamsKey::IsDefaultChannel() const
  { return (fCrate==-1 && fSlot==-1 && fChannel==-999); }
  
  bool CParamsKey::IsCrate()     const
  { return (fCrate==-999 || (fCrate>=0 && fSlot==-1 && fChannel==-1));}
  bool CParamsKey::IsSlot()      const
  { return ((fCrate>=0 || fCrate==-1) && (fSlot==-999 || fSlot>=0) && fChannel==-1); }
  bool CParamsKey::IsChannel()   const
  { return ((fCrate>=0 || fCrate==-1) && (fSlot>=0 || fSlot==-1) && (fChannel==-999 || fChannel>=0));}

  void CParamsKey::ls() const
  {
    std::ostringstream msg;
    if(this->IsCrate())
      msg << "Crate " << fCrate;
    else if(this->IsSlot())
      msg << "Crate " << fCrate << " Slot " << fSlot;
    else
      msg << "Crate " << fCrate << " Slot " << fSlot << " Channel " << fChannel;
    msg << " Configuration" << std::endl;
    std::cout<<msg.str();
  }
  
  void CParams::ls() const
  {
    std::ostringstream msg;
    msg << std::endl
	<< "    Mask          : ";
    for(size_t i=0; i < 8*sizeof(size_t); ++i) 
      msg << bool((fMask >> i) & 0x1);
    msg<<std::endl
       << std::endl
       << "    ... with following parameter values:" << std::endl;

      for(auto const& v : *this)

	msg << "    " << v.first.c_str() << " => " << v.second.c_str() << std::endl;
    
      std::cout<<msg.str()<<std::endl;
  }

  std::string CParams::FhiclDump() const
  {
    std::string fcl("");

    for(auto const& param : *this)

      fcl += param.first + ": " + param.second + "\n";

    fcl += "}\n";
    
    return fcl;

  }


}

#endif
