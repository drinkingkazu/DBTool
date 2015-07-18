--SET ROLE uboonedaq_admin;

------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS ExistMainConfig(MCfgName TEXT);
CREATE OR REPLACE FUNCTION ExistMainConfig(MCfgName TEXT) RETURNS BOOLEAN AS $$
DECLARE
  exist BOOLEAN;
BEGIN
  exist := FALSE;
  SELECT TRUE FROM MainConfigTable WHERE ConfigName = MCfgName LIMIT 1 INTO exist;
  RETURN exist;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS ExistMainConfig(MCfgID INT);
CREATE OR REPLACE FUNCTION ExistMainConfig(MCfgID INT) RETURNS BOOLEAN AS $$
DECLARE
  exist BOOLEAN;
BEGIN
  exist := FALSE;
  SELECT TRUE FROM MainConfigTable WHERE ConfigID = MCfgID LIMIT 1 INTO exist;
  RETURN exist;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTIONS IF EXISTS MainConfigName(MCfgID INT);
CREATE OR REPLACE FUNCTION MainConfigName(MCfgID INT) RETURNS TEXT AS $$
DECLARE
  res TEXT;
BEGIN
  IF NOT ExistMainConfig(MCfgID) THEN
    RAISE EXCEPTION '++++++++++++ No MainConfig w/ ID % ++++++++++++',MCfgID;
  END IF;
  SELECT INTO res ConfigName FROM MainConfigTable WHERE ConfigID = MCfgID LIMIT 1;
  RETURN res;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTIONS IF EXISTS MainConfigID(MCfgName TEXT)/
CREATE OR REPLACE FUNCTION MainConfigID(MCfgName TEXT) RETURNS TEXT AS $$
DECLARE
  res INT;
BEGIN
  IF NOT ExistMainConfig(MCfgName) THEN
    RAISE EXCEPTION '++++++++++++ No MainConfig w/ Name % ++++++++++++',MCfgName;
  END IF;
  SELECT INTO res ConfigID FROM MainConfigTable WHERE ConfigName = MCfgName LIMIT 1;
  RETURN res;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS ExistSubConfig(SCfgName TEXT);
CREATE OR REPLACE FUNCTION ExistSubConfig(SCfgName TEXT) RETURNS BOOLEAN AS $$
DECLARE
  exist BOOLEAN;
BEGIN
   SELECT TRUE FROM ConfigLookUp WHERE SubConfigName = SCfgName LIMIT 1 INTO exist;
   IF exist IS NULL THEN RETURN FALSE;
   END IF;
   RETURN TRUE;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS ExistSubConfig(SCfgType INT);
CREATE OR REPLACE FUNCTION ExistSubConfig(SCfgType INT) RETURNS BOOLEAN AS $$
DECLARE
  exist BOOLEAN;
BEGIN
   SELECT TRUE FROM ConfigLookUp WHERE SubConfigType = SCfgType LIMIT 1 INTO exist;
   IF exist IS NULL THEN RETURN FALSE;
   END IF;
   RETURN TRUE;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS ExistSubConfig(SCfgName TEXT, SCfgID INT);
CREATE OR REPLACE FUNCTION ExistSubConfig(SCfgName TEXT, SCfgID INT) RETURNS BOOLEAN AS $$
DECLARE
  exist BOOLEAN;
  query TEXT;
BEGIN
   IF NOT ExistSubConfig(SCfgName) THEN RETURN FALSE;
   ELSE
     query := 'SELECT TRUE AS ConfigExist FROM ' || SCfgName || ' WHERE ConfigID = ' || SCfgID || ';';
     EXECUTE query INTO exist;
     IF exist IS NULL OR NOT exist THEN
       RETURN FALSE;
     END IF;
   END IF;
   RETURN TRUE;
END;
$$ LANGUAGE plpgsql;

DROP FUNCTIONS IF EXISTS SubConfigName(SCfgType INT);
CREATE OR REPLACE FUNCTION SubConfigName(SCfgType INT) RETURNS TEXT AS $$
DECLARE
  res TEXT;
BEGIN
  IF NOT ExistSubConfig(SCfgType) THEN
    RAISE EXCEPTION '++++++++++++ No SubConfig w/ Type % ++++++++++++',SCfgType;
  END IF;

  SELECT SubConfigName FROM ConfigLookUp WHERE SubConfigType = SCfgType LIMIT 1 INTO res;
  RETURN res;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTIONS IF EXISTS SubConfigType(SCfgName TEXT);
CREATE OR REPLACE FUNCTION SubConfigType(SCfgName TEXT) RETURNS INT AS $$
DECLARE
  res INT;
