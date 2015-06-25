#!/usr/bin/env python
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
if not reader.ExistMainConfig(sys.argv[1]):
    print
    print '        Not a registered Run-Config...'
    print
    sys.exit(1)

cfg = reader.GetMainConfig(sys.argv[1])

print cfg.Dump()


