#!/usr/bin/env python
from dbtool import SubConfigParser
from dbtool import SubConfigUploader
import sys

parser=SubConfigParser()
upload=SubConfigUploader()

for x in xrange(len(sys.argv)-1):

    parser.read_file(sys.argv[x+1])

for cfg in parser._scfg_v:
    upload.AddSubConfig(cfg)

upload.Upload()
