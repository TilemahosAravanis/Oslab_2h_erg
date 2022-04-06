#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"


void fork_procs( struct tree_node *pr, pid_t pd){
	change_pname(pr->name);
	uint k=pr->nr_children;
	if(k=0){
		sleep(10);
		exit(1);
	}
	else if(k>0){

		pd=fork();

		if (pd< 0) {
                /* fork failed */
                perror("fork");
                exit(1);
        }
                if (pd== 0) {         
					for(int i=0; i<k; i++){
						fork_procs(pr->children+i,pd);
						}
						exit(1);
						}
	}
	/*else perror();*/
}

int main(int argc, char *argv[])
{
	struct tree_node *root;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_tree_file>\n\n", argv[0]);
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
                fork_procs(root,pid);
                exit(1);
        }


	show_pstree(pid);

	return 0;
}
