#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/wait.h>

int main() {
	int pipefd[2];
	pid_t pid;
	char buffer[100];
	char *message = "hello from parent!";

	if (pipe(pipefd) == -1) {
	perror("pipe failed"); //found perror on internet. Thank you Google!
	return 1;


	pid = fork();
	if (pid == -1) {
		perror("fork failed");
		return 1;
}
	if (pid == 0) {
	close(pipefd[1]);

	int read_byte = read(pipefd[0], buffer, sizeof(buffer) - 1);
	if (read_byte < 0) {
	perror("Read failed");
	return 1;
}

buffer[read_byte] = '\0';

printf("Child received : %s/n", buffer);

close(pipefd[0]);
} else {
close(pipefd[0]);

	if (write(pipefd[1], message, strlen(message)) == -1) {
	perror("write failed");
	return 1;
}

close(pipefd[1]);

wait (NULL);

}

return 0;
}
}

