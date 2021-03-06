SET ROLE uboonedaq_admin;

------------------------------------------------------------------------------

--------------------------------------------------------------------
-- ExistMainConfig from Name
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- ExistMainConfig from ID
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- MainConfig Name from ID
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS MainConfigName(MCfgName TEXT);
DROP FUNCTION IF EXISTS MainConfigName(MCfgID INT);
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

--------------------------------------------------------------------
-- MainConfig ID from Name
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS MainConfigID(MCfgName TEXT);
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

--------------------------------------------------------------------
-- ReSetRunType for MainConfig (Name)
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS SetRunType(TEXT,SMALLINT);
CREATE OR REPLACE FUNCTION SetRunType(MCfgName TEXT, RType SMALLINT)
       	  	  RETURNS VOID AS $$
DECLARE
  mcfg_id INT;
  myrec RECORD;
BEGIN
  IF NOT ExistMainConfig(MCfgName) THEN
    RAISE EXCEPTION '++++++++++++ No MainConfig w/ Name % ++++++++++++',MCfgName;
  END IF;

  SELECT MainConfigID(MCfgName) INTO mcfg_id;

  SELECT INTO myrec MainConfigTable.RunType FROM MainConfigTable WHERE MainConfigTable.ConfigID = mcfg_id LIMIT 1;

  IF myrec.RunType >= 0 THEN
    RAISE EXCEPTION '++++++++++++ Cannot re-set RunType! (already a valid ID %d) +++++++++++++',myrec.RunType;
  END IF;

  UPDATE MainConfigTable SET RunType = RType WHERE ConfigID = mcfg_id;
  UPDATE MainRun SET RunType = RType WHERE ConfigID = mcfg_id;

END;
$$ LANGUAGE PLPGSQL;

--------------------------------------------------------------------
-- ReSetRunType for MainConfig (ID)
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS SetRunType(INT,SMALLINT);
CREATE OR REPLACE FUNCTION SetRunType(MCfgID INT, RType SMALLINT)
       	  	  RETURNS VOID AS $$
DECLARE
  mcfg_name TEXT;
  res RECORD;
BEGIN
  IF NOT ExistMainConfig(MCfgID) THEN
      RAISE EXCEPTION '++++++++++++ No MainConfig w/ ID % ++++++++++++',MCfgID;
  END IF;

  SELECT MainConfigName(MCfgID) INTO mcfg_name;
  SELECT SetRunType(mcfg_name,RType) INTO res;

END;
$$ LANGUAGE PLPGSQL;

--------------------------------------------------------------------
-- ReSetName for MainConfig (Name)
--------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ResetMainConfigName(MCfgName TEXT, NewName TEXT)
       	  	  RETURNS VOID AS $$
DECLARE
BEGIN
  IF NOT ExistMainConfig(MCfgName) THEN
      RAISE EXCEPTION '++++++++++++ No MainConfig w/ Name % ++++++++++++',MCfgName;
  END IF;

  UPDATE MainConfigTable SET ConfigName = NewName WHERE ConfigName = MCfgName;

END;
$$ LANGUAGE PLPGSQL;

--------------------------------------------------------------------
-- ReSetName for MainConfig (ID)
--------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ResetMainConfigName(MCfgID INT, NewName TEXT)
       	  	  RETURNS VOID AS $$
DECLARE
BEGIN
  IF NOT ExistMainConfig(MCfgID) THEN
      RAISE EXCEPTION '++++++++++++ No MainConfig w/ ID % ++++++++++++',MCfgID;
  END IF;

  UPDATE MainConfigTable SET ConfigName = NewName WHERE ConfigID = MCfgID;

END;
$$ LANGUAGE PLPGSQL;


--------------------------------------------------------------------
-- ExistSubConfig w/ Name
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- ExistSubConfig w/ ID
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- ExistSubConfig w/ Name & ID
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- FindSubConfig w/ Contents
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS FindSubConfig(TEXT,HSTORE,HSTORE);

CREATE OR REPLACE FUNCTION FindSubConfig( SCfgName  TEXT,
       	  	  	   		  params    HSTORE,
					  paramsets HSTORE )
		  RETURNS INT AS $$
DECLARE
  query TEXT;
  myrec RECORD;
