#include <stdio.h>
#include <inttypes.h>

#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include "jpegutils.h"
#define MAXPIXELS (2048*1024)//1280*1024)  /* Maximum size of final image */

typedef struct _parameters {
//  char *jpegformatstr;
  char jpegformatstr[10];
  uint32_t begin;       /* the video frame start */
  int32_t numframes;   /* -1 means: take all frames */
//  y4m_ratio_t framerate;
//  y4m_ratio_t aspect_ratio;
  int interlace;   /* will the YUV4MPEG stream be interlaced? */
  int interleave;  /* are the JPEG frames field-interleaved? */
  int verbose; /* the verbosity of the program (see mjpeg_logging.h) */

  int width;
  int height;
  int colorspace;
  int loop;
  int rescale_YUV;
} parameters_t;

//parameters_t t_param;
struct jpeg_decompress_struct dinfo;
struct jpeg_error_mgr jerr;

int Zoon_YUV420_Linear(int dWidth, int dHeight, int sWidth, int sHeight,
              unsigned char * aDestY, unsigned char * aDestU, unsigned char * aDestV,
              unsigned char * aSrcY, unsigned char * aSrcU, unsigned char * aSrcV);
/*
 * The file handling parts
 */

/** init_parse_files
 * Verifies the JPEG input files and prepares YUV4MPEG header information.
 * remember first filename for later
 * @returns 0 on success
 */
