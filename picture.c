#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>       //sem_t

#define CommonH
#include "common.h"
void DisplayPictureWindow(int pictype);    //显示照片窗口（一级）
void OperatePictureWindow(short wintype, int currwindow);    //照片窗口操作（二级）

void ShowPicList(int pictype);    //照片显示方框（一级）  照片列表
void DisplayPicContent(int pictype, int picno);    //照片显示方框（二级）  照片内容
void OperatePicContent(short pictype, int currwindow);    //照片显示方框（二级）  照片内容 操作
void ShowPicContent(int pictype, int picno);    // 照片内容
//写照片配置文件
void WritePicIniFile(int PicType);
//当前信息窗口状态
struct InfoStatus1 PicStatus;

InfoNode1 *CurrPic_Node; //当前信息结点
//照片数量结构
extern struct Info1 PicStrc[2];
// *h保存表头结点的指针，*p指向当前结点的前一个结点，*s指向当前结点
extern InfoNode1 *PicNode_h[2]; //照片

struct displayinfo1
 {
 	 int totalpage;
	  int pageno;
	  int totalrow;
	  char content_row[MAXROW][40];
	  int isDeleted;  //在显示信息窗口时删除了信息
 }displayinfo;
//---------------------------------------------------------------------------
void DisplayPictureWindow(int pictype)    //显示照片窗口（一级）
{
  int i;
  int xTop;
  char jpgfilename[80];
  int InfoTypeX[5] = {29, 29, 513, 513, 513};
  int InfoTypeY[5] = {127, 175, 223, 271, 319};
  int InfoRowX[3] = {47, 47, 47};
  int InfoRowY[3] = {203, 252, 303};
  xTop = 36;
  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"picture.jpg");
  DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  //照片类型
  PicStatus.CurrType = pictype;  //当前照片类型
  PicStatus.CurrWin = 0;    //照片列表
  PicStatus.CurrNo = 0;       //当前照片序号  0---n-1
  PicStrc[PicStatus.CurrType].CurrentInfoPage = 1; //当前照片页为第0页
  PicStrc[PicStatus.CurrType].CurrNo = PicStatus.CurrNo;

  for(i=0; i<3; i++)
   {
    inforow_button[i].xLeft = InfoRowX[i];
    inforow_button[i].yTop = InfoRowY[i];
   }

  ShowPicList(PicStatus.CurrType);
  
  Local.PrevWindow = Local.CurrentWindow;
  Local.CurrentWindow = 21;

  for(i=0; i<3; i++)
    DisplayImageButton(&info_button[i], IMAGEUP, 0);

  ioctl(gpio_fd, IO_CLEAR, 3);
}
//---------------------------------------------------------------------------
void OperatePictureWindow(short wintype, int currwindow)    //照片窗口操作（二级）
{
  int i;
  int TmpPicNo;
  InfoNode1 *tmp_node;
  int numperpage;
  int xLeft, yTop, yHeight;

  if(((wintype >= 0) && (wintype <= 2)))
   {

    DisplayImageButton(&info_button[wintype], IMAGEDOWN , 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&info_button[wintype], IMAGEUP, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }

  if(Local.HavePicRecorded_flag == 1) Local.HavePicRecorded_flag = 0;
  
  switch(wintype)
   {
    case 0://上翻       a
             if(PicStrc[PicStatus.CurrType].CurrNo > 0)
              {
               PicStrc[PicStatus.CurrType].CurrNo --;
               PicStatus.CurrNo = PicStrc[PicStatus.CurrType].CurrNo;
               ShowPicList(PicStatus.CurrType);
              }
           break;
    case 1://下翻     b
             if(PicStrc[PicStatus.CurrType].CurrNo < (PicStrc[PicStatus.CurrType].TotalNum -1))
              {
               PicStrc[PicStatus.CurrType].CurrNo++;
               PicStatus.CurrNo = PicStrc[PicStatus.CurrType].CurrNo;
               ShowPicList(PicStatus.CurrType);
              }
           break;
    case 2://删除           c
              //删除当前信息
             //当前信息结点
             CurrPic_Node=locate_infonode(PicNode_h[PicStatus.CurrType], PicStrc[PicStatus.CurrType].CurrNo + 1);
             if(CurrPic_Node != NULL)
              {
                displayinfo.isDeleted = 1;  //在显示信息窗口时删除了信息
                unlink(CurrPic_Node->Content);  //删除照片文件
                delete_infonode(CurrPic_Node);
                PicStrc[PicStatus.CurrType].TotalNum --;
                //写入文件
                //锁定互斥锁
                pthread_mutex_lock (&Local.save_lock);
                //查找可用存储缓冲并填空
                for(i=0; i<SAVEMAX; i++)
                 if(Save_File_Buff[i].isValid == 0)
                  {
                   Save_File_Buff[i].Type = 7;
                   Save_File_Buff[i].InfoType = PicStatus.CurrType;
                   Save_File_Buff[i].isValid = 1;
                   break;
                  }
                //打开互斥锁
                pthread_mutex_unlock (&Local.save_lock);
                sem_post(&save_file_sem);

                ShowPicList(PicStatus.CurrType);
             }
           break;
    case 4://信息1行           c
    case 5://信息2行           c
    case 6://信息3行           c
           //当前信息序号
             TmpPicNo = (PicStrc[PicStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE+(wintype - 4);
             printf("TmpPicNo = %d, PicStrc[PicStatus.CurrType].TotalNum = %d \n", TmpPicNo,
                    PicStrc[PicStatus.CurrType].TotalNum);
             if(PicStatus.CurrNo == TmpPicNo)
              {
               if(TmpPicNo < (PicStrc[PicStatus.CurrType].TotalNum))
                {
                 PicStrc[PicStatus.CurrType].CurrNo = TmpPicNo;
                 PicStatus.CurrNo = TmpPicNo;
                 DisplayPicContent(PicStatus.CurrType, PicStatus.CurrNo);
                }
              }
             else
              {
               if(TmpPicNo < (PicStrc[PicStatus.CurrType].TotalNum))
                {
                 PicStrc[PicStatus.CurrType].CurrNo = TmpPicNo;
                 PicStatus.CurrNo = TmpPicNo;
                 ShowPicList(PicStatus.CurrType);
                }
              }
           break;
    case 30:  //首页
    case 31:  //安防
    case 32:  //家电
    case 33:  //对讲
    case 34:  //信息
    case 35:  //设置
           DisplayMainWindow(wintype - 30);
           break;
   }
}
//---------------------------------------------------------------------------
void ShowPicList(int pictype)    //照片显示方框（一级）  照片列表
{
  int xLeft,yTop,yHeight;
  int i, j;
  int PageTotalNum;  //当页信息数量
  char jpgfilename[80];
  char str[3];
  InfoNode1 *tmp_node;
  char tmp_con[20];
  int tmp_len;  //预览长度
  int fontcolor;
  int NoInPage;
  xLeft = 120;
  yTop = 36;
  tmp_len = 19;


  PicStatus.CurrWin = 0;    //信息列表
  printf("PicStatus.CurrType  = %d\n", PicStatus.CurrType);
  switch(pictype)
   {
    case 0: //呼叫照片
    case 1: //通话照片
           for(i = 0; i < INFONUMPERPAGE; i++)
              DisplayImageButton(&inforow_button[i], IMAGEUP, 0);
           if(PicStrc[pictype].TotalNum > 0)
            {
             //总页数
             if((PicStrc[pictype].TotalNum % INFONUMPERPAGE) == 0)
               PicStrc[pictype].TotalInfoPage = PicStrc[pictype].TotalNum /INFONUMPERPAGE;
             else
               PicStrc[pictype].TotalInfoPage = PicStrc[pictype].TotalNum /INFONUMPERPAGE + 1;

             xLeft = 300;
             yTop = 50;
             yHeight = 40;
             //当前页
             PicStrc[pictype].CurrentInfoPage = PicStatus.CurrNo /INFONUMPERPAGE + 1;
             if(PicStrc[pictype].CurrentInfoPage < PicStrc[pictype].TotalInfoPage)
               PageTotalNum = INFONUMPERPAGE;
             else
               PageTotalNum = PicStrc[pictype].TotalNum - (PicStrc[pictype].CurrentInfoPage - 1)*INFONUMPERPAGE;
             //当前信息在本页中的位置
             NoInPage = (PicStatus.CurrNo)%INFONUMPERPAGE;

             DisplayImageButton(&inforow_button[NoInPage], IMAGEDOWN, 0);

             for(i = 0; i < PageTotalNum; i++)
              {
               if(i !=  NoInPage)
                 fontcolor = cBlack;
               else
                 fontcolor = cWhite;

               tmp_node=locate_infonode(PicNode_h[pictype], (PicStrc[pictype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);

               //序号
               sprintf(tmp_con, "%02d\0", (PicStrc[pictype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);
               outxy24(inforow_button[i].xLeft+50, inforow_button[i].yTop+10, 1,
                       fontcolor, 1, 1, tmp_con, 0, 0);

               if(tmp_node->Content.Type == 0)
                 outxy24(inforow_button[i].xLeft+100, inforow_button[i].yTop+10, 1,
                         fontcolor, 1, 1, "未接听", 0, 0);
               else
                 outxy24(inforow_button[i].xLeft+100, inforow_button[i].yTop+10, 1,
                         fontcolor, 1, 1, "已接听", 0, 0);
               //时间
               memcpy(tmp_con, tmp_node->Content.Time, tmp_len);
                 tmp_con[tmp_len] = '\0';
               outxy24(inforow_button[i].xLeft+250, inforow_button[i].yTop+10, 1,
                       fontcolor, 1, 1, tmp_con, 0, 0);
              }
            }
           break;
    case 4:  //返回
           break;
   }
}
//---------------------------------------------------------------------------
void DisplayPicContent(int pictype, int picno)   
{
  int i;
  char jpgfilename[80];

  displayinfo.isDeleted = 0;  //在显示信息窗口时删除了信息

  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"picwin.jpg");
  DisplayJPG(175, 22, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  Local.CurrentWindow = 22;

  for(i=0; i<3; i++)
    DisplayImageButton(&picwin_button[i], IMAGEUP, 0);

  PicStatus.CurrWin = 1;    //信息内容

  if(PicStatBuf.PicContent == NULL)
   {
    PicStatBuf.PicContent = (unsigned char *)malloc(82*80*3/2);  //保存一块屏幕缓冲
    SavePicBuf_Func(379, 18, 82, 80, PicStatBuf.PicContent, 0);
   }

  ShowPicContent(pictype, picno);
}
//---------------------------------------------------------------------------
void ShowPicContent(int pictype, int picno)    // 照片内容
{
  int xLeft,yTop,yHeight;
  int i, j;
  int PageTotalNum;  //当页信息数量
  InfoNode1 *tmp_node;
  char tmp_con[50];
  int tmp_len;  //预览长度
  int tmp_row;
  int numperpage;
  char str[3];

  xLeft = 379;
  yTop = 18;
  tmp_len = 20;  

  yHeight = 20;
  if(picno <= PicStrc[pictype].TotalNum - 1)
   {
    printf("picno=%d\n",picno + 1);
    tmp_node=locate_infonode(PicNode_h[pictype], picno + 1);
    //当前信息结点
    CurrPic_Node = tmp_node;

    DisplayJPG(239, 61, tmp_node->Content.Content, 1, SCRWIDTH, SCRHEIGHT, 0);
   }
}
//---------------------------------------------------------------------------
void OperatePicContent(short wintype, int currwindow)    //照片显示方框（二级）  照片内容 操作
{
  int i;
  int TmpInfoNo;
  InfoNode1 *tmp_node;
  int numperpage;
  int xLeft, yTop, yHeight;
  char jpgfilename[80];

  if(((wintype >= 0) && (wintype <= 2)))
   {
    DisplayImageButton(&picwin_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != 22)
      return;
    DisplayImageButton(&picwin_button[wintype], IMAGEUP, 0);
   }

  switch(wintype)
   {
    case 0://信息上翻           o
             if(PicStrc[PicStatus.CurrType].CurrNo > 0)
              {
               PicStrc[PicStatus.CurrType].CurrNo --;
               PicStatus.CurrNo = PicStrc[PicStatus.CurrType].CurrNo;
               ShowPicContent(PicStatus.CurrType, PicStatus.CurrNo);
              }
           break;
    case 1://信息下翻           p
             if(PicStrc[PicStatus.CurrType].CurrNo < (PicStrc[PicStatus.CurrType].TotalNum -1))
              {
               PicStrc[PicStatus.CurrType].CurrNo++;
               PicStatus.CurrNo = PicStrc[PicStatus.CurrType].CurrNo;
               ShowPicContent(PicStatus.CurrType, PicStatus.CurrNo);
              }
           break;
    case 2: //返回             h
          //恢复屏幕
           //DisplayPictureWindow(PicStatus.CurrType);
           strcpy(jpgfilename, sPath);
           strcat(jpgfilename,"picture.jpg");
           DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);


           ShowPicList(PicStatus.CurrType);
  
           Local.CurrentWindow = 21;

           for(i=0; i<3; i++)
             DisplayImageButton(&info_button[i], IMAGEUP, 0);

           break;
   }
}
//---------------------------------------------------------------------------
//写照片配置文件
void WritePicIniFile(int PicType)
{
  FILE *read_fd;
  unsigned char tmpchar[30];
  unsigned char readname[80];

  FILE *pic_fd;
  InfoNode1 *tmp_node;
  int j;
  uint8_t isValid;
  char filename[80];  
  //写照片配置文件
  if(PicType == 0)
    strcpy(filename, picini_name1);
  else
    strcpy(filename, picini_name2);
 // printf("PicType = %d\n", PicType);
  if((pic_fd = fopen(filename, "wb")) == NULL)
    printf("无法打开照片配置文件\n");
  else
   {
    //重写该类信息文件
    tmp_node=PicNode_h[PicType];
    //写有用信息
    for(j = 0; j < PicStrc[PicType].TotalNum; j++)
     {
      tmp_node = tmp_node->rlink;
      if(tmp_node != NULL)
        fwrite(&tmp_node->Content, sizeof(tmp_node->Content), 1, pic_fd);
     }
    fclose(pic_fd);
   }
}
//---------------------------------------------------------------------------

