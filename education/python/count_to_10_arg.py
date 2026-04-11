#!/usr/bin/env python3
###############################################################################
# 
# count_to_10_arg.py(1) -- Print integers from 1 to number given on cmd line.
#
###############################################################################
import sys
#import pxssh
from optparse import OptionParser

#
# Parse options.
#
version = '1.0'
parser = OptionParser(usage="usage: %prog [options]", version="%prog " + version)
#parser.add_option("-j", "--json",
#                    action="store",
#                    dest="json",
#                    type="int",
#                    metavar="JSON",
#                    default="0",
#                    help="produce JSON-formatted output")
#parser.add_option("-n", "--nodes",
#                    action="store",
#                    dest="nodes",
#                    type="string",
#                    metavar="TS_NUM_NODES",
#                    default="0",
#                    help="specify number of nodes present in this config")
#parser.add_option("-r", "--rundir",
#                    action="store",
#                    dest="rundir",
#                    type="string",
#                    metavar="TS_RUN_DIR",
#                    default="/var/tidalscale/disk",
#                    help="run directory on local TS platform")
#parser.add_option("-v", "--verbose",
#                    action="store",
#                    dest="verbose",
#                    type="int",
#                    metavar="VERBOSE",
#                    default="0",
#                    help="produce verbose output")
parser.add_option("-n", "--number",
                    action="store",
                    dest="number",
                    type="int",
                    metavar="VERBOSE",
                    default="10",
                    help="count to this number")

(options, args) = parser.parse_args()

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

print_range(options.number)
exit()
