#ifndef DBTOOL_DEATHSTAR_CXX
#define DBTOOL_DEATHSTAR_CXX

#include "DeathStar.h"

namespace ubpsql {

  int DeathStar::CreateSubConfiguration(const SubConfig& cfg)
  {
    if(this->ExistSubConfig(cfg.ID().Name())) {
      Print(msg::kWARNING,__FUNCTION__,
	    Form("SubConfig \"%s\" already exists...",cfg.ID().Name().c_str()));
      return -1;
    }

    PGresult* res = _conn->Execute(Form("SELECT * FROM CreateSubConfiguration('%s');",cfg.ID().Name().c_str()));
    if(!res) return -1;
    int id = std::atoi(PQgetvalue(res,0,0));
    PQclear(res);
    return id;
  }

  void DeathStar::SuperBeam()
  {
    if(!Connect()) throw ConnectionError();
    Print(msg::kWARNING,__FUNCTION__,Form("Death Star is completed. Attempting to destroy Endor via SuperBeam."));
    PGresult* res = _conn->Execute("SELECT CleanConfigDB();");
    Print(msg::kWARNING,__FUNCTION__,Form("Bahahahaha!"));
    if(res) PQclear(res);
    return;
  }

}
#endif
