#!/usr/bin/env python
from dbtool import ubpsql

if ubpsql.ConfigReader().Connect():
    print "Reader Connection Made..."
if ubpsql.ConfigWriter().Connect():
    print "Writer Connection Made..."
#ubpsql.DeathStar().Connect()
