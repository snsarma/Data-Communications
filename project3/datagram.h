// File:        datagram.h
// Author:      The 0603-420/740 class of 20011
// Contributors: K. Reek
// Description: Layouts of the datagram and ack frames for sendfile-udp
// SCCS ID:     @(#)datagram.h  1.2     10/04/01
//

#ifndef _DATAGRAM_H
#define _DATAGRAM_H

//
// Name:        Datagram layout
//
//              Note that when sending a datagram with fewer than 512 bytes
//              of data, the remaining (empty) bytes in the data array are
//              NOT sent.  The server uses the length of the received
//              datagram to determine how much data it contains.
//
#define MAX_DATA        512     // Max data allowed in a single datagram

        enum    TYPE { DATA, ACK, SYN, FIN } type;      // Type of this datagram
typedef struct {
        TYPE type;      // Type of this datagram
        unsigned int seq;       // Sequence number of this datagram
        char data[ MAX_DATA ];  // The data itself (if any).
} Datagram;

#endif  // _DATAGRAM_H


