#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/

#define CommonH
#include "common.h"

#define FALSE  -1
#define TRUE   0

int Comm2fd;  //串口2句柄
int Comm3fd;  //串口3句柄
int Comm4fd;  //串口4句柄
int OpenDev(char *Dev);  //打开串口
int set_speed(int fd, int speed);  //设置波特率
int set_Parity(int fd,int databits,int stopbits,int parity);  //设置参数

int OpenComm(int CommPort,int BautSpeed,int databits,int stopbits,int parity);  //打开串口

short CommRecvFlag=1;
pthread_t comm2_rcvid;
pthread_t comm3_rcvid;
pthread_t comm4_rcvid;
void CreateComm2_RcvThread(int fd);
void CreateComm3_RcvThread(int fd);
void CreateComm4_RcvThread(int fd);
void Comm2_RcvThread(int fd);  //Comm2接收线程函数
void Comm3_RcvThread(int fd);  //Comm3接收线程函数
void Comm4_RcvThread(int fd);  //Comm4接收线程函数
int CommSendBuff(int fd,unsigned char buf[1024],int nlength);
void CloseComm();

int CheckZoomDelayTime(void);  //放大缩小图像时检查延迟
int CheckTouchDelayTime(void);  //触摸屏处理时检查延迟
void PressTouchControFunc(unsigned char*inputbuff);

void TouchScr_Func(unsigned char *inputbuff); //触摸屏输入
void CheckIsTouchMenu(int XLCD, int YLCD); //检查触摸区域在菜单条内
int CheckTSInImage(struct TImage * t_image,int XLCD,int YLCD);
int CheckTSInButton(struct TImageButton *t_button, int XLCD, int YLCD); //检查触摸点是否在该按钮内
int CheckTSInLabel(struct TLabel *t_label, int XLCD, int YLCD); //检查触摸点是否在该Label内
int CheckTSInEdit(struct TEdit *t_edit, int XLCD, int YLCD); //检查触摸点是否在该Edit内
int CheckTSInVideoScreen(int XLCD, int YLCD); //检查触摸点是否在该视频窗口内
int CheckTSInPicScreen(int XLCD, int YLCD); //检查触摸点是否在该照片窗口内
extern int Calib_X[4];
extern int Calib_Y[4];
void SendHostOrder(unsigned char norder, unsigned char TerType, unsigned char TerNo, unsigned char InfoType, unsigned char Info); //发送主动命令
//---------------------------------------------------------------------------
int OpenDev(char *Dev)
{
  int	fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY
  if (FALSE == fd)
   {
    perror("Can't Open Serial Port");
    return FALSE;
   }
  else
    return fd;
}
//---------------------------------------------------------------------------
/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
					B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
					19200,  9600, 4800, 2400, 1200,  300, };
