#!/usr/bin/env python
import sys
from dbtool.colored_msg import info,warning,error
from dbtool import ask_binary
from dbtool import ubpsql

if not len(sys.argv) in [3,4]:
    warning('Usage: %s CONFIG_KEY RUN_NUMBER [BATCH_FLAG]' % sys.argv[0])
    sys.exit(1)

run=0
try:
    cfg = ''
    if sys.argv[1].isdigit():
        cfg = int(sys.argv[1])
    else:
        cfg = sys.argv[1]

    run = int(sys.argv[2])

    batch_mode = False
    if len(sys.argv) == 4:
        batch_mode = bool(int(sys.argv[3]))

except ValueError:
    warning('Usage: %s CONFIG_KEY RUN_NUMBER [BATCH_FLAG]' % sys.argv[0])
    sys.exit(1)    
    
info('Attempting to insert a new subrun w/ Run %d MainConfig %s' % (run,cfg))

if not batch_mode and not ask_binary('Proceed? [y/n]:'):
    sys.exit(0)

writer = ubpsql.ConfigWriter()

if not writer.ExistMainConfig(cfg):

    error('No such MainConfig found...')
    sys.exit(1)

if not writer.ExistRun(run):

    error('Run %d does not exist...' % run)
    sys.exit(1)

new_run = writer.InsertNewSubRun(cfg,run)

if new_run >= 0:
    info('New run number inserted: %d' % new_run)
    sys.exit(0)
else:
    error('Failed to insert a new run!')
    sys.exit(1)

