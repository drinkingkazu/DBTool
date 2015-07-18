#ifndef DBTOOL_RUNINFO_CXX
#define DBTOOL_RUNINFO_CXX

#include "RunInfo.h"

namespace ubpsql {
  RunInfo::RunInfo(const unsigned int   run,
		   const unsigned int   subrun,
		   const unsigned short type,
		   const TTimeStamp&    start_ts,
		   const TTimeStamp&    end_ts)
    : _run      ( run      )
    , _subrun   ( subrun   )
    , _type     ( type     )
    , _start_ts ( start_ts )
    , _end_ts   ( end_ts   )
  {}
}

#endif
