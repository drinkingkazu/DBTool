#!/usr/bin/env python
from dbtool import ubpsql

#if ubpsql.ConfigReader().Connect():
k=ubpsql.ConfigReader()
if k.Connect():
    print "Reader Connection Made..."
j=ubpsql.ConfigWriter()
if j.Connect():
    print "Writer Connection Made..."
#ubpsql.DeathStar().Connect()
