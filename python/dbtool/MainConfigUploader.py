from UBPyCPP import ubpsql
from UBPyBase import UBPyBase
from UBPySQLException import ParseIException
from ask_binary import ask_binary
import fcl2py
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
                validated = self.Check( cfg )
                if validated is False:
                    self.error('  Config %s fails the validation!' % cfg.Name() )
                    continue
                upload_list.append(cfg)

        if not upload_list: return True
            
        if not ask_binary("  Found %d new main-configurations..." % len(upload_list)): return True
        
        for index in xrange(len(upload_list)):
            cfg = upload_list[index] # This is one "main config"
            
            self.warning('  Attempting to upload: %s ' % cfg.Name())
            cfg_id = self._api.InsertMainConfiguration(cfg)
            if cfg_id >= 0:
                self.info('  Successfully uploaded (ID=%d)' % cfg_id)

    def Check( self, cfg ):
        Params = self.LoadParams( cfg )
        validate = self.CheckSebAndAssembler( Params )
        if validate is False:
            return False
        return True
    # Check( self, cfg )

    # Check the consistency between the Assembler and Sebs.  NU stream only.
    def CheckSebAndAssembler( self, Params ):
        for iseb in xrange( 1, 11 ):
            sebReg = "seb%d" % iseb
            sebConfig = "sebAppseb%02d" % iseb
            if sebReg in Params['assemblerAppevb'][0]['registration'].keys():
                if not sebConfig in Params.keys():
                    self.error('  Cannot find the sebApp config %s!' % sebConfig )
                    return False
                for param in [ 'name', 'type' ]:
                    if Params['assemblerAppevb'][0]['registration'][sebReg][param] != Params[sebConfig][0]['nu_stream'][param]:
                        self.error('  The %s of the nu_stream in %s (%s) should be identical to that in the registration of the assembler (%s)!' % ( param, sebConfig, Params[sebConfig][0]['nu_stream'][param], Params['assemblerAppevb'][0]['registration'][sebReg][param] ) )
                        return False
                for param in [ 'port' ]:
                    if int(Params['assemblerAppevb'][0]['registration'][sebReg][param]) != int(Params[sebConfig][0]['nu_stream'][param]):
                        self.error('  The %s of the nu_stream in %s (%s) should be identical to that in the registration of the assembler (%s)!' % ( param, sebConfig, Params[sebConfig][0]['nu_stream'][param], Params['assemblerAppevb'][0]['registration'][sebReg][param] ) )
                        return False


        return True
    # CheckSebApps( self, Params ):

    def LoadParams( self, cfg ):
        params = {}
        subconfigs = cfg.ListSubConfigIDs()
        Reader = ubpsql.ConfigReader()
        for subconfig in subconfigs:
            isubcfg = Reader.GetSubConfig( subconfig.Name(), subconfig.ID() )
            d = fcl2py.Parse( isubcfg.Dump() )
            params[subconfig.Name()] = d[subconfig.Name()]
            
        return params
    # LoadParams( self, cfg )

