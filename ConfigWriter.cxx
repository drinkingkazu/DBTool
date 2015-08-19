#ifndef DBTOOL_CONFIGWRITER_CXX
#define DBTOOL_CONFIGWRITER_CXX

#include "ConfigWriter.h"
#include <TTimeStamp.h>
#include <stdlib.h>
namespace ubpsql {

  //-------------------------------------------------------------------------------------------  
  unsigned int ConfigWriter::InsertNewRun(const unsigned int config_id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT InsertNewRun(%d);",config_id));
    if(!res) return false;

    unsigned int run = atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return run;
  }

  //-------------------------------------------------------------------------------------------
  unsigned int ConfigWriter::DeathStarPlusPlus(const unsigned int config_id,
					       const unsigned int new_run_number,
					       const unsigned int age_of_eric)
  {
    if(!Connect()) throw ConnectionError();
    if(age_of_eric != kAGEOFECHURCH) {
      std::cerr << "\033[93mYou failed to contribute in DeathStar community.\033[00m" << std::endl;
      return kINVALID_UINT;
    }
    PGresult* res = _conn->Execute(Form("SELECT DeathStarPlusPlus(%d,%d);",config_id,new_run_number));
    if(!res) return false;

    unsigned int run = atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return run;
  }

  //-------------------------------------------------------------------------------------------
  unsigned int ConfigWriter::DeathStarPlusPlus(const std::string  config_name,
					       const unsigned int new_run_number,
					       const unsigned int age_of_eric)
  {
    return DeathStarPlusPlus( this->MainConfigID(config_name),
			      new_run_number,
			      age_of_eric );
  }
 
  //-------------------------------------------------------------------------------------------  
  unsigned int ConfigWriter::InsertNewRun(const std::string& cfg_name)
  { return InsertNewRun( this->MainConfigID(cfg_name) ); }

  //-------------------------------------------------------------------------------------------
  unsigned int ConfigWriter::InsertNewSubRun(const unsigned int config_id, const unsigned int run)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT InsertNewSubRun(%d,%d);",config_id,run));
    if(!res) return false;

