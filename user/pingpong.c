#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int BUFF_SIZE = 1;

int main(int argc, char *argv[])
{
    // pipe: 
    // pf1 is parent 2 child
    // pf2 is child 2 parent
    int pf1[2];
    int pf2[2];

    // create pipes
    pipe(pf1);
    pipe(pf2);

    // fork a child process
    switch (fork())
    {
    case -1:
        // error
        fprintf(2, "failed to create a process\n");
        break;
    case 0:
        // child process
        // read from pf1
        close(pf1[1]);
        char read_buff[BUFF_SIZE];
        if(read(pf1[0], read_buff, BUFF_SIZE) != 1) {
            fprintf(2, "child: failed read from parent\n");
            exit(1);
        }
        close(pf1[0]);
        // write to pf2
        close(pf2[0]);
        // print ping
        int child_pid = getpid();
        printf("%d: received ping\n", child_pid);
        // write
        if(write(pf2[1], read_buff, BUFF_SIZE) != 1) {
            fprintf(2, "child: failed write to parent\n");
            exit(1);
        }
        close(pf2[1]);
        // exit
        exit(0);
    }
    // father process
    // write to pf1
    close(pf1[0]);
    char write_buff[] = {1};
    if(write(pf1[1], write_buff, BUFF_SIZE) != 1) {
        fprintf(2, "parent: failed write to child\n");
        exit(1);
    }
    close(pf1[1]);
    // read from pf2
    close(pf2[1]);
    if(read(pf2[0], write_buff, BUFF_SIZE) != 1) {
        fprintf(2, "parent: failed read from child\n");
        exit(1);
    }
    close(pf2[0]);
    // print pong
    int parent_pid = getpid();
    printf("%d: received pong\n", parent_pid);
    // exit
    exit(0);
}
