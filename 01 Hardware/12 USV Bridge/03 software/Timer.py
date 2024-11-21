class cTimer:
    """ Wrapper class cTimer for the machine.Timer class with start and stop functions.
        
    """
    ONE_SHOT = int(0)
    PERIODIC = int(1)
    
    def __init__(self, mode:int=1, freq=-1, period=-1, numToRuns:int=-1, callback=None, args=(), start:bool=True):
        """ Constructor for class cTimer
            cTimer(mode=1, freq=-1, period=-1, numToRuns=-1, callback=None, args=())
                mode - periodic (1) or oneshot (0)
                freq - value for frequency mode in Hz (1/s), only periodic mode
                period - value for the periode mode in ms, use with one shot mode
                numToRuns - set value to run specified nuber of calls
                callback - callback function for call after trigger
                args - user arguments for the callback functions
                start - start the timer with the constructor
            return cTimer object
        """
        self.__eTimer = __import__("machine").Timer
        self.__sleep = __import__("time").sleep
        self.__iTimer = None
        
        self.mode = mode
        self.freq = freq
        self.period = period
        self.numRuns = 0
        if mode == self.ONE_SHOT:
            self.numToRuns = 1
        else:
            self.numToRuns = numToRuns
        
        if not callable(callback):
            raise(Exception("Parameter 'callback' must be an callable function."))
        
        self.__Callback = callback
        self.__args = args
        
        self.stopAtError = True
        
        if start:
            self.start()
        
        return
    
    def start(self):
        self.stop()
        self.numRuns = 0
        if (self.period > 0) and (self.freq == -1):
            self.__iTimer = self.__eTimer(mode=self.mode, period=int(self.period), callback=self.__TimerCallback)
        elif (self.freq > 0) and (self.period == -1):
            self.__iTimer = self.__eTimer(mode=self.PERIODIC, freq=int(self.freq), callback=self.__TimerCallback)
        else:
            self.__iTimer = self.__eTimer(mode=self.mode, period=1000, callback=self.__TimerCallback)
    
    def stop(self):
        if self.__iTimer is not None:
            self.__iTimer.deinit()
            self.__iTimer = None
        return
    
    def deinit(self):
        if self.__iTimer is not None:
            self.__iTimer.deinit()
        del self
        return
        
    def __TimerCallback(self,*args):
        self.numRuns = self.numRuns+1
        if (self.numRuns >= self.numToRuns) and (self.numToRuns > 0):
            self.stop()
        if callable(self.__Callback):
            a = self.__args
            try:
                self.__Callback(*a)
            except Exception as e:
                sys = __import__("sys")
                print("Error in Timer callback function!")
                sys.print_exception(e)
                if self.stopAtError:
                    print("And stop Timer!")
                    self.stop()
            except:
                print("Error in Timer callback function!")
                if self.stopAtError:
                    print("And stop Timer!")
                    self.stop()
        self.__sleep(0.0001)
        return
    
if __name__ == '__main__':
    t2 = cTimer(period=1500, mode=cTimer.ONE_SHOT, callback=lambda *args:print("timer 2"))