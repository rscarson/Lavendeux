from lavendeux import Types, Errors

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)
	print args[0]
	return (Types.STRING, args[0])