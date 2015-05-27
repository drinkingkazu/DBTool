from dbtool import ubpsql
from ROOT import TString
import sys

if len(sys.argv) < 2:
    print
    print '    Provide RunConfig name as 1st argument (for which fcl is dumped)'
    print
    sys.exit(1)

reader = ubpsql.ConfigReader()

print
print '    Attempting fcl dump for a Run-Config:',sys.argv[1]
print
if not reader.ExistRunConfig(sys.argv[1]):
    print
    print '        Not a registered Run-Config...'
    print
    sys.exit(1)

cfg = reader.RunConfigFromName(sys.argv[1])

fcl_maker = ubpsql.FhiclMaker()
run_pset  = fcl_maker.FhiclParameterSet(cfg)
fcl_txt   = ''
#fcl_txt   = run_pset.Dump()
sub_pset_v = run_pset.ListNodes()
for x in xrange(sub_pset_v.size()):
    fcl_txt += run_pset.GetNode(sub_pset_v[x]).Dump()
print fcl_txt
