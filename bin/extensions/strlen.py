from lavendeux import Types, Errors

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)
	return (Types.INT, len(args[0]))

def help():
	return "Function taking one string argument, returning its length."