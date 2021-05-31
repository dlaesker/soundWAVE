#ifndef SOUNDWAVE_H
#define SOUNDWAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#ifndef M_PI // Defined in math.h
#define M_PI 3.14159265358979323846264338327950288
#endif

#define CHMONO   0x1
#define CHSTEREO 0x2

#define SEMITONE_RATIO (pow(2, 1.0/12.0))           // Twelth root of 2
#define A4F 440.0                                   // Equal Temperament A440
#define C4F (A4F*pow(SEMITONE_RATIO, -9))           // 9 semitones below A4
#define C0F C4F*(pow(0.5, 4))                       // 4 octaves below C4

#define CDSRATE 0xAC44                              // CD Standard Sampling Rate
#define GETSAMPLE(wt)(sin(wt/CDSRATE))              // Where wt = 2*pi*t
#define PAUSE 0

/* Rather than having to remember the frequency of every note, one may use the name of the note instead, where 'S' stands for Sharp (#)... */

typedef enum {
	C0, CS0, D0, DS0, E0, F0, FS0, G0, GS0, A0, AS0, B0,
	C1, CS1, D1, DS1, E1, F1, FS1, G1, GS1, A1, AS1, B1,
	C2, CS2, D2, DS2, E2, F2, FS2, G2, GS2, A2, AS2, B2,
	C3, CS3, D3, DS3, E3, F3, FS3, G3, GS3, A3, AS3, B3,
	C4, CS4, D4, DS4, E4, F4, FS4, G4, GS4, A4, AS4, B4,
	C5, CS5, D5, DS5, E5, F5, FS5, G5, GS5, A5, AS5, B5,
	C6, CS6, D6, DS6, E6, F6, FS6, G6, GS6, A6, AS6, B6,
	C7, CS7, D7, DS7, E7, F7, FS7, G7, GS7, A7, AS7, B7,
	C8, CS8, D8, DS8, E8, F8, FS8, G8, GS8, A8, AS8, B8
} NOTE;

/* ...then make a call to NOTEF(note) to retrieve the note's corresponding frequency. For example, NOTEF(A4) = 440.0 Hz (Equal Temperament). One may modify this to employ a different temperament. */

#define NOTEF(S)(C0F*pow(SEMITONE_RATIO,S))

/* Singly-linked-list abstraction to aid in writing various notes to a file, each with its own frequency, amplitude, and duration. */
typedef struct NOTEARR {
	NOTE    note;
	float   amp;
	float   dur;
	float   *samples;
	struct  NOTEARR *next;
} NOTES;

typedef unsigned int   DWORD;  // Unsgined 32 bits
typedef unsigned short WORD;   // Unsgined 16 bits
typedef signed short   SWORD;  // Signed   16 bits
typedef unsigned char  BYTE;   // Unsgined  8 bits
 
typedef DWORD CKSIZE;          // 32-bit unsigned size value
typedef BYTE ERRCODE;

/* WAVE Audio Formats
        Name    Value       Format Category */
#define PCM     0x0001      // Microsoft Pulse Code Modulation (PCM) 	[Supported]
#define MULAW   0x0101      // IBM mu-law															[Not Supported]
#define ALAW    0x0102      // IBM a-law															[Not Supported]
#define AVC     0x0103      // IBM AVC																[Not Supported]

/* FORMAT chunk */
/* Since we are writing a WAVE file, there is only need to group and keep track of those values yet to be determined. That is, we know, for example, that values such as "RIFF" and "WAVE" will have to be written to the file. */
typedef struct FORMAT {
	BYTE    ID[4 + 1];
	CKSIZE  SZ;
	WORD    wAudioFormat;   // Format Category
	WORD    wChannels;      // 1 = mono, 2 = stereo. Macros have been defined.
	DWORD   dwSampleRate;   // Sampling rate. A macro for CD standard has been defined.
	DWORD   dwByteRate;     // For buffer estimation
	WORD    wBlockAlign;    // Data block size (in bytes). I believe this is also known as sample width.
													// Refers to the size of data.
	WORD    wBitsPerSample; // For PCM. Specifies the number of bits of data used to represent ea. sample in ea. channel.
} FMT;

