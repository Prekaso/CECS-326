#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <semaphore.h>
#include "myShm.h"

int main(int argc, char **argv) {

	/*Shared memory file descriptor*/
	int shm_fileDes;

	/*Takes user input from commandline argument such as
    number of children, shared memory name, semaphore name and stores it*/
	const char *segmentName = argv[0];	
	int childNum = atoi(argv[1]);
	const char *semaphoreName = argv[2];

	/*Declare required size of memory request*/
	const int SIZE = sizeof(struct CLASS);

	/*Local variable that holds next free index*/
	int *i;

	/*Create base address as pointer to struct CLASS*/
	struct CLASS *shm_baseAdd;

	/*Output identifying message*/
	printf("Slave %d begins execution\n",childNum);

	/*Output child number and segment name from execlp() system call */
	printf("I am child number %d, received shared memory name, %s, and semaphore name, %s\n",childNum,segmentName,semaphoreName);

	/*The shm_open() function returns a file descriptor that is associated with the shared "memory object" specified by name.*/
	shm_fileDes = shm_open(segmentName, O_RDWR, 0666); 

	/*Checking for failure*/
	if (shm_fileDes == -1) {
		printf("ERROR from Slave %d. Shared Memory failed; shm_open() failed: %s\n",childNum, strerror(errno));
	 	exit(1);
	}

	/*The mmap() function establishes a mapping between an address space of a process and a file associated with the file descriptor
    at the offset for length of bytes.*/
	shm_baseAdd = (struct CLASS *) mmap(0, SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shm_fileDes, 0); 
	
	/*Checking for failure*/
	if (shm_baseAdd == MAP_FAILED) { 
		printf("ERROR from Slave %d. Map failed; shm_mmap() failed: %s\n",childNum, strerror(errno));
		exit(1);
 	}

	/*The sem_open() function opens a named semaphore, returning a semaphore pointer that may be used on 
	subsequent calls to sem_post(), sem_wait(), and sem_close().

	The parameters of sem_open() are:
	1. name - a pointer to the null-terminated name of the semaphore to be opened
	2. oflag - option flags
	3. mode - permission flags
	4. value - initial value of the named semaphore */
    sem_t *mutex_sem = sem_open(semaphoreName, O_CREAT, 0666, 1); 

	/*Checking for failure*/
	if (mutex_sem == SEM_FAILED) {
       	printf("ERROR from Slave %d: sem_open failed(): %s\n",childNum, strerror(errno));
       	exit(1);
   	}

	/*The sem_wait() function decrements by one the value of the semaphore. 
	The semaphore will be decremented when its value is greater than zero. 
	If the value of the semaphore is zero, then the current thread will block until the semaphore's value becomes greater than zero.
	
	The parameter of sem_wait() is:
	1. sem - a pointer to an initialized unnamed semaphore or opened named semaphore */
    if (sem_wait(mutex_sem) == -1) { 
    	printf("ERROR from Slave %d sem_wait() failed: %s/n",childNum, strerror(errno));
     	exit(1);
	}

	/*Output acknowledgement message*/
	printf("Slave %d acquires access to shared memory segment, and structures it according to struct CLASS\n",childNum);

	/*Update index value*/
	i = &(shm_baseAdd->index);
	printf("Slave %d copies index to a local variable i\n",childNum);

	/*Write to shared memory*/
	shm_baseAdd->response[*i] = childNum;  
	printf("Slave %d writes its child number in response[%d]\n",childNum,*i);

	/*Increment shared memory base address index*/
	*i += 1;
	printf("Slave %d increments index\n",childNum);
	
	/*The sem_post() function posts to a semaphore, incrementing its value by one. 
	If the resulting value is greater than zero and if there is a thread waiting on the semaphore, 
	the waiting thread decrements the semaphore value by one and continues running.
	
	The parameter of sem_post() is:
	1. sem - a pointer to an initialized unnamed semaphore or opened named semaphore */
    if (sem_post(mutex_sem) == -1) {
       	printf("ERROR from Slave %d: sem_post() failed: %s\n",childNum, strerror(errno));
    	exit(1);
    }

	/*The sem_close() function closes a named semaphore that was previously opened by a thread of the current process using sem_open(). 
	The sem_close() function frees system resources associated with the semaphore on behalf of the process.

	The parameter for sem_close() is:
	1. sem - a pointer to an opened named semaphore. This semaphore is closed for this process.*/
 	if (sem_close(mutex_sem) == -1) {
	 	printf("ERROR from Slave %d: sem_close() failed: %s\n",childNum, strerror(errno));
     	exit(1);
    }

	/*The munmap() function shall remove any mappings for those entire pages containing
	any part of the address space of the process starting at (shared memory base) address and continuing for (SIZE) length bytes*/
	if (munmap(shm_baseAdd, SIZE) == -1) {
 		printf("ERROR from Slave %d. Unmap failed; munmap() failed; %s\n",childNum, strerror(errno));
 		exit(1);
 	}

	/*The close() function is used to close a file, in this case it is used to close the shared memory segment as if it was a file*/
	if (close(shm_fileDes) == -1) { 
 		printf("ERROR from Slave %d. Close failed; close() failed: %s\n",childNum,strerror(errno));
 		exit(1);
	}

	printf("Slave %d closes access to shared memory and semaphore and terminates\n",childNum);
	printf("Slave %d exits\n",childNum);

	/*Exit program*/
	exit(1);
}
