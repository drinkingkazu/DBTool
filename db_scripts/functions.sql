

-- CREATE LANGUAGE plpgsql;



--CREATE OR REPLACE FUNCTION testfunc(integer) RETURNS integer AS $PROC$
--          ....
--$PROC$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION InsertNewRun(CfgID INT)  RETURNS integer AS $$
DECLARE
    lastrun mainrun.RunNumber%TYPE;
BEGIN
     SELECT INTO lastrun GetLastRun();
   
     INSERT INTO MainRun(RunNumber,SubRunNumber,ConfigID,RunType,TimeStart) VALUES(lastrun+1,1,CfgID,333, 'now');


    RETURN lastrun+1;

END;
$$ LANGUAGE plpgsql;

------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS ExistSubConfig(SCfgName TEXT);
CREATE OR REPLACE FUNCTION ExistSubConfig(SCfgName TEXT) RETURNS INT AS $$
   SELECT SubConfigType FROM ConfigLookUp WHERE SubConfigName = SCfgName LIMIT 1
$$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS ExistSubConfig(SCfgName TEXT, SCfgID INT);
CREATE OR REPLACE FUNCTION ExistSubConfig(SCfgName TEXT, SCfgID INT) RETURNS INT AS $$
DECLARE
  exist INT;
  ctr INT;
  query TEXT;
BEGIN
   SELECT INTO exist ExistSubConfig(SCfgName);
   ctr := 0;
   IF exist IS NOT NULL AND exist>=0 THEN
     query := 'SELECT 1 FROM ' || SCfgName || ' WHERE ConfigID = ' || SCfgID || ';';
     RAISE NOTICE '%',query;
     EXECUTE query INTO ctr;
     IF ctr IS NULL THEN
       ctr := 0;
     END IF;
     RETURN ctr;
   END IF;
   RETURN 0;
END;
$$ LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS ListSubConfigParameters(SCfgName TEXT);
CREATE OR REPLACE FUNCTION ListSubConfigParameters(SCfgName TEXT) RETURNS SETOF TEXT AS $$
   SELECT skeys(setupcolumns) FROM configlookup WHERE subconfigname = SCfgName
$$ LANGUAGE SQL;

DROP FUNCTION IF EXISTS ListSubConfigNameAndID(CfgID INT);
DROP TYPE IF EXISTS SubConfigNameAndID;

CREATE TYPE SubConfigNameAndID AS (SubConfigName TEXT, SubConfigID INT);
CREATE OR REPLACE FUNCTION ListSubConfigNameAndID(CfgID INT) RETURNS SETOF SubConfigNameAndID AS $$
DECLARE
   tmp_type_and_id RECORD;
   tmp_name_and_id RECORD;
   res SubConfigNameAndID;
BEGIN
   FOR tmp_type_and_id IN SELECT SubConfigType, SubConfigID FROM MainConfigTable WHERE ConfigID = CfgID ORDER BY SubConfigType LOOP
   SELECT INTO tmp_name_and_id SubConfigName FROM ConfigLookUp WHERE SubConfigType = tmp_type_and_id.SubConfigType LIMIT 1;

   res.SubConfigName := tmp_name_and_id.SubConfigName;
   res.SubConfigID   := tmp_type_and_id.SubConfigID;
   RETURN NEXT res;
   END LOOP;
   RETURN;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION ListSubConfigNameAndID() RETURNS SETOF RECORD AS $$
       SELECT SubConfigName, SubConfigType FROM ConfigLookUp ORDER BY SubConfigType
$$ LANGUAGE SQL;


-------------------------------------------------------------------------------
---- determine what the relation between ConfitType and ConfigID is.
--------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION InsertNewSubRun(CfgID INT,run INT DEFAULT -1) RETURNS integer AS $$
DECLARE
    lastrun mainrun.RunNumber%TYPE;
    lastsubrun mainrun.SubRunNumber%TYPE;
BEGIN
     IF run = -1  THEN
	  SELECT INTO lastrun GetLastRun();
     ELSE 
          lastrun := run;
     END IF;    

     SELECT INTO lastsubrun GetLastSubRun(lastrun);

     INSERT INTO MainRun(RunNumber,SubRunNumber,ConfigID,RunType,TimeStart) VALUES(lastrun,lastsubrun+1,CfgID,333,'now');

    RETURN lastsubrun+1;
END;
$$ LANGUAGE plpgsql;



-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION GetLastRun() RETURNS integer AS $$
DECLARE
    lastrun mainrun.RunNumber%TYPE;
BEGIN
    
    SELECT INTO lastrun RunNumber FROM MainRun  ORDER BY RunNumber DESC LIMIT 1;

    RETURN lastrun;
END;
$$ LANGUAGE plpgsql;


-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION GetLastSubRun(run INT) RETURNS integer AS $$
DECLARE
    lastsubrun mainrun.SubRunNumber%TYPE;
BEGIN
    
    SELECT INTO lastsubrun SubRunNumber FROM MainRun WHERE RunNumber=run ORDER BY SubRunNumber DESC LIMIT 1;

    RETURN lastsubrun;
END;
$$ LANGUAGE plpgsql;


-------------------------------------------------------------------------------
--------------------------------------------------------------------------------


--to work on pmtid for the time being:

CREATE OR REPLACE FUNCTION emp_stamp() RETURNS trigger AS $emp_stamp$
    BEGIN
        -- Check that empname and salary are given
        IF NEW.ModuleID IS NULL THEN
            RAISE EXCEPTION 'ModuleID cannot be null';
        END IF;
    --    IF NEW.parameters IS NULL THEN
    --        RAISE EXCEPTION '% cannot have parameters =0', NEW.ModuleID;
      --  END IF;

       
     --   IF NEW.parameters < 0 THEN
     --       RAISE EXCEPTION '% cannot have a parameterx < 0', NEW.moduleID;
     --   END IF;

        -- Remember who changed the payroll when
        NEW.timestamp := 'now';
        NEW.userid := current_user;
        RETURN NEW;
    END;
$emp_stamp$ LANGUAGE plpgsql;

--CREATE TRIGGER emp_stamp BEFORE INSERT OR UPDATE ON pmtconfig
--    FOR EACH ROW EXECUTE PROCEDURE emp_stamp();



-- at least one line with the configuration must exist
CREATE OR REPLACE FUNCTION mainruncheck() RETURNS trigger AS $mainruncheck$
    DECLARE
    myrec RECORD;
    BEGIN
        -- Check that empname and salary are given
       SELECT INTO myrec * FROM mainconfigtable WHERE mainconfigtable.configid=NEW.configid;
       IF NOT FOUND THEN
	  RAISE EXCEPTION 'Configuration % not found', NEW.configid;
      END IF;

       
       -- Remember who changed the payroll when
      --  NEW.timestart := 'now';
       -- NEW.userid := current_user;
        RETURN NEW;
    END;
