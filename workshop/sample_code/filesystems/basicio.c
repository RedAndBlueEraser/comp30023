#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>


int
main(int argc, char **argv)
{
    int fd = open("./file.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    assert(fd >= 0);
    
    char buffer[20];
    sprintf(buffer, "I love C\n");
    
    int rc = write(fd, buffer, strlen(buffer));
    assert(rc == (strlen(buffer)));
    printf("wrote %d bytes\n", rc);

    close(fd);
    
    return 0;
}
