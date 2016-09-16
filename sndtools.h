#ifndef SNDTOOLS_H
#define SNDTOOLS_H

#include <linux/soundcard.h>

#define AUDIODSP  1
#define AUDIODSP1 2

#define FMT8BITS 8 
#define FMT16BITS 16

#define FMT8K 8000
#define FMT16K 16000
#define FMT22K 22000
#define FMT44K 44000

#define MONO 0
#define STERO 1

#define AUDIORECBLK  512
#define AUDIOPLAYBLK  512

//#define AUDIOBLK 512   //ÿ֡32ms
#define AUDIOBLK 128   //ÿ֡8ms
#define AFRAMETIME AUDIOBLK/16   //ÿ֡ms

//Open sound device, return 1 if open success
//else return 0
int OpenSnd(int nWhich);     //1 record  2 play

//Close sound device
int CloseSnd(int nWhich);

//Set record or playback format, return 1 if success
//else return 0
int SetFormat(int nWhich, int bits, int hz, int chn);

//Record
int Record(char *buf, int size);

//Playback
int Play(char *buf, int size);

#endif //ifndef SNDTOOLS_H
