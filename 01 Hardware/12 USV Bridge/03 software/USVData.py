class cUSVData:
    callWoClass = lambda obj,cFcn,*args: cFcn(*args)
    blockLen = __import__("micropython").const(1024)
    CRCPoly = __import__("micropython").const(b'\xD5')
    cTimer = __import__("Timer").cTimer
    time = __import__("time")
    
    def __init__(self, uart, extRBRX, Callback = None, args = (), printFcn=None):
        # init class Variables to default
        self.eprint = print
        self.init = False
        self.uart = None
        self.TimOut = None
        self.CRC = None
        self.state = 10
        self.extRBRX = None
        self.readNum = 0
        self.Callback = None
        self.args = ()
        self.TimOutAgs = {"period": 5000, "mode": self.cTimer.ONE_SHOT, "callback": self.callbackTimOut}
        self.print = self.dummyPrint
        self.numFalseStateOnStart = 0
        if printFcn is not None:
            if callable(printFcn):
                self.print = printFcn
        
        # run constuctor
        try:
            cUART = __import__("cUART").cUART
        except:
            cUART = None
        try:
            thUART = __import__("thUART").cUART
        except:
            thUART = None
        cRB = __import__("Ringbuffer").RINGBUFFER
        cTim = __import__("machine").Timer
        typUart = type(uart)
        if ((typUart == cUART) or (typUart == thUART))and (type(extRBRX) == cRB):
            self.uart = uart
            self.uart.flushRX()
            self.uart.setBytesToRead(1)
            self.uart.setCallbackRXFunc(self.callbackRX);
            self.extRBRX = extRBRX
            self.CRC = __import__("CRC8").cCRC8(self.CRCPoly)
            if callable(Callback):
                self.Callback = Callback
                self.args = args
            self.init = True
        else:
            raise(Exception(f"No class cUART get class {type(uart)} or/and no class RINGBUFFER get class {type(extRBRX)} are commited"))
        return
    
    def callbackRX(self, data, *args):
        print = self.print
        time = self.time
        #print("receive Frame")
        #print(data)
        #print(f"state: {self.state}")
        #print(f"len(data): {len(data)}")
        if data is not None:
            if self.state == 0: # state for wait start byte
                if data[0] == 0xA5:
                    print(f"state: {self.state}")
                    if self.readNum == 0:
                        print(f"--USV Block first frame--   lt: {time.localtime()[0:6]}")
                    print("-USV frame Start-")
                    self.uart.setBytesToRead(4)
                    self.state = 1
                elif data[0] == 0xA2:
                    print(f"state: {self.state}")
                    print("USV get NACK")
                    self.state = 10
            elif self.state == 1: # state for get header from frame
                print(f"state: {self.state}")
                ID = data[0]
                addr = data[1:2]
                fLen = data[3]
                print(f"USV ID: {ID}")
                print(f"USV Frame length: {fLen}")
                self.uart.setBytesToRead(fLen-5)
                self.state = 2
            elif self.state == 2: # state get header from frame
                print(f"state: {self.state}")
                if data[-1] == 0xA6:
                    CRC = data[-1]
                    print(f"-Data length: {len(data[:-2])}")
                    self.extRBRX.copyPut(data[:-2])
                    self.readNum = self.extRBRX.diff_readdata()
                    print(f"-readNum {self.readNum}")
                    if self.readNum < self.blockLen:
                        frameLen = min(248,self.blockLen-self.readNum)
                        print(f"-data remain length: {self.blockLen-self.readNum}")
                        print(f"-New Frame with data length: {frameLen}")
                        GetFrame = self.createGetFrame(self.readNum,frameLen)
                        print("-USV frame end detected-")
                        print(f"-Get new Frame! with length: {frameLen}-")
                        print(''.join('{:02x} '.format(x) for x in GetFrame))
                        self.uart.flushRX()
                        self.uart.setBytesToRead(1)
                        self.uart.write(GetFrame)
                        self.state = 0
                    else:
                        print("-USV frame end detected-")
                        self.uart.flushRX()
                        self.uart.setBytesToRead(1)
                        self.state = 10
                        print(f"--USV Block get final--    lt: {time.localtime()[0:6]}")
                        if callable(self.Callback):
                            a = self.args
                            self.callWoClass(self.Callback,*a)
                else:
                    print("-NO USV frame end detected-")
                    self.uart.flushRX()
                    self.uart.setBytesToRead(1)
                    self.state = 10
            elif self.state == 10:
                print(f"state: {self.state}")
                pass
            else:
                print(f"state: {self.state}")
                pass
        #print(f"new state: {self.state}")
        return
    
    def callbackTimOut(self,*args):
        print = self.print
        print("USV time out detected")
        self.uart.flushRX()
        self.uart.setBytesToRead(1)
        self.state = 10
        return
    
    def startBlockRequest(self):
        if self.init:
            print = self.print
            if self.state == 10:
                print("--USV start Block request--")
                self.numFalseStateOnStart = 0
                self.readNum = 0
                self.state = 0
                frameLen = min(248,self.blockLen-self.readNum)
                GetFrame = self.createGetFrame(0,frameLen)
                l = self.uart.RBRX.diff_readdata()
                if l>0:
                    print(f"-USV Frame not all Data reading: l={l}--")
                    #self.uart.PrintRBRX()
                    self.extRBRX.flush()
                    #raise(Exception("Error in previous USV Frame!"))
                self.uart.flushRX()
                self.uart.setBytesToRead(1)
                self.uart.write(GetFrame)
            else:
                self.eprint("--USV start Block request not ready--")
                self.numFalseStateOnStart += 1
                if self.numFalseStateOnStart >3:
                    self.eprint(f"--USV start Block request not ready--\n\tstate: {self.state}\n\tnumFalseStateOnStart: {self.numFalseStateOnStart}")
                if self.numFalseStateOnStart >10:
                    self.eprint(f"--hard USV start Block request--\n\t set state to 10")
                    self.state = 10
        return
    
    def callbackTimer(self,*args):
        print = self.print
        return
    
    def createGetFrame(self, adress, length):
        print = self.print
        GetFrame = bytearray(b'\xA5\x00\x00\x40\x08\x00\x00\xA6')
        adress = min(adress,4095)
        adress = adress.to_bytes(2,'little')
        length = min(length,248)
        GetFrame[2] = adress[0]
        GetFrame[3] = GetFrame[3] | adress[1]
        #print(f"addr low: {GetFrame[2]}")
        #print(f"addr high: {GetFrame[3]}")
        GetFrame[5] = length
        GetFrame[6] = self.CRC.calc([length])
        return GetFrame
    
    def dummyPrint(self,*args):
        return
    
