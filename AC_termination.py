#### imports ####

import sys
import time
from datetime import datetime

try:
    import requests
except ImportError:
    sys.exit("Error: dependency missing: requests. run: \"pip install requests\" ")

try:
    import schedule
except ImportError:
    sys.exit("Error: dependency missing: schedule. run: \"pip install schedule\" ")

#### code ####
def pushTheButton(AC_server_IP, PushDuration):
	URL_push = "http://" + AC_server_IP + "/?button1on"
	URL_release = "http://" + AC_server_IP + "/?button1off"

	requests.get(url = URL_push)
	print(datetime.now().strftime("%H:%M:%S"), ": ", URL_push)
	time.sleep(PushDuration)
	requests.get(url = URL_release)
	print(datetime.now().strftime("%H:%M:%S"), ": ",URL_release)



def scheduledJob(AC_server_IP, PushDuration):
	pushTheButton(AC_server_IP, PushDuration)
	return

def startSchedular(AC_server_IP: str, PushDuration: int, timeToPushTheButton: str):
	print("starting schedular")
	print("AC_server_IP: ", AC_server_IP)
	print("PushDuration: ", PushDuration)
	print("timeToPushTheButton: ", timeToPushTheButton)

	schedule.every().day.at(timeToPushTheButton).do(scheduledJob, AC_server_IP, PushDuration)
	while True:
		schedule.run_pending()
		time.sleep(60) # wait one minute



def main_():
	cmdArgument = ""
	if len(sys.argv) < 3:
		print("usage: python AC_termination.py <AC_server_IP> <PushDuration> [timeToPushTheButton]\n")
		print("	AC_server_IP: the IP of the AC termination server. format: 192.168.0.1")
		print("	PushDuration: the duration the button will be held in the pushed state before released, in seconds")
		print("	timeToPushTheButton: optional - push the button every day at this time. format: hour:minute\n\
                             If left blank the button will be pushed immediatly and only once")
	else:
		AC_server_IP = sys.argv[1]
		PushDuration = int(sys.argv[2])
		if len(sys.argv) == 3:
			pushTheButton(AC_server_IP, PushDuration)
		else: #argv > 3
			timeToPushTheButton = sys.argv[3]
			startSchedular(AC_server_IP, PushDuration, timeToPushTheButton)

if __name__ == "__main__":
  try:
      main_()
  except KeyboardInterrupt:
      try:
          sys.exit(0)
      except SystemExit:
          os._exit(0)

