from UBPyLogger import UBPyLogger

class UBPyBase(object):

    _logger = None

    def __init__(self):
        if not self.__class__._logger:
            self.__class__._logger = UBPyLogger.GetME(self.__class__.__name__)
            UBPyLogger.SetLevelMSG(self.__class__.__name__,1)

        (self.debug, self.info, \
         self.warning, self.error, \
         self.critical) = (self._logger.debug, self._logger.info,\
                           self._logger.warning, self._logger.error,\
                           self._logger.critical)
    

    

    
