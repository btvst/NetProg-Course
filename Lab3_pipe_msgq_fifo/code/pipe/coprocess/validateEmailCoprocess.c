#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXSIZE 100

int main ()
{
  char buf[MAXSIZE];
  int n;
  sleep(10);

  while ((n = read (0, buf, MAXSIZE)) > 0){
    buf[n] = '\0';
    if (strstr(buf, "@") > 0)
      if (strstr(buf, ".") > 0)
        write (1, "1\n", 2);
      else
        write (1, "-2\n", 3);
    else
      write (1, "-3\n", 3);
  }

  return 0;
}