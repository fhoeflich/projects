#!/usr/bin/env python3
###############################################################################
# 
# justified_fields.py(1) -- http://stackoverflow.com/questions/42569812/python-how-to-write-to-a-specific-positioncolumn-in-text-file
#
###############################################################################
import sys

# using a comma
#for x in range(10):
#    print (x),
#print ("\n")

# using sys.stdout.write()
#for x in range(10):
#    sys.stdout.write(str(x))
#    sys.stdout.write(' ')
#sys.stdout.write('\n')

fhand = open('test.txt','w')    
id,name,email = 1,'abc','abc@email.com'
id1,name1,email1 = 2,'adfadsfbnn','addbnn@email.com'
fhand.write('%s %10s %30s \n' %(id,name,email) )
print(id,name,email)
fhand.write('%s %10s %30s \n' %(id1,name1,email1) )
print(id1,name1,email1)
fhand.close()

#test.txt file contains,
#1        abc                  abc@email.com 
#2 adfadsfbnn               addbnn@email.com

exit()
