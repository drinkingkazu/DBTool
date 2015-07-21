#!/usr/bin/env python
from dbtool import ubpsql
from dbtool.colored_msg import info,error
import sys, time

if not len(sys.argv) in [4,5]:
    msg  = 'Usage: %s RUN SUBRUN TIME_STAMP\n' % sys.argv[0]
    msg += '... where TIME_STAMP can be an SQL-like string or unix time\n'
    msg += '    the "unix time" must be 2 integers: second and micro-seconds.'
    error(msg)
    sys.exit(1)

run    = 0
subrun = 0
sec    = 0
usec   = 0
try:
    run    = int(sys.argv[1])
    subrun = int(sys.argv[2])
    if sys.argv[3].isdigit():
        if len(sys.argv) == 5:
            usec  = int(sys.argv[4])
            usec  = usec%1e7
        if not usec == int(sys.argv[4]):
            error("Second time argument (\"%s\") not in micro-seconds scale..." % sys.argv[4])
            sys.exit(1)
        sec = int(sys.argv[3])
    elif len(sys.argv) == 4:
        ts = time.mktime(time.strptime(sys.argv[3],'%Y-%m-%d %H:%M:%S'))
        sec  = int(ts)
        usec = int((ts - sec) * 1.e6)
    else:
        raise Exception
except Exception as e:
    raise e
    error("Invalid argument: failed to convert input arguments...")
    sys.exit(1)
    
writer = ubpsql.ConfigWriter()

if not writer.ExistRun(run,subrun):

    error("Run %d SubRun %d does not exist..." % (run,subrun))
    sys.exit(1)

exec('writer.UpdateStartTime(%d,%d,%d,%d)' % (run,subrun,sec,usec))
