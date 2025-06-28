#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#define CMD 1024
#define ARGS 64
struct timespec start, end;

/*==================================================Starting interface look=============================================================*/
void interface() {
    printf("\033[1;33m"); 
    printf("--------------------------------------------------\n");
    printf("|                                                 |\n");
    printf("|                                                 |\n");
    printf("|                                                 |\n");
    printf("|                                                 |\n");
    printf("\033[1;31m");
    printf("|       Welcome to Ayush's & Sanjeet's Shell      |\n");
    printf("\033[1;33m"); 
    printf("|                                                 |\n");
    printf("|                                                 |\n");
    printf("|                                                 |\n");
    printf("|                                                 |\n");
    printf("--------------------------------------------------\n");
    printf("\033[0m"); 
}

typedef struct Node
{
    char *name;//storing the command names
    char *args[ARGS];//Storing commands in array
    pid_t pid;//stores the pid of executable commands
    time_t start_time;//timestamp when command executed
    double duration;//storing duration in seconds
    struct Node *next;//pointer to next node
} Node;

Node *head = NULL;

/*=======================================================================================================================================*/

//Function to stop when CTRL +C is pressed!!!!
void sigintHandler(int a)
{
    Node *current =head;
    int count =1;
    while (current !=NULL)
    {
        printf("%d: ",count++);

        for(int i =0; i<ARGS && current->args[i] !=NULL;i++)
        {
            printf("%s ",current->args[i]);
        }
        printf("\n");

        //printing the required details given 
        printf("PID: %d\n",current->pid);
        printf("Start time: %s",ctime(&current->start_time));
        printf("Duration: %.2f seconds\n",current->duration);

        printf("\n");

        current =current->next;//moving the next command in the list
         
    }
}