BEGIN

  IF ExistSubConfig(SCfgName) THEN
    query := format('SELECT ConfigID, Parameters AS ps, ParameterSets AS psets FROM %s;',SCfgName);
    EXECUTE query INTO myrec;
    FOR myrec IN EXECUTE query LOOP
      IF params <@ myrec.ps AND params @> myrec.ps AND paramsets <@ myrec.psets AND paramsets @> myrec.psets THEN
        RETURN myrec.ConfigID;
      END IF;
    END LOOP;
  END IF;
  RETURN -1;
END;
$$ LANGUAGE PLPGSQL;

--------------------------------------------------------------------
-- ExistSubConfig w/ Contents
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS ExistSubConfig( TEXT, HSTORE, HSTORE );

CREATE OR REPLACE FUNCTION ExistSubConfig( SCfgName  TEXT,
       	  	  	   		   params    HSTORE,
					   paramsets HSTORE )
		  RETURNS BOOLEAN AS $$
DECLARE
  scfg_id INT;
BEGIN

  SELECT INTO scfg_id FindSubConfig(SCfgName,params,paramsets);
  IF scfg_id >= 0 THEN
    RETURN TRUE;
  END IF;

  RETURN FALSE;
END;
$$ LANGUAGE PLPGSQL;

--------------------------------------------------------------------
-- SubConfig Name from Type
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS SubConfigName(SCfgType INT);
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

--------------------------------------------------------------------
-- SubConfig Type from Name
--------------------------------------------------------------------
DROP FUNCTION IF EXISTS SubConfigType(SCfgName TEXT);
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

DROP FUNCTIOn IF EXISTS ListAllMainConfigs();
CREATE OR REPLACE FUNCTION ListAllMainConfigs()
       	  	  RETURNS TABLE (ConfigName TEXT, ConfigID INT, RunType SMALLINT, Expert BOOLEAN, Arxived BOOLEAN) AS $$
DECLARE
  rec RECORD;
BEGIN

  FOR rec IN SELECT DISTINCT MainConfigTable.ConfigName,
	       	      	     MainConfigTable.ConfigID,
			     MainConfigTable.RunType,
			     MainConfigTable.Expert,
		       	     MainConfigTable.Arxived
	     FROM MainConfigTable
	     ORDER BY MainConfigTable.ConfigID
  LOOP
      RETURN QUERY SELECT rec.ConfigName::TEXT, rec.ConfigID::INT, rec.RunType::SMALLINT, rec.Expert::BOOLEAN, rec.Arxived::BOOLEAN;
  END LOOP;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS ListMainConfigs();
CREATE OR REPLACE FUNCTION ListMainConfigs()
       	  	  RETURNS TABLE (ConfigName TEXT, ConfigID INT, RunType SMALLINT, Expert BOOLEAN, Arxived BOOLEAN) AS $$
DECLARE
  rec RECORD;
BEGIN
  FOR rec IN SELECT DISTINCT MainConfigTable.ConfigName,
      	     	    	     MainConfigTable.ConfigID,
			     MainConfigTable.RunType,
			     MainConfigTable.Expert,
			     MainConfigTable.Arxived
      	     FROM MainConfigTable
	     WHERE NOT Arxived
  LOOP
    RETURN QUERY SELECT rec.ConfigName::TEXT, rec.ConfigID::INT, rec.RunType::SMALLINT, rec.Expert::BOOLEAN, rec.Arxived::BOOLEAN;
  END LOOP;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS MainConfigMetaData(TEXT);
CREATE OR REPLACE FUNCTION MainConfigMetaData(CfgName TEXT)
       	  	  RETURNS TABLE (ConfigName TEXT, ConfigID INT, RunType SMALLINT, Expert BOOLEAN, Arxived BOOLEAN) AS $$
DECLARE
  rec RECORD;
BEGIN
  IF NOT ExistMainConfig(CfgName) THEN
    RAISE EXCEPTION '+++++++++++ No MainConfig w/ name % +++++++++++', MCfg;
  END IF;
  FOR rec IN SELECT DISTINCT MainConfigTable.ConfigName,
      	     	    	     MainConfigTable.ConfigID,
			     MainConfigTable.RunType,
			     MainConfigTable.Expert,
			     MainConfigTable.Arxived
      	     FROM MainConfigTable
	     WHERE MainConfigTable.ConfigName = CfgName
	     LIMIT 1
  LOOP
    RETURN QUERY SELECT rec.ConfigName::TEXT, rec.ConfigID::INT, rec.RunType::SMALLINT, rec.Expert::BOOLEAN, rec.Arxived::BOOLEAN;
  END LOOP;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ArxiveMainConfig(MCfg TEXT) RETURNS INT AS $$
