#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Written in 2022 by Adam Klotblixt (adam.klotblixt@gmail.com)

To the extent possible under law, the author have dedicated all
copyright and related and neighboring rights to this software to the
public domain worldwide.
This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication
along with this software. If not, see
<http://creativecommons.org/publicdomain/zero/1.0/>.
"""

import os
import numpy
import sys
import argparse

parser = argparse.ArgumentParser(description="Convert a raw audio file to Zeddy Audio Jukebox format.\n\
				 Requires raw, mono, 8-bit signed, 15700 Hz audio.")

parser.add_argument("in_file",
		    help="input audio file",
		    type=str)
parser.add_argument("out_file",
		    help="output audio file",
		    type=str)
parser.add_argument("-d",
		    help="dither amount, 0.0 - 1.0, defaults to 0.25",
		    nargs="?",
		    type=float,
		    default="0.25")
parser.add_argument("-e",
		    help="error diffusion amount, 0.0 - 1.0, defaults to 0.75",
		    nargs="?",
		    type=float,
		    default="0.75")
parser.add_argument("-p",
		    help="preview, 1 or 4 bit output",
		    type=int,
		    choices=[1,4])

args = parser.parse_args()
errorFlag = False
if ((args.d < 0.0) or (args.d > 1.0)):
	print("Error: -d range is 0.0 - 1.0")
	errorFlag = True
if ((args.e < 0.0) or (args.d > 1.0)):
	print("Error: -e range is 0.0 - 1.0")
	errorFlag = True
if (errorFlag):
	quit()
#else:
#	print(args)
	
outPSG = [ 
  0,  1,  1,  2,  3,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,
  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,
  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10,
 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
]

samplePSG = [
   0 - 128,
   2 - 128,
   3 - 128,
   4 - 128,
   6 - 128,
   8 - 128,
  11 - 128,
  16 - 128,
  23 - 128,
  32 - 128,
  45 - 128,
  64 - 128,
  90 - 128,
 128 - 128,
 180 - 128,
 255 - 128
]

inputFile = args.in_file
inData = open(inputFile, 'rb')
inDataLen = os.stat(inputFile).st_size
inDataBytes = numpy.ndarray(inDataLen, numpy.int8)
inDataBytes = inData.read(inDataLen)
inData.close()

outputFile = args.out_file
outData = open(outputFile, 'wb')
outDataLen = 256 * int(inDataLen / 256)
outDataBytes = numpy.ndarray(outDataLen, numpy.int8)

print("nr of sound blocks: ", int(outDataLen / 256))

errorScale = args.e
ditherScale = args.d
error = 0
for i in range(outDataLen):
	inByte = numpy.int8(inDataBytes[i])
	dither = numpy.random.triangular(-128, 0, 127)
	fixedSample = int(inByte + (ditherScale * dither) - (errorScale * error))
	if (fixedSample < -128):
		fixedSample = -128
	elif (fixedSample > 127):
		fixedSample = 127
	outByte = outPSG[fixedSample + 128]
	error = samplePSG[outByte] - inByte

	if (args.p == 1):
		if (fixedSample < 0):
			outDataBytes[i] = -128
		else:
			outDataBytes[i] = 127
	elif (args.p == 4):	
		outDataBytes[i] = samplePSG[outByte]
	else:
		if (fixedSample < 0):
			outDataBytes[i] = outByte
		else:
			outDataBytes[i] = outByte + 128
	
outData.write(outDataBytes)
outData.close()	
