#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_LEN 256

typedef struct {
    const char *hostname;
    int port;
    const char *username;
    const char *password;
    int verbose;
} thread_args_t;

void *try_credentials(void *args) {
    thread_args_t *t_args = (thread_args_t *)args;
    ssh_session my_ssh_session = ssh_new();
    if (my_ssh_session == NULL) return NULL;

    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, t_args->hostname);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &t_args->port);

    int rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to %s:%d: %s\n", t_args->hostname, t_args->port, ssh_get_error(my_ssh_session));
        ssh_free(my_ssh_session);
        return NULL;
    }

    if (t_args->verbose) {
        printf("Attempting - Username: %s, Password: %s\n", t_args->username, t_args->password);
        fflush(stdout); // Ensure output is printed immediately
    }

    rc = ssh_userauth_password(my_ssh_session, t_args->username, t_args->password);
    if (rc == SSH_AUTH_SUCCESS) {
        printf("Credentials found - Username: %s, Password: %s\n", t_args->username, t_args->password);
    }

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
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
    } else if (pass_file != NULL) {
        // Only password list test
        while (fgets(word, sizeof(word), pass_file)) {
            // Remove newline character
            char *newline = strchr(word, '\n');
            if (newline) *newline = '\0';

            // Prepare thread arguments
            thread_args_t *t_args = malloc(sizeof(thread_args_t));
            t_args->hostname = hostname;
            t_args->port = port;
            t_args->username = NULL; // Use NULL for username since it's from password list
            t_args->password = strdup(word); // Use password list for password
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

    create_threads(hostname, port, username, password, pass_file, thread_count, verbose);

    if (pass_file != NULL) fclose(pass_file);

    return 0;
}
