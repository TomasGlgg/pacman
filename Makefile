
all:  main

main:
	gcc base.h base.c client.h client.c server.h server.c main.c -lcurses -o main

clean:
	rm base.o client.o server.o main
