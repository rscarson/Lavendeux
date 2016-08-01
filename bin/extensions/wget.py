# coding: utf-8

from lavendeux import Types, Errors
from  urllib2 import urlopen
import encodings.idna

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)

	try:
		response = urlopen(url=args[0], timeout=0.5)
		html = response.read()
	except (IOError, ValueError):
		return (Types.STRING, args[0])

	return (Types.STRING, html)

def help():
	return "A function taking 1 argument. Fetches a network resource."