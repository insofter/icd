#include <stdio.h>
#include <unistd.h>

int main() {
  int i;

  scanf("%i",&i);
  fprintf(stderr, "read 1 sss %i sss", i);
  sleep(10);
  scanf("%i",&i);
  fprintf(stderr, "read 2 sss %i sss", i);


  return 0;
}
