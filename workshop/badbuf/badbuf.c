#include <stdio.h>
#include <string.h>

void welcome(char *str) {
 printf(str); 
}
void goodbye(char *str) {
 void exit();
 printf(str);
 exit(1); 
}
main(){
  char name[128], pw[128];
  char *good = "Welcome to The Machine!\n";
  char *evil = "Invalid identity, exiting!\n";
  printf("login: ");
  scanf("%s", name);
  printf("password: "); 
  scanf("%s", pw);
  if( strcmp(name,pw) == 0 )
    welcome( good );
  else
    goodbye(evil );
}