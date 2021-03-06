#!/usr/bin/env python
import sys
from dbtool.colored_msg import info,warning,error
from dbtool import ask_binary
from dbtool import ubpsql

if not len(sys.argv) in [2,3]:
    warning('Usage: %s CONFIG_KEY [BATCH_FLAG]' % sys.argv[0])
    sys.exit(1)

try:
    cfg = ''
    if sys.argv[1].isdigit():
        cfg = int(sys.argv[1])
    else:
        cfg = sys.argv[1]

    batch_mode = False
    if len(sys.argv) == 3:
        batch_mode = bool(int(sys.argv[2]))

except ValueError:
    warning('Usage: %s CONFIG_KEY [BATCH_FLAG]' % sys.argv[0])
    sys.exit(1)
    
info('Attempting to insert a new run w/ MainConfig %s' % cfg)

if not ask_binary('Proceed? [y/n]:'):
    sys.exit(0)

writer = ubpsql.ConfigWriter()

if not writer.ExistMainConfig(cfg):

    error('No such MainConfig found...')
    sys.exit(1)

new_run = writer.InsertNewRun(cfg)

if new_run >= 0:
    info('New run number inserted: %d' % new_run)
    sys.exit(0)
else:
    error('Failed to insert a new run!')
    sys.exit(1)
