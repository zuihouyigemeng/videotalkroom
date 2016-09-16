#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>       //sem_t

#define CommonH
#include "common.h"
extern char sPath[80];
extern char NullAddr[21];   //空字符串

void DisplayWiringWindow(void);    //显示家电控制窗口（一级）
void OperateWiringWindow(short wintype, int currwindow);    //家电控制窗口操作（一级）

void DisplaySigWiringWindow(void);    //显示单个控制窗口（二级）
void OperateSigWiringWindow(short wintype, int currwindow);    //单个控制窗口操作（二级）

void DisplaySceneWiringWindow(void);    //显示场景模式窗口（二级）
void OperateSceneWiringWindow(short wintype, int currwindow);    //场景模式窗口操作（二级）
//---------------------------------------------------------------------------
void DisplayWiringWindow(void)    //显示家电控制窗口（一级）
{
  int xTop;
  int i;
  char jpgfilename[80];
  char infonum[20];
  char str[3];
  xTop = 0;//36;

   strcpy(jpgfilename, sPath);
   strcat(jpgfilename,"wiring.jpg");
   DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  Local.PrevWindow = Local.CurrentWindow;
  Local.CurrentWindow = 2;

  for(i=0; i<2; i++)
    DisplayImageButton(&wiring_button[i], IMAGEUP, 0);
}
//---------------------------------------------------------------------------
void OperateWiringWindow(short wintype, int currwindow)    //家电控制窗口操作（一级）
{
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char str[3];
  xLeft = 120;
  yTop = 36;

  if(wintype < 2)
   {
    DisplayImageButton(&wiring_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&wiring_button[wintype], IMAGEUP, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }
  switch(wintype)
   {
    case 0:  //单个控制
           DisplaySigWiringWindow();
           break;
    case 1:  //场景模式
           DisplaySceneWiringWindow();
           break;
    case 30:  //首页
    case 31:  //安防
    //case 32:  //家电
    case 33:  //对讲
    case 34:  //信息
    case 35:  //设置
           if(Local.Status == 0)
             DisplayMainWindow(wintype - 30);
           break;
   }
}
//---------------------------------------------------------------------------
void DisplaySigWiringWindow(void)    //显示单个控制窗口（二级）
{
  int xTop;
  int i;
  char jpgfilename[80];
  char infonum[20];
  char str[3];
  xTop = 0;//36;

  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"single.jpg");
  DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  Local.PrevWindow = Local.CurrentWindow;
  Local.CurrentWindow = 91;

  for(i=0; i<4; i++)
    DisplayImage(&sigwiring_image[i], 0);

  //开关灯
  wiringcrtl_button[0].xLeft = sigwiring_image[0].xLeft - 25;
  wiringcrtl_button[0].yTop = sigwiring_image[0].yTop + 124;
  sigwiring_button[0] = wiringcrtl_button[0];

  wiringcrtl_button[1].xLeft = sigwiring_image[0].xLeft + 64;
  wiringcrtl_button[1].yTop = sigwiring_image[0].yTop + 124;
  sigwiring_button[1] = wiringcrtl_button[1];

  //调光灯
  wiringcrtl_button[3].xLeft = sigwiring_image[1].xLeft - 25;
  wiringcrtl_button[3].yTop = sigwiring_image[1].yTop + 124;
  sigwiring_button[2] = wiringcrtl_button[3];
  wiringcrtl_button[4].xLeft = sigwiring_image[1].xLeft + 64;
  wiringcrtl_button[4].yTop = sigwiring_image[1].yTop + 124;
  sigwiring_button[3] = wiringcrtl_button[4];
  //窗帘
  wiringcrtl_button[0].xLeft = sigwiring_image[2].xLeft - 43;
  wiringcrtl_button[0].yTop = sigwiring_image[2].yTop + 124;
  sigwiring_button[4] = wiringcrtl_button[0];
  wiringcrtl_button[1].xLeft = sigwiring_image[2].xLeft + 77;
  wiringcrtl_button[1].yTop = sigwiring_image[2].yTop + 124;
  sigwiring_button[5] = wiringcrtl_button[1];
  wiringcrtl_button[2].xLeft = sigwiring_image[2].xLeft + 19;
  wiringcrtl_button[2].yTop = sigwiring_image[2].yTop + 124;
  sigwiring_button[6] = wiringcrtl_button[2];
  //空调
  wiringcrtl_button[0].xLeft = sigwiring_image[3].xLeft - 25;
  wiringcrtl_button[0].yTop = sigwiring_image[3].yTop + 124;
  sigwiring_button[7] = wiringcrtl_button[0];
  wiringcrtl_button[1].xLeft = sigwiring_image[3].xLeft + 64;
  wiringcrtl_button[1].yTop = sigwiring_image[3].yTop + 124;
  sigwiring_button[8] = wiringcrtl_button[1];

  for(i=0; i<9; i++)
    DisplayImageButton(&sigwiring_button[i], IMAGEUP, 0);
}
//---------------------------------------------------------------------------
void OperateSigWiringWindow(short wintype, int currwindow)    //单个控制窗口操作（二级）
{
  int i, j;
  if(wintype < 9)
   {
    DisplayImageButton(&sigwiring_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&sigwiring_button[wintype], IMAGEUP, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }
  switch(wintype)
   {
    //开关灯
    case 0:  //开
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x01, 0x01, 0x01, 0x01, 0x01);
           break;
    case 1:  //关
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x02, 0x01, 0x01, 0x01, 0x02);
           break;
    //调光灯
    case 2:  //加
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x04, 0x02, 0x01, 0x02, 10);
           break;
    case 3:  //减
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x05, 0x02, 0x01, 0x02, 10);
           break;
    //窗帘
    case 4:  //开
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x01, 0x03, 0x01, 0x01, 0x01);
           break;
    case 5:  //停
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x01, 0x03, 0x01, 0x01, 0x01);
           break;
    case 6:  //关
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x02, 0x03, 0x01, 0x01, 0x02);
           break;
    //空调
    case 7:  //开
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x01, 0x04, 0x01, 0x01, 0x01);
           break;
    case 8:  //关
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x02, 0x04, 0x01, 0x01, 0x02);
           break;
    case 30:  //首页
    case 31:  //安防
    case 32:  //家电
    case 33:  //对讲
    case 34:  //信息
    case 35:  //设置
           if(Local.Status == 0)
             DisplayMainWindow(wintype - 30);
           break;
   }
}
//---------------------------------------------------------------------------
void DisplaySceneWiringWindow(void)    //显示场景模式窗口（二级）
{
  int xTop;
  int i;
  char jpgfilename[80];
  char infonum[20];
  char str[3];
  xTop = 0;//36;

   strcpy(jpgfilename, sPath);
   strcat(jpgfilename,"scene.jpg");
   DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  Local.PrevWindow = Local.CurrentWindow;
  Local.CurrentWindow = 92;
}
//---------------------------------------------------------------------------
void OperateSceneWiringWindow(short wintype, int currwindow)    //场景模式窗口操作（二级）
{
  int i, j;
  if(wintype < 5)
   {
    DisplayImageButton(&scenewiring_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&scenewiring_button[wintype], IMAGEUP, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }
  switch(wintype)
   {
    case 0:  //会客模式
    case 1:  //就餐模式
    case 2:  //就寝模式
    case 3:  //全开模式
    case 4:  //全关模式
           //norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
           SendHostOrder(0x10, 0x00, wintype + 1, 0x00, 0x01);
           break;
    case 30:  //首页
    case 31:  //安防
    case 32:  //家电
    case 33:  //对讲
    case 34:  //信息
    case 35:  //设置
           if(Local.Status == 0)
             DisplayMainWindow(wintype - 30);
           break;
   }

}
//---------------------------------------------------------------------------

