#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

void fork_procs(struct tree_node *ptr){
        change_pname(ptr->name);

        uint k = ptr->nr_children;

        if(k==0){
                printf("%s: Sleeping...\n",ptr->name);
                sleep(SLEEP_PROC_SEC);
                printf("%s: Exiting...\n",ptr->name);
                exit(1);
        }

        else if(k>0){
                pid_t pid;
                int status;

                int i;
                for(i=0; i<k; i++){

                        fprintf(stderr, "%s: Is being created...\n",(ptr->children+i)->name);
                        pid = fork();
                        if (pid <  0) {
                                /* fork failed */
                                perror("fork");
                                exit(1);
                        }
                        if (pid== 0) {
                                fork_procs(ptr->children+i);
                        }
                }

                for(i=0; i<k; i++){
                        pid = waitpid(-1,&status,0); // I do not print waiting messages for certain children-let the "fastest" terminate first
                        explain_wait_status(pid, status);
                }

		printf("%s: Exiting...\n",ptr->name);
                exit(1);
        }
}

int main(int argc, char *argv[]){

        struct tree_node *root;

        if (argc != 2) {
                fprintf(stderr, "Usage: %s <input_tree_file>\n\n", argv[0]);
                exit(1);
        }

        root = get_tree_from_file(argv[1]);
        print_tree(root);

        pid_t pid;
        int status;
        fprintf(stderr, "%s: Is being created...\n", root->name);
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

        /* for ask2-{fork, tree} */
        sleep(SLEEP_TREE_SEC);

        /* Print the process tree-root at pid */
        show_pstree(pid);

        /* Wait for the root of the process tree to terminate */
        pid = wait(&status);
        explain_wait_status(pid, status);

        return 0;
}