// $com CC $@ -o $* -lsocket -lnsl -library=iostream
//
// File:	tcp-server.C
// Author:	K. Reek
// Modified by:	Narayan J. Kulkarni
// Description:	Server, full duplex TCP connection
// Usage:	tcp-server port
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
using namespace std;

#define	PORT_MIN	1024
#define	PORT_MAX	65535

int main( int ac, char **av ){
	int	s;		// file descriptor for our socket
	struct	sockaddr_in server; // server's IP address
	int	port;		// server's port number

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
	// Make this socket passive, that is, one that awaits connections
	// rather than initiating them.
	//
	if( listen( s, 1 ) < 0 ){
		cerr << "listen: " << strerror( errno ) << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Now start serving clients
	//
	for(;;){
		int	ns;		// new socket for the connection
		char	buf[512];	// data buffer
		int	len;		// buffer length

		//
		// Wait for a client's connection request
		//
		cout << "Waiting for a connection ..." << endl;
		ns = accept( s, 0, 0 );
		if( ns < 0 ){
			cerr << "accept: " << strerror( errno ) << endl;
			continue;
		}

		cout << "Connected to client " << endl;

		//
		// Now read and respond to messages until end of file is
		// reached (which means the client closed the connection).
		//
		// NOTE: This code assumes that the data is written by the
		// client all at once, and that it arrives all at once!  If
		// it doesn't, we'll just look at the first part of the
		// data, and we'll return the wrong result.  In addition,
		// the remaining data may be concatenated with the next
		// thing that the client writes, ruining that reply as well.
		//
		len = read( ns, buf, sizeof( buf ) );
		while( len > 0 ){
			//
			// Print the message we got
			//
			cout << "Client sent: " << buf << endl;

			//
			// Send back the message length.
			//
			cout << "Echoing the string to client"<< endl;
			len = ntohl(len);
			if( write( ns, (char*)&len, sizeof(len)) !=sizeof(len)) {
				cerr << "write: " << strerror( errno ) << endl;
				exit( EXIT_FAILURE );
			}

			//
			// Read the next message
			//
			len = read( ns, buf, sizeof( buf ) );
		}

		//
		//
		if( len < 0 ){
			cerr << "read: " << strerror( errno ) << endl;
			exit( EXIT_FAILURE );
		}

		//
		// When client closes connection, the read returns with zero bytes. 
		// End of data -- the client closed the connection.
		// Close the connection socket and resume listening on
		// the original socket.
		//
		close( ns );
		cout << "Connection closed.\n" << endl;
	}
}
