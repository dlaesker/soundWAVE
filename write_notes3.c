#include "soundwave.h"
#include <time.h>

/* ######################### *
	 # Author: Denis Laesker #
*  ######################### */

int main(){
	srand(time(NULL));
	// Create an instance of
	SOUNDWAVE *swave = INIT_PCMWAVE(
		"sound.wav", 	// Filename. Make sure it does not already exist or it will be overwritten.
		24, 					// Bits per sample (must be a multiple of 8).
		CHMONO, 			// No. of channels. CHMONO = 1, CHSTEREO
		CDSRATE);			// Sampling rate. CDSRATE = 44,100
	if(swave == NULL){
		printf("Something went wrong...\n");
		exit(EXIT_FAILURE);
	}
	
	// Singly-linked-list
	NOTES *notes = NULL;
	notes = APPEND_NOTE(notes,
		C4,		// Note
		0.5,	// Duration
		100);	// Amplitude
	
	notes = APPEND_NOTE(notes, FS4, 0.5, 100);
	
	// Write notes to file
	WRITE_NOTES(swave, notes);
	
	// IMPORTANT:
	WRITE_HDR(swave);								// Write the WAVE header
	swave = DELETE_PCMWAVE(swave);	// Deallocate memory
	CLEAR_NOTE_LIST(notes);
	
	return 0;
}


