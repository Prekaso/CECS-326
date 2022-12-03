/* myShm.h*/
/* Header file to be used with master.c and slave.c
*/
#include <semaphore.h>

struct CLASS {

    sem_t mutex_sem;

    /*Index to next available response slot*/
    int index;

    /*Each child writes its child number here*/
    int response[10];
    
};
