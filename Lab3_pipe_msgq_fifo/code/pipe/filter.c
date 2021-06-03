#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* getchar reads from buffered input as its stdin, not the keyboard, so we have to press ENTER before getchar gets anything to read. If it has nothing to read in the buffer, it returns EOF */

void err_sys(char *str){
  perror (str);
  exit (-1);
}


int main(void){
  int c;
  while((c = getchar ()) != EOF){
    if (islower (c))
      c = toupper (c);
    if (putchar (c) == EOF)
      err_sys ("output error");
    if (c == '\n')
      fflush (stdout);
  }

  exit(0);
}
