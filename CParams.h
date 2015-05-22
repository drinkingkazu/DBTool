/**
 * \file CParams.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class CParams and CParamsKey
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_CPARAMS_H
#define DBTOOL_CPARAMS_H

#include "DBBase.h"
#include "DBEnv.h"
#include "DBException.h"
#include <iostream>
#include <string>
#include <map>

namespace ubpsql {
  /**
     \class ParamKey
  */
  class CParamsKey : public DBBase {

  public:

    /// Restricted constructor
    CParamsKey(int    crate     = -1,
	       int    slot      = -1,
	       int    channel   = -1);
    
    /// Default destructor
    virtual ~CParamsKey(){}

    int    Crate()    const { return fCrate;    }
    int    Slot()     const { return fSlot;     }
    int    Channel()  const { return fChannel;  }

    bool IsDefaultCrate() const;
    bool IsDefaultSlot() const;
    bool IsDefaultChannel() const;

    bool IsCrate()     const;
    bool IsSlot()      const;
    bool IsChannel()   const;

    void ls() const;

    inline bool operator== (const CParamsKey& rhs) const
    {
      return ( fCrate   == rhs.Crate   () &&
	       fSlot    == rhs.Slot    () &&
	       fChannel == rhs.Channel () );
    }

    inline bool operator!= (const CParamsKey& rhs) const
    { return !( (*this) == rhs ); }

    inline bool operator< (const CParamsKey& rhs) const
    {
      if( fCrate    < rhs.Crate   () ) return true;
      if( fCrate    > rhs.Crate   () ) return false;
      if( fSlot     < rhs.Slot    () ) return true;
      if( fSlot     > rhs.Slot    () ) return false;
      if( fChannel  < rhs.Channel () ) return true;
      if( fChannel  > rhs.Channel () ) return false;
      return false;
    }

    inline bool operator> (const CParamsKey& rhs) const
    { return !((*this)<rhs); }

    bool valid() const;

  private:
    int    fCrate;
    int    fSlot;
    int    fChannel;
  };

  class CParams : public std::map<std::string,std::string>,
		  public DBBase {

  public:
    
    /// Restricted constructor
    CParams(size_t mask = 0x0,
	    const std::string& name="")
      : fMask(mask)
      , fName(name)
    {}
    
    /// Default destructor
    virtual ~CParams(){}

    void clear() { fMask=0x0; fName=""; std::map<std::string,std::string>::clear(); }
    
    size_t Mask()               const { return fMask;   }
    const std::string& Name()   const { return fName;   }

    void Mask (size_t m) { fMask=m; }
    void Name (const std::string& name);
    
    std::string FhiclDump() const;

    void ls() const;

    void append(const std::string& key, const std::string& value);
    
    const std::map<std::string,std::string>& Params() const
    { return (*this); }

  private:
    size_t fMask;
    std::string fName;
  };
}
                                                                    
#ifndef __GCCXML__
namespace std {
  template <>
  class less<ubpsql::CParamsKey*>
  {
  public:
    bool operator()( const ubpsql::CParamsKey* lhs, const ubpsql::CParamsKey* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif
/** @} */ // end of doxygen group 

