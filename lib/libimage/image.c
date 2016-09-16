#include <stdio.h>
#include "image.h"
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

struct ImageStrc1{
     unsigned char *fbmem;
     int fb_width;
     int fb_height;
     int buf_len;
     int uv_offset;
     }ImageStrc;
//---------------------------------------------------------------------------
int InitLibImage(unsigned char *fbmem, int fb_width, int fb_height, struct flcd_data *f_data)
{
  ImageStrc.fbmem = fbmem;
  ImageStrc.fb_width = fb_width;
  ImageStrc.fb_height = fb_height;
  ImageStrc.buf_len = f_data->buf_len;
  ImageStrc.uv_offset = f_data->uv_offset;

  return 1;
}
//---------------------------------------------------------------------------
int DisplayJPG(int xleft, int ytop, char *jpegname, int true_size, int rect_width, int rect_height, int PageNo)
{
    //set the default value
  int i,j;
  int rect1width,rect1height;
  parameters_t param;
  uint8_t *yuv[3];
  unsigned char *rgb24data;

  strcpy(param.jpegformatstr, "jpeg");
  param.begin = 0;
  param.numframes = -1;
//  param.framerate = y4m_fps_UNKNOWN;
//  param.interlace = Y4M_UNKNOWN;
  param.interleave = -1;
  param.verbose = 1;
  param.loop = 1;
  param.rescale_YUV = 1;
  if (init_parse_files(&param, jpegname)) {
    printf("* Error processing the JPEG input.");
    return 0;
  }

//  yuv[0] = malloc(param.width * param.height * sizeof(yuv[0][0]));
//  yuv[1] = malloc(param.width * param.height / 4 * sizeof(yuv[1][0]));
//  yuv[2] = malloc(param.width * param.height / 4 * sizeof(yuv[2][0]));
  yuv[0] = (unsigned char *)malloc(param.width * param.height);
  yuv[1] = (unsigned char *)malloc(param.width * param.height / 4);
  yuv[2] = (unsigned char *)malloc(param.width * param.height / 4);


  if (generate_YUV4MPEG(&param,jpegname, 0, 0, yuv))
//  if(Decode_Jpeg2YUV(jpegname, param.width, param.height, yuv) == 0)
   {
    printf("* Error processing the input files.");
    return 0;
   } 
  if(true_size == 1)
   {
    rect1width = param.width;
    rect1height = param.height;
   }
  else
   {
    rect1width = rect_width;
    rect1height = rect_height;
   }
  //要显示的尺寸必须小于图像实际尺寸 
  if(rect1width > param.width)
    rect1width = param.width;
  if(rect1height > param.height)
    rect1height = param.height;
  //    printf("seg 38\n");
  for(j=0;j<rect1height;j++)
    memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo+(j+ytop)*ImageStrc.fb_width+xleft, yuv[0]+j*rect1width, rect1width);

  for(j=0;j<rect1height/2;j++)
    memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+ytop/2)*ImageStrc.fb_width/2 +xleft/2,
           yuv[1]+j*rect1width/2, rect1width/2);

  for(j=0;j<rect1height/2;j++)
    memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+ytop/2)*ImageStrc.fb_width/2 +xleft/2,
           yuv[2]+j*rect1width/2, rect1width/2);

  free(yuv[0]);
  free(yuv[1]);
  free(yuv[2]);
