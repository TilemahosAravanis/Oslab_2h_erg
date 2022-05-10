#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"

void fork_procs(struct tree_node *ptr,int fd){

        printf("PID = %ld, name %s, starting...\n", (long)getpid(), ptr->name);
        change_pname(ptr->name);
        uint k = ptr->nr_children;

        if(k==0){
                // Has no children so just writes it's name-value to the pipe and exits
                char Num = atoi(ptr->name) + '0'; //Collects int value from string and returns it's ASCII represantation

                printf("%s: write value to pipe\n",ptr->name);
                if (write(fd, &Num, sizeof(Num)) != sizeof(Num)) {
                        perror("child: write to pipe");
                        exit(1);
                }
                printf("%s: Wrote to pipe and exiting...\n",ptr->name);
                exit(7);
        }

        // Has two children to reach that spot
        int pfd2[2];
        printf("%s: Creating pipe...\n",ptr->name);
        if (pipe(pfd2) < 0) {
                perror("pipe");
                exit(1);
        }

        pid_t pid;
        int status;

        int i;
        if(k>0){
                for(i=0; i<2; i++){
                        // Has 2 children forks them and passes them the writing end
                        pid = fork();
                        if (pid <  0) {
                                /* fork failed */
                                perror("fork");
                                                                exit(1);
                        }
                        if (pid == 0) {
                                fork_procs(ptr->children+i,pfd2[1]);
                        }
                }
        }

        char val[2];
        for(i=0; i<2; i++){
                // WAITS to read child's value from pipe then waits for it's termination
                if (read(pfd2[0], &val[i], sizeof(val[i])) != sizeof(val[i])) {
                        perror("child: read from pipe");
                        exit(1);
                }
                printf("Parent: received value %i from the pipe.\n", val[i] - '0' );
 
                pid = waitpid(-1,&status,0);
                explain_wait_status(pid,status);
        }
        close (pfd2[0]);
        close (pfd2[1]);

        //cast from ASCII code back to int
        int a=val[0] - '0';
        int b=val[1] - '0';

        //calcutates it's value and saves it's ASCII representation
        char value;
        switch(*ptr->name){
                case '*':value=a*b + '0'; break;
                case '+':value=a+b + '0'; break;
        }

        if (write(fd, &value, sizeof(value)) != sizeof(value)) {
                perror("parent: write to pipe");
                exit(1);
        }

        printf("%s: Wrote to pipe and exiting...\n",ptr->name);
        exit(7);
}


int main(int argc, char *argv[]){

        struct tree_node *root;

        if (argc < 2){

                fprintf(stderr, "Usage: %s <tree_file>\n", argv[0]);
                exit(1);
        }

        root = get_tree_from_file(argv[1]);
        print_tree(root);

        pid_t pid;
        int pfd[2];
        int status;

        printf("Root: Creating pipe...\n");

        if (pipe(pfd) < 0) {
                perror("pipe");
                exit(1);
        }

        fprintf(stderr, "%s: Is being created...\n", root->name);
        pid = fork();

        if (pid < 0) {
                perror("main: fork");
                exit(1);
        }

        if (pid == 0) {
                /* Child */
                fork_procs(root,pfd[1]);
                exit(1);
        }

        //show_pstree(pid);

        char val;
        if (read(pfd[0], &val, sizeof(val)) != sizeof(val)) {
                    perror("root: read from pipe");
                    exit(1);
        }

        printf("Root: received value %i from the pipe. \n", val - '0');
       
        /* Wait for the root of the process tree to terminate */
        wait(&status);
        explain_wait_status(pid, status);
        close(pfd[0]);
        close(pfd[1]);
        printf("Result %i\n", val - '0');

        return 0;
}