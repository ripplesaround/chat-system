program:client server
client:client.c
	gcc -o client client.c `pkg-config --cflags --libs gtk+-2.0`

server:server.c
	gcc -o server server.c `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm *.o server client