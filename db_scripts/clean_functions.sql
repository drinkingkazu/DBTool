
DROP FUNCTION IF EXISTS ExistMainConfig(MCfgName TEXT);
DROP FUNCTION IF EXISTS ExistMainConfig(MCfgID INT);
DROP FUNCTION IF EXISTS MainConfigName(MCfgID INT);
DROP FUNCTION IF EXISTS MainConfigName(MCfgName TEXT);
DROP FUNCTION IF EXISTS ExistSubConfig(SCfgName TEXT);
DROP FUNCTION IF EXISTS ExistSubConfig(SCfgType INT);
DROP FUNCTION IF EXISTS ExistSubConfig(SCfgName TEXT, SCfgID INT);
DROP FUNCTION IF EXISTS SubConfigName(SCfgType INT);
DROP FUNCTION IF EXISTS SubConfigType(SCfgName TEXT);
DROP FUNCTION IF EXISTS MainConfigName(MCfgName TEXT);
DROP FUNCTION IF EXISTS ListSubConfigs(CfgID INT);
DROP FUNCTION IF EXISTS CreateSubConfiguration( config_name TEXT );
DROP FUNCTION IF EXISTS SubConfigType( sub_conf_name TEXT );
DROP FUNCTION IF EXISTS CleanSubConfigurationID( cfg_name TEXT,
       	  	  	   		         cfg_id   INT);
DROP FUNCTION IF EXISTS ListParameters(SCfgName TEXT, SCfgID INT);
DROP FUNCTION IF EXISTS ListParameterSets(SCfgName TEXT, SCfgID INT);
DROP FUNCTION IF EXISTS ListSubConfigs();
DROP FUNCTION IF EXISTS ListMainConfigs();
DROP FUNCTION IF EXISTS ListDependencies(SCfgName TEXT, SCfgID INT);
DROP FUNCTION IF EXISTS ListDependencies(sub_config HSTORE);
DROP FUNCTION IF EXISTS ListDependencies(main_config_id INT);
DROP FUNCTION IF EXISTS mainconfigcheck();
DROP FUNCTION IF EXISTS InsertSubConfiguration( config_name TEXT,
                                      config_id   INT,
                                      params      HSTORE,
                                      paramsets   HSTORE
                                      );
DROP FUNCTION IF EXISTS MaxSubConfigID(configtablename TEXT);
DROP FUNCTION IF EXISTS SubConfigType( sub_conf_name TEXT );
DROP FUNCTION IF EXISTS SubConfigID( main_conf_name TEXT,sub_conf_name TEXT );
DROP FUNCTION IF EXISTS InsertMainConfiguration( subconfigparameters HSTORE,
     	      			       confname TEXT);
DROP FUNCTION IF EXISTS CleanSubConfig( cfg_name TEXT,
       	     		      cfg_id   INT);
DROP FUNCTION IF EXISTS CleanSubConfig( cfg_name TEXT );
DROP FUNCTION IF EXISTS CleanMainConfig( cfg_name TEXT);
DROP FUNCTION IF EXISTS CleanConfigDB();

DROP FUNCTION IF EXISTS InsertNewRun(CfgID INT);
DROP FUNCTION IF EXISTS InsertNewSubRun(CfgID INT,run INT);
DROP FUNCTION IF EXISTS RunExist( run INT );
DROP FUNCTION IF EXISTS RunExist( run INT, subrun INT );
DROP FUNCTION IF EXISTS InsertNewSubRun( run INT,
                                       	 subrun INT,
                                         ts TIMESTAMP,
                                         te TIMESTAMP,
                                         CfgID INT);
DROP FUNCTION IF EXISTS UpdateStopTime( run INT,
                                           subrun INT,
                                           ts TIMESTAMP);
DROP FUNCTION IF EXISTS GetLastRun();
DROP FUNCTION IF EXISTS GetLastSubRun(run INT);
