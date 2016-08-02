from lavendeux import Types, Errors

def call(args):
	# Check number of arguments
	if len(args) not in (1, 2):
		return (Types.ERROR, Errors.INVALID_ARGS)

	try:
		data = read(args[0]) if len(args)==1 else read(args[0], args[1])
		return (Types.STRING, data)
	except IOError:
		return (Types.STRING, u"File not found: "+args[0])
	return (Types.STRING, args[0])

def read(filename, n=-1):
	lines = []
	with open(filename) as log:
		for line in log:
			lines.append(line.strip())
	lines.reverse()
	ret = []
	chars = 0
	for line in lines:
		if chars+len(line) > 4095 or (len(ret) >= n and n>0):
			break
		chars += len(line)
		ret.insert(0, line)
	return '\n'.join(ret)

def help():
	return "A function taking 1 or 2 arguments. Fetches as many complete lines as possible from the end of the file in argument 1, up to 4096 characters, or the number of lines in the second argument."