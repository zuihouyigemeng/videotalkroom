
/*
 * Copyright (c) 2d3D, Inc.
 * Written by Abraham vd Merwe <abraham@2d3d.co.za>
 * All rights reserved.
 *
 * $Id: flashcp.c,v 1.1.1.1 2006/08/30 03:16:23 ivan Exp $
 *
 * Renamed to flashcp.c to avoid conflicts with fcp from fsh package
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in the
 *	  documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of other contributors
 *	  may be used to endorse or promote products derived from this software
 *	  without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "mtd/mtd-user.h"
#include <getopt.h>

#define CommonH
#include "common.h"

typedef int bool;
#define true 1
#define false 0

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/* for debugging purposes only */
#ifdef DEBUG
#undef DEBUG
#define DEBUG(fmt,args...) { log_printf (LOG_ERROR,"%d: ",__LINE__); log_printf (LOG_ERROR,fmt,## args); }
#else
#undef DEBUG
#define DEBUG(fmt,args...)
#endif

#define KB(x) ((x) / 1024)
#define PERCENTAGE(x,total) (((x) * 100) / (total))

/* size of read/write buffer */
#define BUFSIZE (10 * 1024)

/* cmd-line flags */
#define FLAG_NONE		0x00
#define FLAG_VERBOSE	0x01
#define FLAG_HELP		0x02
#define FLAG_FILENAME	0x04
#define FLAG_DEVICE		0x08

/* error levels */
#define LOG_NORMAL	1
#define LOG_ERROR	2
void SendUdpOne(unsigned char Order, unsigned char SonOrder, unsigned char PerCent, char *cFromIP);
//---------------------------------------------------------------------------
static void log_printf (int level,const char *fmt, ...)
{
   FILE *fp = level == LOG_NORMAL ? stdout : stderr;
   va_list ap;
   va_start (ap,fmt);
   vfprintf (fp,fmt,ap);
   va_end (ap);
   fflush (fp);
}
//---------------------------------------------------------------------------
static int safe_open (const char *pathname,int flags)
{
   int fd;

   fd = open (pathname,flags);
   if (fd < 0)
	 {
		log_printf (LOG_ERROR,"While trying to open %s",pathname);
		if (flags & O_RDWR)
		  log_printf (LOG_ERROR," for read/write access");
		else if (flags & O_RDONLY)
		  log_printf (LOG_ERROR," for read access");
		else if (flags & O_WRONLY)
		  log_printf (LOG_ERROR," for write access");
		log_printf (LOG_ERROR,": %m\n");
		return -1;
	 }

   return (fd);
}
//---------------------------------------------------------------------------
static void safe_read (int fd,const char *filename,void *buf,size_t count,bool verbose)
{
   ssize_t result;

   result = read (fd,buf,count);
   if (count != result)
	 {
		if (verbose) log_printf (LOG_NORMAL,"\n");
		if (result < 0)
		  {
			 log_printf (LOG_ERROR,"While reading data from %s: %m\n",filename);
			 return;
		  }
		log_printf (LOG_ERROR,"Short read count returned while reading from %s\n",filename);
		return;
	 }
}
//---------------------------------------------------------------------------
static void safe_rewind (int fd,const char *filename)
{
   if (lseek (fd,0L,SEEK_SET) < 0)
	 {
		log_printf (LOG_ERROR,"While seeking to start of %s: %m\n",filename);
		return;
	 }
}
//---------------------------------------------------------------------------
static int dev_fd = -1;

