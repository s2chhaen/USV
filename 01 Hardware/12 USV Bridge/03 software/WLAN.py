"""
Created on 10.09.2024 15:05:45

Program history
10.09.2024    V 00.10    init Version crate class for connecting and deconnecting wlan with key
12.09.2024    V 00.20    add keyfile to connecting
20.09.2024    V 00.40    add autoconnecting and reconnecting to one w-lan net, (reset of the internal wlan object (not safe reconnect in all states))
22.09.2024    V 00.15    add wlan state to onboard 'LED'
25.09.2024    V 00.50    update to encrypted WLAN key
28.09.2024    V 00.60    add implement ethernet ntp protocol to get the date time for the internal rtc
17.10.2024    V 00.70    upgrade the rtc to date time from the ethernet ntp protocol with calculating to the local time zone
30.10.2024    V 00.90    add system name for net response
TODO          V 01.00    add reading json file for wlan credentials, locked with key

WLAN Modul with
WLAN Class
- autoconnection to one wlan net
- print state to onboard LED
- run outside from other program code
- update internal rtc modul with datetime over ethernet ntp protocol
- TODO add cypt to class function, locked with key
- TODO add reading json file for wlan credentials, locked with key

@author: S. Franke
"""
class WLAN:
    """
    WLAN wrapper class for Raspberry Pi pico W
     for automated connect and reconnect in standalone operating system
     - use encrypted WLAN key
    
    WLAN(keyFile=None, activate=True, stateChangeCallback = None):
        - input parameter
          keyFile - set the key file for the encrypted wlan key
          activate - activate the w-lan modul
          stateChangeCallback - set the callback function for the w-lan state change
          Hostname - set the Hostname from the Controller
    """
    __network = __import__("network")
    __binascii = __import__("binascii")
    __machine = __import__("machine")
    __time = __import__("time").time
    __ticks_ms = __import__("time").ticks_ms
    __localtime = __import__("time").localtime
    __ntptime = __import__("ntptime")
    #__decrypt = lambda o,keyFile,encryptedStr: __import__("cipher").decrypt(keyFile,encryptedStr)
    __subSecondsIntervall = 4
    __modCindex05s = 2
    __modCindex1s = 4
    __modCindex30s = 30*4
    __modCindex1h = 60*60*4
    timeZone = [1,1] # [std time zone, add hour to daylight time]
    def __init__(self, keyFile=None, activate=True, stateChangeCallback = None, Hostname = None):
        self.__isPresent = False
        self._SSID = None
        self.__KeyFile = None
        self.__Key = None
        self.__autoConnecting = False
        self.__printFnc = self.__printDummy
        self._foundSSIDs = None
        self._netConfig = None
        self._chkTimer = None
        self._State = 255
        self.__stateChangeCallback = None
        self.__c = 0
        self.__Cindex30s = 0
        self.__reloadNTP = False
        self.__LED = self.__machine.Pin('LED', self.__machine.Pin.OUT, value=0)
        # Hostname configuration
        if type(Hostname) == str:
            self.__network.hostname(Hostname)
        
        # contry configuration
        self.__network.country('DE')
        self.__wlan = self.__network.WLAN(self.__network.STA_IF)
        self.__wlan.active(activate)
        self._SSID = self.__wlan.config('ssid')
        self.__getNetConfig()
        self.__KeyFile = keyFile
        if callable(stateChangeCallback):
            self.__stateChangeCallback = stateChangeCallback
        self.__getState()
        if self._State == 3:
            self.__LED.on()
        elif self._State == 0:
            self.__LED.off()
        self._chkTimer = self.__machine.Timer(period=int(1000/self.__subSecondsIntervall), mode=self.__machine.Timer.PERIODIC, callback=self.__stateChange)
        return
    
    def deinit(self):
        """
        WLAN destructor
        - input value
          None
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        
        self._chkTimer.deinit()
        self.__wlan.deinit()
        print("die WLAN class")
        return
    
    def __del__(self):
        """
        alternate WLAN destructor, run the deinit function
        - input value
          None
        - output value
          None
        """
        self.deinit()
        return
    
    def scanWLAN(self,printOut=False):
        """
        scan all WLAN nets and save the response internaly
        - input parameter
          printOut - set switch for print all founded nets to shell window
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        print(f'Scan WLAN for Nets')
        printThreadID = lambda : print(f"ThreadID: {__import__('_thread').get_ident()}")
        printThreadID()
        scannedNETs = self.__wlan.scan()
        self._foundSSIDs = []
        if printOut:
            print("found SSIDs")
            print("______________")
            print(scannedNETs)
        for foundNET in scannedNETs:
            self._foundSSIDs.append(foundNET[0].decode('utf-8'))
            if printOut:
                print(foundNET[0].decode('utf-8'))
                print("0x"+foundNET[1].hex())
                print(foundNET[3])
                print("______________")
        self.__isPresent = False
        for SSID in self._foundSSIDs:
            if SSID == self._SSID:
                self.__isPresent = True
                break
        print(f'found SSID: {self.__isPresent}')
        return
    
    def connecting(self, key : bytes, SSID=None):
        """
        connecting to WLAN net with the ssid
        connecting(key : bytes, SSID=None)
        - input parameter
          key - key for the WLAN net or when Key File is set key for this Key File
          SSID - ssid for connection when is None use internaly saved ssid
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        print(f'start function connecting')
        
        sleep = __import__("time").sleep
        if not self.__wlan.isconnected():
            if SSID!=None:
                self._SSID = SSID
            self.scanWLAN()
            if self.__isPresent:
            #if True:
                try:
                    if self.__KeyFile==None:
                        print(f'connection to WLAN {self._SSID}')
                        self.__wlan.connect(self._SSID, self.__binascii.a2b_base64(key))
                    else:
                        print(f'connection to WLAN {self._SSID}')
                        self.__wlan.connect(self._SSID, self.__decrypt(keyFile=self.__KeyFile, encryptedStr=key))
                    i = 10
                except Exception as e:
                    print("WLAN connecting issue")
                    print(f'Error message: {e}')
                    i = 0
                # WLAN-Verbindungsstatus prüfen
                while (not self.__wlan.isconnected()) and (i >= 0) and (self.__wlan.status() != -2):
                    print('wait for WLAN connection')
                    self.__LED.on()
                    sleep(0.25)
                    self.__LED.off()
                    sleep(0.75)
                    i = i-1
                if self.__wlan.isconnected():
                    self.__LED.on()
                    sleep(0.25)
                    self.__LED.off()
                    sleep(0.25)
                    self.__LED.on()
                    sleep(0.25)
                    self.__LED.off()
                    print(f"WLAN connected to {self._SSID} with")
                    self.__getNetConfig()
                    print(f'  IPv4 address: {self._netConfig[0]} / {self._netConfig[1]},')
                    print(f'  Standard-Gateway: {self._netConfig[2]} and')
                    print(f'  DNS-Server: {self._netConfig[3]}')
                    self.__loadNTPTime()
                else:
                    print(f"Wlan not conected to {self._SSID}")
            else:
                print(f"WLAN {self._SSID} not found")
        else:
            print(f"WLAN are connected to {self._SSID} with")
            self.__getNetConfig()
            print(f'  IPv4 address: {self._netConfig[0]} / {self._netConfig[1]},')
            print(f'  Standard-Gateway: {self._netConfig[2]} and')
            print(f'  DNS-Server: {self._netConfig[3]}')
        return
    
    def autoconnecting(self, key : bytes, SSID=None):
        """
        set to auto connecting state to an WLAN net with the ssid
        autoconnecting(key : bytes, SSID=None)
        - input parameter
          key - key for the WLAN net or when Key File is set key for this Key File
          SSID - ssid for connection when is None use internaly saved ssid
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        
        self.__Key = key
        if SSID!=None:
            self._SSID = SSID
        
        print(f"config for auto connection to {self._SSID}")
        t = (self.__time()<<2)
        self.__Cindex30s = (t%self.__modCindex30s)+4
        self.__autoConnecting = True
        return
    
    def disconnecting(self):
        """
        disconnecting from the conected WLAN net
        autoconnecting()
        - input parameter
          None
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        print('WLAN disconnecting')
        sleep = __import__("time")
        # WLAN-Verbindung beenden
        if self.__wlan.isconnected():
            self.__wlan.disconnect()
            print('WLAN connection closed')
        self.__wlan.deinit()
        print('WLAN deinit')
        #del(self.__wlan)
        self._netConfig = ('','','','')
        #self.__wlan = self.__network.WLAN(self.__network.STA_IF)
        self.__wlan.active(True)
        print('WLAN activate')
        return
    
    def disableautoconnecting(self):
        """
        disable autoconecting and disconnecting from the conected WLAN net
        disableautoconnecting()
        - input parameter
          None
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        
        print('disable WLAN auto connection')
        self.__autoConnecting = False
        self.disconnecting()
        return
    
    def reset(self):
        """
        reset WLAN stack
        reset()
        - input value
          None
        - output value
          None
        """
        self.__wlan.deinit()
        print('WLAN deinit')
        #del(self.__wlan)
        self._netConfig = ('','','','')
        #self.__wlan = self.__network.WLAN(self.__network.STA_IF)
        self.__wlan.active(True)
        print('WLAN activate')
        return
    
    def isconnected(self) -> bool:
        """
        get the state if WLAN is connected to an net
        isconnected()
        - input value
          None
        - output value
          bool - connected to an net, True or False
        """
        return self.__wlan.isconnected()
    
    def state(self) -> int:
        """
        get WLAN state as a number
        isconnected()
        - input value
          None
        - output value
          int - number from WLAN state, 0 - disconected, 3 - connected
        """
        self.__getState()
        return self._State
    
    def ifconfig(self) -> list:
        """
        get configuration from WLAN connecting
        isconnected()
        - input value
          None
        - output value
          list - configuration from the connecting
        """
        return self.__wlan.ifconfig()
    
    def setPrintFunc(self, fnc=None):
        """
        overload the internal print function
        isconnected()
        - input value
          fnc - callable object or function
        - output value
          None
        """
        if callable(fnc):
            self.__printFnc = fnc
        else:
            self.__printFnc = self.__printDummy
        return
    
    def __stateChange(self,*args):
        """
        private function:
          Call as a timer event function, to check state differences and set the state to the onboard LED.
        __stateChange(*args)
        - input value
          *args - dummy variable for unused transfer parameter
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        t = (self.__time()<<2) + self.__c
        
        try:
            # 0.25s tick time
            #if (t%4) == 0:
            if self.__wlan.status() != self._State:
                self.__getState()
                print(f"new WLAN state: {self._State}")
                if self._State == 0:
                    self.__LED.off()
                elif self._State == 3:
                    self.__LED.on()
                
                if callable(self.__stateChangeCallback):
                        self.__stateChangeCallback(self._State)
            
            # 30s tick time
            if (t%self.__modCindex30s) == self.__Cindex30s:
                #print("start 30s Tik in statemachine")
                if self.__autoConnecting:
                    if self._State == 0:
                        print("state 0 autoconnection -> scan WLAN")
                        print(self.__wlan)
                        t1 = self.__ticks_ms()
                        self.scanWLAN(False)
                        t2 = self.__ticks_ms()
                        dt = t2-t1
                        print(f"scan time: {dt} ms")
                        if self.__isPresent:
                            print("WLAN is pesent")
                            print("state 0 autoconnection -> connecting")
                            self.connecting(self.__Key, self._SSID)
                        else:
                            print("WLAN is not pesent")
                    if self._State == 1:
                        print("state 1 autoconnection -> disconnecting")
                        self.disconnecting()
                if self._State == -2:
                    if not self.__autoConnecting:
                        print("state -1 not autoconnection -> connecting")
                        self.connecting(self.__Key, self._SSID)
                if self._State == 3 and self.__reloadNTP:
                    self.__loadNTPTime()
            
            # 1s tick time for debug
            #if self._State == 3:
            #    if (t%self.__modCindex1s) == 0:
            #        self.__LED.toggle()
            #        print(f"W.st.: {self.__wlan.status()}")
            
            # 1/2s tick time blink LED slow
            if self._State == -2:
                if (t%self.__modCindex05s) == 0:
                    self.__LED.toggle()
            
            # 1/4s tick time blink LED fast
            if self._State == 1 or self._State == 2:
                self.__LED.toggle()
            
            # 1s tick time pulse LED fast
            if self._State == 0 and self.__autoConnecting:
                if t%self.__modCindex1s == 0:
                    self.__LED.on()
                if t%self.__modCindex1s == 1:
                    self.__LED.off()
                    
            # 1h tick time
            if (t%self.__modCindex1h) == 0 and self._State == 3:
                self.__loadNTPTime()
        except Exception as e:
            print("__stateChange exeption")
            print(f'\tError message: {e}')
        
        self.__c = (self.__c+1)%self.__subSecondsIntervall
        return
    
    # get state
    def __getState(self):
        """
        private function:
          Update the state for the wlan object.
        __getState()
        - input value
          None
        - output value
          None
        """
        self._State = self.__wlan.status()
        return
    
    def __getNetConfig(self):
        """
        private function:
          Update the net configuration for the wlan object.
        __getState()
        - input value
          None
        - output value
          None
        """
        self._netConfig = self.__wlan.ifconfig()
        return
    
    def __loadNTPTime(self):
        """
        private function:
          Load the net time over the wlan and set it to the system time.
        __getState()
        - input value
          None
        - output value
          None
        """
        # print überladen
        print = self.__printFnc
        try:
            self.__ntptime.settime()
            self.setTimeWithTimeZone()
            print(f'  Update System time to: {self.__localtime()}')
            self.__reloadNTP = False
        except Exception as e:
            print(f'  Error in update System time')
            print(f'{e}')
            self.__reloadNTP = True
        return
    
    def setTimeWithTimeZone(self):
        """
        Set the system time to the internal saved time zone.
        self.timeZone[std time zone, add hour to daylight time]
          - std time zone - hour offset to utc time
          - add hour to daylight time - addintional hour offset for the daylight time
        __getState()
        - input value
          None
        - output value
          None
        """
        import time, machine
        rtc = machine.RTC()
        year = time.localtime()[0]       #get current year
        HHMarch   = time.mktime((year,3 ,(31-(int(5*year/4+4))%7),2-self.timeZone[0],0,0,0,0,0)) #Time of March change to CEST
        HHOctober = time.mktime((year,10,(31-(int(5*year/4+1))%7),2-self.timeZone[0],0,0,0,0,0)) #Time of October change to CET
        now=time.time()
        if now < HHMarch :               # we are before last sunday of march
            cet=time.localtime(now+3600*self.timeZone[0]) # CET:  UTC+self.timeZone[0]
        elif now < HHOctober :           # we are before last sunday of october
            cet=time.localtime(now+3600*(self.timeZone[0]+self.timeZone[1])) # CEST: UTC+self.timeZone[0]+self.timeZone[1]
        else:                            # we are after last sunday of october
            cet=time.localtime(now+3600*self.timeZone[0]) # CET:  UTC+self.timeZone[0]
            
        # resort the values for the RTC datetime function
        datetime = cet[0:3]+cet[6:7]+cet[3:6]+(0,)
        # save the new datetime
        rtc.datetime(datetime)
        return
    
    @staticmethod
    def __decrypt(keyFile: str, encryptedStr: str)->bytes:
        """
        Decrypt base64 data string to an bytes array, use the key from the key file.
         Load the value of padding bytes from the first byte in the decrypted data array and 
         reduce the data array with this value.
        __decrypt(keyFile: str, data: bytes)
        - input value
          keyFile - key file to store the generated key : str
          encryptedStr - to decrypted data string : str
        - output value
          decrypted data array : bytes
        """
        from ucryptolib import aes
        import binascii
        MODE_ECB = 1
        MODE_CBC = 2
        MODE_CTR = 6
        BLOCK_SIZE = 16
        
        fid = open(keyFile, 'r')
        keyBase64 = fid.read()
        fid.close()
        
        cipher = aes(binascii.a2b_base64(keyBase64), MODE_ECB)
        
        dataEncrypted = binascii.a2b_base64(encryptedStr)
        data = cipher.decrypt(dataEncrypted)
        pad = data[0] #1 byte is int, int.from_bytes(1,'little')
        
        return data[1:-pad]
    
    @staticmethod
    def __printDummy(*args):
        return
    
    @staticmethod
    def __writeLog(valueLog):
        import time
        now = time.localtime()
        strTime = "Date: {:02}.{:02}.{:04} {:02}:{:02}:{:02}: ".format(now[1], now[2], now[0], now[3], now[4], now[5])
        #print(valueLog) # optional
        file = open('WLAN_Log.txt', 'a')
        file.write( strTime+ f"{valueLog}" + "\n")
        file.close()
#---------------------------------------------------------------------------#
def Test():
    WLANkey = b'eWgmsExxcwO9SN0Tq/GD97GjHpPJWrcPMBDctBEfuy4=\n'

    wlan = WLAN(keyFile='./key.txt', activate=True)
    wlan.scanWLAN()

    wlan.connecting(WLANkey,"MB-ME")

    if wlan.isconnected():
        import socket
        import time
        import ntptime
        ntptime.host = 'melab-server.fritz.box'
        ntptime.settime()
        
        addr_info = socket.getaddrinfo("melab00n.fritz.box", 23)
        addr = addr_info[0][-1]
        sock = socket.socket(socket.AF_INET, #internet
                                 socket.SOCK_DGRAM) #UDP
        
        i = 120
        while i>0:
            time.sleep(1)
            i = i-1
            now = time.localtime()
            message = "Date: {}.{}.{} {}:{}:{} {} {}\n".format(now[1], now[2], now[0], now[3], now[4], now[5], now[6], now[7])
            try:
                sock.sendto(message.encode(), (addr[0], 5001))
                print (f"message sent: {message}")
            except:
                print ("Network issue")
        sock.close()
