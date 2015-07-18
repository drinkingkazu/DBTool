/**
 * \file DBException.h
 *
 * \ingroup UBOnlineDBI
 * 
 * \brief Class def header for a class DBException
 *
 * @author kazuhiro
 */

/** \addtogroup UBOnlineDBI

    @{*/
#ifndef DBTOOL_DBEXCEPTION_H
#define DBTOOL_DBEXCEPTION_H

#include <iostream>
#include <exception>

namespace ubpsql {
  /**
     \class DBException
     Generic (base) exception class
  */
  class DBException : public std::exception{
  public:
    DBException() : std::exception() {}
    ~DBException() throw(){};
    virtual const char* what() const throw() 
    {return "\033[91mException occurred\033[00m";}
  };

  /**
     \class InvalidUser
     Exception for creating connection w/ invalid user
   */
  class InvalidUser : public DBException{
  public:
    InvalidUser() : DBException() {}
    ~InvalidUser() throw(){};
    virtual const char* what() const throw()
    { return "\033[91mInvalid DB Connection User Type\033[00m"; }
  };

  /**
     \class ConnectionError
     Exception for failing to open a connection
   */
  class ConnectionError : public DBException{
  public:
    ConnectionError() : DBException() {}
    ~ConnectionError() throw(){};
    virtual const char* what() const throw()
    { return "\033[91mFailed to establish DB connection\033[00m";}
  };

  /**
     \class QueryError
     Exception for failing to execute a query
   */
  class QueryError : public DBException{
  public:
    QueryError() : DBException() {}
    ~QueryError() throw() {}
    virtual const char* what() const throw()
    { return "\033[91mFailed to execute the SQL query!\033[00m"; }
  };

  /**
     \class RunNotFoundError
  */
  class RunNotFoundError : public DBException{
  public:
    RunNotFoundError() : DBException() {}
    ~RunNotFoundError() throw() {}
    virtual const char* what() const throw()
    { return "\033[91mRun/SubRun specified not ound!\033[00m"; }

  /**
     \class TableDataError
     Exception for an unexpected table content
   */
  class TableDataError : public DBException{
  public:
    TableDataError() : DBException() {}
    ~TableDataError() throw() {}
    virtual const char* what() const throw()
    { return "\033[91mInvalid table data content!\033[00m"; }
  };

  /**
     \class ConfigError
     Exception for invalid handling of ubpsql::RunConfig, ubpsql::SubConfig, ubpsql::CParams and ubpsql::CParamsKey
   */
  class ConfigError : public DBException{
  public:
    ConfigError() : DBException() {}
    ~ConfigError() throw() {}
    virtual const char* what() const throw()
    { return "\033[91mInvalid config handling!\033[00m";}
  };

}
#endif
/** @} */ // end of doxygen group 

