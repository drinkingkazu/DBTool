#!/usr/bin/env python
from dbtool import ubpsql
import sys
writer = ubpsql.DeathStar()

if len(sys.argv)<2: 
    print 
    print '    Must give Main-Config name as an argument.'
    print
    sys.exit(1)

cfg_name = sys.argv[1]

if not writer.ExistMainConfig(cfg_name):
    print
    print '    Main-Config %s does not exist...' % cfg_name
    print
    sys.exit(1)

print
print '    Attempting to remove Main-Config: %s' % cfg_name
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
    res = writer.CleanMainConfig(cfg_name)
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




