from .Handlers import *
from .Classes import *

__SUPPORTED_SYSTEMS = {"Windows":["arp -a", WindowsHandler], "Linux":[["/usr/sbin/arp", "-a"], LinuxHandler], "Darwin":[["/usr/sbin/arp", "-a"], LinuxHandler]}

def Detect():
	"""
	Detect ARP poisoning attack.
	"""
	
	import platform, subprocess
	current_system = platform.system()
	
	if current_system not in __SUPPORTED_SYSTEMS.keys():
		raise NotImplementedError("{0} is not supported".format(current_system))

	command_output = subprocess.check_output(__SUPPORTED_SYSTEMS[current_system][0]).decode("utf-8")
	arp_table = __SUPPORTED_SYSTEMS[current_system][1](command_output)
	return __SearchDupliacteMACs(arp_table)

def __SearchDupliacteMACs(arp_table):
	"""
	Private method. You should not call it directly.
	Search dupliacte MAC addresses from a given ARP table.
	"""

	import itertools
	poisons_found = []

	for interface_table in arp_table.table:
		sortedEntires = sorted(interface_table.entries,key=lambda x:x[1])
		for key, group in itertools.groupby(sortedEntires, key=lambda x:x[1]):
		
			# Ignore a special MAC address (broadcast address)
			if key == "ff-ff-ff-ff-ff-ff" or key == "ff:ff:ff:ff:ff:ff":
				continue

			entriesForMac = list(group)
			if len(entriesForMac) > 1:			
				ip_addresses = []

				# collect all the IP addresses to a list
				for ip_mac_entry in entriesForMac:
					ip_addresses.append(ip_mac_entry[0])
				
				poison_details = ArpPoisonDetails(interface_table.ip, key, ip_addresses)

				poisons_found.append(poison_details)

	return poisons_found
