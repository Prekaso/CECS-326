/*
 *	Name: Dylan Dang
 *  ID: 026158052
 *	Course: CECS 326-01
 *	Professor: Shui Lam
 *	Program: Assignment 2 - Message Process
 *  File: sender.cc
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
struct my_msgbuf {
	long mtype;
	char msg[50];
};

/* Function prototype */
void sendMessage(int qid);

int main(int argc, const char *argv[]){

	/* Sender gets PID and begins execution */
	cout << "Sender, PID " << getpid() << ", begins execution" << endl;

	/* Converting the command line argument (message id) into an integer. */
	int msg_qid = atoi(argv[1]);

	/* Pass message queue id to sendMessage() function */
	sendMessage(msg_qid);
	cout << "Sender terminates" << endl;
	return 0;
}

/* Function responsible for sending message to receiver. */
void sendMessage(int qid){
	my_msgbuf msg;

	/* Calculating the message size */
	int size = sizeof(msg) - sizeof(long);

	/* Defining the message type. As long as both the sender and receiver
	   share the same value, any value would work. */
	msg.mtype = 113;

	/* Holds user-inputted message */
	string message;

	cout << "Sender received message queue id " << qid << " through commandline parameter" << endl;
	cout << "Sender: Please input your message " << endl;

	/* Getting user input */
	cin >> message;

	/* Storing the message in message buffer (msgbuf) */
	strcpy(msg.msg, message.c_str());

	/* The msgsend() system call identifies
   	the message queue by the identifier that was passed in via
   	a commandline argument and then appends a copy of the user's
   	inputted message that is pointed to by into the message buffer. */
	msgsnd(qid, (struct my_msgbuf*)&msg, size, 0);
	cout << "Sender sent message to message queue" << endl;
	cout << "Sender receives acknowledgement of receipt of message" << endl;
}
