#!/usr/bin/env python
from dbtool import ubpsql
import sys
writer = ubpsql.ConfigWriter()

if len(sys.argv)<2: 
    print 
    print '    Must give Sub-Config name as an argument.'
    print
    sys.exit(1)

cfg_name = sys.argv[1]
cfg_id = -1

if len(sys.argv)>2:
    if not sys.argv[2].isdigit():
        print 
        print '    2nd argument must be integer Sub-Config id!'
        print
        sys.exit(1)
    cfg_id = int(sys.argv[2])

if not writer.ExistSubConfig(cfg_name):
    print
    print '    Sub-Config %s does not exist...' % cfg_name
    print
    sys.exit(1)

print
print '    Attempting to remove Sub-Config: %s' % cfg_name
print
user_input=''
while 1:
    sys.stdout.write('    [y/n]: ')
    sys.stdout.flush()
    user_input = sys.stdin.readline().rstrip('\n')
    if user_input.lower() == 'y':
        break
    elif user_input.lower() == 'n':
        print
        print '    Exiting...'
        sys.exit(1)
    else:
        print '    Invalid input: \"%s\"' % user_input
        continue

res=None
try:
    if cfg_id >=0 :
        res = writer.CleanSubConfig(cfg_name,cfg_id)
    else:
        res = writer.CleanSubConfig(cfg_name)
except Exception:
    print '    Logic failure! Contact an expert.'
    print
    sys.exit(1)
if res: 
    print '    Success!'
    print
    sys.exit(0)
else: 
    print '    Failed...'
    print
    sys.exit(1)




