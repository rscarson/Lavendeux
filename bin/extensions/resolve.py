from lavendeux import Types, Errors
from socket import gethostbyname, setdefaulttimeout

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)

	setdefaulttimeout(0.5)
	try:
		return (Types.STRING, gethostbyname(args[0]))
	except socket.gaierror:
		return (Types.STRING, args[0])

def help():
	return "Function which resolves a domain name to an IP address."