#!/usr/bin/env python
import sys
from dbtool.colored_msg import info,warning,error
from dbtool import ubpsql

reader = ubpsql.ConfigReader()

info('Last Run Number: %d' % reader.GetLastRun())

sys.exit(0)
