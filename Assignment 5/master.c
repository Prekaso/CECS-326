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

	/*Shared memory file descriptor*/
	int shm_fileDes; 

	/*Takes user input from commandline argument such as
	number of children, shared memory name, semaphore name and stores it*/
	int numChildren = atoi(argv[1]); 
	const char *segmentName = argv[2];
	const char *semaphoreName = argv[3];

	/*Declare required size of memory request*/
	const int SIZE = sizeof(struct CLASS);

	/*Create base address as pointer to struct CLASS*/
	struct CLASS  *shm_baseAdd; 

	/*The sem_open() function opens a named semaphore, returning a semaphore pointer that may be used on 
	subsequent calls to sem_post(), sem_wait(), and sem_close().
	This is the named semaphore used when outputting to display */
	sem_t *semDisplay = sem_open(semaphoreName, O_CREAT, 0660, 1);

	/*The sem_wait() function decrements by one the value of the semaphore.
	This function will reserve space for the below outputs*/
	sem_wait(semDisplay);

	/*When executed, master outputs a message to identify itself*/
	printf("Master begins execution\n");

	/*Output shared memory segment name*/
	printf("Master created a shared memory segment named %s\n", segmentName);

	/*Output semaphore name*/
	printf("Master created a semaphore named %s\n", semaphoreName);

	/*Output starting index of response*/
	printf("Master initializes index in the shared struct to 0\n");
	
	/*The sem_post() function posts to a semaphore, incrementing its value by one. 
	The function will close the reserved space for above outputs*/
	sem_post(semDisplay);

	/*The shm_open() function returns a file descriptor that is associated with the shared "memory object" specified by name.*/
	shm_fileDes = shm_open(segmentName,O_CREAT | O_RDWR,0666);
	
	/*The ftruncate() function truncates the file indicated by the open file descriptor to the indicated length. */
	ftruncate(shm_fileDes, SIZE);
	
	/*The mmap() function establishes a mapping between an address space of a process and a file associated with the file descriptor
    at the offset for length of bytes.*/
	shm_baseAdd = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fileDes,0); 
	
	/*Update/Track index*/
	shm_baseAdd->index = 0;

	/*Create pointer from myShm.h*/
	struct CLASS *count_ptr = (struct CLASS *) shm_baseAdd;

	/*The sem_init() function initializes the unnamed semaphore at the address pointed to by sem.
	Initialize the unnamed semaphore, called mutex_sem, in the shared memory*/ 
	sem_init(&(count_ptr->mutex_sem),0,1);

	/*Determine number of child process to create*/
	for (int i = 0; i < numChildren; i++) { 

		/*fork() system call is used to create child process*/
		pid_t childPID = fork();

		/*buffer for second argument in execlp()*/
		char childNum[5]; 

		/*Convert integer to char in order to be passed as an argument*/
		sprintf(childNum,"%d",i + 1);

		/*execute child process via execlp() function call*/
		if (childPID == 0){ 
			execlp("./slave",segmentName,childNum,semaphoreName, NULL);
		}
	}

	/*This function will reserve space for the below outputs*/
	sem_wait(semDisplay);

	/*Output number of child process created by master to be executed by slave*/
    printf("Master created %d child processes to execute slave\n", numChildren);
	
	/*Waiting for all child processes to exit*/
    printf("Master waits for all child processes to terminate\n");
	
	/*This function will close the reserve space for the above outputs*/
	sem_post(semDisplay);

	/*Waiting for processes*/
	while(wait(NULL) != -1);

	/*This function reserves space for outputs below*/
	sem_wait(semDisplay);

	/*Output the content of the shared memory segment*/
	printf("Master received termination signals from all %d child processes\n",numChildren);
	printf("Content of shared memory segment filled by child processes:\n");
	printf("--- content of shared memory ---\n");
	
	for (int i = 0; i < numChildren; i++) {
		/*For every child process, output the content of the shared memory filled in by the slave*/
		printf("%d\n", shm_baseAdd -> response[i]);
	}	

	/*This function closes the reserved space for above outputs*/
	sem_post(semDisplay);

    /*The munmap() function shall remove any mappings for those entire pages containing
	any part of the address space of the process starting at (shared memory base) address and continuing for (SIZE) length bytes*/    
    if (munmap(shm_baseAdd, SIZE) == -1) {
 		printf("ERROR from Master: Unmap failed; munmap() failed; %s\n", strerror(errno));
 		exit(1);
 	}

	/*The close() function is used to close a file, in this case it is used to close the shared memory segment as if it was a file*/
	if (close(shm_fileDes) == -1) { 
 		printf("ERROR from Master: Close failed; close() failed: %s\n", strerror(errno));
 		exit(1);
	}

	/*The shm_unlink function removes a memory object name, and, once all processes have unmapped the object,
 	de-allocates and destroys the contents of the associated memory region. */
	shm_unlink(semaphoreName);

	/*This function reserves space for outputs below*/
	sem_wait(semDisplay);

	/*Output shared memory segment removal confirmation message*/
	printf("Master removed shared memory segment and exits\n");
	
	/*This function will close the reserve space for the above outputs*/
	sem_post(semDisplay);

	/*The sem_destroy() function destroys the unnamed semaphore at the address pointed to by sem.*/
	sem_destroy(&(count_ptr->mutex_sem));

	/*The sem_close() function closes a named semaphore that was previously opened by a thread of the current process using sem_open(). 
	The sem_close() function frees system resources associated with the semaphore on behalf of the process.sem_close()*/
    if (sem_close(semDisplay) == -1) { 
    	printf("ERROR from Master: sem_close failed: %s\n", strerror(errno));
       	exit(1);
  	}

	/*The sem_unlink() function unlinks a named semaphore. 
	The name of the semaphore is removed from the set of names used by named semaphores.*/
	if (sem_unlink(semaphoreName) == -1) { 
        	printf("ERROR from Master: sem_unlink() failed: %s\n", strerror(errno));
        	exit(1);
	}

	/*Exit program*/
	exit(0);
}
