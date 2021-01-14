#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

char in_buff[100];
char out_buff[100];
char pid_string[6];
int private_fd[2];

char *add_terminate(char *in_buff) {
	char *temp = in_buff;
	int i = 0;
	while(*(in_buff + i) != '\n') {
		i++;
	}
	*(in_buff + i) = 0;
	return temp;
}

void connect() {
	int wkp[2];

	strcpy(out_buff, "SYN");
	sprintf(pid_string, "%d", getpid());
	strcat(out_buff, pid_string);
	wkp[1] = open("well_known_pipe", O_WRONLY);
	write(wkp[1], out_buff, sizeof(out_buff));
	printf("Sent data: %s\n", out_buff);
	close(wkp[1]);

	while (1) {
		wkp[0] = open("well_known_pipe", O_RDONLY);
		read(wkp[0], in_buff, sizeof(in_buff));
		printf("Received data: %s\n", in_buff);
		if (!strcmp(in_buff, "ACK")) {
			mkfifo(pid_string, 0666);
			strcpy(out_buff, "SYN-ACK");
			wkp[1] = open("well_known_pipe", O_WRONLY);
			write(wkp[1], out_buff, sizeof(out_buff));
			printf("Sent data: %s\n", out_buff);
			break;
		}
		close(wkp[0]);
		close(wkp[1]);
	}
}

int main() {
	printf("Connecting to server...\n");
	connect();
	printf("Connection established\n");

	while (1) {
		//Prompt user
		printf("Input data:\n");
		fgets(out_buff, sizeof(out_buff), stdin);
		add_terminate(out_buff);

		//Send data to server
		private_fd[1] = open(pid_string, O_WRONLY);
		write(private_fd[1], out_buff, sizeof(out_buff));
		printf("Sent data: %s\n", out_buff);

		//Receive processed data from server
		private_fd[0] = open(pid_string, O_RDONLY);
		read(private_fd[0], in_buff, sizeof(in_buff));
		printf("Received data: %s\n", in_buff);

		//Close private pipe
		close(private_fd[0]);
		close(private_fd[1]);
	}
	return 0;
}