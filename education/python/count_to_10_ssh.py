#!/usr/bin/env python3
###############################################################################
# 
# count_to_10_ssh.py(1) -- ssh to a server, invoke count_to_10.py there,
#				and expect the result.
#
###############################################################################
import sys
#import pxssh				# Python 2-ism ...
from pexpect import pxssh	# Python 3 version (requires pexpect)
from optparse import OptionParser

#
# Parse options.
#
version = '1.0'
parser = OptionParser(usage="usage: %prog [options]", version="%prog " + version)
parser.add_option("-n", "--number",
                    action="store",
                    dest="number",
                    type="int",
                    metavar="NUMBER",
                    default="10",
                    help="count to this number")

parser.add_option("-s", "--server",
                    action="store",
                    dest="server",
                    type="string",
                    metavar="SERVER",
                    default="piranha",
                    help="server to ssh and count on")

(options, args) = parser.parse_args()
#print ("server: %s" % options.server)
#print ("number: %d" % options.number)

s = pxssh.pxssh()
s.login(options.server,'tsadmin','')
if 0:
    print ("SSH session failed on login.")
    print (str(s))
    exit()

s.setecho(False)
s.sendline('/var/tmp/practice/count_to_10.py')
s.expect("0 1 2 3 4 5 6 7 8 9 \r\n")	# yes, you need the trailing space!!

s.logout()
exit()
