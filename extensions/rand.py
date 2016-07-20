# Random integer generation
# 	As function: 2 args, return int between a and b -> rand(a, b)

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