from lavendeux import Types, Errors
import time

def call(args):
	# Check number of arguments
	if len(args) != 0:
		return (Types.ERROR, Errors.INVALID_ARGS)
	return(Types.INT, int(time.time()))

def decorate(value):
	return time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime(value))

def help():
	return """A function not taking any arguments, Returns the current unix timestamp.
A decorator that formats a timestamp"""