from dbtool import ubpsql
import sys

if not len(sys.argv) == 2 or not sys.argv[1].isdigit():

    print 'Usage: %s MainConfigID'
    sys.exit(-1)

cfg_id = int(sys.argv[1])

k=ubpsql.ConfigWriter()
if not k.ExistMainConfig(cfg_id):

    k.error('MainConfig w/ ID = %d does not exist!' % cfg_id)

if k.InsertNewRun(cfg_id): sys.exit(0)
else: sys.exit(1)
