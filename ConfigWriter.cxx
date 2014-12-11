#ifndef DBTOOL_CONFIGWRITER_CXX
#define DBTOOL_CONFIGWRITER_CXX

#include "ConfigWriter.h"
#include <stdlib.h>
namespace ubpsql {

  bool ConfigWriter::CreateConfigType(const std::string config_name)
  {
    if(!Connect()) throw ConnectionError();

    // Form a command string
    std::string cmd(Form("SELECT CreateSubConfiguration('%s');",config_name.c_str()));

    PGresult* res = _conn->Execute(cmd);
    if(!res) return false;
    PQclear(res);
    return true;
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

  bool ConfigWriter::InsertSubConfiguration(const SubConfig& cfg)
  {
    auto const& config_id   = cfg.ConfigID();
    auto const& config_name = cfg.Name();

    auto const& crate_default   = cfg.GetParams(-999,-1,-1);
    auto const& slot_default    = cfg.GetParams(-1,-999,-1);
    auto const& channel_default = cfg.GetParams(-1,-1,-999);

    if(!Connect()) throw ConnectionError();
    std::string cmd(Form("SELECT InsertSubConfiguration('%s'::TEXT,%d::INT,",config_name.c_str(),config_id));

    size_t ctr = 0;

    cmd += "'";
    ctr = crate_default.size() - 1;
    TString value="";
    for(auto const& param : crate_default) {
      value = param.second;
      value = value.ReplaceAll("\"","\\\"");
      if( ctr ) cmd += Form(" \"%s\"=>\"%s\",", param.first.c_str(), value.Data());
      else cmd += Form(" \"%s\"=>\"%s\"", param.first.c_str(), value.Data());
      --ctr;
    }
    cmd += " '::HSTORE,";

    cmd += "'";
    ctr = slot_default.size() - 1;
    for(auto const& param : slot_default) {
      value = param.second;
      value = value.ReplaceAll("\"","\\\"");
      if( ctr ) cmd += Form(" \"%s\"=>\"%s\",", param.first.c_str(), value.Data());
      else cmd += Form(" \"%s\"=>\"%s\"", param.first.c_str(), value.Data());
      --ctr;
    }
    cmd += " '::HSTORE,";

    cmd += "'";
    ctr = channel_default.size() - 1;
    for(auto const& param : channel_default) {
      value = param.second;
      value = value.ReplaceAll("\"","\\\"");
      if( ctr ) cmd += Form(" \"%s\"=>\"%s\",", param.first.c_str(), value.Data());
      else cmd += Form(" \"%s\"=>\"%s\"", param.first.c_str(), value.Data());
      --ctr;
    }
    cmd += " '::HSTORE,";

    std::string byte_repr("");

    byte_repr = "";
    cmd += "'";
    for(size_t i=0; i<64; ++i)
      byte_repr = std::to_string((crate_default.Mask() >> i) & 0x1) + byte_repr;
    cmd += byte_repr + "'::BIT(64),";

    byte_repr = "";
    cmd += "'";
    for(size_t i=0; i<64; ++i)
      byte_repr = std::to_string((slot_default.Mask() >> i) & 0x1) + byte_repr;
    cmd += byte_repr + "'::BIT(64),";

    byte_repr = "";
    cmd += "'";
    for(size_t i=0; i<64; ++i)
      byte_repr = std::to_string((channel_default.Mask() >> i) & 0x1) + byte_repr;
    cmd += byte_repr + "'::BIT(64));";

    PGresult* res = _conn->Execute(cmd);
    if(!res) return false;
    
    PQclear(res);
    return true;
  }

  bool ConfigWriter::FillSubConfiguration(const SubConfig &data)
  {
    if(!Connect()) throw ConnectionError();
    auto const& cfg_name = data.Name();
    auto const  cfg_id   = data.ConfigID();
    for(auto const& key_params : data) {
      auto const& key    = key_params.first;
      auto const& params = key_params.second;
      if(key.IsDefaultCrate()   ||
	 key.IsDefaultChannel() ||
	 key.IsDefaultSlot()) continue;

      Print(msg::kDEBUG,__FUNCTION__,
	    Form("Filling SubConfig \"%s\" (ID=%d) ... Crate=%d Slot=%d Ch=%d",
		 cfg_name.c_str(),
		 cfg_id,
		 key.Crate(),
		 key.Slot(),
		 key.Channel()));

      std::string cmd(Form("SELECT FillSubConfiguration('%s'::TEXT, %d::INT, %d::INT, %d::INT, %d::INT,",
			   cfg_name.c_str(),
			   cfg_id,
			   key.Crate(),
			   key.Slot(),
			   key.Channel())
		      );

      cmd += "'";
      std::string byte_repr = "";
      for(size_t i=0; i<64; ++i)
	byte_repr = std::to_string((params.Mask()>>i) & 0x1) + byte_repr;
      cmd += byte_repr + "'::BIT(64),'";

      for(auto const& key_value : params){
	std::string value(key_value.second.c_str());
	if(value.find("\"")==0) value = "\\" + value;
	if(value.rfind("\"")==(value.size()-1)) {
	  value = value.substr(0,value.size()-1);
	  value = value + "\\" + "\"";
	}
	cmd += Form(" \"%s\"=>\"%s\",", key_value.first.c_str(), value.c_str());
      }
      if(params.find("name")==params.end())
	cmd += " \"name\"=>\"\"";

      cmd += " '::HSTORE); ";
      
      PGresult* res = _conn->Execute(cmd);

      if(!res) return false;
      PQclear(res);
    }
    return true;
  }

  bool ConfigWriter::CheckNewSubConfiguration(const SubConfig &data)
  {
    if(!Connect()) throw ConnectionError();
    bool good = true;
    auto const& cfg_name = data.Name();
    auto const  cfg_id   = data.ConfigID();
    for(auto const& key_params : data) {
      auto const& key    = key_params.first;
      auto const& params = key_params.second;
      std::string cmd(Form("SELECT CheckNewSubConfiguration('%s'::TEXT, %d::INT, %d::INT, %d::INT, %d::INT",
			   cfg_name.c_str(),
			   cfg_id,
			   key.Crate(),
			   key.Slot(),
			   key.Channel()
			   )
		      );
      cmd += "'";

      for(auto const& key_value : params)
	cmd += Form(" \"%s\"=>\"%s\",", key_value.first.c_str(), key_value.second.c_str());

      if(params.find("name")==params.end())
	cmd += Form(" \"name\"=>\"\"");

      cmd += " '::HSTORE); ";
      
      PGresult* res = _conn->Execute(cmd);
      if(!res) return false;
      
      if(PQntuples(res))
	good = good && (std::atoi(PQgetvalue(res,0,0)) == 0);
      PQclear(res);
      if(!good) break;
    }
    return good;
  }

  int ConfigWriter::InsertMainConfiguration(const RunConfig& cfg)
  {
    if(this->ExistRunConfig(cfg.Name())) {
      Print(msg::kERROR,__FUNCTION__,Form("Main config %s already exist!",cfg.Name().c_str()));
      return -1;
    }
    try {
      for(auto const& name : cfg.List()) {
	if(!(this->ExistSubConfig(name))) {
	  Print(msg::kERROR,__FUNCTION__,Form("Sub-Config \"%s\" not found!",name.c_str()));
	  return -1;
	}
	if(!(this->ExistSubConfig(name,cfg.Get(name).ConfigID()))){
	  Print(msg::kERROR,__FUNCTION__,Form("Sub-Config \"%s\" w/ ID=%d not found!",
						name.c_str(),
						cfg.Get(name).ConfigID())
		);
	  return -1;
	}
      }
    }catch(ConfigError& e) {
      return -1;
    }
    if(!cfg.List().size()) {
      Print(msg::kERROR,__FUNCTION__,"Input data is empty...");
      return -1;
    }

    std::string cmd("");
    cmd += "SELECT InsertMainConfiguration('";
    for(auto const& n : cfg.List()) {
      cmd += ("\"" + n + "\"");
      cmd += "=>";
      cmd += std::to_string(cfg.Get(n).ConfigID());
      cmd += ",";
    }
    cmd  = cmd.substr(0,cmd.size()-1);
    cmd += "'::HSTORE,'";

    for(auto const& n : cfg.List()) {
      auto const& d = cfg.Get(n);
      cmd += ("\"" + n + "\"");
      cmd += "=>";
      std::string byte_repr="";
      for(size_t i=0; i<(8*sizeof(size_t)); ++i)
	byte_repr = std::to_string((d.Mask() >> i) & 0x1) + byte_repr;
      cmd += "\"" + byte_repr + "\",";
    }
    cmd = cmd.substr(0,cmd.size()-1);
    cmd += "'::HSTORE,'" + cfg.Name() + "');";

    int val=-1;
    PGresult* res = _conn->Execute(cmd);
    if(!res) return val;

    if(PQntuples(res))
      val = std::atoi(PQgetvalue(res,0,0));

    PQclear(res);
    return val;
  }

  bool ConfigWriter::CleanMainConfig(const std::string& name)
  {
    if(!Connect()) throw ConnectionError();
    Print(msg::kWARNING,__FUNCTION__,Form("Joining the Dark Side to destroy a MainConfig."));
    if(!(this->ExistRunConfig(name))) {
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
