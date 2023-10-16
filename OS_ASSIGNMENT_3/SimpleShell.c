#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX_QUEUE_SIZE 100

int info[100];
int x = 0;


// Specialised Queue
typedef struct
{
    pid_t arr[100];
    int front;
    int rear;
    int count;
    int priority[100];
    sem_t mutex;
} Queue;

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        for (int i = 0; i < x; i++)
        {   
            printf("Process ID- %d\n", info[i]);  
        }     
        exit(0);
    }
}

// function to add element
void enqueue(Queue *q, pid_t to_enqueue, int priority)
{
    q->rear++;
    q->arr[q->rear] = to_enqueue;
    q->priority[q->rear] = priority;
    q->count++;
}

// function to delete element
pid_t dequeue(Queue *q)
{
    pid_t to_return = q->arr[q->front + 1];
    q->front++;
    q->count--;
    return to_return;
}

// Execution of the program code with priority
void executeProgram(Queue *j, const char *program, int priority)
{
    char strip[100];
    if (strncmp(program, "./", 2) == 0)
    {
        strncpy(strip, program + 2, sizeof(strip) - 1);
    }
    strip[sizeof(strip) - 1] = '\0';

    pid_t m = fork();
    if (m == 0)
    {
        kill(getpid(), SIGSTOP);      // Stop the process until it get signal to continue
        execlp(program, strip, NULL); // executing the command
        perror("execlp");
        exit(1);
    }
    else if (m > 0)
    {
        sem_wait(&(j->mutex));
        enqueue(j, m, priority); // enqueing the pid in the queue
        info[x] = m;
        x++;

        // Sorting the Queue to get Prioritized Queue
        for (int i = j->front + 1; i <= j->rear; i++)
        {
            for (int k = i + 1; k <= j->rear; k++)
            {
                if (j->priority[i] > j->priority[k])
                {
                    int temp = j->priority[i];
                    pid_t d = j->arr[i];
                    j->priority[i] = j->priority[k];
                    j->arr[i] = j->arr[k];
                    j->priority[k] = temp;
                    j->arr[k] = d;
                }
            }
        }
        sem_post(&(j->mutex));
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
    if (argc != 3)
    {
        printf("Take input in the correct way");
        return 1;
    }

    int NCPU = atoi(argv[1]);
    int TSLICE = atoi(argv[2]);

    // Creating the Shared memory
    int fd = shm_open("my_sm", O_RDWR | O_CREAT, 0666);
    ftruncate(fd, sizeof(Queue));
    Queue *j = mmap(0, sizeof(Queue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sem_init(&(j->mutex), 1, 1); // initialising the semaphore
    j->count = 0;
    j->front = -1;
    j->rear = -1;
    pid_t p;

    p = fork();
    if (p == 0)
    {
        char ncpu[16];
        char tslice[16];

        // Converting into String
        snprintf(ncpu, sizeof(ncpu), "%d", NCPU);
        snprintf(tslice, sizeof(tslice), "%d", TSLICE);

        execlp("./Scheduler", "Scheduler", ncpu, tslice, NULL);
        perror("execlp");
        exit(0);
    }
    else
    {
        while (1)
        {
            char input[100];
            printf("\033[0;32mSimpleShell$ \033[0m");
            fgets(input, sizeof(input), stdin);

            if (strlen(input) <= 1)
            {
                continue;
            }

            input[strcspn(input, "\n")] = '\0';

            if (strncmp(input, "submit", 6) == 0)
            {
                char *program = input + 7;

                int initial = 1; // default priority
                char *v = strchr(program, ' ');
                if (v != NULL)
                {
                    initial = atoi(v + 1);
                    *v = '\0';
                }
                executeProgram(j, program, initial);
            }
        }
    }
    return 0;
}