int init_parse_files(parameters_t *param_t, char *jpegname)
{
  FILE *jpegfile;
  struct jpeg_decompress_struct dinfo1;
  struct jpeg_error_mgr jerr1;

  if (param_t->jpegformatstr) {
    //   snprintf(jpegname, FILENAME_MAX, param_t->jpegformatstr, param_t->begin);
  //     printf("jpegname is %s\n",jpegname);
    jpegfile = fopen(jpegname, "rb");
    if (jpegfile == NULL) {
      printf("Read from '%s' failed!\n", jpegname);
      return 1;
      }
  }
/*  else {
       char *p;
       p = fgets(jpegname, FILENAME_MAX, stdin);
       if (p) {
           strip(jpegname);
           jpegfile = fopen(jpegname, "rb");
       }
       else {
           jpegfile = NULL;
       }
  }       */
//  printf("seg 4\n");
//  mjpeg_debug("Analyzing %s to get the right pic params", jpegname);

  if (jpegfile == NULL)
    printf("System error while opening:");

  /* Now open this JPEG file, and examine its header to retrieve the
     YUV4MPEG info that shall be written */
  dinfo1.err = jpeg_std_error(&jerr1);  // ??????????

  jpeg_create_decompress(&dinfo1);

  jpeg_stdio_src(&dinfo1, jpegfile);

  jpeg_read_header(&dinfo1, 1);

//  printf("dinfo.src->free_in_buffer0= %d\n", dinfo.src->bytes_in_buffer);
//  printf("dinfo.src->next_output_byte0= %x\n", dinfo.src->next_input_byte);


  switch (dinfo1.jpeg_color_space)
    {
    case JCS_YCbCr:
  //    printf("YUV colorspace detected.\n");
      dinfo1.out_color_space = JCS_YCbCr;
      break;
    case JCS_GRAYSCALE:
      printf("Grayscale colorspace detected.\n");
      dinfo1.out_color_space = JCS_GRAYSCALE;
      break;
    default:
   //   mjpeg_error("Unsupported colorspace detected.\n"); break;
    }


  jpeg_start_decompress(&dinfo1);

  if (dinfo1.output_components != 3 && dinfo1.out_color_space == JCS_YCbCr)
    printf("Output components of color JPEG image = %d, must be 3.",
  		      dinfo1.output_components);

  if (dinfo1.output_components != 1 && dinfo1.out_color_space == JCS_GRAYSCALE)
    printf("Output components of grayscale JPEG image = %d, must be 1.",
   	      dinfo1.output_components);

  param_t->width = dinfo1.image_width;
  param_t->height = dinfo1.image_height;
  param_t->colorspace = dinfo1.jpeg_color_space;

//  jpeg_finish_decompress(&dinfo1);

  jpeg_destroy_decompress(&dinfo1);
  fclose(jpegfile);


//  mjpeg_info("Movie frame rate is:  %f frames/second"
//	     );

/*  switch (param->interlace) {
  case Y4M_ILACE_NONE:
    mjpeg_info("Non-interlaced/progressive frames.");
    break;
  case Y4M_ILACE_BOTTOM_FIRST:
    mjpeg_info("Interlaced frames, bottom field first.");
    break;
  case Y4M_ILACE_TOP_FIRST:
    mjpeg_info("Interlaced frames, top field first.");      
    break;
  default:
    mjpeg_error_exit1("Interlace has not been specified (use -I option)");
    break;
  }

  if ((param->interlace != Y4M_ILACE_NONE) && (param->interleave == -1))
    mjpeg_error_exit1("Interleave has not been specified (use -L option)");

  if (!(param->interleave) && (param->interlace != Y4M_ILACE_NONE)) {
    param->height *= 2;
    mjpeg_info("Non-interleaved fields (image height doubled)");
  }
  mjpeg_info("Frame size:  %d x %d", param->width, param->height);
                  */
  return 0;
}
//---------------------------------------------------------------------------
int init_parse_files1(parameters_t *param_t, char *jpegname)
{
  FILE *jpegfile;
//  struct jpeg_decompress_struct dinfo1;
//  struct jpeg_error_mgr jerr1;

  if (param_t->jpegformatstr) {
    jpegfile = fopen(jpegname, "rb");
    if (jpegfile == NULL) {
      printf("Read from '%s' failed!\n", jpegname);
      return 1;
      }
  }

  if (jpegfile == NULL)
    printf("System error while opening:");

  /* Now open this JPEG file, and examine its header to retrieve the
     YUV4MPEG info that shall be written */
/*  dinfo1.err = jpeg_std_error(&jerr1);  // ??????????

  jpeg_create_decompress(&dinfo1);

  jpeg_stdio_src(&dinfo1, jpegfile);

  jpeg_read_header(&dinfo1, 1);

//  printf("dinfo.src->free_in_buffer0= %d\n", dinfo.src->bytes_in_buffer);
//  printf("dinfo.src->next_output_byte0= %x\n", dinfo.src->next_input_byte);


  switch (dinfo1.jpeg_color_space)
    {
    case JCS_YCbCr:
  //    printf("YUV colorspace detected.\n");
      dinfo1.out_color_space = JCS_YCbCr;
      break;
    case JCS_GRAYSCALE:
      printf("Grayscale colorspace detected.\n");
      dinfo1.out_color_space = JCS_GRAYSCALE;
      break;
    default:
   //   mjpeg_error("Unsupported colorspace detected.\n"); break;
    }


  jpeg_start_decompress(&dinfo1);

  if (dinfo1.output_components != 3 && dinfo1.out_color_space == JCS_YCbCr)
    printf("Output components of color JPEG image = %d, must be 3.",
  		      dinfo1.output_components);

  if (dinfo1.output_components != 1 && dinfo1.out_color_space == JCS_GRAYSCALE)
    printf("Output components of grayscale JPEG image = %d, must be 1.",
   	      dinfo1.output_components);

  param_t->width = dinfo1.image_width;
  param_t->height = dinfo1.image_height;
  param_t->colorspace = dinfo1.jpeg_color_space;

//  jpeg_finish_decompress(&dinfo1);

  jpeg_destroy_decompress(&dinfo1);
//  jpeg_abort_decompress(&dinfo1);  */
  fclose(jpegfile);

  return 0;
}
//---------------------------------------------------------------------------
GLOBAL(void) write_JPEG_file (char * filename, int quality, JSAMPLE * image_buffer,
                              int image_width, int image_height)
{
  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;		/* target file */
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  int row_stride;		/* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return;
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = image_width; 	/* image width and height, in pixels */
  cinfo.image_height = image_height;
  cinfo.input_components = 3;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
}
//---------------------------------------------------------------------------
/*int init_parse_files420(parameters_t *param_t, char *jpegname)
{
//uint8_t * pSrc// this is RGB bit stream
  //   uint8_t * YUV_Image=new uint_8[320*240*3/2];// YUV420 bit stream
int i,j,y;
     int width; // width of the RGB image
     int height; // height of the RGB image
     int uPos, vPos;
     uint8_t isU;
     int pos;
     uint8_t B;
     uint8_t G;
     uint8_t R;
     uint8_t Y;
     uint8_t U;
     uint8_t V;
     uint8_t isChr;
     unsigned char  *buffer;
     unsigned char  *rgbbuffer;
     int rect1width, rect1height, xleft, ytop;

  FILE *jpegfile;
  uint8_t *yuv[3];  // buffer for Y/U/V planes of decoded JPEG 
//  printf("seg 3\n");
//  printf("jpegname is %s\n",jpegname);
  if (param_t->jpegformatstr) {
   //      printf("seg 9\n");
    //   snprintf(jpegname, FILENAME_MAX, param_t->jpegformatstr, param_t->begin);
  //     printf("jpegname is %s\n",jpegname);
       jpegfile = fopen(jpegname, "rb");
  }
  else {
       char *p;
         printf("seg 10\n");
       p = fgets(jpegname, FILENAME_MAX, stdin);
       if (p) {
           strip(jpegname);
           jpegfile = fopen(jpegname, "rb");
       }
       else {
           jpegfile = NULL;
       }
  }
//  printf("seg 4\n");
//  mjpeg_debug("Analyzing %s to get the right pic params", jpegname);
  
  if (jpegfile == NULL)
    printf("System error while opening:"
		      );

  // Now open this JPEG file, and examine its header to retrieve the
  //   YUV4MPEG info that shall be written
  dinfo.err = jpeg_std_error(&jerr);  // ??????????
//  printf("seg 5\n");
  jpeg_create_decompress(&dinfo);
//  printf("seg 6\n");
  jpeg_stdio_src(&dinfo, jpegfile);
//  printf("seg 7\n");
  jpeg_read_header(&dinfo, 1);
  printf("seg 8\n");

  switch (dinfo.jpeg_color_space)
    {
    case JCS_YCbCr:
  //    printf("YUV colorspace detected.\n");
      dinfo.out_color_space = JCS_YCbCr;
      break;
    case JCS_GRAYSCALE:
      printf("Grayscale colorspace detected.\n");
      dinfo.out_color_space = JCS_GRAYSCALE;
      break;
    default:
   //   mjpeg_error("Unsupported colorspace detected.\n"); break;
    }

 // printf("Starting decompression\n");

  jpeg_start_decompress(&dinfo);

  if (dinfo.output_components != 3 && dinfo.out_color_space == JCS_YCbCr)
    printf("Output components of color JPEG image = %d, must be 3.",
  		      dinfo.output_components);

  if (dinfo.output_components != 1 && dinfo.out_color_space == JCS_GRAYSCALE)
    printf("Output components of grayscale JPEG image = %d, must be 1.",
   	      dinfo.output_components);

//  printf("Image dimensions are %dx%d\n",
//	     dinfo.image_width, dinfo.image_height);
//  printf("dinfo.output_components= %d\n",
//	     dinfo.output_components);

  param_t->width = dinfo.image_width;
  param_t->height = dinfo.image_height;
  param_t->colorspace = dinfo.jpeg_color_space;

  yuv[0] = malloc(param_t->width * param_t->height * sizeof(yuv[0][0]));
  yuv[1] = malloc(param_t->width * param_t->height / 4 * sizeof(yuv[1][0]));
  yuv[2] = malloc(param_t->width * param_t->height / 4 * sizeof(yuv[2][0]));

  printf("dinfo.output_width= %d\n",dinfo.output_width);
  printf("dinfo.output_height= %d\n",dinfo.output_height);
  printf("dinfo.output_components= %d\n",dinfo.output_components);
  	y = 0;
        rgbbuffer = (unsigned char *) malloc(dinfo.output_width * dinfo.output_height * dinfo.output_components);
	buffer = (unsigned char *) malloc(dinfo.output_width * dinfo.output_components);

	while (dinfo.output_scanline < dinfo.output_height) {
		jpeg_read_scanlines(&dinfo, &buffer, 1);
                memcpy(rgbbuffer + y*dinfo.output_width, buffer, dinfo.output_width);
//printf("cinfo.output_scanline=%d\n",cinfo.output_scanline);

		y++;					// next scanline
	}
     printf("seg 9\n");
     width = dinfo.output_width;
     height = dinfo.output_height;
     uPos=0;
     vPos=0;
     for( i=0;i< height;i++ ){
         isU=0;
         if( i%2==0 ) isU=1; // this is a U line
         for( j=0;j<width;j++ ){

              pos = width * i + j; // pixel position
     //         printf("pos=%d\n",pos);
              B = rgbbuffer[pos*3];
//     printf("seg 10\n");
              G = rgbbuffer[pos*3+1];
  //   printf("seg 11\n");
              R = rgbbuffer[pos*3+2];
  Y = (uint8_t)(0.257*R + 0.504*G + 0.098*B + 16);
  U = (uint8_t)(-0.148*R - 0.291*G + 0.439*B + 128);
  V = (uint8_t)(0.439*R - 0.368*G - 0.071*B + 128);

 //    printf("seg 12\n");
              yuv[0][pos] = Y;
 //    printf("seg 13\n");
              isChr=0;  // is this a chroma point
              if( j%2==0 ) isChr=1;
              if( (isChr==1) && (isU==1) ){
                   yuv[1][uPos]=U;
                   uPos++;
      //             printf("seg 14\n");
              }
              if( (isChr==1)&& (isU !=1)){
                   yuv[2][vPos]=V;
                   vPos++;
      //             printf("seg 15\n");
              }
         }
}


        rect1width = param_t->width;
        rect1height = param_t->height;
        xleft = 0;
        ytop = 0;

       printf("seg 38\n");
        for(j=0;j<rect1height;j++)
         memcpy(fbmem+(j+ytop)*720+xleft, yuv[0]+j*rect1width, rect1width);
      //   for(i=rect1Left;i<rect1Left+rect1width;i++)
      //     *(fbmem + j*720 + i) = yuv[0][j*rect1width + i]; //blue
    //   printf("seg 39\n");
        for(j=0;j<rect1height/2;j++)
         memcpy(fbmem + f_data.uv_offset + (j+ytop/2)*360 +xleft/2,
                yuv[1]+j*rect1width/2, rect1width/2);
      //   for(i=rect1Left/2;i<(rect1Left+rect1width)/2;i++)
      //     *(fbmem + f_data.uv_offset + j*360 +i) = yuv[1][j*rect1width/2 +i];  //blue

        for(j=0;j<rect1height/2;j++)
         memcpy(fbmem + f_data.uv_offset+f_data.uv_offset/4 + (j+ytop/2)*360 +xleft/2,
                yuv[2]+j*rect1width/2, rect1width/2);
      //   for(i=rect1Left/2;i<(rect1Left+rect1width)/2;i++)
      //     *(fbmem + f_data.uv_offset+f_data.uv_offset/4 + j*360 +i) = yuv[2][j*rect1width/2 +i]; //blue
     //  printf("seg 40\n");

  free(yuv[0]);
  free(yuv[1]);
  free(yuv[2]);
	free(buffer);
        free(rgbbuffer);

  jpeg_destroy_decompress(&dinfo);
  fclose(jpegfile);

  return 0;
}                        */
//---------------------------------------------------------------------------
/**
  Rescales the YUV values from the range 0..255 to the range 16..235
  @param yp: buffer for Y plane of decoded JPEG
  @param up: buffer for U plane of decoded JPEG
  @param vp: buffer for V plane of decoded JPEG
*/
void rescale_color_vals(int width, int height, uint8_t *yp, uint8_t *up, uint8_t *vp)
{
  int x,y;
  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
      yp[x+y*width] = (float)(yp[x+y*width]) * ((235.0 - 16.0)/255.0) + 16.0;

  for (y = 0; y < height/2; y++)
    for (x = 0; x < width/2; x++)
      {
	up[x+y*width/2] = (float)(up[x+y*width/2]) * ((240.0 - 16.0)/255.0) + 16.0;
	vp[x+y*width/2] = (float)(vp[x+y*width/2]) * ((240.0 - 16.0)/255.0) + 16.0;
      }
}
//---------------------------------------------------------------------------
struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/**
  Open and read a file if the file name
  is not the same as the previous file name.
  @param jpegdata: buffer where the JPEG data will be read into
  @param jpegname: JPEG file name
  @param prev_jpegname: previous JPEG file name
  @returns 0  if the previous read data is still valid.
           -1 if the file could not be opened.
           >0 the number of bytes read into jpegdata.
*/
size_t read_jpeg_data(uint8_t *jpegdata, char *jpegname, char *prev_jpegname)
{
  FILE *jpegfile;
  size_t jpegsize;
//    printf("open1 jpegname is %s\n",jpegname);
//  if (strncmp(jpegname, prev_jpegname, strlen(jpegname)) != 0) {
//    strncpy(prev_jpegname, jpegname, strlen(jpegname));

//    printf("open2 jpegname is %s\n",jpegname);

    jpegfile = fopen(jpegname, "rb");
    if (jpegfile == NULL) {
      jpegsize = -1;
      printf("Read from '%s' failed!\n", jpegname);
    } else {
      jpegsize = fread(jpegdata, sizeof(unsigned char), MAXPIXELS, jpegfile);
      fclose(jpegfile);
    }

//  }
//  else {
//    jpegsize = 0;
//  }
//     printf("jpegsize is %d\n",jpegsize);
  return jpegsize;
}
//---------------------------------------------------------------------------
uint8_t jpegnumdata[MAXPIXELS];  /* that ought to be enough */
int generate_YUV4MPEG(parameters_t *param, char *firstjpeg,
                int rect_width, int rect_height, uint8_t *yuv[3])
{
int i,j;
//int rect1width,rect1height;
  uint32_t frame;
  size_t jpegsize;
  char jpegname[FILENAME_MAX];
  char prev_jpegname[FILENAME_MAX];
  int loops;                                 /* number of loops to go */
//  uint8_t *yuv[3];  /* buffer for Y/U/V planes of decoded JPEG */

  jpegsize = 0;
  loops = param->loop;
//  printf("seg 11\n");

/*  yuv[0] = malloc(param->width * param->height * sizeof(yuv[0][0]));
  yuv[1] = malloc(param->width * param->height / 4 * sizeof(yuv[1][0]));
  yuv[2] = malloc(param->width * param->height / 4 * sizeof(yuv[2][0]));
*/
  prev_jpegname[0] = 0;
  do {
  //   for (frame = param->begin;
  //        (frame < param->numframes + param->begin) || (param->numframes == -1);
  //        frame++) {
   //             printf("seg 12\n");
 //    printf("param_t->jpegformatstr11 is %s\n",param->jpegformatstr);
       if (param->jpegformatstr) {
         strcpy(jpegname, firstjpeg);
	 jpegsize = read_jpeg_data(jpegnumdata, jpegname, prev_jpegname);
       }
       else {
           char *p;
           if (firstjpeg) {
               p = firstjpeg;
               sprintf(jpegname, firstjpeg);
               firstjpeg = NULL;
           }
           else {
               p = fgets(jpegname, FILENAME_MAX, stdin);
           }
           if (p) {
               strip(jpegname);
             jpegsize = read_jpeg_data(jpegnumdata, jpegname, prev_jpegname);
           }
           else {
             jpegsize = 0;
           }
       }        
   //    printf("seg 16\n");
   //   mjpeg_info("Numframes %i  jpegsize %i", param->numframes, jpegsize);
       if (jpegsize == -1) {
		  //	mjpeg_info("in jpegsize < 0");
         if (param->numframes == -1) {
         //  mjpeg_info("No more frames.  Stopping.");
           break;  /* we are done; leave 'while' loop */
         } else {
        //   mjpeg_info("Rewriting latest frame instead.");
         }
       }

       if (jpegsize > 0) {
    //     mjpeg_debug("Preparing frame");

         /* decode_jpeg_raw:s parameters from 20010826
          * jpeg_data:       buffer with input / output jpeg
          * len:             Length of jpeg buffer
          * itype:           0: Interleaved/Progressive
          *                  1: Not-interleaved, Top field first
          *                  2: Not-interleaved, Bottom field first
          * ctype            Chroma format for decompression.
          *                  Currently always 420 and hence ignored.
          * raw0             buffer with input / output raw Y channel
          * raw1             buffer with input / output raw U/Cb channel
          * raw2             buffer with input / output raw V/Cr channel
          * width            width of Y channel (width of U/V is width/2)
          * height           height of Y channel (height of U/V is height/2)
          */

	     decode_jpeg_raw(jpegnumdata, jpegsize,
			     0, 420, param->width, param->height,
			     yuv[0], yuv[1], yuv[2]);
           //     printf("seg 37\n");

    /*	 if (param->rescale_YUV)
	   {
	//     mjpeg_info("Rescaling color values.");
	     rescale_color_vals(param->width, param->height, yuv[0], yuv[1], yuv[2]);
	   }    */
       }
     if (param->loop != -1)
       loops--;

  } while( loops >=1 || loops == -1 );

/*        if(true_size == 1)
         {
          rect1width = param->width;
          rect1height = param->height;
         }
        else
         {
          rect1width = rect_width;
          rect1height = rect_height;
         }

   //    printf("seg 38\n");
        for(j=0;j<rect1height;j++)
         memcpy(fbmem+(j+ytop)*fb_width+xleft, yuv[0]+j*rect1width, rect1width);
      //   for(i=rect1Left;i<rect1Left+rect1width;i++)
      //     *(fbmem + j*720 + i) = yuv[0][j*rect1width + i]; //blue
    //   printf("seg 39\n");
        for(j=0;j<rect1height/2;j++)
         memcpy(fbmem + f_data.uv_offset + (j+ytop/2)*fb_width/2 +xleft/2,
                yuv[1]+j*rect1width/2, rect1width/2);
      //   for(i=rect1Left/2;i<(rect1Left+rect1width)/2;i++)
      //     *(fbmem + f_data.uv_offset + j*360 +i) = yuv[1][j*rect1width/2 +i];  //blue

        for(j=0;j<rect1height/2;j++)
         memcpy(fbmem + f_data.uv_offset+f_data.uv_offset/4 + (j+ytop/2)*fb_width/2 +xleft/2,
                yuv[2]+j*rect1width/2, rect1width/2);
      //   for(i=rect1Left/2;i<(rect1Left+rect1width)/2;i++)
      //     *(fbmem + f_data.uv_offset+f_data.uv_offset/4 + j*360 +i) = yuv[2][j*rect1width/2 +i]; //blue
     //  printf("seg 40\n");

  free(yuv[0]);
  free(yuv[1]);
  free(yuv[2]);        */

  return 0;
}
//---------------------------------------------------------------------------
/*
 * Sample routine for JPEG decompression.  We assume that the source file name
 * is passed in.  We want to return 1 on success, 0 on error.
 */


