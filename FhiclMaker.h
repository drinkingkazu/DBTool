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

  class FPSet : public DBBase{

    friend class FhiclMaker;

  public:

    FPSet(std::string name="");

    virtual ~FPSet(){}

    void append(const std::string& key, 
		const std::string& param);

    void append(const std::string& key,
		const FPSet& pset);

    void append(const std::string& key,
		const std::map<std::string,std::string>& pset);

    const std::string& Name() const { return _name; }

    inline bool operator< (const FPSet& rhs) const
    { return _name < rhs.Name(); }
    
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
  class FhiclMaker{
    
  public:
    
    /// Default constructor
    FhiclMaker();
    
    /// Default destructor
    virtual ~FhiclMaker(){}

  protected:

    FPSet _pset;
    
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

