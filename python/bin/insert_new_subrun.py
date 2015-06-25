from dbtool import ubpsql
import sys

if not len(sys.argv) == 3 or not sys.argv[1].isdigit() or not sys.argv[2].isdigit():

    print 'Usage: %s MainConfigID RunNumber'
    sys.exit(-1)

cfg_id = int(sys.argv[1])
run = int(sys.argv[2])

k=ubpsql.ConfigWriter()
if not k.ExistMainConfig(cfg_id):

    k.error('MainConfig w/ ID = %d does not exist!' % cfg_id)

if not k.ExistRun(run):

    k.error('RunNumber %d does not exist!' % run)

if k.InsertNewSubRun(cfg_id,run): sys.exit(0)
else: sys.exit(1)
