#!/usr/bin/python

#import random

print "Content-type: text/html\n"

with open('/tmp/s0Events') as fd:
    eventCount = int(fd.read())

# if random.randint(1, 2) == 1:

if eventCount < 7 :
	print("build:__green__")
elif eventCount > 9:
	print("build:__red__")
else:
	print("build:__yellow__")
