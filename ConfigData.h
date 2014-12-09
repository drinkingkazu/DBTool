/**
 * \file ConfigData.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class ConfigData
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_CONFIGDATA_H
#define DBTOOL_CONFIGDATA_H

#include "ConfigParams.h"
#include <set>

namespace ubpsql {
  /**
     \class ConfigData
     User defined class ConfigData ... these comments are used to generate
     doxygen documentation!
  */
  class ConfigData : public std::set<ubpsql::ConfigParams>,
		     public DBBase {
    
  public:
    
    /// Default constructor
    ConfigData(std::string name = "",
	       int    config_id = -1,
	       size_t mask      = 0x0);

    /// Default destructor
    virtual ~ConfigData(){}
    
    const std::string& Name() const { return fName; }
    
    int    ConfigID() const { return fConfigID; }
    size_t Mask()     const { return fMask;     }

    const ConfigParams& CrateDefault() const;
    const ConfigParams& SlotDefault() const;
    const ConfigParams& ChannelDefault() const;

    std::string FhiclDump() const;

    bool contains(const ConfigParams& p) const
    { return (this->find(p) != this->end()); }

    bool contains(const std::string name,
		  const int crate,
		  const int slot,
		  const int channel) const
    { ConfigParams p(name,crate,slot,channel); return contains(p);}

    const ConfigParams& GetParams( const std::string name,
				     const int crate,
				     const int slot,
				     const int channel);

    void ls() const;

    inline bool operator== (const ConfigData& rhs) const
    { return ( fName == rhs.Name() && fConfigID == rhs.ConfigID() ); }

    inline bool operator!= (const ConfigData& rhs) const
    { return !( (*this) == rhs ); }

    inline bool operator< (const ConfigData& rhs) const
    {
      if( fName     < rhs.Name    () ) return true;
      if( fName     > rhs.Name    () ) return false;
      if( fConfigID < rhs.ConfigID() ) return true;
      if( fConfigID > rhs.ConfigID() ) return false;
      return false;
    }

    inline bool operator> (const ConfigData& rhs) const
    { return !((*this)<rhs); }

  private:
    
    std::string fName;
    int    fConfigID;
    size_t fMask;
  };
}
                                                                    
#ifndef __GCCXML__
namespace std {
  template <>
  class less<ubpsql::ConfigData*>
  {
  public:
    bool operator()( const ubpsql::ConfigData* lhs, const ubpsql::ConfigData* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif
/** @} */ // end of doxygen group 

