"""
Created on 15.09.2024 10:41:09

Program history
17.10.2024    V. 00.80    init Version with async or threaded uart modul (Author Franke)
22.10.2024    V. 00.81    update function PrintRBRX() to none args

cUART Modul with
cUART Class
- use the internal machine.UART class
- TX callback call not implement yet

@author: S. Franke
"""

__version__ = '00.81'
__author__ = 'S. Franke'

class cUART:
    """
    cUart Class
      - handle UART connection to send and get data with callback functions on RX and TX finish
      - has two unblocked working function for RX and TX and two seperate secondary Ringbuffers
    
    cUART(number=0, baudrate=9600, parity=None, stop=1, buffersize=256, numToReading=1):
    - input value
      number - internal uart number, on rp2 use 0 or 1
      baudrate - uart baudrate in bps
      parity - parity of uart, use None or 0 (even) or 1 (odd)
      stop - number of stop bits, use 1 or 2
      rx - RX Pin for the UART must be a Pin class
      tx - TX Pin for the UART must be a Pin class
      invert - can invert RX and/or TX line, use cUART.INV_RX or cUART.INV_TX or both cUART.INV_RX | cUART.INV_TX
      buffersize - size of the RX and TX ringbuffer
    """
    __Time = __import__("time")
    __txbuf = 64
    __rxbuf = 256
    __hrxbuf = 256//2
    _stdTx = __import__("machine").Pin(16)
    _stdRx = __import__("machine").Pin(17)
    CTS = __import__("machine").UART.CTS
    RTS = __import__("machine").UART.RTS
    INV_RX = __import__("machine").UART.INV_RX
    INV_TX = __import__("machine").UART.INV_TX
    def __init__(self, number=0, baudrate=9600, bits=8, parity=None, stop=1, tx=None, rx=None, invert=0, buffersize=256):
        self.RBRX = None
        self.RBTX = None
        self.callbackRXfull = None
        self.callbackRX = None
        self.isValidRXData = None
        self.callbackTXempty = None
        self.numToReading = 1
        self.numReads = None
        self.trx = None
        self.__uart = None
        self.isRunning = [0,0,]
        self.byteTime = None
        
        UART = __import__("machine").UART
        Pin = __import__("machine").Pin
        Timer = __import__("machine").Timer
        cRB = __import__("Ringbuffer").RINGBUFFER
        Event = __import__("asyncio").Event
        
        # check Pin class for tx
        if type(tx) is not Pin:
            tx = cUART._stdTx
        # check Pin class for rx
        if type(rx) is not Pin:
            rx = cUART._stdRx
        
        # check input buffersize
        buffersize = max(buffersize,self.__rxbuf)
        
        self.RBRX = cRB(buffersize)
        self.RBTX = cRB(buffersize)
        isValidRXData = Event()
        pLen = 0
        buflen = 8
        if parity is not None:
            pLen = 1
        self.byteTime = float(1000*(9+stop+pLen))/float(baudrate)
        # test for big UART buffers
        #self.__rxbuf = buffersize
        #self.__hrxbuf = buffersize//2
        self.__uart = UART(number, baudrate, tx=tx, rx=rx, parity=parity, stop=stop, invert=invert, timeout=int(buflen*self.byteTime), txbuf=self.__txbuf, rxbuf=self.__rxbuf)
        self.numReads = 0
        return
    
    def read(self):
        """
        read function
        read()
        - input value
          None
        - output value
          data as bytearray with .numToReading length
        """
        return self.RBRX.copyGet(self.numToReading)
    
    def read(self,length:int):
        """
        read data function
        read(length)
        - input value
          length - read data with length from receive FIFO
        - output value
          data as bytearray with length of 'length'
        """
        if length >= 1:
            retvar = self.RBRX.copyGet(length)
        else:
            retvar = bytearray(0)
        return retvar
    
    def write(self,data):
        """
        write data function
        write(data)
        - input value
          data - data for send, write to transmission FIFO
        - output value
          write length, is smaller as length from data then FIFO is full
        """
        return self.RBTX.copyPut(data)
    
    def flushRX(self):
        """
        flush the receive FIFO and set number reads to zero
        flushRX()
        - input value
          None
        - output value
          None
        """
        #self.__uart.flush()
        self.RBRX.flush()
        self.numReads = 0
        return
    
    def unblockDo(self,*args):
        """
        unblock working function for RX and TX byte transfer
        use in an infity async or thread function
        function is not blocking the core
        unblockDo()
        - input value
          None
        - output value
          None
        """
        self.unblockRXCollector()
        self.unblockTXWriter()
        return
    
    def unblockTXWriter(self):
        """
        internal unblock working function for TX bytes transfer
        fill the lowlevel uart tx buffer
        unblockTXWriter()
        - input value
          None
        - output value
          None
        """
        if (self.RBTX.chk_for_readdata()) and (self.__uart.txdone()):
            lS = min(self.RBTX.diff_readdata(),self.__txbuf)
            #print(f"write length: {lS}")
            data = self.RBTX.copyGet(lS)
            
            #tS = self.__Time.ticks_us()
            self.__uart.write(data)
            #tE = self.__Time.ticks_us()
            
            #dT = tE-tS
            #print(f"send Time: {dT} us")
            #nB = (float(dT)/1000.0) / self.byteTime
            #print(f"send Time Bytes: {nB} Bytes")
        return
    
    def unblockRXCollector(self):
        """
        internal unblock working function for RX bytes collecting
        read all bytes from the lowlevel uart rx buffer
        unblockRXCollector()
        - input value
          None
        - output value
          None
        """
        #if self.__uart.any()>0:
        #    print(f"uart.any:{self.__uart.any()}")
        #    print("RX status")
        #    print(self.numToReading)
        #    print(self.RBRX.diff_readdata())
        #    print(min(self.numToReading-self.RBRX.diff_readdata(),self.__hrxbuf))
        if self.__uart.any() >= min(self.numToReading-self.RBRX.diff_readdata(),self.__hrxbuf):
            bData = self.__uart.read()
            if bData is not None:
                #print(f"len bData:{len(bData)}")
                self.RBRX.copyPut(bData)
                self.numReads = self.numReads+len(bData)
                #print(f"self.numReads {self.numReads}")
            if self.RBRX.diff_readdata() >= self.numToReading:
                self.numReads = self.numReads-self.numToReading
                if callable(self.callbackRX):
                    data = self.RBRX.copyGet(self.numToReading)
                    #print("run uart callbackRX")
                    self.callbackRX(data)
                #print(f"self.numReads {self.numReads}")
        return
    
    def setCallbackRXFunc(self,func):
        """
        set the RX done callback function
        setCallbackRXFunc(func)
        - input value
          func - function is calling if number of bytes to reading is finish
                 func(data)
                 - input value
                   data - receive data with length .numToReading
        - output value
          None
        """
        if callable(func):
            self.callbackRX = func
        else:
            self.callbackRX = None
        return
    
    def setCallbackTXFunc(self,func):
        """
        set the TX empty callback function (not implement yet)
        setCallbackTXFunc(func)
        - input value
          func - function is calling if number of bytes to reading is finish
                 func()
                 - input value
                   None
        - output value
          None
        """
        if callable(func):
            self.callbackTXempty = func
        else:
            self.callbackTXempty = None
        return
    
    def setBytesToRead(self,num:int):
        """
        set the bytes to reading after this equation the receive callback function is calling
        setCallbackTXFunc(num)
        - input value
          num - number of bytes to read, minimum value is one
        - output value
          None
        """
        self.numToReading = max(num,1)
        return
    
    def deinit(self):
        """
        destructor function
        delete all internal variables
        deinit()
        - input value
          None
        - output value
          None
        """
        print("run uart deinit")
        self.__uart.deinit()
        del self.__uart
        self.RBRX.deinit()
        del self.RBRX
        self.RBTX.deinit()
        del self.RBTX
        if self.trx is not None:
            del self.trx
        del self
        return
    
    def __del__(self):
        """
        alternative destructor function
        delete all internal variables
        __del__()
        - input value
          None
        - output value
          None
        """
        print("run del")
        self.deinit()
        return
    
    def PrintRBRX(self,*args):
        """
        debugger print function for the internal RX FIFO
        PrintRBRX()
        - input value
          None
        - output value
          None
        """
        print(f" uart RX buffer diff_readdata: {self.RBRX.diff_readdata()}")
        print(f" uart RX buffer diff_writedata: {self.RBRX.diff_writedata()}")
        print(f" uart RX buffer index get: {self.RBRX.index_get}")
        print(f" uart RX buffer index put: {self.RBRX.index_put}")
        if len(args) >= 2:
            data = self.RBRX.data[args[0]:args[1]]
        else:
            data = self.RBRX.data[:]
        int_values = [x for x in data]
        print("RBRX data int values:")
        print(int_values)
        return
        
if __name__ == '__main__':
    getThreadID = None
    def test():
        """
        test function for cUART class
        test()
        - input value
          None
        - output value
          None
        """
        Pin = __import__("machine").Pin
        cThread = __import__("Thread").cThread
        cShell = __import__("Shell").cShell
        global st
        st = 0
        def callbackRX(data):
            global st, getThreadID
            if callable(getThreadID):
                print(f"thread ID: {getThreadID()}")
            
            print("Callback RX in Main")
            print(data)
            uart.setBytesToRead(7)
            if st == 0:
                st = 1
                print(uart)
            cl = uart.write(b"0123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567")
            print(f"copy length: {cl}")
            return
        
        def thread0(*args):
            counter = 0
            while args[0][0].is_set():
                pass
            args[0][1].clear()
            return
        
        thread = cThread(abbortButton=Pin(28, Pin.IN), Shell=cShell())
        uart = cUART()
        thread.addNonBlockingCallbackFcn(uart.unblockDo)
        uart.setBytesToRead(1)
        uart.setCallbackRXFunc(callbackRX);
        
        thread.runCores()
        __import__("time").sleep(0.1)
        uart.deinit()
        #del uart
        return
    test()