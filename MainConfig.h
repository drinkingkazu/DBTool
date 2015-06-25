/**
 * \file MainConfig.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class MainConfig
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_MAINCONFIG_H
#define DBTOOL_MAINCONFIG_H

#include "SubConfig.h"
#include <vector>
namespace ubpsql {

  /**
     \class MainConfig
     User defined class MainConfig ... these comments are used to generate
     doxygen documentation!
  */
  class MainConfig : public DBBase {

  public:
    
    /// Default constructor
    MainConfig(std::string name){ fName=name; }
    
    /// Default destructor
    virtual ~MainConfig(){}

    /// Checker if a Sub-Config exists
    bool Exist(const std::string& name) const
    { return !(_data.find(name) == _data.end()); }

    /// Name getter
    const std::string& Name() const { return fName;}

    /// Getter for a Sub-Config
    const SubConfig& Get(const std::string& name) const;

    /// Dump
    std::string Dump(const bool stack_main_cfg_name=false) const;

    /// Sub-Config name list
    std::vector<std::string> ListSubConfigs() const;

    /// Sub-ConfigID list
    std::vector<ubpsql::SubConfigID> ListSubConfigIDs() const;

    /// Sub-Config list cout
    void ls() const;

    /// ConfigData inserter
    void AddSubConfig(const SubConfig& cfg);

  private:
    /// Name
    std::string fName;

    /// Hidden config data
    std::map<std::string,ubpsql::SubConfig> _data;
  };
}

#endif
/** @} */ // end of doxygen group 

