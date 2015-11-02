// File:	Timer.C
// Author: 	K. Reek
// Contributors: 
// Description:	Implementation of a utility timer capability.  See Timer.h
//		for more details.
//

static	char	sccs_id[] = "@(#)Timer.C	1.1	10/03/01";

#include "Timer.h"
#include <unistd.h>
#include <signal.h>


//
// Private data (accessible only to the timer routines).
//
extern "C" {
static	bool	*flag_ptr;		// pointer to user's flag
static	void	(*old_handler)( int ) = NULL; 	// old signal handler
}

//
// Name:	alarm_handler (private helper function)
//
// Description:	Once the timer has been set, this function will be the
//		one called when the timeout expires.  It restores the
//		original signal handler, and then sets the user's flag
//		to true.
// Modifies:	user's flag (set to true)
// Note:	This function is extern "C" because its address is
//		passed to the signal system call, which requires a
//		C (not C++) parameter.
//
extern "C" {
static void alarm_handler( int ){
	//
	// Restore original signal handling
	//
	signal( SIGALRM, old_handler );
	old_handler = NULL;

	//
	// Set user's flag to indicate that a timeout occurred
	//
	*flag_ptr = true;
}
}

//
// Name:	start_timer
//
void start_timer( int seconds, bool &flag ){
	//
	// Arrange for our function to be called when the signal occurs
	//
	old_handler = signal( SIGALRM, alarm_handler );

	//
	// Save a pointer to the user's flag; initialize the flag to false
	//
	flag = false;
	flag_ptr = &flag;

	//
	// Arrange for a signal after the specified number of seconds
	//
	(void)alarm( seconds );
}

//
// Name:	stop_timer
//
void stop_timer(){
	//
	// Cancel any pending signal
	//
	(void)alarm( 0 );

	//
	// Restore the old signal handling, if we know what it is
	//
	if( old_handler != NULL ){
		signal( SIGALRM, old_handler );
		old_handler = NULL;
	}
}
