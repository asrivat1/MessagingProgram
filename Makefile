CC=gcc
LD=gcc
CFLAGS=-g -Wall 
CPPFLAGS=-I. -I/home/cs437/exercises/ex3/include
SP_LIBRARY_DIR=/home/cs437/exercises/ex3

all: server client

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

server:  $(SP_LIBRARY_DIR)/libspread-core.a server.o lamp_struct.o room.o like_list.o server_include.o msg_stack.o user_list.o
	$(LD) -o $@ server.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a lamp_struct.o room.o like_list.o server_include.o msg_stack.o user_list.o

client:  $(SP_LIBRARY_DIR)/libspread-core.a client.o lamp_struct.o room.o msg_stack.o like_list.o server_include.o user_list.o
	$(LD) -o $@ client.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a lamp_struct.o room.o msg_stack.o like_list.o server_include.o user_list.o

clean:
	rm -f *.o sp_user class_user server client

