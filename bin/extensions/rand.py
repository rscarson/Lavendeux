from lavendeux import Types, Errors
import random

def call(args):
	# Check number of arguments
	if len(args) == 2:
		return (Types.INT, random.randint(int(args[0]), int(args[1])))
	elif len(args) == 0:
		return (Types.FLOAT, random.random())
	else:
		return (Types.ERROR, Errors.INVALID_ARGS)

def help():
	return "A function taking 0 or 2 arguments. 2 integer arguments forming a range from which an integer is randomly selected. Or 0 arguments, returning a random floating point number between 0 and 1."