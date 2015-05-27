SET ROLE uboonedaq_admin;
-- ---
-- Globals
-- ---
--CREATE EXTENSION hstore;
-- SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
-- SET FOREIGN_KEY_CHECKS=0;

-- ---
-- Table 'MainRun'
-- This table is updated at the start of each run and stores configuration variables and run start and endtimes.
-- ---

DROP TABLE IF EXISTS  MainConfigTable ;
		
CREATE TABLE  MainConfigTable  (
   ConfigID       INT NOT NULL   DEFAULT 0,
   ConfigName     VARCHAR NULL DEFAULT NULL,
   SubConfigType  INT  NOT NULL DEFAULT 0 , -- 'Specifies which subtable we want',
   SubConfigID    INT  NOT NULL DEFAULT 0,
   SubConfigMask  BIT(64)  NOT NULL , -- 'which subsystems are on',
   userID  VARCHAR NULL DEFAULT NULL,
   TimeStamp  TIMESTAMP NULL DEFAULT NULL,
  PRIMARY KEY ( ConfigID ,  SubConfigType )
-- KEY ( SubConfigType ,  SubConfigID )
); -- 'This table stores all the configuration parameters';

-- CREATE TRIGGER mainconfigcheck BEFORE INSERT OR UPDATE ON mainconfigtable
--     FOR EACH ROW EXECUTE PROCEDURE mainconfigcheck();


DROP TABLE IF EXISTS  MainRun ;
		
CREATE TABLE  MainRun  (
   RunNumber  INT NOT NULL   DEFAULT 0 , -- 'Run Number',
   SubRunNumber INT NOT NULL DEFAULT 0, -- 'Sub Run Number',
   RunType  SMALLINT  NOT NULL DEFAULT 0 , -- 'Defines what type of run we''re running. Can connect to Confi',
   TimeStart  TIMESTAMP  NOT NULL DEFAULT NULL,
   TimeStop  TIMESTAMP NULL DEFAULT NULL,
   ConfigID  INT  NOT NULL DEFAULT 0 , -- 'Stores Configuration ID', --REFERENCES  MainConfigTable  ( ConfigID )
-- KEY ( ConfigID ),
  PRIMARY KEY ( RunNumber,SubRunNumber )
); -- 'This table is updated at the start of each run and stores co';


-- CREATE TRIGGER mainruncheck BEFORE INSERT OR UPDATE ON mainrun
--     FOR EACH ROW EXECUTE PROCEDURE mainruncheck();



-- ---
-- Table 'ConfigLookUp'
-- This table stores the names and columns of the different configuration sets
-- ---

DROP TABLE IF EXISTS  ConfigLookUp ;
		
CREATE TABLE  ConfigLookUp  (
   SubConfigType  INT NOT NULL,
   SubConfigName VARCHAR  NOT NULL,
   SetupColumns  HSTORE,
   userID  VARCHAR NULL DEFAULT NULL,
   TimeStamp  TIMESTAMP NULL DEFAULT NULL,
  PRIMARY KEY ( SubConfigType )
); -- 'Calibration Hardware parameters';

DROP TYPE IF EXISTS UnitConfig CASCADE;
CREATE TYPE UnitConfig AS ( Crate INT, Slot INT, Channel INT, Params HSTORE );

DROP TYPE IF EXISTS subconfigreturntype CASCADE; 

-- ---
-- Table 'SlowMonTable'
-- 
-- ---

DROP TABLE IF EXISTS  SlowMonTable ;
		
CREATE TABLE  SlowMonTable  (
   TimeStamp  TIMESTAMP  NOT NULL DEFAULT NULL,
   VarType  VARCHAR  NOT NULL DEFAULT 0,
   Value  VARCHAR  NOT NULL DEFAULT 0,
  PRIMARY KEY ( TimeStamp ,  VarType )
);

-- ---
-- Table 'CryoMonTable'
-- 
-- ---

DROP TABLE IF EXISTS  CryoMonTable ;
		
CREATE TABLE  CryoMonTable  (
   TimeStamp  TIMESTAMP  NOT NULL DEFAULT NULL,
   VarType  VARCHAR  NOT NULL DEFAULT 0,
   Value  VARCHAR  NOT NULL DEFAULT 0,
  PRIMARY KEY ( TimeStamp ,  VarType )
);

-- ---
-- Foreign Keys 
-- ---

--ALTER TABLE  MainRun  ADD FOREIGN KEY (ConfigID) REFERENCES  MainConfigTable  ( ConfigID );
--ALTER TABLE  MainConfigTable  ADD FOREIGN KEY (SubConfigID) REFERENCES  PMTConfig  ( ConfigID );
--ALTER TABLE  MainConfigTable  ADD FOREIGN KEY (SubConfigID) REFERENCES  TPCConfig  ( ConfigID );
--ALTER TABLE  MainConfigTable  ADD FOREIGN KEY (SubConfigID) REFERENCES  CalibrationConfig  ( ConfigID );

-- ---
-- Table Properties
-- ---

-- ALTER TABLE  MainRun  ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
-- ALTER TABLE  PMTConfig  ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
-- ALTER TABLE  TPCConfig  ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
-- ALTER TABLE  MainConfigTable  ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
-- ALTER TABLE  CalibrationConfig  ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
-- ALTER TABLE  SlowMonTable  ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
-- ALTER TABLE  CryoMonTable  ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

-- ---
-- Test Data
-- ---

-- INSERT INTO  MainRun  ( RunNumber , RunType , TimeStart , TimeStop , ConfigID ) VALUES
-- ('','','','','');
-- INSERT INTO  PMTConfig  ( ConfigID , Crate , Channel , ParameterX , userID , TimeStamp ) VALUES
-- ('','','','','','');
-- INSERT INTO  TPCConfig  ( ConfigID , Crate , Channel , ParameterX , userID , TimeStamp ) VALUES
-- ('','','','','','');
-- INSERT INTO  MainConfigTable  ( ConfigID , SubConfigType , SubConfigID , SubConfigParentID , SubConfigOnMask , ConfigName , userID , TimeStamp ) VALUES
-- ('','','','','','','','');
-- INSERT INTO  CalibrationConfig  ( ConfigID , Crate , Channel , ParameterX , userID , TimeStamp ) VALUES
-- ('','','','','','');
-- INSERT INTO  SlowMonTable  ( TimeStamp , VarType , Value ) VALUES
-- ('','','');
-- INSERT INTO  CryoMonTable  ( TimeStamp , VarType , Value ) VALUES
-- ('','','');
