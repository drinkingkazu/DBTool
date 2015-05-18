import sys,__main__
from dbtool import CUploader
from ROOT import ubpsql
if not len(sys.argv) == 2 or (sys.argv[1].endswith('.sub') and sys.argv[1].endswith('.main')):
    print
    print 'Usage: %s CONFIG_FILE' % __main__.__file__
    print 'CONFIG_FILE should have an extension .sub or .main'
    print

k = CUploader()
# Uncomment for verbose action
k._conn.SetVerbosity(ubpsql.msg.kDEBUG)

if sys.argv[1].endswith('.sub'): k.upload_subconfig(sys.argv[1])
if sys.argv[1].endswith('.main'): k.upload_mainconfig(sys.argv[1])
