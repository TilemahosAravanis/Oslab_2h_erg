#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"

void fork_procs(struct tree_node *ptr){

        printf("PID = %ld, name %s, starting...\n",
                        (long)getpid(), ptr->name);
        change_pname(ptr->name);
        uint k = ptr->nr_children;

        pid_t pid[k];
        int status;
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
                                fork_procs(ptr->children+i);
                        }
                }
        }

        // Waits for every children to stop
        wait_for_ready_children(k);

        // Stops and waits to be awakened by father
        raise(SIGSTOP);

        // These messages are Preordered (DFS)
        printf("PID = %ld, name = %s is awake\n",
                        (long)getpid(), ptr->name);

        for(i=0; i<k; i++){
                // Awakens child and waits for it's termination
                kill(pid[i], SIGCONT);
                // printf("%s: waiting for %s to terminate...\n", ptr->name, (ptr->children+i)->name);
                pid[i] = waitpid(pid[i], &status, 0);
                explain_wait_status(pid[i], status);
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
        int status;
        pid = fork();
        if (pid < 0) {
                perror("main: fork");
                exit(1);
        }
        if (pid == 0) {
                /* Child */
                fork_procs(root);
                exit(1);
        }

        wait_for_ready_children(1);

        show_pstree(pid);

        kill(pid, SIGCONT);

        /* Wait for the root of the process tree to terminate */
        wait(&status);
        explain_wait_status(pid, status);

        return 0;
}

