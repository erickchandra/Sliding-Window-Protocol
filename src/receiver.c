/* File name			: receiver.c
 * Author				: Erick Chandra
 * Author email			: erickchandra.1@gmail.com
 * Colleague 1			: Candy Olivia Mawalim
 * Colleague 1 email	: 
 * Colleague 2			: Ignatius Alriana Moel
 * Colleague 3 email	: 
 * Description			: This program is created to complete Computer Networks course assignment.
 * Topic				: Sliding Window Protocol + flow control and error control
 * References			: Found in the document
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <strings.h>
#include <unistd.h>
#include "dcomm.h"

// Global dictionaries


// Function Declaration
void hError(char *msg);

// Main program
int main(int argc, char *argv[]) {
	
	return 0;
}

// Function Definition Realisation
void hError(char *msg) {
	perror(msg);
	exit(1);
}
