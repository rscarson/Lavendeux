from lavendeux import Types, Errors
from socket import getfqdn, setdefaulttimeout

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)

	setdefaulttimeout(0.5)
	try:
		return (Types.STRING, getfqdn(args[0]))
	except socket.gaierror:
		return (Types.STRING, args[0])

def help():
	return "Function which translates an ip address into its associated domain name."