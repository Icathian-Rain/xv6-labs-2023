#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), 0, DIRSIZ - strlen(p));
    return buf;
}


void find(char *path, char *fileName)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // open directory
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        // file type is DEVICE or FILE
        // compare if it's name is fileName
        // fmtname(path): the filename
        // path: the full name
        if(strcmp(fileName, fmtname(path)) == 0) {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        // file type is DIR
        // find again
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        // buf: the full path
        // p: the directory name
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
                continue;
            find(buf, fileName);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    // need 2 para
    if (argc < 3)
    {
        fprintf(2, "Usage: find path filename\n");
        // exit
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}