BEGIN
  IF NOT ExistSubConfig(SCfgName) THEN
    RAISE EXCEPTION '++++++++++++ No SubConfig w/ Name % ++++++++++++',SCfgName;
  END IF;

  SELECT SubConfigType FROM ConfigLookUp WHERE SubConfigName = SCfgName LIMIT 1 INTO res;
  RETURN res;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTIONS IF EXISTS MainConfigName(MCfgName TEXT)/
CREATE OR REPLACE FUNCTION MainConfigName(MCfgName TEXT) RETURNS TEXT AS $$
DECLARE
  res INT;
BEGIN
  IF NOT ExistMainConfig(MCfgName) THEN
    RAISE EXCEPTION '++++++++++++ No MainConfig w/ Name % ++++++++++++',MCfgName;
  END IF;
  SELECT INTO res ConfigID FROM MainConfigTable WHERE ConfigName = MCfgName LIMIT 1;
  RETURN res;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS ListSubConfigs(CfgID INT);
CREATE OR REPLACE FUNCTION ListSubConfigs(CfgID INT) RETURNS TABLE (SubConfigName TEXT, SubConfigID INT) AS $$
DECLARE
  tmp_type_and_id RECORD;
  tmp_name_and_id RECORD;
BEGIN
  FOR tmp_type_and_id IN SELECT MainConfigTable.SubConfigType, MainConfigTable.SubConfigID
         		 FROM MainConfigTable
			 WHERE ConfigID = CfgID
			 ORDER BY SubConfigType
  LOOP
    SELECT INTO tmp_name_and_id ConfigLookUp.SubConfigName FROM  ConfigLookUp
    	   						   WHERE ConfigLookUp.SubConfigType = tmp_type_and_id.SubConfigType
							   LIMIT 1;

    RETURN QUERY SELECT tmp_name_and_id.SubConfigName::TEXT, tmp_type_and_id.SubConfigID;

  END LOOP;
   RETURN;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION ListParameters(SCfgName TEXT, SCfgID INT)
       	  	  RETURNS TABLE (ParamName TEXT, ParamValue TEXT) AS $$
DECLARE
  myquery TEXT;
  params HSTORE;
  pair   RECORD;
BEGIN
  IF NOT ExistSubConfig(SCfgName, SCfgID) THEN
    RAISE EXCEPTION '+++++++++ SubConfig % with ID % does not exist!  +++++++++', SCfgName, SCfgID;
  END IF;

  myquery := 'SELECT Parameters FROM ' || SCfgName || ' WHERE ConfigID = ' || SCfgID || ';';
  EXECUTE myquery INTO params;

  FOR pair IN SELECT (each(params)).*
  LOOP
    RETURN QUERY SELECT pair.key::TEXT, pair.value::TEXT;
  END LOOP;
  RETURN;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ListParameterSets(SCfgName TEXT, SCfgID INT)
       	  	  RETURNS TABLE (SubConfigName TEXT, SubConfigID INT) AS $$
DECLARE
  myquery  TEXT;
  psets  HSTORE;
  pair   RECORD;
BEGIN
  IF NOT ExistSubConfig(SCfgName, SCfgID) THEN
    RAISE EXCEPTION '+++++++++ SubConfig % with ID % does not exist!  +++++++++', SCfgName, SCfgID;
  END IF;

  myquery := 'SELECT ParameterSets FROM ' || SCfgName || ' WHERE ConfigID = ' || SCfgID || ';';
  EXECUTE myquery INTO psets;

  FOR pair IN SELECT (each(psets)).*
  LOOP
    RETURN QUERY SELECT pair.key::TEXT, pair.value::INT;
  END LOOP;
  RETURN;
END;
$$ LANGUAGE PLPGSQL;


CREATE OR REPLACE FUNCTION ListSubConfigs() RETURNS TABLE (SubConfigName TEXT, SubConfigID INT) AS $$
DECLARE
  rec RECORD;
BEGIN
  FOR rec IN SELECT SubConfigName, SubConfigType FROM ConfigLookUp ORDER BY SubConfigType
  LOOP
    RETURN QUERY SELECT SubConfigName::TEXT, SubConfigType::INT;
  END LOOP;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ListAllMainConfigs()
       	  	  RETURNS TABLE (ConfigName TEXT, ConfigID INT, Enabled BOOLEAN) AS $$
DECLARE
  rec RECORD;
