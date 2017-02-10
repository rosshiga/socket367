all: server.c client.c server367.c client367.c
	gcc -o server server.c
	gcc -o client client.c
	gcc -o server367 server367.c
	gcc -o client367 client367.c
clean:
	rm server client
