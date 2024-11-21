'''
Created on 09.10.2024 12:08:09

Program history
02.10.2024    V. 00.10    init Version asyncClass for none blocking functions (Author Franke)

asyncClass Modul with
asyncClass Class
- use the internal machine.UART class
- TX callback call not implement yet
- TODO deinit function

@author: S. Franke
'''

__version__ = '00.10'
__author__ = 'S. Franke'

class asyncClass():
    """
    asyncClass Class
      - run as async thread function with the member function do
      - handle configurable callback functions with arguments, who runs in a row without break in the member function do
    
    asyncClass(ID:str, waitTime:int=100):
    - input value
      ID - set the ID for the external event for running : String
      waitTime - loop wait time in ms : Integer
    """
    def __init__(self, ID:str, waitTime:int=100):
        self.ID = ID
        self.waitTime = waitTime
        self.callbacks = []
        self.args = []
    
    def append(self, CallbackFcn, args=()):
        """
        append a unblocked callback function to running in the internal async loop
        append(CallbackFcn, args=())
        - input value
          CallbackFcn - unblocked Callback function
          args - Arguments for the callback function
        - output value
          None
        """
        if callable(CallbackFcn):
            self.callbacks.append(CallbackFcn)
            self.args.append(args)
    
    async def do(self, evS, *args):
        """
        async do function for running all unblocked callback functions in a row
        do(evS, *args)
        - input value
          evS - a directory with asyncio.Event objects identified with the Name ID whos configurated in the constuctor
          *args - dummy variable for unused transfer parameter
        - output value
          None
        """
        asyncio = __import__("asyncio")
        _thread = __import__("_thread")
        Event = __import__("asyncio").Event
        
        def printVar(name:str,var):
            print(f"{name}: {var}")
            
        def g_tick(period:int):
            import time
            t = time.ticks_ms()
            while True:
                t += period
                yield max(t - time.ticks_ms(),0)
        
        isKey = False
        for k in evS.keys():
            if k == self.ID:
                isKey = True
                break
        if isKey:
            if type(evS[self.ID]) != Event:
                raise(Exception(f"Variable evS[{self.ID}] is not a Event class"))
        else:
            raise(Exception(f"Key ({self.ID}) not found in evS"))
        
        a_lock = _thread.allocate_lock()
        g = g_tick(self.waitTime)
        while evS[self.ID].is_set():
            with a_lock:
                l = len(self.callbacks)
                for i in range(l):
                    a = self.args[i]
                    callback = self.callbacks[i]
                    callback(*a)
            await asyncio.sleep_ms(next(g))
            #i = j #create an FATAL: uncaught exception despite try except block
        asyncio.sleep(0.1)
        return
