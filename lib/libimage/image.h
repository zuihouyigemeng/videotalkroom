#ifndef IMAGE_H
#define IMAGE_H

#define VIDEO_PICTURE_QUEUE_SIZE_MAX 20

#include <inttypes.h>

//按钮
struct TImageButton{
               char jpgname_up[80];
               char jpgname_down[80];
               char buttonname[20];
               unsigned char *image[2]; //0--up  1--down
               int width;
               int height;
               int xLeft;
               int yTop;
               uint8_t Visible;
               int EventType;
              };
struct TImageGif{
               int framenum; //总帧数
               int currframe; //当前帧
               int time;      //帧间隔时间  ms
               unsigned char *image[10];
               int width;
               int height;
               int xLeft;
               int yTop;
               uint8_t Visible;
              };
struct TImage{
               unsigned char *image;
               unsigned char *image_down;
               int width;
               int height;
               int xLeft;
               int yTop;
               uint8_t Visible;
              };
struct TLabel{
               unsigned char *image;      //正常
               unsigned char *image_h;    //高亮
               int width;
               int height;
               int xLeft;
               int yTop;
               char Text[100];
               uint8_t Visible;
              };
struct TEdit{
               unsigned char *image;
               int width;
               int height;
               int xLeft;
               int yTop;
               int CursorX; //光标X坐标
               int CursorY; //光标Y坐标
               int CursorHeight; //光标高度
               int CursorCorlor;
               int fWidth;     //字宽度
               unsigned char *Cursor_H;     //光标
               char Text[20];
               int BoxLen;         //文本框当前输入长度
               int MaxLen;         //文本框最大输入长度
               uint8_t Focus;        //显示输入光标
               uint8_t Visible;
              };
struct TPopupWin{
               unsigned char *image[2]; //0 --图像  1--背景
               int width;
               int height;
               int xLeft;
               int yTop;
               uint8_t Visible;
              };

struct flcd_data
{
    unsigned int buf_len;
    unsigned int uv_offset;
    unsigned int frame_no;
    unsigned int mp4_map_dma[VIDEO_PICTURE_QUEUE_SIZE_MAX];
};
/*初始化
  fbmem - framebuffer 首页地址
  fb_width - framebuffer 宽度
  fb_height - framebuffer 高度
  f_data - flcd_data 结构
  return - 1:初始化成功  0:初始化失败
*/
int InitLibImage(unsigned char *fbmem, int fb_width, int fb_height, struct flcd_data *f_data);
/*解码一JPG文件并显示到framebuffer上
  xleft - 图像 x 座标
  ytop - 图像 y 座标
  jpegname - 文件名称（linux只支持绝对路径，不支持相对路径）
  true_size - 1: 按图像实际大小显示  0: 显示到一个小的区域内（不是缩放显示，是截取左上部分显示）
  rect_width、rect_height -  true_size = 0 时有效，小区域的宽度和高度
  PageNo - 将该图像显示到framebuffer的第几页（0为第一页）
  return - 1:成功  0:失败
  注：由于该函数需要先解码JPG，然后显示，故速度较慢，适合于显示一些不常用的图像，
      如要显示一些常用图像，建议采用ImageLoadFromFile和DisplayImage来显示
      由于解码的原因，JPG图像请用ACDsee软件转化，JPG图像的高度需为16的倍数，否则可能引起程序异常
*/
int DisplayJPG(int xleft, int ytop, char *jpegname, int true_size, int rect_width, int rect_height, int PageNo);

/*从JPG文件中截取图像缓冲, pic_buf 为YUV420格式
  jpegname - 文件名称（linux只支持绝对路径，不支持相对路径）
  rect_width - 图像宽度
  rect_height - 图像高度
  pic_buf - 缓冲区地址
  return - 1:成功  0:失败
  注：由于解码的原因，JPG图像请用ACDsee软件转化，JPG图像的高度需为16的倍数，否则可能引起程序异常
*/
int CapturePicFromJPG(char *jpegname, int rect_width, int rect_height, uint8_t *pic_buf);

/*从JPG文件中加载Button图形按键
  jpegname - 文件名称（linux只支持绝对路径，不支持相对路径）
  t_button - 按键Button结构
  buttontype - 按键Button 图像的类型，0 : 正常图像  1 : 按下图像
  return - 1:成功  0:失败
  注：由于解码的原因，JPG图像请用ACDsee软件转化，JPG图像的高度需为16的倍数，否则可能引起程序异常  
*/
int ImageButtonLoadFromFile(char *jpegname, struct TImageButton *t_button, uint8_t buttontype);

