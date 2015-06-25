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

#include <set>
#include <TString.h>
#include "DBBase.h"
namespace ubpsql {

  class SubConfigID : public std::pair<std::string,unsigned int> {
  public:
    SubConfigID(const std::string& name="",
		const unsigned int id=kINVALID_UINT);

    const std::string& Name() const { return this->first; }

    unsigned int ID() const { return this->second; }

    inline bool operator<(const SubConfigID& rhs) const
    {
      if( this->first  < rhs.Name() ) return true;
      if( this->first  > rhs.Name() ) return false;
      if( this->second < rhs.ID()   ) return true;
      if( this->second > rhs.ID()   ) return false;
      return false;
    }
    
  };
  
  class SubConfig : public DBBase {

  public:

    SubConfig(const std::string& name="",
	      const unsigned int id=kINVALID_UINT);

    virtual ~SubConfig(){}

    const SubConfigID& ID() const { return _id; }

    void Append(const std::string& key, 
		const std::string& param);

    void Append(const SubConfig& scfg);

    void Clear() {
      _leaf.clear();
      _node.clear();
    }

    void Dump(std::string& content,size_t level=0) const;

    inline bool operator< (const SubConfig& rhs) const
    {
      return (_id < rhs.ID());
    }

    std::string Dump() const
    { std::string res; Dump(res); return res;}

    std::vector<std::string> ListParameters() const;

    const std::map<std::string,std::string>& Parameters() const;
    
    std::vector<std::string> ListSubConfigs() const;

    std::vector< ubpsql::SubConfigID > ListSubConfigIDs() const;

    std::set< ubpsql::SubConfigID > Dependencies() const;

    const SubConfig& GetSubConfig(const std::string node_name) const;
    
  protected:

    SubConfigID _id;
    
    std::map<std::string,std::string>   _leaf;

    std::map<std::string,ubpsql::SubConfig> _node;

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

