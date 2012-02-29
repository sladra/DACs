SRCDIR = ./src

CC          = gcc
CFLAGS      =  -O9


all:   test cleanO

clean: 
	rm -f *.o core test

cleanO: 
	rm -f *.o   

test:   basics.o bitrankw32int.o dacs.o
	$(CC) $(CFLAGS) basics.o bitrankw32int.o dacs.o -o test $(SRCDIR)/test.c  
	
dacs.o:  basics.o bitrankw32int.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/dacs.c

bitrankw32int.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/bitrankw32int.c
	
basics.o: basics.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c
	
basics.c: basics.h
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics.c	

basics.h: 
#	touch $(SRCDIR)/$(SRCDIRUTILS)/basics.h
	