int set_speed(int fd, int speed)
{
  int   i;
  int   status;
  struct termios   Opt;
  tcgetattr(fd, &Opt);
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
   {
    if  (speed == name_arr[i])
     {
      tcflush(fd, TCIOFLUSH);
      cfsetispeed(&Opt, speed_arr[i]);
      cfsetospeed(&Opt, speed_arr[i]);
      status = tcsetattr(fd, TCSANOW, &Opt);
      if  (status != 0)
       {
        perror("tcsetattr fd1");
        return FALSE;
       }
      tcflush(fd,TCIOFLUSH);
     }
   }
  return TRUE;
}
//---------------------------------------------------------------------------
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{
  struct termios options;
  if  ( tcgetattr( fd,&options)  !=  0)
   {
    perror("SetupSerial 1");
    return(FALSE);
   }
  options.c_cflag &= ~CSIZE;
  switch (databits) /*设置数据位数*/
   {
    case 7:
           options.c_cflag |= CS7;
           break;
    case 8:
           options.c_cflag |= CS8;
           break;
    default:
           fprintf(stderr,"Unsupported data size\n"); return (FALSE);
   }
  switch (parity)
   {
    case 'n':
    case 'N':
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
    case 'o':
    case 'O':
		options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
		options.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
    case 'e':
    case 'E':
		options.c_cflag |= PARENB;     /* Enable parity */    
		options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
    case 'S':
    case 's':  /*as no parity*/
       	        options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;break;  
    default:
		fprintf(stderr,"Unsupported parity\n");    
		return (FALSE);  
	}  
/* 设置停止位*/
  switch (stopbits)
   {
	case 1:    
		options.c_cflag &= ~CSTOPB;  
		break;  
	case 2:    
		options.c_cflag |= CSTOPB;  
	   break;
	default:    
		 fprintf(stderr,"Unsupported stop bits\n");  
		 return (FALSE); 
   }
/* Set input parity option */ 
  if (parity != 'n')
    options.c_iflag |= INPCK;

  //一些串口设置  
  options.c_cflag |= CLOCAL | CREAD;
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag &= ~OPOST;
  options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  tcflush(fd,TCIFLUSH);
  options.c_cc[VTIME] = 10; /* 设置超时10 seconds*/
  options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
  if (tcsetattr(fd,TCSANOW,&options) != 0)
   {
     perror("SetupSerial 3");
     return (FALSE);
   }
  return (TRUE);
}
//---------------------------------------------------------------------------
int OpenComm(int CommPort,int BautSpeed,int databits,int stopbits,int parity)
{
  char dev[20]  = "/dev/ttyS"; //串口二
  char   tmp[3];
  int Commfd;
  sprintf(tmp,   "%ld", CommPort-1 );
  strcat(dev,tmp);
  printf("Comm is %s\n",dev);
  Commfd = OpenDev(dev);
  if(Commfd == FALSE)
    return FALSE;
  if(set_speed(Commfd,BautSpeed) == FALSE)
   {
    printf("Set Baut Error\n");
    return FALSE;
   }
  if (set_Parity(Commfd,databits,stopbits,parity) == FALSE)
   {
    printf("Set Parity Error\n");
    return FALSE;
   }
  switch(CommPort)
   {
    case 2:
           CreateComm2_RcvThread(Commfd);
           break;
    case 3:
           CreateComm3_RcvThread(Commfd);
           break;
    case 4:
           CreateComm4_RcvThread(Commfd);
           break;
   }
  return Commfd;
}
//---------------------------------------------------------------------------
void CreateComm2_RcvThread(int fd)
{
  int i,ret;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  ret=pthread_create(&comm2_rcvid, &attr, (void *)Comm2_RcvThread, fd);
  pthread_attr_destroy(&attr);
  printf ("Create comm2 pthread!\n");
  if(ret!=0){
    printf ("Create comm2 pthread error!\n");
  }
}
//---------------------------------------------------------------------------
void CreateComm3_RcvThread(int fd)
{
  int ret;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  ret=pthread_create(&comm3_rcvid, &attr, (void *)Comm3_RcvThread, fd);
  pthread_attr_destroy(&attr);
  printf ("Create comm3 pthread!\n");
  if(ret!=0){
    printf ("Create comm3 pthread error!\n");
  }
}
//---------------------------------------------------------------------------
void CreateComm4_RcvThread(int fd)
{
  int ret;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  ret=pthread_create(&comm4_rcvid, &attr, (void *)Comm4_RcvThread, fd);
  pthread_attr_destroy(&attr);
  printf ("Create comm4 pthread!\n");
  if(ret!=0){
    printf ("Create comm4 pthread error!\n");
  }
}
//---------------------------------------------------------------------------
void Comm2_RcvThread(int fd)  //Comm2接收线程函数
{
  struct timeval tv;
  uint32_t prev_comm_time;
  uint32_t nowtime;
  int len;
  int i;
  int TouchPressed;  //触摸屏按下  1 按下  0 弹起
  char buff[128];
  unsigned char validbuff[20];
  struct commbuf1 commbuf;
  printf("This is comm2 pthread.\n");
  //上一次串口数据接收时间
  gettimeofday(&tv, NULL);
  prev_comm_time = tv.tv_sec *1000 + tv.tv_usec/1000;
  TouchPressed = 0;
  while (CommRecvFlag == 1)     //循环读取数据
   {
    //系统初始化标志,如没有初始化完成则等待
     while(InitSuccFlag == 0)
       usleep(10000);
     while((len = read(fd, buff, 512))>0)
      {
       gettimeofday(&tv, NULL);
       nowtime = tv.tv_sec *1000 + tv.tv_usec/1000;
       //与上一次接收超过50ms,则判断为超时
       if((nowtime - prev_comm_time) >= 50)
        {
         commbuf.iget = 0;
         commbuf.iput = 0;
         commbuf.n = 0;
        }
       prev_comm_time = nowtime;

       printf("comm2 Len %d\n",len);
       buff[len] = '\0';

    //   for(i=0; i<len; i++)
    //     printf("buff[%d] = 0x%X, ", i, buff[i]);

       memcpy(commbuf.buffer + commbuf.iput, buff, len);
       commbuf.iput += len;
       if(commbuf.iput >= COMMMAX)
         commbuf.iput = 0;
       commbuf.n += len;
       while(commbuf.n >=5)
        {
         if((commbuf.buffer[commbuf.iget] == 0x81)||(commbuf.buffer[commbuf.iget] == 0x80)||(commbuf.buffer[commbuf.iget] == 0xFE))
          {
           if(commbuf.n >=5)
            {
             memcpy(validbuff, commbuf.buffer + commbuf.iget, 5);
             commbuf.iget +=5;
             if(commbuf.iget >= COMMMAX)
               commbuf.iget = 0;
             commbuf.n -= 5;

             if((validbuff[0] == 0x81)&&(TouchPressed == 0))
              {
               	TouchPressed = 1;
              	 TouchScr_Func(validbuff); //触摸屏输入
              }
             if(validbuff[0] == 0x80)
               TouchPressed = 0;
	    if(validbuff[0] == 0xfe)
	    {
			printf("test!\n");
			PressTouchControFunc(validbuff);
	    }
            }
           else
             break;
          }
         else
          {
           commbuf.iget ++;
           if(commbuf.iget >= COMMMAX)
             commbuf.iget = 0;
           commbuf.n --;
          }
        }
      }
   }
}
//---------------------------------------------------------------------------
void Comm3_RcvThread(int fd)  //Comm3接收线程函数
{
  int len;
  char buff[128];
  printf("This is comm3 pthread.\n");
  while (CommRecvFlag == 1)     //循环读取数据
   {
     while((len = read(fd, buff, 512))>0)
      {
       printf("\ncomm 3 Len %d\n",len);
       buff[len+1] = '\0';
       printf( "\n%s", buff);
       if(strcmp(buff,"exit")==0)
        {
         printf("recvfrom888888888");
         CommRecvFlag=0;
     //   break;
        }
      }
   }
}
//---------------------------------------------------------------------------
void Comm4_RcvThread(int fd)  //Comm4接收线程函数
{
/*  int len;
  char buff[128];
  printf("This is comm4 pthread.\n");  
  while (CommRecvFlag == 1)     //循环读取数据
   {
     while((len = read(fd, buff, 512))>0)
      {
       printf("\ncomm4 Len %d\n",len);
       buff[len+1] = '\0';
       printf( "\n%s", buff);
       if(strcmp(buff,"exit")==0)
        {
         printf("recvfrom888888888");
         CommRecvFlag=0;
     //   break;
        }
      }
   }      */
}
//---------------------------------------------------------------------------
void CloseComm()
{
  //Comm2数据接收线程
  CommRecvFlag = 0;
  usleep(40*1000);
  pthread_cancel(comm2_rcvid);
//  pthread_cancel(comm3_rcvid);
  pthread_cancel(comm4_rcvid);
  close(Comm2fd);
//  close(Comm3fd);
  close(Comm4fd);
}
//---------------------------------------------------------------------------
int CommSendBuff(int fd,unsigned char buf[1024],int nlength)
{
  int nByte = write(fd, buf ,nlength);
  return nByte;
}
//---------------------------------------------------------------------------
void PressTouchControFunc(unsigned char*inputbuff)
{
	int key;
	char wavFile[80];

	//// after 30s no operation will go to main window
	PicStatBuf.KeyPressTime = 0;
	Local.LcdLightTime = 0;

	if(Local.Status == 0)
	{
		sprintf(wavFile,"%ssound1.wav\0",wavPath);
		StartPlayWav(wavFile,0);
	}

	if(Local.LcdLightFlag == 0)
	{
		OpenLCD();
		Local.LcdLightFlag = 1;
		Local.LcdLightTime = 0;
	}
	else
	{
		key = inputbuff[4];

		switch(key)
		{
		case 0x01:	/// open lock
			if((Local.Status == 6)&&(Local.CurrentWindow == TalkPicWindow))//Local.Status == 2
			{
				OpenLock_Func();
			}
			break;
		case 0x02:	////watch
			break;
		case 0x03:	////info
			if((Local.Status == 0)&&(Local.CurrentWindow != TalkAreaMessageWindow))
			{
				InitParam = 0;
				DisplayAreaMessageWindow();
			}
			break;
		case 0x04:	///talk
			if((Local.Status == 0)&&(Local.CurrentWindow != TalkMenuWindow)&&(Local.CurrentWindow != TalkPicWindow))/// if system is idle,goto talk operate interface
				DisplayTalkWindow();
			if((Local.Status != 0)&&(Local.CurrentWindow == TalkPicWindow))
			{
				switch(Local.Status)
				{
				case 2:
					OperateTalkPicWindow(0,TalkPicWindow);
					break;
				case  5:
				case 6:
					OperateTalkPicWindow(3,TalkPicWindow);
					break;
				}
				Local.RecordPic = 0;
				Local.IFrameCount = 0;
				Local.IFrameNo = 0;
			}
			break;
		default:
			break;
		}
	}
}

