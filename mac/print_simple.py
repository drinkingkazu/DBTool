from ROOT import ubpsql
from ROOT import TString
import sys
i = ubpsql.ConfigReader()
j = i.RunConfigFromName(sys.argv[1])

a = ubpsql.FhiclMaker()
a.set(j)
s=a.pset().dump()
print s
