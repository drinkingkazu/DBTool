/**
 * \file FhiclMaker.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class FhiclMaker
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_FHICLMAKER_H
#define DBTOOL_FHICLMAKER_H
#include "RunConfig.h"
#include <TString.h>
namespace ubpsql {

  class FhiclMaker;

  class FPSet : public DBBase {

    friend class FhiclMaker;

  public:

    FPSet(std::string name="");

    virtual ~FPSet(){}

    const std::string& Name() const { return _name; }
    
    void Append(const std::string& key, 
		const std::string& param);

    void Append(const FPSet& pset);

    void Append(const std::string& key,
		const std::map<std::string,std::string>& pset);

    void Clear(std::string new_name="") { 
      if(!new_name.empty()) _name = new_name;
      _leaf.clear();
      _node.clear();
    }

    void Dump(std::string& content,size_t level=0) const;

    inline bool operator< (const FPSet& rhs) const
    { return _name < rhs.Name(); }

    std::string Dump() const
    { std::string res; Dump(res); return res;}

    std::vector<std::string> ListNodes() const;

    const FPSet& GetNode(const std::string node_name) const;
    
  protected:

    std::map<std::string,std::string>   _leaf;

    std::map<std::string,ubpsql::FPSet> _node;

    std::string _name;
  };

  /**
     \class FhiclMaker
     User defined class FhiclMaker ... these comments are used to generate
     doxygen documentation!
  */
  class FhiclMaker : public DBBase{
    
  public:
    
    /// Default constructor
    FhiclMaker();
    
    /// Default destructor
    virtual ~FhiclMaker(){}

    FPSet FhiclParameterSet(const SubConfig& cfg);

    FPSet FhiclParameterSet(const RunConfig& cfg);

  private:

    FPSet& CratePSet( FPSet& pset, const std::string& crate_name );

    FPSet& SlotPSet ( FPSet& pset,
		      const std::string& crate_name,
		      const std::string& slot_name );

    FPSet& ChannelPSet ( FPSet& pset,
			 const std::string& crate_name,
			 const std::string& slot_name,
			 const std::string& channel_name);

    FPSet& PSet( FPSet& pset,
		 const std::string& crate_name,
		 const std::string& slot_name="",
		 const std::string& channel_name="" );
  };
}

#ifndef __GCCXML__
namespace std {
  template <>
  class less<ubpsql::FPSet*>
  {
  public:
    bool operator()( const ubpsql::FPSet* lhs, const ubpsql::FPSet* rhs )
    { return (*lhs) < (*rhs); }
  };
}
#endif

#endif
/** @} */ // end of doxygen group 