// Declaring function!!!
void execommand(Node *cmd);//execute the command input
void freecommands(Node *head);//free the list of commands
void handlePipes(char *head);//function to all handles pipes command

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Main function~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int main(int argc, char *argv[])
{
    char input[CMD];
    char inputcopy[CMD];
    interface(); // Displaying the initial  interface 

    // Checking if a script file is provided as a command-line argument or not !!!
    if(argc ==2) {
        FILE *file =fopen(argv[1], "r");
        if (file ==NULL) {
            perror("Error in opening file");
            exit(1);
        }

    //read  and execute  commands from the script file
    while (fgets(input,sizeof(input),file)) {
        strcpy(inputcopy,input);
        input[strcspn(input,"\n")] ='\0';

      
        if (strchr(input,'|') !=NULL) {//check the command is it pipe or not 
            handlePipes(input);
        } 
        else
        {
            char *token =strtok(input," \t\n");
            int b =0;
            Node *new =(Node*)malloc(sizeof(Node));
            if (new ==NULL){
                perror("Memory allocation error");
                exit(1);
            }

            for (int i =0;i<ARGS;i++) {
                new->args[i]=NULL;
            }

            while (token !=NULL && b<ARGS) {
                new->args[b++] =strdup(token);
                token = strtok(NULL," \t\n");
            }

            new->name =strdup(new->args[0]);
            new->pid =0;
            new->start_time =0;
            new->duration =0.0;

            new->next =head;
            head =new;

            if (strchr(inputcopy,'|') !=NULL) {
                continue;
            }

            void printhistory(Node *current,int count,int total) {
                if (current ==NULL) {
                    return;
                }
                printhistory(current->next,count + 1,total);
                printf("%d: ",total - count +1);
                for (int i = 0; i<ARGS && current->args[i] != NULL;i++) {
                    printf("%s ",current->args[i]);
                }
                printf("\n");
            }

            if (strcmp(new->name,"history") ==0) {
                int total =0;
                Node *current =head;
                while (current != NULL) {
                    total++;
                    current=current->next;
                }
                printhistory(head, 1, total);
                continue;
            }

            int a=0;
            if (b>0 && strcmp(new->args[b-1],"&") == 0) {
                a=1;
                new->args[b -1] =NULL;
            }

            if (!a) {
                execommand(new);
            } else {
                pid_t child_pid =fork();
                if (child_pid< 0) {
                    perror("Fork error");
                    exit(1);
                } else if (child_pid ==0) {
                    execommand(new);
                    exit(0);
                }
            }
        }
    }

    fclose(file);//closing the file!!!
    }
    else{
    while (1)
    {
        //Displaying the customize prompt
        printf("\033[0;32m");//green text color
        printf("iiitd@possum~$");
        printf("\033[0m");//resting the color
        fgets(input,sizeof(input),stdin);//reading user input

        signal(SIGINT,sigintHandler);
        input[strcspn(input,"\n")] ='\0';//removing the trailing newlines
        strcpy(inputcopy,input);//creating input copy

        //checking for empty line
        if(input[0] =='\0')
        {
            continue;
        }

        //checking for the command whether there "|" present or not!!
        if(strchr(input,'|') !=NULL)
        {   
            Node *new = (Node *)malloc(sizeof(Node));
            new->args[0] = strdup(input);
            new->pid = 0;
            new->start_time = time(NULL);
            new->duration = 0.05;
            new->next = head;
            head = new;
           //handling the piped commands
            handlePipes(input);
        }

    
        else
        {   
            if(strchr(inputcopy,'|') !=NULL)//checking pipe character 
            {
                continue;
            }         
            char *token =strtok(input," \t\n");
            int b =0;

            Node *new =(Node*)malloc(sizeof(Node));//allocate memory for  a new command node 
            if(new ==NULL)
            {
                perror("Memory allocation error");
                exit(1);
            }

            for (int i =0;i<ARGS;i++)
            {
                new->args[i] =NULL;
            }
            
            while (token !=NULL && b<ARGS)//praising the commands and its argument 
            {
                new->args[b++] =strdup(token);
                token =strtok(NULL," \t\n");
            }

            //Storing  command names separately!!!!
            new->name =strdup(new->args[0]);
            new->pid =0;
            new->start_time =0;
            new->duration =0.0;

            new->next =head;
            head =new;
 
            //function to print  the history when history command called
            void printhistory(Node *current,int count,int total)
            {
                if (current ==NULL)
                {
                    return;
                }
                printhistory(current->next,count + 1,total);
                printf("%d: ",total - count + 1);
                for (int i = 0;i<ARGS && current->args[i] !=NULL;i++)//print the command and also its argument 
                {
                    printf("%s ",current->args[i]);
                }
                printf("\n");
            }

            if (strcmp(new->name,"history") ==0)//checking the command if is it history!!!!
            {
                head=new->next;
                int total = 0;
                Node *current =head;
                while(current !=NULL)//calculating the total commands in the list 
                {
                    total++;
                    current =current->next;
                }
                //print the command history
                printhistory(head,1,total);
                continue;
            }

            int a =0;
            //checking the last argument is & or not for background execution
            if(b >0 && strcmp(new->args[b-1],"&") ==0)
            {
                a =1;
                new->args[b - 1] =NULL;//remove the "&" from args
            }

            if(!a)
            {
                //execute the other commands on screen
                execommand(new);
            }
            else
            {
                // Execute the command in the back screen
                pid_t child_pid =fork();
                if (child_pid <0)
                {
                    perror("Fork error");
                    exit(1);
                }
                else if(child_pid ==0)
                {
                    // Child process for a backscreen execution
                    execommand(new);
                    exit(0);
                }
            }
        }
    }
    }
//free the memory  before exiting
    freecommands(head);

    return 0;
}

/*###############################################Function to execute the command#######################################################*/

