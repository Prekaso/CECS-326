#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main (int children, char* children_data[]) {

	// since passed in data are in name-gender pairs, we divide by 2.
	int numOfChildren = children/2;

	// printing total number of children.
	printf("I have %d children.", numOfChildren);
	fflush(stdout);

	// creating process id for child and child wait status
	pid_t child_pid, childStatus_pid;

	// childIndex keeps track of number of children
	char childIndex[3];

	for (int i = 0; i < numOfChildren; i++) {

		// label new process as the child
		child_pid = fork();

		// make sure child process exists
		if ((child_pid == 0)) {
			sprintf(childIndex, "%d", i+1);

			// prepping the data to be sent to child program.
			char *dataForChild[] = {childIndex, children_data[i*2+1], children_data[i*2+2], NULL};

			// executing the child program with the data provided.
			execv("./child", dataForChild);

			// exit child program
			exit(0);
		}
	}

	// waiting for all child processes to finish
	int childStatus = 0;
	while ((childStatus_pid = wait(&childStatus)) > 0);
	printf ("\nAll child processes terminated. Parent exits.\n");

}
