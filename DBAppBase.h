/**
 * \file DBAppBase.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Base class for DB application class
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_DBAPPBASE_H
#define DBTOOL_DBAPPBASE_H

#include <TString.h>

#include "DBConn.h"

namespace ubpsql {
  /**
     \class DBAppBase
     Base class for DB interface application classes
  */
  class DBAppBase : public DBBase{
    
  public:
    
    /// Default constructor
    DBAppBase(){ _user_type = kCUSTOM_USER; _conn=0; };
    
    /// Default destructor
    virtual ~DBAppBase(){};

    /// Method to make a connection to the database
    bool Connect();

    virtual void SetVerbosity(msg::MSGLevel_t level)
    { DBBase::SetVerbosity(level); if(_conn) _conn->SetVerbosity(level); }

  protected:

    DBI_USER_t _user_type;

    DBConn* _conn;
    
  };
}

#endif
/** @} */ // end of doxygen group 