/*显示图形按键
  t_button - 按键Button结构
  buttontype - 按键Button 图像的类型，0 : 正常图像  1 : 按下图像
  PageNo - 将该图像显示到framebuffer的第几页（0为第一页）
  return - 1:成功  0:失败
*/
int DisplayImageButton(struct TImageButton *t_button, uint8_t buttontype, int PageNo);

//释放图形按键缓存
//  return - 1:成功  0:失败
int FreeImageButton(struct TImageButton *t_button);

//从JPG文件中加载Gif
//  return - 1:成功  0:失败
int GifLoadFromFile(char *jpegname, struct TImageGif *t_gif);
//显示Gif
//  return - 1:成功  0:失败
int DisplayGif(struct TImageGif *t_gif, int PageNo);
//释放Gif
//  return - 1:成功  0:失败
int FreeGif(struct TImageGif *t_gif);

/*从JPG文件中加载Image图形
  jpegname - 文件名称（linux只支持绝对路径，不支持相对路径）
  t_image - Image图形结构
  return - 1:成功  0:失败
  注：由于解码的原因，JPG图像请用ACDsee软件转化，JPG图像的高度需为16的倍数，否则可能引起程序异常
*/
int ImageLoadFromFile(char *jpegname, struct TImage *t_image);

/*显示Image
  PageNo - 将该图像显示到framebuffer的第几页（0为第一页）
  return - 1:成功  0:失败
*/
int DisplayImage(struct TImage *t_image, int PageNo);

//释放Image
//  return - 1:成功  0:失败
int FreeImage(struct TImage *t_image);

/*从JPG文件中加载Edit文本框
  jpegname - 文件名称（linux只支持绝对路径，不支持相对路径）
  t_edit - Edit文本框结构
  return - 1:成功  0:失败
  注：由于解码的原因，JPG图像请用ACDsee软件转化，JPG图像的高度需为16的倍数，否则可能引起程序异常
*/
int EditLoadFromFile(char *jpegname, struct TEdit *t_edit);
//显示Edit文本框
//  return - 1:成功  0:失败
int DisplayEdit(struct TEdit *t_edit, int PageNo);
//释放缓存
//  return - 1:成功  0:失败
int FreeEdit(struct TEdit *t_edit);

/*显示弹出窗口
  t_popupwin - PopupWin弹出窗口结构
  w_type - 0 : 显示  1 : 隐藏
  PageNo - 将该图像显示到framebuffer的第几页（0为第一页）
  return - 1:成功  0:失败
*/
int DisplayPopupWin(struct TPopupWin *t_popupwin, uint8_t w_type, int PageNo);

/*从屏幕上保存一块区域到缓冲区
  xleft - 屏幕 x 座标
  ytop - 屏幕 y 座标
  nwidth - 缓冲区宽度
  nheight - 缓冲区高度
  PicBuf - 缓冲区地址
  PageNo - 屏幕是framebuffer的第几页
  return - 1:成功  0:失败
*/
int SavePicBuf_Func(int xleft, int ytop, int nwidth, int nheight, unsigned char *PicBuf, int PageNo);

/*写一块缓冲区到屏幕
  xleft - 屏幕 x 座标
  ytop - 屏幕 y 座标
  nwidth - 缓冲区宽度
  nheight - 缓冲区高度
  PicBuf - 缓冲区地址
  PageNo - 屏幕是framebuffer的第几页
  return - 1:成功  0:失败
*/
int RestorePicBuf_Func(int xleft, int ytop,
                   int nwidth, int nheight, unsigned char *PicBuf, int PageNo);

/*从一个缓存区保存一块区域到另一个缓冲区
  xleft -  x 座标
  ytop -  y 座标
  dwidth - 目标(新)缓冲区宽度
  dheight - 目标(新)缓冲区高度
  dest_PicBuf - 目标(新)缓冲区地址
  swidth - 源(旧)缓冲区宽度
  sheight - 源(旧)缓冲区高度
  src_PicBuf - 源(旧)缓冲区地址
  return - 1:成功  0:失败
  注：目标(新)缓冲区宽度、高度需小于或等于源(旧)缓冲区宽度、高度
*/
int SavePicS_D_Func(int xleft, int ytop, int dwidth, int dheight,
              unsigned char *dest_PicBuf,int swidth, int sheight, unsigned char *src_PicBuf);
#endif /* IMAGE_H */
