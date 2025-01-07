"""
Created on 18.09.2024 11:50:00

Program history
18.09.2024    V. 01.00    init Version with cShell modul (Author Franke)

Shell Modul with
cShell Class
- read the stdin to an buffer and call a callback function if an next line are received
- can use for console input in an running program

@author: S. Franke
"""

__version__ = '01.00'
__author__ = 'S. Franke'

class cShell:
    """
    cShell Class
      - handle UART connection to send and get data with callback functions on RX and TX finish
      - has two unblocked working function for RX and TX and two seperate secondary Ringbuffers
    
    cShell(callback=None, args = ()):
    - input value
      callback - callback(cmd:str, *args) function for call after next line received : function handle
        - input values for the callback function
          cmd - receive string
          *args - additional arguments
      args - get additional arguments for the callback function : tubel
    """
    uselect = __import__("uselect")
    sys = __import__("sys")
    binascii = __import__("binascii")
    endline = "\x0a\x0a"
    callWoClass = lambda obj,cFcn,*args: cFcn(*args)
    
    def __init__(self, callback=None, args = ()):
        # init class Variables to default
        self.spoll = None
        self.cmdBuffer = ""
        self.callback = None
        self.args = ()
    
        self.spoll = self.uselect.poll()
        self.spoll.register(self.sys.stdin, self.uselect.POLLIN)
        if callable(callback):
            self.callback = callback
            self.args = args
        else:
            self.callback = self.callbackShell
        return
    
    def __readChar(self):
        """
        internal read function for one char
        __readChar()
        - input value
          None
        - output value
          str | None
        """
        return(self.sys.stdin.read(1) if self.spoll.poll(0) else None)

    def readStdinNonBlocking(self,*args):
        """
        Non blocking work function for reading the chars from the sys.stdin, 
        if endline receive run the saved or the internal callback function.
        Using in an work loop.
        
        readStdinNonBlocking(*args)
        - input values
          *args - arguments from the thread or async call
        - output value
          None
        """
        try:
            c = self.__readChar()
        except:
            c = None
        if c != None:
            self.cmdBuffer += c
            cB = self.binascii.hexlify(self.cmdBuffer).decode()
            if self.endline == self.cmdBuffer[-2:]:
                ret = self.cmdBuffer[:-2]
                self.cmdBuffer = ""
                if callable(self.callback):
                    argsO = self.args+args
                    self.callback(ret,*argsO)
                    pass
        return
    
    @staticmethod
    def callbackShell(cmd,*args):
        """
        Standard shell callback function.
        Run command (cmd) with exec function.
        
        callbackShell(cmd,*args)
        - input values
          cmd - command to run with exec
          *args - saved arguments (not used)
        - output value
          None
        """
        if cmd != None:
            sleep = __import__("time").sleep
            try:
                exec(cmd+"\n")
            except:
                print("wrong insert")
            sleep(0.01)
        return
    
if __name__ == '__main__':
    def Test():
        Pin = __import__("machine").Pin
        cThread = __import__("Thread").cThread
        cShell = __import__("Shell").cShell
        
        def callbackShell(cmd,*args):
            if cmd != None:
                print(f"cmd: {cmd}")
            return
        abbortButton = Pin(28, Pin.IN)
        
        shell = cShell()
        thread = cThread(None,(),None,(),abbortButton,shell)
        
        thread.runCores()
        return
    Test()