//  getchar();

  return 1;
}
//---------------------------------------------------------------------------
//从JPG图像中截取图像缓冲
int CapturePicFromJPG(char *jpegname, int rect_width, int rect_height,uint8_t *pic_buf)
{
    //set the default value
  int i,j;
  int rect1width,rect1height;
  parameters_t param;
  uint8_t *yuv[3];
//  param.jpegformatstr = strdup("jpeg");
  strcpy(param.jpegformatstr, "jpeg");
  param.begin = 0;
  param.numframes = -1;
//  param.framerate = y4m_fps_UNKNOWN;
//  param.interlace = Y4M_UNKNOWN;
  param.interleave = -1;
  param.verbose = 1;
  param.loop = 1;
  param.rescale_YUV = 1;
  if (init_parse_files(&param, jpegname)) {
    printf("* Error processing the JPEG input.");
    return 0;
  }
  yuv[0] = pic_buf;
  yuv[1] = pic_buf + param.width * param.height;
  yuv[2] = pic_buf + param.width * param.height *5/ 4 ;
  if (generate_YUV4MPEG(&param,jpegname, rect_width, rect_height, yuv))
//  if(Decode_Jpeg2YUV(jpegname, param.width, param.height, yuv) == 0)
   {
    printf("* Error processing the input files.");
    return 0;
   }

  return 1;
}
//---------------------------------------------------------------------------
//从JPG文件中加载图像缓冲
int ImageButtonLoadFromFile(char *jpegname, struct TImageButton *t_button, uint8_t buttontype)
{
    //set the default value
  int i,j;
  int rect1width,rect1height;
  parameters_t param;
  uint8_t *yuv[3];
//  param.jpegformatstr = strdup("jpeg");
  strcpy(param.jpegformatstr, "jpeg");
  param.begin = 0;
  param.numframes = -1;
//  param.framerate = y4m_fps_UNKNOWN;
//  param.interlace = Y4M_UNKNOWN;
  param.interleave = -1;
  param.verbose = 1;
  param.loop = 1;
  param.rescale_YUV = 1;
  if(t_button->image[buttontype] == NULL)
   {
    if (init_parse_files(&param, jpegname))
     {
      printf("* Error processing the JPEG input.");
      return 0;
     }

    t_button->image[buttontype] = (unsigned char *)malloc(param.width*param.height*3/2);

    yuv[0] = t_button->image[buttontype];
    yuv[1] = t_button->image[buttontype] + param.width * param.height;
    yuv[2] = t_button->image[buttontype] + param.width * param.height *5/ 4 ;
    t_button->width = param.width;
    t_button->height = param.height;
    if (generate_YUV4MPEG(&param,jpegname, param.width, param.height, yuv))
//    if(Decode_Jpeg2YUV(jpegname, param.width, param.height, yuv) == 0)
     {
      printf("* Error processing the input files.");
      return 0;
     }
   }
  return 1;
}
//---------------------------------------------------------------------------
//从JPG文件中加载Gif
int GifLoadFromFile(char *jpegname, struct TImageGif *t_gif)
{
    //set the default value
  int i,j;
  int rect1width,rect1height;
  char giffilename[80];
  parameters_t param;
  uint8_t *yuv[3];
//  param.jpegformatstr = strdup("jpeg");
  strcpy(param.jpegformatstr, "jpeg");
  param.begin = 0;
  param.numframes = -1;
//  param.framerate = y4m_fps_UNKNOWN;
//  param.interlace = Y4M_UNKNOWN;
  param.interleave = -1;
  param.verbose = 1;
  param.loop = 1;
  param.rescale_YUV = 1;


  for(i=0; i<t_gif->framenum; i++)
   {
    strcpy(giffilename, jpegname);
    sprintf(giffilename, "%s%d.jpg\0", jpegname, i+1);
    if(t_gif->image[i] == NULL)
     {
      if (init_parse_files(&param, giffilename))
       {
        printf("* Error processing the JPEG input.");
        return 0;
       }
      t_gif->image[i] = (unsigned char *)malloc(param.width*param.height*3/2);

      yuv[0] = t_gif->image[i];
      yuv[1] = t_gif->image[i] + param.width * param.height;
      yuv[2] = t_gif->image[i] + param.width * param.height *5/ 4 ;
      t_gif->width = param.width;
      t_gif->height = param.height;
      if (generate_YUV4MPEG(&param,giffilename, param.width, param.height, yuv))
//      if(Decode_Jpeg2YUV(giffilename, param.width, param.height, yuv) == 0)
       {
        printf("* Error processing the input files.");
        return 0;
       }
     }
   }
  return 1;
}
//---------------------------------------------------------------------------
//显示Gif
int DisplayGif(struct TImageGif *t_gif, int PageNo)
{
  int j;
  if(t_gif->image[t_gif->currframe] != NULL)
   {
    for(j=0;j<t_gif->height;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo+(j+t_gif->yTop)*ImageStrc.fb_width+t_gif->xLeft,
             t_gif->image[t_gif->currframe]+j*t_gif->width, t_gif->width);

    for(j=0;j<t_gif->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+t_gif->yTop/2)*ImageStrc.fb_width/2 +t_gif->xLeft/2,
             t_gif->image[t_gif->currframe]+t_gif->width*t_gif->height+j*t_gif->width/2, t_gif->width/2);

    for(j=0;j<t_gif->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+t_gif->yTop/2)*ImageStrc.fb_width/2 +t_gif->xLeft/2,
             t_gif->image[t_gif->currframe]+t_gif->width*t_gif->height*5/4+j*t_gif->width/2, t_gif->width/2);
   }
  return 1;
}
//---------------------------------------------------------------------------
//释放Gif
int FreeGif(struct TImageGif *t_gif)
{
  int i;
  for(i=0; i<t_gif->framenum; i++)
   {
    if(t_gif->image[i] != NULL)
     {
      free(t_gif->image[i]);
      t_gif->image[i] = NULL;
     }
    else
      return 0;
   }
  return 1;
}
//---------------------------------------------------------------------------
//从JPG文件中加载Edit
int EditLoadFromFile(char *jpegname, struct TEdit *t_edit)
{
    //set the default value
  int i,j;
  int rect1width,rect1height;
  parameters_t param;
  uint8_t *yuv[3];
//  param.jpegformatstr = strdup("jpeg");
  strcpy(param.jpegformatstr, "jpeg");
  param.begin = 0;
  param.numframes = -1;
//  param.framerate = y4m_fps_UNKNOWN;
//  param.interlace = Y4M_UNKNOWN;
  param.interleave = -1;
  param.verbose = 1;
  param.loop = 1;
  param.rescale_YUV = 1;
  if(t_edit->image == NULL)
   {
    if (init_parse_files(&param, jpegname))
     {
      printf("* Error processing the JPEG input.");
      return 0;
     }
    t_edit->image = (unsigned char *)malloc(param.width*param.height*3/2);

    yuv[0] = t_edit->image;
    yuv[1] = t_edit->image + param.width * param.height;
    yuv[2] = t_edit->image + param.width * param.height *5/ 4 ;
    t_edit->width = param.width;
    t_edit->height = param.height;
    if (generate_YUV4MPEG(&param,jpegname, param.width, param.height, yuv))
//    if(Decode_Jpeg2YUV(jpegname, param.width, param.height, yuv) == 0)
     {
      printf("* Error processing the input files.");
      return 0;
     }
   }
  return 1;
}
//---------------------------------------------------------------------------
//显示Edit
int DisplayEdit(struct TEdit *t_edit, int PageNo)
{
  int j;
  if(t_edit->image != NULL)
   {
    for(j=0;j<t_edit->height;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo+(j+t_edit->yTop)*ImageStrc.fb_width+t_edit->xLeft, t_edit->image+j*t_edit->width, t_edit->width);

    for(j=0;j<t_edit->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+t_edit->yTop/2)*ImageStrc.fb_width/2 +t_edit->xLeft/2,
             t_edit->image+t_edit->width*t_edit->height+j*t_edit->width/2, t_edit->width/2);

    for(j=0;j<t_edit->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+t_edit->yTop/2)*ImageStrc.fb_width/2 +t_edit->xLeft/2,
             t_edit->image+t_edit->width*t_edit->height*5/4+j*t_edit->width/2, t_edit->width/2);
   }
  else
    return 0;
  return 1;
}
//---------------------------------------------------------------------------
//释放Edit
int FreeEdit(struct TEdit *t_edit)
{
  int i;
  if(t_edit->image == NULL)
    printf("t_edit is fail\n");
  if(t_edit->image != NULL)
   {
    free(t_edit->image);
    t_edit->image = NULL;
   }
  else
    return 0;
  return 1;
}
//---------------------------------------------------------------------------
//显示弹出窗口
int DisplayPopupWin(struct TPopupWin *t_popupwin, uint8_t w_type, int PageNo)
{
  int j;
  if(t_popupwin->image[w_type] != NULL)
   {
    for(j=0;j<t_popupwin->height;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo+(j+t_popupwin->yTop)*ImageStrc.fb_width+t_popupwin->xLeft, t_popupwin->image[w_type]+j*t_popupwin->width, t_popupwin->width);

    for(j=0;j<t_popupwin->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+t_popupwin->yTop/2)*ImageStrc.fb_width/2 +t_popupwin->xLeft/2,
             t_popupwin->image[w_type]+t_popupwin->width*t_popupwin->height+j*t_popupwin->width/2, t_popupwin->width/2);

    for(j=0;j<t_popupwin->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+t_popupwin->yTop/2)*ImageStrc.fb_width/2 +t_popupwin->xLeft/2,
             t_popupwin->image[w_type]+t_popupwin->width*t_popupwin->height*5/4+j*t_popupwin->width/2, t_popupwin->width/2);
   }
  else
    return 0;
  return 1;
}
//---------------------------------------------------------------------------
//从JPG文件中加载Image
int ImageLoadFromFile(char *jpegname, struct TImage *t_image)
{
    //set the default value
  int i,j;
  int rect1width,rect1height;
  parameters_t param;
  uint8_t *yuv[3];
//  param.jpegformatstr = strdup("jpeg");
  strcpy(param.jpegformatstr, "jpeg");
  param.begin = 0;
  param.numframes = -1;
//  param.framerate = y4m_fps_UNKNOWN;
//  param.interlace = Y4M_UNKNOWN;
  param.interleave = -1;
  param.verbose = 1;
  param.loop = 1;
  param.rescale_YUV = 1;
  if(t_image->image == NULL)
   {
    if (init_parse_files(&param, jpegname))
     {
      printf("* Error processing the JPEG input.");
      return 0;
     }
    t_image->image = (unsigned char *)malloc(param.width*param.height*3/2);

    yuv[0] = t_image->image;
    yuv[1] = t_image->image + param.width * param.height;
    yuv[2] = t_image->image + param.width * param.height *5/ 4 ;
    t_image->width = param.width;
    t_image->height = param.height;
    if (generate_YUV4MPEG(&param,jpegname, param.width, param.height, yuv))
//    if(Decode_Jpeg2YUV(jpegname, param.width, param.height, yuv) == 0)
     {
      printf("* Error processing the input files.");
      return 0;
     }
   }
  return 1;
}
//---------------------------------------------------------------------------
//显示Image
int DisplayImage(struct TImage *t_image, int PageNo)
{
  int j;
  if(t_image->image != NULL)
   {
    for(j=0;j<t_image->height;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo+(j+t_image->yTop)*ImageStrc.fb_width+t_image->xLeft, t_image->image+j*t_image->width, t_image->width);

    for(j=0;j<t_image->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+t_image->yTop/2)*ImageStrc.fb_width/2 +t_image->xLeft/2,
             t_image->image+t_image->width*t_image->height+j*t_image->width/2, t_image->width/2);

    for(j=0;j<t_image->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+t_image->yTop/2)*ImageStrc.fb_width/2 +t_image->xLeft/2,
             t_image->image+t_image->width*t_image->height*5/4+j*t_image->width/2, t_image->width/2);
   }
  else
    return 0;
  return 1;
}
//---------------------------------------------------------------------------
//释放Image
int FreeImage(struct TImage *t_image)
{
  int i;
  if(t_image->image != NULL)
   {
    free(t_image->image);
    t_image->image = NULL;
   }
  else
    return 0;
  return 1;
}
//---------------------------------------------------------------------------
//显示图形按键
int DisplayImageButton(struct TImageButton *t_button, uint8_t buttontype, int PageNo)
{
  int j;
  if(t_button->image[buttontype] != NULL)
   {
    t_button->Visible = 1;
    for(j=0;j<t_button->height;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo+(j+t_button->yTop)*ImageStrc.fb_width+t_button->xLeft,
             t_button->image[buttontype]+j*t_button->width, t_button->width);

    for(j=0;j<t_button->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+t_button->yTop/2)*ImageStrc.fb_width/2 +t_button->xLeft/2,
             t_button->image[buttontype]+t_button->width*t_button->height+j*t_button->width/2, t_button->width/2);

    for(j=0;j<t_button->height/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+t_button->yTop/2)*ImageStrc.fb_width/2 +t_button->xLeft/2,
             t_button->image[buttontype]+t_button->width*t_button->height*5/4+j*t_button->width/2, t_button->width/2);
   }
  else
    return 0;
  return 1;
}
//---------------------------------------------------------------------------
//释放缓存
int FreeImageButton(struct TImageButton *t_button)
{
  int i;
  if(t_button->image[0] == NULL)
    printf("is fail\n");
  for(i=0; i<2; i++)
   if(t_button->image[i] != NULL)
    {
     free(t_button->image[i]);
     t_button->image[i] = NULL;
    }
  return 1;
}
//---------------------------------------------------------------------------
//从一个缓存区保存一块
int SavePicS_D_Func(int xleft, int ytop, int dwidth, int dheight,
              unsigned char *dest_PicBuf,int swidth, int sheight, unsigned char *src_PicBuf)
{
  int j;
  if(dest_PicBuf == NULL)
    return 0;
  if(src_PicBuf == NULL)
    return 0;
  if((dwidth > swidth)||(dheight > sheight))
    return 0;
  for(j=0;j<dheight;j++)
    memcpy(dest_PicBuf+j*dwidth, src_PicBuf+(j+ytop)*swidth+xleft, dwidth);
  for(j=0;j<dheight/2;j++)
    memcpy(dest_PicBuf+dwidth*dheight+j*dwidth/2,
           src_PicBuf + swidth*sheight + (j+ytop/2)*swidth/2 +xleft/2, dwidth/2);
  for(j=0;j<dheight/2;j++)
    memcpy(dest_PicBuf+dwidth*dheight*5/4+j*dwidth/2,
           src_PicBuf + swidth*sheight*5/4 + (j+ytop/2)*swidth/2 +xleft/2, dwidth/2);
  return 1;
}
//---------------------------------------------------------------------------
//保存一块屏幕
int SavePicBuf_Func(int xleft, int ytop, int nwidth, int nheight, unsigned char *PicBuf, int PageNo)
{
  int j;
  for(j=0;j<nheight;j++)
    memcpy(PicBuf+j*nwidth, ImageStrc.fbmem + ImageStrc.buf_len*PageNo +(j+ytop)*ImageStrc.fb_width+xleft, nwidth);

  for(j=0;j<nheight/2;j++)
    memcpy(PicBuf+nwidth*nheight+j*nwidth/2,
           ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+ytop/2)*ImageStrc.fb_width/2 +xleft/2, nwidth/2);

  for(j=0;j<nheight/2;j++)
    memcpy(PicBuf+nwidth*nheight*5/4+j*nwidth/2,
           ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+ytop/2)*ImageStrc.fb_width/2 +xleft/2, nwidth/2);
  return 1;
}
//---------------------------------------------------------------------------
//恢复一块屏幕
int RestorePicBuf_Func(int xleft, int ytop,
                   int nwidth, int nheight, unsigned char *PicBuf, int PageNo)
{
  int j;
  if(PicBuf != NULL)
   {
    for(j=0;j<nheight;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo+(j+ytop)*ImageStrc.fb_width+xleft, PicBuf+j*nwidth, nwidth);

    for(j=0;j<nheight/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset + (j+ytop/2)*ImageStrc.fb_width/2 +xleft/2,
             PicBuf+nwidth*nheight+j*nwidth/2, nwidth/2);

    for(j=0;j<nheight/2;j++)
      memcpy(ImageStrc.fbmem + ImageStrc.buf_len*PageNo + ImageStrc.uv_offset+ImageStrc.uv_offset/4 + (j+ytop/2)*ImageStrc.fb_width/2 +xleft/2,
             PicBuf+nwidth*nheight*5/4+j*nwidth/2, nwidth/2);
   }
  else
    printf("PicBuf is null \n");
  return 1;
}
//---------------------------------------------------------------------------



