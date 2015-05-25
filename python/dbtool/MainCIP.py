from UBPySQLException import *
from UBPyBase import UBPyBase
import os,sys,ctypes
from DBToolCPP import ubpsql, std

class MainCIP(UBPyBase):

    _cfg_def_key = 'DEFINE_CONFIG'
    _cfg_add_key = 'ADD_CONFIG'

    def __init__(self):
        super(MainCIP,self).__init__()

    def parse(self,fname):

        if not os.path.isfile(fname):
            self.critical('%s does not exist!' % fname)
            raise ParseIException

        content = open(fname,'r').read()
        content = self._strip_comment_(content)
        (content,cfg_id) = self._parse_id_(content)
        for x in self._parse_params_(content):
            cfg_id.AddSubConfig(x)
        
        cfg_id.ls()
        return cfg_id
        
    def _strip_comment_(self,content):
        new_content = ''
        for line in content.split('\n'):
            if line.find('#') < 0: new_content += '%s\n' % line
            else: new_content += '%s\n' % line[0:line.find('#')]

        return new_content

    def _parse_one_param_(self,content):
        # Check if config param key is there
        cfg_loc = content.find(self._cfg_add_key)
        if cfg_loc < 0: return (content,None)

        cfg_start   = content.find('{',cfg_loc+1)
        cfg_end     = content.find('}',cfg_start+1)

        if cfg_start < 0 or cfg_end < 0 or \
           not content[cfg_loc:cfg_start].split(None) == [self._cfg_add_key]:
            self.critical('Keyword \"%s\" not followed by a valid {} block' % self._cfg_add_key)
            raise ParseIException

        tmp_cfg_def = content[cfg_start+1:cfg_end].split(None)
        cfg_def = []
        for x in tmp_cfg_def:
            if not x: continue
            if x.find('=') >=0: 
                for y in x.split('='):
                    if y: cfg_def.append(y)
            else: cfg_def.append(x)

        if not (len(cfg_def)) in [4,6]:
            self.critical('%s block contains invalid number of params (crate, slot, ch, and/or mask/name)' % \
                          self._cfg_add_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException

        # Get parameters
        cfg_name = ''
        cfg_id   = -1000
        cfg_mask = 0
        for i in xrange(len(cfg_def)-1):
            try:
                if cfg_def[i] == 'mask':
                    cfg_mask = int(cfg_def[i+1],0)
                if cfg_def[i] == 'config_id':
                    cfg_id   = int(cfg_def[i+1])
                if cfg_def[i] == 'name':
                    cfg_name = str(cfg_def[i+1])
            except TypeError, ValueError:
                self.critical('%s block contains invalid value (\"%s\") for a key %s' % \
                              (self._cfg_add_key,cfg_def[i+1],cfg_def[i]))
                self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                raise ParseIException

        if not cfg_name:
            self.critical('%s block does not contain \"name\" key!' % self._cfg_add_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException
        if cfg_id < 0:
            self.critical('%s block contains negative sub-config ID (%d)' % (self._cfg_add_key,cfg_id))
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException

        res = ubpsql.SubConfig(cfg_name,cfg_id,cfg_mask)

        return (content.replace(content[cfg_loc:cfg_end+1],''),res)
            
    def _parse_params_(self,content):
        
        params = []
        while 1:
            (content,ps) = self._parse_one_param_(content)
            if not ps: break
            params.append(ps)
        return params

    def _parse_id_(self,content):

        # Check if config def key is there
        cfg_loc = content.find(self._cfg_def_key)

        if cfg_loc < 0:
            self.critical('Did not find %s keyword' % self._cfg_def_key)
            raise ParseIException
        if not content.find(self._cfg_def_key,cfg_loc+1) < 0:
            self.critical('Found a keyword %s in 2 places' % self._cfg_def_key)
            raise ParseIException
        
        cfg_start = content.find('{',cfg_loc)
        cfg_end   = content.find('}',cfg_loc)

        if cfg_start < 0 or cfg_end < 0 or cfg_end < cfg_start or not content[cfg_loc:cfg_start].split(None) == [self._cfg_def_key]:
            self.critical('Keyword \"%s\" not followed by a valid {} block' % self._cfg_def_key)
            raise ParseIException

        # make sure there are necessary (and only necessary) components in def block
        tmp_cfg_def = content[cfg_start+1:cfg_end].split(None)
        cfg_def = []
        for x in tmp_cfg_def:
            if not x: continue
            if x.find('=') >=0: 
                for y in x.split('='):
                    if y: cfg_def.append(y)
            else: cfg_def.append(x)

        # check contents
        if not (len(cfg_def)) == 2:
            self.critical('%s block contains invalid number of params (name, id, and/or mask)' % self._cfg_def_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException
        cfg_name = ''
        for i in xrange(len(cfg_def)-1):
            try:
                if cfg_def[i] == 'name':
                    cfg_name = cfg_def[i+1]
            except TypeError, ValueError:
                self.critical('%s block contains invalid value (\"%s\") for a key %s' % \
                              (self._cfg_def_key,cfg_def[i+1],cfg_def[i]))
                self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                raise ParseIException

        if not cfg_name:
            self.critical('%s block does not contain \"name\" key!' % self._cfg_def_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException

        return ( content.replace(content[cfg_loc:cfg_end+1],''), 
                 ubpsql.RunConfig(cfg_name))

if __name__ == '__main__':

    c=MainCIP()
    c.parse(sys.argv[1])
