#ifndef DBTOOL_CONFIGREADER_CXX
#define DBTOOL_CONFIGREADER_CXX

#include "ConfigReader.h"

namespace ubpsql {

  //----------------------------------------------------------------------------------------
  // MainRunTable functions
  //----------------------------------------------------------------------------------------

  unsigned int ConfigReader::GetLastRun()
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute("SELECT GetLastRun();");
    if(!res) return std::numeric_limits<unsigned int>::max();

    unsigned int run = atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return run;
  }

  unsigned int ConfigReader::GetLastSubRun(unsigned int run)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT GetLastSubRun(%d);",run));
    if(!res) return std::numeric_limits<unsigned int>::max();

    unsigned int subrun = atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return subrun;
  }

  bool ConfigReader::ExistRun(const unsigned int run,
			      const unsigned int subrun)
  {
    if(!Connect()) throw ConnectionError();
    std::string query = Form("SELECT 1 FROM MainRunTable WHERE RunNumber = %d",run);
    if(subrun!=kINVALID_UINT)
      query += Form(" AND SubRunNumber = %d",subrun);
    query += ";";

    PGresult* res = _conn->Execute(query);

    bool exist = false;
    if(res && PQntuples(res))
      exist = std::atoi(PQgetvalue(res,0,0));
    if(res) PQclear(res);
    return exist;
  }

  unsigned int ConfigReader::RunNumber2ConfigID(const unsigned int run, 
						const unsigned int subrun)
  {
    if(!ExistRun(run,subrun)) {
      if(subrun!=kINVALID_UINT)
	Print(msg::kERROR,__FUNCTION__,Form("Run %d does not exist!",run));
      else
	Print(msg::kERROR,__FUNCTION__,Form("Run %d SubRun %d does not exist!",run,subrun));
      throw TableDataError();
    }
    
    std::string query = Form("SELECT ConfigID FROM MainRunTable WHERE RunNumber = %d",run);
    if(subrun!=kINVALID_UINT)
      query += Form(" AND SubRunNumber = %d",subrun);
    query += " ORDER BY SubRunNumber DESC LIMIT 1;";
    
    PGresult* res = _conn->Execute(query);

    unsigned int id = kINVALID_UINT;
    if(res && PQntuples(res))
      id = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    if(id == kINVALID_UINT) throw QueryError();
    return id;
  }

  //----------------------------------------------------------------------------------------
  // MainConfigTable functions
  //----------------------------------------------------------------------------------------

  std::vector<std::string> ConfigReader::MainConfigNames()
  {
    std::vector<std::string> config_ids;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute("SELECT * FROM ListMainConfigs();");
    if(!res)  return config_ids;

    for(size_t i=0; i<PQntuples(res); ++i)
      config_ids.push_back(PQgetvalue(res,i,0));

    PQclear(res);
    return config_ids;
  }

  std::vector<unsigned int> ConfigReader::MainConfigIDs()
  {
    std::vector<unsigned int> config_ids;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute("SELECT * FROM ListMainConfigs();");
    if(!res)  return config_ids;

    for(size_t i=0; i<PQntuples(res); ++i)
      config_ids.push_back(atoi(PQgetvalue(res,i,1)));

    PQclear(res);
    return config_ids;
  }

  std::vector<std::pair<std::string,unsigned int> >  ConfigReader::ListSubConfigs(const std::string& cfg_name)
  { return ListSubConfigs(MainConfigID(cfg_name));; }

  std::vector<std::pair<std::string,unsigned int> >  ConfigReader::ListSubConfigs(const unsigned int id)
  {
    if(!ExistMainConfig(id)) {
      Print(msg::kERROR,__FUNCTION__,Form("MainConfig ID %d does not exist!",id));
      throw TableDataError();
    }

    std::vector<std::pair<std::string,unsigned int> > result;
    if(!Connect()) throw ConnectionError();
    std::string cmd("");
    cmd += Form("SELECT * FROM ListSubConfigs(%d);",id);
    PGresult* res = _conn->Execute(cmd);

    if(res && PQntuples(res))
      for(size_t i=0; i<PQntuples(res); ++i)
	result.push_back(std::make_pair(PQgetvalue(res,i,0),
					(unsigned int)(std::atoi(PQgetvalue(res,i,1)))
					)
			 );
    if(res) PQclear(res);
    return result;
  }

  /*
  std::map<std::string,int> ConfigReader::ListSubConfigNameAndID(int run_config_id)
  {
    std::map<std::string,int> config_ids;
    if(!Connect()) return config_ids;
    PGresult* res = _conn->Execute(Form("SELECT ListSubConfigNameAndID('%d');",run_config_id));
    if(!res) return config_ids;

    for(size_t i=0; i<PQntuples(res); ++i) {

      int subconfig_id = atoi(PQgetvalue(res,i,1));
      std::string name = PQgetvalue(res,i,0);

      if(config_ids.find(name) != config_ids.end())

	throw TableDataError();

      else

	config_ids.insert(std::make_pair(name,subconfig_id));
      
    }
    PQclear(res);
    return config_ids;    
  }  
  */

  bool ConfigReader::ExistMainConfig(const std::string& cfg_name)
  {
    bool exist=false;
    if(cfg_name.empty()) return exist;

    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ExistMainConfig('%s');",cfg_name.c_str()));
    if(!res) return exist;

    if(PQntuples(res))
      exist = (std::string(PQgetvalue(res,0,0)) == "t");
    PQclear(res);

    return exist;
  }

  bool ConfigReader::ExistMainConfig(const unsigned int cfg_id)
  {
    bool exist=false;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ExistMainConfig(%d);",cfg_id));
    if(!res) return exist;

    if(PQntuples(res))
      exist = (std::string(PQgetvalue(res,0,0)) == "t");
    PQclear(res);

    return exist;
  }

  unsigned int ConfigReader::MainConfigID(const std::string& cfg_name)
  {
    unsigned int id=0;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT ConfigID FROM MainConfigTable WHERE ConfigName = '%s';",cfg_name.c_str()));

    if(res && PQntuples(res))
      id = std::atoi(PQgetvalue(res,0,0));
    else
      throw QueryError();
    PQclear(res);
    return id;
  }

  std::string ConfigReader::MainConfigName(const unsigned int cfg_id) 
  {
    std::string name("");
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT ConfigName FROM MainConfigTable WHERE ConfigID = %d;",cfg_id));

    if(res && PQntuples(res))
      name = PQgetvalue(res,0,0);
    else
      throw QueryError();
    PQclear(res);
    return name;
  }

  SubConfig ConfigReader::GetSubConfig(const std::string& scfg_name,
				       const unsigned int scfg_id)
  {
    if(!ExistSubConfig(scfg_name,scfg_id)){
      Print(msg::kERROR,__FUNCTION__,
	    Form("SubConfig %s w/ ID %d does not exist!",
		 scfg_name.c_str(),
		 scfg_id)
	    );
      throw QueryError();
    }

    SubConfig result(scfg_name,scfg_id);

    std::string query;

    query = Form("SELECT ParamName, ParamValue FROM ListParameters('%s',%d);",
		 scfg_name.c_str(),
		 scfg_id);
    
    PGresult* res = _conn->Execute(query);

    if(!res) throw QueryError();
      
    for(size_t i=0; i<PQntuples(res); ++i)

      result.Append(PQgetvalue(res,i,0),PQgetvalue(res,i,1));

    PQclear(res);

    query = Form("SELECT SubConfigName, SubConfigID FROM ListParameterSets('%s',%d);",
		 scfg_name.c_str(),
		 scfg_id);

    res = _conn->Execute(query);

    if(!res) throw QueryError();

    std::vector<std::pair<std::string,unsigned int> > name_and_id_v;
    for(size_t i=0; i<PQntuples(res); ++i)

      name_and_id_v.emplace_back(PQgetvalue(res,i,0),std::atoi(PQgetvalue(res,i,1)));

    for(auto const& name_and_id : name_and_id_v)

      result.Append(GetSubConfig(name_and_id.first,name_and_id.second));

    PQclear(res);

    return result;
  }

  MainConfig ConfigReader::GetMainConfig(const std::string& cfg_name)
  {
    if(!ExistMainConfig(cfg_name)){
      Print(msg::kERROR,__FUNCTION__,Form("MainConfig %s does not exist!",cfg_name.c_str()));
      throw QueryError();
    }

    MainConfig result(cfg_name);
    for(auto const& name_id : ListSubConfigs(result.Name()))
      
      result.AddSubConfig(this->GetSubConfig(name_id.first,name_id.second));

    return result;
  }

  //---------------------------------------------------------------------------
  // Sub-Config functions
  //---------------------------------------------------------------------------

  std::vector<std::string> ConfigReader::SubConfigNames()
  {
    std::vector<std::string> result;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute("SELECT SubConfigName FROM ConfigLookUp;");

    if(res && PQntuples(res))
      for(size_t i=0; i<PQntuples(res); ++i)
	result.push_back(PQgetvalue(res,i,0));
    if(res) PQclear(res);
    return result;
  }

  std::vector<unsigned int> ConfigReader::SubConfigIDs(const std::string& name)
  {
    std::vector<unsigned int> result;
    if(!Connect()) throw ConnectionError();
    if(!ExistSubConfig(name)) {
      Print(msg::kERROR,__FUNCTION__,Form("SubConfig: \"%s\" not found!",name.c_str()));
      throw TableDataError();
    }
    PGresult* res = _conn->Execute(Form("SELECT DISTINCT ConfigID FROM %s ORDER BY ConfigID ASC;",name.c_str()));
    if(res && PQntuples(res)){
      for(size_t i=0; i<PQntuples(res); ++i)
	result.push_back(std::atoi(PQgetvalue(res,i,0)));
    }
    return result;
  }

  bool ConfigReader::ExistSubConfig(std::string sub_config_name)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ExistSubConfig('%s');",sub_config_name.c_str()));
    if(!res) return false;

    bool exist=false;
    if(PQntuples(res))
      exist = (std::string(PQgetvalue(res,0,0)) == "t");
    PQclear(res);

    return exist;
  }

  bool ConfigReader::ExistSubConfig(std::string sub_config_name, unsigned int id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT * FROM ExistSubConfig('%s',%d);",sub_config_name.c_str(),id));
    if(!res) return false;

    bool exist=false;
    if(PQntuples(res))
      exist = (std::string(PQgetvalue(res,0,0)) == "t");
    PQclear(res);

    return exist;
  }

  /*
  std::vector<std::string> ConfigReader::GetSubConfigParameterNames(std::string const& sub_config_name)
  {
    std::vector<std::string> params;
    if(!Connect()) return params;
    PGresult* res = _conn->Execute(Form("SELECT ListSubConfigParameters('%s');",sub_config_name.c_str()));
    if(!res) return params;
    
    for(size_t i=0; i<PQntuples(res); ++i)
      params.push_back(PQgetvalue(res,i,0));

    PQclear(res);
    return params;
  }
  */

  int ConfigReader::MaxSubConfigID(std::string sub_config_name)
  {
    int subconfig_id=-2;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT MaxSubConfigID('%s');",sub_config_name.c_str()));
    if(!res) return subconfig_id;

    subconfig_id = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return subconfig_id;
  }
 
}

#endif
