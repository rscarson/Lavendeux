from lavendeux import Types, Errors
import urllib

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)

	try:
		response = urllib.urlopen(args[0])
		html = response.read()
	except IOError:
		return (Types.STRING, args[0])

	return (Types.STRING, html)