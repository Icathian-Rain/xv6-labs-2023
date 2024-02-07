#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

void xargs(int argc, char *argv[])
{
    char *params[MAXARG];
    // copy original params
    for (int i = 1; i < argc; i++)
    {
        params[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(params[i - 1], argv[i]);
    }
    // begin params position
    int begin = argc - 1;
    int i = 0;
    char buff[1024];
    while (1)
    {
        // exec everline
        // end: now the position
        int end = begin;
        i = 0;
        while (read(0, buff + i, 1))
        {
            // break the next line
            if (buff[i] == '\n')
            {
                break;
            }
            // jump the ' ' char
            else if (buff[i] == ' ' && i != 0)
            {
                buff[i++] = 0;
                params[end] = malloc(i);
                strcpy(params[end], buff);
                i = 0;
                end++;
                if (end >= MAXARG)
                {
                    fprintf(2, "too much parameters\n");
                    exit(1);
                }
            }
            else if (buff[i] != ' ')
            {
                i++;
            }
        }
        // get the last param
        if (i != 0)
        {
            buff[i++] = 0;
            params[end] = malloc(i);
            strcpy(params[end], buff);
            end++;
            i = 0;
        }
        // null
        if (end == begin)
        {
            break;
        }
        params[end] = 0;
        if (fork() == 0)
        {
            // test
            // for(int i = 0; i<end; i++)
            // {
            //     printf("%d: %s,", i, params[i]);
            // }
            // printf("\n");
            exec(params[0], params);
            exit(1);
        }
        else
        {
            wait(0);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: xargs command [para ...]\n");
        exit(1);
    }
    xargs(argc, argv);
    
    exit(0);
}