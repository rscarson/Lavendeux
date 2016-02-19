from lavendeux import Types, Errors

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)

	# Get RBG bytes, and convert
	r,g,b = extractRGB(args[0])
	h,s,l = toHSL(r,g,b)

	print (r,g,b)
	print (h,s,l)

	# Complement
	h = (h + 180) % 360

	# Convert back
	r,g,b = toRGB(h,s,l)

	# Pack up
	val = packRGB(r,g,b)

	return (Types.INT, val)

def extractRGB(colour):
	b = colour & 0xFF
	colour = colour >> 8
	g = colour & 0xFF
	colour = colour >> 8
	r = colour & 0xFF

	return (r,g,b)

def packRGB(r,g,b):
	val = 0
	val = val | r
	val = val << 8
	val = val | g
	val = val << 8
	val = val | b

	return val

def toHSL(r,g,b):
	h = 0
	s = 0
	l = 0

	R = r/255
	G = g/255
	B = b/255

	Cmax = max([R,B,G])
	Cmin = min([R,B,G])
	Cdelta = Cmax - Cmin

	l = (Cmax + Cmin) / 2

	if Cdelta == 0:
		h = 0
	elif R == Cmax:
		h = 60 * (((G-B)/Cdelta)%6)
	elif B == Cmax:
		h = 60 * (((B-R)/Cdelta)+2)
	else:
		h = 60 * (((R-G)/Cdelta)+4)

	if Cdelta == 0:
		s = 0
	else:
		s = Cdelta / (1 - abs(2*l - 1))

	return (h,s,l)

def toRGB(h,s,l):
	c = (1 - abs(2*l - 1)) * s
	x = c * (1 - abs((h/60)%2 - 1))
	m = l - c/2

	RGB = (0,0,0)
	if h >= 0 and h < 60:
		RGB = (c,x,0)
	elif h >= 60 and h < 120:
		RGB = (x,c,0)
	elif h >= 120 and h < 180:
		RGB = (0,c,x)
	elif h >= 180 and h < 240:
		RGB = (0,x,c)
	elif h >= 240 and h < 300:
		RGB = (x,0,c)
	elif h >= 300 and h < 360:
		RGB = (c,0,x)

	R,G,B = RGB
	r = (R+m)*255
	g = (G+m)*255
	b = (B+m)*255

	return (r,g,b)


error, value = call([0xFF0000,])
print extractRGB(value)