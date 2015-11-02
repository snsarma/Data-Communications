// File:	Timer.h
// Author:	K. Reek
// Contributors:
// Description:	These functions use the Unix alarm signal to implement
//		a convenient timeout capability.  The client sets a
//		timeout by calling start_timer with the desired duration
//		and a pointer to a boolean flag.  The flag is initialized
//		to false and the timeout is set up. 
//
//		Usually, the client then begins doing something that
//		may take a long time (such as a system call, or a long
//		loop).  If the timeout expires before this work is done,
//		the flag will be set.  If the client was blocked in a
//		system call (such as read or recvfrom), the call will
//		return with an error.  The client should stop the timer,
//		and then check the flag to see whether the work completed
//		or the timeout elapsed.
//
// SCCS ID:	@(#)Timer.h	1.1	10/03/01
//

//
// Name:	start_timer
//
// Description:	Initialize the flag to false and start a timeout.
//		If the timeout expires before it is cancelled, the 
//		flag will be set true.
// Arguments:	The timeout interval (in seconds), and a flag.
// Modifies:	The flag is initialized to false, and a different
//		handler is set up for the SIGALRM signal.
// 
void start_timer( int seconds, bool &flag );

//
// Name:	stop_timer
//
// Description:	Cancels a timeout.  This function will have no effect
//		if it is called after a timeout has elapsed.
// Modifies:	Restores the original SIGALRM handler.
//
void stop_timer();
