#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>

/* When longjmp moves control out of the calling function, the values of the variables in the calling function do not change */

/* setjmp and longjmp are used to handle exceptions in C */

void func(void);
jmp_buf place;
jmp_buf place1;

double square1(double n2){
        if(n2==0){
                longjmp(place, 2);
        }
        if(n2==1){
                longjmp(place1, 3);
        }
        
        printf("Inside square1: %lf\n",n2);
        return n2*n2;
}

double compute(int n){
        printf("Compute called---------------------------------\n");
        printf("n: %d\n",n);
        printf("-----------------------------------------------\n");
        if(n==0){ 
                longjmp(place, 1);
        }
        if(n==1){
                longjmp(place1, 2);
        }

        double n1=sqrt((double)n);
        double n2=square1(n1);

        printf("Inside compute: n1: %lf -- n2: %lf\n", n1, n2); //Printing double values with %lf format specifier will give absurd and wrong values.
        
        return(n2-n);
}


int main(){
        int retval, retval1;
        int i=1, n;
        /*
         * First call returns 0,
         * a later longjmp will return non-zero.
        */

        /* The below declaration isnot recommended */
        // retval = setjmp(place); //longjmp returns the place where setjmp is placed.
        // printf("AAAA\n");

        if((retval = setjmp(place)) != 0){
                printf("retval: Returned using longjmp\n");
                printf("Ret value: %d\n", retval);
        }

        if((retval1 = setjmp(place1)) != 0){ //1 (b)
                printf("retval1: Returned using longjmp\n");
                printf("Ret value: %d\n", retval);
        }

        while(i<100){
                printf("Enter value %d: ", i);
                scanf("%d", &n);
                double n3=compute(n);
                printf("Result=%lf\n", n3);
                i++;
        }

        return 0;
}