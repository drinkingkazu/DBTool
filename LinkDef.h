//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace ubpsql;
#pragma link C++ namespace ubpsql::msg;

#pragma link C++ class ubpsql::DBException+;
#pragma link C++ class ubpsql::InvalidUser+;
#pragma link C++ class ubpsql::ConnectionError+;
#pragma link C++ class ubpsql::ConfigError+;

#pragma link C++ class ubpsql::DBBase+;
#pragma link C++ class ubpsql::DBAppBase+;
#pragma link C++ class ubpsql::PQMessenger+;
#pragma link C++ class ubpsql::DBConn+;

#pragma link C++ class ubpsql::ConfigReader+;
#pragma link C++ class ubpsql::ConfigWriter+;
#pragma link C++ class std::map<string,string>+;
#pragma link C++ class ubpsql::ConfigParams+;
#pragma link C++ class std::pair<std::set<ubpsql::ConfigParams>::iterator,bool>+;
#pragma link C++ class std::set<ubpsql::ConfigParams>+;
#pragma link C++ class ubpsql::ConfigData+;
#pragma link C++ class std::vector<ubpsql::ConfigData>+;
#pragma link C++ class ubpsql::RunConfig+;
//ADD_NEW_CLASS ... do not change this line
#endif







