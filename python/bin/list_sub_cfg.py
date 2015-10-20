#!/usr/bin/env python
from dbtool import ubpsql
from dbtool.colored_msg import error,warning,info
import sys

reader=ubpsql.ConfigReader()

names = reader.SubConfigNames()

if names.size() < 1:
    warning('Did not find any sub-config...')
    sys.exit(1)
    
print
msg = 'List of SubConfigs...\n'
for x in xrange(names.size()):
    name = names[x]
    ids  = reader.SubConfigIDs(name)
    msg  += '    \033[93m%-20s\033[00m ... ' % name
    for y in xrange(ids.size()):
        msg += '\033[95m%d\033[00m,' % ids[y]
    msg = msg[0:len(msg)-1]
    msg += '\n'
info(msg)

