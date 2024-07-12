from socket import *
import json, time, sys
import dz_ship  # virtual ship
import base64


def read_config():
    """ APRS read all configuration infos """ 
    global ServerHost, ServerPort, Callsign, SSID, Password, Latitude, Longitude, Comment, Status, Status_Packet, Primary_Symbol_Key, Secondary_Symbol_Key, delay
    
    with open('aprs_config.json', 'r') as config_file:
        data = json.load(config_file)

    # Dekodieren des Passworts
    decoded_password = base64.b64decode(data["Password"]).decode('utf-8')
    
    # Zugriff auf einzelne Werte
    ServerHost = data["ServerHost"]
    ServerPort = data["ServerPort"]
    Callsign = data["Callsign"]
    SSID = data["SSID"]
    Password = decoded_password
    Latitude = data["Latitude"]
    Longitude = data["Longitude"]
    Comment = data["Comment"]
    Status = data["Status"]
    Status_Packet = data["Status_Packet"]
    Primary_Symbol_Key = data["Primary_Symbol_Key"]
    Secondary_Symbol_Key = data["Secondary_Symbol_Key"]
    delay = data["Delay"]


def check_config():
    global ServerHost, ServerPort, Callsign, SSID, Password, Latitude, Longitude, Comment, Status, Status_Packet, Primary_Symbol_Key, Secondary_Symbol_Key, delay

    errors = []

    def add_error(condition, message):
        if condition:
            errors.append(message)

    add_error(ServerHost == '', "No server host defined!")
    add_error(ServerPort is None, "No server port defined!")
    add_error(Callsign == '' or Callsign == 'N0CALL', "No Callsign defined!")
    add_error("-" in Callsign, "SSID specified in the Callsign field! Please specify the SSID in the SSID field!")
    add_error(Password == '' or Password == '12345', "No password defined!")
    add_error(Latitude == '', "No Latitude defined!")
    add_error(Longitude == '', "No Longitude defined!")
    add_error(len(Comment) > 43, f"Comment is over 43 characters ({len(Comment)}/43)!")
    add_error(len(Status) > 43, f"Status is over 43 characters ({len(Status)}/43)!")

    if errors:
        for error in errors:
            print(f"CONFIG ERROR: {error} Check configuration and try again!")
        sys.exit(1)

    if delay is None:
        delay = 600  # Defaults to 10 mins
    
    print("CONFIGURATION OK")


def countdown(t):
    while t:
        mins, secs = divmod(t, 60)
        timeformat = f'Next Packet: {mins:02d}:{secs:02d}'
        print(timeformat)
        time.sleep(1)
        t -= 1
    print('Time\'s up!')


def send_packet(next_position):
    global ServerHost, ServerPort, Callsign, SSID, Password, Latitude
    global Longitude, Comment, Status, Status_Packet, Primary_Symbol_Key 
    global Secondary_Symbol_Key, delay

	# create socket & connect to server
    sSock = socket(AF_INET, SOCK_STREAM)
    sSock.connect((ServerHost, ServerPort))
	# logon
    RAWpacket = f'user {Callsign} pass {Password} vers "USV-Communication" \n'
    sSock.send(bytes(RAWpacket, 'utf-8'))
	
	# send BEACON packet
#	BEACONpacket = f'{Callsign}{SSID}>APE,TCPIP*:={Latitude}{Primary_Symbol_Key}{Longitude}{Secondary_Symbol_Key}{Comment}\n'
    BEACONpacket = f'{Callsign}{SSID}>APE,TCPIP*:={next_position}{Secondary_Symbol_Key}{Comment}\n'
    sSock.send(bytes(BEACONpacket, 'utf-8'))
	
    print(BEACONpacket)
    print("[BEACON packet sent]")
	
    if Status_Packet == True and Status != '':
        ntdown(5)
		# send STATUS packet
        TUSpacket = f'{Callsign}{SSID}>APE,TCPIP*:>{Status}\n'
        ck.send(bytes(STATUSpacket, 'utf-8'))
		
        nt(STATUSpacket)
        nt("[STATUS packet sent]")
    else:
      pass
	
	# close socket -- must be closed to avoid buffer overflow
    sSock.shutdown(0)
    sSock.close()
    print("\n----------")


def send_telemetry(telemetry):
	global ServerHost, ServerPort, Callsign, SSID, Password, Latitude, Longitude, Comment, Status, delay
    
	# create socket & connect to server
	sSock = socket(AF_INET, SOCK_STREAM)
	sSock.connect((ServerHost, ServerPort))
	# logon
	RAWpacket = f'user {Callsign} pass {Password} vers "USV-Communication" \n'
	sSock.send(bytes(RAWpacket, 'utf-8'))
	
	# send TELEMETRY packet
	TELEMETRYpacket = f'{Callsign}{SSID}>APRS:{telemetry}\n'
	sSock.send(bytes(TELEMETRYpacket, 'utf-8'))
		
	# close socket -- must be closed to avoid buffer overflow
	sSock.shutdown(0)
	sSock.close()
	print("\n----------")


# main
read_config()  # APRS config
check_config()
t_paket = 0  # Start Number of Telemetrie-Paket
p_delay = 6  # time between two frames
tx_flag = 1  # 0 without transmit / 1 transmit over APRS

while True:
    aprs_msg = dz_ship.get_data(p_delay, t_paket)
    
    for i, msg in enumerate(aprs_msg):
        print(msg)
        if tx_flag == 1:
            if i == 0:
                send_packet(msg)
            else:
                send_telemetry(msg)
        time.sleep(p_delay)  # Delay for APRS transmit
    
    t_paket += 3
    countdown(delay)   



    
   