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
#include "myShm.h"

int main(int argc, char* argv[]) {

    /*Before starting, make sure argument count is valid*/
    if (argc >= 3) {
        /*When executed, master outputs a message to identify itself*/
        printf("Master begins execution\n");

        /*Takes user input from commandline argument such as
        number of children and shared memory name and stores it*/
        char* numChildren = argv[1];
        char* segmentName = argv[2];

        /*Output shared memory segment name*/
        printf("Master created a shared memory segment named %s\n", segmentName);

        /*Declare required size of memory request*/
        const int SIZE = 4096;

        /*Create base address as pointer to struct CLASS*/
        struct CLASS *shm_baseAdd;

        /*Create shared memory file descriptor to reference values from response array*/
        int shm_fileDes;

        /*The shm_open() function returns a file descriptor that is associated with the shared "memory object" specified by name.
        This file descriptor is used by other functions such as mmap() & mprotect() to refer to the shared memory object.
        The state of the shared memory object, including all data associated with it, persists until the shared memory object is unlinked
        and all other references are gone.

        The parameters of shm_open() are:
        1. The name of the shared memory segment
        2. The value of oflag (O_RDONLY, O_RDWR, O_CREAT, O_EXCL, O_TRUNC) = read-only, read and write, create, error if create and file exists, truncate to zero length
        3. The permission of the shared memory object*/
        shm_fileDes = shm_open(segmentName, O_CREAT | O_RDWR, 0666);

        /*The ftruncate() function truncates the file indicated by the open file descriptor to the indicated length (in this case 4096).
        If the file size exceeds length, any extra data is discarded. If the file size is smaller than length, bytes between the old and new lengths
        are read as zero. A change to the size of the file has no impact on the file offset.

        The parameters of ftruncate() are:
        1. The file descriptor
        2. The indicated length to be truncated to*/
        ftruncate(shm_fileDes, SIZE);

        /*The mmap() function establishes a mapping between an address space of a process and a file associated with the file descriptor
        at the offset for length of bytes.

        The parameters of mmap() are:
        1. The starting address of the memory region to be mapped
        2. The length
        3. The permitted access to the pages being mapped (read, write, execute, etc.)
        4. The handling of the mapped region (MAP_SHARED write references to the memory region by the calling process may change the file and are visible in all MAP_SHARED mappings of the same portion of the file by any proceess)
        5. The file descriptor
        6. The file byte offset at which the mapping starts*/
        shm_baseAdd = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fileDes, 0);

        /*Must convert numChildren from char to int so it can be used in the for loop*/
        int intChildren = atoi(numChildren);

        for (int i = 0; i < intChildren; i++) {
            /*fork() system call is used to create child process*/
            pid_t childPID = fork();

            /*Make sure that fork was successful. A '0' means the fork was successful*/
            if (childPID == 0) {
                char numList[10];

                /*Convert integer to char in order to be passed as an argument*/
                sprintf(numList, "%d", i+1);

                /*Arguments that will be passed to the child program*/
                char *args[] = {numList, segmentName, NULL};

                /*The child process is executed via execv() system call which executes the file at the specific pathname
                which in this case is /slave */
                execv("./slave", args);

                /*Exiting child process*/
                exit(0);
            }
        }

        /*Output number of child process created by master to be executed by slave*/
        printf("Master created %s child processes to execute slave\n", numChildren);

        /*Waiting for all child processes to exit*/
        printf("Master waits for all child processes to terminate\n");
        while(wait(NULL) != -1);

        /*Output acknowledgement message*/
        printf("Master received termination signals from all %s child processes\n", numChildren);

        /*Output the content of the shared memory segment*/
        printf("Content of shared memory segment filled by child processes:\n");
        printf("--- content of shared memory ---\n");

        for (int i = 1; i < intChildren + 1; i++) {
            /*For every child process, output the content of the shared memory filled in by the slave*/
            printf("%d\n", shm_baseAdd -> response[i]);
        }

        /*The munmap() function shall remove any mappings for those entire pages containing
        any part of the address space of the process starting at (shared memory base) address and continuing for (SIZE) length bytes*/
        munmap(shm_baseAdd, SIZE);

        /*The close() function is used to close a file, in this case it is used to close the shared memory segment as if it was a file*/
        close(shm_fileDes);

        /*The shm_unlink() function shall remove the name of the shared memory object named by the string pointed to by (segmentName)*/
        shm_unlink(segmentName);

        /*Removes the shared memory and then exits*/
        printf("Master removed shared memory segment, and is exiting\n");
    }

    /*Exit process*/
    exit(0);
}
