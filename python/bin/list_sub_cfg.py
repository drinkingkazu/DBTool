#!/usr/bin/env python
from dbtool import ubpsql

reader=ubpsql.ConfigReader()

sub_cfg = reader.SubConfigNames()

print
print '  Registered Sub-Configs:'
for x in xrange(sub_cfg.size()):
    print '    ', sub_cfg[x], '...', 
    ids = reader.SubConfigIDs(sub_cfg[x])
    for y in xrange(ids.size()):
        print ids[y],
    print
