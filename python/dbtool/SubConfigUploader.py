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
                print "cfg %s ... %d" % (cfg.ID().Name(),cfg.ID().ID())
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
                        validated = self.Check( cfg )
                        if validated is False:
                            self.error('  Config %s %d fails the validation!' % ( cfg.ID().Name(), cfg.ID().ID() ) )
                            continue
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

    def Check( self, cfg ):
        validated = True

        # Below is the list of the subconfigs which we need to check
        sebAppsebConfigs = []
        for i in xrange(1, 11):
            name = "%s%02d" % ("sebAppseb", i)
            sebAppsebConfigs.append(name)

        # Now decide if this subconfig has something to be checked...
        if cfg.ID().Name() in sebAppsebConfigs:
            validated = self.CheckSebAppConfig( cfg )

        if cfg.ID().Name() == 'controller':
            validated = self.CheckController( cfg )

        return validated
    # Check( self, cfg )                

    def CheckSebAppConfig( self, cfg ):
        self.info('  Checking sub-config %s ...' % cfg.ID().Name() )
        subconfigNames = cfg.ListSubConfigs()
        # List of all the possible modules, PCIes, streams, etc.
        # Subject to update
        Modules = [ 'xmit', 'controller', 'tpc', 'pmt', 'trigger', 'shaper' ]
        PCIes   = [ 'neviscard_controller', 'neviscard_nu', 'neviscard_sn' ]
        Streams = [ 'nu_stream', 'sn_stream' ]
        EnabledModules = [ isubconfig for isubconfig in subconfigNames if isubconfig in Modules ]
        EnabledPCIes   = [ isubconfig for isubconfig in subconfigNames if isubconfig in PCIes ]
        EnabledStreams = [ isubconfig for isubconfig in subconfigNames if isubconfig in Streams ]
        Params = {}

        if not "registration" in subconfigNames:
            self.error('  Sub-config %s must have registration!' % cfg.ID().Name() )
            return False

        if not "controller" in subconfigNames:
            self.error('  Sub-config %s must have controller!' % cfg.ID().Name() )
            return False
        
        for isubconfig in subconfigNames:
            iID = cfg.GetSubConfig( isubconfig )
            Params[isubconfig] = self.LoadSubParams( iID )

        # Check if we need an xmit
        nStreams = 0
        for istream in EnabledStreams:
            # How many readout streams?
            if not ( Params[istream]['source'] == 'pseudo_data' ):
                nStreams += 1
        if ( nStreams > 0 ) and not ( 'xmit' in EnabledModules ):
            self.error('  Need XMIT for the readout stream!')
            return False
        if ( nStreams > 0 ) and not ( 'neviscard_controller' in EnabledPCIes ):
            self.error('  Need neviscard_controller for the readout stream!')
            return False

        # Check if any slot is colliding
        slot = [ 'none' ] * 21
        biLoc = [ 'tpc', 'pmt', 'shaper' ]
        for islot in xrange( 21 ):
            for imodule in EnabledModules:
                loc = int(Params[imodule]['slot'], 0)
                if imodule in biLoc:
                    if ( loc & ( 1 << islot ) ):
                        if slot[islot] == 'none':
                            slot[islot] = imodule
                        else:
                            self.error('  Slot %d have %s and %s!' %( slot[islot], imodule ) )
                            return False
                else:
                    if ( loc == islot ):
                        if slot[islot] == 'none':
                            slot[islot] = imodule
                        else:
                            self.error('  Slot %d have %s and %s!' %( slot[islot], imodule ) )
                            return False

        # Now check if the xmit is on the right of TPC/PMT FEMs, and if TPC/PMT FEMs are consecutive
        stage = 'beforeFEMs'
        for islot in xrange( 21 ):
            if ( slot[islot] == 'xmit' ):
                if ( stage == 'beforeFEMs' ):
                    stage = 'xmit'
                else:
                    self.error('  A FEM is at the right side of xmit!')
                    return False

            elif ( slot[islot] == 'tpc' ):
                if ( stage == 'xmit' ):
                    stage = 'tpc'
                elif ( stage == 'tpc' ):
                    pass
                else:
                    self.error('  Neither XMIT nor a TPC FEM is at the right side of this TPC FEM!')
                    return False

            elif ( slot[islot] == 'pmt' ):
                if ( stage == 'xmit' ):
                    stage = 'pmt'
                elif ( stage == 'pmt' ):
                    pass
                else:
                    self.error('  Neither XMIT nor a PMT FEM is at the right side of this PMT FEM!')
                    return False

            else:
                if ( stage == 'tpc' ) or ( stage == 'pmt' ):
                    stage = 'afterFEMs'
                elif ( stage == 'xmit' ):
                    self.error('  There is an XMIT before %s' % slot[islot] )
                    return False


        # Check nu and sn streams
        for istream in EnabledStreams:
            type = istream.split('_')[0]
            isource = '%s_source' % type
            icompress = '%s_compress' % type
            ipcie = 'neviscard_%s' % type

            if not Params[istream]['trigger_source'] == Params['registration']['trigger_source']:
                self.error('  trigger_source in %s (%s) should be identical to that in registration (%s)!' %( istream, Params[istream]['trigger_source'], Params['registration']['trigger_source'] ) )
                return False
            if not Params[istream]['xmit_timesize'] == Params['xmit']['timesize']:
                self.error('  xmit_timesize in %s (%s) should be identical to timesize in xmit (%s)!' % ( istream, Params['xmit']['timesize'], Params[istream]['xmit_timesize']) )
                return False
            if not Params[istream]['source'] == Params['xmit'][isource]:
                self.error('  source in %s (%s) should be identical to %s in xmit (%s)!' % ( istream, Params[istream]['source'], isource, Params['xmit'][isource] ) )
                return False
            if not Params[istream]['compress'] == Params['xmit'][icompress]:
                self.error('  compress flag in %s (%s) should be identical to %s flag in xmit (%s)!' % ( istream, Params[istream]['compress'], icompress, Params['xmit'][icompress] ) )
                return False
            if not ipcie in EnabledPCIes:
                self.error('  Need %s PCIe for %s' % ( ipcie, istream ) )
                return False

        return True
    # CheckSebAppConfig( self, cfg )

    def CheckController( self, cfg ):
        print cfg
        Param = cfg.Parameters()
        if not int(Param["slot"]) == 0:
            self.error('  Controller must be in slot 0!')
            return False

        return True
    # CheckController( self, cfg )

    def LoadSubParams( self, iID ):
        iReader = ubpsql.ConfigReader()
        icfg = iReader.GetSubConfig( iID.ID().Name(), iID.ID().ID() )
        iParam = icfg.Parameters()
        return iParam
    # LoadParams( self, cfg, subcfgName )