    auto subrun = atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return subrun;
  }

  //-------------------------------------------------------------------------------------------
  unsigned int ConfigWriter::InsertNewSubRun(const std::string& cfg_name, const unsigned int run)
  { return InsertNewSubRun( this->MainConfigID(cfg_name), run); }

  //-------------------------------------------------------------------------------------------
  bool ConfigWriter::UpdateStartTime( const unsigned int  run,
				      const unsigned int  subrun,
				      const unsigned long time_sec,
				      const unsigned int  time_usec )
  {
    if(!(this->ExistRun(run,subrun)))
      throw RunNotFoundError();
    
    if(!Connect()) throw ConnectionError();

    if(time_usec >= 1e7)
      throw TableDataError();

    PGresult* res = _conn->Execute(Form("SELECT UpdateStartTime(%d,%d,to_timestamp(%lu.%06d)::TIMESTAMP);",
					run, subrun, time_sec, time_usec));
    
    if(!res) return false;

    bool success = !( atoi(PQgetvalue(res,0,0)));
    return success;
  }

  bool ConfigWriter::UpdateStopTime( const unsigned int  run,
				     const unsigned int  subrun,
				     const unsigned long time_sec,
				     const unsigned int  time_usec)
  {
    if(!(this->ExistRun(run,subrun)))
      throw RunNotFoundError();
    
    if(!Connect()) throw ConnectionError();

    if(time_usec >= 1e7)
      throw TableDataError();
    
    PGresult* res = _conn->Execute(Form("SELECT UpdateStopTime(%d,%d,to_timestamp(%lu.%06d)::TIMESTAMP);",
					run, subrun, time_sec, time_usec));

    if(!res) return false;

    bool success = !( atoi(PQgetvalue(res,0,0)));
    return success;
  }
  
  //
  // Configuration related
  //
  bool ConfigWriter::ActivateMainConfiguration(const std::string name)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ActivateMainConfig('%s');",name.c_str()));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;
  }

  bool ConfigWriter::ActivateMainConfiguration(const unsigned int id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ActivateMainConfig(%d);",id));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;
  }

  bool ConfigWriter::ArxiveMainConfiguration(const std::string name)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ArxiveMainConfig('%s');",name.c_str()));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;
  }

  bool ConfigWriter::ArxiveMainConfiguration(const unsigned int id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ArxiveMainConfig(%d);",id));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;
  }

  bool ConfigWriter::MakeExpertConfiguration(const std::string name)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM MakeExpertConfig('%s');",name.c_str()));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;    
  }

  bool ConfigWriter::MakeExpertConfiguration(const unsigned int id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM MakeExpertConfig(%d);",id));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;
  }

  bool ConfigWriter::MakeNonExpertConfiguration(const std::string name)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM MakeNonExpertConfig('%s');",name.c_str()));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;        
  }

  bool ConfigWriter::MakeNonExpertConfiguration(const unsigned int id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM MakeNonExpertConfig(%d);",id));
    if(!res) return false;
    
    bool success = (atoi(PQgetvalue(res,0,0)) == 0);
    PQclear(res);
    return success;    
  }
  
  bool ConfigWriter::InsertSubConfiguration(const SubConfig& cfg)
  {
    if(!(this->ExistSubConfig(cfg.ID().Name())))  {
      Print(msg::kERROR,__FUNCTION__,
	    Form("SubConfig \"%s\" does not exist yet. Create one from the DeathStar...",cfg.ID().Name().c_str())
	    );
      return false;
    }

    std::string params_hstore,psets_hstore;
    TString value="";
    size_t ctr = 0;

    params_hstore = "'";
    auto const& params = cfg.Parameters();
    ctr = params.size();
    value = "";
    for(auto const& param : params) {
      value = param.second;
      value = value.ReplaceAll("\"","\\\"");
      if( ctr ) params_hstore += Form(" \"%s\"=>\"%s\",", param.first.c_str(), value.Data());
      else params_hstore += Form(" \"%s\"=>\"%s\"", param.first.c_str(), value.Data());
      --ctr;
    }
    params_hstore += " '::HSTORE";

    psets_hstore = "'";
    auto const& psets = cfg.ListSubConfigIDs();
    ctr = psets.size();
    value = "";
    for(auto const& pset : psets) {
      if( ctr ) psets_hstore += Form(" \"%s\"=>%d,", pset.first.c_str(), pset.second);
      else psets_hstore += Form(" \"%s\"=>%d", pset.first.c_str(), pset.second);
      --ctr;
    }
    psets_hstore += " '::HSTORE";

    std::string query;

    query = Form("SELECT InsertSubConfiguration('%s',%d,%s,%s);",
		 cfg.ID().Name().c_str(),
		 cfg.ID().ID(),
		 params_hstore.c_str(),
		 psets_hstore.c_str());

    PGresult* res = _conn->Execute(query);
    PQclear(res);
    return true;
  }

  int ConfigWriter::InsertMainConfiguration(const MainConfig& cfg)
  {
    std::string query("SELECT * FROM InsertMainConfiguration('");
    size_t ctr = 0;

    auto const& sub_config_v = cfg.ListSubConfigIDs();
    ctr = sub_config_v.size();
    for(auto const& sub_config : sub_config_v) {
      if( ctr ) query += Form("\"%s\"=>%d,", sub_config.first.c_str(),sub_config.second);
      else query += Form(" \"%s\"=>%d", sub_config.first.c_str(),sub_config.second);
      --ctr;
    }

    query += " '::HSTORE,'";
    query += cfg.Name() + "'::TEXT,";
    query += std::to_string(cfg.RunType());
    query += "::SMALLINT);";
    //std::cout<<query.c_str()<<std::endl;
    PGresult* res = _conn->Execute(query);
    if(!res) return -1;
    int id = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return id;
  }

  bool ConfigWriter::CleanSubConfig(const std::string cfg_name, unsigned int cfg_id)
  {
    Print(msg::kWARNING,__FUNCTION__,Form("Constructing the Death Star to destroy a SubConfig ID."));
    if(!Connect()) throw ConnectionError();
    std::string cmd(Form("SELECT CleanSubConfig('%s',%d);",cfg_name.c_str(),cfg_id));

    PGresult* res = _conn->Execute(cmd);
    if(!res) return false;
    
    bool status = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return status;
  }

  bool ConfigWriter::CleanSubConfig(const std::string cfg_name)
  {
    Print(msg::kWARNING,__FUNCTION__,Form("Eliminating a SubConfig for the empire."));
    if(!Connect()) throw ConnectionError();
    std::string cmd(Form("SELECT CleanSubConfig('%s');",cfg_name.c_str()));

    PGresult* res = _conn->Execute(cmd);
    if(!res) return false;
    
    bool status = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return status;
  }

  bool ConfigWriter::CleanMainConfig(const std::string& name)
  {
    if(!Connect()) throw ConnectionError();
    Print(msg::kWARNING,__FUNCTION__,Form("Joining the Dark Side to destroy a MainConfig."));
    if(!(this->ExistMainConfig(name))) {
      Print(msg::kERROR,__FUNCTION__,Form("MainConfig \"%s\" does not exist!",name.c_str()));
      return false;
    }
    std::string cmd = Form("SELECT CleanMainConfig('%s');",name.c_str());
    PGresult* res = _conn->Execute(cmd);
    if(!res || !PQntuples(res)) return false;
    bool result = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return result;
  }

}

#endif
