from dbtool import ubpsql
import sys

if not len(sys.argv) == 2 or not sys.argv[1].isdigit():

    print 'Usage: %s RUN_NUMBER' % sys.argv[0]
    sys.exit(-1)

k=ubpsql.ConfigReader()

print k.GetLastSubRun(int(sys.argv[1]))
