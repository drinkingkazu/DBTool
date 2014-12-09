from ROOT import ubpsql

i = ubpsql.ConfigReader()
j = i.RunConfigFromName("phys_test0");
k = j.Get("pmt_config")
k.ls()
