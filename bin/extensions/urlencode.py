from lavendeux import Types, Errors
from urllib import quote_plus

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)
	return (Types.STRING, quote_plus(args[0]))

def help():
	return "Function taking one string argument, returning a urlencoded version."