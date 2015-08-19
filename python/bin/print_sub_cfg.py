#!/usr/bin/env python
from dbtool import ubpsql
from dbtool.colored_msg import error,info
import sys

reader=ubpsql.ConfigReader()

if not len(sys.argv) in [3,4,5]:
    error('Usage: %s CONFIG_NAME CONFIG_ID [detail|format]' % sys.argv[0])
    sys.exit(1)

cfg_name = sys.argv[1]
cfg_id   = sys.argv[2]
if not cfg_id.isdigit():
    error('Usage: %s CONFIG_NAME CONFIG_ID' % sys.argv[0])
    sys.exit(1)
cfg_id = int(cfg_id)

sub_cfg = reader.GetSubConfig(cfg_name,cfg_id)

msg=''
if 'detail' in sys.argv:
    msg  = 'Dumping full details for %s ID=%d\n' % (cfg_name,cfg_id)
    msg += sub_cfg.Dump()
    info(msg)
else:
    
    if 'format' in sys.argv:
        msg = 'SUB_CONFIG_START %s %d\n' % (cfg_name,cfg_id)
    else:
        msg = 'SubConfig Name=%s ID=%d\n' % (cfg_name,cfg_id)

    param_names = sub_cfg.ListParameters()
    param_map = sub_cfg.Parameters()
    for x in xrange(param_names.size()):
        name = param_names[x]
        msg += '  %s : %s\n' % (name,param_map[name])
    scfg_ids = sub_cfg.ListSubConfigIDs()
    for x in xrange(scfg_ids.size()):
        msg += '  %s => %d\n' % (scfg_ids[x].first,scfg_ids[x].second)
        
    if 'format' in sys.argv:
        msg += 'SUB_CONFIG_END'
        print msg
    else:
        info(msg)
