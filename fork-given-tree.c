#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

/*
 * Create this process tree:
 * A-+-B---D
 *   `-C
 */
void fork_procs(void)
{
        /*
         * initial process is A.
         */

        change_pname("A");

        pid_t pB, pC; // A has to wait for 2 childs with different PID
        int status;

        fprintf(stderr, "A: Creating B...\n");
        pB = fork();
        if (pB < 0) {
                /* fork failed */
                perror("fork");
                exit(1);
        }
                if (pB == 0) {
                /* B in process */
                change_pname("B");

                fprintf(stderr, "B: Creating D...\n");
                pB = fork();
                if (pB < 0) {
                        /* fork failed */
                        perror("fork");
                        exit(1);
                }
                if (pB == 0) {
                        /* D in process */
                        change_pname("D");
                        printf("D: Sleeping...\n");
                        sleep(SLEEP_PROC_SEC + 1);
                        printf("D: Exiting...\n");
                        exit(13);
                }

                printf("B: waiting for D to terminate...\n");
                pB = waitpid(pB, &status, 0);
                explain_wait_status(pB, status);

                printf("B: Exiting...\n");

                exit(19);
        }

        fprintf(stderr, "A: Creating C...\n");
        pC = fork();
        if (pC < 0) {
                /* fork failed */
                perror("fork");
                exit(1);
        }
        if (pC == 0) {
                /* In child process */
                 change_pname("C");
                 printf("C: Sleeping...\n");
                 sleep(SLEEP_PROC_SEC);
                 printf("C: Exiting...\n");
                 exit(17);
        }
                printf("A: waiting for C to terminate...\n");
        pC = waitpid(pC, &status, 0);
        explain_wait_status(pC, status);

        printf("A: waiting for B to terminate...\n");
        pB = waitpid(pB, &status, 0);
        explain_wait_status(pB, status);


        printf("A: Exiting...\n");
        exit(16);
}

/*
 * The initial process forks the root of the process tree,
 * waits for the process tree to be completely created,
 * then takes a photo of it using show_pstree().
 *
 * How to wait for the process tree to be ready?
 * In ask2-{fork, tree}:
 *      wait for a few seconds, hope for the best.
 */

int main(void)
{
        pid_t pid;
        int status;

       /* Fork root of process tree */

        fprintf(stderr, "A: Is being created...\n");
        pid = fork();
        if (pid < 0) {
                perror("main: fork");
                exit(1);
        }
        if (pid == 0) {
                /* Child */
                fork_procs();
                exit(1);
        }
        /*
         * Father
         */

        /* for ask2-{fork, tree} */
        sleep(SLEEP_TREE_SEC);

        /* Print the process tree-root at pid */
        show_pstree(pid);
        //show_pstree(getpid());
        
        /* Wait for the root of the process tree to terminate */
        pid = wait(&status);
        explain_wait_status(pid, status);

        return 0;
}
