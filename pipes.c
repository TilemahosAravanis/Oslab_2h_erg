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

        pid_t pid[k];
        int status;

        int pfd2[k];
        printf("%s: Creating pipe...\n",ptr->name);        
	    if (pipe(pfd2) < 0) {
		    perror("pipe");
		    exit(1);
        } 

        if(k==0){
            raise(SIGSTOP);

            char name=atoi(ptr->name)+'0';

            printf("%i: write value to pipe\n",name);
	        if (write(fd, &name, sizeof(name)) != sizeof(name)) {
		        perror("parent: write to pipe");
	    	    exit(1);
	        }
            printf("%i: Wrote to pipe and exiting...\n",name);
            exit(1);
        } 

        int i;       
        if(k>0){
                for(i=0; i<k; i++){
                        pid[i] = fork();
                        if (pid[i] <  0) {
                                /* fork failed */
                                perror("fork");
                                exit(1);
                        }
                        if (pid[i] == 0) {
                            close (pfd2[0]);
                            fork_procs(ptr->children+i,pfd2[1]);
                        }
                }
        }

        close(pfd2[1]);

        // Waits for every children to stop
        wait_for_ready_children(k);

        // Stops and waits to be awakened by father
        raise(SIGSTOP);

        // These messages are Preordered (DFS)
        printf("PID = %ld, name = %s is awake\n",
                        (long)getpid(), ptr->name);


        char val[2];            

        for(i=0; i<k; i++){
                // Awakens child and waits for it's termination
                kill(pid[i], SIGCONT);

                if (read(pfd2[0], &val[i], sizeof(val[i])) != sizeof(val[i])) {
	                	perror("child: read from pipe");
	                	exit(1);
	            }
                printf("Parent: received value %i from the pipe.\n", val[i]);

                                // printf("%s: waiting for %s to terminate...\n", ptr->name, (ptr->children+i)->name);
                pid[i] = waitpid(pid[i], &status, 0);
                explain_wait_status(pid[i], status);
        }

        int a=val[0]-'0'; //cast to int
        int b=val[1]-'0';

        char value;

        switch(*ptr->name){
            case '*':value=a*b+'0'; break;
            case '+':value=a+b+'0'; break;
        }

        if (write(fd, &value, sizeof(value)) != sizeof(value)) {
		    perror("parent: write to pipe");
		    exit(1);
	    }

        // These messages are Postordered (DFS)
        printf("%s: Exiting...\n",ptr->name);
        exit(0);
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

        wait_for_ready_children(1);

        show_pstree(pid);


        /* for ask2-signals */

        kill(pid, SIGCONT);

        char val;
        if (read(pfd[0], &val, sizeof(val)) != sizeof(val)) {
		    perror("root: read from pipe");
		    exit(1);
	    }
    	printf("Root: received value %i from the pipe. \n", val);

        /* Wait for the root of the process tree to terminate */

        wait(&status);
        explain_wait_status(pid, status);
        close(pfd[0]);
        close(pfd[1]);
        printf("Apotelesma=%i\n", val-'0'); 

        return 0;
}