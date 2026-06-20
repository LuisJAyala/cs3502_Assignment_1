#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>   


volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t print_stats_flag = 0;
int total_lines = 0;
int total_chars = 0;
void handle_sigint(int sig) {
    shutdown_flag = 1; 
}
void handle_sigusr1(int sig) {
    print_stats_flag = 1;

}
int main(int argc, char *argv[]) {
    int max_lines = -1;          
    int verbose = 0;             
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

    while ((opt = getopt(argc, argv, "n:v")) != -1) {
        switch (opt) {
            case 'n':
                max_lines = atoi(optarg);
                if (max_lines < 0) {
                    fprintf(stderr, "Error: Max lines limit cannot be negative.\n");
                    exit(1);
                }
                break;

            case 'v':
               verbose = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n max] [-v]\n", argv[0]);
                exit(1);
        }
    }
    long long total_lines = 0;
    long long total_chars = 0;
    int ch;

    while ((ch = getchar()) != EOF) {
        if (max_lines != -1 && total_lines >= max_lines) {
           break;
       }
        total_chars++;
        if (verbose) {
            putchar(ch);
        }
        if (ch == '\n') {
            total_lines++;
        }
    }
    if (ch == EOF && total_chars > 0 && (max_lines == -1 || total_lines < max_lines)) {
    }

    fprintf(stderr, "\n--- Consumer Statistics ---\n");
    fprintf(stderr, "Lines processed     : %lld\n", total_lines);
    fprintf(stderr, "Characters processed: %lld\n", total_chars);

	fprintf(stderr, "\n[Consumer Final] Total lines: %lld, Total chars: %lld\n", total_lines, total_chars);

    return 0;
}
 