GLOBAL(int)
read_JPEG_file (char * filename, unsigned char *rgb24data)
{
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  unsigned char *jbuffer;
  int row_stride;		/* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
//  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  jbuffer = (unsigned char *)malloc(800*480*3);//cinfo.output_width*cinfo.output_height*6);
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
//     printf("cinfo.output_scanline = %d,cinfo.output_width = %d, cinfo.output_height = %d,cinfo.output_components=%d\n",
//      cinfo.output_scanline, cinfo.output_width, cinfo.output_height, cinfo.output_components);
 //    getchar();
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
  //  getchar();
    memcpy(jbuffer + cinfo.output_width*cinfo.output_scanline*3, buffer[0], cinfo.output_width*3);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
//    put_scanline_someplace(buffer[0], row_stride);
  }
 free(jbuffer); 
  memcpy(rgb24data , jbuffer, cinfo.output_width*cinfo.output_height*3);
  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
  return 1;
}
//---------------------------------------------------------------------------
int Zoon_YUV420_Linear(int dWidth, int dHeight, int sWidth, int sHeight,
              unsigned char * aDestY, unsigned char * aDestU, unsigned char * aDestV,
              unsigned char * aSrcY, unsigned char * aSrcU, unsigned char * aSrcV)
{
//  int sw = aSrc->Width - 1, sh = aSrc->Height - 1, dw = aDest->Width - 1, dh = aDest->Height - 1;
  int i, j, k;
  int sw, sh, dw, dh;
  int B, N, x, y;
  int nPixelSize;// = GetPixelSize( aDest->PixelFormat );
  unsigned char * pLinePrev, *pLineNext;
  unsigned char * pDest;
  unsigned char * pA, *pB, *pC, *pD;
  nPixelSize = 1;//3
  //Y
  sw = sWidth - 1;
  sh = sHeight - 1;
  dw = dWidth - 1;
  dh = dHeight - 1;
  for ( i = 0; i <= dh; ++i )
   {
    pDest = ( unsigned char * )aDestY + i*dWidth;//->ScanLine[i];
    y = i * sh / dh;
    N = dh - i * sh % dh;
    pLinePrev = ( unsigned char * )aSrcY + y*sWidth;//->ScanLine[y++];
    pLineNext = ( N == dh ) ? pLinePrev : ( unsigned char * )aSrcY + y*sWidth;//->ScanLine[y];
    for ( j = 0; j <= dw; j+=4 )
     {
      x = j * sw / dw;// * nPixelSize;
      B = dw - j * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + (dw * dh) / 2 ) / ( dw * dh )
       );

      x = (j+1) * sw / dw;// * nPixelSize;
      B = dw - (j+1) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + (dw * dh) / 2 ) / ( dw * dh )
       );

      x = (j+2) * sw / dw;// * nPixelSize;
      B = dw - (j+2) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + (dw * dh) / 2 ) / ( dw * dh )
       );

      x = (j+3) * sw / dw;// * nPixelSize;
      B = dw - (j+3) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + (dw * dh) / 2 ) / ( dw * dh )
       );

/*      x = (j+4) * sw / dw;// * nPixelSize;
      B = dw - (j+4) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+5) * sw / dw;// * nPixelSize;
      B = dw - (j+5) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+6) * sw / dw;// * nPixelSize;
      B = dw - (j+6) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+7) * sw / dw;// * nPixelSize;
      B = dw - (j+7) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );      */
     }
   }

  //U
  sw = sWidth/2 - 1;
  sh = sHeight/2 - 1;
  dw = dWidth/2 - 1;
  dh = dHeight/2 - 1;
  for ( i = 0; i <= dh; ++i )
   {
    pDest = ( unsigned char * )aDestU + i*dWidth/2;//->ScanLine[i];
    y = i * sh / dh;
    N = dh - i * sh % dh;
    pLinePrev = ( unsigned char * )aSrcU + y*sWidth/2;//->ScanLine[y++];
    pLineNext = ( N == dh ) ? pLinePrev : ( unsigned char * )aSrcU + y*sWidth/2;//->ScanLine[y];
    for ( j = 0; j <= dw; j+=4 )
     {
      x = j * sw / dw;// * nPixelSize;
      B = dw - j * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+1) * sw / dw;// * nPixelSize;
      B = dw - (j+1) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+2) * sw / dw;// * nPixelSize;
      B = dw - (j+2) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+3) * sw / dw;// * nPixelSize;
      B = dw - (j+3) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

