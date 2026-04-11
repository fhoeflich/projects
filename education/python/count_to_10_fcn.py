#!/usr/bin/env python3
###############################################################################
# 
# count_to_10_fcn.py(1) -- Print integers from 1 to 10 by calling a function.
#
###############################################################################
import sys

def print_range(x):
    # using a comma
    for y in range(x):
        print (y),
    print ("\n")

    # using sys.stdout.write()
    for y in range(x):
        sys.stdout.write(str(y))
        sys.stdout.write(' ')
    sys.stdout.write('\n')

print_range(10)
exit()
