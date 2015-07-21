#ifndef DBTOOL_RUNINFO_H
#define DBTOOL_RUNINFO_H

#include "DBConstants.h"

namespace ubpsql {

  class RunInfo {

  public:

    RunInfo();

    RunInfo(const unsigned int  run,
	    const unsigned int  subrun,
	    const short         type,
	    const unsigned int  config_id,
	    const time_t        tstart_sec,
	    const unsigned int  tstart_usec,
	    const time_t        tstop_sec,
	    const unsigned int  tstop_usec);

    RunInfo(const unsigned int run,
	    const unsigned int subrun,
	    const short        type,
	    const unsigned int config_id,
	    const double       tstart,
	    const double       tstop);

    ~RunInfo() {}

    std::string Print() const;

    TTimeStamp GetStartTimeStamp() const;
    TTimeStamp GetStopTimeStamp() const;
    unsigned int GetStartTimeMicroSeconds() const;
    unsigned int GetStopTimeMicroSeconds() const;
    unsigned int GetRunNumber() const;
    unsigned int GetSubRunNumber() const;
    short        GetRunType() const;
    unsigned int GetConfigID() const;
    
  protected:
    unsigned int  _run;
    unsigned int  _subrun;
    short         _type;
    unsigned int  _config_id;
    time_t        _tstart_sec;
    time_t        _tstop_sec;
    unsigned int  _tstart_usec;
    unsigned int  _tstop_usec;
  };

}
#endif
