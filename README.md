# soundWAVE
soundWAVE is a small, simplistic, C library for pure tone sine wave creation encoded in WAVE format. This is an ongoing project of mine so perhaps a lot more is possible :) 

## How to use soundWAVE
The following code snippet serves as a template for the examples that follow:
```
#include "soundwave.h"	// Include the soundWAVE header file
int main(){
	// Create an instance of 
	SOUNDWAVE *swave = INIT_PCMWAVE(
		"sound.wav", 	// Filename. Make sure it does not already exist or it will be overwritten.  	
		24, 					// Bits per sample (must be a multiple of 8).
		CHMONO, 			// No. of channels. CHMONO = 1, CHSTEREO = 2
		CDSRATE);			// Sampling rate. CDSRATE = 44,100
	if(swave == NULL){
		printf("Something went wrong...\n");
		exit(EXIT_FAILURE);
	}
	
	// *****************
	// Code goes here...
	// *****************
	
	// IMPORTANT:
	WRITE_HDR(swave); 							// Write the WAVE header
	swave = DELETE_PCMWAVE(swave);	// Deallocate memory
	return 0;
}
```
Suppose the code above is contained within a file by the name of sound.c. Then it may be compiled as:
```
gcc sound.c soundwave.c -o MAKE_SOUND
./MAKE_SOUND
```
Voila!

### Example 1: Writing a single note. 
The code snippet below demonstrates how to write a single note to a WAVE file using the soundWAVE library. 
```
...
	// Writes A440 to the file. Notes are in MIDI format from 0 (C0) to 108 (B8). 
	WRITE_NOTE(swave, 
		A4, 	// Note to write
		1.0, 	// Duration
		100); // Volume (0 - 100)
...
}
```

### Example 2: Writing multiple notes. 
There are various ways to achieve this. One may simply make successive calls to "WRITE_NOTE" as:
```
...
WRITE_NOTE(swave, A4, 1.0, 100); 
...
```


