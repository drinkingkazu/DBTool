#!/usr/bin/env python
from dbtool import ubpsql
from dbtool.colored_msg import error,warning,info
import sys
reader = ubpsql.ConfigReader()

if not len(sys.argv) == 2:
    error('Usage: %s MAIN_CONFIG_KEY' % sys.argv[0])
    sys.exit(1)

cfg_name = ''
cfg_id   = 0
if sys.argv[1].isdigit():
    cfg_id   = int(sys.argv[1])
    if not reader.ExistMainConfig(cfg_id):
        error('No MainConfig with an ID %s' % str(cfg_id))
        sys.exit(1)
    cfg_name = reader.MainConfigName(cfg_id)
else:
    cfg_name = str(sys.argv[1])
    if not reader.ExistMainConfig(cfg_name):
        error('No MainConfig with a name %s' % str(cfg_name))
        sys.exit(1)
    cfg_id   = reader.MainConfigID(cfg_name)

sub_cfgs = reader.ListSubConfigs(cfg_name)
print
print '    \033[95mConfig ID %d\033[00m => \033[93m%s\033[00m ' % (cfg_id,cfg_name)
for y in xrange(sub_cfgs.size()):
    print '        |- \"%s\" (ID=%d)' % (sub_cfgs[y].first,sub_cfgs[y].second)
print
