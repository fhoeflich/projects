#!/usr/bin/env python3

p = (4, 5)
x, y = p
print ("x is", x)
print ("y is", y)

print ("")

data = [ 'ACME', 50, 91.1, (2012, 12, 21) ]
name, shares, price, date = data
print ("name is", name)
print ("shares is", shares)
print ("price is", price)
print ("date is", date)

print ("")

name, shares, price, (year, month, day) = data
print ("name is", name)
print ("shares is", shares)
print ("price is", price)
print ("year is", year)
print ("month is", month)
print ("day is", day)

