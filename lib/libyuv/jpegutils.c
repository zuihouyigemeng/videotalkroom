/*
 *  jpegutils.c: Some Utility programs for dealing with
 *               JPEG encoded images
 *
 *  Copyright (C) 1999 Rainer Johanni <Rainer@Johanni.de>
 *  Copyright (C) 2001 pHilipp Zabel  <pzabel@gmx.de>
 *
 *  based on jdatasrc.c and jdatadst.c from the Independent
 *  JPEG Group's software by Thomas G. Lane
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <jerror.h>
#include <assert.h>
#include <inttypes.h>


#include "jpegutils.h"


 /*
 * jpeg_data:       buffer with input / output jpeg
 * len:             Length of jpeg buffer
 * itype:           0: Not interlaced
 *                  1: Interlaced, Top field first
 *                  2: Interlaced, Bottom field first
 * ctype            Chroma format for decompression.
 *                  Currently only Y4M_CHROMA_{420JPEG,422}  are available
 * raw0             buffer with input / output raw Y channel
 * raw1             buffer with input / output raw U/Cb channel
 * raw2             buffer with input / output raw V/Cr channel
 * width            width of Y channel (width of U/V is width/2)
 * height           height of Y channel (height of U/V is height/2)
 */


static void jpeg_buffer_src  (j_decompress_ptr cinfo, unsigned char *buffer,
                       long num);
static void jpeg_buffer_dest (j_compress_ptr cinfo,   unsigned char *buffer,
                       long len);
static void jpeg_skip_ff (j_decompress_ptr cinfo);

/*******************************************************************
 *                                                                 *
 *    The following routines define a JPEG Source manager which    *
 *    just reads from a given buffer (instead of a file as in      *
 *    the jpeg library)                                            *
 *                                                                 *
 *******************************************************************/


static void add_huff_table (j_decompress_ptr dinfo,
			    JHUFF_TBL **htblptr, 
			    const UINT8 *bits, const UINT8 *val)
/* Define a Huffman table */
{
  int nsymbols, len;

  if (*htblptr == NULL)
    *htblptr = jpeg_alloc_huff_table((j_common_ptr) dinfo);

  /* Copy the number-of-symbols-of-each-code-length counts */
  memcpy((*htblptr)->bits, bits, sizeof((*htblptr)->bits));

  /* Validate the counts.  We do this here mainly so we can copy the right
   * number of symbols from the val[] array, without risking marching off
   * the end of memory.  jchuff.c will do a more thorough test later.
   */
  nsymbols = 0;
  for (len = 1; len <= 16; len++)
    nsymbols += bits[len];
//  if (nsymbols < 1 || nsymbols > 256)
//    mjpeg_error_exit1("jpegutils.c:  add_huff_table failed badly. ");

  memcpy((*htblptr)->huffval, val, nsymbols * sizeof(UINT8));
}



