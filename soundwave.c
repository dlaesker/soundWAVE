/*  ###############################################  *
    # Author: Denis Laesker                       #
    # Description: This file implements a simple  #
    # mechanism to write pure tones to WAVE files #
 *  ###############################################  */

#include "soundwave.h"

ERRCODE isValidBitsPerSample(WORD *bitsPerSample){
    //May be used for all multiples of 8. Maybe dangerous to allow this mechanism? E.g., 256 bits = 32 bytes. If the sampling rate is 44,100, then we have ~1400 Mb for a second of audio. I am also not sure what happens when values that are not multiples of 8 are used. 
    //return ((bitsPerSample % 8) == 0) ? 1 : 0;
    if((*bitsPerSample != 8)     &&
       (*bitsPerSample != 16)    &&
       (*bitsPerSample != 24)){
        return 0;
    }
    return 1;
}

ERRCODE isValidNumberOfChannels(WORD *nChannels){
    return ((*nChannels < 1) || (*nChannels > 2)) ? 0 : 1;
}

/* For now, I will treat ea. Audio Format separately. */
SOUNDWAVE *INIT_PCMWAVE(char *filename, WORD bitsPerSample, WORD nChannels, DWORD srate){
     if(!isValidBitsPerSample(&bitsPerSample)) return NULL;
     if(!isValidNumberOfChannels(&nChannels)) return NULL;
     
     WAVEFILE *fp = fopen(filename, "wb");
     if(fp == NULL){
         perror("Could not create file!");
         return NULL;
     }
    
    SOUNDWAVE *new_wave = (SOUNDWAVE *)malloc(sizeof(SOUNDWAVE));
    if(new_wave == NULL){
        perror("Malloc failed!");
        return NULL;
    }
    
    new_wave->fp                    = fp;                   // Keep a pointer to the file
    new_wave->DATASZ                = 0;                    // Initially, no data has been written to the file
    new_wave->FORMAT.wAudioFormat   = PCM;
    new_wave->FORMAT.wChannels      = nChannels;
    new_wave->FORMAT.dwSampleRate   = srate;
    new_wave->FORMAT.dwByteRate     = srate;
    new_wave->FORMAT.wBlockAlign    = bitsPerSample / 8;    // Determine sample width (in bytes)
    new_wave->FORMAT.wBitsPerSample = bitsPerSample;
    
    return new_wave; // If everything goes well :)
}

SOUNDWAVE *DELETE_PCMWAVE(SOUNDWAVE *S){
    if(S != NULL){
				if(S->fp != NULL) fclose(S->fp);
        free(S);
    }
    return NULL;
}

/* Helper function to assign a note, duration, and amplitude to a Note in the list of Notes */
void MAKE_NOTE(NOTES *new_note, NOTE *note, float *dur, float *amp){
    new_note->note  = *note;
    new_note->dur   = *dur;
    new_note->amp   = *amp;
}

/* Typical singly-linked list append function. */
NOTES *APPEND_NOTE(NOTES *list, NOTE note, float dur, float amp){
    NOTES *new_note = (NOTES *)malloc(sizeof(NOTES));
    if(new_note == NULL){
        perror("Malloc failed!");
        return NULL;
    }
    
    MAKE_NOTE(new_note, &note, &dur, &amp);
    new_note->next = NULL;
    
    if(list == NULL) return new_note;
    
    NOTES* cur = list;
    for(; cur->next != NULL; cur = cur->next)
        ;
    
    cur->next = new_note;
    return list;
}

/* Clears previously allocated memory to a list of NOTES. If using this datatype, user is responsible for freeing up space. */
NOTES *CLEAR_NOTE_LIST(NOTES *list){
    NOTES  *hold = NULL;
    while(list != NULL){
        hold = list;
        list = list->next;
        if(hold) free(hold); // Condition is not really necessary, but one can never be too sure :)
    }
    return NULL;
}

ERRCODE isValidNote(NOTE *note){ return (*note < C0 || *note > B8) ? 0 : 1;}
/* Writes a single NOTE (and its duration) to the file */
int WRITE_NOTE(SOUNDWAVE *S, NOTE note, float dur, float amp){
	if(S == NULL){
			perror("Soundwave is NULL!");
			return -1;
	}

	if(!isValidNote(&note)){
			perror("Not a valid note!");
			return -1;
	}

	// If the first call to this function, we want to start writing the data (samples) only after the memory reserved for the header.
	if(ftell(S->fp) == 0){
			fpos_t pos;
			fgetpos(S->fp, &pos);

			pos = pos + 44;
			fsetpos(S->fp, &pos);
	}
			
	WORD smp_width = S->FORMAT.wBlockAlign;

	int Amp = (amp / 100) * MAX_AMP(smp_width);

	float freq = NOTEF(note);

	WORD buff_size = smp_width;
	BYTE buff[buff_size];

	int t, i, j;
	for(t = 0; t < S->FORMAT.dwSampleRate * dur; ++t){
		DWORD s = SAMPLE_OFFSET[smp_width]+Amp*GETSAMPLE(2*M_PI*freq*t);
		for(j = 0; j < buff_size; ++j) buff[j] = (s >> (8*j)) & 0xFF;

		fwrite(buff, 1, buff_size, S->fp);
	}
			

	S->DATASZ += (S->FORMAT.dwSampleRate * smp_width * dur);
	return 1;
}

/* Write a collection of notes, each with its own frequency, duration, and amplitude */
int WRITE_NOTES(SOUNDWAVE *S, NOTES *notes){
    NOTES *cur = notes;
    for(; cur != NULL; cur = cur->next)
        if(WRITE_NOTE(S, cur->note, cur->dur, cur->amp) < 0) return -1;
    
	return 1;
}

