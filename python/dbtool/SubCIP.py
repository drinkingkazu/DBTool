from UBPySQLException import *
from UBPyBase import UBPyBase
import os,sys,ctypes
from ROOT import ubpsql, std

class SubCIP(UBPyBase):

    _cfg_def_key = 'DEFINE_CONFIG'
    _cfg_add_key = 'ADD_CONFIG'

    def __init__(self):
        super(SubCIP,self).__init__()

    def parse(self,fname):

        if not os.path.isfile(fname):
            self.critical('%s does not exist!' % fname)
            raise ParseIException

        content = open(fname,'r').read()
        content = self._strip_comment_(content)
        (content,cfg_id) = self._parse_id_(content)

        for x in self._parse_params_(content):
            cfg_id.append(x[0],x[1])

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
        param_start = content.find('{',cfg_start+1)
        param_end   = content.find('}',param_start+1)
        cfg_end     = content.find('}',param_end+1)

        if cfg_start < 0 or param_start < 0 or param_end < 0 or cfg_end < 0 or \
           not content[cfg_loc:cfg_start].split(None) == [self._cfg_add_key] or \
           not len(content[cfg_start:param_start].split(None)) or \
           not content[cfg_start:param_start].split(None)[-1] == 'PARAMETERS':

            self.critical('Keyword \"%s\" not followed by a valid {} block' % self._cfg_add_key)
            raise ParseIException

        tmp_cfg_def = content[cfg_start+1:param_start].split(None)
        cfg_def = []
        for x in tmp_cfg_def:
            if not x: continue
            if x == 'PARAMETERS': continue
            if x.find('=') >=0: 
                for y in x.split('='):
                    if y: cfg_def.append(y)
            else: cfg_def.append(x)

        if not (len(cfg_def)) in [6,8]:
            self.critical('%s block contains invalid number of params (crate, slot, ch, and/or mask)' % \
                          self._cfg_add_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException

        # Get parameter ID
        crate = []
        slot  = []
        ch    = []
        mask  = 0
        for i in xrange(len(cfg_def)-1):
            try:
                if cfg_def[i] == 'crate':
                    for x in cfg_def[i+1].split(','):
                        if x.find('>>') < 0:
                            crate.append(int(x))
                        else:
                            ends=x.split('>>')
                            if ( not len(ends)==2 or
                                 not ends[0].isdigit() or
                                 not ends[1].isdigit() or
                                 int(ends[0]) > int(ends[1]) ):
                                self.critical('%s block contains invalid crate expression (\"%s\")' % \
                                              (self._cfg_add_key,cfg_def[i+1]))
                                raise ParserIException
                            start,end = (int(ends[0]),int(ends[1]))
                            for crate_index in xrange(end-start+1):
                                crate.append(start+crate_index)
                if cfg_def[i] == 'slot':
                    for x in cfg_def[i+1].split(','):
                        if x.find('>>') < 0:
                            slot.append(int(x))
                        else:
                            ends=x.split('>>')
                            if ( not len(ends)==2 or
                                 not ends[0].isdigit() or
                                 not ends[1].isdigit() or
                                 int(ends[0]) > int(ends[1]) ):
                                self.critical('%s block contains invalid slot expression (\"%s\")' % \
                                              (self._cfg_add_key,cfg_def[i+1]))
                                raise ParserIException
                            start,end = (int(ends[0]),int(ends[1]))
                            for slot_index in xrange(end-start+1):
                                slot.append(start+slot_index)
                if cfg_def[i] == 'channel':
                    for x in cfg_def[i+1].split(','):
                        if x.find('>>') < 0:
                            ch.append(int(x))
                        else:
                            ends=x.split('>>')
                            if ( not len(ends)==2 or
                                 not ends[0].isdigit() or
                                 not ends[1].isdigit() or
                                 int(ends[0]) > int(ends[1]) ):
                                self.critical('%s block contains invalid ch expression (\"%s\")' % \
                                              (self._cfg_add_key,cfg_def[i+1]))
                                raise ParserIException
                            start,end = (int(ends[0]),int(ends[1]))
                            for ch_index in xrange(end-start+1):
                                ch.append(start+ch_index)
            except TypeError, ValueError:
                self.critical('%s block contains invalid value (\"%s\") for a key %s' % \
                              (self._cfg_add_key,cfg_def[i+1],cfg_def[i]))
                self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                raise ParseIException

        check_map = {'crate' : crate, 'slot' : slot, 'channel' : ch}

        for x in check_map.keys():
            if not len(check_map[x]) :
                self.critical('%s block does not contain \"%s\" key!' % (self._cfg_add_key,x))
                self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                raise ParseIException
            for y in check_map[x]:
                if y <0 and not y in [-1,-999]:
                    self.critical('%s block contain invalid value (\"%s\") for \"%s\" key!' % (self._cfg_add_key,y,x))
                    self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                    raise ParseIException                    
        
        # Now parse parameter values
        params = {}
        target = content[param_start+1:param_end]
        target = target.replace('=',' = ').split(None)

        if not len(target)%3 == 0:
            self.critical('%s block contains invalid parameter key-value pair expression' % self._cfg_add_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException

        for x in xrange(len(target)/3):

            index = 3*x
            if not target[index+1] == '=':
                self.critical('%s block contains invalid parameter key-value pair expression' % self._cfg_add_key)
                print index,target
                self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                raise ParseIException

            key   = target[index]
            value = target[index+2]

            if key in params.keys():
                self.critical('%s block contains duplicate parameter key (\"%s\")' % self._cfg_add_key)
                self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                raise ParseIException
            params[key]=value

        res = []
        for c in crate:
            for s in slot:
                for i in ch:
                    res.append([ubpsql.CParamsKey(c,s,i),ubpsql.CParams(mask)])

                    for x in params.keys():
                        res[-1][-1].append(x,params[x])

        return (content.replace(content[cfg_loc:cfg_end+1],''),res)
            
    def _parse_params_(self,content):
        
        params = []
        while 1:
            (content,ps) = self._parse_one_param_(content)
            if not ps: break
            for p in ps:
                params.append(p)
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
        if not (len(cfg_def)) in [4,6]:
            self.critical('%s block contains invalid number of params (name, id, and/or mask)' % self._cfg_def_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException
        cfg_name = ''
        cfg_id   = -1
        cfg_mask = 0
        for i in xrange(len(cfg_def)-1):
            try:
                if cfg_def[i] == 'name':
                    cfg_name = cfg_def[i+1]
                if cfg_def[i] == 'id':
                    cfg_id   = int(cfg_def[i+1])
                if cfg_def[i] == 'mask':
                    cfg_mask = int(cfg_def[i+1],0)
            except TypeError, ValueError:
                self.critical('%s block contains invalid value (\"%s\") for a key %s' % \
                              (self._cfg_def_key,cfg_def[i+1],cfg_def[i]))
                self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
                raise ParseIException

        if not cfg_name:
            self.critical('%s block does not contain \"name\" key!' % self._cfg_def_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException
        if cfg_id < 0:
            self.critical('%s block does not contain \"id\" key!' % self._cfg_def_key)
            self.info('Contents shown below\n %s' % content[cfg_loc:cfg_end+1])
            raise ParseIException

        return ( content.replace(content[cfg_loc:cfg_end+1],''), 
                 ubpsql.SubConfig(cfg_name, cfg_id, cfg_mask) )

if __name__ == '__main__':

    c=SubCIP()
    c.parse(sys.argv[1])
