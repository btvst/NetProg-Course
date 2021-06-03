#include <stdio.h>
#include <stdlib.h>

int main(){
  execl("/bin/ls", "ls", "-l", (char *)0);
  //execlp("ls", "ls", "-l", (char *)0); use execlp to just mention the filename
  printf("hello");

  return 0;
}