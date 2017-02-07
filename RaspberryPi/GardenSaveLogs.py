

'''
Receives serial data from Arduino, saves the data in a local database and 
sends a Twitter every hour aprox with the actual status of the garden. 


http://gardenbot.org/howTo/soilMoisture/
https://learn.sparkfun.com/tutorials/raspberry-pi-twitter-monitor?_ga=1.248133262.863112450.1472145466
https://github.com/ryanmcgrath/twython/blob/master/examples/update_status.py
http://pyserial.readthedocs.io/en/latest/shortintro.html
https://docs.python.org/2/library/time.html
https://www.tutorialspoint.com/python/python_date_time.htm
https://www.tutorialspoint.com/python/python_files_io.htm

'''

#--- Libraries ------------------------------------------------------------
import serial
from twython import Twython, TwythonError
import time

#--- Objects --------------------------------------------------------------
#ser = serial.Serial('/dev/ttyUSB0')		#Linux
ser = serial.Serial('COM7', 9600, timeout=1)		#Windows




# def GetLocalTime():
# 	localtime = time.localtime(time.time())
# 	year = localtime[0]
# 	month = localtime[1]
# 	day = localtime[2]
# 	hour = localtime[3]
# 	minutes = localtime[4]
# 	seconds = localtime[5]
# 	return localtime


while True:
	data = ser.readline()
	#data = "Data"

	if data[0] == "D":
		localtime = time.localtime(time.time())
		year = localtime[0]
		month = localtime[1]
		day = localtime[2]
		hour = localtime[3]
		minutes = localtime[4]
		seconds = localtime[5]

		# Correct format
		if hour < 10:
			hour = '0'+str(hour)
		if minutes < 10:
			minutes = '0'+str(minutes)
		if seconds < 10:
			seconds = '0'+str(seconds)

		timeText = str(year) + '/' + str(month) + '/' + str(day) + ' ' + str(hour) + ':' + str(minutes) + ':' + str(seconds)  
		
		sensors = ser.readline()
		#sensors = "ta248 ma915 la887"
		print "sensors: " + sensors

		text = sensors.split()
		temp = (int(text[0][2:]))/10.0
		text = timeText + "," + str(temp) + "," + (text[1][2:]) + "," + (text[2][2:-1]) + '\n'
		print "text " + text

		# Makes sure that data set is completly transmitted.
		if sensors[-3:-2] == 'E':
			fo = open("GardenLog.txt", 'a')
			fo.write(text)
			fo.close()
		else:
			print "Error in data..."
			print sensors[-3:-2]



		# lt = time.localtime(time.time())
		# file_name = "Garden_" + str(lt[0]) + "_" + str(lt[1]) + "_" + str(lt[2]) + "___" + str(lt[3])
		# fo = open(file_name, "a")
		# fo.write()




