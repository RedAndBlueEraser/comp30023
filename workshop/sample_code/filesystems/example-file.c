#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <dirent.h> 
#include <errno.h> 
#include <utime.h>
#include <sys/stat.h>
#include <time.h> 
#include <unistd.h>
#include <fcntl.h> 


int main(int argc, char **argv) 
{
  int fd;

  /* create a directory */
  if(mkdir("foo", S_IRWXU)) {
    perror("mkdir() failure");
    exit(1);
  }

  /* change the mode of the new directory */
  
  if(chmod("foo", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
    perror("chmod() failure");
    exit(2);
  } 

  /* change into new directory */
  if(chdir("foo")) {
    perror("chdir() failure");
    exit(2);
  }

  /* create a new file named 'bar' */
  if((fd=open("bar", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU 
  	  				| S_IRWXG | S_IRWXO)) < 0) {
    perror("open() failure");
    exit(2);
  } 
  
  /* writ to file */
  write(fd, "I love C\n\n", 10);
  if(close(fd)) {
    perror(NULL);
    exit(2);
  } 

  /* create a symlink to 'bar' named 'symbar' */
  if(symlink("bar", "symbar")) {
    perror("symlink() failure");
    exit(2);
  }
  
  /* create a hard link to 'bar' named 'hardbar' */
  if(link("bar", "hardbar")) {
    perror("link() failure");
    exit(2);
  } 
  
  /* Wait ... explore directory */
  printf("Explore directory ... then [enter] to continue...\n");
  getchar();

  /* "delete" a file */
  if(unlink("hardbar")) {
    perror("unlink() failure");
    exit(2);
  } 

  /* delete the symlink */
  if (unlink("symbar"))
  {
	perror("unlink() failure");
    exit(2);
  }

  /* remove the created bar file */
  if (remove("bar") < 0)
  {
	perror("remove() failure");
    exit(2);
  }

  /* move up a directory */
  if (chdir(".."))
  {
    perror("chdir() failure");
    exit(2);
  }

  /* create a symlink to 'foo' named 'symfoo' */
  if (symlink("foo", "symfoo"))
  {
    perror("symlink() failure");
    exit(2);
  }

  /* Wait ... explore working directory */
  printf("Explore working directory ... then [enter] to continue...\n");
  getchar();

  /* unlink "symfoo" */
  if (unlink("symfoo"))
  {
	perror("unlink() failure");
	exit(2);
  }

  /* remove "foo" directory */
  if (rmdir("foo") < 0)
  {
	perror("rmdir() failure");
	exit(2);
  }

  return 0;
}
