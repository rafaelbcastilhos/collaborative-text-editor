cliente:
	gcc client.c traits.h -o client

servidor:
	gcc -pthread server.c traits.h -o server

all:
	gcc -pthread server.c traits.h -o server && gcc client.c traits.h -o client

clean:
	rm -rf *.o *~ client server