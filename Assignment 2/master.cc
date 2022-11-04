/*
 *	Name: Dylan Dang
 *  ID: 026158052
 *	Course: CECS 326-01
 *	Professor: Shui Lam
 *	Program: Assignment 2 - Message Process
 *  File: master.cc
*/

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

int main() {

	/* The message queue is created via the msgget() system call
	   in which the key_t type "IPC_PRIVATE" and the flag field
	   "IPC_EXCL" return the identifier associated with the
	   value of the key argument. */
	int qid = msgget(IPC_PRIVATE, IPC_EXCL|IPC_CREAT|0600);

	/* Master gets a PID and begins execution.
	   Master also receives the message queue id */
	cout << "Master, PID " << getpid() << ", begins execution" << endl;
	cout << "Master acquired a message queue, id " << qid << endl;

	/* fork() system call is used to create the 1st child process - sender.
	   A '0' means that the fork is successful. */
	pid_t cpid = fork();

	/* This block catches any failures during child process creation */
	if(cpid < 0) {
		cout << "Fork Failed! - child process not created." << endl;
		return 1;
	}

	/* After verifying no failures, sender is loaded in the 1st child
	   and will generate the following messages. */
	else if(cpid == 0) {
		string qidString = to_string(qid);
		char const *qidc = qidString.c_str();
		long pid1;
		cout << "Master created a child process with PID " << pid1 << " to execute sender" << endl;

		/* The child process is executed via the execlp() system call
		   which executed the file at the specified pathname
		   which in this case is /sender. */
		execlp("./sender", "sender", qidc, NULL);
		sleep(1);

		/* Exiting child process. */
		exit(0);
	}

	/* Waiting for sender child to terminate. */
	wait(NULL);

	/* fork() is called again to create 2nd child process - receiver. */
	cpid = fork();

	/* Checks for failure in child process creation. */
	if(cpid < 0) {
		cout << "Fork Failed! - child process not created." << endl;
		return 1;
	}

	else if(cpid == 0) {
		string qidString = to_string(qid);
		char const *qidc = qidString.c_str();
		long pid2;

		/* Like mentioned previously, execlp() is used to
		   execute the file at /receiver. */
		cout << "Master created a child process with PID " << pid2 << " to execute receiver" << endl;
		execlp("./receiver", "receiver", qidc, NULL);

		/* Exiting child process. */
		exit(0);
	}

	/* Waiting for receiver child to terminate */
	wait(NULL);

	/* Waiting for both children to terminate. */
	cout << "Master waits for both child processes to terminate" << endl;
	while(wait(NULL) != -1);

	/* Once the interprocess communication is completed,
	   the msgctl() system call removes the message queue
	   in which it performs control operations on a given
	   message queue if the given argument is "IPC_RMID". */
	msgctl(qid, IPC_RMID, NULL);
	cout << "Master received termination signals from both child processes, removed message queue, and terminates" << endl;
	return 0;
}