if __name__ == '__main__':
    a = 2
    #def test(USVData):
    Pin = __import__("machine").Pin
    cThread = __import__("Thread").cThread
    cShell = __import__("Shell").cShell
    cUART = __import__("cUART").cUART
    cRB = __import__("Ringbuffer").RINGBUFFER
    #cUSVData = __import__("USVData").cUSVData
    Event = __import__("asyncio").Event
    
    def callbackShell(cmd,*args):
        if cmd != None:
            sleep = __import__("time").sleep
            print(f"Thread ID: {getThreadID()}")
            if cmd == "s":
                USVData.startBlockRequest()
            elif cmd == "g":
                print(f"numReads: {USVData.uart.numReads}\n numToReading: {USVData.uart.numToReading}")
            else:
                try:
                    exec(cmd+"\n")
                    print("->> ")
                except:
                    print("wrong insert")
                sleep(0.01)
        return
    
    def callbackBlockFinal(EvBF,*args):
        print(f"Thread ID: {getThreadID()}")
        EvBF.set()
        return
    
    def th0(EvTh,EvBF,*args):
        RXData = args[0]
        while EvTh[0].is_set():
            if len(args) > 0:
                if EvBF.is_set():
                    print("call block final in main")
                    print("________________________")
                    data = RXData.copyGet(-1)
                    print(len(data))
                    int_values = [x for x in data]
                    print(int_values)
                    EvBF.clear()
                
        return
    
    evGetBlock = Event()
    
    RXData = cRB(2049)
    thread = cThread(th0, (evGetBlock,RXData,), abbortButton=Pin(28, Pin.IN), Shell=cShell(callbackShell))
    uart = cUART(thread,baudrate=250000, parity=1, buffersize=300)
    USVData = cUSVData(uart,RXData, Callback=callbackBlockFinal,args=(evGetBlock,))
    thread.runCores()
    #return USVData
    uart.deinit()
    RXData.deinit()
    #debug command: USVData.uart.PrintRBRX(0,261)