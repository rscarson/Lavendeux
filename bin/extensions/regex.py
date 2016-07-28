from lavendeux import Types, Errors
import re

def call(args):
	# Check number of arguments
	if len(args) != 2:
		return (Types.ERROR, Errors.INVALID_ARGS)

	m = re.search(str(args[0]), str(args[1]))
	if m == None:
		return (Types.STRING, " ")
	return (Types.STRING, m.group(0))

def help():
	return "A function taking 2 string arguments. Returns the result of a search in the second argument using the regular expression in the first argument."