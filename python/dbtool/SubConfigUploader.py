from UBPyCPP import ubpsql
from UBPyBase import UBPyBase
from UBPySQLException import ParseIException
from ask_binary import ask_binary
import sys

class SubConfigUploader(UBPyBase):

    _scfg_v = []
    _api = ubpsql.ConfigWriter()

    def __init__(self):
        super(self.__class__,self).__init__()
        if not self._api.Connect():
            self.error('Failed to connect to the DB!')
            raise ParseIException() 

    def AddSubConfig(self,cfg):
        if not type(cfg) == type(ubpsql.SubConfig('fake',1)):
            self.error('Cannot add incompatible type SubConfig...')
            raise ParseIException()
        self._scfg_v.append(cfg)

    def Upload(self):

        dep_map={}
        for cfg in self._scfg_v:
            child_id_v = cfg.ListSubConfigIDs()
            if not child_id_v.size() in dep_map:
                dep_map[child_id_v.size()]=[]
            dep_map[child_id_v.size()].append(cfg)
        keys=dep_map.keys()
        keys.sort()
        local_conf_list=[]
        upload_list=[]
        creation_list=[]
        for k in keys:
            for cfg in dep_map[k]:
                name_id = (cfg.ID().Name(),cfg.ID().ID())
                # Check this is already-existing cfg or not
                if self._api.ExistSubConfig(name_id[0],name_id[1]):
                    self.info('  Already existing configuration \"%s\" w/ ID = %d' % name_id)
                else:
                    # Check if this config can be registered or not
                    self.warning('  New Configuration \"%s\" w/ ID = %d' % name_id)
                    child_id_v = cfg.ListSubConfigIDs()
                    missing_list = []
                    for child_id_index in xrange(child_id_v.size()):
                        child_id = child_id_v[child_id_index]
                        if self._api.ExistSubConfig(child_id.Name(),child_id.ID()):
                            continue
                        if (child_id.Name(),child_id.ID()) in local_conf_list:
                            continue
                        missing_list.append((child_id.Name(),child_id.ID()))
                    if len(missing_list):
                        self.warning('    ... but cannot add this config because of missing dependencies:')
                        for missing_id in missing_list:
                            self.error('    MISSING: %s w/ ID = %d' % missing_id)
                    else:
                        local_conf_list.append(name_id)
                        upload_list.append(cfg)
                        if not self._api.ExistSubConfig(name_id[0]):
                            creation_list.append(name_id[0])
        if not upload_list:
            self.warning('  Nothing to be uploaded.')
            return True

        if not ask_binary("  Found %d new sub-configurations..." % len(upload_list)): return True

        if creation_list:
            self.warning('  Needs to create the following new sub-configurations!')
            for name in creation_list:
                self.warning('    \"%s\"' % name)
            if not ask_binary('  Proceed?'): return True
            death_star = ubpsql.DeathStar()
            for name in creation_list:
                death_star.CreateSubConfiguration(name)

        for index in xrange(len(upload_list)):
            cfg = upload_list[index]
            name_id = local_conf_list[index]
            self.warning('  Attempting to upload: %s w/ ID = %d' % name_id)
            self._api.InsertSubConfiguration(cfg)

                                    
