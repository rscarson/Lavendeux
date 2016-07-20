# Color complement
# 	As function: 1 arg... Hexadecimal color code, return Complementary color code

from lavendeux import Types, Errors
import colorsys

def call(args):
	# Check number of arguments
	if len(args) != 1:
		return (Types.ERROR, Errors.INVALID_ARGS)

	# Get RBG bytes, and convert
	r,g,b = extractRGB(args[0])
	r, g, b = [x/255 for x in r, g, b]
	h,l,s = colorsys.rgb_to_hls(r,g,b)

	print r
	print g
	print b

	# Complement and convert back
	h = (h + 0.5) % 1.0
	r,g,b = colorsys.hls_to_rgb(h,l,s)
	r, g, b = [x*255 for x in r, g, b]

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
	val = val | int(round(r))
	val = val << 8
	val = val | int(round(g))
	val = val << 8
	val = val | int(round(b))

	return val