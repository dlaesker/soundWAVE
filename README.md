# soundWAVE
soundWAVE is a small, simplistic, C library for pure tone sine wave creation encoded in WAVE format. This is an ongoing project of mine so perhaps a lot more is possible :) 

## How to use soundWAVE
The following code snippet serves as a template for the examples that follow:
```
#include "soundwave.h"	// Include the soundWAVE header file
int main(){
// Create an instance of a SOUNDWAVE abstraction
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
// More code goes here...
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
	100); // Volume/Amplitude (0 - 100)
...
}
```

### Example 2: Writing multiple notes. 
There are various ways to achieve this. One may simply make successive calls to "WRITE_NOTE" as:
```
write_notes1.c
...
WRITE_NOTE(swave, A4, 1.0, 100); 
WRITE_NOTE(swave, B4, 1.0, 100);
WRITE_NOTE(swave, C4, 1.0, 100); 
WRITE_NOTE(swave, CS4, 1.0, 100); // S for sharp '#'
...
```
Now, this will eventually get cumbersome if we want to write a large series of notes. Therefore, a better way could be to use a loop:
```
write_notes2.c
...

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
...
```
Both previously presented code snippets deal with static implementations. However, what if we wanted to let a user choose notes at runtime? We can use a NOTEARR or NOTES structure to do so. 
```
write_notes3.c
...
NOTES *notes = NULL;

// Singly-linked-list
NOTES *notes = NULL;
notes = APPEND_NOTE(notes,
	C4,		// Note
	0.5,	// Duration
	100);	// Amplitude

notes = APPEND_NOTE(notes, FS4, 0.5, 2);

// Write notes to file
WRITE_NOTES(swave, notes);

// Must also deallocate memory for the list of notes
CLEAR_NOTE_LIST(notes);
...
```
### Example 3: Stereo. 

To write to two channels requires that two lists of notes (one for each channel) be passed to the WRITE_NOTES_STEREO function.  For example:

```
stereo.c
...
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
...
```
### Tritone Paradox
Here I use the soundWAVE library to replicate the sound used in the famous experiment by Diana Deutsch. You can read more about it [here](http://deutsch.ucsd.edu/psychology/pages.php?i=206). Do you hear an ascending or descending pattern? 

```
tritone_paradox.c
...
NOTE tritone_notes[] = {
	C4, FS4, CS4, G4, D4, GS4, DS4, A4,
	E4, AS4, F4, B4, FS4, C4, G4, CS4,
	GS4, D4, A4, DS4, AS4, E4, B4, F4
};

float t[] = {
	.5,.5,.5,.5,.5,.5,.5,.5,
	.5,.5,.5,.5,.5,.5,.5,.5,
	.5,.5,.5,.5,.5,.5,.5,.5
};
	

NOTES *LNOTES = NULL, *RNOTES = NULL;
int i;
for(i = 0; i < 24; ++i){
	if(i % 2 == 0 && i != 0)
		WRITE_NOTE(s, PAUSE, 0.5, 0);
	WRITE_NOTE(s, tritone_notes[i], 0.5, 100);
	
}
	
WRITE_NOTES(s, LNOTES);
...
```




