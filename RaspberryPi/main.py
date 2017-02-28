#===========================================================================


#=== Libraries =============================================================
import time
import serial
# https://github.com/ryanmcgrath/twython/blob/master/docs/usage/advanced_usage.rst
# from twython import Twython, TwythonError
# https://www.raspberrypi.org/documentation/usage/camera/python/README.md
# import picamera

#=== Constants =============================================================
# Twitter account details
APP_KEY 				= '0'
APP_SECRET 				= '0'
OAUTH_TOKEN 			= '0'
OAUTH_TOKEN_SECRET		= '0'
# Flags constants
SERIAL_DATA_ERROR		= 2
SERIAL_DATA_AVAILABLE	= 1
NO_SERIAL_DATA			= 0
PROCESSED_VALID_DATA 	= 1
PROCESSED_DATA_ERROR 	= 0
LOG_SUCCESS				= 1
LOG_ERROR				= 0
# Serial
SERIAL_PORT				= 'COM7'			# Windows
#SERIAL_PORT				= '/dev/ttyAMA0'	# Raspberry Pi
SERIAL_BAUD 			= 9600
SERIAL_TIMEOUT 			= 1					# In seconds
# Alerts
TEMPERATURE 			= 0
MOISTURE 				= 1
MAX_TEMP				= 24.3				# Configurable
MIN_TEMP				= 22.2				# Configurable
HIGH_TEMP_ALERT			= 2
LOW_TEMP_ALERT			= 1
NO_TEMP_ALERT			= 0
MIN_SOIL_MOISTURE		= 500				# Configurable, try and error
LOW_SOIL_MOISTURE_ALERT = 1
NO_SOIL_MOISTURE_ALERT	= 0



#=== Objects ===============================================================
ser = serial.Serial(SERIAL_PORT, SERIAL_BAUD, timeout=SERIAL_TIMEOUT)
# twitter = Twython(APP_KEY, APP_SECRET, OAUTH_TOKEN, OAUTH_TOKEN_SECRET)
# camera = picamera.PiCamera()

#=== Variables =============================================================
# Dictionaries
actualDate = {}
lastDate = {}
# Flags
serialFlag = 0
dataFlag = 0
logFlag = 0
twitterSendAlertFlag = [0,0]
# Global general variables
dateString = ''
logFileName = ''
pictureName = ''
alerts = [NO_TEMP_ALERT,NO_SOIL_MOISTURE_ALERT]
serialData = []

#=== Functions =============================================================
def SaveLog():
	print "Save Log"

	global logFileName, dateString, serialData
	
	fp = open(logFileName, "a")
	fp.write(dateString)
	for word in serialData:
		fp.write(',')
		fp.write(str(word))
	fp.write('\n')
	fp.close()

	return LOG_SUCCESS

def TakePicture():
	print "Take Picture"
	# pictureName = dateString + '.jpg'
	# camera.capture(pictureName)

def SendTwitter():
	print "Send Twitter"

	global twitterSendAlertFlag, alerts, pictureName

	if sendStatusTwitter == 1:
		print "Sending picture via Twitter"
		# photo = open('images/image.jpg', 'rb')	
		# response = twitter.upload_media(media=photo)
		# # Add Temperature, soil moisture and light to message
		# message = "Hi, the temperature is %s C"
		# twitter.update_status(status=message, media_ids=[response['media_id']])


	# Twitter send alerts
	for i in range(2):
		if alerts[i] != 0:
			if twitterSendAlertFlag[i] == 0:
				if i == TEMPERATURE:
					# Send temp alert
					if alerts[i] == 1:
						text = '[ALERT] : I am getting too hot!'
					else:
						text = '[ALERT] : I am getting too cold... Brrr!'
					twitter.update_status(status=text)
				elif i == MOISTURE:
					# Send I'm thisty twitter
					text = '[ALERT] : I am thisty! =('
					twitter.update_status(status=text)
				twitterSendAlertFlag[i] = 1
		else:
			twitterSendAlertFlag[i] = 0


