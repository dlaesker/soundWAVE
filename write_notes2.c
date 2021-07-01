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
	
	// Declare a sequence of notes
	NOTE note_sequence[] = {
		F4, PAUSE, F4, PAUSE, DS4, PAUSE, GS4,
		AS4, C4, AS4, DS4, PAUSE, DS4, PAUSE, CS4,
		C4, AS3, PAUSE, GS3, AS3, CS4, AS3, CS4,
		DS4, PAUSE, C4, AS3, GS3, GS3, PAUSE, DS4, CS4
	};
	
	// Declare the durations for each note
	float note_duration[] = {
		.4, .2, .4, .2, .8, .4, .2, .2,
		.2, .2, .4, .2, .4, .2, .4, .2,
		.4, .2, .2, .2, .2, .2, .6, .4,
		.2, .2, .6, .4, .6, .2, .8, .8
	};
	
	// Finally, write them to the file
	int i;
	for(i = 0; i < 32; ++i){
		WRITE_NOTE(swave, note_sequence[i], note_duration[i], 100);
	}

	// IMPORTANT:
	WRITE_HDR(swave);								// Write the WAVE header
	swave = DELETE_PCMWAVE(swave);	// Deallocate memory
	
	return 0;
}


