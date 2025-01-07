import socket
import network
import os
import _thread
import asyncio
from time import sleep
from asyncio import Event

def FTP_Runtime(t1exit):
    import socket
    import network
    import os
    
    timeOut = 60
    
    DATA_PORT = 13333

    ftpsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    datasocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    ftpsocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    datasocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    ftpsocket.bind(socket.getaddrinfo("0.0.0.0", 21)[0][4])
    datasocket.bind(socket.getaddrinfo("0.0.0.0", DATA_PORT)[0][4])

    ftpsocket.listen(1)
    datasocket.listen(1)
    datasocket.settimeout(10)

    dataclient = None
    
    monthList = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    
    def send_list_data(cwd, dataclient):
        for file in os.listdir(cwd):
            stat = os.stat(get_absolute_path(cwd, file))
            file_permissions = "drwxr-xr-x" if (stat[0] & 0o170000 == 0o040000) else "-rw-r--r--"
            file_size = stat[6]
            datetime = time.localtime(stat[7])
            strDate = f"{monthList[datetime[1]-1]} {datetime[2]:2}  {datetime[0]:04}"
            strDateTime = f"{monthList[datetime[1]-1]} {datetime[2]:2} {datetime[3]:2}:{datetime[4]:02}"
            description = "{}    1 owner group {:>13} {} {}\r\n".format(file_permissions, file_size, strDateTime,file)
            dataclient.sendall(description)
            
    def send_file_data(path, dataclient):
        with open(path) as file:
            chunk = file.read(128)
            while len(chunk) > 0:
                dataclient.sendall(chunk)
                chunk = file.read(128)

    def save_file_data(path, dataclient):
        with open(path, "w") as file:
            chunk = dataclient.read(128)
            while len(chunk) > 0:
                file.write(chunk)
                chunk = dataclient.read(128)

    def get_absolute_path(cwd, payload):
        # if it doesn't start with / consider
        # it a relative path
        if not payload.startswith("/"):
            payload = cwd + "/" + payload
            payload = payload.rstrip("/")
        # and don't leave any trailing /
        return payload
    
    try:
        print("start runtime FTP-Server")
        dataclient = None
        ftpsocket.settimeout(0.05)
        getConnection = Event()
        dataConnection = Event()
        getConnection.set()
        while getConnection.is_set():
            cwd = "/"
            try:
                cl, remote_addr = ftpsocket.accept()
                cl.settimeout(0.05)
                print("FTP connection open")
                dataConnection.set()
                print("FTP connection from:", remote_addr)
                cl.sendall("220 Hello, this is the RPI Zero.\r\n")
                timeoutConter = timeOut*10
                while dataConnection.is_set():
                    getData = False
                    try:
                        data = cl.readline()
                        if len(data) <= 0:
                            print("Client is dead")
                            dataConnection.clear()
                        else:
                            getData = True
                            timeoutConter = timeOut*10
                    except OSError as e:
                        if e.errno != 110:
                            print(f"OSError occurred in readline loop: {e}")
                            time.sleep(0.2)
                            cl.close()
                            print("FTP Data connection close")
                            dataConnection.clear()
                        else:
                            if timeoutConter > 0:
                                timeoutConter = timeoutConter-1
                            else:
                                cl.close()
                                dataConnection.clear()
                                print("Client is dead")
                        sleep(0.1)
                    except Exception as e:
                        print(f"error occurred in readline loop: {e}")
                        time.sleep(0.2)
                        cl.close()
                        print("FTP Data connection close")
                        dataConnection.clear()
                        getConnection.clear()
                    except:
                        print("error occurred in readline loop")
                        time.sleep(0.2)
                        cl.close()
                        print("FTP Data connection close")
                        dataConnection.clear()
                        getConnection.clear()
                    sleep(0.05)
                    if getData:
                        try:
                            data = data.decode("utf-8").replace("\r\n", "")
                            command, payload =  (data.split(" ") + [""])[:2]
                            command = command.upper()
                            sleep(0.1)
                            print("Command={}, Payload={}".format(command, payload))
                            if command == "USER":
                                print("230 Logged in.")
                                cl.sendall("230 Logged in.\r\n")
                            elif command == "SYST":
                                cl.sendall("215 RPI Zero MicroPython\r\n")
                            elif command == "SYST":
                                cl.sendall("502\r\n")
                            elif command == "PWD":
                                print(cwd)
                                sendData = '257 "{}"\r\n'.format(cwd)
                                print(f"sendData: {sendData}")
                                cl.sendall(sendData)
                            elif command == "CWD":
                                print("CWD")
                                path = get_absolute_path(cwd, payload)
                                print(f"path: {path}")
                                try:
                                    files = os.listdir(path)
                                    cwd = path
                                    cl.sendall('250 Directory changed successfully\r\n')
                                except:
                                    cl.sendall('550 Failed to change directory\r\n')
                            elif command == "EPSV":
                                cl.sendall('502\r\n')
                            elif command == "TYPE":
                                # probably should switch between binary and not
                                cl.sendall('200 Transfer mode set\r\n')
                            elif command == "SIZE":
                                path = get_absolute_path(cwd, payload)
                                try:
                                    size = os.stat(path)[6]
                                    cl.sendall('213 {}\r\n'.format(size))
                                except:
                                    cl.sendall('550 Could not get file size\r\n')
                            elif command == "QUIT":
                                cl.sendall('221 Bye.\r\n')
                            elif command == "PASV":
                                addr = network.WLAN().ifconfig()[0]
                                cl.sendall('227 Entering Passive Mode ({},{},{}).\r\n'.format(addr.replace('.',','), DATA_PORT>>8, DATA_PORT%256))
                                dataclient, data_addr = datasocket.accept()
                                print("FTP Data connection from:", data_addr)
                            elif command == "LIST":
                                print("LIST")
                                try:
                                    send_list_data(cwd, dataclient)
                                    dataclient.close()
                                    cl.sendall("150 Here comes the directory listing.\r\n")
                                    cl.sendall("226 Listed.\r\n")
                                except:
                                    cl.sendall('550 Failed to list directory\r\n')
                                finally:
                                    dataclient.close()
                            elif command == "RETR":
                                try:
                                    send_file_data(get_absolute_path(cwd, payload), dataclient)
                                    dataclient.close()
                                    cl.sendall("150 Opening data connection.\r\n")
                                    cl.sendall("226 Transfer complete.\r\n")
                                except:
                                    cl.sendall('550 Failed to send file\r\n')
                                finally:
                                    dataclient.close()
                            elif command == "STOR":
                                try:
                                    cl.sendall("150 Ok to send data.\r\n")
                                    save_file_data(get_absolute_path(cwd, payload), dataclient)
                                    dataclient.close()
                                    cl.sendall("226 Transfer complete.\r\n")
                                except:
                                    cl.sendall('550 Failed to send file\r\n')
                                finally:
                                    dataclient.close()
                            elif command == "DELE":
                                try:
                                    print(payload)
                                    os.remove(payload)
                                    cl.sendall("250 Requested to delete file, complete.\r\n")
                                except:
                                    cl.sendall("450 Delete file are incomplete.\r\n")
                            else:
                                cl.sendall("502 Command not implemented.\r\n")
                                print("Unsupported command '{}' with payload '{}'".format(command, payload))
                            time.sleep(0.1)
                            #await asyncio.sleep(0.05)
                        except Exception as e:
                            print(f"decode error in data loop occurred: {e}")
                            cl.close()
                            print(f"FTP Data connection close")
                            dataConnection.clear()
                        except:
                            print("decode error in data loop occurred")
                            cl.close()
                            print(f"FTP Data connection close")
                            dataConnection.clear()
                    #--if getData: end--#
                time.sleep(0.1)
                #await asyncio.sleep(0.05)
                #--while end--#
            except OSError as e:
                if e.errno != 110:
                    print(f"error in loop occurred: {e}")
                sleep(0.1)
            except Exception as e:
                print(f"error in loop occurred: {e}")
                getConnection.clear()
            except:
                print(f"error in loop occurred")
                getConnection.clear()
            #--try end--#
        #-- while getConnection.is_set(): end--#
    except Exception as e:
        print(f"error in thread: {e}")
        time.sleep(0.2)
    except:
        print("error in FTP_Runtime")
    finally:
        datasocket.close()
        ftpsocket.close()
        if dataclient is not None:
            dataclient.close()
        print("exit runtime FTP-Server")
        t1exit.clear()

def FTPServerRun():
    t1exit = Event()
    t1exit.set()
    FTP_Runtime(t1exit)
    #_thread.stack_size(1024)
    #_thread.start_new_thread(FTP_Runtime, (t1exit,))
    while t1exit.is_set():
        sleep(0.1)
        pass

if __name__ == '__main__':
    FTPServerRun()