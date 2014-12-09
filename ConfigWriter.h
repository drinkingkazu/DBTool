/**
 * \file ConfigWriter.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief RunControl DB interface class for writing into the database
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_CONFIGWRITER_H
#define DBTOOL_CONFIGWRITER_H

#include "ConfigReader.h"

namespace ubpsql {
  /**
     \class ConfigWriter
     RunControl DB interface for writing into the database.
     Inherits from ConfigReader to make use of read functions.
  */
  class ConfigWriter : public ConfigReader{
    
  public:
    
    /// Default constructor
    ConfigWriter() : ConfigReader()
    { _user_type = kCUSTOM_USER; }

    /// Default destructor
    virtual ~ConfigWriter(){};

    /**
       Create New Configuration Type, i.e. PMTFEM, TPCFEM, Laser... to be used sparingly.
       Needs list of defined hstore keys that will later be checked via triggers when inserting new configs
       takes list of strings that it then converts to hstore values. Needs to know the total number of channels 
     */
    bool CreateConfigType(const std::string config_name);

    /// Remove a specific config ID
    bool CleanSubConfig(const std::string cfg_name, unsigned int cfg_id);

    /// Create a new run number in the database
    bool InsertNewRun(unsigned int config_id);

    /// Create a new sub run number in the database
    bool InsertNewSubRun(unsigned int config_id, unsigned int run);

    /// Method to insert a new configuration ID with default parameters sets
    bool InsertSubConfiguration(const ConfigData& cfg);

    /// Method to check if a candidate configuration is valid
    bool CheckNewSubConfiguration(const ConfigData &data);

    /// Method to fill sub-config parameters to a configuration set
    bool FillSubConfiguration(const ConfigData &data);

    /// Method to create a run configuration. Return value >= 0 represents a new main-config ID
    int InsertMainConfiguration(const RunConfig& cfg);

    /// Drop MainConfig
    bool CleanMainConfig(const std::string& name);
      
  };
}

#endif
/** @} */ // end of doxygen group 

