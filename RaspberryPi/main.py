#===========================================================================


#=== Libraries =============================================================

#=== Objects ===============================================================

#=== Constants =============================================================

#=== Variables =============================================================

#=== Functions =============================================================
def SaveLog():
	print "Save Log"


def SendTwitter():
	print "Send Twitter"

def ReadSerialPort():
	print "Read Serial Port"

def UpdateTime():
	print "Get Time"
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
	

def ProcessData():
	print "Process Data"



#=== Main ==================================================================
def main:
	print "Main"
	while True:
		lastTime = Time
		time = UpdateTime()
		
		if lastTime.Hour != time.hour:
			SendTwitter()


try:  
	main() 
except KeyboardInterrupt:  
	GPIO.cleanup()       # clean up GPIO on CTRL+C exit
except Exception: 
	pass
GPIO.cleanup()           # clean up GPIO on normal exit 