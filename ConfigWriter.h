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
    { _user_type = kUBDAQ_WRITER; }

    /// Default destructor
    virtual ~ConfigWriter(){};

    //
    // Run table related
    //

    /// Create a new run number in the database with an existing main-config id
    unsigned int InsertNewRun(unsigned int config_id);

    /// Create a new run number in the database with an existing main-config name
    unsigned int InsertNewRun(const std::string& cfg_name);

    /// Insert a new sub-run number with an existing main-config name
    unsigned int InsertNewSubRun(const std::string& cfg_name,
				 const unsigned int run);

    /// Insert a new sub-run number with an existing main-config id
    unsigned int InsertNewSubRun(const unsigned int cfg_id,
				 const unsigned int run);

    /// Update run start time
    bool UpdateStartTime(const unsigned int  run,
			 const unsigned int  subrun,
			 const unsigned long time_sec,
			 const unsigned int  time_usec = 0);

    /// Update run stop time
    bool UpdateStopTime(const unsigned int  run,
			const unsigned int  subrun,
			const unsigned long time_sec,
			const unsigned int  time_usec = 0);

    //
    // Config table related
    //
    
    /// Method to insert a new sub configuration
    bool InsertSubConfiguration(const SubConfig& cfg);

    /// Remove a specific sub-config ID
    bool CleanSubConfig(const std::string cfg_name, unsigned int cfg_id);

    /// Remove sub-config
    bool CleanSubConfig(const std::string cfg_name);

    /// Method to create a run configuration. Return value >= 0 represents a new main-config ID
    int InsertMainConfiguration(const MainConfig& cfg);
    
    /// Drop MainConfig
    bool CleanMainConfig(const std::string& name);

  };
}

#endif
/** @} */ // end of doxygen group 

