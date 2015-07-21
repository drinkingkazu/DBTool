/**
 * \file ConfigReader.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief RunControl DB interface class for reading from the database
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_CONFIGREADER_H
#define DBTOOL_CONFIGREADER_H

#include "DBAppBase.h"
#include "MainConfig.h"
#include "RunInfo.h"
namespace ubpsql {
  /**
     \class ConfigReader
     RunControl DB interface class for reading from the database.
     To write something into the database, one should use RCWriter.
  */
  class ConfigReader : public DBAppBase {
    
  public:
    
    /// Default constructor
    ConfigReader() : DBAppBase() 
    { _user_type = kUBDAQ_READER; }
    //{ _user_type = kUBDAQ_READER; }
    
    /// Default destructor
    virtual ~ConfigReader(){}

    //
    // Run-Control functions
    //
    
    /// Getter for last run number
    unsigned int GetLastRun();
    /// Getter for last sub-run number
    unsigned int GetLastSubRun(const unsigned int run);
    /// Getter for a run info
    RunInfo GetRunInfo(const unsigned int run,const unsigned int subrun);

    /// Existence checker
    bool ExistRun(const unsigned int run,
		  const unsigned int subrun=kINVALID_UINT);
    /// Getter for run-config ID for a given run number
    unsigned int RunNumber2ConfigID(const unsigned int run, 
				    const unsigned int subrun=kINVALID_UINT);

    //
    // Run-Config functions
    //
    /// List Run-Config name
    std::vector<std::string> MainConfigNames();
    /// List Run-Config ID
    std::vector<unsigned int> MainConfigIDs();
    /// List All Sub-Config name/ID pair for a given MainConfig name
    //std::vector<std::pair<std::string,unsigned int> >  ListSubConfigs(const std::string& cfg_name);
    /// Existence checker.
    bool ExistMainConfig(const std::string& cfg_name);
    /// Existence checker.
    bool ExistMainConfig(const unsigned int cfg_id);
    /// Run-Config Name=>ID
    unsigned int MainConfigID(const std::string& cfg_name);
    /// Run-Config ID=>Name
    std::string MainConfigName(const unsigned int cfg_id);
    /// Sub-Config getter
    SubConfig GetSubConfig(const std::string& scfg_name, const unsigned int scfg_id);
    /// Run-Config getter
    MainConfig GetMainConfig(const std::string& cfg_name);
    /// Run-Config getter
    MainConfig GetMainConfig(const unsigned int cfg_id)
    { return GetMainConfig( MainConfigName(cfg_id) ); }
    /// Run-Config getter
    MainConfig MainConfigFromRunNumber(const unsigned int run,
				     const unsigned int subrun=kINVALID_UINT)
    { return GetMainConfig( MainConfigName(RunNumber2ConfigID(run,subrun)) ); }

    //
    // Sub-Config functions
    //
    /// List Sub-Configs for a specific MainConfig name
    std::vector<std::pair<std::string,unsigned int> > ListSubConfigs(const std::string& cfg_name);
    /// List Sub-Configs for a specific MainConfig id
    std::vector<std::pair<std::string,unsigned int> > ListSubConfigs(const unsigned int id);
    /// List all Sub-Configs
    std::vector<std::string> SubConfigNames();
    /// List Sub-Config IDs
    std::vector<unsigned int> SubConfigIDs(const std::string& name);
    /// Existence checker.
    bool ExistSubConfig(std::string sub_config_name);
    /// Existence checker for a specific sub-config id
    bool ExistSubConfig(std::string sub_config_name, unsigned int id);
    /// Getter for the maximum sub-config ID number used 
    int MaxSubConfigID(std::string sub_config_name);

  protected:

    std::vector<std::string> _scfg_type_2_name;
    
  };
}

#endif
/** @} */ // end of doxygen group 

