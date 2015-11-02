// File:	tcp-server-mt.C
// Created by:  Narayan J. Kulkarni
// Modified by: Shravya Narayan Sarma
// Description: Multithreaded Server, full duplex TCP connection
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
#include <vector>

using namespace std;

#define	PORT_MIN	1024
#define	PORT_MAX	65535

/* function prototypes and global variables */
void *do_chld(void*);		// Prototype for the function that handles each connection
int	threadCount;
int threadnumber ;// Global variable thread count

typedef struct {
	int thread_num;
	int sock_fd;
	struct in_addr client_ip;           // structure to store client information
	int port;
	
}client_info;

typedef struct{
	int command;
	int id;
	char name[32];			// structure to store client record
	int age;
	}client_record;

	vector <client_record>record;

int main( int ac, char **av ){
	int	s, clilen, new_sockfd;		// file descriptor for our socket
	struct	sockaddr_in server, cli_addr; // server's IP address
	pthread_t chld_thr;
	int	port;		// server's port number
 	
	threadCount=0;
	threadnumber = 1;


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
	for(;;){

		cout << "MAIN THREAD-WAITING FOR THE FIRST  CONNECTION FROM CLIENT ..." << endl;
		clilen = sizeof(cli_addr);
		new_sockfd = accept(s, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen); 

		if(new_sockfd < 0)
			cout<<"error"<<endl;
			
			client_info c;
			c.thread_num = threadnumber;
			c.sock_fd = new_sockfd;
			c.client_ip = cli_addr.sin_addr;
			c.port = cli_addr.sin_port;


		// Create a thread to handle this connection.
		pthread_create(&chld_thr,NULL,do_chld, (void *)&c); 
		threadCount++;
		threadnumber++;
		/* the server is now free to accept another socket request */
		cout<<"NEW THREAD CREATED: NO. "<<c.thread_num<<endl;
		cout<<"MAIN THREAD - WAITING FOR THE NEXT CONNECTION FROM CLIENT..."<<endl;
		}
	return(0);
}

void *do_chld(void* arg)
{
int 	mysocfd;
char 	buf [100];
int 	i, len;
	
	 client_info *c;
	 c =reinterpret_cast<client_info *>(arg);
 	cout<<"Entering Thread # "<<c->thread_num<<" Client IP: "<<inet_ntoa(c->client_ip)<<", Port: "<<c->port<<":"<<endl;
	cout<<"=============================================="<<endl;
	 //Wait till the output of the main thread completes
	 for(int x=0; x<10000000;x++);
			         

/*	//Read buffer from stdio
                len = read( mysocfd, buf, sizeof( buf ) );
                while( len > 0 ){
                        //
                        // Print the message we got
			cout<<"Client sent: "<<buf<<endl;
			//
                        // Send back the message length.
                        //
			cout<<"Sendling length to client "<<len<<endl;
                        if( write( mysocfd, (char*)&len, sizeof(len)) !=sizeof(len)) {
                                cerr << "write: " << strerror( errno ) << endl;
                                exit( EXIT_FAILURE );
                        }

                        //
                        // Read the next message
                        //
                        len = read( mysocfd, buf, sizeof( buf ) );
                }*/

	/* close the socket and exit this thread */
 client_record d1;
         len = read(c->sock_fd,(void *)&d1,sizeof(d1));

while(len>0){	
	//client_info *c;
//	c= reinterpret_cast<client_info *>(&arg);



	 d1.command = ntohl(d1.command);
	 d1.id = ntohl(d1.id);
	 
	 d1.age = ntohl(d1.age);

	
	cout<<"Received "<<(len-1)<<" bytes ASCII string from the socket"<<endl;
		 	
		int  s = d1.command;
		bool hasRecord = false;
		
		switch(s)
		{
		case 0:{
			client_record d2;
			 cout<<"Command is "<<d1.command<<endl;
			 cout<<"Id is "<<d1.id<<endl;
			 cout<<"Name is "<<d1.name<<endl;
			 cout<<"Age is "<<d1.age<<endl;
			  vector<client_record>::iterator it;
			  
			  for (it = record.begin(); it != record.end(); it++) {
			  
			  	if (it->id == d1.id) {
					hasRecord = true;
					break;
				}
				
			  }
			  if (!hasRecord) {
			  	record.push_back(d1);
				cout<<"Record added to the database. Size of the Map : "<<record.size()<<endl;
				hasRecord = false;
				d2.command = ntohl(0);
				write(c->sock_fd,(void *)&d2, sizeof(int));


			  }
			 
			  else{
			  cout<<"Record with ID already exists in the database."<<endl;
			  d2.command = ntohl(1);
			  write(c->sock_fd,(void *)&d2,sizeof(int));

			  }
			  break;
			}
		
		case 1:{
			cout<<"Command is "<<d1.command<<endl;
			cout<<"Record Id is "<<d1.command<<endl;

			vector<client_record>::iterator it;
			bool found = false;
			int count =0;

			for(it = record.begin(); it != record.end(); it++){

			if(it->id == d1.id) {
				found = true;
				cout<<"Name is "<<it->name<<endl;
				cout<<"Age is " <<it->age<<endl;
				cout<<"Record exists in the database"<<endl;
	                        client_record d3;
				d3= record.at(count);
                                d3.command = ntohl(0);
				d3.id = ntohl(d3.id);
				d3.age = ntohl(d3.age);
				if(write(c->sock_fd,(void *)&d3, sizeof(d3)) != sizeof(d3)) {
				cout<<"Write: "<<strerror(errno)<<endl;
				exit( EXIT_FAILURE);
				}

				break;
				}
				count++;
			}
			if(!found){
				client_record d4;
				d4.command = ntohl(1);
				cout<<"Record does not exist in database."<<endl;
				write(c->sock_fd,(void *)&d4,sizeof(int));
				}

				break;
		}
		case 2:{
			close(c->sock_fd);
			threadCount--;
			cout << "Exiting Thread # "<<c->thread_num<<" Client IP: "<<inet_ntoa(c->client_ip)<<", Port: "<<c->port<<": ... client closed the socket=============="<<endl; 

			cout<<"Total # of threads running at this time is "<<threadCount<<endl;
			pthread_exit((void *)0);
			return 0;
			break;
		}
		default: 
			cout<<"default"<<endl;
			break;
			}
	   len = read(c->sock_fd,(void *)&d1,sizeof(d1));

}	
	close(c->sock_fd);
	threadCount--;
	 cout << "Exiting Thread # "<<c->thread_num<<" Client IP: "<<inet_ntoa(c->client_ip)<<", Port: "<<c->port<<": ... client closed the socket=============="<<endl;

	  cout<<"Total # of threads running at this time is "<<threadCount<<endl;
	pthread_exit((void *)0);
	return 0;
}
