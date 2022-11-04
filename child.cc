#include <iostream>
#include <stdio.h>

using namespace std;

int main (int children, char* children_data[]) {
	// output children with child number, gender, and name
	printf("\n Child #%s: I am a %s and my name is %s.", children_data[0], children_data[1], children_data[2]);
	return 0;
}
