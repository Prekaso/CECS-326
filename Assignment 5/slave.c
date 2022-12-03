#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <semaphore.h>
#include "myShm.h"

int main(int argc, char** argv) {

	//Share memory file
	int shm_fileDes; 

	/*Takes user input from commandline argument such as
    number of children, shared memory name, semaphore name and stores it*/
	const char *segmentName = argv[0];	
	int childNum = atoi(argv[1]);
	const char *semaphoreName = argv[2];

	/*Declare required size of memory request*/
	const int SIZE = sizeof(struct CLASS);
	
	/*Create base address as pointer to struct CLASS*/
	struct CLASS  *shm_baseAdd; 

	/*Local variable that holds next free index*/
	int *i;

	/*The sem_open() function opens a named semaphore, returning a semaphore pointer that may be used on 
	subsequent calls to sem_post(), sem_wait(), and sem_close().
	This is the named semaphore used when outputting to display */
	sem_t *semDisplay = sem_open(semaphoreName, O_CREAT, 0660, 1);

	/*The sem_wait() function decrements by one the value of the semaphore.
	This function will reserve space for the below outputs*/
	sem_wait(semDisplay);

	/*Output identifying message*/
	printf("Slave %d begins execution\n",childNum);

	/*The sem_post() function posts to a semaphore, incrementing its value by one. 
	The function will close the reserved space for above outputs*/
	sem_post(semDisplay);
	
	/*This function will reserve space for the below outputs*/
	sem_wait(semDisplay);

	/*Output child number and segment name from execlp() system call */
	printf("I am child number %d, received shared memory name, %s, and semaphore name, %s\n",childNum,segmentName,semaphoreName);
	
	/*The function will close the reserved space for above outputs*/
	sem_post(semDisplay);

	/*The shm_open() function returns a file descriptor that is associated with the shared "memory object" specified by name.*/
	shm_fileDes = shm_open(segmentName, O_RDWR, 0666); 

	/*Checking for failure*/
	if (shm_fileDes == -1) {
		printf("ERROR from Slave %d. Shared Memory failed; shm_open() failed: %s\n",childNum, strerror(errno));
	 	exit(1);
	}

	/*The mmap() function establishes a mapping between an address space of a process and a file associated with the file descriptor
    at the offset for length of bytes.*/
	shm_baseAdd = mmap(0, SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shm_fileDes, 0); 

	/*Create pointer from myShm.h*/
	struct CLASS *count_ptr = (struct CLASS *) shm_baseAdd;

	/*The sem_init() function initializes the unnamed semaphore at the address pointed to by sem.
	Initialize the unnamed semaphore, called mutex_sem, in the shared memory*/ 
	sem_init(&(count_ptr->mutex_sem),0,1);

	/*This is the beginning of the critical section and stop all other child processes while one is in here*/
	sem_wait(&(count_ptr->mutex_sem));

	/*This named semaphore is included in here as to make sure only one child process can output in the critical section, contains more than ouputs*/
	sem_wait(semDisplay);

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
	
	/*This is the end of the critical section. The child process has finish doing what it to do with the shared memory and index
	The named semaphore releases the outputs */
    sem_post(semDisplay);

	/*Mutual exclusion is no longer required*/
	sem_post(&(count_ptr->mutex_sem));

	/*The munmap() function shall remove any mappings for those entire pages containing
	any part of the address space of the process starting at (shared memory base) address and continuing for (SIZE) length bytes*/
	if (munmap(shm_baseAdd, SIZE) == -1) {
 		printf("ERROR from Slave %d. Unmap failed; munmap() failed; %s\n",childNum, strerror(errno));
 		exit(1);
 	}

	/*The close() function is used to close a file, in this case it is used to close the shared memory segment as if it was a file*/
	if (close(shm_fileDes) == -1) { 
 		printf("ERROR from Slave %d. Close failed; close() failed: %s\n",childNum, strerror(errno));
 		exit(1);
	}
	
	/*This function will reserve space for the below outputs*/
	sem_wait(semDisplay);

	/*Output acknowledgement*/
	printf("Slave %d closes access to shared memory segment\n",childNum);
	printf("Slave %d exits\n",childNum);

	/*The function will close the reserved space for above outputs*/
	sem_post(semDisplay);
	
	/*The sem_destroy() function destroys the unnamed semaphore at the address pointed to by sem.*/
	sem_destroy(&(count_ptr->mutex_sem));

	/*The sem_close() function closes a named semaphore that was previously opened by a thread of the current process using sem_open(). 
	The sem_close() function frees system resources associated with the semaphore on behalf of the process.sem_close()*/
    if (sem_close(semDisplay) == -1) { 
    	printf("ERROR from Slave: sem_close failed: %s\n", strerror(errno));
       	exit(1);
  	}

	/*Exit program*/
	exit(1);
}
