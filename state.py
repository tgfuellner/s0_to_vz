#!/usr/bin/python

#import random

print "Content-type: text/html\n"

with open('/tmp/s0Events') as fd:
    eventCount = int(fd.read())

# if random.randint(1, 2) == 1:

if eventCount < 8 :
	print("build:__green__")
else:
	print("build:__red__")
	