$mainruncheck$ LANGUAGE plpgsql;

--CREATE TRIGGER mainruncheck BEFORE INSERT OR UPDATE ON mainrun
 --   FOR EACH ROW EXECUTE PROCEDURE mainruncheck();




-- at least one line with the configuration must exist
CREATE OR REPLACE FUNCTION mainconfigcheck() RETURNS trigger AS $mainconfigcheck$
    DECLARE
    myrec RECORD;
    tablename ConfigLookUp.SubConfigName%TYPE;
    query   text;
    BEGIN
        -- Check that empname and salary are given
      RAISE INFO 'subconfigid % ', NEW.subconfigtype;

      SELECT  configname FROM configtypes WHERE configtypes.configtype=NEW.subconfigtype INTO tablename;

      RAISE INFO 'SELECTED % ', tablename;

     
   
  --  
    query := 'SELECT configid FROM ' || tablename || ' WHERE ' || tablename || '.configid =' || NEW.subconfigid ;

    RAISE INFO ' % ', query ;

    EXECUTE query INTO myrec; 
      IF myrec.configid IS NULL  THEN
	  RAISE EXCEPTION 'Configuration % not found', NEW.subconfigid;
      END IF;

     query := 'SELECT configid FROM ' || tablename || ' WHERE ' || tablename || '.configid =' || NEW.subconfigparentid ;

      RAISE INFO ' % ', query ;

      EXECUTE query INTO myrec; 
	IF myrec.configid IS NULL  THEN
	  RAISE EXCEPTION 'Parent Configuration % not found', NEW.subconfigparentid;
	END IF;
--       EXECUTE 'SELECT configid FROM '
-- 	|| tablename 
-- 	|| ' WHERE '
-- 	|| tablename
-- 	|| '.configid ='
-- 	|| substring(NEW.subconfigid,1,1) 
--       INTO myrec;
      --SELECT INTO myrec * FROM mainconfigtable WHERE mainconfigtable.configid=NEW.configid;
       


       
       -- Remember who changed the payroll when
       -- NEW.timestop := 'now';
       -- NEW.userid := current_user;
        RETURN NEW;
    END;
$mainconfigcheck$ LANGUAGE plpgsql VOLATILE STRICT;

--CREATE TRIGGER mainconfigcheck BEFORE INSERT OR UPDATE ON mainconfigtable
--    FOR EACH ROW EXECUTE PROCEDURE mainconfigcheck();

--------------------------------------------------------------------------------
-----------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS CreateCrateConfiguration( config_name TEXT );
DROP FUNCTION IF EXISTS CreateSubConfiguration( config_name TEXT );
CREATE OR REPLACE FUNCTION CreateSubConfiguration( config_name TEXT ) RETURNS INT AS $$

  DECLARE
    newconfig INT;
    myrec RECORD;
    query text;  
  BEGIN
    -- First find if a config like this exists, if not, insert it.
    IF EXISTS ( SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = config_name ) THEN
      RAISE EXCEPTION '++++++++++ Configuration Type % Already Exists +++++++++++', config_name;
    END IF;

    SELECT INTO myrec * FROM  ConfigLookUp ORDER BY SubConfigType DESC LIMIT 1;
    IF NOT FOUND THEN
	  newconfig:=1;
    ELSE 
          newconfig:=myrec.SubConfigType+1;
    END IF;

  -- INSERT test of HSTORE here.

  --CREATE TABLE - they all look the same.

   query := 'CREATE TABLE '|| config_name ||' (
   ConfigID        INT NOT  NULL DEFAULT NULL,
   Crate      SMALLINT NOT  NULL DEFAULT -1,
   Slot       SMALLINT NOT  NULL DEFAULT -1,
   Channel    SMALLINT NOT  NULL DEFAULT -1,
   Parameters   HSTORE NOT  NULL,
   Mask       BIT(64)  NOT  NULL,
   userID      VARCHAR NULL DEFAULT NULL,
   TimeStamp TIMESTAMP NULL DEFAULT NULL,
   PRIMARY KEY ( ConfigID, Crate, Slot, Channel ))';
 
   EXECUTE query; 
