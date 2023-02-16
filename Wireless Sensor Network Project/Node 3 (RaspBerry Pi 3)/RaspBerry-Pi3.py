import sys
import pigpio
import RPi.GPIO as GPIO
import os
import string
import getopt
import socket
from time import sleep
import Adafruit_DHT # DHT library
import urllib2 # HTTP library
from coapthon.client.helperclient import HelperClient  # CoAP library
from coapthon.utils import parse_uri

#support variables and input pin
DEBUG = 1		
RCpin = 14

#how many seconds between posting data
myDelay = 300 

#input pin set as analog
GPIO.setmode(GPIO.BCM)
GPIO.setup(RCpin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

#read DHT sensor data
def getSensorData():
    print "In getSensorData";
	
	#reading
    humidity0, temperature0 = Adafruit_DHT.read_retry(Adafruit_DHT.DHT22, 2)
	
	#returning
    return ("{0:.2f}".format(humidity0), "{0:.2f}".format(temperature0))
	
#read light
def RCtime(RCpin):
    LT = 0
	
    #check and update
    if (GPIO.input(RCpin) == True):
        LT += 1
		
	#returning
    return (str(LT))
	
def main():
    print 'starting...'
	
	#used for the authentication page, now disabled
    password_mgr = urllib2.HTTPPasswordMgrWithDefaultRealm()
    top_level_url = "http://www.emanuelepagliari.it/iot/"
	
	#login credentials
    username = "admin"
    password = "password"
	
    #handler creation
    password_mgr.add_password(None, top_level_url, username, password)
    handler = urllib2.HTTPBasicAuthHandler(password_mgr)

    #opener creation
    opener = urllib2.build_opener(handler)

    #opener install for use it
    urllib2.install_opener(opener)

	#loop
    while True:
        try:
            print "Reading Sensor Data now"
			
			#sensor function recall
            RHW, TW= getSensorData()
            LT = RCtime(RCpin)
			
			#data print
            print TW + " " + RHW + " " + LT
			
			#HTTP POST of the data
            opener.open("http://www.emanuelepagliari.it/iot/add_indoor.php?temp="+TW+"&hum="+RHW+"&light="+LT).close()

			#CoAP Client init
            global client
			
			#path
            path = "coap://192.168.178.149:5683/indtemp"
			
			#parse port, host and path
            host, port, path = parse_uri(path)
			
            try:
				#test
                tmp = socket.gethostbyname(host)
                host = tmp
            except socket.gaierror:
                pass
			#prepare client
            client = HelperClient(server=(host, port))
			
            #POST temperature
            client.post(path, str(TW))
            
			#POST humidity
            client.post("indhum", str(RHW))

			#POST Light ON/OFF based on the readings
            if(LT=="1"):
                client.post("light", "0")
            else:
                client.post("light", "1")
              
			#close client
            client.stop()
			
			#sleep
            sleep(int(myDelay))
            
        except Exception as e:
            print e
            print 'exiting.'
            break

# call main
if __name__ == '__main__':
    main()
