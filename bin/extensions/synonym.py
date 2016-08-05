# Thesaurus service provided by words.bighugelabs.com
# ------------------------
# PLEASE GET AN API KEY FROM THE FOLLOWING URL AND ADD IT TO apis.py:
# https://words.bighugelabs.com/api.php
# Only 1000 total requests per day for all users can be made via the default key
# ------------------------

from lavendeux import Types, Errors
from apis import ApiKeys
from random import choice
from urllib2 import urlopen
from urllib import quote_plus
from json import loads

def call(args):
	# Check number of arguments
	if len(args) not in (1, 2):
		return (Types.ERROR, Errors.INVALID_ARGS)

	if ApiKeys.SYNONYM != None:
		return (Types.STRING, synonym(args[0], args[1] if len(args)>1 else None, ApiKeys.SYNONYM))

	url = u'http://dev.richardcarson.ca/lavendeux.php?function=synonym&word='+quote_plus(args[0])
	if len(args) == 2:
		url += u'&type='+quote_plus(args[1])
	try:
		response = urlopen(url=url, timeout=0.5)
		value = response.read()
	except (IOError, ValueError):
		return (Types.STRING, args[0])

	return (Types.STRING, value)

def synonym(word, word_type, key):
	url = "http://words.bighugelabs.com/api/2/"+key+"/"+quote_plus(word)+"/json"
	try:
		response = urlopen(url=url, timeout=0.5)
		value = response.read()
		if len(value) > 0:
			value = loads(value)
	except (IOError, ValueError):
		return word

	if word_type != None and word_type in value.keys():
		return choice(value[word_type]['syn'])
	typed = value[choice(value.keys())]
	return choice(typed['syn'])


def help():
	return "A function taking 1 or 2 arguments. Returns a random synonym of the first argument. noun or verb can be specified as the second argument"