-- now that we've created the table, let's also insert the name into the lookup table.
       

   query:= 'INSERT INTO ConfigLookUp(SubConfigType,SubConfigName) VALUES('||newconfig||','''||config_name||''')';
   RAISE INFO '%',query;
   execute query;
   RAISE INFO 'Inserted new configuration type with ID %',newconfig;
  
    RETURN newconfig;
    --END IF;
   
    END;
$$ LANGUAGE plpgsql VOLATILE STRICT;


------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION IsDigit(text) RETURNS BOOLEAN as 'select $1 ~ ''^(-)?[0-9]+$'' as result'
LANGUAGE SQL;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS CheckNewCrateConfiguration( config_name TEXT,
     	      	 			            config_id   INT,
						    crate_id    INT,
						    slot_id     INT,
						    channel_id  INT,
						    params      HSTORE );

DROP FUNCTION IF EXISTS CheckNewSubConfiguration( config_name TEXT,
     	      	 			          config_id   INT,
						  crate_id    INT,
						  slot_id     INT,
						  channel_id  INT,
						  params      HSTORE );

CREATE OR REPLACE FUNCTION CheckNewSubConfiguration( config_name TEXT,
       	  	  	   			     config_id   INT,
						     crate_id    INT,
						     slot_id     INT,
						     channel_id  INT,
						     params      HSTORE ) RETURNS INT AS $$
  DECLARE
    myrec RECORD;
    newconfig INT;
    query text;  
    ColumnPair RECORD;
  BEGIN

 -- Check 0) Possible combination of crate_id, slot_id, and channel_id
    IF crate_id < 0 AND NOT (crate_id = -1 OR crate_id = -999) THEN
      RAISE WARNING '++++++++++ Invalid Crate ID: % +++++++++++', crate_id;
      RETURN 1;

    ELSIF slot_id < 0 AND NOT (slot_id = -1 OR slot_id = -999) THEN
      RAISE WARNING '++++++++++ Invalid Slot ID: % +++++++++++', slot_id;
      RETURN 1;

    ELSIF channel_id < 0 AND NOT (channel_id = -1 OR channel_id = -999) THEN
      RAISE WARNING '++++++++++ Invalid Channel ID: % +++++++++++', channel_id;
      RETURN 1;

    ELSIF ((crate_id + channel_id + slot_id) < -1001) THEN
      RAISE WARNING '++++++++++ Duplicate -999 values among (crate,slot,channel)=(%,%,%) ++++++++++++', crate_id, slot_id, channel_id;
      RETURN 1;

    ELSIF channel_id = -999 AND (crate_id = -1  AND slot_id >= 0) THEN
      RAISE WARNING '++++++++++ Crate ID must be 0 or positive integer for a specific slot default channel! ++++++++++';
      RETURN 1;

    ELSIF slot_id = -999 AND  channel_id != -1 THEN
      RAISE WARNING '++++++++++ Channel ID must be -1 if Slot ID is -999! +++++++++++';
      RETURN 1;

    ELSIF crate_id = -999 AND (slot_id >=0 OR channel_id >=0 ) THEN
      RAISE WARNING '++++++++++ Slot and Channel ID must be -1 for default Crate config (ID=-999) ++++++++++ (%,%,%)',crate_id,slot_id,channel_id;
      RETURN 1;

    ELSIF crate_id = -1 AND (slot_id >= 0 OR channel_id >= 0) THEN
      RAISE WARNING '++++++++++ (Crate,Slot,Channel) = (-1, %, %) not allowed! +++++++++++',slot_id,channel_id;
      RETURN 1;

    ELSIF crate_id >= 0 AND slot_id = -1 AND channel_id >= 0 THEN
      RAISE WARNING '++++++++++ (Crate,Slot,Channel) = (%, -1, %) not allowed! +++++++++++',crate_id,channel_id;
      RETURN 1;

    END IF;

 -- Check 1) find if this configuration type exists. If not, don't do anything
    IF NOT EXISTS ( SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = config_name) THEN
      RAISE WARNING '++++++++++ Configuration % is not defined yet! +++++++++++', config_name;
      RETURN 1;
    END IF;

 -- Check 2) find if this config ID is already defined or not. If not, don't do anything
    query := format('SELECT 1 AS VAL FROM %s WHERE ConfigID=%s LIMIT 1;',config_name,config_id);
    EXECUTE query INTO myrec;
    IF myrec.VAL IS NULL THEN
      RAISE WARNING '++++++++++ Configuration % with ID % is not defined yet! +++++++++++', config_name, config_id;
      RETURN 1;
    END IF;

 -- Check 3) find if a config like this exists. If does, don't do anything
    query := format('SELECT TRUE AS VAL FROM %s WHERE ConfigID=%s AND Crate=%s AND Slot=%s AND Channel=%s',config_name,config_id, crate_id, slot_id, channel_id);
    EXECUTE query INTO myrec;
    IF NOT myrec.VAL IS NULL THEN
      RAISE WARNING '++++++++++ (ConfigID,Crate,Slot,Channel) = (%,%,%,%) already exists! ++++++++++', config_id, crate_id, slot_id, channel_id;
      RETURN 1;
    END IF;

    -- INSERT HSTORE checking, i.e. for each(hstore from ConfigLookUp Table, check that hstore key exists). TBD.
    --IF NOT EXISTS (SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = configtablename AND columns ?& AKEYS(SetUpColumns)) THEN
      --RAISE NOTICE '++++++++++ Provided HSTORE columns are lacking necessary pieces! +++++++++++';
      --RETURN 1;
    --END IF;
    RETURN 0;
    END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------
DROP FUNCTION IF EXISTS  InsertCrateConfiguration( config_name     TEXT,
  	      	 	 	  		   config_id       INT,
     	      	 	  			   crate_default   HSTORE, 
						   slot_default      HSTORE, 
						   channel_default HSTORE, 
						   crate_mask   BIT(64),
						   slot_mask    BIT(64),
						   channel_mask BIT(64)
						   );

DROP FUNCTION IF EXISTS  InsertSubConfiguration( config_name     TEXT,
  	      	 	 			 config_id       INT,
     	      	 	 			 crate_default   HSTORE, 
						 slot_default      HSTORE, 
						 channel_default HSTORE, 
						 crate_mask   BIT(64),
						 slot_mask    BIT(64),
						 channel_mask BIT(64)
						 );

CREATE OR REPLACE FUNCTION InsertSubConfiguration( config_name     TEXT,
     	      	 	 			   config_id       INT,
     	      	 	 			   crate_default   HSTORE, 
						   slot_default    HSTORE, 
 						   channel_default HSTORE,
						   crate_mask   BIT(64) DEFAULT '111111111111111'::BIT(64),
						   slot_mask    BIT(64) DEFAULT '1111111111111111111111111111111111111111111111111111111111111111',
 						   channel_mask BIT(64) DEFAULT '0'::BIT(64)
						   ) RETURNS INT AS $$
  DECLARE
  query text;
  myrec RECORD;
  BEGIN

  IF NOT EXISTS ( SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = config_name) THEN
    RAISE EXCEPTION '++++++++++ Configuration % does not exist! +++++++++++', config_name;
  END IF;

  query := format('SELECT TRUE FROM %s WHERE ConfigID=%s LIMIT 1',config_name,config_id);
  EXECUTE query INTO myrec;
  IF NOT myrec.bool IS NULL THEN
    RAISE EXCEPTION '++++++++++ Configuration % with ID % is already defined! +++++++++++', config_name, config_id;
  END IF;

--  query := 'INSERT INTO '||config_name||'(ConfigID,Crate,Slot,Channel,Parameters,Mask) VALUES('||config_id||',-999,-1,-1,'''||crate_default||''','''||crate_mask||''')';
  query := 'INSERT INTO '||config_name||'(ConfigID,Crate,Slot,Channel,Parameters,Mask) VALUES('||config_id||',-999,-1,-1,'''||crate_default||''','''||crate_mask||''')';
  execute query;

  query := 'INSERT INTO '||config_name||'(ConfigID,Crate,Slot,Channel,Parameters,Mask) VALUES('||config_id||',-1,-999,-1,'''||slot_default||''','''||slot_mask||''')';
  execute query;

  query := 'INSERT INTO '||config_name||'(ConfigID,Crate,Slot,Channel,Parameters,Mask) VALUES('||config_id||',-1,-1,-999,'''||channel_default||''','''||channel_mask||''')';
  execute query;

  RETURN 0;
  END;
$$ LANGUAGE plpgsql VOLATILE STRICT;


------------------------------------------------------------------------
DROP FUNCTION IF EXISTS  AppendCrateConfiguration( config_name TEXT,
     	      	 	 			   config_id   INT,
						   crate_id    INT,
						   slot_id     INT,
						   channel_id  INT,
						   Mask        BIT(64),
						   params      HSTORE );

DROP FUNCTION IF EXISTS  AppendSubConfiguration( config_name TEXT,
     	      	 	 			 config_id   INT,
						 crate_id    INT,
						 slot_id     INT,
						 channel_id  INT,
						 Mask        BIT(64),
						 params      HSTORE );

DROP FUNCTION IF EXISTS  FillSubConfiguration( config_name TEXT,
       	  	  	   		       config_id   INT,
					       crate_id    INT,
					       slot_id     INT,
					       channel_id  INT,
					       Mask        BIT(64),
					       params      HSTORE );

CREATE OR REPLACE FUNCTION FillSubConfiguration( config_name TEXT,
       	  	  	   			 config_id   INT,
						 crate_id    INT,
						 slot_id     INT,
						 channel_id  INT,
						 Mask        BIT(64),
						 params      HSTORE ) RETURNS INT AS $$
  DECLARE
  query text;
  rec   RECORD;
  BEGIN
  
    SELECT CheckNewSubConfiguration(config_name, config_id, crate_id, slot_id, channel_id, params) AS VAL INTO rec;
    IF rec.VAL = 1 THEN
      RAISE EXCEPTION '+++++++++ Invalid subconfiguration! Aborting +++++++++';
    RETURN 1;
  END IF;

  query := 'INSERT INTO '||config_name||'(ConfigID,Crate,Slot,Channel,Parameters,Mask) VALUES('||config_id||','||crate_id||','''||slot_id||''','''||channel_id||''','''||params||''','''||Mask||''')';
  RAISE INFO '%', query;
  execute query;

  RETURN 0;
  END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION MaxSubConfigID(configtablename TEXT) RETURNS INT AS $$
DECLARE
   myrec RECORD;
   query TEXT;
BEGIN

   -- First find if this configuration type exists. If not, don't do anything
   IF NOT EXISTS ( SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = configtablename )    
     THEN RAISE EXCEPTION '++++++++++ Configuration % is not defined yet! +++++++++++', configtablename;
   END IF;

   query := format('SELECT ConfigID FROM %s ORDER BY ConfigID DESC LIMIT 1;',configtablename);
   EXECUTE query INTO myrec;

   IF myrec.ConfigID IS NULL
       THEN RETURN -1;
   ELSE 
       RETURN myrec.ConfigID;
   END IF;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS MaskToArray( Mask BIT(64) );
CREATE OR REPLACE FUNCTION MaskToArray( Mask BIT(64) ) RETURNS INT[] AS $$
DECLARE
   index      INT;
   enabled_id INT[];
   local_mask BIGINT;
BEGIN
   
   FOR i IN 0..63 LOOP
     IF ((Mask<<i)::BIT(1) & 1::BIT(1)) = '1'::BIT(1) THEN
       enabled_id = enabled_id || (63-i);
     END IF;
   END LOOP;

   RETURN enabled_id;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;


-------------------------------------------------------------------------

DROP FUNCTION IF EXISTS SubConfigType( sub_conf_name TEXT );

CREATE OR REPLACE FUNCTION SubConfigType( sub_conf_name TEXT ) RETURNS INT AS $$
DECLARE
  sub_conf_type INT;
BEGIN
  SELECT SubConfigType FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO sub_conf_type;
  IF sub_conf_type IS NULL THEN
    RAISE EXCEPTION '++++++++++++ Config % not found! +++++++++++++',sub_conf_name;
  END IF;
  RETURN sub_conf_type;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------------------

DROP FUNCTION IF EXISTS SubConfigID( main_conf_name TEXT,
     	      	 		     sub_conf_name TEXT );

CREATE OR REPLACE FUNCTION SubConfigID( main_conf_name TEXT,
       	  	  	   		sub_conf_name TEXT ) RETURNS INT AS $$
DECLARE
  sub_conf_type INT;
  sub_conf_id INT;
BEGIN
  
  SELECT SubConfigType(sub_conf_name) INTO sub_conf_type;

  SELECT SubConfigID FROM MainConfigTable WHERE ConfigName = main_conf_name AND SubConfigType = sub_conf_type INTO sub_conf_id;

  IF sub_conf_id IS NULL THEN
    RAISE EXCEPTION '++++++++++++ Main Config = %, Sub-Config = % not found in MainConfigTable! +++++++++++++++',main_conf_name,sub_conf_name;
  END IF;

  RETURN sub_conf_id;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------------------

DROP FUNCTION IF EXISTS ListEnabledCrates( main_conf_name TEXT,
     	      	 			   sub_conf_name TEXT );

CREATE OR REPLACE FUNCTION ListEnabledCrates( main_conf_name TEXT,
     	      	 			      sub_conf_name TEXT ) RETURNS INT[] AS $$
DECLARE
  rec        RECORD;
  sub_conf_type INT;
  crate_mask BIT(64);
  crate_v    INT[];
BEGIN
  
  SELECT SubConfigType(sub_conf_name) INTO sub_conf_type;

  SELECT SubConfigMask FROM MainConfigTable WHERE ConfigName = main_conf_name AND SubConfigType = sub_conf_type INTO crate_mask;
  IF NOT FOUND THEN
    RAISE EXCEPTION '+++++++++++ (Config,SubConfig) = (%,%) not found in MainConfigTable ++++++++++++',main_conf_name,sub_conf_name;
  END IF;

  SELECT MaskToArray(crate_mask) INTO crate_v;
  RETURN crate_v;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ListEnabledSlots( sub_conf_name TEXT, 
     	      	 			  sub_conf_id   INT,
					  crate_id      INT );

CREATE OR REPLACE FUNCTION ListEnabledSlots( sub_conf_name TEXT, 
     	      	 			     sub_conf_id   INT,
					     crate_id      INT ) RETURNS INT[] AS $$
DECLARE
  exist BOOLEAN;
  query TEXT;
  slot_mask BIT(64);
  slot_v INT[];
BEGIN

  IF crate_id < 0 THEN
    RAISE EXCEPTION '+++++++++++ Query Crate ID (%) must be positive! ++++++++++++',crate_id;
  END IF;

  SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++ No Sub-Config named as % ++++++++++++',sub_conf_name;
  END IF;

  query := format('SELECT TRUE FROM %s WHERE ConfigID = %s LIMIT 1',sub_conf_name,sub_conf_id);
  EXECUTE query INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++ ConfigID % not viable! +++++++++++++',sub_conf_id;
  END IF;

  query := format( 'SELECT Mask FROM %s WHERE ConfigID = %s AND (Crate = %s OR Crate = -999) AND Slot = -1 AND Channel = -1 ORDER BY Crate DESC LIMIT 1;',
  	   	   sub_conf_name,
		   sub_conf_id,
		   crate_id);
  EXECUTE query INTO slot_mask;
  IF slot_mask IS NULL THEN
    RAISE EXCEPTION '+++++++++++ Did not find mask for crate % (LOGIC ERROR ... default must exist!) ++++++++++++',crate_id;
  END IF;

  SELECT MaskToArray(slot_mask) INTO slot_v;
  RETURN slot_v;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;


-------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ListEnabledChannels( sub_conf_name TEXT, 
     	      	 			     sub_conf_id   INT,
					     crate_id      INT,
					     slot_id       INT );

CREATE OR REPLACE FUNCTION ListEnabledChannels( sub_conf_name TEXT, 
     	      	 			     	sub_conf_id   INT,
					     	crate_id      INT,
						slot_id       INT ) RETURNS INT[] AS $$
DECLARE
  exist BOOLEAN;
  query TEXT;
  channel_mask BIT(64);
  channel_v INT[];
BEGIN

  IF crate_id < 0 OR slot_id < 0 THEN
    RAISE EXCEPTION '+++++++++++ Query Crate ID (%) and Slot ID (%) must be positive! ++++++++++++',crate_id,slot_id;
  END IF;

  SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++ No Sub-Config named as % ++++++++++++',sub_conf_name;
  END IF;

  query := format('SELECT TRUE FROM %s WHERE ConfigID = %s LIMIT 1',sub_conf_name,sub_conf_id);
  EXECUTE query INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++ ConfigID % not viable! +++++++++++++',sub_conf_id;
  END IF;

  query := format( 'SELECT Mask FROM %s WHERE ConfigID = %s AND (Slot = -999 OR (Crate = %s AND Slot = %s) ) AND Channel = -1 ORDER BY Slot DESC, Crate DESC LIMIT 1;',
  	   	   sub_conf_name,
		   sub_conf_id,
		   crate_id,
		   slot_id);

  EXECUTE query INTO channel_mask;
  IF channel_mask IS NULL THEN
    RAISE EXCEPTION '+++++++++++ Did not find mask for (Crate,Slot) (%,%) (LOGIC ERROR ... default must exist!) ++++++++++++',crate_id,slot_id;
  END IF;

  SELECT MaskToArray(channel_mask) INTO channel_v;
  RETURN channel_v;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------------------

DROP FUNCTION IF EXISTS LoopEnabledCrateSlotChannel( main_conf_name TEXT,
     	      	 			   	     sub_conf_name TEXT );

CREATE OR REPLACE FUNCTION LoopEnabledCrateSlotChannel( main_conf_name TEXT,
     	      	 	      		 		sub_conf_name TEXT ) RETURNS SETOF INT[] AS $$
DECLARE
  sub_conf_id INT;
  crate_v   INT[];
  slot_v    INT[];
  channel_v INT[];
  crate_id   INT;
  slot_id    INT;
  channel_id INT;

BEGIN

  SELECT ListEnabledCrates(main_conf_name,sub_conf_name) INTO crate_v;
  IF crate_v IS NULL THEN
    RAISE EXCEPTION '++++++++++++ Failed retrieving crate list! ++++++++++++++';
  END IF;

  SELECT SubConfigID(main_conf_name, sub_conf_name) INTO sub_conf_id;

  FOREACH crate_id IN ARRAY crate_v LOOP
    SELECT ListEnabledSlots(sub_conf_name,sub_conf_id,crate_id) INTO slot_v;
    FOREACH slot_id IN ARRAY slot_v LOOP
      SELECT ListEnabledChannels(sub_conf_name,sub_conf_id,crate_id,slot_id) INTO channel_v;
      FOREACH channel_id IN ARRAY channel_v LOOP
        RETURN NEXT ARRAY[crate_id,slot_id,channel_id];
      END LOOP;
    END LOOP;
  END LOOP;
  
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS PrintEnabledCrateSlotChannel( main_conf_name TEXT,
     	      	 	 		   	      sub_conf_name TEXT );

CREATE OR REPLACE FUNCTION PrintEnabledCrateSlotChannel( main_conf_name TEXT,
     	      	 	      		 		 sub_conf_name TEXT ) RETURNS INT AS $$
DECLARE
  enabled_combination INT[];
BEGIN

  FOR enabled_combination IN SELECT LoopEnabledCrateSlotChannel(main_conf_name,sub_conf_name) LOOP
    RAISe INFO 'Enabled (Crate,Slot,Channel) = %',enabled_combination;
  END LOOP;
  RETURN 1;
  
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS GetCrateConfig( sub_conf_name TEXT,
     	      	 			sub_conf_id   INT,
					crate_id      INT );

CREATE OR REPLACE FUNCTION GetCrateConfig( sub_conf_name TEXT,
     	      	 			   sub_conf_id   INT,
					   crate_id      INT ) RETURNS UnitConfig AS $$
DECLARE
  query  TEXT;
  exist  BOOLEAN;
  conf_params UnitConfig;
  rec    RECORD;
BEGIN

  conf_params.Crate   := -1;
  conf_params.Slot    := -1;
  conf_params.Channel := -1;
  conf_params.Params  := '';

  SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not exist! ++++++++++++++',sub_conf_name;
  END IF;
  
  query := format('SELECT TRUE FROM %s WHERE ConfigID = %s',sub_conf_name,sub_conf_id);
  EXECUTE query INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not have config ID %! ++++++++++++++',sub_conf_name,sub_conf_id;
  END IF;

  conf_params.Crate := crate_id;

  query := format( 'SELECT Parameters FROM %s WHERE ConfigID = %s AND (Crate = %s OR Crate = -999) AND SLOT = -1 AND Channel = -1 ORDER BY Crate ASC;',
  	   	   sub_conf_name,
		   sub_conf_id,
		   crate_id);
  FOR rec IN EXECUTE query LOOP
    conf_params.Params := conf_params.Params || rec.Parameters;
  END LOOP;

  --RAISE INFO '%',conf_params;
  RETURN conf_params;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS GetSlotConfig( sub_conf_name TEXT,
     	      	 		       sub_conf_id   INT,
				       crate_id      INT,
				       slot_id       INT );

CREATE OR REPLACE FUNCTION GetSlotConfig( sub_conf_name TEXT,
     	      	 			  sub_conf_id   INT,
					  crate_id      INT,
					  slot_id       INT ) RETURNS UnitConfig AS $$
DECLARE
  query  TEXT;
  exist  BOOLEAN;
  conf_params UnitConfig;
  rec    RECORD;
  slot_v INT[];
BEGIN

  conf_params.Crate   := -1;
  conf_params.Slot    := -1;
  conf_params.Channel := -1;
  conf_params.Params  := '';

  SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not exist! ++++++++++++++',sub_conf_name;
  END IF;
  
  query := format('SELECT TRUE FROM %s WHERE ConfigID = %s',sub_conf_name,sub_conf_id);
  EXECUTE query INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not have config ID %! ++++++++++++++',sub_conf_name,sub_conf_id;
  END IF;

  SELECT ListEnabledSlots(sub_conf_name, sub_conf_id, crate_id) INTO slot_v;
  IF NOT ARRAY[slot_id] <@ slot_v THEN
    RAISE NOTICE '++++++++++++ (Crate,Slot) = (%,%) is not enabled! ++++++++++++',crate_id,slot_id;
    RETURN conf_params;
  END IF;

  conf_params.Crate   := crate_id;
  conf_params.Slot    := slot_id;

  query := format( 'SELECT Parameters FROM %s WHERE ConfigID = %s AND (Slot=-999 OR Slot=%s) AND (Crate=%s OR Crate=-1) AND Channel = -1 ORDER BY Slot ASC, Crate ASC;',
  	   	   sub_conf_name,
		   sub_conf_id,
		   slot_id,
		   crate_id);

  FOR rec IN EXECUTE query LOOP
    conf_params.Params := conf_params.Params || rec.Parameters;
  END LOOP;

  --RAISE INFO '%',conf_params;
  RETURN conf_params;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS GetChannelConfig( sub_conf_name TEXT,
     	      	 		       	  sub_conf_id   INT,
				       	  crate_id      INT,
				       	  slot_id       INT,
					  channel_id    INT );

CREATE OR REPLACE FUNCTION GetChannelConfig( sub_conf_name TEXT,
     	      	 			     sub_conf_id   INT,
					     crate_id      INT,
					     slot_id       INT,
					     channel_id    INT ) RETURNS UnitConfig AS $$
DECLARE
  query  TEXT;
  exist  BOOLEAN;
  conf_params UnitConfig;
  rec    RECORD;
  channel_v INT[];
BEGIN

  conf_params.Crate   := -1;
  conf_params.Slot    := -1;
  conf_params.Channel := -1;
  conf_params.Params  := '';

  SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not exist! ++++++++++++++',sub_conf_name;
  END IF;
  
  query := format('SELECT TRUE FROM %s WHERE ConfigID = %s',sub_conf_name,sub_conf_id);
  EXECUTE query INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not have config ID %! ++++++++++++++',sub_conf_name,sub_conf_id;
  END IF;

  SELECT ListEnabledChannels(sub_conf_name, sub_conf_id, crate_id, slot_id) INTO channel_v;
  IF NOT ARRAY[channel_id] <@ channel_v THEN
    RAISE NOTICE '++++++++++++ (Crate,Slot,Channel) = (%,%,%) is not enabled! ++++++++++++',crate_id,slot_id,channel_id;
    RETURN conf_params;
  END IF;

  conf_params.Crate   := crate_id;
  conf_params.Slot    := slot_id;
  conf_params.Channel := channel_id;
  conf_params.Params  := '';  

  query := format( 'SELECT Parameters FROM %s WHERE ConfigID = %s AND (Crate=%s OR Crate=-1) AND (Slot=%s OR Slot=-1) AND (Channel=%s OR Channel=-999) ORDER BY Channel ASC, Slot ASC, Crate ASC;',
  	   	   sub_conf_name,
		   sub_conf_id,
		   crate_id,
		   slot_id,
		   channel_id);
  FOR rec IN EXECUTE query LOOP
    conf_params.Params := conf_params.Params || rec.Parameters;
  END LOOP;

  --RAISE INFO '%',conf_params;
  RETURN conf_params;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;


------------------------------------------------------------------------

DROP FUNCTION IF EXISTS GetSlotChannelConfig( sub_conf_name TEXT,
     	      	 		       	      sub_conf_id   INT,
				       	      crate_id      INT,
				       	      slot_id       INT);

CREATE OR REPLACE FUNCTION GetSlotChannelConfig( sub_conf_name TEXT,
     	      	 			     	 sub_conf_id   INT,
					     	 crate_id      INT,
					     	 slot_id       INT) RETURNS SETOF UnitConfig AS $$
DECLARE
  query  TEXT;
  exist  BOOLEAN;
  conf_params UnitConfig;
  params HSTORE;
  rec    RECORD;
  rec_set RECORD;
  slot_v    INT[];
  channel_v INT[];
  channel_id INT;
BEGIN

  conf_params.Crate   := -1;
  conf_params.Slot    := -1;
  conf_params.Channel := -1;
  conf_params.Params  := '';

  SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not exist! ++++++++++++++',sub_conf_name;
  END IF;

  query := format('SELECT TRUE FROM %s WHERE ConfigID = %s',sub_conf_name,sub_conf_id);
  EXECUTE query INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not have config ID %! ++++++++++++++',sub_conf_name,sub_conf_id;
  END IF;

  SELECT ListEnabledSlots(sub_conf_name, sub_conf_id, crate_id) INTO slot_v;
  IF NOT ARRAY[slot_id] <@ slot_v THEN
    RAISE NOTICE '++++++++++++ (Crate,Slot) = (%,%) is not enabled! ++++++++++++',crate_id,slot_id;
    RETURN NEXT conf_params;
  ELSE

    conf_params.Crate   := crate_id;
    conf_params.Slot    := slot_id;
    conf_params.Channel := -1;
    conf_params.Params  := '';

    query := format( 'SELECT Parameters FROM %s WHERE ConfigID = %s AND (Slot=-999 OR Slot=%s) AND (Crate = %s OR Crate=-1) AND Channel = -1 ORDER BY Slot ASC, Crate ASC;',
  	   	     sub_conf_name,
		     sub_conf_id,
		     slot_id,
		     crate_id);

    FOR rec IN EXECUTE query LOOP
      conf_params.Params := conf_params.Params || rec.Parameters;
    END LOOP;

    RETURN NEXT conf_params;

    SELECT ListEnabledChannels(sub_conf_name, sub_conf_id, crate_id, slot_id) INTO channel_v;  
    IF channel_v IS NOT NULL THEN
      FOREACH channel_id IN ARRAY channel_v LOOP

        SELECT GetChannelConfig(sub_conf_name, sub_conf_id, crate_id, slot_id, channel_id) INTO rec;
      
        conf_params := rec.getchannelconfig;
      
        RETURN NEXT conf_params;

      END LOOP;
    END IF;
  END IF;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS GetCrateSlotChannelConfig( sub_conf_name TEXT,
     	      	 		       	           sub_conf_id   INT,
				       	      	   crate_id      INT);

CREATE OR REPLACE FUNCTION GetCrateSlotChannelConfig( sub_conf_name TEXT,
     	      	 			     	      sub_conf_id   INT,
					     	      crate_id      INT) RETURNS SETOF UnitConfig AS $$
DECLARE
  query  TEXT;
  exist  BOOLEAN;
  conf_params UnitConfig;
  rec    RECORD;
  rec_set RECORD;
  slot_v    INT[];
  slot_id   INT;
BEGIN

  conf_params.Crate   := -1;
  conf_params.Slot    := -1;
  conf_params.Channel := -1;
  conf_params.Params  := '';

  SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = sub_conf_name INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not exist! ++++++++++++++',sub_conf_name;
  END IF;
  
  query := format('SELECT TRUE FROM %s WHERE ConfigID = %s',sub_conf_name,sub_conf_id);
  EXECUTE query INTO exist;
  IF exist IS NULL THEN
    RAISE EXCEPTION '+++++++++++++ Sub-Config % does not have config ID %! ++++++++++++++',sub_conf_name,sub_conf_id;
  END IF;

  --SELECT GetCrateConfig(sub_conf_name, sub_conf_id, crate_id) INTO conf_params;
  SELECT GetCrateConfig(sub_conf_name, sub_conf_id, crate_id) INTO rec;
  conf_params := rec.getcrateconfig;
  RETURN NEXT conf_params;

  SELECT ListEnabledSlots(sub_conf_name, sub_conf_id, crate_id) INTO slot_v;
  IF slot_v IS NOT NULL THEN
    FOREACH slot_id IN ARRAY slot_v LOOP
      FOR rec IN SELECT GetSlotChannelConfig(sub_conf_name, sub_conf_id, crate_id, slot_id) LOOP
        conf_params := rec.GetSlotChannelConfig;
        RETURN NEXT conf_params;
      END LOOP;
    END LOOP;
  END IF;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

DROP FUNCTION IF EXISTS GetCrateSlotChannelConfig( main_conf_name TEXT,
     	      	 		       	           sub_conf_name  TEXT );

CREATE OR REPLACE FUNCTION GetCrateSlotChannelConfig( main_conf_name TEXT,
     	      	 			     	      sub_conf_name  TEXT ) RETURNS SETOF UnitConfig AS $$
DECLARE
  query  TEXT;
  sub_conf_id INT;
  crate_v INT[];
  crate_id INT;
  conf_params UnitConfig;
  rec    RECORD;
BEGIN

  SELECT SubConfigID( main_conf_name,sub_conf_name) INTO sub_conf_id;
  SELECT ListEnabledCrates( main_conf_name, sub_conf_name ) INTO crate_v;
  FOREACH crate_id IN ARRAY crate_v LOOP
    FOR rec in SELECT GetCrateSlotChannelConfig(sub_conf_name, sub_conf_id, crate_id) LOOP
      conf_params := rec.GetCrateSlotChannelConfig;
      RETURN NEXT conf_params;
    END LOOP;
  END LOOP;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION InsertMainConfiguration( subconfigparameters HSTORE,
       	  	  	   			    subconfigmasks HSTORE,
       	  	  	   			    confname text DEFAULT 'no_name') RETURNS INT AS $$
    DECLARE
    myrec1 RECORD;
    myrec2 RECORD;
    myrec3 RECORD;
    ColumnPair RECORD;
    SubConfigCount INT;
    -- maskconfig INT;
    newconfig INT;
    query text;  
    SubConfT INT;
    --mainconfigexists INT;
    localconfigexists INT;
    BEGIN    

    -- 1st CHECK: check if the provided Config ID already exsits in the MainConfigTable or not
    IF  (confname <> 'no_name' ) AND EXISTS (SELECT ConfigID FROM MainConfigTable WHERE MainConfigTable.ConfigName = confname)
        THEN RAISE EXCEPTION '+++++++++++++ Config with name % already exists in MainConfigTable! +++++++++++++',confname;
	RETURN -1;
    END IF;
    -- now we know that the name doesn't exist or the user doesn't care.

    -- 2nd CHECK: check if specified SubConfigType and SubConfigID are available or not
    FOR ColumnPair IN SELECT (each(subconfigparameters)).*
    LOOP
        -- Check if SubConfigType is valid or not
        --        RAISE NOTICE 'key:%, value: %', ColumnPair.key, ColumnPair.value;
	IF NOT EXISTS ( SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = ColumnPair.key)
	    THEN RAISE EXCEPTION '++++++++++++ Configuration % is not defined! +++++++++++++++', ColumnPair.key;
	    RETURN -1;
	END IF;
	-- Check if SubConfigID is valid or not
	query := format('SELECT TRUE FROM %s WHERE ConfigID=%s',ColumnPair.key,ColumnPair.value);
	RAISE NOTICE '%', query;
	EXECUTE query INTO myrec2;
	IF myrec2.bool IS NULL
	    THEN RAISE EXCEPTION '++++++++++++ SubConfig % does not contain ConfigID=%++++++++++++',ColumnPair.key,ColumnPair.value;
	    RETURN -1;
	END IF;
	
    END LOOP;
    -- 3rd CHECK: subconfigmasks HSTORE keys has to be a subset of subconfigparameters HSTORE, and values must be BIGINT compatible type
    IF NOT AKEYS(subconfigparameters) @> AKEYS(subconfigmasks) THEN
      RAISE EXCEPTION '++++++++++++++ Mask contains configuration that is not in the list of subconfigurations! +++++++++++++';
    END IF;

    FOR ColumnPair IN SELECT (each(subconfigmasks)).*
    LOOP
	SELECT textregexeq(ColumnPair.value,'^[0-1]+(\.[0-1]+)?$') AS VAL INTO myrec2;
	IF NOT myrec2.VAL THEN
	  RAISE EXCEPTION '++++++++++++ Config % Mask % is not a valid binary expression +++++++++++++',ColumnPair.key,ColumnPair.value;
	END IF;
    END LOOP;

    -- mainconfigexists:=1;  -- assuming it exists. Let's be proven otherwise.
    --let's check if the configuration exists:
    -- since we want to check the configuration sets, it's ok if we just choose the first subconfigtype and ID.
    -- and then we loop over all of the subsets of MainconfigTable to see if they contain all of the other ones.

    -- !!! Still missing. Find that the configuration we havefound does not have more rows than we need. !!!


    SELECT (each(subconfigparameters)).* LIMIT 1 INTO ColumnPair;  -- only use the first.
    --get subconfig type
    ------------------------------ this repeats the functionality from later.

    SELECT  SubconfigType FROM ConfigLookUp WHERE SubConfigName = ColumnPair.key INTO SubConfT;
    --looping over all mainconfigs that have the above parameters. 
    for myrec3 IN SELECT DISTINCT configID FROM MainconfigTable WHERE SubConfigType = SubConfT AND SubConfigID = CAST(ColumnPair.value AS INT)
      LOOP

         -- Report
--         RAISE NOTICE 'Checking against Main Config ID %', myrec3;

	 -- First check the # of sub-config types in this configuration. If not same as the # elements in subconfigparameters, continue.
	 SELECT COUNT(TRUE) FROM MainConfigTable WHERE ConfigID = myrec3.ConfigID INTO SubConfigCount;
--	 SubConfigCount := SubConfigCount - ARRAY_LENGTH(AKEYS(subconfigparameters));
--	 SELECT COUNT(TRUE) - SubConfigCount FROM 
--	 IF SubConfigCount = 0
	 IF NOT SubConfigCount = ARRAY_LENGTH(AKEYS(subconfigparameters),1)
	    THEN CONTINUE;
	 END IF;

         -- loop over all of the new, to be insterted config setting and check if the current configID has them all.
         localconfigexists := 1;
	 for ColumnPair IN SELECT (each(subconfigparameters)).*
	     LOOP
	     SELECT SubConfigType FROM ConfigLookUp WHERE SubConfigName = ColumnPair.key INTO SubConfT;
             IF NOT EXISTS( SELECT TRUE FROM MainconfigTable WHERE SubConfigType = SubConfT AND subconfigid=CAST(ColumnPair.value AS INT) AND ConfigID=myrec3.ConfigID )
		  THEN localconfigexists := 0;
		  EXIT;   -- break out of loop. We already know, this is not the right config.
	     END IF;
             END LOOP;
         IF localconfigexists = 1    -- we've gone through all of the PMT settings and all exist.
	    THEN RAISE EXCEPTION '+++++++++++++ This Configuration exists MainConfigTable, with ID: %! +++++++++++++', myrec3.ConfigID;
	    RETURN -1;
	 END IF;
     END LOOP;  -- end of myrec3 FOR loop over all of the subconfigs


--     IF mainconfigexists = 1
--       THEN RAISE EXCEPTION '+++++++++++++ Config already exists in MainConfigTable, with ID: %! +++++++++++++',-1;
--     END IF;
   ----------- To be improved
   --------------------------------- Find the last entry
    SELECT configID FROM MainConfigTable ORDER BY configID DESC LIMIT 1 INTO newconfig ;
    IF newconfig IS NULL
      THEN  newconfig := 0;
    ELSE
       newconfig := newconfig + 1;
    END IF;
    
    RAISE NOTICE 'Inserting a new entry in MainConfigTable: Name=% ... ID=%',confname,newconfig;
   
    -- Reaching this point means input values are valid. Let's insert.
   FOR myrec1 IN SELECT (each(subconfigparameters)).*
    LOOP
    SELECT SubconfigType FROM ConfigLookUp WHERE SubConfigName = myrec1.key INTO SubConfT;
    SELECT subconfigmasks->myrec1.key AS VAL INTO myrec2;
    IF myrec2.VAL IS NULL THEN
      INSERT INTO MainconfigTable ( ConfigID,
    	   			    SubConfigType,
				    SubConfigID, 
				    SubConfigMask,
				    ConfigName) VALUES
				    ( newconfig, SubConfT, CAST(myrec1.value AS INT), 
				      '111111111111'::BIT(64),
				      confname);
    ELSE
      INSERT INTO MainconfigTable ( ConfigID,
    	   			    SubConfigType,
				    SubConfigID, 
				    SubConfigMask,
				    ConfigName) VALUES
				    ( newconfig, SubConfT, CAST(myrec1.value AS INT), 
				      CAST(myrec2.VAL AS BIT(64)),
				      confname);
    END IF;

    END LOOP;

   RETURN newconfig;
   END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------

DROP FUNCTION IF EXISTS CleanSubConfigurationID( cfg_name TEXT,
       	  	  	   		         cfg_id   INT);

DROP FUNCTION IF EXISTS CleanSubConfig( cfg_name TEXT,
       	  	  	   	        cfg_id   INT);

CREATE OR REPLACE FUNCTION CleanSubConfig( cfg_name TEXT,
       	  	  	   		   cfg_id   INT) RETURNS INT AS $$
DECLARE
query TEXT;
sub_conf_type INT;
sub_conf_id INT;
sub_conf_ctr INT;
BEGIN

  sub_conf_type := -1;  
  SELECT INTO sub_conf_type SubConfigType(cfg_name);
  IF sub_conf_type IS NULL THEN 
  RETURN 0;
  END IF;

  sub_conf_id :=-1;

  SELECT SubConfigID FROM MainConfigTable WHERE SubConfigType = sub_conf_type AND SubConfigID = cfg_id INTO sub_conf_id;

  IF sub_conf_id IS NOT NULL AND sub_conf_id >= 0 THEN
    RAISE WARNING '++++++++++++ Sub-Config %s w/ ID % currently in use within MainConfigTable! ++++++++++++',cfg_name,cfg_id;
    RETURN 0;
  ELSE
    query := 'DELETE FROM ' || cfg_name || ' WHERE ConfigID = ' || cfg_id || ';';
    EXECUTE query;
    RETURN 1;    
  END IF;    
  RETURN 0;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------

DROP FUNCTION IF EXISTS CleanSubConfig( cfg_name TEXT);

CREATE OR REPLACE FUNCTION CleanSubConfig( cfg_name TEXT ) RETURNS INT AS $$
DECLARE
query TEXT;
sub_conf_type INT;
sub_conf_id INT;
sub_conf_ctr INT;
BEGIN

  sub_conf_type := -1;  
  SELECT INTO sub_conf_type SubConfigType(cfg_name);
  IF sub_conf_type IS NULL THEN 
  RETURN 0;
  END IF;

  sub_conf_id :=-1;
  SELECT SubConfigID FROM MainConfigTable WHERE SubConfigType = sub_conf_type INTO sub_conf_id;

  IF sub_conf_id IS NOT NULL AND sub_conf_id >= 0 THEN
    RAISE WARNING '++++++++++++ Sub-Config % currently in use within MainConfigTable! ++++++++++++',cfg_name;
    RETURN 0;
  ELSE
    query := 'DROP TABLE ' || cfg_name || ';';
    EXECUTE query;
    DELETE FROM ConfigLookUp WHERE SubConfigName = cfg_name;
    RETURN 1;    
  END IF;    
  RETURN 0;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------
DROP FUNCTION IF EXISTS CleanMainConfig( cfg_name TEXT );
CREATE OR REPLACE FUNCTION CleanMainConfig( cfg_name TEXT) RETURNS INT AS $$
DECLARE
ret_val INT;
conf_id INT;
run_ctr INT;
BEGIN
  ret_val := 0;
  SELECT ConfigID FROM MainConfigTable WHERE ConfigName = cfg_name INTO conf_id;
  IF conf_id IS NULL THEN
    RAISE WARNING '++++++++++ Main-COnfig % does not seem to exist... ++++++++++',cfg_name;
  ELSE
    SELECT COUNT(RunNumber) FROM MainRun WHERE ConfigID = conf_id INTO run_ctr;
    IF run_ctr IS NOT NULL AND run_ctr>0 THEN
      RAISE WARNING '++++++++++ Cannot remove Main-Config % (used in MainRun) +++++++++++',cfg_name;
    ELSE
      DELETE FROM MainConfigTable WHERE ConfigID = conf_id;
      ret_val :=1;
    END IF;
  END IF;
  RETURN ret_val;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------

CREATE OR REPLACE FUNCTION CleanConfigDB() RETURNS INT AS $$
DECLARE
  ConfigTable RECORD;
  query TEXT;
BEGIN
    DELETE FROM MainConfigTable;
    FOR ConfigTable IN SELECT DISTINCT SubConfigName FROM ConfigLookUp
    LOOP
        query := 'DROP TABLE IF EXISTS '||ConfigTable.SubConfigName||';';
	EXECUTE query;
    END LOOP;
    DELETE FROM ConfigLookUp;
    RETURN 0;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;


DROP FUNCTION IF EXISTS GetSubConfig( text, INT) ;

------------------create TYPES for each table?


