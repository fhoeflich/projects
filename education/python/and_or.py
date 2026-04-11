#!/usr/bin/env python3
###############################################################################
# 
# and_or.py(1) -- Simple logical operators.
#
###############################################################################
import sys

# print expression as a string
five = 5
print ('Expression as a string:')
two = 2
print (str(five) + " and " + str(two))

print

# evaluate expression and print result
print ('Evaluated expression:')
print (five + two)

print

# logical stuff
if not(two and five):
    print ("two and five")

if (0xf & 0x8):
    print ("0xf and 0x8")

exit()