static void cleanup (void)
{
   if (dev_fd > 0) close (dev_fd);
}
//---------------------------------------------------------------------------
//imagebuff  Ó³Ïñ»º´æ¡¡¡¡imagelen  Ó³Ïñ´óÐ¡¡¡
int flashcp(unsigned char *imagebuff, int imagelen, char *cFromIP)
{
   char device[20] = "/dev/mtd0";
   int i,flags = FLAG_NONE;
   size_t result,size,written;
   struct mtd_info_user mtd;
   struct erase_info_user erase;
   unsigned char src[BUFSIZE],dest[BUFSIZE];
   char FromIP[20];
   char systemtext[50];

   strcpy(FromIP, cFromIP);
   flags |= FLAG_VERBOSE;

   atexit (cleanup);

   // get some info about the flash device
   dev_fd = safe_open (device,O_SYNC | O_RDWR);
   if (ioctl (dev_fd,MEMGETINFO,&mtd) < 0)
	 {
		DEBUG("ioctl(): %m\n");
		log_printf (LOG_ERROR,"This doesn't seem to be a valid MTD flash device!\n");
                return -1;
	 }

   // does it fit into the device/partition?
   if (imagelen > mtd.size)
	 {
		log_printf (LOG_ERROR,"imagebuff won't fit into %s!\n", device);
		return -1;
	 }

   //
   //	* erase enough blocks so that we can write the file *
   erase.start = 0;
   erase.length = imagelen & ~(mtd.erasesize - 1);
   if (imagelen % mtd.erasesize) erase.length += mtd.erasesize;
   if (flags & FLAG_VERBOSE)
	 {
		// if the user wants verbose output, erase 1 block at a time and show him/her what's going on
		int blocks = erase.length / mtd.erasesize;
		erase.length = mtd.erasesize;
		log_printf (LOG_NORMAL,"Erasing blocks: 0/%d (0%%), erase.length = %d, mtd.erasesize = %d",blocks, erase.length, mtd.erasesize);
		for (i = 1; i <= blocks; i++)
		  {
			 log_printf (LOG_NORMAL,"\rErasing blocks: %d/%d (%d%%)",i,blocks, PERCENTAGE (i,blocks));
                         if((PERCENTAGE (i,blocks) % 5) == 0)
                           SendUdpOne(DOWNLOADIMAGE, ERASEFLASH, PERCENTAGE (i,blocks), FromIP); //ÕýÔÚÉ¾³ýFlash
			 if (ioctl (dev_fd,MEMERASE,&erase) < 0)
			   {
				  log_printf (LOG_NORMAL,"\n");
				  log_printf (LOG_ERROR,
						   "While erasing blocks 0x%.8x-0x%.8x on %s: %m\n",
						   (unsigned int) erase.start,(unsigned int) (erase.start + erase.length),device);
                                  SendUdpOne(DOWNLOADIMAGE, ERRORFLASH, 0, FromIP);  //Ê§°Ü
				  return -1;
			   }
			 erase.start += mtd.erasesize;
		  }
		log_printf (LOG_NORMAL,"\rErasing blocks: %d/%d (100%%)\n",blocks,blocks);
	 }
   else
	 {
		// if not, erase the whole chunk in one shot
		if (ioctl (dev_fd,MEMERASE,&erase) < 0)
		  {
				  log_printf (LOG_ERROR,
						   "While erasing blocks from 0x%.8x-0x%.8x on %s: %m\n",
						   (unsigned int) erase.start,(unsigned int) (erase.start + erase.length),device);
			 return -1;
		  }
	 }
   DEBUG("Erased %u / %luk bytes\n", erase.length, imagelen);

   //**********************************
   //	* write the entire file to flash *
   if (flags & FLAG_VERBOSE)
     log_printf (LOG_NORMAL,"Writing data: 0k/%luk (0%%)",KB (imagelen));
   size = imagelen;
   i = BUFSIZE;
   written = 0;
   while (size)
	 {
		if (size < BUFSIZE) i = size;
		if (flags & FLAG_VERBOSE)
		  log_printf (LOG_NORMAL,"\rWriting data: %dk/%luk (%lu%%)",
				  KB (written + i),
				  KB (imagelen),
				  PERCENTAGE (written + i, imagelen));
                if((PERCENTAGE (written + i, imagelen) % 5) == 0)
                  SendUdpOne(DOWNLOADIMAGE, WRITEFLASH, PERCENTAGE (written + i, imagelen), FromIP); //ÕýÔÚÐ´Flash

		// read from buff
                memcpy(src, imagebuff + written, i);

		// write to device
		result = write (dev_fd, src, i);
		if (i != result)
		  {
			 if (flags & FLAG_VERBOSE) log_printf (LOG_NORMAL,"\n");
			 if (result < 0)
			   {
				  log_printf (LOG_ERROR,
						   "While writing data to 0x%.8x-0x%.8x on %s: %m\n",
						   written,written + i,device);
                                  SendUdpOne(DOWNLOADIMAGE, ERRORFLASH, 0, FromIP);  //Ê§°Ü
				  return -1;
			   }
			 log_printf (LOG_ERROR,
					  "Short write count returned while writing to x%.8x-0x%.8x on %s: %d/%lu bytes written to flash\n",
					  written,written + i,device,written + result,imagelen);
                         SendUdpOne(DOWNLOADIMAGE, ERRORFLASH, 0, FromIP);  //Ê§°Ü
			 return -1;
		  }

		written += i;
		size -= i;
	 }
   if (flags & FLAG_VERBOSE)
	 log_printf (LOG_NORMAL,
				 "\rWriting data: %luk/%luk (100%%)\n",
				 KB (imagelen),
				 KB (imagelen));
   DEBUG("Wrote %d / %luk bytes\n",written,imagelen);

   //**********************************
   //	* verify that flash == file data *

   safe_rewind (dev_fd, device);
   size = imagelen;
   i = BUFSIZE;
   written = 0;
   if (flags & FLAG_VERBOSE) log_printf (LOG_NORMAL,"Verifying data: 0k/%luk (0%%)",KB (imagelen));
   while (size)
	 {
		if (size < BUFSIZE) i = size;
		if (flags & FLAG_VERBOSE)
		  log_printf (LOG_NORMAL,
					  "\rVerifying data: %dk/%luk (%lu%%)",
					  KB (written + i),
					  KB (imagelen),
					  PERCENTAGE (written + i,imagelen));
                if((PERCENTAGE (written + i, imagelen) % 5) == 0)
                  SendUdpOne(DOWNLOADIMAGE, CHECKFLASH, PERCENTAGE (written + i, imagelen), FromIP); //ÕýÔÚÐ´Flash

		// read from filename
                memcpy(src, imagebuff + written, i);

		// read from device
		safe_read (dev_fd,device,dest,i,flags & FLAG_VERBOSE);

		// compare buffers
		if (memcmp (src,dest,i))
		  {
			 log_printf (LOG_ERROR,
					  "File does not seem to match flash data. First mismatch at 0x%.8x-0x%.8x\n",
					  written,written + i);
                         SendUdpOne(DOWNLOADIMAGE, ERRORFLASH, 0, FromIP);  //Ê§°Ü
			 return -1;
		  }

		written += i;
		size -= i;
	 }
   if (flags & FLAG_VERBOSE)
	 log_printf (LOG_NORMAL,
				 "\rVerifying data: %luk/%luk (100%%)\n",
				 KB (imagelen),
				 KB (imagelen));
   DEBUG("Verified %d / %luk bytes\n",written,imagelen);
   SendUdpOne(DOWNLOADIMAGE, ENDFLASH, 0, FromIP);  //Íê³ÉÐ´Image

   sprintf(systemtext, "rm %s\0", mtdexe_name);
   system(systemtext);
   sync();
   system("reboot");

   return 1;
}
//---------------------------------------------------------------------------
void SendUdpOne(unsigned char Order, unsigned char SonOrder, unsigned char PerCent, char *cFromIP)
{
	unsigned char send_b[1500];
	int sendlength;
	memcpy(send_b, UdpPackageHead, 6);
	send_b[6] = Order;
	send_b[7] = ASK;    //Ö÷½Ð
	send_b[8] = SonOrder;    //×ÓÃüÁî
	memcpy(send_b + 9, FLAGTEXT, 20);       //±êÖ¾
	send_b[29] = PerCent;
	sendlength = 30;
	UdpSendBuff(m_DataSocket, cFromIP, send_b , sendlength);
}
//---------------------------------------------------------------------------
                        