def ReadSerialPort():
	print "Read Serial Port"

	global serialData

	data = ser.read(100)
	
	if len(data) > 0:
		# print data
		if data[0] == "D":
			ser.write('O')
			# print "Last is: " + data[-3:-2] + "\n"
			if data[-3:-2] == "E":				# Last char before eol
				# Save data into global var and split into a list
				ser.write('K')
				serialData = data[2:-4].split(',')	
				return SERIAL_DATA_AVAILABLE
		return SERIAL_DATA_ERROR
	return NO_SERIAL_DATA

def UpdateDate():
	print "Get Time"

	global actualDate

	localtime = time.localtime(time.time())
	
	actualDate['year'] = localtime[0]
	actualDate['month'] = localtime[1]
	actualDate['day'] = localtime[2]
	actualDate['hour'] = localtime[3]
	actualDate['minutes'] = localtime[4]
	actualDate['seconds'] = localtime[5]

def ProcessData():
	print "Processing Data"

	global dateString, logFileName, pictureName, serialData, alerts

	# Process time for saving log file
	dateString = str(actualDate['year']) + '_'
	dateString += str(actualDate['month']) + '_'
	dateString += str(actualDate['day']) + '_'
	dateString += '___'

	if actualDate['hour'] < 10:
		dateString += '0' + str(actualDate['hour']) + '_'
	else:
		dateString += str(actualDate['hour']) + '_'

	logFileName = dateString[:-1] + '.log'

	if actualDate['minutes'] < 10:
		dateString += '0' + str(actualDate['minutes']) + '_'
	else:
		dateString += str(actualDate['minutes']) + '_'

	pictureName = dateString[:-1] + '.jpg'

	if actualDate['seconds'] < 10:
		dateString += '0' + str(actualDate['seconds'])
	else:
		dateString += str(actualDate['seconds'])

	# Process Serial Data
	# Check for extreme temperatures
	
	# Check for Dry soil
	print "Serial data: "
	print serialData
	for i in range(0,len(serialData),2):
		#before comparing, preferable to convert to floats and scale data

		if serialData[i][0] == 't':
			serialData[i+1] = int(serialData[i+1]) / 10.0
		elif serialData[i][0] == 'm' or serialData[i][0] == 'r':
			serialData[i+1] = int(serialData[i+1])

		if serialData[i][0] == 't':
			if serialData[i+1] > MAX_TEMP:
				alerts[TEMPERATURE] = HIGH_TEMP_ALERT
			elif serialData[i+1] < MIN_TEMP:
				alerts[TEMPERATURE] = LOW_TEMP_ALERT
			else:
				alerts[TEMPERATURE] = NO_TEMP_ALERT
		
		elif serialData[i][0] == 'm':
			if serialData[i+1] < MIN_SOIL_MOISTURE:
				alerts[MOISTURE] = LOW_SOIL_MOISTURE_ALERT
			else:
				alerts[MOISTURE] = NO_SOIL_MOISTURE_ALERT

	return 1



#=== Main ==================================================================
def main():
	# Init
	print "Main"
	global serialFlag

	UpdateDate()
	lastDate = actualDate

	while True:
		# Always do it
		timer = time.time()
		serialFlag = ReadSerialPort()
		UpdateDate()

		# If hour has changed then send a Twitt
		if lastDate['hour'] != actualDate['hour']:
			#SendTwitter()
			lastDate = actualDate

		if serialFlag == SERIAL_DATA_AVAILABLE:
			serialFlag = NO_SERIAL_DATA
			dataFlag  = ProcessData()
			if dataFlag == PROCESSED_VALID_DATA:
				logFlag = SaveLog()
				if logFlag != LOG_SUCCESS:
					print "Error saving data"

		time.sleep(2)


try:
	main() 
except KeyboardInterrupt:
	ser.close()  
	#GPIO.cleanup()       # clean up GPIO on CTRL+C exit
	print "KeyboardInterrupt"
# except Exception: 
# 	pass
#GPIO.cleanup()           # clean up GPIO on normal exit 