typedef FILE WAVEFILE;		// Not necessary. Just a naming convention.

/*
 Requires some explanation. Assuming that we may have samples represented by either 8, 16, or 32 bits, the range of possible values changes according to the sample width (Block Align in our case). That is, if ea. sample is 16 bits (signed 2's complement) long, then the range of all possible values is (2^(16-1), 2^(16-1) - 1) or (-32,768, 32,767). This is also true for 32 bits, at a much larger range of values of course: (-2,147,483,648, 2,147,483,647). With 8 bits, however, we are now dealing with only positive (unsigned) values. Thus, our range becomes 0 to 2^(8) - 1 (-1 because 0 - 256 is 2^8 possible values).
 
 Now, all of this does not really represent a problem. That is, we could handle ea. case separately. However, consider the following when calculating the max amplitude. BlockAlign provides us with the data block size (in bytes). In other words, 1, 2, and 4 byte/s for 8, 16, and 32 bits, respectively. Therefore, if we wanted to calculate the max amplitude for ea. sample width, we could do so as (2^(bits*BlockAlign-1)-1). However, there is a slight problem with this equation. For 8 bits, our result is (2^((8*1)-1) - 1) = 127. Nevertheless, due to the fact that 8 bit-value samples range from 0 - 255 (remember, we are dealing with unsigned values), we need to add an offset to the resulting max amplitude. I do so by defining an offset array, which is later indexed using the wBlockAlign member of the FMT struct.
 */

#define MAX_AMP(swidth)((pow(2,8*swidth-1)) - 1)    //Where swidth = Sample Width (Block Align).
static const BYTE SAMPLE_OFFSET[] = {0, 127, 0, 0, 0};

/*
 The SOUNDWAVE abstraction helps keeping important information together in case it needs to referenced at a later time.
 
 When declaring a SOUNDWAVE variable, call INIT_PCMWAVE in order to initialize it. The function allocates the necessary memory, performs error checking, and finally (if everything goes well) returns a pointer to the newly allocated data structure.
 
 You are responsible for deallocating any previously allocated memory. Therefore, make a call to DELETE_PCMWAVE! Maybe I'll make it so that you don't have to worry about it at some later time. For now, clean your stuff up!
 
 You may write as many notes as you'd like to the file. However, you must make a call to WRITE_HDR before quiting the program. Since in most cases, there is no way to know how much information will be written to the file, the SOUNDWAVE instantiation keeps track of how much data has already been written. A call to WRITE_HDR then takes that into account.
 */

typedef struct {
    WAVEFILE    *fp;
    DWORD       DATASZ;
    FMT         FORMAT;
} SOUNDWAVE;

ERRCODE isValidBitsPerSample(WORD *bitsPerSample);
ERRCODE isValidNumberOfChannels(WORD *nChannels);
ERRCODE isValidNote(NOTE *note);
SOUNDWAVE *INIT_PCMWAVE(char *filename, WORD bitsPerSample, WORD nChannels, DWORD srate);
SOUNDWAVE *DELETE_PCMWAVE(SOUNDWAVE *S);
void MAKE_NOTE(NOTES *new_note, NOTE *note, float *dur, float *amp);
NOTES *APPEND_NOTE(NOTES *list, NOTE note, float dur, float amp);
NOTES *CLEAR_NOTE_LIST(NOTES *list);
int WRITE_NOTE(SOUNDWAVE *S, NOTE note, float dur);
int WRITE_NOTES(SOUNDWAVE *S, NOTES *notes);
int WRITE_NOTES_STEREO(SOUNDWAVE *S, NOTES *L_CH, NOTES *R_CH);
int WRITE_HDR(SOUNDWAVE *S);
void SOUNDWAVE_INFO(SOUNDWAVE *S);

#endif
