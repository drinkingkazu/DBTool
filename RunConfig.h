/**
 * \file RunConfig.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class RunConfig
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_RUNCONFIG_H
#define DBTOOL_RUNCONFIG_H

#include "ConfigData.h"
#include <vector>
namespace ubpsql {

  /**
     \class RunConfig
     User defined class RunConfig ... these comments are used to generate
     doxygen documentation!
  */
  class RunConfig : public DBBase {

  public:
    
    /// Default constructor
    RunConfig(std::string name){ fName=name; }
    
    /// Default destructor
    virtual ~RunConfig(){}

    /// Checker if a Sub-Config exists
    bool Exist(const std::string& name) const
    { return !(_data.find(name) == _data.end()); }

    /// Name getter
    const std::string& Name() const { return fName;}

    /// Getter for a Sub-Config
    const ConfigData& Get(const std::string& name) const;

    /// Sub-Config name list
    std::vector<std::string> List() const;

    /// Sub-Config list cout
    void ls() const;

    /// ConfigData inserter
    void AddSubConfig(const ConfigData& cfg);

  private:
    /// Name
    std::string fName;

    /// Hidden config data
    std::map<std::string,ubpsql::ConfigData> _data;

  };
}

#endif
/** @} */ // end of doxygen group 

