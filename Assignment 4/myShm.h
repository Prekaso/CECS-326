/* myShm.h*/
/* Header file to be used with master.c and slave.c
*/
struct CLASS {

	/*Index to next available response slot*/
	int index;

	/*Each child writes its child number here*/
	int response[10];
};
