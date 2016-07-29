from lavendeux import Types, Errors
from random import choice

def call(args):
	# Check number of arguments
	if len(args) == 0:
		return (Types.ERROR, Errors.INVALID_ARGS)
	item = choice(args)
	if type(item) is int or type(item) is long:
		return (Types.INT, item)
	elif type(item) is float:
		return (Types.FLOAT, item)
	else:
		return (Types.STRING, item)


def help():
	return "Function returning one of its arguments at random."