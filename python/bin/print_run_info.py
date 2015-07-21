#!/usr/bin/env python
from dbtool import ubpsql
from dbtool.colored_msg import info,error
import sys, time

if not len(sys.argv) in [3]:
    msg  = 'Usage: %s RUN SUBRUN \n' % sys.argv[0]
    error(msg)
    sys.exit(1)

run    = 0
subrun = 0
sec    = 0
usec   = 0
try:
    run    = int(sys.argv[1])
    subrun = int(sys.argv[2])
except Exception as e:
    raise e
    error("Invalid argument: failed to convert input arguments...")
    sys.exit(1)
    
writer = ubpsql.ConfigWriter()

info = writer.GetRunInfo(run,subrun)
print info.Print()
