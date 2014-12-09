import os, sys
from UBPySQLException import UploadException
from SubCIP import SubCIP
from MainCIP import MainCIP
from UBPyBase import UBPyBase
from ROOT import ubpsql, std

class CUploader(UBPyBase):

    def __init__(self):
        super(CUploader,self).__init__()
        self._conn   = ubpsql.ConfigWriter()
        self._conn.SetVerbosity(ubpsql.msg.kNORMAL)
        self._sub_parser = SubCIP()
        self._main_parser = MainCIP()

    def ask_binary(self):
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

    def upload_mainconfig(self,fname,batch=False):
        data = None
        try:
            data = self._main_parser.parse(fname)
        except Exception as e:
            print e
            return False

        if not data:
            self.error('No data read-in ')
            return False        

        if not batch:
            print '\033[95m' + 'ATTENTION!' + '\033[00m',
            print '... Attempting to upload Main-Config!'
            print
            data.ls()
            if not self.ask_binary(): return False
                    
        self._conn.Connect()
        # Check if configuration already exists or not
        if self._conn.ExistRunConfig(data.Name()):
            self.error('Main-Configuration \"%s\" already exist!' % data.Name())
            return False
            
        try: 
            self._conn.InsertMainConfiguration(data)
        except Exception as e:
            print e
            self.critical('Failed to insert a new configuration: name=\"%s\"' % data.Name())
            return False
            
        return True

    def upload_subconfig(self,fname,batch=False):

        data = None
        try:
            data = self._sub_parser.parse(fname)
        except Exception as e:
            return False

        if not data:
            self.error('No data read-in ')
            return False

        if not batch:
            print '\033[95m' + 'ATTENTION!' + '\033[00m',
            print '... Attempting to upload Sub-Config!'
            print
            data.ls()
            if not self.ask_binary(): return False
                    
        self._conn.Connect()
        # Check if configuration already exists or not
        if not self._conn.ExistSubConfig(data.Name()):

            self.warning('Sub-Configuration \"%s\" does not exist' % data.Name())
            self.warning('Attempting to create...')
            self._conn.CreateConfigType(data.Name())

            if not self._conn.ExistSubConfig(data.Name()):
                self.critical('Failed to create a sub-config \"%s\"' % data.Name())
                raise UploadException
            else:
                self.info('Successful!')
            
        try: 
            self._conn.InsertSubConfiguration(data)
        except Exception as e:
            print e
            self.critical('Failed to insert a new configuration: name=\"%s\" id=%d' % (data.Name(),data.ConfigID()))
            return False

        try:
            self._conn.FillSubConfiguration(data)
        except Exception as e:
            print e
            self.critical('Failed to insert a new configuration: name=\"%s\" id=%d' % (data.Name(),data.ConfigID()))
            self.critical('Removing it from DB...')
            self._conn.CleanSubConfig(data.Name(),data.ConfigID())
            return False
        return True

if __name__ == '__main__':

    k = CUploader()
    if sys.argv[1].endswith('sub'): k.upload_subconfig(sys.argv[1])
    if sys.argv[1].endswith('main'): k.upload_mainconfig(sys.argv[1])
