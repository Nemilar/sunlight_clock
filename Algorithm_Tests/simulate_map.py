#!/usr/bin/python

WIDTH=32
HEIGHT=16


LEDs = {}


for latRow in range(1,HEIGHT+1):
	LEDs[latRow] = {}
	print LEDs
	thisLat = ((90-(latRow*5.625)))
	print "Starting row " + str(latRow) + " with latitude " + str(thisLat)

	for longCol in range(1,WIDTH+1):
		thisLong = ((-180+(longCol*11.25)))

		LEDs[latRow][longCol] = (thisLat, thisLong)
