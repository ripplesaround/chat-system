main:main.o login_window.o chatting_window.o chat.o
	gcc -o $@ $^ `pkg-config --cflags --libs gtk+-2.0`
main.o:main.c
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
login_window.o:login_window.c login_window.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
chatting_window.o:chatting_window.c chatting_window.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
chat.o:chat.c chat.h
	gcc -c $<
all:main
clean:all
	rm -f *.o
	
