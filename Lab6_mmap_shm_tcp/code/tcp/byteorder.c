#include <stdio.h>
#include <stdlib.h>

u_int32_t swapEndianess(u_int32_t num){
	u_int32_t b0, b1, b2, b3;
	u_int32_t res;

	b0 = (num & 0x000000ff) << 24;
	b1 = (num & 0x0000ff00) << 8;
	b2 = (num & 0x00ff0000) >> 8;
	b3 = (num & 0xff000000) >> 24;

	res = b0 | b1 | b2 | b3;
	return res;
}

int
main(int argc, char **argv)
{
	union {
	  short  s;
      char   c[sizeof(short)];
    } un;

	un.s = 0x0102;
	printf("local system: ");
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2)
			printf("big-endian\n");
		else if (un.c[0] == 2 && un.c[1] == 1)
			printf("little-endian\n");
		else
			printf("unknown\n");
	} else
		printf("sizeof(short) = %d\n", sizeof(short));

	exit(0);
}
