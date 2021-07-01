#include "soundwave.h"

/* ######################### *
	 # Author: Denis Laesker #
*  ######################### */

int main(){
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
	
	/* Writes A440 to the file. Notes are in MIDI format from 0 (C0) to 108 (B8). */
	WRITE_NOTE(swave,
		A4, 	// Note to write
		1.0, 	// Duration
		100); // Volume (0 - 100)
	
	// IMPORTANT:
	WRITE_HDR(swave);
	swave = DELETE_PCMWAVE(swave);
	
	return 0;
}
