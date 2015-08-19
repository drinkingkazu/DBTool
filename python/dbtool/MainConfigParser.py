from UBPyCPP import ubpsql
from UBPyBase import UBPyBase
from UBPySQLException import ParseIException
import sys

class MainConfigParser(UBPyBase):
    _start_key = 'MAIN_CONFIG_START'
    _end_key = 'MAIN_CONFIG_END'
    _mcfg_v  = []

    def __init__(self):
        super(self.__class__,self).__init__()

    def read_file(self,fname):
        tmp_lines=open(fname,'r').read().split('\n')
        contents=''
        for tmp_l in tmp_lines:
            if '#' in tmp_l:
                tmp_l = tmp_l[0:tmp_l.find('#')]
            l=tmp_l.strip(' ').rstrip(' ')
            if not l: continue
            contents += l + '\n'

        contents.rstrip('\n')

        self._blocks=[]
        while contents:
            start_index = contents.find(self._start_key)
            end_index = contents.find(self._end_key)
            if start_index < 0 or end_index < 0:
                self.error("Undefined text block...\n%s" % contents)
                raise ParseIException
            if end_index < start_index:
                self.error("No block begin declared...\n%s" % contents[0:end_index])
                raise ParseIException
            block = contents[start_index+len(self._start_key):end_index]
            block = block.strip(' ').rstrip(' ').rstrip('\n').rstrip(' ')
            if not block:
                self.error("Found an empty block...")
                raise ParseIException
            if block.split('\n') < 2:
                self.error("Found a block with no parameters...\n%s" % block)
                raise ParseIException
            self._mcfg_v.append(self.parse_block(block))
            contents=contents[end_index+len(self._end_key):len(contents)]
            contents=contents.strip(' ').rstrip(' ').rstrip('\n')

        return len(self._blocks)

    def parse_block(self,block):
        lines = block.split('\n')
        words = lines[0].split()
        if not len(words) == 2:
            self.error("%s needs name & run type (found \"%s\")" % (self._start_key,lines[0]))
            raise ParseIException
        runtype = ubpsql.RunTypeFromName(words[1])
        if runtype < 0:
            self.error("RunType string was invalid (\"%s\")" % words[1])
            raise ParseIException

        # MainConfigMetaData
        meta_mcfg=ubpsql.MainConfigMetaData()
        meta_mcfg.fName = words[0]
        meta_mcfg.fRunType = runtype
        mcfg = ubpsql.MainConfig(meta_mcfg)
        
        for x in xrange(len(lines)):
            if x == 0: continue
            line = lines[x]
            if line.find('=>') > 0:
                words=line.split('=>')
                if not len(words) == 2:
                    self.error('Invalid parameter set specification:\n%s' % line)
                    raise ParseIException()
                words[0]=words[0].strip(' ').rstrip(' ')
                words[1]=words[1].strip(' ').rstrip(' ')
                if not words[1].isdigit():
                    self.error('Invalid parameter set ID (\"%s\")' % words[1])
                    raise ParseIException()
                tmp = ubpsql.SubConfig(words[0],int(words[1]))
                mcfg.AddSubConfig(tmp)
            else:
                self.error('Invalid line inside sub-config block:\n%s' % line)
                raise ParseIException
                
        return mcfg

if __name__=='__main__':
    k=SubConfigParser()
    k.read_file(sys.argv[1])