void TouchScr_Func(unsigned char *inputbuff) //触摸屏输入
{
	int RowHei;
	int DeltaHei;
	int X,Y, XLCD, YLCD;
	int i;
	int isInTouch;
	char wavFile[80];      

	/*校正*/
	
/*
	X = (inputbuff[1] * 128 + inputbuff[2]);//*4;
	Y = (inputbuff[3] * 128 + inputbuff[4]);//*4;
	XLCD = (X - LocalCfg.Ts_X0)*SCRWIDTH/LocalCfg.Ts_deltaX + SCRWIDTH/2;
	YLCD = (Y - LocalCfg.Ts_Y0)*SCRHEIGHT/LocalCfg.Ts_deltaY + SCRHEIGHT/2;

*/
	XLCD  = (inputbuff[1] * 32 + inputbuff[2]);//*4;
	YLCD  = (inputbuff[3] * 32 + inputbuff[4]);//*4;
	

	if(XLCD < 0)
		XLCD = 0;
	if(XLCD > SCRWIDTH)
		XLCD = SCRWIDTH;
	if(YLCD < 0)
		YLCD = 0;
	if(YLCD > SCRHEIGHT)
		YLCD = SCRHEIGHT;



	PicStatBuf.KeyPressTime = 0;
	Local.LcdLightTime = 0;
	if(Local.LcdLightFlag == 0)
	{
		//点亮LCD背光
		OpenLCD();
		Local.LcdLightFlag = 1; //LCD背光标志
		Local.LcdLightTime = 0; //时间
	}
	else
	{
		isInTouch = 0;
		switch(Local.CurrentWindow)
		{
		case MainWindow: //主窗口

			///main menu
			for(i=0; i<5; i++) 
			{
				isInTouch = CheckTSInButton(&(menu_button[i]), XLCD, YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i + 'a');
					break;
				}
			}

			///press hotkey
			if(isInTouch == 0)
			{
				if(Local.ShowHotkey == 1)
				{
					for(i=0;i<6;i++)
					{
						isInTouch = CheckTSInImage(&hotkey_image[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+5);
							break;
						}
					}
				}
				else if(Local.ShowHotkey == 0)
				{
					isInTouch = CheckTSInImage(&hotkey_image[6],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc('a'+11);
						break;
					}
				}
			}
			///misscall 
			if(isInTouch == 0)
			{
				isInTouch = CheckTSInImage(&misscall_image[0],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc('a'+12);
					break;
				}
			}
			if(isInTouch == 0)
			{
				isInTouch = CheckTSInImage(&missmessage_image[0],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a'+8);
					break;
				}
			}
			if(isInTouch == 0)
			{
				isInTouch = CheckTSInImage(&news_image[0],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a'+9);
					break;
				}
			}
			break;

		case TalkMenuWindow:
			/// number key
			for(i=0;i<12;i++)
			{
				isInTouch = CheckTSInButton(&talk_keynum_button[i],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a');
					break;
				}
			}

			///hotkey
			if(isInTouch == 0)
			{
				for(i=0;i<3;i++)
				{
					isInTouch = CheckTSInButton(&talk_hotkey_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+12);
						break;
					}
				}
			}

			///talk menu key
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
						break;
					}
				}
			}

			///big menu key
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+30);
						break;
					}
				}
				
			}
			break;
		case TalkPicWindow:
			if(isInTouch == 0)
			{
				if((Local.PlayPicSize == 2) ||(Local.CurrFbPage ==1))
				{
					if(CheckZoomDelayTime() == 0)
						return;
					if(Local.ZoomInOutFlag == 1)
						break;
					if(Local.CurrFbPage == 1)
						ZoomIn_Func();
					if(Local.CurrFbPage != 0)
					{
						Local.CurrFbPage = 0;
						fb_setpage(fbdev,Local.CurrFbPage);
						TalkOsd();
					}
					break;
				}
			}
			if(isInTouch == 0)
			{
				if(Local.ShowRecPic == 0)
				{
					for(i=0;i<4;i++)
					{
						isInTouch = CheckTSInButton(&talkpic_button[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a');
							break;
						}
					}
				}
				else
				{
					printf("++++++++++++\n");
					for(i=1;i<5;i++)
					{
						isInTouch = CheckTSInButton(&talkpic_button[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a');
							break;
						}
					}
				}
			}
			break;
		case TalkAreaMessageWindow:
			for(i=0;i<5;i++)
			{
				isInTouch = CheckTSInImage(&AreaMessageComKey[i],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a');
					break;
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<9;i++)
				{
					isInTouch = CheckTSInButton(&talk_info_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+5);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_message_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
						break;
					}
				}
			}
			break;
		case TalkInfoContentWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<4;i++)
				{
					isInTouch = CheckTSInImage(&talk_infocontent_comkey[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}

			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
		case TalkLocalMessageWindow:
			for(i=0;i<6;i++)
			{
				isInTouch = CheckTSInButton(&LocalMessageComKey[i],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a');
					break;
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<9;i++)
				{
					isInTouch = CheckTSInButton(&talk_info_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+6);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_message_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
					}
				}
			}
			break;
		case TalkPushMessageWindow:
			for(i=0;i<5;i++)
			{
				isInTouch = CheckTSInButton(&PushMessageComKey[i],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a');
					break;
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<9;i++)
				{
					isInTouch = CheckTSInButton(&talk_info_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+5);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_message_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
					}
				}
			}
			break;
		case TalkCustomMessageWindow:
			for(i=0;i<5;i++)
			{
				isInTouch = CheckTSInButton(&CustomMessageComKey[i],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a');
					break;
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<9;i++)
				{
					isInTouch = CheckTSInButton(&talk_info_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+5);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_message_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
					}
				}
			}
			break;
		case TalkMisscallWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<4;i++)
				{
					isInTouch = CheckTSInButton(&talk_callrec_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
						 break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<9;i++)
				{
					isInTouch = CheckTSInButton(&talk_info_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+5);
						break;
					}
				}
			}
			
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInImage(&MisscallComKey[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			break;
		case TalkCalledWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<4;i++)
				{
					isInTouch = CheckTSInButton(&talk_callrec_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
						 break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<9;i++)
				{
					isInTouch = CheckTSInButton(&talk_info_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+5);
						break;
					}
				}
			}
			
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInImage(&CalledComKey[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			break;
		case TalkCallWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<4;i++)
				{
					isInTouch = CheckTSInButton(&talk_callrec_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+15);
						 break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<9;i++)
				{
					isInTouch = CheckTSInButton(&talk_info_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+5);
						break;
					}
				}
			}
			
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInImage(&CallComKey[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			break;
		case TalkPhonebookWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			break;
		case TalkBlacklistWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&talk_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+43);
						break;
					}
				}
			}
			break;

		case LanSetWindow:

			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInEdit(&LanSet_edit[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}

			if(isInTouch == 0)
			{
				if(Local.keypad_show == 1)
				{
					for(i=0;i<32;i++)
					{
						isInTouch = CheckTSInButton(&keyboard_num[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+7);
							break;
						}
					}
				}
				else
				{
					for(i=0;i<7;i++)
					{
						isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+42);
							break;
						}
					}
				}
					
			}
			
			
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case WlanSetWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<7;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+42);
						break;
					}
				}
			}

			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case RoomSetWindow:
			


			if(isInTouch == 0)
			{
				if(Local.keypad_show == 1)
				{
					for(i=0;i<32;i++)
					{
						isInTouch = CheckTSInButton(&keyboard_num[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+7);
							break;
						}
					}
				}
				else
				{
					for(i=0;i<7;i++)
					{
						isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+42);
							break;
						}
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			for(i=0;i<6;i++)
			{
				isInTouch = CheckTSInEdit(&RoomSet_edit[i],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a');
					break;
				}
			}
			break;
		case ScreenContrastWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<3;i++)
				{
					isInTouch = CheckTSInButton(&setup_screen_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}

			if(isInTouch == 0)
			{
				for(i=0;i<7;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+ 42);
						break;
					}
				}
			}
			
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case ScreenSavingWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<3;i++)
				{
					isInTouch = CheckTSInButton(&setup_screen_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}

			if(isInTouch == 0)
			{
				for(i=0;i<7;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+ 42);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case ScreenCalibrateWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<3;i++)
				{
					isInTouch = CheckTSInButton(&setup_screen_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}

			if(isInTouch == 0)
			{
				for(i=0;i<7;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+ 42);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case VoiceSetWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<7;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+42);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case TimeSetWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<3;i++)
				{
					isInTouch = CheckTSInEdit(&TimeSet_edit[i],XLCD,YLCD);
					printf("isInTouch = %d\n,XLCD=%d,YLCD=%d\n",isInTouch,XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				if(Local.keypad_show == 0)
				{
					for(i=0;i<7;i++)
					{
						isInTouch = CheckTSInButton(&setup_menu_button[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+42);
							break;
						}
					}
				}
				else if(Local.keypad_show == 1)
				{
					for(i=0;i<32;i++)
					{
						isInTouch = CheckTSInButton(&keyboard_num[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+7);
							break;
						}
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case PassSetWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<3;i++)
				{
					isInTouch = CheckTSInEdit(&PassSet_edit[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				if(Local.keypad_show == 0)
				{
					for(i=0;i<5;i++)
					{
						isInTouch = CheckTSInButton(&setup_menu_button[i+7],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+42);
							break;
						}
					}
				}
				else if(Local.keypad_show == 1)
				{
					for(i=0;i<32;i++)
					{
						isInTouch = CheckTSInButton(&keyboard_num[i],XLCD,YLCD);
						if(isInTouch == 1)
						{
							KeyAndTouchFunc(i+'a'+7);
							break;
						}
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case LangSetWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i+7],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+42);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case UpgradeSDWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<2;i++)
				{
					isInTouch = CheckTSInButton(&setup_upgrade_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i+7],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+42);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case UpgradeRemoteWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<2;i++)
				{
					isInTouch = CheckTSInButton(&setup_upgrade_menu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a');
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i+7],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+42);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;
		case SystemInfoWindow:
			if(isInTouch == 0)
			{
				for(i=0;i<5;i++)
				{
					isInTouch = CheckTSInButton(&setup_menu_button[i+7],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+42);
						break;
					}
				}
			}
			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
						break;
					}
				}
			}
			break;

		case SetupMainWindow:
			for(i=0;i<15;i++)
			{
				isInTouch = CheckTSInButton(&setup_keynum_button[i],XLCD,YLCD);
				if(isInTouch == 1)
				{
					KeyAndTouchFunc(i+'a');
					break;
				}
			}

			if(isInTouch == 0)
			{
				for(i=0;i<6;i++)
				{
					isInTouch = CheckTSInButton(&bigmenu_button[i],XLCD,YLCD);
					if(isInTouch == 1)
					{
						KeyAndTouchFunc(i+'a'+50);
					}
				}
			}
			break;
