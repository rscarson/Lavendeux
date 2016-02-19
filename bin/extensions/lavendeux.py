class Types:
	ERROR = 0
	FLOAT = 1
	INT = 2

class Errors:
	UNKNOWN = 0
	INVALID_ARGS = 2
	INVALID_NAME = 3
	SYNTAX_ERROR = 4
	ALLOCATION = 5
	TYPE = 6
	BAD_EXTENSION = 7

def call(args):
	return (Types.ERROR, Errors.BAD_EXTENSION)