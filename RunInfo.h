#ifndef DBTOOL_RUNINFO_H
#define DBTOOL_RUNINFO_H

#include "DBConstants.h"

namespace ubpsql {

  class RunInfo {

  public:

    RunInfo(const unsigned int   run      = kINVALID_UINT,
	    const unsigned int   subrun   = kINVALID_UINT,
	    const unsigned short type     = kINVALID_USHORT,
	    const TTimeStamp&    start_ts = kINVALID_TIMESTAMP,
	    const TTimeStamp&    end_ts   = kINVALID_TIMESTAMP);

    ~RunInfo() {}

    std::string Print() const;

  protected:
    unsigned int   _run;
    unsigned int   _subrun;
    unsigned short _type;
    TTimeStamp     _start_ts;
    TTimeStamp     _end_ts;
  };

}
#endif
