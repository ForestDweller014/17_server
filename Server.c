#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

char in_buff[100];
char out_buff[100];
char pid_string[6];
int private_fd[2];

void sleepm(int ms) {
	struct timespec ts;
	ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, &ts);
}

void listen() {
	mkfifo("well_known_pipe", 0666);
	int wkp[2];

	while (1) {
		wkp[0] = open("well_known_pipe", O_RDONLY);
		read(wkp[0], in_buff, sizeof(in_buff));
		printf("Received data: %s\n", in_buff);
		if (!strcmp(in_buff, "SYN-ACK")) {
			break;
		} else if (!strncmp(in_buff, "SYN", 3)) {
			strcpy(pid_string, in_buff + 3);
			strcpy(out_buff, "ACK");
			wkp[1] = open("well_known_pipe", O_WRONLY);
			sleepm(100);
			write(wkp[1], out_buff, sizeof(out_buff));
			printf("Sent data: %s\n", out_buff);
		}
		close(wkp[0]);
		close(wkp[1]);
	}
}

int main() {
	while (1) {
		printf("Waiting for a connection...\n");
		listen();
		printf("Client %s connected\n", pid_string);

		while (1) {
			//Receive data from client
			private_fd[0] = open(pid_string, O_RDONLY);
			read(private_fd[0], in_buff, sizeof(in_buff));
			printf("Received data: %s\n", in_buff);

			//Process data
			strncpy(out_buff, in_buff, sizeof(in_buff));
			strcat(out_buff, " is the best");

			//Send processed data to client
			private_fd[1] = open(pid_string, O_WRONLY);
			sleepm(100);
			write(private_fd[1], out_buff, sizeof(out_buff));
			printf("Sent data: %s\n", out_buff);

			//Close private pipe
			close(private_fd[0]);
			close(private_fd[1]);
		}
	}
	return 0;
}