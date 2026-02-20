#
## CSCI 370: Makefile for OCCI compilation for the project
#

G = g++-4.9
CFLAGS = -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei
all: project

project: main.cpp
	$(G) $(CFLAGS) $(LFLAGS) main.cpp -o project

clean:
	rm -f *.o project