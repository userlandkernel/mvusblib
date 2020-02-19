class ArpTable:
	"""
	Represents an ARP table. Consists of a list of ArpInterface entries.
	"""

	def __init__(self):
		self.table = []

	def __str__(self):
		toRet = {}
		for interface in self.table:
			toRet[interface.ip] = interface.entries
		return str(toRet)

	def __repr__(self):
		return self.__str__()

class ArpInterface:
	"""
	Represents an network interface. Consists of the address of the interface and its ARP entries.
	"""

	def __init__(self):
		self.ip = ""
		self.entries = []

	def __str__(self):
		return "Interface IP: " + self.ip
	
	def __repr__(self):
		return self.__str__()

class ArpPoisonDetails:
	"""
	Represens a single ARP poison attack that was detected. Consists of the IP of the interface, the MAC address of the attack and the IP addresses that present themselves as this MAC address.
	"""

	def __init__(self, interface, MAC, ip_addresses):
		self.interface = interface
		self.MAC = MAC
		self.ip_addresses = ip_addresses

	def __str__(self):
		return "MAC: " + self.MAC + ", Addresses: " + str(self.ip_addresses)

	def __repr__(self):
		return self.__str__()
