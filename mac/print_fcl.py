from ROOT import ubpsql
from ROOT import TString
i = ubpsql.ConfigReader()
j = i.RunConfigFromName("phys_test0");

a = ubpsql.FhiclMaker()
a.set(j)
s=a.pset().dump()
print s
