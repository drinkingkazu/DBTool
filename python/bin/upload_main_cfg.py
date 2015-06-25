#!/usr/bin/env python
from dbtool import MainConfigParser
from dbtool import MainConfigUploader
import sys

parser=MainConfigParser()
upload=MainConfigUploader()

parser.read_file(sys.argv[1])
for cfg in parser._mcfg_v:
    upload.AddMainConfig(cfg)
upload.Upload()
