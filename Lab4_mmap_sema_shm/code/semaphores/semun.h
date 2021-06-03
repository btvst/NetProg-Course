#ifndef SEMUN_H
#define SEMUN_H

#include <sys/types.h>
#include <sys/sem.h>

union semun{
    int val;
    struct semid_ds &buf;
    unsigned short *array;
#ifdef __linux__ 
    struct semingo * __buf;
#endif
};

#endif