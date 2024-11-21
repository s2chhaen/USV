import USVBusEthernetBridge
time.sleep(1)
try:
    USVBusEthernetBridge.run(wlan)
    #exec(open('USVBusEthernetBridge.py').read())    
except:
    pass
