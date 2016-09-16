
/*
 * g711.h
 *
 * u-law, A-law and linear PCM conversions.
 */

#ifndef G711_H
#define G711_H

extern void G711Encoder(short *pcm,unsigned char *code,int size,int lawflag);
extern void G711Decoder(short *pcm,unsigned char *code,int size,int lawflag);


#endif /* G711_H */