BEGIN

  FOR rec IN SELECT DISTINCT MainConfigTable.ConfigName,
	       	      	     MainConfigTable.ConfigID,
		       	     MainConfigTable.Enabled
	     FROM MainConfigTable
  LOOP
      RETURN QUERY SELECT rec.ConfigName::TEXT, rec.ConfigID::INT, rec.Enabled::BOOLEAN;
  END LOOP;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ListMainConfigs() RETURNS TABLE (ConfigName TEXT, ConfigID INT) AS $$
DECLARE
  rec RECORD;
BEGIN
  FOR rec IN SELECT DISTINCT MainConfigTable.ConfigName, MainConfigTable.ConfigID FROM MainConfigTable
  LOOP
    RETURN QUERY SELECT rec.ConfigName::TEXT, rec.ConfigID::INT;
  END LOOP;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION DisableMainConfig(MCfg TEXT) RETURNS INT AS $$
DECLARE
  rec RECORD;
BEGIN

  IF NOT ExistMainConfig(MCfg) THEN
    RAISE EXCEPTION '+++++++++++ No MainConfig w/ name % +++++++++++', MCfg;
  END IF;

  UPDATE MainConfigTable SET Enabled = FALSE WHERE ConfigName = MCfg;
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION EnableMainConfig(MCfg TEXT) RETURNS INT AS $$
DECLARE
  rec RECORD;
BEGIN

  IF NOT ExistMainConfig(MCfg) THEN
    RAISE EXCEPTION '+++++++++++ No MainConfig w/ name % +++++++++++', MCfg;
  END IF;

  UPDATE MainConfigTable SET Enabled = TRUE WHERE ConfigName = MCfg;
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION DisableMainConfig(MCfgID INT) RETURNS INT AS $$
DECLARE
  MCfgName TEXT;
BEGIN
  SELECT INTO MCfgName MainConfigName(MCfgID);
  SELECT DisableMainConfig(MCfgName);
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION EnableMainConfig(MCfgID INT) RETURNS INT AS $$
DECLARE
  MCfgName TEXT;
BEGIN
  SELECT INTO MCfgName MainConfigName(MCfgID);
  SELECT EnableMainConfig(MCfgName);
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ListDependencies(SCfgName TEXT, SCfgID INT) RETURNS TABLE(SubConfigName TEXT,SubConfigID INT) AS $$
DECLARE
  query TEXT;
  pair  RECORD;
  psets RECORD;
  
  rec   RECORD;  
--  name_id  SubConfigNameAndID;
--  dep_cfg_id SubConfigNameAndID;
BEGIN
  --IF NOT ExistSubConfig(SCfgName,SCfgID) THEN
  --  RAISE EXCEPTION '+++++++++ SubConfig % with ID % does not exist!  +++++++++', SCfgName, SCfgID;
  --END IF;
  --query := 'SELECT ParameterSets FROM ' || SCfgName || ' WHERE ConfigID = ' || SCfgID || ';';
  --EXECUTE query INTO psets;

  --FOR pair IN SELECT (each(psets)).*
  --LOOP
  --  FOR rec IN SELECT * FROM ListDependencies(pair.key::TEXT, pair.value::INT)
  --  LOOP
  --    RETURN QUERY SELECT rec.SubConfigName::TEXT, rec.SubConfigID::INT;
  --  END LOOP;
  --  RETURN QUERY SELECT pair.key::TEXT, pair.value::INT;
  --END LOOP;
  --RETURN;

  FOR psets IN SELECT * FROM ListParameterSets(SCfgName,SCfgID)
  LOOP
    FOR rec IN SELECT * FROM ListDependencies(psets.SubConfigName,psets.SubConfigID)
    LOOP
      RETURN QUERY SELECT rec.SubConfigName::TEXT, rec.SubConfigID::INT;
    END LOOP;
    RETURN QUERY SELECT psets.SubConfigName::TEXT,psets.SubConfigID::INT;
  END LOOP;

END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ListDependencies(sub_config HSTORE) RETURNS TABLE(SubConfigName TEXT, SubConfigID INT) AS $$
DECLARE
  myrec1 RECORD;
  myrec2 RECORD;
BEGIN

  FOR myrec1 IN SELECT (each(sub_config)).*
  LOOP
    -- Check if SubConfig name & id are valid
    IF NOT ExistSubConfig(myrec1.key::TEXT, myrec1.value::INT) THEN
      RAISE EXCEPTION '++++++++++++ Configuration % ID = % does not exist! +++++++++++++++', myrec1.key, myrec1.value;
    END IF;
  END LOOP;
  FOR myrec1 IN SELECT (each(sub_config)).*
  LOOP
    FOR myrec2 IN SELECT * FROM ListDependencies(myrec1.key::TEXT,myrec1.value::INT)
    LOOP
      RETURN QUERY SELECT myrec2.SubConfigName::TEXT, myrec2.SubConfigID::INT;
    END LOOP;
  END LOOP;

