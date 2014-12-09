/**
 * \file ConfigParams.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class ConfigParams
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_CONFIGPARAMS_H
#define DBTOOL_CONFIGPARAMS_H

#include "DBBase.h"
#include "DBException.h"
#include <iostream>
#include <string>
#include <map>

namespace ubpsql {
  /**
     \class ConfigParams
     User defined class ConfigParams ... these comments are used to generate
     doxygen documentation!
  */
  class ConfigParams : public std::map<std::string,std::string>,
		       public DBBase {

  public:
    
    /// Restricted constructor
    ConfigParams(std::string name = "",
		 int    crate     = -1,
		 int    slot      = -1,
		 int    channel   = -1,
		 size_t mask      = 0x0);
    
    /// Default destructor
    virtual ~ConfigParams(){}
    
    const std::string& Name() const { return fName; }
    
    int    Crate()    const { return fCrate;    }
    int    Slot()     const { return fSlot;     }
    int    Channel()  const { return fChannel;  }
    size_t Mask()     const { return fMask;     }

    void Crate   (int c   ) { fCrate=c;    }
    void Slot    (int s   ) { fSlot=s;     }
    void Channel (int ch  ) { fChannel=ch; }
    void Mask    (size_t m) { fMask=m;     }

    bool IsDefaultCrate() const
    { return (fCrate==-999 && fSlot==-1 && fChannel==-1); }

    bool IsDefaultSlot() const
    { return (fCrate==-1 && fSlot==-999 && fChannel==-1); }

    bool IsDefaultChannel() const
    { return (fCrate==-1 && fSlot==-1 && fChannel==-999); }

    std::string FhiclDump() const;

    void ls() const;

    void append(const std::string& key, const std::string& value)
    {this->insert(std::make_pair(key,value));}
    
    inline bool operator== (const ConfigParams& rhs) const
    {
      return ( fName    == rhs.Name    () &&
	       fCrate   == rhs.Crate   () &&
	       fSlot    == rhs.Slot    () &&
	       fChannel == rhs.Channel () );
    }

    inline bool operator!= (const ConfigParams& rhs) const
    { return !( (*this) == rhs ); }

    inline bool operator< (const ConfigParams& rhs) const
    {
      if( fName     < rhs.Name    () ) return true;
      if( fName     > rhs.Name    () ) return false;
      if( fCrate    < rhs.Crate   () ) return true;
      if( fCrate    > rhs.Crate   () ) return false;
      if( fSlot     < rhs.Slot    () ) return true;
      if( fSlot     > rhs.Slot    () ) return false;
      if( fChannel  < rhs.Channel () ) return true;
      if( fChannel  > rhs.Channel () ) return false;
      return false;
    }

    inline bool operator> (const ConfigParams& rhs) const
    { return !((*this)<rhs); }

    const std::map<std::string,std::string>& Params() const
    { return (*this); }

  private:
    
    std::string fName;
    int    fCrate;
    int    fSlot;
    int    fChannel;
    size_t fMask;
  };
}
                                                                    
#ifndef __GCCXML__
namespace std {
  template <>
  class less<ubpsql::ConfigParams*>
  {
  public:
    bool operator()( const ubpsql::ConfigParams* lhs, const ubpsql::ConfigParams* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif
/** @} */ // end of doxygen group 

