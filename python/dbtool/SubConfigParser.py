from ROOT import ubpsql
from UBPyBase import UBPyBase
from UBPySQLException import ParseIException
from colored_msg import error,info,debug
import sys

class SubConfigParser(UBPyBase):
    _start_key = 'SUB_CONFIG_START'
    _end_key = 'SUB_CONFIG_END'
    _scfg_v  = []

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
                error("Undefined text block...\n%s" % contents)
                raise ParseIException
            if end_index < start_index:
                error("No block begin declared...\n%s" % contents[0:end_index])
                raise ParseIException
            block = contents[start_index+len(self._start_key):end_index]
            block = block.strip(' ').rstrip(' ').rstrip('\n').rstrip(' ')
            if not block:
                error("Found an empty block...")
                raise ParseIException
            if block.split('\n') < 2:
                error("Found a block with no parameters...\n%s" % block)
                raise ParseIException
            self._scfg_v.append(self.parse_block(block))
            contents=contents[end_index+len(self._end_key):len(contents)]
            contents=contents.strip(' ').rstrip(' ').rstrip('\n')

        return len(self._blocks)

    def parse_block(self,block):
        lines = block.split('\n')
        words = lines[0].split()
        if not len(words) == 2:
            error("%s needs name & id (found \"%s\")" % (self._start_key,lines[0]))
            raise ParseIException
        if not words[1].isdigit():
            error("Invalid SubConfig (%s) ID ... \"%s\"" % (words[0],words[1]))
            raise ParseIException

        scfg = ubpsql.SubConfig(words[0],int(words[1]))

        for x in xrange(len(lines)):
            if x == 0: continue
            line = lines[x]
            if line.find('=>') > 0:
                words=line.split('=>')
                if not len(words) == 2:
                    error('Invalid parameter set specification:\n%s' % line)
                    raise ParseIException()
                words[0]=words[0].strip(' ').rstrip(' ')
                words[1]=words[1].strip(' ').rstrip(' ')
                if not words[1].isdigit():
                    error('Invalid parameter set ID (\"%s\")' % words[1])
                    raise ParseIException()
                tmp = ubpsql.SubConfig(words[0],int(words[1]))
                scfg.Append(tmp)
            elif line.find(':') > 0:
                key=line[0:line.find(':')].strip(' ').rstrip(' ')
                value=line[line.find(':')+1:len(line)].strip(' ').rstrip(' ')
                if not key or not value:
                    error('Invalid parameter specification:\n%s' % line)
                    raise ParseIException()
                scfg.Append(key,value)
            else:
                error('Invalid line inside sub-config block:\n%s' % line)
                raise ParseIException
                
        return scfg

if __name__=='__main__':
    k=SubConfigParser()
    k.read_file(sys.argv[1])
