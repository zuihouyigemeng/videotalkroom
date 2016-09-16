#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>       //sem_t

#define CommonH
#include "common.h"

void DisplayCancelFortifyWindow(void);    //显示撤防窗口（二级）
void OperateCancelFortifyWindow(short wintype, int currwindow);    //撤防窗口操作（二级）
void DisplayFortifyWindow(void);    //显示布防窗口（二级）
void OperateFortifyWindow(short wintype, int currwindow);    //布防窗口操作（二级）
void DisplayAlarmWindow(void);  //显示报警窗口（二级）
void OperateAlarmWindow(short wintype, int currwindow);    //报警窗口操作（二级）
void DisplayCancelAlarmWindow(void);    //显示取消报警窗口（二级）
void OperateCancelAlarmWindow(short wintype, int currwindow);    //取消报警窗口操作（二级）
//---------------------------------------------------------------------------
void DisplayCancelFortifyWindow(void)    //显示撤防窗口（二级）
{
  int yTop;
  int i;
  char jpgfilename[80];
  char infonum[20];
  char str[3];
  yTop = 36;

  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"fortify.jpg");
  DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  //存储背景
  SavePicBuf_Func(CancelFortifyWin.xLeft, CancelFortifyWin.yTop, CancelFortifyWin.width, CancelFortifyWin.height, CancelFortifyWin.image[1], 0);
  DisplayPopupWin(&CancelFortifyWin, SHOW, 0);
  Local.TmpWindow = Local.CurrentWindow;
  Local.CurrentWindow = 31;

  outxy24(167, 161, 2, cBlack, 1, 1, "输入密码撤防", 0, 0);

  for(i=0; i<12; i++)
   {
    num1_button[i].xLeft = NumxLeft2[i];
    num1_button[i].yTop = NumyTop2[i];
   }

  cancelfortify_button[0].xLeft = 132;
  cancelfortify_button[0].yTop = 272;

  cancelfortify_button[1].xLeft = 212;
  cancelfortify_button[1].yTop = 272;

  cancelfortify_button[2].xLeft = 290;
  cancelfortify_button[2].yTop = 272;

  cancelfortify_edit.xLeft = 156;
  cancelfortify_edit.yTop = 202;
  cancelfortify_edit.BoxLen = 0;
  cancelfortify_edit.Text[0] = '\0';
  CurrBox = 0;
  //打开光标
  WriteCursor(&cancelfortify_edit, 1, 1, 0);
  DisplayEdit(&cancelfortify_edit, 0);

  for(i=0; i<3; i++)
    DisplayImageButton(&cancelfortify_button[i], IMAGEUP, 0);
  for(i=0; i<12; i++)
    DisplayImageButton(&num1_button[i], IMAGEUP, 0);


  Local.PassLen = 0;

  Local.CurrentWindow = 31;
}
//---------------------------------------------------------------------------
void OperateCancelFortifyWindow(short wintype, int currwindow)    //撤防窗口操作（二级）
{
  char wavFile[80];
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char str[3];
  xLeft = 120;
  yTop = 36;

  if(wintype < 12)
   {
    DisplayImageButton(&num1_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&num1_button[wintype], IMAGEUP, 0);
   }
  if((wintype >= 12)&&(wintype <= 14))
   {
    DisplayImageButton(&cancelfortify_button[wintype-12], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&cancelfortify_button[wintype-12], IMAGEUP, 0);
   }

  //停止光标,并清一下
  WriteCursor(&cancelfortify_edit, 0, 0, 0);
  switch(wintype)
   {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
           if(cancelfortify_edit.BoxLen < cancelfortify_edit.MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             strcat(cancelfortify_edit.Text, str);
             str[0] = '*';
             str[1] = '\0';
             outxy24(cancelfortify_edit.xLeft + cancelfortify_edit.CursorX + cancelfortify_edit.BoxLen*cancelfortify_edit.fWidth,
                     cancelfortify_edit.yTop + cancelfortify_edit.CursorY, 2, cBlack, 1, 1, str, 0, 0);
             cancelfortify_edit.BoxLen ++;
            }
           break;
    case 10: //清除
    case 13: //清除
          cancelfortify_edit.Text[0] = 0;
          cancelfortify_edit.BoxLen = 0;
          DisplayEdit(&cancelfortify_edit, 0);
          break;
    case 11: //确认
    case 12:  //确定
          printf("cancelfortify_edit.BoxLen = %d\n", cancelfortify_edit.BoxLen);
          cancelfortify_edit.Text[cancelfortify_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
          if(strcmp(cancelfortify_edit.Text, LocalCfg.EngineerPass) == 0)
           {
        //     ShowStatusText(0, 0 , 3, cBlack, 1, 1, "撤防成功", 0);
              if((Local.AlarmDelayFlag[0] == 1)||(Local.AlarmDelayFlag[1] == 1))
               {
                StopPlayWavFile();  //关闭报警延时提示音
                usleep(200*1000);  //延时是为了等待声音播放完成，否则会有杂音
               }
              for(i=0; i<2; i++)
               {
                Local.AlarmDelayFlag[i] = 0;     //报警延时标志
                Local.AlarmDelayTime[i] = 0;
               }

              sprintf(wavFile, "%scancelfortify.wav\0", wavPath);
              StartPlayWav(wavFile, 0);


              //清除输入框
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, 0);

              ioctl(gpio_fd, IO_CLEAR, 2);
              LocalCfg.DefenceStatus = 0;
                //布防状态
                if(LocalCfg.DefenceStatus == 0)
                 if(Local.CurrentWindow == 0)
                  DisplayImage(&state_image[1], 0);
                else
                 if(Local.CurrentWindow == 0)
                  DisplayImage(&state_image[0], 0);              
              DisplayFortifyWindow();
           }
          else
             {
              sprintf(wavFile, "%spasserror.wav\0", wavPath);
              StartPlayWav(wavFile, 0);
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, 0);
             }
          break;
    case 14:  //返回
           DisplayMainWindow(0);
           break;
   }
   if(Local.CurrentWindow == 31)
     //打开光标
       WriteCursor(&cancelfortify_edit, 1, 1, 0);
}
//---------------------------------------------------------------------------
void DisplayFortifyWindow(void)    //显示布防窗口（二级）
{
  int yTop;
  int i;
  char jpgfilename[80];
  char infonum[20];
  char str[3];

  if(Local.PrevWindow != 32)
   {
    Local.PrevWindow = Local.CurrentWindow;
   }
  Local.CurrentWindow = 32;

  yTop = 36;
  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"fortify.jpg");
  DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);
  //安防窗口（一级）提示条
  if(fortify_label.image == NULL)
   {
    fortify_label.width = TIPW;
    fortify_label.height = TIPH;
    fortify_label.xLeft = SETUP1X;
    fortify_label.yTop = SETUP1Y;
    fortify_label.image = (unsigned char *)malloc(TIPW*TIPH*3/2);  //保存一块屏幕缓冲
    SavePicBuf_Func(SETUP1X, SETUP1Y, TIPW, TIPH, fortify_label.image, 0);
   }

  for(i=0; i<3; i++)
    DisplayImageButton(&fortify_button[i], IMAGEUP, 0);
}
//---------------------------------------------------------------------------
void OperateFortifyWindow(short wintype, int currwindow)    //布防窗口操作（二级）
{
  char wavFile[80];
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char str[3];
  xLeft = 120;
  yTop = 36;
  printf("wintype = %d\n", wintype);
  printf("DefenceCfg.DefenceStatus = %d\n", LocalCfg.DefenceStatus);
  if((wintype >= 2)&&(wintype < 3))
   {
    DisplayImageButton(&fortify_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&fortify_button[wintype], IMAGEUP, 0);
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
    case 0: //外出布防
           if((LocalCfg.DefenceStatus !=1)&&(LocalCfg.DefenceStatus !=4))
            {
             DisplayImageButton(&fortify_button[0], IMAGEDOWN, 0);
             DisplayImageButton(&fortify_button[1], IMAGEUP, 0);
             usleep(200*1000);
             sprintf(wavFile, "%sfortifydelay.wav\0", wavPath);
             StartPlayWav(wavFile, 0);

             LocalCfg.DefenceStatus = 4;
             Local.DefenceDelayFlag = 1;    //布防延时标志
             Local.DefenceDelayTime = 0;    //计数
            }
           break;
    case 1: //在家布防
           if((LocalCfg.DefenceStatus !=2)&&(LocalCfg.DefenceStatus !=5))
            {
             DisplayImageButton(&fortify_button[0], IMAGEUP, 0);
             DisplayImageButton(&fortify_button[1], IMAGEDOWN, 0);
             usleep(200*1000);
             sprintf(wavFile, "%sfortifydelay.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
                    
             LocalCfg.DefenceStatus = 5;
             Local.DefenceDelayFlag = 1;    //布防延时标志
             Local.DefenceDelayTime = 0;    //计数
            }
           break;
    case 2: //取消布防延时
             ioctl(gpio_fd, IO_CLEAR, 2); 
             LocalCfg.DefenceStatus = 0;
             Local.DefenceDelayFlag = 0;    //布防延时标志
             Local.DefenceDelayTime = 0;    //计数
             for(i=0; i<3; i++)
               DisplayImageButton(&fortify_button[i], IMAGEUP, 0);
           break;
    case 30:  //首页
    //case 31:  //安防
    case 32:  //家电
    case 33:  //对讲
    case 34:  //信息
    case 35:  //设置
           DisplayMainWindow(wintype - 30);
           break;
   }
}
//---------------------------------------------------------------------------
void DisplayAlarmWindow(void)  //显示报警窗口（二级）
{
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char alarm_str[10];
  char tmp_str[10];
  char wavFile[80];
  xLeft = 120;
  yTop = 36;

  PicStatBuf.Flag = 0;

  if(Local.CurrentWindow != 34)
   {
    strcpy(jpgfilename, sPath);
    strcat(jpgfilename,"alarm.jpg");
    DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

    DisplayImageButton(&alarm_button[0], IMAGEUP, 0);

    sprintf(wavFile, "%salarm.wav\0", wavPath);
    StartPlayWav(wavFile, 1);
   }
  for(i=0; i<6; i++)
   if(LocalCfg.DefenceInfo[i][3] != 0)
    {
     sprintf(tmp_str, "%d\0", i+1);
     outxy16(325+16*i, 413, 3, cWhite, 1, 1, tmp_str, 0, 0);
    }
  AlarmGif.Visible = 1;

  Local.CurrentWindow = 34;
}
//---------------------------------------------------------------------------
void OperateAlarmWindow(short wintype, int currwindow)    //报警窗口操作（二级）
{
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char str[3];
  xLeft = 120;
  yTop = 36;
  if(wintype < 1)
   {
    DisplayImageButton(&alarm_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&alarm_button[wintype], IMAGEUP, 0);
   }
  switch(wintype)
   {
    case 0:  //取消报警
           AlarmGif.Visible = 0;
           StopPlayWavFile();  //关闭报警音
           usleep(200*1000);  //延时是为了等待声音播放完成，否则会有杂音
           DisplayCancelAlarmWindow();
           break;
   }
}
//---------------------------------------------------------------------------
void DisplayCancelAlarmWindow(void)    //显示取消报警窗口（二级）
{
  int yTop;
  int i;
  char jpgfilename[80];
  char infonum[20];
  char str[3];
  yTop = 36;

  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"fortify.jpg");
  DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  //存储背景
  SavePicBuf_Func(CancelFortifyWin.xLeft, CancelFortifyWin.yTop, CancelFortifyWin.width, CancelFortifyWin.height, CancelFortifyWin.image[1], 0);
  DisplayPopupWin(&CancelFortifyWin, SHOW, 0);
  Local.TmpWindow = Local.CurrentWindow;
  Local.CurrentWindow = 31;

  outxy24(167, 161, 2, cBlack, 1, 1, "输入密码消警", 0, 0);

  for(i=0; i<12; i++)
   {
    num1_button[i].xLeft = NumxLeft2[i];
    num1_button[i].yTop = NumyTop2[i];
   }

  cancelfortify_button[0].xLeft = 150;
  cancelfortify_button[0].yTop = 272;

  cancelfortify_button[1].xLeft = 272;
  cancelfortify_button[1].yTop = 272;

  cancelfortify_edit.xLeft = 156;
  cancelfortify_edit.yTop = 202;
  cancelfortify_edit.BoxLen = 0;
  cancelfortify_edit.Text[0] = '\0';
  CurrBox = 0;
  //打开光标
  WriteCursor(&cancelfortify_edit, 1, 1, 0);
  DisplayEdit(&cancelfortify_edit, 0);

  for(i=0; i<2; i++)
    DisplayImageButton(&cancelfortify_button[i], IMAGEUP, 0);
  for(i=0; i<12; i++)
    DisplayImageButton(&num1_button[i], IMAGEUP, 0);


  Local.PassLen = 0;

  Local.CurrentWindow = 35;
}
//---------------------------------------------------------------------------
void OperateCancelAlarmWindow(short wintype, int currwindow)    //取消报警窗口操作（二级）
{
  char wavFile[80];
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char str[3];
  int Special;
  char SpecialPass[10];
  xLeft = 120;
  yTop = 36;

  if(wintype < 12)
   {
    DisplayImageButton(&num1_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&num1_button[wintype], IMAGEUP, 0);
   }
  if((wintype >= 12)&&(wintype <= 13))
   {
    DisplayImageButton(&cancelfortify_button[wintype-12], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&cancelfortify_button[wintype-12], IMAGEUP, 0);
   }

  //停止光标,并清一下
  WriteCursor(&cancelfortify_edit, 0, 0, 0);
  switch(wintype)
   {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
           if(cancelfortify_edit.BoxLen < cancelfortify_edit.MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             strcat(cancelfortify_edit.Text, str);
             str[0] = '*';
             str[1] = '\0';
             outxy16(cancelfortify_edit.xLeft + cancelfortify_edit.CursorX + cancelfortify_edit.BoxLen*cancelfortify_edit.fWidth,
                     cancelfortify_edit.yTop + cancelfortify_edit.CursorY, 2, cBlack, 1, 1, str, 0, 0);
             cancelfortify_edit.BoxLen ++;
            }
           break;
    case 10: //清除
    case 13: //清除
          cancelfortify_edit.Text[0] = 0;
          cancelfortify_edit.BoxLen = 0;
          DisplayEdit(&cancelfortify_edit, 0);
          break;
    case 11: //确认
    case 12:  //确定
          printf("cancelfortify_edit.BoxLen = %d\n", cancelfortify_edit.BoxLen);
          cancelfortify_edit.Text[cancelfortify_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
          printf("LocalCfg.EngineerPass = %s,cancelfortify_edit.Text = %s\n", LocalCfg.EngineerPass, cancelfortify_edit.Text);
          Special = atoi(LocalCfg.EngineerPass);
          Special ++;
          if(Special > 9999)
            Special = 0;
          sprintf(SpecialPass, "%04d\0", Special);
          if((strcmp(cancelfortify_edit.Text, LocalCfg.EngineerPass) == 0)||(strcmp(cancelfortify_edit.Text, SpecialPass) == 0))
           {
              for(i=0; i<8; i++)
               for(j=0; j<10; j++)
                LocalCfg.DefenceInfo[i][3] = 0;
        //     ShowStatusText(0, 0 , 3, cBlack, 1, 1, "撤防成功", 0);
              SendCancelAlarmFunc(); //取消报警函数

              sprintf(wavFile, "%scancelalarm.wav\0", wavPath);
              StartPlayWav(wavFile, 0);

              //劫持报警
              if(strcmp(cancelfortify_edit.Text, SpecialPass) == 0)
                SendAlarmFunc(0x01, 0);
              //ioctl(gpio_fd, IO_CLEAR, 4);
              
              //延时清提示信息标志
              PicStatBuf.Type = Local.CurrentWindow;
              PicStatBuf.Time = 0;
              PicStatBuf.Flag = 1;
              //清除输入框
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, 0);

              Local.CurrentWindow = 0;      
              DisplayMainWindow(0);
           }
          else
             {
              sprintf(wavFile, "%spasserror.wav\0", wavPath);
              StartPlayWav(wavFile, 0);

              //清除输入框
              cancelfortify_edit.Text[0] = 0;
              cancelfortify_edit.BoxLen = 0;
              DisplayEdit(&cancelfortify_edit, 0);
             }
          break;
   }
   if(Local.CurrentWindow == 35)
     //打开光标
       WriteCursor(&cancelfortify_edit, 1, 1, 0);
}
//---------------------------------------------------------------------------
