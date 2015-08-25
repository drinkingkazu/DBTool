#!/usr/bin/env python
from dbtool import fcl2py,ubpsql,ask_binary
from dbtool.colored_msg import info,warning,error
import sys

db_writer = ubpsql.ConfigWriter()

class lite_cfg(dict):

    def __init__(self,name='',d=dict(),level=-1):
        self._name=name
        self._level=level
        for x in d:
            self[x] = d[x]

def make_lite_cfg(name, d, level):
    res = lite_cfg(name,d,level)
    contains_dict = False
    for key, value in d.iteritems():
        if isinstance(value, dict):
           res[key] = make_lite_cfg(key,value,level+1)
    return res

def print_lite_cfg(cfg,indent=0):
   contains_cfg = False
   for key, value in cfg.iteritems():
      msg = '\t' * indent
      if isinstance(value, lite_cfg):
         print msg,'\033[93m',str(key),'\033[00m','Level',value._level
         print_lite_cfg(value, indent+1)
         contains_cfg=True
      else:
         print msg,str(key),':',str(value)
   if indent and not contains_cfg:
      print '\t' * indent,'\033[95mBASE SUB-CFG\033[00m'

def register_lite_cfg(cfg):
   db_cfg_name = cfg._name
   child_cfg=[]
   child_param=[]
   for key,value in cfg.iteritems():
      if isinstance(value,lite_cfg):
         child_id = register_lite_cfg(value)
         if child_id < 0:
            return child_id
         child_cfg.append(ubpsql.SubConfig(str(key),child_id))
      else:
         child_param.append((key,value))

   # See if this sub-config already exist
   #cmd='tmp_db_cfg=ubpsql.SubConfig(\'%s\',0)' % db_cfg_name
   #exec(cmd)
   tmp_db_cfg = ubpsql.SubConfig(str(db_cfg_name))
   for cfg in child_cfg:
      tmp_db_cfg.Append(cfg)
   for param in child_param:
      tmp_db_cfg.Append(str(param[0]),str(param[1]))
   db_cfg_id = db_writer.FindSubConfig(tmp_db_cfg)

   if db_cfg_id >= 0: return db_cfg_id

   # New config found. Upload...
   if db_writer.ExistSubConfig(str(db_cfg_name)):
      ids = db_writer.SubConfigIDs(str(db_cfg_name))
      for id_index in xrange(ids.size()):
         if db_cfg_id < ids[id_index]:
            db_cfg_id = ids[id_index]
      db_cfg_id += 1
   else:
      db_cfg_id = 0

   msg = '  Uploading a new sub config: %s (ID=%d)\n' % (db_cfg_name,db_cfg_id)
   db_cfg = ubpsql.SubConfig(str(db_cfg_name),db_cfg_id)
   for param in child_param:
      db_cfg.Append(str(param[0]),str(param[1]))
      msg += '    %s : %s\n' % (param[0],param[1])
   for cfg in child_cfg:
      db_cfg.Append(cfg)
      msg += '    %s => %d\n' % (cfg.ID().first,cfg.ID().second)

   warning(msg)
   if not ask_binary('Proceed?'): return -1
   if not db_writer.InsertSubConfiguration(db_cfg):
      raise Exception()

   return db_cfg_id
   
def dict_config(f):
   
   contents = open(f,'r').read()
   contents = fcl2py.Parse(contents)

   if not type(contents) == type(dict()):
      print 'Error: top level must be dict!'
      print type(contents)
      sys.exit(1)

   for k in contents:
      if not type(contents[k]) == type(list()):
         print 'Error: 2nd level must be list!'
         sys.exit(1)
      if not len(contents[k]) == 1:
         print 'Error: 2nd level must be length 1!'
         sys.exit(1)

   for k in contents:
      contents[k]=contents[k][0]

   return contents

if __name__ == '__main__':
    config = dict_config(sys.argv[1])
    kazu = make_lite_cfg('main',config,0)
    top_sub_cfg_v = []
    for value in kazu.values():
        sub_cfg_name = value._name
        sub_cfg_id   = register_lite_cfg(value)
        
        if sub_cfg_id < 0 :
            warning('Aborting...')
            sys.exit(1)
        top_sub_cfg_v.append((sub_cfg_name,sub_cfg_id))

    if not ask_binary('\033[95mWould you like to upload this fcl as a main configuration?\033[00m'):
        sys.exit(0)
    cfg_name=''
    while not cfg_name:
        print '\033[95mEnter MainConfig name:\033[00m ',
        sys.stdout.flush()
        cfg_name=sys.stdin.readline().rstrip('\n')
        if cfg_name.find(' ')>=0:
            error('MainConfig name cannot contain space!')
            cfg_name = ''
            continue
        if db_writer.ExistMainConfig(str(cfg_name)):
            error('MainConfig \"%s\" already exists!' % cfg_name)
            cfg_name = ''
            continue

    print '\033[95mChoose RunType for this config:\033[00m '
    options=[]
    for x in xrange(ubpsql.kRunTypeMax):
        print '  %d => %s' % (x,ubpsql.RunTypeName(x))
        options.append(x)
    run_type=ubpsql.kRunTypeMax
    while run_type == ubpsql.kRunTypeMax:
        print '\033[95mEnter %s: ' % str(options),
        sys.stdout.flush()
        user_input = sys.stdin.readline().rstrip('\n')
        if not user_input.isdigit() or not int(user_input) in options:
            print '\033[91mInvalid input:\033[00m',user_input
            continue
        run_type = int(user_input)
    
    main_cfg_metadata = ubpsql.MainConfigMetaData()
    main_cfg_metadata.fName = cfg_name
    main_cfg_metadata.fRunType = run_type

    main_cfg = ubpsql.MainConfig(main_cfg_metadata)
    for top_sub_cfg_id in top_sub_cfg_v:
        main_cfg.AddSubConfig(ubpsql.SubConfig(str(top_sub_cfg_id[0]),int(top_sub_cfg_id[1])))
    
    main_cfg_id = db_writer.InsertMainConfiguration(main_cfg)
    if main_cfg_id < 0:
        error('Failed inserting a main configuration...')
    else:
        info('Done. MainConfig ID = %d' % main_cfg_id)
    sys.exit(0)
