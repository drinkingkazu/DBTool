#!/usr/bin/env python
from dbtool import ubpsql
from dbtool.colored_msg import error
import sys

reader=ubpsql.ConfigReader()

if not len(sys.argv) == 3:
    error('Usage: %s CONFIG_NAME CONFIG_ID' % sys.argv[0])
    sys.exit(1)

cfg_name = sys.argv[1]
cfg_id   = sys.argv[2]
if not cfg_id.isdigit():
    error('Usage: %s CONFIG_NAME CONFIG_ID' % sys.argv[0])
    sys.exit(1)
cfg_id = int(cfg_id)

sub_cfg = reader.GetSubConfig(cfg_name,cfg_id)

print sub_cfg.Dump()

