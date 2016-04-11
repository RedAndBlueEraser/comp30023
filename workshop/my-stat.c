/*
 * my-stat.c
 * Version 20160411
 * Written by Harry Wong
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct stat buf;

    if (argc >= 2)
    {
        if (stat(argv[1], &buf) < 0)
        {
            perror("stat() error");
        }

        printf("          Inode number: %d\n", buf.st_ino);
        printf("             File size: %d bytes\n", buf.st_size);
        printf("      Blocks allocated: %d blocks\n", buf.st_blocks);
        printf("Reference (link) count: %d\n", buf.st_nlink);
        printf("               User ID: %d\n", buf.st_uid);
        printf("              Group ID: %d\n", buf.st_gid);
        printf("      Last access time: %d\n", buf.st_atime);
        printf("Last modification time: %d\n", buf.st_mtime);
        printf("    Last status change: %d\n", buf.st_ctime);

        // link count of a directory = 2 + number of sub-directories (just next level).
    }

    return 0;
}