#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>  // Used for isatty, to check for redirection

#define SHM_NAME "/my_shm"
#define BUF_SIZE 4096
#define SEGMENT_SIZE 15
#define MAX_SEGMENTS (BUF_SIZE / SEGMENT_SIZE)

int debug_mode = 0;

/* Shared memory structure */
struct thread_args {
    struct shared_memory* shm;
    struct report* rep;
};

struct shared_memory {
    char buf[BUF_SIZE];       // Buffer for message segments
    int segment_count;        // Number of segments
    sem_t sem1;               // Semaphore for synchronization
    sem_t sem2;               // Semaphore for synchronization
    int complete;             // Flag to indicate message completion
    int termination_flag;     // Flag to indicate termination
    // message sent from process_a to process_b
    int message_A_to_B;
    // message sent from process_b to process_a
    int message_B_to_A;
};

/* Reporting structure */
struct report {
    int messages_sent;
    int messages_received;
    int total_segments;
    double avg_wait_time;
};



/* Function to segment messages */
void segment_message(const char* message, struct shared_memory* shm) {
    int len = strlen(message);
    int segment_index = 0;
    shm->segment_count = 0;

    if (debug_mode) {
        printf("--Segmenting message: %s", message);
    }

    while (segment_index * SEGMENT_SIZE < len) {
        strncpy(&shm->buf[segment_index * SEGMENT_SIZE], &message[segment_index * SEGMENT_SIZE], SEGMENT_SIZE);
        shm->segment_count++;
        segment_index++;
    }

    shm->complete = 1;  // Indicate that the message segmentation is complete
}

/* Function to reassemble messages */
void reassemble_message(char* message, struct shared_memory* shm) {
    if (debug_mode) {
        printf("--Reassembling message...");
    }
    strcpy(message, "");  // Clear the message buffer
    for (int i = 0; i < shm->segment_count; i++) {
        strncat(message, &shm->buf[i * SEGMENT_SIZE], SEGMENT_SIZE);
    }
}

/* Thread function for sending messages */
void* send_message(void* arg) {
    struct thread_args* args = (struct thread_args*)arg;
    struct shared_memory* shm = args->shm;
    struct report* rep = args->rep;
    char message[BUF_SIZE];

    while (1) {
        
        if (isatty(fileno(stdin))) {
            printf("Enter message: ");
        }
        fgets(message, BUF_SIZE, stdin);

        if (shm->termination_flag){
            break;
        }


        // Check for termination signal
        if (strcmp(message, "#BYE#\n") == 0) {
            if (debug_mode) {
                printf("\n--Termination signal sent\n");
            }
            shm->termination_flag = 1;
            sem_post(&shm->sem2);
            break;
        }

        sem_wait(&shm->sem1);
        if (debug_mode) {
            printf("--Sending message: %s", message);
        }
        segment_message(message, shm);  // Segment and send the message
        rep->messages_sent++;
        rep->total_segments += shm->segment_count;
        shm->complete = 1;  // Indicate that the message segmentation is complete
        if (debug_mode) {
            printf("--Message sent from process A to process B\n");
        }
        sem_post(&shm->sem2);
        shm->message_A_to_B= 1;
        shm->message_B_to_A = 0;


    }
    
    return NULL;
}


/* Thread function for receiving messages */
void* receive_message(void* arg) {
    struct thread_args* args = (struct thread_args*)arg;
    struct shared_memory* shm = args->shm;
    struct report* rep = args->rep;
    char message[BUF_SIZE];
    struct timeval start, end;
    long seconds, useconds;
    double total_time;
    int first_segment_received;

    while (1) {
        if (shm->termination_flag) {
            if (debug_mode) {
                printf("\n--Termination flag received in receive_message\n");
            }
            sem_post(&shm->sem1);
            break;
        }

        first_segment_received = 0;
        gettimeofday(&start, NULL);  // Start timer before waiting for a new message

        sem_wait(&shm->sem2);

        if (shm->complete && shm->message_A_to_B == 0) {
            if (!first_segment_received) {
                gettimeofday(&end, NULL);  // Stop timer after receiving first segment
                if (debug_mode) {
                    printf("\n--First segment received\n");
                }
                seconds = end.tv_sec - start.tv_sec;
                useconds = end.tv_usec - start.tv_usec;
                total_time = seconds + useconds / 1000000.0;
                rep->avg_wait_time += total_time;

                first_segment_received = 1;  // Indicate that the first segment has been received
            }
            if (debug_mode) {
                printf("--Message received from process B to process A\n");
            }
            reassemble_message(message, shm);
            printf("\n-Received: %s", message);
            printf("Enter message: ");
            fflush(stdout);
            rep->messages_received++;
            shm->complete = 0;  // Reset the complete flag
            sem_post(&shm->sem1);
        } else {
            sem_post(&shm->sem2);
        }
    }

    return NULL;
}


int main(int argc, char *argv[]) {
    int fd;
    struct shared_memory* shm;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-debug") == 0) {
            debug_mode = 1;
            printf("--Debug mode enabled\n");
        } else if (strcmp(argv[i], "-file") == 0 && i + 1 < argc) {
            // Redirect stdout to the specified file
            if (freopen(argv[++i], "w", stdout) == NULL) {
                perror("Failed to open file for writing");
                exit(EXIT_FAILURE);
            }
        }
    }


    /* Create shared memory object */
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    /* Set the size of the shared memory object */
    if (ftruncate(fd, sizeof(struct shared_memory)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    
    /* Map the shared memory object */
    shm = (struct shared_memory*)mmap(NULL, sizeof(*shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* Initialize semaphores */
    sem_init(&shm->sem1, 1, 1);  // Initial value 1
    sem_init(&shm->sem2, 1, 0);  // Initial value 0

    pthread_t send_thread, receive_thread;
    struct report rep = {0, 0, 0, 0.0};

    /* Create threads for sending and receiving messages */
    struct thread_args args;
    args.shm = shm;
    args.rep = &rep;

    shm->termination_flag = 0;

    /* Create threads for sending and receiving messages */
    pthread_create(&send_thread, NULL, send_message, (void*)&args);
    pthread_create(&receive_thread, NULL, receive_message, (void*)&args);

    // Loop to check for termination flag
    while (!shm->termination_flag) {
        sleep(1); // Sleep for a short while before checking again
    }

    // If termination flag is set, cancel the send_message thread
    pthread_cancel(send_thread);

    /* Wait for threads to finish */
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    /* Print report */
    printf("\n\nReport:\n");
    printf("Messages sent: %d\n", rep.messages_sent);
    printf("Messages received: %d\n", rep.messages_received);

    if (rep.messages_sent > 0) {
        printf("Average segments per message sent: %.2f\n", (double)rep.total_segments / rep.messages_sent);
    }
    if (rep.messages_received > 0) {
        printf("Average segments per message received: %.2f\n", (double)rep.total_segments / rep.messages_received);
    }

    printf("Total segments: %d\n", rep.total_segments);
    
    if (rep.messages_received > 0) {
        printf("Average wait time: %.2f seconds\n", rep.avg_wait_time / rep.messages_received);
    }

    /* Cleanup */
    sem_destroy(&shm->sem1);
    sem_destroy(&shm->sem2);
    munmap(shm, sizeof(*shm));
    shm_unlink(SHM_NAME);

    return 0;
}
