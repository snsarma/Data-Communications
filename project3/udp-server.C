// $com CC $@ -o $* -lsocket -lnsl -library=iostream
//
// File:	udp-server.C
// Author:	K. Reek
// Modified by:	Narayan J. Kulkarni
// Description: Server, full duplex UDP connection
// Usage:	udp-server port
//		where 'port' is the port number the server should read from.
//
// This server program receives data from a client program on another
// machine, and send replies back to the client.
//

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

#define	PORT_MIN	1024
#define	PORT_MAX	65535

int main( int ac, char **av ){
	int	s;		// file descriptor for our socket
	struct	sockaddr_in addr; // to bind our port number
	int	port;		// our port number

	//
	// Make sure that the port argument was given.
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
	// Create a datagram socket.  The protocol argument is left 0 to
	// allow the system to choose an appropriate protocol.
	//
	s = socket( AF_INET, SOCK_DGRAM, 0 );
	if( s < 0 ){
		cerr << "socket: " << strerror( errno ) << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Fill in the "addr" structure with the port number given by
	// the user.  The OS kernel will fill in the host portion of
	// the address depending on which network interface is used.
	//
	bzero( &addr, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );

	//
	// Bind this port number to the socket.
	//
	if( bind( s, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 ){
		cerr <<  "bind: " << strerror( errno ) << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Now wait for datagrams to arrive, and respond to each one.
	//
	for(;;){
		struct	sockaddr_in client; // client's IP address
		socklen_t	client_len; // length of client's addr
		char	buf[ 512 ];	// data buffer
		int	len;		// buffer length

		//
		// Get a datagram, and print its contents
		//
		cout << "Waiting for a datagram ..." << endl;
		bzero( &client, sizeof( client ) );
		client_len = sizeof( client );
		len = recvfrom( s, buf, sizeof( buf ), 0,
		    (struct sockaddr *)&client, &client_len );
		if( len < 0 ){
			cerr << "recvfrom: " << strerror( errno ) << endl;
			exit( EXIT_FAILURE );
		}
		cout<<"Client sent: " << buf << endl;
		cout<<"Echoing to the server: " << buf << endl;

		//
		// Send back the message length.
		//
		len = htonl(len);
		if( sendto( s, (char*)&len,  sizeof(len) , 0,
		    (struct sockaddr *)&client, sizeof( client ) ) !=
		     sizeof(len) ) {
			cerr << "sendto: " << strerror( errno ) << endl;
			exit( EXIT_FAILURE );
		}
	}
}
