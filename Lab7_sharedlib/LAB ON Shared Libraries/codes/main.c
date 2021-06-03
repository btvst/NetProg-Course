#include <stdio.h>
#include "checkPrime.h"

int main(){
	int n = 0;
	printf("Enter Number\n");
	scanf("%d",&n);
	if(checkPrime(n) == 0){
		printf("%d is not Prime\n",n);
	}else{
		printf("%d is Prime\n",n);
	}
	return 0;
}
