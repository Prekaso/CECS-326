/*
 *	Name: Dylan Dang
 *  ID: 026158052
 *	Course: CECS 326-01
 *	Professor: Shui Lam
 *	Program: Assignment 2 - Message Process
 *  File: receiver.cc
*/

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

/* The 'my_msgbuf' struct acts as a pointer argument
   for both msgsend() and msgrvc(). */
struct my_msgbuf{
	long mtype;
	char msg[50];
};

/* Function Prototype */
void receiveMessage(int qid);

int main(int argc, const char *argv[]){

	/* Receiver gets PID and begins execution */
	cout << "Receiver, PID " << getpid() << ", begins execution" << endl;

	/* Converting the command line argument (message id) into an integer. */
	int msg_qid = atoi(argv[1]);

	/* Pass message queue id to receiveMessage() function */
	receiveMessage(msg_qid);
	cout << "Receiver terminates" << endl;
	return (0);
}

/* Function responsible for receiving message and outputting message. */
void receiveMessage(int qid){
	my_msgbuf msg;

	/* Calculating the message size */
	int size = sizeof(msg) - sizeof(long);

	/* The msgrcv() system call takes the message out of the message buffer
	   by identifying the message queue using the identifier argument.
	   The message is then placed into the buffer. */
	msgrcv(qid, (struct my_msgbuf*)&msg, size, 113, 0);
	cout << "Receiver received message queue id " << qid << " through commandline parameter" << endl;
	cout << "Receiver acknowledged receipt of message" << endl;

	/* Outputs message gotten from message queue. */
	cout << "Receiver: Retrieved the following message from message queue \n" << msg.msg << endl;
}