#if 0		
case 2: //家电控制窗口（一级）
for(i=0; i<2; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&wiring_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 91: //单个控制窗口（二级）
for(i=0; i<9; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&sigwiring_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 92: //单个控制窗口（二级）
for(i=0; i<9; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&scenewiring_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 3: //对讲窗口（户户通话 一级）
for(i=0; i<12; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&num_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&talk_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 12);
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 13://监视窗口（二级）
if((Local.PlayPicSize == 2)||(Local.CurrFbPage == 1))
{
if(CheckZoomDelayTime() == 0)  //放大缩小图像时检查延迟
return;

if(Local.ZoomInOutFlag == 1)   //正在放大缩小中
break;
if(Local.CurrFbPage == 1)
ZoomIn_Func();  //视频缩小
//设置FB页面
if(Local.CurrFbPage != 0)
{                                        
Local.CurrFbPage = 0;
fb_setpage(fbdev, Local.CurrFbPage);
TalkOsd();  //通话和监视时OSD显示
}
break;
}
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&watch_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
if(i == 1)  //放大
{
if(CheckZoomDelayTime() == 0)  //放大缩小图像时检查延迟
return;
}
//键盘和触摸屏处理函数
if(Local.PlayPicSize == 1)
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)  //视频窗口
{
isInTouch = CheckTSInVideoScreen(XLCD, YLCD); //检查触摸点是否在该视频窗口内
if(isInTouch == 1)
{
if(CheckZoomDelayTime() == 0)  //放大缩小图像时检查延迟
return;

if(Local.ZoomInOutFlag == 1)   //正在放大缩小中
break;
//键盘和触摸屏处理函数
if(Local.PlayPicSize == 1)
KeyAndTouchFunc(10 + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 16://显示图像操作窗口（二级）,有呼叫时自动显示
if((Local.PlayPicSize == 2)||(Local.CurrFbPage == 1))
{
if(CheckZoomDelayTime() == 0)  //放大缩小图像时检查延迟
return;

if(Local.ZoomInOutFlag == 1)   //正在放大缩小中
break;
if(Local.CurrFbPage == 1)
ZoomIn_Func();  //视频缩小              
//设置FB页面
if(Local.CurrFbPage != 0)
{
Local.CurrFbPage = 0;
fb_setpage(fbdev, Local.CurrFbPage);
TalkOsd();  //通话和监视时OSD显示
}
break;
}
for(i=0; i<4; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&talkpic_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
if(i == 1)  //放大
{
if(CheckZoomDelayTime() == 0)  //放大缩小图像时检查延迟
return;
}                
//键盘和触摸屏处理函数
if(Local.PlayPicSize == 1)
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)  //视频窗口
{
isInTouch = CheckTSInVideoScreen(XLCD, YLCD); //检查触摸点是否在该视频窗口内
if(isInTouch == 1)
{
if(CheckZoomDelayTime() == 0)  //放大缩小图像时检查延迟
return;

if(Local.ZoomInOutFlag == 1)   //正在放大缩小中
break;
//键盘和触摸屏处理函数
if(Local.PlayPicSize == 1)
KeyAndTouchFunc(11 + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 31:  //撤防界面操作（二级）
for(i=0; i<12; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&num1_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&cancelfortify_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 12);
break;
}
}
break;
case 32:  //布防界面操作（二级）
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&fortify_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 34://显示报警窗口（二级）
for(i=0; i<1; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&alarm_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
break;
case 35:  //取消报警界面操作（二级）
for(i=0; i<12; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&num1_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
for(i=0; i<2; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&cancelfortify_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 12);
break;
}
}
break;
break;
case 4://信息窗口（一级）
if(isInTouch == 0)
for(i=0; i<3; i++)
{
isInTouch = CheckTSInButton(&info_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc('a' + i);
break;
}
}
if(isInTouch == 0)
for(i=0; i<INFONUMPERPAGE; i++)  //信息行
{
isInTouch = CheckTSInButton(&inforow_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 4);
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 77://信息显示窗口（二级，弹出）
if(isInTouch == 0)
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&infowin_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
break;
case 21://照片窗口（一级）
if(isInTouch == 0)
for(i=0; i<4; i++)
{
isInTouch = CheckTSInButton(&info_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc('a' + i);
break;
}
}
if(isInTouch == 0)
for(i=0; i<INFONUMPERPAGE; i++)  //信息行
{
isInTouch = CheckTSInButton(&inforow_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 4);
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 22://照片显示窗口（二级，弹出）
if(isInTouch == 0)
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&picwin_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
break;
case 5://设置窗口（一级）
for(i=0; i<12; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&num2_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
if(isInTouch == 0)
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&setup_pass_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 12);
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 161://网络地址设置窗口（二级）
for(i=0; i<12; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&num2_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&setup_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 12);
break;
}
}
if(isInTouch == 0)
for(i=0; i<5; i++)  //文本框
{
isInTouch = CheckTSInEdit(&ip_edit[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 16);
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 159://房号设置窗口（二级）
for(i=0; i<12; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&num2_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&setup_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 12);
break;
}
}
if(isInTouch == 0)
for(i=0; i<5; i++)  //文本框
{
isInTouch = CheckTSInEdit(&addr_edit[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 16);
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}
break;
case 156://修改密码窗口（二级）
for(i=0; i<12; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&num2_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a');
break;
}
}
for(i=0; i<3; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&setup_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 12);
break;
}
}
if(isInTouch == 0)
for(i=0; i<2; i++)  //窗口按钮
{
isInTouch = CheckTSInButton(&modi_engi_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 16);
break;
}
}
if(isInTouch == 0)
for(i=0; i<2; i++)  //文本框
{
isInTouch = CheckTSInEdit(&modi_engi_edit[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 18);
break;
}
}
if(isInTouch == 0)
for(i=0; i<6; i++)  //底部菜单按钮
{
isInTouch = CheckTSInButton(&bigmenu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
KeyAndTouchFunc(i + 'a' + 30);
break;
}
}             
break;
case 190: //校准触摸屏窗口
if(Local.Status == 0)
{
sprintf(wavFile, "%ssound1.wav\0", wavPath);
StartPlayWav(wavFile, 0);
}
Calib_X[Local.CalibratePos] = X;
Calib_Y[Local.CalibratePos] = Y;
OperateCalibrateWindow(0);
break;    
#endif

}

}
}
//---------------------------------------------------------------------------
int CheckZoomDelayTime(void)  //放大缩小图像时检查延迟
{
struct timeval tv;
gettimeofday(&tv, NULL);
Local.newzoomtime = tv.tv_sec *1000 + tv.tv_usec/1000;
if(((Local.newzoomtime - Local.oldzoomtime) < ZOOMMAXTIME)&&(Local.newzoomtime > Local.oldzoomtime))
return 0;
else
Local.oldzoomtime = Local.newzoomtime;
return 1;
}
//---------------------------------------------------------------------------
int CheckTouchDelayTime(void)  //触摸屏处理时检查延迟
{
struct timeval tv;
gettimeofday(&tv, NULL);
Local.newtouchtime = tv.tv_sec *1000 + tv.tv_usec/1000;
if(((Local.newtouchtime - Local.oldtouchtime) < TOUCHMAXTIME)&&(Local.newtouchtime > Local.oldtouchtime))
return 0;
else
Local.oldtouchtime = Local.newtouchtime;
return 1;
}
//---------------------------------------------------------------------------
//检查触摸区域在菜单条内
void CheckIsTouchMenu(int XLCD, int YLCD)
{
int i;
int isInTouch;
for(i=0; i<10; i++)
{
isInTouch = CheckTSInButton(&menu_button[i], XLCD, YLCD);
if(isInTouch == 1)
{
//键盘和触摸屏处理函数
// KeyAndTouchFunc(i);
break;
}
}
}
//---------------------------------------------------------------------------
int CheckTSInImage(struct TImage *t_image,int XLCD,int YLCD)
{
	if((XLCD >= t_image->xLeft)&&(XLCD <= (t_image->xLeft + t_image->width))
		&&(YLCD >= t_image->yTop)&&(YLCD <= (t_image->yTop + t_image->height)))
		return 1;
	else
		return 0;
}


