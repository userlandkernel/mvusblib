import re
from .Classes import *

def LinuxHandler(command_output):
	lines = command_output.split("\n")

	table = ArpTable()

	entries = {}

	for line in lines:
		if line == "":
			continue

		line_parts = re.match(r".*\((.*)\) at (.*) \[.*\] on (.*)", line)
		
		# if the line is not in the format of the provided regex
		if line_parts == None:
			continue

		ip = line_parts.group(1)
		mac = line_parts.group(2)
		interface = line_parts.group(3)
		
		if interface not in entries:
			entries[interface] = []

		entries[interface].append([ip, mac])

	for interface in entries:
		arpInterface = ArpInterface()
		arpInterface.ip = interface
		arpInterface.entries = entries[interface]
		table.table.append(arpInterface)

	return table

def WindowsHandler(command_output):
	interfaces = command_output.split("Interface: ")
	del interfaces[0] # remove the first entry of the list as it is empty

	arp_table = ArpTable()

	for interface_string in interfaces:
		outputParts = re.match(r"(.*) --- 0x(.*?)\r\n  Internet Address      Physical Address      Type\r\n (.*)\r\n",interface_string, re.M | re.S)
		interface_ip = outputParts.group(1)
		entries_string = outputParts.group(3)
		entriesByRow = entries_string.split("\r\n")
		
		arp_interface = ArpInterface()
		arp_interface.ip = interface_ip

		for entryRow_string in entriesByRow:
			if entryRow_string == "":
				continue

			entryParts = entryRow_string.split()
			arp_interface.entries.append([entryParts[0],entryParts[1]])
		
		arp_table.table.append(arp_interface)
	
	return arp_table