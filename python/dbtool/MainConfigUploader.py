from UBPyCPP import ubpsql
from UBPyBase import UBPyBase
from UBPySQLException import ParseIException
from ask_binary import ask_binary
import sys

class MainConfigUploader(UBPyBase):

    _mcfg_v = []
    _api = ubpsql.ConfigWriter()

    def __init__(self):
        super(self.__class__,self).__init__()
        if not self._api.Connect():
            self.error('Failed to connect to the DB!')
            raise ParseIException() 

    def AddMainConfig(self,cfg):
        if not type(cfg) == type(ubpsql.MainConfig('fake')):
            self.error('Cannot add incompatible type MainConfig...')
            raise ParseIException()
        self._mcfg_v.append(cfg)

    def Upload(self):

        dep_map={}
        upload_list=[]
        for cfg in self._mcfg_v:
            if self._api.ExistMainConfig(cfg.Name()):
                self.info('  Already existing configuration \"%s\"' % cfg.Name())
                continue
            child_id_v  = cfg.ListSubConfigIDs()
            self.info('  New MainConfig: %s' % cfg.Name())
            missing_list = []
            for child_index in xrange(child_id_v.size()):
                child_id = child_id_v[child_index]
                if not self._api.ExistSubConfig(child_id.Name(),child_id.ID()):
                    missing_list.append((child_id.Name(),child_id.ID()))
            if missing_list:
                self.warning('    ... but cannot add this config because of missing dependencies:')
                for missing_id in missing_list:
                    self.error('    MISSING: %s w/ ID = %d' % missing_id)
            else:
                upload_list.append(cfg)

        if not upload_list: return True
            
        if not ask_binary("  Found %d new main-configurations..." % len(upload_list)): return True
        
        for index in xrange(len(upload_list)):
            cfg = upload_list[index]
            self.warning('  Attempting to upload: %s ' % cfg.Name())
            cfg_id = self._api.InsertMainConfiguration(cfg)
            if cfg_id >= 0:
                self.info('  Successfully uploaded (ID=%d)' % cfg_id)
                                    
