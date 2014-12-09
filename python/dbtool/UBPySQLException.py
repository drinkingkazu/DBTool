class UBPyException(Exception):

    def __init__(self): pass
        
    def __str__(self):
        return self.wrap("Generic UBPyException")

    def _wrap_(self,msg):
        return "\033[93m[EXCEPTION]\033[00m %s" % msg

class UBPySQLException(UBPyException):

    def __init__(self): pass
        
    def __str__(self):
        return self.wrap("Generic SQL Exception")

class ParseIException(UBPyException):

    def __init__(self): pass
        
    def __str__(self):
        return self.wrap("Failure parsing an input file")

class UploadException(UBPyException):

    def __init__(self): pass
        
    def __str__(self):
        return self.wrap("Failure uploading configuration")