END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

CREATE OR REPLACE FUNCTION ListDependencies(main_config_id INT) RETURNS TABLE(SubConfigName TEXT, SubConfigID INT) AS $$
DECLARE
  myquery TEXT;
  myrec   RECORD;
  myrow   RECORD;
  sub_configs HSTORE;
  ctr INT;
BEGIN
  IF NOT ExistMainConfig(main_config_id) THEN
    RAISE EXCEPTION '+++++++++++++ No main configuration with ID % ++++++++++++++',main_config_id;
  END IF;

  ctr := 0;
  myquery := 'SELECT * FROM ';
  FOR myrec IN SELECT * FROM ListSubConfigs(main_config_id)
  LOOP
    IF ctr = 0 THEN
      myquery := 'SELECT * FROM ListDependencies(''' || myrec.SubConfigName || ''',' || myrec.SubConfigID || ') ';
    ELSE
      myquery := myquery || ' UNION ';
      myquery := myquery || ' (SELECT * FROM ListDependencies(''' || myrec.SubConfigName || ''',' || myrec.SubConfigID || ')) ';
    END IF;
    ctr := ctr + 1;
    myquery := myquery || ' UNION VALUES (''' || myrec.SubConfigName || ''',' || myrec.SubConfigID || ') ';
  END LOOP;

  --RAISE WARNING '%',myquery;

  FOR myrow IN EXECUTE myquery
  LOOP  
    RETURN QUERY SELECT myrow.SubConfigName::TEXT, myrow.SubConfigID::INT;
  END LOOP;
END;
$$ LANGUAGE PLPGSQL VOLATILE STRICT;


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

DROP FUNCTION IF EXISTS CreateSubConfiguration( config_name TEXT );
CREATE OR REPLACE FUNCTION CreateSubConfiguration( config_name TEXT ) RETURNS INT AS $$

  DECLARE
    newconfig INT;
    myrec RECORD;
    query text;  
  BEGIN
    -- First find if a config like this exists, if not, insert it.
    IF ExistSubConfig(config_name) THEN
      RAISE EXCEPTION '++++++++++ Configuration % Already Exists +++++++++++', config_name;
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
   ConfigID         INT NOT NULL DEFAULT NULL,
   Parameters    HSTORE NOT NULL,
   ParameterSets HSTORE NOT NULL,
   userID      VARCHAR NULL DEFAULT NULL,
   TimeStamp TIMESTAMP NULL DEFAULT NULL,
   PRIMARY KEY ( ConfigID ))';
 
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


CREATE OR REPLACE FUNCTION InsertSubConfiguration( config_name TEXT,
     	      	 	 			   config_id   INT,
						   params      HSTORE,
						   paramsets   HSTORE
						   ) RETURNS INT AS $$
  DECLARE
  query TEXT;
  myrec RECORD;
  prohibit BOOLEAN;
  num_cfg_all INT;
  num_cfg_unique INT;
  BEGIN

  IF NOT ExistSubConfig(config_name) THEN
    SELECT CreateSubConfiguration(config_name) INTO myrec;
    IF NOT ExistSubConfig(config_name) THEN
      RAISE EXCEPTION '++++++++++ Configuration % does not exist! +++++++++++', config_name;
    END IF;
  END IF;

  IF ExistSubConfig(config_name, config_id) THEN
    RAISE EXCEPTION '++++++++++ Configuration % with ID % is already defined! +++++++++++', config_name, config_id;
  END IF;

  FOR myrec IN SELECT (each(paramsets)).*
  LOOP
    -- Check if SubConfig name & id are valid
    IF NOT ExistSubConfig(myrec.key::TEXT, myrec.value::INT) THEN
      RAISE EXCEPTION '++++++++++++ Configuration % ID = % does not exist! +++++++++++++++', myrec.key, myrec.value;
    END IF;
    -- Xi should not depend on Xj (part 1)
    IF myrec.key::TEXT = config_name THEN
      RAISE EXCEPTION '+++++++++++ Configuration % depends on self! +++++++++++', config_name;
    END IF;
  END LOOP;

  -- Xi should not depend on Xj (part 2)
  prohibit := False;
  SELECT INTO prohibit TRUE FROM ListDependencies(paramsets) WHERE SubConfigName = config_name;
  IF prohibit THEN
    RAISE EXCEPTION '+++++++++++ Configuration % depends on self! +++++++++++', config_name;
  END IF;

  -- Xi should not depend on Yj twice
  SELECT INTO num_cfg_all COUNT(*) FROM ListDependencies(paramsets);
  SELECT INTO num_cfg_unique COUNT(DISTINCT (SubConfigName,SubConfigID)) FROM ListDependencies(paramsets);
  IF not num_cfg_unique = num_cfg_all THEN
    RAISE EXCEPTION '++++++++++++ Duplicate Configuration Dependency! +++++++++++++';
  END IF;

  query := 'INSERT INTO '||config_name||'(ConfigID,Parameters,ParameterSets) VALUES('||config_id||','''||params||''','''||paramsets||''')';
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
   IF NOT ExistSubConfig(configtablename) THEN
     RAISE EXCEPTION '++++++++++ Configuration % is not defined yet! +++++++++++', configtablename;
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


------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION InsertMainConfiguration( subconfigparameters HSTORE,
       	  	  	   			    confname text DEFAULT 'no_name') RETURNS INT AS $$
  DECLARE
  myrec RECORD;
  ctr INT;
  ColumnPair1 RECORD;
  ColumnPair2 RECORD;
  uniqueness BOOLEAN;
  SubConfigCount INT;
  newconfig INT;
  query text;  
  SubConfT INT;
  localconfigexists INT;
  BEGIN    
    -- 1st CHECK: check if the provided Config ID already exsits in the MainConfigTable or not
    IF  (confname <> 'no_name' ) AND EXISTS (SELECT ConfigID FROM MainConfigTable WHERE MainConfigTable.ConfigName = confname)
        THEN RAISE EXCEPTION '+++++++++++++ Config with name % already exists in MainConfigTable! +++++++++++++',confname;
	RETURN -1;
    END IF;
    -- now we know that the name doesn't exist or the user doesn't care.

    -- 2nd CHECK:
    ---- Make sure subconfig exists & unique
    FOR ColumnPair1 IN SELECT (each(subconfigparameters)).*
    LOOP
      -- Check if SubConfigType is valid or not
      IF NOT ExistSubConfig(ColumnPair1.key::TEXT, ColumnPair1.value::INT) THEN
        RAISE EXCEPTION '++++++++++++ Configuration % ID = % does not exist! +++++++++++++++', ColumnPair1.key, ColumnPair1.value;
      END IF;
      ctr := 0;
      FOR ColumnPair2 IN SELECT (each(subconfigparameters)).*
      LOOP
        IF ColumnPair1.key = ColumnPair2.key AND ColumnPair1.value = ColumnPair2.value THEN
	  ctr := ctr + 1;
	END IF;
      END LOOP;
      IF ctr > 1 THEN
        RAISE EXCEPTION '++++++++++++ Configuration % ID = % is duplicated! +++++++++++++++',ColumnPair1.key,ColumnPair1.value;
      END IF;
    END LOOP;

    -- 3rd CHECK:
    ---- Avoid making another main config entry with identical contents
      FOR myrec IN SELECT * FROM ListAllMainConfigs()
    LOOP
      SELECT COUNT(TRUE) FROM MainConfigTable WHERE ConfigID = myrec.ConfigID INTO SubConfigCount;
      IF NOT SubConfigCount = ARRAY_LENGTH(AKEYS(subconfigparameters),1)
	THEN CONTINUE;  
      END IF;
      uniqueness := FALSE;
      for ColumnPair1 IN SELECT (each(subconfigparameters)).*
      LOOP
	ctr := 0;
	SELECT INTO ctr COUNT(TRUE) FROM ListSubConfigs(myrec.ConfigID) AS A
	       		   	    WHERE A.SubConfigName = ColumnPair1.key::TEXT AND A.SubConfigID = ColumnPair1.value::INT;
	--RAISE WARNING '% : % ... Counter = %',ColumnPair1.key::TEXT,ColumnPair1.value::INT,ctr;
	IF ctr = 0 THEN
	  uniqueness := TRUE;
	  EXIT;
	END IF;
	--RAISE WARNING 'Continuing...';
      END LOOP;
      IF NOT uniqueness THEN
        RAISE EXCEPTION '+++++++++++++ This Configuration already exists as % ... ID = %! +++++++++++++',myrec.ConfigName, myrec.ConfigID;
      END IF;
    END LOOP;
    
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
   FOR myrec IN SELECT (each(subconfigparameters)).*
    LOOP
    SELECT SubconfigType FROM ConfigLookUp WHERE SubConfigName = myrec.key INTO SubConfT;
    INSERT INTO MainconfigTable ( ConfigID,
      			    	  SubConfigType,
				  SubConfigID, 
				  ConfigName) VALUES
				  ( newconfig, SubConfT, CAST(myrec.value AS INT), confname );
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


