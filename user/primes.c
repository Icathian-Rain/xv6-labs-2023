#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int END = 35;

void primes(int *fd) {
    int data, p;
    // read
    close(fd[1]);
    if(read(fd[0], &p, sizeof(p)) != sizeof(p)) {
        fprintf(2, "failed read!\n");
        exit(1);
    }
    printf("prime %d\n", p);
    // read next
    if(read(fd[0], &data, sizeof(data))) {
        int fd1[2];
        pipe(fd1);
        switch (fork())
        {
        case 0:
            // child process
            primes(fd1);
            break;
        default:
            // read from parent and write to its child
            close(fd1[0]);
            do
            {
                if(data % p != 0) {
                    write(fd1[1], &data, sizeof(data));
                }
            } while (read(fd[0], &data, sizeof(data)));
            // when its parent send no data, close and return 
            close(fd[0]);
            close(fd1[1]);
            wait(0);
            break;
        }
    }
    exit(0);
}


int main(int argc, char *argv[])
{
    // main process

    // new pipe
    int fd[2];
    pipe(fd);
    switch (fork())
    {
    case 0:
        // child process
        primes(fd);
        break;
    default:
        // parent process
        // write
        close(fd[0]);
        for(int i = 2; i<=END; i++) {
            write(fd[1], &i, sizeof(i));
        }
        // close write
        close(fd[1]);
        wait(0);
        break;
    }

    exit(0);
}
