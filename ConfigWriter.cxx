#ifndef DBTOOL_CONFIGWRITER_CXX
#define DBTOOL_CONFIGWRITER_CXX

#include "ConfigWriter.h"
#include <stdlib.h>
namespace ubpsql {

  bool ConfigWriter::InsertSubConfiguration(const SubConfig& cfg)
  {
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
      if( ctr ) query += Form(" \"%s\"=>%d,", sub_config.first.c_str(),sub_config.second);
      else query += Form(" \"%s\"=>%d", sub_config.first.c_str(),sub_config.second);
      --ctr;
    }
    query += " '::HSTORE,'";
    query += cfg.Name() + "');";

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

  bool ConfigWriter::InsertNewRun(unsigned int config_id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT InsertNewRun(%d);",config_id));
    if(!res) return false;

    PQclear(res);
    return true;
  }

  bool ConfigWriter::InsertNewSubRun(unsigned int config_id, unsigned int run)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT InsertNewSubRun(%d,%d);",config_id,run));
    if(!res) return false;
    
    PQclear(res);
    return true;
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

  void ConfigWriter::SuperLaser()
  {
    if(!Connect()) throw ConnectionError();
    Print(msg::kWARNING,__FUNCTION__,Form("Death Star is completed. Attempting to destroy Endor via SuperLaser."));
    PGresult* res = _conn->Execute("SELECT CleanConfigDB();");
    Print(msg::kWARNING,__FUNCTION__,Form("Bahahahaha!"));
    if(res) PQclear(res);
    return;
  }
  
}

#endif