DECLARE
  rec RECORD;
BEGIN

  IF NOT ExistMainConfig(MCfg) THEN
    RAISE EXCEPTION '+++++++++++ No MainConfig w/ name % +++++++++++', MCfg;
  END IF;

  UPDATE MainConfigTable SET Arxived = TRUE WHERE ConfigName = MCfg;
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ActivateMainConfig(MCfg TEXT) RETURNS INT AS $$
DECLARE
  rec RECORD;
BEGIN

  IF NOT ExistMainConfig(MCfg) THEN
    RAISE EXCEPTION '+++++++++++ No MainConfig w/ name % +++++++++++', MCfg;
  END IF;

  UPDATE MainConfigTable SET Arxived = FALSE WHERE ConfigName = MCfg;
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ArxiveMainConfig(MCfgID INT) RETURNS INT AS $$
DECLARE
  MCfgName TEXT;
  Success INT;
BEGIN
  SELECT INTO MCfgName MainConfigName(MCfgID);
  SELECT INTO Success ArxiveMainConfig(MCfgName);
  RETURN Success;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION ActivateMainConfig(MCfgID INT) RETURNS INT AS $$
DECLARE
  MCfgName TEXT;
  Success INT;
BEGIN
  SELECT INTO MCfgName MainConfigName(MCfgID);
  SELECT INTO Success ActivateMainConfig(MCfgName);
  RETURN Success;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION MakeNonExpertConfig(MCfg TEXT) RETURNS INT AS $$
DECLARE
  rec RECORD;
BEGIN

  IF NOT ExistMainConfig(MCfg) THEN
    RAISE EXCEPTION '+++++++++++ No MainConfig w/ name % +++++++++++', MCfg;
  END IF;

  UPDATE MainConfigTable SET Expert = FALSE WHERE ConfigName = MCfg;
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION MakeExpertConfig(MCfg TEXT) RETURNS INT AS $$
DECLARE
  rec RECORD;
BEGIN

  IF NOT ExistMainConfig(MCfg) THEN
    RAISE EXCEPTION '+++++++++++ No MainConfig w/ name % +++++++++++', MCfg;
  END IF;

  UPDATE MainConfigTable SET Expert = TRUE WHERE ConfigName = MCfg;
  RETURN 0;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION MakeNonExpertConfig(MCfgID INT) RETURNS INT AS $$
DECLARE
  MCfgName TEXT;
  Success INT;
BEGIN
  SELECT INTO MCfgName MainConfigName(MCfgID);
  SELECT INTO Success MakeNonExpertConfig(MCfgName);
  RETURN Success;
END;
$$ LANGUAGE PLPGSQL;

DROP FUNCTION IF EXISTS MainExpertConfig(MCfgID INT);
CREATE OR REPLACE FUNCTION MakeExpertConfig(MCfgID INT) RETURNS INT AS $$
DECLARE
  MCfgName TEXT;
  Success INT;
BEGIN
  SELECT INTO MCfgName MainConfigName(MCfgID);
  SELECT INTO Success MakeExpertConfig(MCfgName);
  RETURN Success;
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
  duplicate_cfg_id INT;
  BEGIN

  IF NOT ExistSubConfig(config_name) THEN
    --SELECT CreateSubConfiguration(config_name) INTO myrec;
    --IF NOT ExistSubConfig(config_name) THEN
    RAISE EXCEPTION '++++++++++ Configuration % does not exist! +++++++++++', config_name;
    --END IF;
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

  -- Make sure there is no duplication --
  SELECT INTO duplicate_cfg_id FindSubConfig(config_name,params,paramsets);
  IF duplicate_cfg_id >= 0 THEN
    RAISE EXCEPTION '++++++++++++ Duplicate Configuration Contents (already exist as ID=%) +++++++++++++',duplicate_cfg_id;
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
DROP FUNCTION IF EXISTS InsertMainConfiguration(HSTORE,TEXT);
DROP FUNCTION IF EXISTS InsertMainConfiguration(HSTORE,TEXT,SMALLINT);
CREATE OR REPLACE FUNCTION InsertMainConfiguration( subconfigparameters HSTORE,
       	  	  	   			    confname text,
						    confRunType SMALLINT DEFAULT -1)
		  RETURNS INT AS $$
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
				  ConfigName,
				  RunType,
				  Arxived,
				  Expert) VALUES
				  ( newconfig, SubConfT, CAST(myrec.value AS INT), confname, CAST(confRunType AS SMALLINT), FALSE, TRUE );
    END LOOP;

   RETURN newconfig;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------

