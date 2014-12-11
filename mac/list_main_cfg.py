from ROOT import ubpsql
import sys
reader = ubpsql.ConfigReader()

cfgs = reader.RunConfigNames()

detail=False
if len(sys.argv)>1:
    detail = sys.argv[1]=='detail'

print
print '  Registered Main-Configs:'
for x in xrange(cfgs.size()):
    print '    %s' % cfgs[x]
    if not detail: continue
    sub_cfgs = reader.SubConfigNameAndIDs(cfgs[x])
    for y in xrange(sub_cfgs.size()):
        print '        |- \"%s\" (ID=%d)' % (sub_cfgs[y].first,sub_cfgs[y].second)
    print
