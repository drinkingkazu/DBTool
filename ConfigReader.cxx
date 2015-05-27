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

  std::vector<std::string> ConfigReader::RunConfigNames()
  {
    std::vector<std::string> config_ids;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute("SELECT DISTINCT ConfigName,ConfigID FROM MainConfigTable ORDER BY ConfigID DESC;");
    if(!res)  return config_ids;

    for(size_t i=0; i<PQntuples(res); ++i)
      config_ids.push_back(PQgetvalue(res,i,0));

    PQclear(res);
    return config_ids;
  }

  std::vector<unsigned int> ConfigReader::RunConfigIDs()
  {
    std::vector<unsigned int> config_ids;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute("SELECT DISTINCT ConfigID FROM MainConfigTable ORDER BY ConfigID;");
    if(!res)  return config_ids;

    for(size_t i=0; i<PQntuples(res); ++i)
      config_ids.push_back(atoi(PQgetvalue(res,i,0)));

    PQclear(res);
    return config_ids;
  }

  std::vector<std::pair<std::string,unsigned int> >  ConfigReader::SubConfigNameAndIDs(const std::string& cfg_name)
  {
    if(!ExistRunConfig(cfg_name)) {
      Print(msg::kERROR,__FUNCTION__,Form("MainConfig \"%s\" does not exist!",cfg_name.c_str()));
      throw TableDataError();
    }

    std::vector<std::pair<std::string,unsigned int> > result;
    if(!Connect()) throw ConnectionError();
    std::string cmd("");
    cmd += "SELECT B.SubConfigName, A.SubConfigID FROM MainConfigTable AS A JOIN ConfigLookUp AS B ";
    cmd += Form(" ON A.SubConfigType = B.SubConfigType AND A.ConfigName='%s';",cfg_name.c_str());
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

  bool ConfigReader::ExistRunConfig(const std::string& cfg_name)
  {
    bool exist=false;
    if(cfg_name.empty()) return exist;

    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT ConfigID FROM MainConfigTable WHERE ConfigName = '%s';",cfg_name.c_str()));
    if(!res) return exist;

    exist = (PQntuples(res));
    PQclear(res);
    return exist;
  }

  bool ConfigReader::ExistRunConfig(const unsigned int cfg_id)
  {
    bool exist=false;
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT ConfigID FROM MainConfigTable WHERE ConfigID = '%d';",cfg_id));
    if(!res) return exist;

    exist = (PQntuples(res));
    PQclear(res);
    return exist;
  }

  unsigned int ConfigReader::RunConfigID(const std::string& cfg_name)
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

  std::string ConfigReader::RunConfigName(const unsigned int cfg_id) 
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

  RunConfig ConfigReader::RunConfigFromName(const std::string& cfg_name)
  {
    if(!ExistRunConfig(cfg_name)){
      Print(msg::kERROR,__FUNCTION__,Form("MainConfig %s does not exist!",cfg_name.c_str()));
      throw QueryError();
    }

    RunConfig result(cfg_name);
    for(auto const& name_id : SubConfigNameAndIDs(result.Name())) {

      SubConfig d(name_id.first,name_id.second);

      if(!Connect()) throw ConnectionError();
      std::string cmd("");
      cmd += "SELECT Crate,Slot,Channel,each(params) FROM ";
      cmd += Form("GetCrateSlotChannelConfig('%s','%s')",
		  result.Name().c_str(),
		  d.Name().c_str());
      cmd += " ORDER BY Crate, Slot, Channel;";
      PGresult* res = _conn->Execute(cmd);

      if(!res) throw QueryError();

      if(!PQntuples(res)) {
	Print(msg::kERROR,Form("SubConfig (name=\"%s\",id=%d) returned null result!",
			       d.Name().c_str(),
			       d.ConfigID()
			       )
	      );
	throw TableDataError();
      }

      CParamsKey key;
      CParams p;
      std::string pset_name_prefix;
      for(size_t i=0; i<PQntuples(res); ++i) {

	int crate = std::atoi(PQgetvalue(res,i,0));
	int slot  = std::atoi(PQgetvalue(res,i,1));
	int ch    = std::atoi(PQgetvalue(res,i,2));

	CParamsKey tmp_key(crate,slot,ch);
	if(!i) key = CParamsKey(crate,slot,ch);
	else if(key!=tmp_key) {
	  if(p.Name().empty()) {
	    
	    if(key.IsCrate()){
	      if(pset_name_prefix.empty()) p.Name(Form("Crate%02d",key.Crate()));
	      else p.Name(Form("%s%02d",pset_name_prefix.c_str(),key.Crate()));
	    }
	    else if(key.IsSlot()){
	      if(pset_name_prefix.empty()) p.Name(Form("Slot%02d",key.Slot()));
	      else p.Name(Form("%s%02d",pset_name_prefix.c_str(),key.Slot()));
	    }
	    else if(key.IsChannel()){
	      if(pset_name_prefix.empty()) p.Name(Form("Ch%02d",key.Channel()));
	      else p.Name(Form("%s%02d",pset_name_prefix.c_str(),key.Slot()));
	    }
	  }
	  
	  if(p.Name().empty()){
	    std::cout<<"Cannot have an empty name!"<<std::endl;
	    key.ls();
	    throw TableDataError();
	  }
	  d.insert(std::make_pair(key,p));
	  key = tmp_key;
	  pset_name_prefix="";
	  p.clear();
	}

	std::string param = PQgetvalue(res,i,3);
	if( param.size()<3 
	    || param.find('(') != 0 || param.rfind(')') != (param.size()-1)
	    || param.find(',') > param.size() ) {
	  Print(msg::kERROR,__FUNCTION__,Form("Unexpected HSTORE expression: %s",param.c_str()));
	  throw TableDataError();
	}

	param = param.substr(1,param.size()-1);
	std::string param_key   = param.substr(0,param.find(","));
	std::string param_value = param.substr(param.find(",")+1,(param.size()-param.find(",")-2));
	if(param_value.find("\"\"\"")==0)
	  param_value = param_value.substr(2,param_value.size()-2);
	else if(param_value.find("\"")==0)
	  param_value = param_value.substr(1,param_value.size()-1);
	if(param_value.rfind("\"")<param_value.size() && param_value.rfind("\"\"\"")==(param_value.size()-3))
	  param_value = param_value.substr(0,param_value.size()-2);
	else if(param_value.rfind("\"")<param_value.size())
	  param_value = param_value.substr(0,param_value.size()-1);
	/*
	if(param_value.find("\"") == 0) {
	  if(param_value.find("\"\"\"")==0) param_value = param_value.substr(2,param_value.size()-2);
	  else if(param_value.size()>1) param_value = param_value.substr(1,param_value.size()-1);
        }
	if(param_value.rfind("\"")<param_value.size()) {
	  if(param_value.rfind("\"\"\"")==(param_value.size()-3)) param_value = param_value.substr(0,param_value.size()-2);
	  else param_value = param_value.substr(0,param_value.size()-1);
	}
	*/
	size_t pos = param_value.find("\"\"");
	while(pos < param_value.size()) {
	  param_value.replace(pos, 2, "\"");
	  pos = param_value.find("\"\"");
	}

	if(param_key == kPSET_NAME_KEY)
	  p.Name(param_value);
	else if(param_key == kPSET_NAME_PREFIX_KEY)
	  pset_name_prefix = param_value;
	else
	  p.append(param_key,param_value);
	
      }
      
      if(!d.contains(key)) {
	if(p.Name().empty()) {
	  
	  if(key.IsCrate()){
	    if(pset_name_prefix.empty()) p.Name(Form("Crate%02d",key.Crate()));
	    else p.Name(Form("%s%02d",pset_name_prefix.c_str(),key.Crate()));
	  }
	  else if(key.IsSlot()){
	    if(pset_name_prefix.empty()) p.Name(Form("Slot%02d",key.Slot()));
	    else p.Name(Form("%s%02d",pset_name_prefix.c_str(),key.Slot()));
	  }
	  else if(key.IsChannel()){
	    if(pset_name_prefix.empty()) p.Name(Form("Ch%02d",key.Channel()));
	    else p.Name(Form("%s%02d",pset_name_prefix.c_str(),key.Slot()));
	  }
	}
	
	if(p.Name().empty()){
	  std::cout<<"Cannot have an empty name!"<<std::endl;
	  key.ls();
	  throw TableDataError();
	}
	d.insert(std::make_pair(key,p));
      }

      result.AddSubConfig(d);
    }
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
    PGresult* res = _conn->Execute(Form("SELECT ExistSubConfig('%s');",sub_config_name.c_str()));
    if(!res) return false;

    int scfg_id = -1;
    if(PQntuples(res))
      scfg_id = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    if(scfg_id<=0) return false;
    return true;
  }

  bool ConfigReader::ExistSubConfig(std::string sub_config_name, unsigned int id)
  {
    if(!Connect()) throw ConnectionError();
    PGresult* res = _conn->Execute(Form("SELECT ExistSubConfig('%s',%d);",sub_config_name.c_str(),id));
    if(!res) return false;

    bool exist=false;
    if(PQntuples(res))
      exist = std::atoi(PQgetvalue(res,0,0));
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
