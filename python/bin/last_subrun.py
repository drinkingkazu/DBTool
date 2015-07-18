#!/usr/bin/env python
import sys
from dbtool.colored_msg import info,warning,error
from dbtool import ubpsql

if not len(sys.argv) == 2 or not sys.argv[1].isdigit():
    error('Usage: %s RUN_NUMBER' % sys.argv[0])
    sys.exit(1)

run = int(sys.argv[1])
    
reader = ubpsql.ConfigReader()

info('Run %d ... Last SubRun Number: %d' % (run,reader.GetLastSubRun(run)))

sys.exit(0)
