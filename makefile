all: server client

server: Server.c
	gcc -o server Server.c

client: Client.c
	gcc -o client Client.c