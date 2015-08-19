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

#include "DBEnv.h"
#include "SubConfig.h"
#include <vector>
#include <sstream>
namespace ubpsql {

  class  MainConfigMetaData {
  public:
    std::string  fName;
    unsigned int fID;
    bool         fArxived;
    bool         fExpert;
    short        fRunType;
    MainConfigMetaData()
      : fName()
      , fID()
      , fArxived(true)
      , fExpert(true)
      , fRunType(-1)
    {}
    ~MainConfigMetaData(){}
    std::string Dump()
    {
      std::stringstream ss;
      ss << "Config Name: " << fName.c_str() << std::endl
	 << "Config ID  : " << fID << std::endl
	 << "Run Type   : " << fRunType << std::endl
	 << "Arxived?   : " << fArxived << std::endl
	 << "Expert?    : " << fExpert << std::endl;
      return ss.str();
    }
  };
  
  /**
     \class MainConfig
     User defined class MainConfig ... these comments are used to generate
     doxygen documentation!
  */
  class MainConfig : public DBBase {

  public:
    
    /// Default constructor
    MainConfig(std::string name){ fMetaData.fName=name; }

    /// Alternative ctor
    MainConfig(const MainConfigMetaData& data)
      : fMetaData(data)
    {}
    
    /// Default destructor
    virtual ~MainConfig(){}

    /// MetaData setter
    void SetMetaData(const MainConfigMetaData& data) { fMetaData = data; }

    /// Checker if a Sub-Config exists
    bool Exist(const std::string& name) const
    { return !(_data.find(name) == _data.end()); }

    /// Name getter
    const std::string& Name() const { return fMetaData.fName;}

    /// ID
    unsigned int ID() const { return fMetaData.fID; }

    /// Run Type
    short RunType() const { return fMetaData.fRunType; }

    /// Expert or not
    bool Expert() const { return fMetaData.fExpert; }

    /// Archived or not
    bool Arxived() const { return fMetaData.fArxived; }

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
    /// MetaData
    MainConfigMetaData fMetaData;

    /// Hidden config data
    std::map<std::string,ubpsql::SubConfig> _data;
  };
}

#endif
/** @} */ // end of doxygen group 

