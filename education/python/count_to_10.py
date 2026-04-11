#!/usr/bin/env python3
###############################################################################
# 
# count_to_10.py(1) -- Print integers from 1 to 10.
#
###############################################################################
import sys

# using a comma
for x in range(10):
    print (x),
print ("\n")

# using sys.stdout.write()
for x in range(10):
    sys.stdout.write(str(x))
    sys.stdout.write(' ')
sys.stdout.write('\n')

exit()
