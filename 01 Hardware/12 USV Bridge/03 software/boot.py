'''
Boot file for an Raspberry Pi Pico W
 configure WLAN for automated oprations
'''
from uos import listdir as ls
from uos import remove as rm
from uos import rmdir
from uos import mkdir
from uos import rename
from uos import chdir as cd
from uos import getcwd as pwd
from uos import stat
import time

def freeSpace(root='/'):
    from uos import statvfs
    stat = statvfs(root)
    return [stat[0]*stat[3],'bytes',stat[3],'blocks']
def usedSpace(root='/'):
    from uos import statvfs
    stat = statvfs(root)
    return [stat[0]*stat[2]-stat[0]*stat[3],'bytes',stat[2]-stat[3],'blocks']

def setWLanGlobal(wlanIn):
    global wlan
    wlan = wlanIn
    return

def runFile(file:str):
    try:
        if type(file) == str:
            exec(open(file).read())
        else:
            raise(Exception("In function 'run' variable 'file' must be a string!"))
    except Exception as e:
        raise(e)

global printThreadID
global getThreadID

getThreadID = __import__('_thread').get_ident
printThreadID = lambda : print(f"ThreadID: {__import__('_thread').get_ident()}")

from WLAN import WLAN
import ntptime

'''
config = {'Hostname':'NAME',
          'ntpHost':'NTPSERVER',
          'flagWriteToLog':True,
          'initWLAN':True,
          'WLANconfigFile':'./WLAN.json',}
 
WLANcfg = {'WLANSSID':['SSID1',],
           'WLANkey':[b'WLANKEYasBASE64\n',],
           'WLANprio':[0,]}
'''

config = None
WLANcfg = None

import json
with open("./config.json") as f:
    config = json.load(f)

if config is not None:
    with open(config['WLANconfigFile']) as f:
        WLANcfg = json.load(f)
del(f)
del(json)

if config is not None:
    if 'ntpHost' in config.keys():
        ntptime.host = config['ntpHost']
    
    if 'initWLAN' in config.keys():
        if config['initWLAN']:
            wlan = WLAN(keyFile='./key.txt',activate=True, Hostname=config['Hostname'])
            
            if 'flagWriteToLog' in config.keys():
                if config['flagWriteToLog']:
                    wlan.setPrintFunc(print)
            
            if WLANcfg is not None:
                if 'WLANkey' in WLANcfg.keys() and 'WLANSSID' in WLANcfg.keys():
                    wlan.autoconnecting(key=WLANcfg['WLANkey'][0],SSID=WLANcfg['WLANSSID'][0])

setWLanGlobal(wlan)

del(ntptime)
del(setWLanGlobal)
del(WLAN)
del(WLANcfg)

t = time.localtime()
print(f"system time: {t[2]:02}.{t[1]:02}.{t[0]:04} {t[3]:02}:{t[4]:02}:{t[5]:02}")
del(t)
