# Bibleajax program
# Bob Kasper, MVNU Computer Science
# updated January 2020 to use
# c++11 compiler option, current paths on cs.mvnu.edu

USER= isasavage

# Use GNU C++ compiler with C++11 standard
CC= g++
CFLAGS= -g -std=c++11

all: PutCGI PutHTML biblelookupclient.cgi biblelookupserver
# Project 3p2 Build
# PutCGI PutHTML biblelookupclient.cgi biblelookupserver
# Project 3p1 Build
# testreader
# Project 1 & 2 Build
# bibleajax.cgi PutCGI PutHTML

bibleajax.cgi:	bibleajax.o Ref.o Verse.o Bible.o
		$(CC) $(CFLAGS) -o bibleajax.cgi bibleajax.o Ref.o Verse.o Bible.o -lcgicc
		# -l option is necessary to link with cgicc library

biblelookupclient.cgi : biblelookupclient.o Bible.o Verse.o Ref.o fifo.o logfile.h
		$(CC) $(CFLAGS) -o biblelookupclient.cgi biblelookupclient.o Bible.o Verse.o Ref.o fifo.o -lcgicc

testreader: Ref.o Verse.o Bible.o testreader.o
	$(CC) $(CFLAGS) -o testreader Ref.o Verse.o Bible.o testreader.o

biblelookupserver : biblelookupserver.o Bible.o Verse.o Ref.o fifo.o
		$(CC) $(CFLAGS) -o biblelookupserver biblelookupserver.o Bible.o Verse.o Ref.o fifo.o

biblelookupclient.o : biblelookupclient.cpp Bible.h Verse.h Ref.h fifo.h logfile.h
		$(CC) $(CFLAGS) -c biblelookupclient.cpp

biblelookupserver.o : biblelookupserver.cpp Bible.h Verse.h Ref.h fifo.h
		$(CC) $(CFLAGS) -c biblelookupserver.cpp

bibleajax.o:	bibleajax.cpp
		$(CC) $(CFLAGS) -c bibleajax.cpp

testreader.o : Ref.h Verse.h Bible.h testreader.cpp
	$(CC) $(CFLAGS) -c testreader.cpp

# piping mechanism
fifo.o : fifo.h fifo.cpp
	$(CC) $(CFLAGS) -c fifo.cpp

Ref.o : Ref.h Ref.cpp
	$(CC) $(CFLAGS) -c Ref.cpp

Verse.o : Ref.h Verse.h Verse.cpp
	$(CC) $(CFLAGS) -c Verse.cpp

Bible.o : Ref.h Verse.h Bible.h Bible.cpp
	$(CC) $(CFLAGS) -c Bible.cpp
			
PutCGI:	biblelookupclient.cgi
		chmod 755 biblelookupclient.cgi
		cp biblelookupclient.cgi /var/www/html/class/csc3004/$(USER)/cgi-bin

		echo "Current contents of your cgi-bin directory: "
		ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

PutHTML:
		cp bibleindex.html /var/www/html/class/csc3004/$(USER)
		cp styles.css /var/www/html/class/csc3004/$(USER)

		echo "Current contents of your HTML directory: "
		ls -l /var/www/html/class/csc3004/$(USER)

clean:		
		rm *.o core bibleajax.cgi testreader biblelookupclient.cgi biblelookupserver