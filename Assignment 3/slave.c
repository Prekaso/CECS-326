#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "myShm.h"

int main(int argc, char* argv[]) {

    /*Before starting, make sure argument count is valid*/
    if (argc >= 2) {
        /*Output identifying message*/
        printf("Slave begins execution\n");

        /*Store number of children and shared memory name*/
        char* childNum = argv[0];
        char* segmentName = argv[1];

        /*Convert char to integer*/
        int int_childNum = atoi(childNum);

        /*Declare required size of memory request*/
        const int SIZE = 4096;

        /*Output child number and segment name from exec() system call */
        printf("I am child number %s, received shared memory name %s\n", childNum, segmentName);

        /*Create base address as pointer to struct CLASS*/
        struct CLASS *shm_baseAdd;

        /*Create shared memory file descriptor to reference values from response array*/
        int shm_fileDes;

        /*The shm_open() function returns a file descriptor that is associated with the shared "memory object" specified by name.
        This file descriptor is used by other functions such as mmap() & mprotect() to refer to the shared memory object.
        The state of the shared memory object, including all data associated with it, persists until the shared memory object is unlinekd
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

        /*Writes its child number into the next available slot in the shared memory*/
        shm_baseAdd -> index = int_childNum;
        shm_baseAdd -> response[int_childNum] = int_childNum;

        /*Output acknowledgement message*/
        printf("I have written my child number to shared memory\n");

        /*The munmap() function shall remove any mappings for those entire pages containing 
        any part of the address space of the process starting at (shared memory base) address and continuing for (SIZE) length bytes*/
        munmap(shm_baseAdd, SIZE);

        /*The close() function is used to close a file, in this case it is used to close the shared memory segment as if it was a file*/
        close(shm_fileDes);

        /*Removes shared memory*/
        printf("Slave closed access to shared memory and terminates\n");
    }

    /*Exit process*/
    exit(0);
}
