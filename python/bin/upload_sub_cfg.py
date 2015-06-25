#!/usr/bin/env python
from dbtool import SubConfigParser
from dbtool import SubConfigUploader
import sys

parser=SubConfigParser()
upload=SubConfigUploader()

parser.read_file(sys.argv[1])
for cfg in parser._scfg_v:
    upload.AddSubConfig(cfg)

upload.Upload()