/* Must work on a way to avoid the use of separate functions for MONO and STEREO audio. However, for now this will have to do. */
int WRITE_NOTES_STEREO(SOUNDWAVE *S, NOTES *L_CH, NOTES *R_CH){
	NOTES *lch = L_CH, *rch = R_CH, *l_last = L_CH, *r_last = R_CH;
	
	float dur = 0.5;
	
	WORD smp_width = S->FORMAT.wBlockAlign;
	BYTE lbuff[smp_width], rbuff[smp_width];
	DWORD lsmp, rsmp;
	
	int count = 0;
	
	float Amp = MAX_AMP(smp_width);
	
	/* Figure out:
			(1) How to write channels of different durations;
			(2) how to optimize this code */
	int flag = 1;
	while(lch || rch){
		count += 1;
		if(lch && rch){
				float lfrq = NOTEF(l_last->note);
				float rfrq = NOTEF(r_last->note);
				
				int t, j;
				for(t = 0; t < S->FORMAT.dwSampleRate * dur; ++t){
						lsmp = SAMPLE_OFFSET[smp_width]+Amp*GETSAMPLE(2*M_PI*lfrq*t);
						if(flag){
//								printf("%u\n", lsmp);
								flag = 0;
						}
						rsmp = SAMPLE_OFFSET[smp_width]+Amp*GETSAMPLE(2*M_PI*rfrq*t);
						
						for(j = 0; j < smp_width; ++j) lbuff[j] = (lsmp >> (8*j)) & 0xFF;
						for(j = 0; j < smp_width; ++j) rbuff[j] = (rsmp >> (8*j)) & 0xFF;
								
						fwrite(lbuff, 1, smp_width, S->fp);
						fwrite(rbuff, 1, smp_width, S->fp);
				}
		}
		
		if(lch) {
				l_last  = lch;
				lch     = lch->next;
		}else WRITE_NOTE(S, l_last->note, l_last->dur, l_last->amp);
		
		if(rch) {
				WRITE_NOTE(S, rch->note, rch->dur, rch->amp); // Write right channel
				r_last  = rch;
				rch     = rch->next;
		}else WRITE_NOTE(S, r_last->note, r_last->dur, r_last->amp);
	}
	
	S->DATASZ += (S->FORMAT.dwSampleRate * smp_width * dur * 2 * count);
	return 1;
}

int WRITE_HDR(SOUNDWAVE *S){
	if(S == NULL) return -1;
	
	rewind(S->fp);
	
	BYTE header[44];
	
	DWORD data_sz = S->DATASZ + 44;
			
	header[0] = 0x52; // 'R'
	header[1] = 0x49; // 'I'
	header[2] = 0x46; // 'F'
	header[3] = 0x46; // 'F'
	
	int i;
	for(i = 0; i < 4; ++i)
			header[4 + i] = (data_sz >> (8*i)) & 0xFF;
	
	header[8]   = 0x57; // 'W'
	header[9]   = 0x41; // 'A'
	header[10]  = 0x56; // 'V'
	header[11]  = 0x45; // 'E'
	
	header[12]  = 0x66; // 'f'
	header[13]  = 0x6D; // 'm'
	header[14]  = 0x74; // 't'
	header[15]  = 0x20; // ' '
	
	// Size of the 'fmt ' chunk
	header[16]  = 0x10;
	header[17]  = 0x00;
	header[18]  = 0x00;
	header[19]  = 0x00;
	
	for(i = 0; i < 2; ++i)
			header[20 + i] = (PCM >> (8*i)) & 0xFF;
	
	for(i = 0; i < 2; ++i)
			header[22 + i] = (S->FORMAT.wChannels >> (8*i)) & 0xFF;
	
	for(i = 0; i < 4; ++i)
			header[24 + i] = (S->FORMAT.dwSampleRate >> (8*i)) & 0xFF;
	
	for(i = 0; i < 4; ++i)
			header[28 + i] = (S->FORMAT.dwByteRate >> (8*i)) & 0xFF;
	
	for(i = 0; i < 2; ++i)
			header[32 + i] = (S->FORMAT.wBlockAlign >> (8*i)) & 0xFF;
	
	for(i = 0; i < 2; ++i)
			header[34 + i] = (S->FORMAT.wBitsPerSample >> (8*i)) & 0xFF;
	
	header[36]  = 0x64; // 'd'
	header[37]  = 0x61; // 'a'
	header[38]  = 0x74; // 't'
	header[39]  = 0x61; // 'a'
	
	for(i = 0; i < 4; ++i)
			header[40 + i] = (data_sz >> (8*i)) & 0xFF;
	
	fwrite(header, 1, 44, S->fp);
	return 1;
}

/* Prints out information about the S data structure. */
void SOUNDWAVE_INFO(SOUNDWAVE *S){
	if(S){
		printf("AUDIO FORMAT: %u\n", 		S->FORMAT.wAudioFormat);
		printf("CHANNELS: %u\n", 				S->FORMAT.wChannels);
		printf("BLOCK ALIGN: %u\n", 		S->FORMAT.wBlockAlign);
		printf("BITS PER SAMPLE: %u\n", S->FORMAT.wBitsPerSample);
		printf("SAMPLE RATE: %u\n", 		S->FORMAT.dwSampleRate);
		printf("BYTE RATE: %u\n", 			S->FORMAT.dwByteRate);
	}else printf("A problem occurred\n");
}
