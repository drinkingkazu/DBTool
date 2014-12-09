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
    PGresult* res = _conn->Execute("SELECT DISTINCT ConfigName FROM MainConfigTable ORDER BY ConfigID;");
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

  std::vector<std::string> ConfigReader::SubConfigNames(const std::string &cfg_name)
  {
    auto const name_ids = SubConfigNameAndIDs(cfg_name);
    std::vector<std::string> result;
    result.reserve(name_ids.size());
    for(auto const& iter : name_ids) result.push_back(iter.first);
    return result;
  }

  std::vector<unsigned int> ConfigReader::SubConfigIDs(const std::string &cfg_name)
  {
    auto const name_ids = SubConfigNameAndIDs(cfg_name);
    std::vector<unsigned int> result;
    result.reserve(name_ids.size());
    for(auto const& iter : name_ids) result.push_back(iter.second);
    return result;
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

      ConfigData d(name_id.first,name_id.second);

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

      ConfigParams p(d.Name());
      for(size_t i=0; i<PQntuples(res); ++i) {

	int crate = std::atoi(PQgetvalue(res,i,0));
	int slot  = std::atoi(PQgetvalue(res,i,1));
	int ch    = std::atoi(PQgetvalue(res,i,2));

	ConfigParams tmp(d.Name(),crate,slot,ch);
	if(!i) p = ConfigParams(d.Name(),crate,slot,ch);
	else if(p!=tmp) {
	  d.insert(p);
	  p = tmp;
	}
	std::string param = PQgetvalue(res,i,3);
	if( param.size()<3 
	    || param.find('(') != 0 || param.rfind(')') != (param.size()-1)
	    || param.find(',') > param.size() ) {
	  std::cout<<param.find('(')<<" : "<<param.rfind(')')<<" : "<<param.size()<< " : "<<param.find(",")<<std::endl;
	  Print(msg::kERROR,__FUNCTION__,Form("Unexpected HSTORE expression: %s",param.c_str()));
	  throw TableDataError();
	}

	param = param.substr(1,param.size()-1);
	std::string key   = param.substr(0,param.find(","));
	std::string value = param.substr(param.find(",")+1,(param.size()-param.find(",")-2));
	if(value.find("\"\"\"")==0) value = value.substr(2,value.size()-2);
	if(value.rfind("\"\"\"")==(value.size()-3)) value = value.substr(0,value.size()-2);
	p.append(key,value);

      }
      
      if(!d.contains(p)) d.insert(p);

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
