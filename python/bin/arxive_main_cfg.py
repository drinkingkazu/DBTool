from dbtool import ubpsql
from dbtool.colored_msg import error
import sys

writer = ubpsql.ConfigWriter()

cfg = sys.argv[1]

if cfg.isdigit():
    cfg = int(cfg)

if not writer.ExistMainConfig(cfg):
    error('Main Config %s does not exist!' % cfg)
    sys.exit(1)

writer.ArxiveMainConfiguration(cfg)
