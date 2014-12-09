CC=gcc
LD=gcc
CFLAGS=-g -Wall 
CPPFLAGS=-I. -I/home/cs437/exercises/ex3/include
SP_LIBRARY_DIR=/home/cs437/exercises/ex3

all: chat_server client test_lamp_struct test_like_list test_room

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

chat_server:  $(SP_LIBRARY_DIR)/libspread-core.a chat_server.o lamp_struct.o room.o like_list.o server_include.o msg_stack.o user_list.o room_list.o client_map.o
	$(LD) -o $@ chat_server.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a lamp_struct.o room.o like_list.o server_include.o msg_stack.o user_list.o room_list.o client_map.o

client:  $(SP_LIBRARY_DIR)/libspread-core.a client.o lamp_struct.o room.o msg_stack.o like_list.o server_include.o user_list.o
	$(LD) -o $@ client.o $(SP_LIBRARY_DIR)/libspread-core.a -ldl -lm -lrt -lnsl $(SP_LIBRARY_DIR)/libspread-util.a lamp_struct.o room.o msg_stack.o like_list.o server_include.o user_list.o

test_lamp_struct: test_lamp_struct.o server_include.o msg_stack.o lamp_struct.o
	$(CC) $(CFLAGS) -o test_lamp_struct test_lamp_struct.o server_include.o msg_stack.o lamp_struct.o

test_like_list: test_like_list.o like_list.o server_include.o
	$(CC) $(CFLAGS) -o test_like_list test_like_list.o like_list.o server_include.o

test_room: test_room.o like_list.o server_include.o room.o user_list.o
	$(CC) $(CFLAGS) -o test_room test_room.o like_list.o server_include.o room.o user_list.o

clean:
	rm -f *.o sp_user class_user chat_server client test_lamp_struct test_like_list test_room

