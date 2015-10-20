import sys

def ask_binary(msg):
    if msg:
        print msg
    user_input=''
    while not user_input:
        print 'Enter [y/n]: ',
        sys.stdout.flush()
        user_input = sys.stdin.readline().replace('\n','')
            
        if user_input in ['N','n']:
            return False
        
        elif user_input in ['Y','y']:
            return True
        
        else:
            print 'Invalid input: \"%s\"' % user_input
            user_input=''
