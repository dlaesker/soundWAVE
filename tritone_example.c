#include "soundwave.h"

/* ######################### *
	 # Author: Denis Laesker #
*  ######################### */

int main(){
	SOUNDWAVE *s = INIT_PCMWAVE("sound.wav", 16, CHMONO, CDSRATE);
		
	// http://deutsch.ucsd.edu/psychology/pages.php?i=206
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
		
	
	int i;
	for(i = 0; i < 24; ++i){
		if(i % 2 == 0 && i != 0)
			WRITE_NOTE(s, PAUSE, 0.5, 0);
		WRITE_NOTE(s, tritone_notes[i], 0.5, 100);
		
	}
    	
	WRITE_HDR(s);
	
	s = DELETE_PCMWAVE(s);
	return 0;
}
