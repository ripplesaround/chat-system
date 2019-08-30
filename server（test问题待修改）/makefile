program:server

server:server.c chat.o
	gcc -o server server.c chat.o -lpthread
chat.o:chat.h chat.c
	gcc -c chat.c
clean:
	rm *.o server
