#!/usr/bin/env python
from dbtool import ubpsql
import sys
reader = ubpsql.ConfigReader()

detail = ('detail' in sys.argv)
arxive = ('arxive' in sys.argv)
expert = ('expert' in sys.argv)

if '-h' in sys.argv or '--help' in sys.argv:

    print 'Usage: %s [detail|arxive|expert]' % sys.argv[0]
    sys.exit(1)

cfgs = reader.ListMainConfigs(-1,expert,arxive)

if not cfgs.size():
    print 'No main-configuration found for the given confition...'
    sys.exit(0)
    
print
print '  Found %d registered main-configs that match the query...' % cfgs.size()
print
for x in xrange(cfgs.size()):
    msg = '    \033[95mConfig ID %d\033[00m => \033[93m%s\033[00m ' % (cfgs[x].fID,cfgs[x].fName)
    if cfgs[x].fArxived or cfgs[x].fExpert:
        msg += '('
        if cfgs[x].fArxived: msg +=' Arxived'
        if cfgs[x].fExpert:  msg +=' Expert'
        msg += ' )'
    print msg
    if not detail: continue
    sub_cfgs = reader.ListSubConfigs(cfgs[x].fName)
    for y in xrange(sub_cfgs.size()):
        print '        |- \"%s\" (ID=%d)' % (sub_cfgs[y].first,sub_cfgs[y].second)
    print
print
