#define MSGQ_PATH "msgq_client.c"    //put the appropriate path 

#define MAXSIZE 50

void initialize(char *ptr){
    for(int i=0; i<MAXSIZE; ++i){
        ptr[i] = '\0';
    }
}