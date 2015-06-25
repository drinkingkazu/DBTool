/**
 * \file DeathStar.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief RunControl DB interface class for writing into the database
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_DEATHSTAR_H
#define DBTOOL_DEATHSTAR_H

#include "ConfigWriter.h"

namespace ubpsql {
  /**
     \class DeathStar
     RunControl DB interface for writing into the database.
     Inherits from ConfigReader to make use of read functions.
  */
  class DeathStar : public ConfigWriter {
    
  public:
    
    /// Default constructor
    DeathStar() : ConfigWriter()
    { _user_type = kUBDAQ_ADMIN; }

    /// Default destructor
    virtual ~DeathStar(){};

    /// Method to insert a new sub configuration
    int CreateSubConfiguration(const SubConfig& cfg);

    /// Don't use this
    void SuperBeam();
      
  };
}

#endif
/** @} */ // end of doxygen group 

