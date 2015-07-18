SET ROLE uboonedaq_admin;

-- CREATE LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION ExistRun(run INT, subrun INT DEFAULT NULL) RETURNS BOOLEAN AS $$
DECLARE
  run_exist BOOLEAN;
BEGIN

  IF subrun IS NULL THEN
    SELECT INTO run_exist TRUE FROM MainRun WHERE RunNumber = run;
  ELSE
    SELECT INTO run_exist TRUE FROM MainRun WHERE RunNumber = run AND SubRunNumber = subrun;
  END IF;

  IF run_exist IS NULL THEN
    run_exist = FALSE;
  END IF;

  RETURN run_exist;

END;
$$ LANGUAGE plpgsql;


--CREATE OR REPLACE FUNCTION testfunc(integer) RETURNS integer AS $PROC$
--          ....
--$PROC$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION InsertNewRun(CfgID INT)  RETURNS integer AS $$
DECLARE
    lastrun mainrun.RunNumber%TYPE;
BEGIN
     SELECT INTO lastrun GetLastRun();
   
     INSERT INTO MainRun(RunNumber,SubRunNumber,ConfigID,RunType) VALUES (lastrun+1,0,CfgID,-1);


    RETURN lastrun+1;

END;
$$ LANGUAGE plpgsql;

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

     INSERT INTO MainRun(RunNumber,SubRunNumber,ConfigID,RunType) VALUES (lastrun,lastsubrun+1,CfgID,-1);

    RETURN lastsubrun+1;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION RunExist( run INT ) RETURNS BOOLEAN AS $$
DECLARE
  res BOOLEAN;
BEGIN
  res := FALSE;
  SELECT TRUE FROM MainRun WHERE RunNumber = run INTO res;
  IF res IS NULL OR NOT res THEN
    RETURN FALSE;
  END IF;
  RETURN TRUE;
END;
$$ LANGUAGE PLPGSQL;

CREATE OR REPLACE FUNCTION RunExist( run INT, subrun INT ) RETURNS BOOLEAN AS $$
DECLARE
  res BOOLEAN;
BEGIN
  res := FALSE;
  SELECT TRUE FROM MainRun WHERE RunNumber = run AND SubRunNumber = subrun INTO res;
  IF res IS NULL OR NOT res THEN
    RETURN FALSE;
  END IF;
  RETURN TRUE;
END;
$$ LANGUAGE PLPGSQL;

-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
--CREATE OR REPLACE FUNCTION InsertNewSubRun( run INT,
--					    subrun INT,
--					    ts TIMESTAMP,
--					    te TIMESTAMP,
--					    CfgID INT DEFAULT -1) RETURNS INT AS $$
--DECLARE
--rtype INT;
--BEGIN
--     IF NOT EXISTS (SELECT TRUE FROM MainRun WHERE RunNumber = run) THEN
--     	RAISE EXCEPTION 'Run % does not exist!', run;
--     ELSE IF EXISTS (SELECT TRUE FROM MainRun WHERE RunNumber = run AND SubRunNumber = subrun) THEN
--     	RAISE EXCEPTION 'Run % SubRun % already exist!', run, subrun;
--     ELSE IF NOT EXISTS (SELECT TRUE FROM MainRun WHERE RunNumber = run AND SubRunNumber = (subrun-1)) THEN
--     	RAISE EXCEPTION 'Run % SubRun % should exist but not...', run, subrun;
--     END IF;
--
--     IF te < ts THEN
--     	RAISE EXCEPTION 'EndTime cannot be smaller than StartTime!';
--     END IF;
--
--     IF CfgID = -1 THEN
--       SELECT INTO CfgID ConfigID FROM MainRun  ORDER BY RunNumber,SubRunNumber DESC LIMIT 1;
--     END IF;
--
--     SELECT INTO rtype RunType FROM MainRun  ORDER BY RunNumber,SubRunNumber DESC LIMIT 1;
--
--     INSERT INTO MainRun(RunNumber,SubRunNumber,ConfigID,RunType,TimeStart,TimeStop) VALUES (run,subrun,CfgID,rtype,ts,te);
--     RETURN 0;
--END;
--$$ LANGUAGE plpgsql;

-----------------------------------------------------------------------------
-------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION UpdateStopTime( run INT,
                                           subrun INT,
					   ts TIMESTAMP) RETURNS integer AS $$
DECLARE
tstart TIMESTAMP;
causality BOOLEAN;
BEGIN
     IF NOT RunExist(run,subrun) THEN
     	RAISE EXCEPTION 'Run % SubRun % already exist!', run, subrun;
     END IF;	
     IF subrun > 0 AND RunExist(run,(subrun-1)) THEN
     	RAISE EXCEPTION 'Run % SubRun % should exist but not...', run, subrun;
     END IF;

     SELECT INTO tstart TimeStart FROM MainRun WHERE RunNumber = run AND SubRunNumber = subrun;

     IF ts < tstart THEN
       RAISE EXCEPTION 'Start time cannot be in future w.r.t. end time...';
     END IF;

     UPDATE MainRun SET TimeEnd=ts WHERE RunNumber=run AND SubRunNumber=subrun;

     RETURN 0;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION UpdateStartTime( run INT,
                                            subrun INT,
					    ts TIMESTAMP) RETURNS integer AS $$
DECLARE
tend TIMESTAMP;
BEGIN
     IF NOT ExistRun(run,subrun) THEN 
     	RAISE EXCEPTION 'Run % SubRun % does not exist!', run, subrun;
     END IF;

     SELECT tend TimeStop FROM MainRun WHERE RunNumber = run AND SubRunNumber = subrun LIMIT 1;

     IF ts > tend THEN
       RAISE EXCEPTION 'Start time cannot be in future w.r.t. end time...';
     END IF;

     UPDATE MainRun SET TimeStart=ts WHERE RunNumber=run AND SubRunNumber=subrun;

     RETURN 0;
     
END;
$$ LANGUAGE plpgsql;

-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION GetLastRun() RETURNS integer AS $$
DECLARE
    lastrun mainrun.RunNumber%TYPE;
BEGIN
    
    SELECT INTO lastrun RunNumber FROM MainRun  ORDER BY RunNumber DESC LIMIT 1;

    IF lastrun IS NULL THEN
      lastrun := 0;
    END IF;
    RETURN lastrun;
END;
$$ LANGUAGE plpgsql;


-----------------------------------------------------------------------------
-------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION GetLastSubRun(run INT) RETURNS integer AS $$
DECLARE
    lastsubrun mainrun.SubRunNumber%TYPE;
BEGIN

    IF NOT ExistRun(run) THEN
      RAISE EXCEPTION '++++++++++ Run % not found... ++++++++++',run;
    END IF;

    SELECT INTO lastsubrun SubRunNumber FROM MainRun WHERE RunNumber=run ORDER BY SubRunNumber DESC LIMIT 1;

    RETURN lastsubrun;
END;
$$ LANGUAGE plpgsql;

--CREATE TRIGGER mainconfigcheck BEFORE INSERT OR UPDATE ON mainconfigtable
--    FOR EACH ROW EXECUTE PROCEDURE mainconfigcheck();

-----------------------------------------------------------------------------
-------------------------------------------------------------------------------


