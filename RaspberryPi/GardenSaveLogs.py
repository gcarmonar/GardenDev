

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
#import serial
#from twython import Twython, TwythonError
import time

#--- Objects --------------------------------------------------------------
#ser = serial.Serial('/dev/ttyUSB0')




# def GetLocalTime():
# 	localtime = time.localtime(time.time())
# 	year = localtime[0]
# 	month = localtime[1]
# 	day = localtime[2]
# 	hour = localtime[3]
# 	minutes = localtime[4]
# 	seconds = localtime[5]
# 	return localtime



#data = ser.readline()
data = "Dod"

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
	#sensors = ser.readline()
	sensors = "t218 m915 l887"
	text = sensors.split()
	temp = (int(text[0][1:]))/10.0
	text = timeText + "," + str(temp) + "," + (text[1][1:]) + "," + (text[2][1:])
	print text 

	fo.write(text)
	fo.close()



	# lt = time.localtime(time.time())
	# file_name = "Garden_" + str(lt[0]) + "_" + str(lt[1]) + "_" + str(lt[2]) + "___" + str(lt[3])
	# fo = open(file_name, "a")
	# fo.write()




