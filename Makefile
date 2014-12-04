CC=gcc
LD=gcc
CFLAGS=-g -Wall 
CPPFLAGS=-I. -I/home/cs437/exercises/ex3/include
SP_LIBRARY_DIR=/home/cs437/exercises/ex3

all: server

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

server:  $(SP_LIBRARY_DIR)/libspread-core.a server.o lamp_struct.o
	$(LD) -o $@ server.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a lamp_struct.o

clean:
	rm -f *.o sp_user class_user server