/*      x = (j+4) * sw / dw;// * nPixelSize;
      B = dw - (j+4) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+5) * sw / dw;// * nPixelSize;
      B = dw - (j+5) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+6) * sw / dw;// * nPixelSize;
      B = dw - (j+6) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+7) * sw / dw;// * nPixelSize;
      B = dw - (j+7) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
//      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );           */
     }
   }

  //V
  sw = sWidth/2 - 1;
  sh = sHeight/2 - 1;
  dw = dWidth/2 - 1;
  dh = dHeight/2 - 1;
  for ( i = 0; i <= dh; ++i )
   {
    pDest = ( unsigned char * )aDestV + i*dWidth/2;//->ScanLine[i];
    y = i * sh / dh;
    N = dh - i * sh % dh;
    pLinePrev = ( unsigned char * )aSrcV + y*sWidth/2;//->ScanLine[y++];
    pLineNext = ( N == dh ) ? pLinePrev : ( unsigned char * )aSrcV + y*sWidth/2;//->ScanLine[y];
    for ( j = 0; j <= dw; j+=4 )
     {
      x = j * sw / dw;// * nPixelSize;
      B = dw - j * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+1) * sw / dw;// * nPixelSize;
      B = dw - (j+1) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+2) * sw / dw;// * nPixelSize;
      B = dw - (j+2) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );

      x = (j+3) * sw / dw;// * nPixelSize;
      B = dw - (j+3) * sw % dw;
      pA = pLinePrev + x;
      pB = pA + nPixelSize;
      pC = pLineNext + x;
      pD = pC + nPixelSize;
      if ( B == dw )
       {
        pB = pA;
        pD = pC;
       }
      for ( k = 0; k < nPixelSize; ++k )
        *pDest++ = ( unsigned char )( int )(
        ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
        + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
        + dw * dh / 2 ) / ( dw * dh )
       );
     }
   }
  return 0;
}
//---------------------------------------------------------------------------
//存储照片，参数1－文件名  参数2－JPG图像质量  参数3－YUV缓存  参数4－图像宽度  参数5－图像高度
void WriteYuvToJpg(char * filename, int quality, unsigned char * image_buffer,
                              int image_width, int image_height)
{
  unsigned char *rgb_data;
	  //初始化转换表
  InitConvertTable();
  rgb_data = (unsigned char *)malloc(image_width*image_height*3);
  ConvertYUV2RGB(image_buffer,
                 image_buffer + image_width*image_height,
                 image_buffer + image_width*image_height*5/4,
                 rgb_data,
                 image_width, image_height);
  write_JPEG_file (filename, 80, rgb_data,
                 image_width, image_height);
  free(rgb_data);
}
//---------------------------------------------------------------------------
