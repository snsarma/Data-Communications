// $com CC $@ -o $* -lsocket -lnsl -library=iostream
//
// File:	tcp-client.C
// Author:	K. Reek
// Modified by:	Narayan J. Kulkarni
// Modified by: Shravya Narayan Sarma
// Description:	Client, full duplex TCP connection
// Usage:	tcp-client hostname port
//		where 'hostname' is the name of the remote host on which
//		the server is running, and 'port' is the port number it
//		is using.
//
// This program uses TCP to send data to a server program on another machine
// and read replies from the server.
//

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
using namespace std;

#define	PORT_MIN	1024
#define	PORT_MAX	65535

typedef struct
{

int command;
int id; 
char name[32];
int age;
}data_record;

int main( int ac, char **av ){
	int inp_command,inp_age,inp_id,input;
	int	s;		// file descriptor for our socket
	char	buf[512];	// data buffer
	char	*hostname;	// name of remote host
	struct hostent *hostent; // to get IP address of remote host
	struct	sockaddr_in server; // address of remote socket
	int	port;		// port number of remote socket
	int 	length,len;
	data_record client_record;
	data_record client_record_recv;
	//
	// Make sure that hostname and port arguments were both given
	//
	if( ac < 3 ){
		cerr << "Usage: " << av[ 0 ] << " hostname port" << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Grab the host name
	//
	hostname = av[ 1 ];

	//
	// Get the port number, and make sure that it is legitimate
	//
	port = atoi( av[ 2 ] );
	if( port < PORT_MIN || port > PORT_MAX ){
		cerr << av[ 2 ] << ": invalid port number" << endl;
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
	// Look up the IP address of the host.  "Hostent" is a pointer
	// to a structure in which the address has been stored.
	//
	hostent = gethostbyname( hostname );
	if( hostent == NULL ){
		cerr << "gethostbyname: error code " << h_errno << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Fill in the "server" structure with the address of the socket
	// that the server is listening to.  First, the entire structure
	// must be zeroed.  Then that the host address and port number
	// are converted to network byte order and stored.
	//
	bzero( &server, sizeof( server ) );
	server.sin_family = AF_INET;
	server.sin_port = htons( port );
	server.sin_addr.s_addr = *(u_long *)hostent->h_addr;

	//
	// Connect our socket "s" to the server's socket.  Note that
	// the server must be awaiting connections for this to succeed.
	//
	if( connect( s, (struct sockaddr *)&server, sizeof( server ) ) < 0 ){
		cerr << "connect: " << strerror( errno ) << endl;
		exit( EXIT_FAILURE );
	}

	//
	// Now read lines from the keyboard, and send each line to the
	// server as a separate message.
	//
	//cout << "Enter your input: ";
	//cin.getline( buf, sizeof( buf ) );
       for(;;) 
      { 
	cout<<"Enter command(0 for Add 1 for retrieve 2 to Quit):";
	cin>>inp_command;
       switch(inp_command)
       {
	
	 case 0:cout<<"Enter the Id:(integer)";
	        cin>>inp_id;
		cout<<"Enter your name(Upto 32 characters:)";
		cin>>buf;
                cout<<"Enter your age:(integer)"; 
		cin>>inp_age;
		client_record.command=ntohl(inp_command);
		client_record.age=ntohl(inp_age);
                memcpy(client_record.name,buf,strlen(buf)+1);
		client_record.id=ntohl(inp_id);
		while( strlen(buf)!=0 ){
		//int	len;	// message length

		//
		// Get the length of the string (plus one for the trailing
		// null byte), and send it to the server.
		//
		len = strlen( buf ) + 1;
		if( write( s, buf, len ) != len ){
			cerr << "write: " << strerror( errno ) << endl;
			exit( EXIT_FAILURE );
		}

		//
		// Read the reply from the server, and print it.
		//
		// NOTE: this code assumes that the data was written by
		// the server all at once, and that it arrives all at once.
		// If either of these is not true, the client will exit
		// when in fact it should just keep reading until it gets
		// all the bytes.
		//

		write(s,(data_record*)&client_record,sizeof(client_record));

		len = read( s, (char*)&client_record_recv, sizeof( client_record_recv ));

                 if((ntohl(client_record_recv.command) == 0) && (ntohl(client_record_recv.id)!= input))
		  cout<<"ID "<<(inp_id)<<"added successfully"<<endl;
                 else if(ntohl(client_record_recv.command) == 1)
		  cout<<"ID "<<(inp_id)<<"already exists"<<endl;

               // }

		break;

		case 1:
		//cout << "The server said the line was " << ntohl(length)<<" bytes long" << endl;
		    
		//
		// Read another line.
		//
		cout << "Enter the id: ";
		cin>>input;
		client_record.command=ntohl(inp_command);
		client_record.id=ntohl(input);
                write(s,(data_record*)&client_record,sizeof(client_record));
	        len=read(s,(char*)&client_record_recv,sizeof(client_record_recv));
		if((ntohl(client_record_recv.command) == 0)&&(ntohl(client_record_recv.id)==input))
                {
                  //cout<<"Record Exists:"<<endl;
                  cout<<"ID:"<<ntohl(client_record_recv.id)<<endl;
                  cout<<"Name:"<<client_record_recv.name<<endl;
		  cout<<"Age:"<<ntohl(client_record_recv.age)<<endl;
		}
		else if(ntohl(client_record_recv.command) == 1)
		  cout<<"ID "<<(input)<<"doesn't exist"<<endl;
		break;
         }
	//break;
		case 2:exit(0);
		       break;
		//cin.getline( buf, sizeof( buf ) );
		default : cout<<"Illegal Command Entered"<<endl;break;
	}

   }
   //
	// We're all done.  Close the socket and quit.
	//
	close( s );

	return EXIT_SUCCESS;
}
