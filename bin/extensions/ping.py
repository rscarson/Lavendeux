from lavendeux import Types, Errors
import socket
import datetime

TYPE_ICMP = 1
ICMP_PORT = 7
PAYLOAD = "\x08\x00\x19\x2f\x00\x00\x00\x00\x70\x69\x6e\x67"

def _ping(host):
	s = socket.socket(socket.AF_INET, socket.SOCK_RAW, TYPE_ICMP)
	s.settimeout(0.5)
	s.connect( (host, ICMP_PORT) )

	before = datetime.datetime.now()
	s.sendall(PAYLOAD)
	data = s.recv(255)
	after = datetime.datetime.now()

	if not data:
		return -1

	delta = after - before
	return delta.microseconds/1000

def ping(host):
	try:
		return _ping(host)
	except (OSError, socket.error):
		print("Must be run as administrator for ping")
		return -1
	except socket.gaierror:
		print("name resolution error")
		return -1
	except socket.timeout:
		print("timeout error")
		return -1

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)
	return (Types.INT, ping(args[0]))

def help():
	return "Ping a host, returning number of milliseconds. Returns -1 on failure. Requires admin privileges."