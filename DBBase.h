/**
 * \file DBBase.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Base class for DBI package
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_DBBASE_H
#define DBTOOL_DBBASE_H

#include <iostream>
#include <sstream>
#include "DBConstants.h"
#include <map>

namespace ubpsql {

  namespace msg{

    /// Message level ENUM
    enum MSGLevel_t {
      kDEBUG=0,
      kINFO,
      kNORMAL,
      kWARNING,
      kERROR
    };
  }
  
  /**
     \class DBBase
     Base class for other classes in DB package
  */
  
  class DBBase{
    
  public:
    
    /// Default constructor
    DBBase(){ _msg_level=msg::kNORMAL;}
    
    /// Default destructor
    virtual ~DBBase(){}

    /// Message level setter
    virtual void SetVerbosity(msg::MSGLevel_t level)
    {_msg_level = level; }

    /// debug level message attribute
    void Print(const msg::MSGLevel_t level,
	       const std::string& msg1,
	       const std::string& msg2="") const;
  protected:

    msg::MSGLevel_t _msg_level;

  };
}

#endif
/** @} */ // end of doxygen group 