int CheckTSInButton(struct TImageButton *t_button, int XLCD, int YLCD) //检查触摸点是否在该按钮内
{
if((XLCD >= t_button->xLeft)&&(XLCD <= (t_button->xLeft + t_button->width))
&&(YLCD >= t_button->yTop)&&(YLCD <= (t_button->yTop + t_button->height)))
return 1;
else
return 0;
}
//---------------------------------------------------------------------------
int CheckTSInLabel(struct TLabel *t_label, int XLCD, int YLCD) //检查触摸点是否在该Label内
{
if((XLCD >= t_label->xLeft)&&(XLCD <= (t_label->xLeft + t_label->width))
&&(YLCD >= t_label->yTop)&&(YLCD <= (t_label->yTop + t_label->height)))
return 1;
else
return 0;
}
//---------------------------------------------------------------------------
int CheckTSInEdit(struct TEdit *t_edit, int XLCD, int YLCD) //检查触摸点是否在该Edit内
{
if((XLCD >= t_edit->xLeft)&&(XLCD <= (t_edit->xLeft + t_edit->width))
&&(YLCD >= t_edit->yTop)&&(YLCD <= (t_edit->yTop + t_edit->height)))
return 1;
else
return 0;
}
//---------------------------------------------------------------------------
int CheckTSInVideoScreen(int XLCD, int YLCD) //检查触摸点是否在该视频窗口内
{
char wavFile[80];
if((XLCD >= 158)&&(XLCD <= (158 + 352))
&&(YLCD >= 114)&&(YLCD <= (114 + 240)))
{
return 1;
}
else
return 0;
}
//---------------------------------------------------------------------------
int CheckTSInPicScreen(int XLCD, int YLCD) //检查触摸点是否在该照片窗口内
{
int i;
int Pic_X[3] = {87, 301, 509};
int Pic_Y[3] = {127, 128, 128};
for(i=0; i<3; i++)
if((XLCD >= Pic_X[i])&&(XLCD <= (Pic_X[i] + 192))
&&(YLCD >= Pic_Y[i])&&(YLCD <= (Pic_Y[i] + 144)))
{
return i+1;
}
return 0;
}
//---------------------------------------------------------------------------
//norder 指令    TerType 设备类型     TerNo 设备编号    InfoType  设备信息类别    Info   设备信息
void SendHostOrder(unsigned char norder, unsigned char TerType, unsigned char TerNo, unsigned char InfoType, unsigned char Info) //发送主动命令
{
	int i,j;
	unsigned char crc;
	//锁定互斥锁
	pthread_mutex_lock (&Local.comm_lock);
	//查找可用发送缓冲并填空
	for(i=0; i<COMMSENDMAX; i++)
	if(Multi_Comm_Buff[i].isValid == 0)
	{
		Multi_Comm_Buff[i].SendNum = 5;
		Multi_Comm_Buff[i].m_Comm = Comm4fd;

		Multi_Comm_Buff[i].buf[0] = 0xFF;   //起始字符
		Multi_Comm_Buff[i].buf[1] = norder;   //命令
		Multi_Comm_Buff[i].buf[2] = TerType;   // 设备类型
		Multi_Comm_Buff[i].buf[3] = TerNo;   //设备编号
		Multi_Comm_Buff[i].buf[4] = InfoType;   //设备信息类别
		Multi_Comm_Buff[i].buf[5] = Info;   //设备信息类别
		crc = 0;
		for(j=1; j<6; j++)
			crc = crc ^ Multi_Comm_Buff[i].buf[j];
		Multi_Comm_Buff[i].buf[6] = crc;
		Multi_Comm_Buff[i].buf[7] = 0xFE;

		Multi_Comm_Buff[i].nlength = 8;
		Multi_Comm_Buff[i].isValid = 1;
		sem_post(&multi_comm_send_sem);
		break;
	}
	//打开互斥锁
	pthread_mutex_unlock (&Local.comm_lock);
}
//---------------------------------------------------------------------------


void RefreshLed(void)
{
	int i;
	int led =  0x0;

	//Local.NewInfo
	if(LocalCfg.news_num != 0)
		led = (led|0x02);
	if(LocalCfg.DefenceStatus != 0)
		led = (led|0x04);
	if(Local.AlarmStatus != 0)
		led = (led|0x08);
		
	pthread_mutex_lock(&Local.comm_lock);
	for(i=0;i<COMMSENDMAX;i++)
	{
		if(Multi_Comm_Buff[i].isValid == 0)
		{
			Multi_Comm_Buff[i].SendNum = 5;
			Multi_Comm_Buff[i].m_Comm = Comm2fd;
			Multi_Comm_Buff[i].buf[0] = 0xff;
			Multi_Comm_Buff[i].buf[1] = 0x00;
			Multi_Comm_Buff[i].buf[2] = 0x00;
			Multi_Comm_Buff[i].buf[3] = 0x00;
			Multi_Comm_Buff[i].buf[4] = (led&0x0f);
			Multi_Comm_Buff[i].nlength = 5;
			Multi_Comm_Buff[i].isValid = 1;
			sem_post(&multi_comm_send_sem);
			break;
		}
	}
	pthread_mutex_unlock(&Local.comm_lock);
}

