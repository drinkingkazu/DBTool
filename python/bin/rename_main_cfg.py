#!/usr/bin/env python
from dbtool import ubpsql
from dbtool.colored_msg import error,warning,info
import sys
writer = ubpsql.ConfigWriter()

if not len(sys.argv) == 3:
    error('Usage:  %s  MAIN_CONFIG_KEY  NEW NAME' % sys.argv[0])
    sys.exit(1)

cfg_name = ''
cfg_id   = 0
if sys.argv[1].isdigit():
    cfg_id   = int(sys.argv[1])
    if not writer.ExistMainConfig(cfg_id):
        error('No MainConfig with an ID %s' % str(cfg_id))
        sys.exit(1)
    cfg_name = writer.MainConfigName(cfg_id)
else:
    cfg_name = str(sys.argv[1])
    if not writer.ExistMainConfig(cfg_name):
        error('No MainConfig with a name %s' % str(cfg_name))
        sys.exit(1)
    cfg_id   = writer.MainConfigID(cfg_name)

new_name = str(sys.argv[2])
if new_name.isdigit():
    error('Cannot re-name to digits...')
    sys.exit(1)

warning('    Attempting to rename Main-Config: %s' % cfg_name)
user_input=''
while 1:
    sys.stdout.write('    [y/n]: ')
    sys.stdout.flush()
    user_input = sys.stdin.readline().rstrip('\n')
    if user_input.lower() == 'y':
        break
    elif user_input.lower() == 'n':
        print
        print '    Exiting...'
        sys.exit(1)
    else:
        print '    Invalid input: \"%s\"' % user_input
        continue

writer.RenameMainConfig(cfg_name,new_name)
print
