// $com CC $@ -o $* -lsocket -lnsl -library=iostream
//
// File:	udp-client.C
// Author:	K. Reek
// Modified by: Narayan J. Kulkarni
// Modified by: Shravya Narayan Sarma
// Description: Client, full duplex UDP connection
// Usage:	udp-client hostname port
//		where 'hostname' is the name of the remote host on which
//		the server is runnning, and 'port' is the port number it
//		is using.
//
// This program uses UDP to send data to a server program on another machine
// and read replies from the server.
//

#include"Timer.h"
#include"datagram.h"
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
using namespace std;

#define	PORT_MIN	1024
#define	PORT_MAX	65535
#define bzero(p,l) memset(p,0,l)

typedef struct{
  
   int command;
   int id;
   char name[32];
   int age;

}myrecord;

int main( int ac, char **av ){
	int	s;		// file descriptor for our socket
	int   ret;           // return code for the method sendto
       char	*hostname;	// name of remote host
	struct	hostent	*hostent; // to get IP address of remote host
	struct	sockaddr_in server; // address of remote socket
       struct sockaddr_in rem_from;//address of remote socket
	int	port;		// port number of remote socket
       int  seq=0;

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
	// Create a datagram (UDP) socket.  The protocol argument is left 0 to
	// allow the system to choose an appropriate protocol.
	//
	s = socket( AF_INET, SOCK_DGRAM, 0 );
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
	// Now read lines from the keyboard, and send each line to the
	// server as a separate datagram.
	//
	//cout<<"Target server is "<<inet_ntoa(server.sin_addr)<<endl;
	//cout << "? ";
	//cin.getline( buf, sizeof( buf ) );
	
       Datagram sendto_data;
       Datagram recv_back;
       sendto_data.type=SYN;
       sendto_data.seq=seq;
       bool time_out=false;
       int len;
       int num_attempts=0;  
       while( (!time_out && recv_back.type != ACK)||(num_attempts>6) ){
		if(sendto(s,(char*)&sendto_data,8,0,(struct sockaddr *)&server,sizeof(server))!=8)
		{
			cerr << "sendto: " << strerror( errno ) << endl;
			exit( EXIT_FAILURE );
		}

		start_timer(1,time_out);
              num_attempts++;
              len = recvfrom(s,(Datagram*)&recv_back,sizeof(recv_back),0,(struct sockaddr *)&rem_from,(socklen_t *)sizeof(rem_from));
              stop_timer();
	}

       if(num_attempts>6){
        cout<<"SYN ACK not received"<<endl;
        return(EXIT_FAILURE);
     
     }

      while(true){

        myrecord myrec;
        string inp_str;
        int command=0;
        int id=0;

        do{

            cout<<"(0 to add 1 to retrieve 2 to exit):";
            getline(cin,inp_str);
            myrec.command=command=atoi(inp_str.c_str());
            if(myrec.command == 2){
             sendto_data.type = FIN;
             sendto_data.seq=seq;
             sendto_data.type=FIN;
         
             while((!time_out && recv_back.type != ACK)||(num_attempts>6))
             {
                if(sendto(s,(char *)&sendto_data,8,0,(struct sockaddr *)&server,sizeof(server))!=8)
                {
                     cerr<<"sendto:"<<strerror(errno)<<endl;
                     exit(EXIT_FAILURE); 
                 }
                  start_timer(1,time_out);
                  num_attempts++;
                  len=recvfrom(s,(Datagram*)&recv_back,sizeof(recv_back),0,(struct sockaddr *)&rem_from,(socklen_t*)sizeof(rem_from));
                  stop_timer(); 
            }
               
               if(num_attempts > 6)
               {
                  cout<<"ACK Not Received:"<<endl;
                  return EXIT_FAILURE;
               } 
               close(s);
               return EXIT_SUCCESS;
            }
     }while(myrec.command !=1 && myrec.command !=0);
	
      cout<<"ID:";
      getline(cin,inp_str);
      myrec.id=atoi(inp_str.c_str());
       if(myrec.command == 0)
       {
         cout<<"Name:";
         inp_str.clear();
         cin.getline(myrec.name,31);
         cout<<"Age:";
         getline(cin,inp_str);
         myrec.age=atoi(inp_str.c_str());

       }

       sendto_data.type=DATA;
       sendto_data.seq=seq;
       memcpy(sendto_data.data,&myrec,sizeof(myrec)); 
       num_attempts=0;
       time_out = false;
            recv_back.type = DATA;
            while ((!time_out && recv_back.type != ACK) || num_attempts > 6) {
                
                if( sendto( s, (char*)&sendto_data, (2*sizeof(int) + sizeof(myrec)), 0, (struct sockaddr *)&server, sizeof(server)) != (2*sizeof(int) + sizeof(myrec)) ){
                    cerr << "sendto: " << strerror( errno ) << endl;
                    exit( EXIT_FAILURE );
                }
                start_timer(1,time_out);
                num_attempts++;
                //Recieve ACK
                len = recvfrom( s, (Datagram*)&recv_back, sizeof( recv_back ), 0, (struct sockaddr *)&rem_from, (socklen_t *)sizeof(rem_from));
                stop_timer();
                if (recv_back.seq != seq) {
                    cout << "Ignore wrong sequence number." << endl;
                    recv_back.type = DATA;
                }
            }
            if (num_attempts > 6) {
                cout << " Acknowledgement not received." << endl;
                return EXIT_FAILURE;
            }
			//Recieve DATA
            while (recv_back.type != DATA) {
                len = recvfrom( s, (Datagram*)&recv_back, sizeof( recv_back ), 0, (struct sockaddr *)&rem_from, (socklen_t *)sizeof(rem_from));
                if (recv_back.seq != seq) {
                    cout << "Disgarded wrong sequence number." << endl;
                    recv_back.type = ACK;
                }
                //Send ACK
                sendto_data.type = ACK;
                sendto_data.seq = seq;
                if( sendto( s, (char*)&sendto_data, 8, 0, (struct sockaddr *)&server, sizeof(server)) != 8 ){
                    cerr << "sendto: " << strerror( errno ) << endl;
                    exit( EXIT_FAILURE );
                }
            }
            //---------------------/DATA STEP
            
            myrecord *recv = (myrecord *)&recv_back.data;
            if (command == 1) {
                if (recv->command == 0) {
                    cout << "ID: " << recv->id << endl << "Name: " << recv->name << endl << "Age: " << recv->age << endl;
                } else {
                    cout << "ID: " << recv->id << " is not found." << endl;
                }
            } else if (command == 0) {
                if (recv->command == 0) {
                    cout << "ID: " << recv->id << " added successfully!" << endl;
                } else {
                    cout << "ID: " << recv->id << " already exists!" << endl;
                }
            }
            seq++;
		}

       //
	// We're all done.  Close the socket and quit.
	//
	//close( s );

	return EXIT_SUCCESS;
}
