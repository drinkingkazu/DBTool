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
#include "RunConfig.h"

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
    { _user_type = kCUSTOM_USER; }
    //{ _user_type = kUBDAQ_READER; }
    
    /// Default destructor
    virtual ~ConfigReader(){}

    //
    // Run-Control functions
    //
    /// Getter for last run number
    unsigned int GetLastRun();
    /// Getter for last sub-run number
    unsigned int GetLastSubRun(unsigned int run);
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
    std::vector<std::string> RunConfigNames();
    /// List Run-Config ID
    std::vector<unsigned int> RunConfigIDs();
    /// List All Sub-Config name/ID pair for a given MainConfig name
    std::vector<std::pair<std::string,unsigned int> >  SubConfigNameAndIDs(const std::string& cfg_name);
    /// Existence checker.
    bool ExistRunConfig(const std::string& cfg_name);
    /// Existence checker.
    bool ExistRunConfig(const unsigned int cfg_id);
    /// Run-Config Name=>ID
    unsigned int RunConfigID(const std::string& cfg_name);
    /// Run-Config ID=>Name
    std::string RunConfigName(const unsigned int cfg_id);
    /// Run-Config getter
    RunConfig RunConfigFromName(const std::string& cfg_name);
    /// Run-Config getter
    RunConfig RunConfigFromID(const unsigned int cfg_id)
    { return RunConfigFromName( RunConfigName(cfg_id) ); }
    /// Run-Config getter
    RunConfig RunConfigFromRunNumber(const unsigned int run,
				     const unsigned int subrun=kINVALID_UINT)
    { return RunConfigFromName( RunConfigName(RunNumber2ConfigID(run,subrun)) ); }

    //
    // Sub-Config functions
    //
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