DROP FUNCTION IF EXISTS CleanSubConfig( cfg_name TEXT,
       	  	  	   	        cfg_id   INT);

CREATE OR REPLACE FUNCTION CleanSubConfig( cfg_name TEXT,
       	  	  	   		   cfg_id   INT) RETURNS INT AS $$
DECLARE
query TEXT;
sub_conf_type INT;
sub_conf_id INT;
sub_conf_ctr INT;
rec1 RECORD;
rec2 RECORD;
res INT;
BEGIN

  sub_conf_type := -1;  
  SELECT INTO sub_conf_type SubConfigType(cfg_name);
  IF sub_conf_type IS NULL THEN 
  RETURN 0;
  END IF;

  sub_conf_id :=-1;
  res := 1;
  -- Check if this is used in MainConfig
  SELECT SubConfigID FROM MainConfigTable WHERE SubConfigType = sub_conf_type AND SubConfigID = cfg_id INTO sub_conf_id;
  IF sub_conf_id IS NOT NULL AND sub_conf_id >= 0 THEN
    RAISE WARNING '++++++++++++ Sub-Config %s w/ ID % currently in use within MainConfigTable! ++++++++++++',cfg_name,cfg_id;
    RETURN res;
  ELSE
    res := 0;
    -- Check if this is used by other SubConfig
    FOR rec1 IN SELECT DISTINCT SubConfigName FROM ConfigLookUp
    LOOP
        query := 'SELECT ConfigID AS ParentID, ParameterSets->''' || cfg_name || ''' AS ChildID FROM ' || rec1.SubConfigName || ' WHERE ParameterSets ? ''' || cfg_name || ''';'; 
	EXECUTE query INTO rec2;
	IF rec2 IS NOT NULL AND rec2.ChildID::INT = cfg_id THEN
	  RAISE WARNING 'SubConfig % is used by another SubConfig % (ID=%) and possibly others...', cfg_name,rec1.SubConfigName,rec2.ParentID;
	  res := 1;
	END IF;
    END LOOP;
    IF res = 0 THEN
      query := 'DELETE FROM ' || cfg_name || ' WHERE ConfigID = ' || cfg_id || ';';
      EXECUTE query;
    END IF;
  END IF;
  RETURN res;
END;
$$ LANGUAGE plpgsql VOLATILE STRICT;

-------------------------------------------------------------

DROP FUNCTION IF EXISTS CleanSubConfig( cfg_name TEXT);

CREATE OR REPLACE FUNCTION CleanSubConfig( cfg_name TEXT ) RETURNS INT AS $$
DECLARE
query TEXT;
rec RECORD;
status INT;
res    INT;
sub_conf_type INT;
sub_conf_id INT;
sub_conf_ctr INT;
BEGIN

  IF NOT ExistSubConfig(cfg_name) THEN
    RAISE WARNING '+++++++++++++ No such SubConfig % +++++++++++++',cfg_name;
    RETURN 1;
  END IF;

  res := 0;
  query := 'SELECT DISTINCT SubConfigID FROM ' || cfg_name || ';';
  FOR rec IN EXECUTE query
  LOOP
    SELECT CleanSubConfig(cfg_name,rec.SubConfigID::INT) INTO status;
    IF status > 0 THEN
      res = res + 1;
    END IF;
  END LOOP;

  IF res = 0 THEN  
    query := 'DROP TABLE ' || cfg_name || ';';
    EXECUTE query;
    DELETE FROM ConfigLookUp WHERE SubConfigName = cfg_name;
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