static void std_huff_tables (j_decompress_ptr dinfo)
/* Set up the standard Huffman tables (cf. JPEG standard section K.3) */
/* IMPORTANT: these are only valid for 8-bit data precision! */
{
  static const UINT8 bits_dc_luminance[17] =
    { /* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
  static const UINT8 val_dc_luminance[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  
  static const UINT8 bits_dc_chrominance[17] =
    { /* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
  static const UINT8 val_dc_chrominance[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  
  static const UINT8 bits_ac_luminance[17] =
    { /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
  static const UINT8 val_ac_luminance[] =
    { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
      0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
      0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
      0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
      0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
      0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
      0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
      0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
      0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
      0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
      0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
      0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
      0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
      0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
      0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
      0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
      0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
      0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
      0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
      0xf9, 0xfa };
  
  static const UINT8 bits_ac_chrominance[17] =
    { /* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
  static const UINT8 val_ac_chrominance[] =
    { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
      0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
      0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
      0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
      0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
      0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
      0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
      0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
      0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
      0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
      0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
      0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
      0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
      0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
      0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
      0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
      0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
      0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
      0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
      0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
      0xf9, 0xfa };
  
  add_huff_table(dinfo, &dinfo->dc_huff_tbl_ptrs[0],
		 bits_dc_luminance, val_dc_luminance);
  add_huff_table(dinfo, &dinfo->ac_huff_tbl_ptrs[0],
		 bits_ac_luminance, val_ac_luminance);
  add_huff_table(dinfo, &dinfo->dc_huff_tbl_ptrs[1],
		 bits_dc_chrominance, val_dc_chrominance);
  add_huff_table(dinfo, &dinfo->ac_huff_tbl_ptrs[1],
		 bits_ac_chrominance, val_ac_chrominance);
}

static void guarantee_huff_tables(j_decompress_ptr dinfo)
{
  if ( (dinfo->dc_huff_tbl_ptrs[0] == NULL) &&
       (dinfo->dc_huff_tbl_ptrs[1] == NULL) &&
       (dinfo->ac_huff_tbl_ptrs[0] == NULL) &&
       (dinfo->ac_huff_tbl_ptrs[1] == NULL) ) {
 //   mjpeg_debug( "Generating standard Huffman tables for this frame.");
    std_huff_tables(dinfo);
  }
}
    /*
 * jpeg_data:       Buffer with jpeg data to decode
 * len:             Length of buffer
 * itype:           0: Not interlaced
 *                  1: Interlaced, Top field first
 *                  2: Interlaced, Bottom field first
 * ctype            Chroma format for decompression.
 *                  Currently only Y4M_CHROMA_{420JPEG,422} are available
 * returns:
 *	-1 on fatal error
 *	0 on success
 *	1 if jpeg lib threw a "corrupt jpeg data" warning.  
 *		in this case, "a damaged output image is likely."
 *	
 */
#define MAX_LUMA_WIDTH   3072
#define MAX_CHROMA_WIDTH 1536

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

static unsigned char buf0[16][MAX_LUMA_WIDTH];
static unsigned char buf1[8][MAX_CHROMA_WIDTH];
static unsigned char buf2[8][MAX_CHROMA_WIDTH];
static unsigned char chr1[8][MAX_CHROMA_WIDTH];
static unsigned char chr2[8][MAX_CHROMA_WIDTH];

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

int decode_jpeg_raw (unsigned char *jpeg_data, int len,
                     int itype, int ctype, int width, int height,
                     unsigned char *raw0, unsigned char *raw1,
                     unsigned char *raw2)
{
   int numfields, hsf[3], vsf[3], field, yl, yc, x, y = 0, i, xsl, xsc, xs, xd,
       hdown;
   int vsfcomm;  //
   struct jpeg_decompress_struct cinfo;   
   struct my_error_mgr jerr;

   JSAMPROW row0[16] = { buf0[0], buf0[1], buf0[2], buf0[3],
      buf0[4], buf0[5], buf0[6], buf0[7],
      buf0[8], buf0[9], buf0[10], buf0[11],
      buf0[12], buf0[13], buf0[14], buf0[15]
   };
   JSAMPROW row1[8] = { buf1[0], buf1[1], buf1[2], buf1[3],
			buf1[4], buf1[5], buf1[6], buf1[7]   };
   JSAMPROW row2[16] = { buf2[0], buf2[1], buf2[2], buf2[3],
			 buf2[4], buf2[5], buf2[6], buf2[7]  };
   JSAMPROW row1_444[16], row2_444[16];
   JSAMPARRAY scanarray[3] = { row0, row1, row2 };

//       printf("seg 17\n");
   /* We set up the normal JPEG error routines, then override error_exit. */
  // dinfo.err = jpeg_std_error (&jerr.pub);
  // jerr.pub.error_exit = my_error_exit;
   /* also hook the emit_message routine to note corrupt-data warnings */
 //  jerr.original_emit_message = jerr.pub.emit_message;
 //  jerr.pub.emit_message = my_emit_message;
 //  jerr.warning_seen = 0;

   /* Establish the setjmp return context for my_error_exit to use. */
 /*  if (setjmp (jerr.setjmp_buffer)) {
      // If we get here, the JPEG code has signaled an error.
      jpeg_destroy_decompress (&dinfo);
      return -1;
   }        */
  //     printf("seg 18\n");

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    return 0;
  }

   jpeg_create_decompress (&cinfo);
//       printf("seg 19\n");
   jpeg_buffer_src (&cinfo, jpeg_data, len);
//       printf("seg 20\n");
   /* Read header, make some checks and try to figure out what the
      user really wants */
   jpeg_read_header (&cinfo, 1);//TRUE);
   //     printf("seg 21\n");
   cinfo.raw_data_out = TRUE;
   cinfo.out_color_space = JCS_YCbCr;//JCS_RGB;//JCS_YCbCr;
   cinfo.dct_method = JDCT_IFAST;
   guarantee_huff_tables(&cinfo);
   //       printf("seg 22\n");
   jpeg_start_decompress (&cinfo);
  //      printf("seg 23\n");
   if (cinfo.output_components != 3) {
  //    mjpeg_error( "Output components of JPEG image = %d, must be 3",
  //             cinfo.output_components);
      goto ERR_EXIT;
   }
 //  printf("seg 24\n");
   for (i = 0; i < 3; i++) {
      hsf[i] = cinfo.comp_info[i].h_samp_factor;
      vsf[i] = cinfo.comp_info[i].v_samp_factor;
   }
//   printf("seg 25\n");
   //mjpeg_info( "Sampling factors, hsf=(%d, %d, %d) vsf=(%d, %d, %d) !", hsf[0], hsf[1], hsf[2], vsf[0], vsf[1], vsf[2]);
   if ((hsf[0] != 2 && hsf[0] != 1) || hsf[1] != 1 || hsf[2] != 1 ||
       (vsf[0] != 1 && vsf[0] != 2) || vsf[1] != 1 || vsf[2] != 1) {
  //    mjpeg_error( "Unsupported sampling factors, hsf=(%d, %d, %d) vsf=(%d, %d, %d) !", hsf[0], hsf[1], hsf[2], vsf[0], vsf[1], vsf[2]);
      goto ERR_EXIT;
   }

   if (hsf[0] == 1)
     {
       if (height % 8 != 0)
	 {
     //	   mjpeg_error( "YUV 4:4:4 sampling, but image height %d not dividable by 8 !\n", height);
	   goto ERR_EXIT;	   
	 }

    //   mjpeg_info("YUV 4:4:4 sampling encountered ! Allocating special row buffer\n");
       for (y = 0; y < 16; y++) // allocate a special buffer for the extra sampling depth
	 {
	   //mjpeg_info("YUV 4:4:4 %d.\n",y);
	   row1_444[y] = (unsigned char *)malloc(cinfo.output_width * sizeof(char));
	   row2_444[y] = (unsigned char *)malloc(cinfo.output_width * sizeof(char));
	 }
       //mjpeg_info("YUV 4:4:4 sampling encountered ! Allocating done.\n");
       scanarray[1] = row1_444; 
       scanarray[2] = row2_444; 
     }

   /* Height match image height or be exact twice the image height */
 //  printf("seg 26\n");
   if (cinfo.output_height == height) {
      numfields = 1;
   } else if (2 * cinfo.output_height == height) {
      numfields = 2;
   } else {
   //   mjpeg_error(
   //            "Read JPEG: requested height = %d, height of image = %d",
   //            height, dinfo.output_height);
      goto ERR_EXIT;
   }

   /* Width is more flexible */
  // printf("seg 27\n");
   if (cinfo.output_width > MAX_LUMA_WIDTH) {
   //   mjpeg_error( "Image width of %d exceeds max",
   //            dinfo.output_width);
      goto ERR_EXIT;
   }
//   printf("seg 28\n");
   if (width < 2 * cinfo.output_width / 3) {
      /* Downsample 2:1 */
      printf("seg 29\n");
      hdown = 1;
      if (2 * width < cinfo.output_width)
         xsl = (cinfo.output_width - 2 * width) / 2;
      else
         xsl = 0;
   } else if (width == 2 * cinfo.output_width / 3) {
      /* special case of 3:2 downsampling */
 //     printf("seg 30\n");
      hdown = 2;
      xsl = 0;
   } else {
      /* No downsampling */
 //     printf("seg 31\n");
      hdown = 0;
      if (width < cinfo.output_width)
         xsl = (cinfo.output_width - width) / 2;
      else
         xsl = 0;
   }
 //  printf("seg 32\n");
   /* Make xsl even, calculate xsc */

   xsl = xsl & ~1;
   xsc = xsl / 2;

   yl = yc = 0;

   for (field = 0; field < numfields; field++) {
      if (field > 0) {
         jpeg_read_header (&cinfo, TRUE);
         cinfo.raw_data_out = TRUE;
         cinfo.out_color_space = JCS_YCbCr;
         cinfo.dct_method = JDCT_IFAST;
         jpeg_start_decompress (&cinfo);
      }
 //   printf("seg 33\n");
      if (numfields == 2) {
    /*     switch (itype) {
         case Y4M_ILACE_TOP_FIRST:
            yl = yc = field;
            break;
         case Y4M_ILACE_BOTTOM_FIRST:
            yl = yc = (1 - field);
            break;
         default:
            mjpeg_error(
                     "Input is interlaced but no interlacing set");
            goto ERR_EXIT;
         }     */
         printf("seg 34\n");
      } else
         yl = yc = 0;
/* printf("numfields =%d\n",numfields);
  printf("width =%d\n",width);
    printf("height =%d\n",height);
 printf("cinfo.output_width =%d\n",cinfo.output_width);
  printf("cinfo.output_height =%d\n",cinfo.output_height);
 printf("cinfo.output_scanline =%d\n",cinfo.output_scanline);
 printf("vsf[0] =%d\n",vsf[0]);     */
 vsfcomm = 8 * vsf[0];
      while (cinfo.output_scanline < cinfo.output_height) {
  //    while ((cinfo.output_scanline + 16) <= cinfo.output_height) {
	/* read raw data */
    //    printf("cinfo.output_scanline = %d  cinfo.output_height = %d, vsfcomm = %d\n",
    //            cinfo.output_scanline, cinfo.output_height, vsfcomm);
        if((cinfo.output_scanline + vsfcomm) > cinfo.output_height)
          vsfcomm = cinfo.output_height - cinfo.output_scanline;
	jpeg_read_raw_data (&cinfo, scanarray, vsfcomm);

         for (y = 0; y < vsfcomm; yl += numfields, y++) {
            xd = yl * width;
            xs = xsl;

            if (hdown == 0)
               for (x = 0; x < width; x++)
                  raw0[xd++] = row0[y][xs++];
            else if (hdown == 1)
               for (x = 0; x < width; x++, xs += 2)
                  raw0[xd++] = (row0[y][xs] + row0[y][xs + 1]) >> 1;
            else
               for (x = 0; x < width / 2; x++, xd += 2, xs += 3) {
                  raw0[xd] = (2 * row0[y][xs] + row0[y][xs + 1]) / 3;
                  raw0[xd + 1] =
                      (2 * row0[y][xs + 2] + row0[y][xs + 1]) / 3;
               }
         }
        //printf("2222\n");

	 /* Horizontal downsampling of chroma */

         for (y = 0; y < 8; y++) {
            xs = xsc;

	    if (hsf[0] == 1)
	      for (x = 0; x < width / 2; x++, xs++) 
		{		  
		  row1[y][xs] = (row1_444[y][2*x] + row1_444[y][2*x + 1]) >> 1;
		  row2[y][xs] = (row2_444[y][2*x] + row2_444[y][2*x + 1]) >> 1;
		}

            xs = xsc;
            if (hdown == 0)
               for (x = 0; x < width / 2; x++, xs++) {
		 chr1[y][x] = row1[y][xs];
		 chr2[y][x] = row2[y][xs];
            } else if (hdown == 1)
               for (x = 0; x < width / 2; x++, xs += 2) {
                  chr1[y][x] = (row1[y][xs] + row1[y][xs + 1]) >> 1;
                  chr2[y][x] = (row2[y][xs] + row2[y][xs + 1]) >> 1;
            } else
               for (x = 0; x < width / 2; x += 2, xs += 3) {
                  chr1[y][x] = (2 * row1[y][xs] + row1[y][xs + 1]) / 3;
                  chr1[y][x + 1] =
                      (2 * row1[y][xs + 2] + row1[y][xs + 1]) / 3;
                  chr2[y][x] = (2 * row2[y][xs] + row2[y][xs + 1]) / 3;
                  chr2[y][x + 1] =
                      (2 * row2[y][xs + 2] + row2[y][xs + 1]) / 3;
               }
         }

	 /* Vertical resampling of chroma */
	 switch (ctype) {
    /*	 case Y4M_CHROMA_422:
	   if (vsf[0] == 1) {
	     // Just copy
	     for (y = 0; y < 8 ; y++, yc += numfields) {
	       xd = yc * width / 2;
	       for (x = 0; x < width / 2; x++, xd++) {
		 raw1[xd] = chr1[y][x];
		 raw2[xd] = chr2[y][x];
	       }
	     }
	   } else {
	     // upsample
	     for (y = 0; y < 8 ; y++) {
	       xd = yc * width / 2;
	       for (x = 0; x < width / 2; x++, xd++) {
		 raw1[xd] = chr1[y][x];
		 raw2[xd] = chr2[y][x];
	       }
	       yc += numfields;
	       xd = yc * width / 2;
	       for (x = 0; x < width / 2; x++, xd++) {
		 raw1[xd] = chr1[y][x];
		 raw2[xd] = chr2[y][x];
	       }
	       yc += numfields;
	     }
	   }
	   break;    */
	 default:
/*
 * should be case Y4M_CHROMA_420JPEG: but use default: for compatibility. Some
 * pass things like '420' in with the expectation that anything other than
 * Y4M_CHROMA_422 will default to 420JPEG.
*/
	   if (vsf[0] == 1) {
	     /* Really downsample */
	     for (y = 0; y < 8 /*&& yc < height/2*/; y += 2, yc += numfields) {
	       xd = yc * width / 2;
	       for (x = 0; x < width / 2; x++, xd++) {
		 assert(xd < (width * height / 4));
		 raw1[xd] = (chr1[y][x] + chr1[y + 1][x]) >> 1;
		 raw2[xd] = (chr2[y][x] + chr2[y + 1][x]) >> 1;
	       }
	     }

	   } else {
	     /* Just copy */
	     for (y = 0; y < 8 /*&& yc < height/2 */; y++, yc += numfields) {
	       xd = yc * width / 2;
	       for (x = 0; x < width / 2; x++, xd++) {
		 raw1[xd] = chr1[y][x];
		 raw2[xd] = chr2[y][x];
	       }
	     }
	   }
	   break;
	 }
      }
//printf("seg 36\n");
      (void) jpeg_finish_decompress (&cinfo);
      if (field == 0 && numfields > 1)
         jpeg_skip_ff (&cinfo);
   }

   if (hsf[0] == 1)
     {
       //mjpeg_info("YUV 4:4:4 sampling encountered ! Deallocating special row buffer\n");
       for (y = 0; y < 16; y++) // allocate a special buffer for the extra sampling depth
	 {
	   free(row1_444[y]);
	   free(row2_444[y]);
	 }
     }

   jpeg_destroy_decompress (&cinfo);
 //  if(jerr.warning_seen)
	   return 1;
 //  else
 //	   return 0;

 ERR_EXIT:
   jpeg_destroy_decompress (&cinfo);
   return -1;
}
/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * Should never be called since all data should be allready provided.
 * Is nevertheless sometimes called - sets the input buffer to data
 * which is the JPEG EOI marker;
 *
 */

static uint8_t EOI_data[2] = { 0xFF, 0xD9 };

static boolean fill_input_buffer (j_decompress_ptr dinfo)
{
   dinfo->src->next_input_byte = EOI_data;
   dinfo->src->bytes_in_buffer = 2;
   return TRUE;
}
   /*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

static void init_source (j_decompress_ptr dinfo)
{
   /* no work necessary here */
}
/*
 * Prepare for input from a data buffer.
 */

 /*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 */

