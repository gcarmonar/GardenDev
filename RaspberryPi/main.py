#===========================================================================


#=== Libraries =============================================================
import time
import serial

#=== Constants =============================================================
# Flags constants
SERIAL_DATA_AVAILABLE	= 1
SERIAL_DATA_ERROR	 	= 0
PROCESSED_VALID_DATA 	= 1
PROCESSED_DATA_ERROR 	= 0
LOG_SUCCESS				= 1
LOG_ERROR				= 0
# Serial
SERIAL_PORT				= 'COM7'
SERIAL_BAUD 			= 9600
SERIAL_TIMEOUT 			= 1

#=== Objects ===============================================================
ser = serial.Serial(SERIAL_PORT, SERIAL_BAUD, timeout=SERIAL_TIMEOUT)

#=== Variables =============================================================
# Dictionaries
actualDate = {}
lastDate = {}
# Flags
serialFlag = 0
dataFlag = 0
logFlag = 0
# Global general variables
dateString = ''
logName = ''

#=== Functions =============================================================
def SaveLog():
	print "Save Log"



def SendTwitter():
	print "Send Twitter"

def ReadSerialPort():
	# print "Read Serial Port"
	data = ser.read(100)
	
	if len(data) > 0:
		# print data
		if data[0] == "D":
			# print "Last is: " + data[-3:-2] + "\n"
			if data[-3:-2] == "E":	# Last char before eol
				#serialFlag = 1
				return SERIAL_DATA_AVAILABLE
	return SERIAL_DATA_ERROR

def UpdateDate():
	print "Get Time"
	localtime = time.localtime(time.time())
	
	actualDate['year'] = localtime[0]
	actualDate['month'] = localtime[1]
	actualDate['day'] = localtime[2]
	actualDate['hour'] = localtime[3]
	actualDate['minutes'] = localtime[4]
	actualDate['seconds'] = localtime[5]

def ProcessData():
	print "Processing Data"

	# year = localtime[0]
	# month = localtime[1]
	# day = localtime[2]
	# hour = localtime[3]
	# minutes = localtime[4]
	# seconds = localtime[5]

	# # Correct format
	# 	if hour < 10:
	# 		hour = '0'+str(hour)
	# 	if minutes < 10:
	# 		minutes = '0'+str(minutes)
	# 	if seconds < 10:
	# 		seconds = '0'+str(seconds)

	dateString = str(actualTime['year']) + '_'
	dateString += str(actualTime['month']) + '_'
	dateString += str(actualTime['day']) + '_'
	dateString += '___'

	if actualTime['hour'] < 10:
		dateString += '0' + str(actualTime['hour']) + '_'
	else:
		dateString += str(actualTime['hour']) + '_'

	if actualTime['minutes'] < 10:
		dateString += '0' + str(actualTime['minutes']) + '_'
	else:
		dateString += str(actualTime['minutes']) + '_'

	logName = dateString

	if actualTime['seconds'] < 10:
		dateString += '0' + str(actualTime['seconds']) + '_'
	else:
		dateString += str(actualTime['seconds']) + '_'



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
			serialFlag = 0
			dataFlag  = ProcessData()
			if dataFlag == PROCESSED_VALID_DATA:
				logFlag = SaveLog()
				if logFlag != LOG_SUCCESS:
					print "Error saving data"


		
		
		



try:  
	main() 
except KeyboardInterrupt:
	ser.close()  
	#GPIO.cleanup()       # clean up GPIO on CTRL+C exit
	print "KeyboardInterrupt"
# except Exception: 
# 	pass
#GPIO.cleanup()           # clean up GPIO on normal exit 
