// File:	tcp-server-mt.C
// Created by:	Narayan J. Kulkarni
// Description:	Multithreaded Server, full duplex TCP connection
// Usage:	tcp-server-mt port
//		where 'port' is the well-known port number to use.
//
// This program uses TCP to establish connections with clients and process
// requests.
//

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
using namespace std;

#define	PORT_MIN	1024
#define	poPORT_MAX	65535


struct client_thread_info
{

  int thread_num;
  int port_num;
  sockaddr_in *cli_ip;

};

/* function prototypes and global variables */
void *do_chld(void*);		// Prototype for the function that handles each connection
int	threadCount;		// Global variable thread count

int main( int ac, char **av ){
	int	s,  newsockfd;			// file descriptor for our socket
	struct	sockaddr_in server, cli_addr; 	// server's IP address
	struct client_thread_info thread_client;
	int	port;				// server's port number
	pthread_t chld_thr;
	unsigned int clilen;
	threadCount=0;

	//
	// Make sure that the port argument was given
	//
	if( ac < 2 ){
		cerr << "Usage: " << av[ 0 ] << " port" << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Get the port number, and make sure that it is legitimate
	//
	port = atoi( av[ 1 ] );
	if( port < PORT_MIN || port > PORT_MAX ){

		cerr << av[ 1 ] << ": invalid port number" << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Create a stream (TCP) socket.  The protocol argument is left 0 to
	// allow the system to choose an appropriate protocol.
	//
	s = socket( AF_INET, SOCK_STREAM, 0 );
	if( s < 0 ){
		cerr << "socket: " << strerror( errno ) << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Fill in the addr structure with the port number given by
	// the user.  The OS kernel will fill in the host portion of
	// the address depending on which network interface is used.
	//
	bzero( &server, sizeof( server ) );
	server.sin_family = AF_INET;
	server.sin_port = htons( port );

	//
	// Bind this port number to this socket so that other programs can
	// connect to this socket by specifying the port number.
	//
	if( bind( s, (struct sockaddr *)&server, sizeof( server ) ) < 0 ){
		cerr << "bind: " << strerror( errno ) << endl;
		exit( EXIT_FAILURE );
	}

	//
	/* set the level of thread concurrency we desire */
   	pthread_setconcurrency(2);

	// Make this socket passive, that is, one that awaits connections
	// rather than initiating them.
	//
	if( listen( s, 5 ) < 0 ){
		cerr << "listen: " << strerror( errno ) << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Now start serving clients
	//

	thread_client.thread_num=newsockfd;
	thread_client.port_num=port;
	thread_client.cli_ip=&cli_addr;

	for(;;){

		cout << "WAITING FOR A CONNECTION ..." << endl;
		clilen = sizeof(cli_addr);
		newsockfd = accept(s, (struct sockaddr *) &cli_addr, &clilen); 

		if(newsockfd < 0)
		cout<<"error"<<endl;
		//Increment thread count
		threadCount++;
		// Create a thread to handle this connection.
		pthread_create(&chld_thr, NULL, do_chld, (void*) newsockfd); 
		/* the server is now free to accept another socket request */
		cout<<"NEW SERVER THREAD CREATED."<<endl;
		}
	return(0);
}

void *do_chld(void* arg)
{
int 	mysocfd;
char 	buf [100];
int 	i, len;
	//copy argument (socket id)
	mysocfd=(int)arg;
	//Wait till the output of the main thread completes 
	for(int x=0; x<10000000;x++);
	//Now proceed
	cout << "New thread started ... total # of threads running at this time is "<<threadCount<<endl;
	//Read buffer from stdio
        len = read( mysocfd, buf, sizeof( buf ) );
        while( len > 0 ){
       		 // Print the message we got
		cout<<"Client sent: "<<buf<<endl;
        	// Send back the message length.
		cout<<"Sendling length to client "<<len<<endl;
		//Convert length to proper byte alignment
		len = htonl(len);
        	if( write( mysocfd, (char*)&len, sizeof(len)) !=sizeof(len)) {
                   	cerr << "write: " << strerror( errno ) << endl;
                   	exit( EXIT_FAILURE );
       		}	

       // Read the next message
       len = read( mysocfd, buf, sizeof( buf ) );
       }
	/* close the socket and exit this thread */
	close(mysocfd);
	threadCount--;
	cout << "Thread exiting... total # of threads running at this time is "<<threadCount<<endl;
	pthread_exit((void *)0);
	return 0;
}
