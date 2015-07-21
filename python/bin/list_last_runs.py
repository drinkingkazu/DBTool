#!/usr/bin/env python
import sys
from dbtool.colored_msg import info,warning,error
from dbtool import ubpsql

reader = ubpsql.ConfigReader()

if not len(sys.argv) in [1,2]:
    warning('Usage: %s [# RUNS TO LIST]' % sys.argv[0])
    sys.exit(1)

nruns=1
if len(sys.argv) == 2 and not sys.argv[1].isdigit():
    error('Argument run number is not digit!')
    sys.exit(1)
    nruns = int(sys.argv[1])

info('Last Run Number: %d' % reader.GetLastRun())

sys.exit(0)
