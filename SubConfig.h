/**
 * \file SubConfig.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class SubConfig
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_SUBCONFIG_H
#define DBTOOL_SUBCONFIG_H

#include "CParams.h"
#include <set>

namespace ubpsql {

  /**
     \class SubConfig
  */
  class SubConfig : public std::map<ubpsql::CParamsKey,ubpsql::CParams>,
		    public DBBase {
    
  public:
    
    /// Default constructor
    SubConfig(std::string name = "",
	       int    config_id = -1,
	       size_t mask      = 0x0);

    /// Default destructor
    virtual ~SubConfig(){}
    
    const std::string& Name() const { return fName; }
    
    int    ConfigID() const { return fConfigID; }
    size_t Mask()     const { return fMask;     }

    bool contains(const CParamsKey& p) const
    { return (this->find(p) != this->end()); }

    bool contains(const int crate,
		  const int slot,
		  const int channel) const
    { CParamsKey p(crate,slot,channel); return contains(p);}

    const CParams& GetParams( const int crate,
			      const int slot,
			      const int channel) const;

    void ls() const;

    void append(const CParamsKey& key,
		const CParams& value)
    { (*this)[key]=value; }

    inline bool operator== (const SubConfig& rhs) const
    { return ( fName == rhs.Name() && fConfigID == rhs.ConfigID() ); }

    inline bool operator!= (const SubConfig& rhs) const
    { return !( (*this) == rhs ); }

    inline bool operator< (const SubConfig& rhs) const
    {
      if( fName     < rhs.Name    () ) return true;
      if( fName     > rhs.Name    () ) return false;
      if( fConfigID < rhs.ConfigID() ) return true;
      if( fConfigID > rhs.ConfigID() ) return false;
      return false;
    }

    inline bool operator> (const SubConfig& rhs) const
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
  class less<ubpsql::SubConfig*>
  {
  public:
    bool operator()( const ubpsql::SubConfig* lhs, const ubpsql::SubConfig* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif
/** @} */ // end of doxygen group 