static void skip_input_data (j_decompress_ptr dinfo, long num_bytes)
{
   if (num_bytes > 0) {
      if (num_bytes > (long) dinfo->src->bytes_in_buffer)
         num_bytes = (long) dinfo->src->bytes_in_buffer;
      dinfo->src->next_input_byte += (size_t) num_bytes;
      dinfo->src->bytes_in_buffer -= (size_t) num_bytes;
   }
}


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 */

static void term_source (j_decompress_ptr cinfo)
{
   /* no work necessary here */
}

static void
jpeg_buffer_src (j_decompress_ptr cinfo, unsigned char *buffer, long num)
{
   /* The source object and input buffer are made permanent so that a series
    * of JPEG images can be read from the same buffer by calling jpeg_buffer_src
    * only before the first one.  (If we discarded the buffer at the end of
    * one image, we'd likely lose the start of the next one.)
    * This makes it unsafe to use this manager and a different source
    * manager serially with the same JPEG object.  Caveat programmer.
    */
   if (cinfo->src == NULL) {    /* first time for this JPEG object? */
      cinfo->src = (struct jpeg_source_mgr *)
          (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                      sizeof (struct jpeg_source_mgr));
   }

   cinfo->src->init_source = init_source;
   cinfo->src->fill_input_buffer = fill_input_buffer;
   cinfo->src->skip_input_data = skip_input_data;
   cinfo->src->resync_to_restart = jpeg_resync_to_restart;	/* use default method */
   cinfo->src->term_source = term_source;
   cinfo->src->bytes_in_buffer = num;
   cinfo->src->next_input_byte = (JOCTET *) buffer;
}

/*
 * jpeg_skip_ff is not a part of the source manager but it is
 * particularly useful when reading several images from the same buffer:
 * It should be called to skip padding 0xff bytes beetween images.
 */

static void
jpeg_skip_ff (j_decompress_ptr cinfo)
{
   while (cinfo->src->bytes_in_buffer > 1
          && cinfo->src->next_input_byte[0] == 0xff
          && cinfo->src->next_input_byte[1] == 0xff) {
      cinfo->src->bytes_in_buffer--;
      cinfo->src->next_input_byte++;
   }
}
