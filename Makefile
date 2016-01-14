CC=gcc
LIBS=-lwiringPi -lcurl

s0_to_vz: s0_to_vz.c
	gcc -o $@ $^ $(LIBS)
