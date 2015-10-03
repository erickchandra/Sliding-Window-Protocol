/* File name			: transmitter.c
 * Author				: Erick Chandra
 * Author email			: erickchandra.1@gmail.com
 * Colleague 1			: Candy Olivia Mawalim
 * Colleague 1 email	: 
 * Colleague 2			: Ignatius Alriana Moel
 * Colleague 2 email	: 
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
int sock, length;
struct sockaddr_in server, from;
struct hostent *hp;

// Function Declaration
void hError(char *msg);

// Main program
int main(int argc, char *argv[]) {
	FILE *fp;
	
	/// Initial Process
		if (argc != 4) {
			fprintf(stderr, "ERROR: Argument(s) missing\n");
			exit(1);
		}
		
		fp = fopen(argv[3], "r");
		if (fp == NULL) {
					hError("ERROR: Opening text file");
		}
		
		fp = fopen(argv[3], "r");
		if (fp == NULL) {
			hError("ERROR: Opening text file");
		}
		
		printf("Opening socket to connect to %s:%d...\n", argv[1], atoi(argv[2]));
		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) {
			hError("ERROR: Opening Socket");
		}
		
		server.sin_family = AF_INET;
		hp = gethostbyname(argv[1]);
		if (hp == 0) {
			hError("ERROR: Unknown host");
		}
		
		bcopy((char *) hp->h_addr, (char *) &server.sin_addr, hp->h_length);
		
		server.sin_port = htons(atoi(argv[2]));
		length = sizeof(struct sockaddr_in);
	
	/// Core Process
		
	
	return 0;
}

// Function Definition Realisation
void hError(char *msg) {
	perror(msg);
	exit(1);
}
