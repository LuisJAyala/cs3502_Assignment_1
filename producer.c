#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>   

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t print_stats_flag = 0;
int total_lines = 0; 
void handle_sigint(int sig) {
  shutdown_flag = 1; 
}
void handle_sigusr1(int sig) {
    print_stats_flag = 1;
}
int main(int argc, char *argv[]) {
	char *filename = NULL;
	int buffer_size = 4096;
	int opt;
struct sigaction sa_int;
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    struct sigaction sa_usr1;
    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = SA_RESTART; 
    sigaction(SIGUSR1, &sa_usr1, NULL);


	while ((opt = getopt(argc, argv, "f:b:")) != -1) {
	switch (opt) {
		case 'f':
		filename = optarg;
		break;
		case 'b': 
		buffer_size = atoi(optarg);
		if (buffer_size <= 0) {
		fprintf(stderr, "error:: Bufer size must be greater than 0\n");
		return 1;
		}
	break;

		
		default:
		fprintf(stderr, "Usage: %s [-f file] [-b siz]\n", argv[0]);
		exit(1);
	}
}

FILE *input_stream = stdin;
if (filename != NULL) {
	input_stream = fopen(filename, "r"); 
	if (input_stream == NULL) {
		perror("error opening input file");
		return 1;
	}
}

char *buffer =malloc(buffer_size);
if (buffer == NULL) {
	perror("Memory allocation failed");
	if (filename != NULL) fclose(input_stream);
	return 1;
} 
size_t read_bytes;
	while ((read_bytes = fread(buffer, 1, buffer_size, input_stream)) > 0) {
	size_t bytes_written = fwrite(buffer, 1, read_bytes, stdout);
	if(bytes_written < read_bytes) {
	perror("Error writing to stdout");
	free(buffer);
	if (filename != NULL) fclose(input_stream);
	return 1;
	}
for (size_t i = 0; i < read_bytes; i++) {
        if (buffer[i] == '\n') {
            total_lines++;
        }
    }

}

free(buffer);
if (filename != NULL) {
fclose(input_stream);
}

fprintf(stderr, "\n[Producer Final] Total lines sent: %d\n", total_lines);

return 0;
}
