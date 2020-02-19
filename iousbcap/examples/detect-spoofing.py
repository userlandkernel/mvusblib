import os
import sys
import subprocess
import time

from Detector import Detector

while True:
	poison_results = Detector.Detect()
	if poison_results == []:
		print("No poisoning found!")
		ps = subprocess.call(['./mvdcsd', '1']) # GREEN
	else:
		print("Detected an ARP spoofing attack.")
		for poison_result in poison_results:
			print("	" + ", ".join(poison_result.ip_addresses) + " share the same MAC address.")
		ps = subprocess.call(['./mvdcsd', '3']) # RED
		exit(1)

	time.sleep(3)
