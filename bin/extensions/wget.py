from lavendeux import Types, Errors
from  urllib2 import urlopen
from urlparse import urlparse

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)

	try:
		args[0] = adjust_url(args[0])
		response = urlopen(url=args[0], timeout=0.5)
		html = response.read()
	except (IOError, ValueError):
		return (Types.STRING, args[0])

	return (Types.STRING, html)

def adjust_url(url):
	url_info = urlparse(url)
	if not url_info.scheme:
		url = 'http://'+url
	return url

def help():
	return "A function taking 1 argument. Fetches a network resource."