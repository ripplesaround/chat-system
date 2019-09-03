program:server
LIB = -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient



server:server.c chat.o
	gcc -o server server.c chat.o -lpthread $(LIB) `pkg-config --cflags --libs gtk+-2.0`
chat.o:chat.h chat.c
	gcc -c chat.c
clean:
	rm *.o server *.dat
