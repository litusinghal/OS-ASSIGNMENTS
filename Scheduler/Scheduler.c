#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_QUEUE_SIZE 100

int NCPU;
int TSLICE;

//Function to find minimum of two number 
int min(int a,int b){
    return a<b ? a:b;
}

//Specialised Queue 
typedef struct{
    pid_t arr[100];
    int front;
    int rear;
    int count;
    int priority[100];
    sem_t mutex;
} Queue;

//Function to Delete the element 
pid_t dequeue(Queue* q) {
    pid_t to_return = q->arr[q->front + 1];
    q->front++;
    q->count--;
    return to_return;
}

//Function to add the element 
void enqueue(Queue* q, pid_t to_enqueue) {
    q->rear++;
    q->arr[q->rear] = to_enqueue;
    q->count++;
}

//Complete Scheduling Code 
void roundrobin(Queue* q,int ncpu,int tslice){
    while(1){
    pid_t to_run[ncpu];
        if(q->count>0){
        sem_wait(&(q->mutex)); 
        int p = min(ncpu,q->count);
        for(int i =0;i<p;i++){
            printf(" ");
            to_run[i]=dequeue(q);
            kill(to_run[i], SIGCONT);
        }

        sem_post(&(q->mutex));

        sleep(TSLICE);  

        sem_wait(&(q->mutex));
        for(int i =0;i<p;i++){
            pid_t s = to_run[i];
            kill(s, SIGSTOP);
            enqueue(q,s);
        }
        sem_post(&(q->mutex));
    }
    }  
}

int main(int argc, char *argv[]) {
    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);

    //Shared memory 
    int fd = shm_open("my_sm", O_RDWR, 0666);
    if (fd == -1) {
        perror("Error in shm_open");
    }
    Queue* j = mmap(0, sizeof(Queue), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    roundrobin(j, NCPU, TSLICE);
    return 0;
}
