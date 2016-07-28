from lavendeux import Types, Errors
import re

def call(args):
	# Check number of arguments
	if len(args) != 2:
		return (Types.ERROR, Errors.INVALID_ARGS)

	m = re.search(str(args[0]), str(args[1]))
	return (Types.STRING, m.group(0))