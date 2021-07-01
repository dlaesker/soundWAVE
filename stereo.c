#include "soundwave.h"

/* ######################### *
	 # Author: Denis Laesker #
*  ######################### */

int main(){
	// Create an instance of
	SOUNDWAVE *swave = INIT_PCMWAVE(
		"sound.wav", 	// Filename. Make sure it does not already exist or it will be overwritten.
		16, 					// Bits per sample (must be a multiple of 8).
		CHSTEREO, 		// No. of channels. CHMONO = 1, CHSTEREO = 2
		CDSRATE);			// Sampling rate. CDSRATE = 44,100
	
	if(swave == NULL){
		printf("Something went wrong...\n");
		exit(EXIT_FAILURE);
	}
	
	// Create a list of notes for ea. channel
	NOTES *lnotes = NULL, *rnotes = NULL;
	lnotes = APPEND_NOTE(lnotes, C4, 1.0, 100);
	rnotes = APPEND_NOTE(rnotes, FS4, 1.0, 100);
	
	// Write notes to file
	WRITE_NOTES_STEREO(swave, lnotes, rnotes);
	
	// IMPORTANT:
	WRITE_HDR(swave);								// Write the WAVE header
	swave = DELETE_PCMWAVE(swave);	// Deallocate memory
	CLEAR_NOTE_LIST(lnotes);
	CLEAR_NOTE_LIST(rnotes);
	
	return 0;
}