void execommand(Node *cmd)
{
    pid_t pid;
    int status;

    pid =fork();
    if (pid<0)
    {
        perror("Fork Error");
        exit(1);
    }
    else if (pid == 0)
    {
        execvp(cmd->args[0],cmd->args);
        perror("Execution Error");
        exit(1);
    }
    else
    {
        //Updating the pid and start time
        cmd->pid = pid;
        cmd->start_time = time(NULL);

        //start time info
        clock_gettime(CLOCK_MONOTONIC, &start);

        waitpid(pid, &status, 0);

       //get end time and get the duration
        clock_gettime(CLOCK_MONOTONIC, &end);
        cmd->duration =(end.tv_sec - start.tv_sec) + (end.tv_nsec-start.tv_nsec)/1000000000.0;

        // Display given details
        if (WIFEXITED(status))
        {
            printf("Process %d terminated with status %d\n", pid, WEXITSTATUS(status));
        }
        else
        {
            printf("Process %d terminated abnormally\n", pid);
        }
    }
}

void freecommands(Node *head)
{
    while (head != NULL)
    {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
}
/*--------------------------------------------------Function to handle pipes commands--------------------------------------------------*/

//Function to split the commands!!!
int stepone(char **pipes,char *token)
{
    int num =0;
    int num_pipefd=0;
    for (int y =0 ;; y++)
    {
    if (token ==NULL)
        break;
    pipes[num++] =token;//Store the command in the array
    token =strtok(NULL, "|");
    num_pipefd=1;
    }
    pipes[num] =NULL;
    return num;
}

//function handle and redirection input and output of the commands!!!
void steptwo(int i, int pipe_fds[][2], int num)
{
    if (i>0)
    {
    close(pipe_fds[i-1][1]);
    dup2(pipe_fds[i-1][0],STDIN_FILENO);
    close(pipe_fds[i-1][0]);
    }

    if (i<num-1)
    {
    close(pipe_fds[i][0]);
    dup2(pipe_fds[i][1],STDOUT_FILENO);
    close(pipe_fds[i][1]);
    }
}

//Function to tokenize and execute the command!!!
void stepthree(int i, char **pipes)
{
    char *args[64];//array storing the arguments of the commands
    int index =0;
    int flag =1;
    int m =0;
    int n =0;
    char *argtoken =strtok(pipes[i]," \t\n"); 
    for (int u = 0;; u++)
    {
    if (argtoken ==NULL)
        break;
    args[index++] =argtoken;//storing the argument in the array!!
    argtoken = strtok(NULL," \t\n");
    }

    while(flag){
    args[index] =NULL; //mark the end of the array with NULL!!!
    execvp(args[0], args);
    flag =0;
    perror("Execution Error");
    }
    exit(1);

}

void handlePipes(char *command)
{
    pid_t pid;
    int status;

    pid =fork();//creatinge the child process
    if (pid <0)
    {
    perror("Fork error");//print the error message !!
    exit(1);
    }
    else if(pid == 0)
    {
        char *pipes[64];//array to store piped commands
        char *token =strtok(command, "|");
        int num =stepone(pipes, token);
        int pipe_fds[num-1][2];//stroing the file descriptor for pipes!

        int i =0;
        while (i<num)
        {
            if(i<num-1)
            {
                if(pipe(pipe_fds[i])<0)//create pipes for communication
                {
                perror("Pipe error");
                exit(1);
                }
            }
            pid_t child_pid =fork();
            if(child_pid<0)
            {
                perror("Fork Error");
                exit(1);
            }
            else if(child_pid ==0)
            {
                //redirect input and output for the commands segment
                steptwo(i,pipe_fds,num);
                //tokenize to execute the command
                stepthree(i,pipes);
            }
            else
            {
                if(i>0)
                {
                    //close the read end of the previous pipe 
                    close(pipe_fds[i-1][0]);
                    //close the write end of the previous pipe
                    close(pipe_fds[i-1][1]);
                }
                waitpid(child_pid, &status, 0);//waiting for the child process to finish!!
                
            }
            i++;
        }
    }
    else
    {
        //wait for all child process in the pipe to finish!
        waitpid(pid,&status,0);
    }
}
