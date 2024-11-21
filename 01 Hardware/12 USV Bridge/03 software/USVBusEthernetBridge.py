"""
Created on 18.10.2024 19:00:04

Program history
18.10.2024    V. 01.00    init Version with async thread functions (Author Franke)
21.11.2024    V. 01.10    add host config over json file


USVBusEthernetBridge Modul with
USVBusEthernetBridge function and
 - async programm function to sen USV Bus data over ethernet
run function
 - run USVBusEthernetBridge function as async threads
 - use WLAN object to detect if is connected with the ethernet

@author: S. Franke
"""

__version__ = '01.10'
__author__ = 'S. Franke'

async def USVBusEthernetBridge(*args):
    """
    working function for the data communication with a stack of internally defined functions
    
    USVBusEthernetBridge(*args)
    - input value
      *args - not used dummy variable
    - output value
      None
    """
    def time_ms():
        """
        get time in milli seconds
        
        time_ms()
        - input value
          None
        - output value
          time in milli seconds as integer: int
        """
        #return ((time.time_ns()>>20)*81920)//78125
        return ((time.time_ns()>>7)*10)//78125
    
    def g_tick(period:int):
        """
        generator function to calculate the difference tick time
          - calculate the difference from the periode and the the current tick time
          - minimal value is 0 ms
        g_tick(period:int)
        - input value
          period - init value for the periode : int
        - output value
          difference tick time : int
        """
        time = __import__("time")
        t = time.ticks_ms()
        while True:
            t += period
            yield max(t - time.ticks_ms(),0)
        return
    
    def printVar(name:str,var):
        """
        formating print function
            - print variable with it name
        
        printVar(name:str,var)
        - input value
          name - variable name : str
          var - variable itself
        - output value
          None
        """
        print(f"{name}: {var}")
        return
    
    def stopAll(evS:Event):
        """
        stop all threads
          - clear all running threads events
        
        stopAll(evS:Event)
        - input value
          evS - event array : Event
        - output value
          None
        """
        for ev in evS:
            evS[ev].clear()
        return
    
    def abbortButtonCallback(*args):
        """
        callback function for the abbort button
          - clear all running threads events with the stopAll function
        
        abbortButtonCallback(*args)
        - input value
          *args - not used dummy variable
        - output value
          None
        """
        printVar('args',args)
        stopAll(evRun)
        return
    
    def shellCallback(cmd,*args):
        """
        callback function for shell input
          - check if stop are received and stop all threads
        
        shellCallback(cmd,*args)
        - input value
          cmd - received command : str
          *args - not used dummy variable
        - output value
          None
        """
        global ts
        
        evRun = args[0]
        USVData = args[1]
        
        if cmd != None:
            if cmd == "stop":
                stopAll(evRun)
            elif cmd == "start":
                ts[0] = time.ticks_us()
                ts[1] = ts[0]
                USVData.startBlockRequest()
            else:
                printVar('cmd',cmd)
        return
    
    async def thSendData(evS,*args):
        """
        async generator function for send data over TCP socked conection
          - get syncronisation and Data from USVData Thread
          - handle connection and disconnection to the server with specified address
        
        thSendData(evS,*args)
        - input value
          evS - event directionary from all threads: directionary
          *args[0] - event get uart data block : Event
          *args[1] - uart RX data block : RINGBUFFER
          *args[2] - server address : str
        - output value
          None
        """
        global ts
        
        if len(args) != 3:
            raise(Exception(f"Numbers of arguments are wrong in thread thSendData! (args: {args})"))
        
        Event = __import__("asyncio").Event
        cRB = __import__("Ringbuffer").RINGBUFFER
        cSocket = __import__("socket").socket
        print_exception = __import__("sys").print_exception
        cBase = __import__("printFormating").Base
        printCol = __import__("printFormating").printCol
        time = __import__("time")
        
        evGetBlock = args[0]
        RXData = args[1]
        addresse = args[2]
        
        if (type(evGetBlock) is not Event):
            raise(Exception(f"Argument 0 are wrong class (args[0]: {type(args[0])})"))
        if (type(RXData) is not cRB):
            raise(Exception(f"Argument 1 are wrong class (args[1]: {type(args[1])})"))
        
        g = g_tick(100)
        
        TCPheaderPut = bytearray(b'\xA5\xF1\x00\x80\xFF\xFF\x00\xA6')
        TCPheaderGet = bytearray(b'\xA5\xF1\x00\x40\x08\x00\x00\xA6')
        
        evNewData = Event()
        evSockCon = Event()
        
        dataArr = []
        counter = 0
        
        
        s = None
        try:
            while evS["thSD"].is_set():
                if evGetBlock.is_set():
                    dt = time.ticks_us()-ts[0]
                    print(f"UART get time: {dt} us")
                    
                    ts[0] = time.ticks_us()
                    if not evNewData.is_set():
                        dataArr = RXData.copyGet(-1)
                        evNewData.set()
                    else:
                        print("---------------old data not deleted")
                        RXData.flush()
                    evGetBlock.clear()
                    
                    dt = time.ticks_us()-ts[0]
                    print(f"copy time: {dt} us")
                
                if (not evSockCon.is_set()) and evNewData.is_set() and (wlan.state() == 3):
                    try:
                        print("create socket")
                        s = cSocket()
                        s.settimeout(0.7)
                        s.connect(addresse)
                        evSockCon.set()
                    except Exception as e:
                        print_exception(e)
                        printCol(f"error in socket creation 0: '{e}'",cBase.FAIL)
                        evSockCon.clear()
                    except:
                        printCol("error in socket creation 0",cBase.FAIL)
                        evSockCon.clear()
                
                await asyncio.sleep_ms(30)
                if evNewData.is_set() and evSockCon.is_set() and (wlan.state() == 3):
                    ts[0] = time.ticks_us()
                    #print(len(dataArr))
                    
                    datalen = len(dataArr).to_bytes(2, 'little') 
                    TCPheaderPut[4] = datalen[0]
                    TCPheaderPut[5] = datalen[1]
                    #print(TCPheaderPut)
                    try:
                        #s = cSocket()
                        #s.settimeout(1)
                        #s.connect(addresse)
                        #print("send header")
                        s.sendall(bytes(TCPheaderPut+dataArr))
                        data = s.recv(1)
                        data = b'\xA1'
                        #print(f"receive answer: '{data}'")
                        if data == b'\xA1':
                            print(f"receive ACK")
                            pass
                        elif data == b'\xA2':
                            print('NACK from Server')
                            sendErrorCount += 1
                        else:
                            print('error')
                            sendErrorCount += 1
                        
                        #print(f"receive answer: '{data}'")
                    except Exception as e:
                        print_exception(e)
                        printCol(f"error in socket connection: '{e}'",cBase.FAIL)
                        evSockCon.clear()
                    except:
                        printCol("error in socket connection",cBase.FAIL)
                        evSockCon.clear()
                    finally:
                        del(dataArr)
                        evNewData.clear()
                        #s.close()
                        #del(s)
                        t = time.ticks_us()
                        dt = t-ts[0]
                        print(f"send time: {dt} us")
                        dt = t-ts[1]
                        print(f"overall time: {dt} us")
                        #print("________________________")
                if evNewData.is_set():
                    del(dataArr)
                    evNewData.clear()
                
                if not evSockCon.is_set():
                    if s is not None:
                        print("delete socket")
                        s.close()
                        del(s)
                        s = None
                
                await asyncio.sleep_ms(next(g))
        finally:
            if evSockCon.is_set():
                if s is not None:
                    print("delete socket")
                    s.close()
                    del(s)
                    s = None    
        return
    
    def callbackBlockFinal(EvBF,*args):
        """
        callback function for the finishing from the received USV Block
          - call from the USV-Data object, it is running in another thread
        
        callbackBlockFinal(EvBF,*args)
        - input value
          EvBF - event for block final : Event
        - output value
          None
        """
        EvBF.set()
        return
    
    def callbackTimer(USVData,*args):
        """
        callback function for the timer, who startet the block request from the USV slave
          - call from the USV-Data object, it is running in another thread
        
        callbackBlockFinal(EvBF,*args)
        - input value
          USVData - USVData class object for start the request : USVData
          *args - 
        - output value
          None
        """
        global ts, wlan
        time = __import__("time")
        ts[0] = time.ticks_us()
        ts[1] = ts[0]
        
        if wlan.state() == 3:
            print("--start block request--")
            USVData.startBlockRequest()
        return
    
    """
    --- function body for USVBusEthernetBridge ---
    """
    #global ts, uart, RXData, evRun, USVData
    global ts, wlan
    
    # check for WLAN object
    print("In USVBusEthernetBridge get WLAN object.")
    if type(wlan) == __import__("WLAN").WLAN:
        print("\t",wlan)
    else:
        raise(Exception("No WLAN object found!\n\tAbbort USVBusEthernetBridge program!"))
    
    # import bibs
    Pin = __import__("machine").Pin
    sleep = __import__("time").sleep
    cUART = __import__("cUART").cUART
    asyncClass = __import__("asyncClass").asyncClass
    cShell = __import__("Shell").cShell
    cRB = __import__("Ringbuffer").RINGBUFFER
    cUSVData = __import__("USVData").cUSVData
    cTimer = __import__("Timer").cTimer
    asyncio = __import__("asyncio")
    Event = asyncio.Event
    json = __import__("json")
    socket = __import__("socket")
    
    print("Run USV Ethernet Bridge program.\n")
    
    host = None
    port = None
    
    # load Host info from json file
    config = None
    with open("./BridgeHost.json") as fid:
        config = json.load(fid)
    
    if config is not None:
        print("Config file reading:\n",config)
        
        if 'SockedPort' in config.keys():
            port = config['SockedPort']
        else:
            port = 8880              # default Port used by server
        
        if 'IPAddress' in config.keys():
            host = config['IPAddress']
        
        if 'HostDNSName' in config.keys():
            # get DNS name
            print("Get DNS name!")
            DNSName = config['HostDNSName']
            try:
                # resolve IP from DNS name
                adressInfo = socket.getaddrinfo(DNSName, port)[0][-1]
                print(f"DNS resolved Address information is: {adressInfo}.")
                host = adressInfo[0]
                port = adressInfo[1]
            except:
                print(f"DNS Address {DNSName} not resolved.")
                pass
        
    # set the host ip address and the host tcp port de default
    if host is None:
        host = "192.168.188.21"      # IP adress of the Server
    
    if port is None:
        port = 8880                  # Port used by server
    
    print(f"Set Host IP to {host} and Host port to {port}!\n")
    
    
    # set debug time stamps
    ts = [0,0,]

    # create the socket connect event
    evSockCon = Event()
    sendErrorCount = 0
    
    evRun = {"thSD":Event(), "asyncClass": Event()}
    for ev in evRun:
        evRun[ev].set()
        
    abbortButton = Pin(28, Pin.IN)
    
    # create the get USV Data block event
    evGetBlock = Event()
    # create USV Data buffer as a RINGBUFFER class object
    RXData = cRB(2049)
    
    # create the async class to handle all unblocking working functions
    thNonBlocking =  asyncClass(ID="asyncClass",waitTime=10)
    
    # create the uart for communication to USV-Slave
    uart = cUART(baudrate=250000, parity=1, buffersize=300)
    # an create the USVData object, whitch handle the communication over the uart to the USV-Slave and decode the protocol data
    USVData = cUSVData(uart, RXData, Callback=callbackBlockFinal, args=(evGetBlock,RXData,),printFcn=None)
    
    # create shell class object and link it to the callback function
    shell = cShell(shellCallback, args=(evRun,USVData,))
    
    # add the unblock working functions to the async class object
    thNonBlocking.append(uart.unblockRXCollector)
    thNonBlocking.append(uart.unblockTXWriter)
    thNonBlocking.append(shell.readStdinNonBlocking)
    #printVar("callbacks",thNonBlocking.callbacks)
    
    # create generators for async threads
    thSD = thSendData(evRun,evGetBlock,RXData,(host,port),)
    do = thNonBlocking.do(evRun)
    
    # print the generators to debug it
    printVar("thSD",thSD)
    printVar("do",do)
    
    # set the callback function for the abbort button
    abbortButton.irq(trigger = Pin.IRQ_RISING, handler = abbortButtonCallback)
    
    # create the timer class object to start the cycle request every 1 second
    Timer = cTimer(period=1000, callback=callbackTimer, args=(USVData,),start=True)
    
    # create the timer 2 class object to test something
    #Timer2 = cTimer(period=1000, callback=lambda *args:print("--call Timer 2--"), start=True)
    
    print("Start all threads!")
    try:
        
        # start all threads
        task1 = asyncio.create_task(thSD)
        task2 = asyncio.create_task(do)
        
        print("All threads running!")
        # wait for final of all threads
        await task1
        await task2
    
    finally:
        # after run of all threads, deinit all class objects
        abbortButton.irq(handler = None)
        uart.deinit()
        RXData.deinit()
        Timer.deinit()
        #Timer2.deinit()
    
    print("end USV Ethernet Bridge")
    return

def run(wlanIn):
    """
    run working function, for the USV ethernet bridge
    
    run(wlanIn)
    - input value
      wlanIn - wlan object input
    - output value
      None
    """
    import asyncio
    global wlan
    wlan = wlanIn
    
    asyncio.run(USVBusEthernetBridge())
