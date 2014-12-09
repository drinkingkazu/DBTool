/**
 * \file PQMessenger.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class PQMessenger
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_PQMESSENGER_H
#define DBTOOL_PQMESSENGER_H

#include <libpq-fe.h>
#include "DBBase.h"

namespace ubpsql {
  /**
     \class PQMessenger
     User defined class PQMessenger ... these comments are used to generate
     doxygen documentation!
  */
  class PQMessenger : public DBBase {

  private:
    /// Default constructor
    PQMessenger() : DBBase()
    { _msg_level = msg::kWARNING; }
    
  public:
    
    static PQMessenger& GetME()
    {
      if(!_me) _me = new PQMessenger;
      return *_me;
    }
    
    /// Default destructor
    virtual ~PQMessenger(){};

  protected:
    static PQMessenger* _me;
  };
}

#endif
/** @} */ // end of doxygen group 

