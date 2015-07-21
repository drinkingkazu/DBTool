#ifndef DBTOOL_RUNINFO_CXX
#define DBTOOL_RUNINFO_CXX

#include "RunInfo.h"
#include <sstream>
namespace ubpsql {

  RunInfo::RunInfo()
    : _run         ( kINVALID_UINT  )
    , _subrun      ( kINVALID_UINT  )
    , _type        ( kINVALID_SHORT )
    , _config_id   ( kINVALID_INT   )
    , _tstart_sec  ( kINVALID_TIME  )
    , _tstart_usec ( kINVALID_UINT  )
    , _tstop_sec   ( kINVALID_TIME  )
    , _tstop_usec  ( kINVALID_UINT  )
  {}
  
  RunInfo::RunInfo(const unsigned int  run,
		   const unsigned int  subrun,
		   const short         type,
		   const unsigned int  config_id,
		   const time_t        tstart_sec,
		   const unsigned int  tstart_usec,
		   const time_t        tstop_sec,
		   const unsigned int  tstop_usec)
    : _run         ( run         )
    , _subrun      ( subrun      )
    , _type        ( type        )
    , _config_id   ( config_id   )
    , _tstart_sec  ( tstart_sec  )
    , _tstart_usec ( tstart_usec )
    , _tstop_sec   ( tstop_sec   )
    , _tstop_usec  ( tstop_usec  )
  {}

  RunInfo::RunInfo(const unsigned int run,
		   const unsigned int subrun,
		   const short        type,
		   const unsigned int config_id,
		   const double       tstart,
		   const double       tstop)
    : _run        ( run              )
    , _subrun     ( subrun           )
    , _type       ( type             )
    , _config_id  ( config_id        )
    , _tstart_sec ( (time_t)(tstart) )
    , _tstop_sec  ( (time_t)(tstop)  )
  {
    _tstart_usec = int( ( tstart - (double)_tstart_sec )*1.e6 );
    _tstop_usec  = int( ( tstop   - (double)_tstop_sec   )*1.e6 );
  }
  
  unsigned int RunInfo::GetStartTimeMicroSeconds() const
  { return _tstart_usec; }

  unsigned int RunInfo::GetStopTimeMicroSeconds() const
  { return _tstop_usec; }
  
  unsigned int RunInfo::GetRunNumber() const
  { return _run; }
  
  unsigned int RunInfo::GetSubRunNumber() const
  { return _subrun; }
  
  short        RunInfo::GetRunType() const
  { return _type; }

  unsigned int RunInfo::GetConfigID() const
  { return _config_id; }
  
  TTimeStamp RunInfo::GetStartTimeStamp() const
  { return TTimeStamp((time_t)_tstart_sec,(int)(_tstart_usec*1e3)); }

  TTimeStamp RunInfo::GetStopTimeStamp() const
  { return TTimeStamp((time_t)_tstop_sec, (int)(_tstart_usec*1e3)); }

  std::string RunInfo::Print() const
  {
    std::stringstream msg;

    msg << "Run       : " << _run       << std::endl;
    msg << "Sub-Run   : " << _subrun    << std::endl;
    msg << "Run-Type  : " << _type      << std::endl;
    msg << "Config ID : " << _config_id << std::endl;  
    msg << "Started   @ " << GetStartTimeStamp().AsString("s") << std::endl;
    msg << "Stopped   @ " << GetStopTimeStamp().AsString("s")  << std::endl;
    return msg.str();
  }
}

#endif
