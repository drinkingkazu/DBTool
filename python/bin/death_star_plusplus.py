#!/usr/bin/env python
import sys
from dbtool.colored_msg import info,warning,error
from dbtool import ask_binary
from dbtool import ubpsql

if not len(sys.argv) in [4,5]:
    warning('Usage: %s CONFIG_KEY RUN_NUMBER AGE_OF_ECHURCH [BATCH_FLAG]' % sys.argv[0])
    sys.exit(1)

cfg = ''
batch_mode = False
new_run_number = -1
age_of_echurch = -1
try:
    cfg = ''
    if sys.argv[1].isdigit():
        cfg = int(sys.argv[1])
    else:
        cfg = sys.argv[1]

    new_run_number = int(sys.argv[2])
    age_of_echurch = int(sys.argv[3])

    batch_mode = False
    if len(sys.argv) == 5:
        batch_mode = bool(int(sys.argv[4]))

    if new_run_number < 0 or age_of_echurch < 0:
        raise ValueError
        
except ValueError:
    warning('Usage: %s CONFIG_KEY RUN_NUMBER AGE_OF_ECHURCH [BATCH_FLAG]' % sys.argv[0])
    sys.exit(1)
    
info('Attempting to insert a new run w/ MainConfig %s' % cfg)

if not ask_binary('Proceed? [y/n]:'):
    sys.exit(0)

writer = ubpsql.ConfigWriter()

if not writer.ExistMainConfig(cfg):

    error('No such MainConfig found...')
    sys.exit(1)

if writer.ExistRun(new_run_number):

    error('Run number %d already exist...' % new_run_number)
    sys.exit(1)

new_run = writer.DeathStarPlusPlus(cfg,new_run_number,age_of_echurch)

if new_run >= 0:
    info('New run number inserted: %d' % new_run)
    sys.exit(0)
else:
    error('Failed to insert a new run!')
    sys.exit(1)
