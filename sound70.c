#include <stdio.h>
#include <stdlib.h>
#include   <time.h>
#include <unistd.h>

#include <dirent.h>

#include <sys/types.h> 
#include <sys/stat.h> 

#include <assert.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <linux/videodev.h>

#include <linux/mtd/mtd.h>

#include "common.h"

#include "fmpeg4_avcodec.h"
#include "fmjpeg_avcodec.h"
#include "ratecontrol.h"
void ChangeBl(int val);

void WriteInfoFile(int InfoType);

//framebuffer 函数
int             fb_open(char *fb_device);
int             fb_close(int fd);
int             fb_display_open(int fd);
int             fb_display_close(int fd);
int             fb_stat(int fd, int *width, int *height, int *depth);
//设置显存页
int             fb_setpage(int fd, int fbn);
void           *fb_mmap(int fd, unsigned int screensize);
int             fb_munmap(void *start, size_t length);
int             fb_pixel(/*unsigned char *fbmem, */int width, int height,
						 int x, int y, unsigned short color, int PageNo);
                                                 
FILE *hzk16fp;
void openhzk16(void);
void closehzk16(void);
void outxy16(int x,int y,int wd,int clr,int mx,int my,char s[128],int pass, int PageNo); //写16点阵汉字
FILE *hzk24fp,*hzk24t,*ascfp;
void openhzk24(void);
void closehzk24(void);
void outxy24(int x,int y,int wd,int clr,int mx,int my,char s[128],int pass, int PageNo); //写24点阵汉字
void fb_line(int start_x,int start_y,int end_x,int end_y, unsigned short color, int PageNo);

//OSD TEST
void OpenOsd(void);   //打开OSD
void CloseOsd(void);  //关闭OSD
void ShowOsd(char *Content); //显示OSD内容
void sig_pwr(void);

char sZkPath[80]="/usr/zk/";
void ReadCfgFile(void);
//写本地设置文件
void WriteCfgFile(void);
//读首页图象及标签位置文件

extern uint32_t ref_time;  //基准时间,音频或视频第一帧

//信息链表
// *h保存表头结点的指针，*p指向当前结点的前一个结点，*s指向当前结点
InfoNode1 *InfoNode_h[INFOTYPENUM];
InfoNode1 *PicNode_h[2]; //照片
InfoNode1 * init_infonode(void); //初始化单链表的函数
//功能描述：求单链表长度
int length_infonode(InfoNode1 *h);
//功能描述：查找未读信息
int length_infonoreaded(InfoNode1 *h);
//功能描述：尾部添加
int creat_infonode(InfoNode1 *h, struct InfoContent1  TmpContent);
//功能描述：插入函数
int insert_infonode(InfoNode1 *h, InfoNode1 *p, struct InfoContent1  TmpContent);
//功能描述：删除函数
int delete_infonode(InfoNode1 *p);
int delete_all_infonode(InfoNode1 *h);
//功能描述：定位函数
InfoNode1 * locate_infonode(InfoNode1 *h,int i);
//功能描述：查找函数
InfoNode1 * find_infonode(InfoNode1 *h, struct InfoContent1  TmpContent);
//
int free_infonode(InfoNode1 *h);
//短信息数量结构
struct Info1 Info[INFOTYPENUM];
//读信息文件
void ReadInfoFile(void);

//照片数量结构
struct Info1 PicStrc[2];
//读照片文件
void ReadPictureFile(void);
//写照片函数
void WritePicFunc(int cType);  //cType 0 呼叫照片  1 通话照片

void OpenLCD(void);
void CloseLCD(void);

//PMU 关闭时钟
void SetPMU(void);
void SaveToFlash(int savetype);

//与守护进程采用共享内存方式通信
extern int shmid;
extern char *c_addr;
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  pthread_attr_t attr;
  int ch;
  int programrun;//程序运行标志
  int dwSize;
  int i,j;
  uint32_t Ip_Int;
//  FILE           *infile;
  unsigned short  color;
  int new_sd_status;
  //declaration for framebuffer device
  char           *fb_device;
  unsigned int    x;
  unsigned int    y;
  DIR *dirp;
  struct timeval tv1,tv2;
  char jpgfilename[80];
  char wavFile[80];


  //时钟
  time_t t;

  DebugMode = 1;
  if(argv[1] != NULL)
   {
    if(strcmp(argv[1], "-debug") == 0)
     {
      DebugMode = 1;           //调试模式
     }
   }

  ref_time = 0;   //初始时间戳
  TimeStamp.OldCurrVideo = 0;       //上一次当前视频时间
  TimeStamp.CurrVideo = 0;
  TimeStamp.OldCurrAudio = 0;       //上一次当前音频时间
  TimeStamp.CurrAudio = 0;  

  //初始化链表头结点
  if(TempVideoNode_h == NULL)
    TempVideoNode_h = init_videonode();
  //初始化链表头结点
  if(TempAudioNode_h == NULL)
    TempAudioNode_h = init_audionode();
    
  strcpy(sPath, "/usr/pic/");
  strcpy(wavPath, "/usr/wav/");
  RemoteDataPort=8300;
  RemoteVideoPort=8302;
  strcpy(RemoteHost, "192.168.0.88");
  LocalDataPort = 8300;   //命令及数据UDP端口
  LocalVideoPort = 8302;  //音视频UDP端口

  Local.NetStatus = 0;   //网络状态 0 断开  1 接通
  Local.OldNetSpeed = 100;  //网络速度   
  Local.ResetPlayRingFlag = 0;  //复位Audio Play flag
  
  Local.RecordPic = 0;  //留照片  0 不留
  Local.IFrameCount = 0; //I帧计数
  Local.IFrameNo = 5;    //留第几个I帧

  Local.ForceEndWatch = 0;  //有呼叫时，强制关监视
  Local.ZoomInOutFlag = 0;  ////正在放大缩小中

Local.ShowRecPic = 0;

Local.AlarmStatus = 0;
  gettimeofday(&tv1, NULL);
  Local.oldzoomtime = tv1.tv_sec *1000 + tv1.tv_usec/1000;  //上一次放大缩小时间
  Local.oldtouchtime = Local.oldzoomtime;    //上一次触摸屏处理时间  

   	time(&t);

	curr_tm_t=localtime(&t);
	if((curr_tm_t->tm_year + 1900) == 1970)
	{
		curr_tm_t->tm_year   = 2008 - 1900;
		curr_tm_t->tm_mon   =   1 - 1;
		curr_tm_t->tm_mday   =   1;
		curr_tm_t->tm_hour   =   0;
		curr_tm_t->tm_min   = 0;
		curr_tm_t->tm_sec   =   0;
		t=mktime(curr_tm_t);
		stime((long*)&t);
	}


  //open framebuffer device
  if ((fb_device = getenv("FRAMEBUFFER")) == NULL)
    fb_device = FB_DEV;
  fbdev = fb_open(fb_device);
  if(DebugMode == 1)

  //get status of framebuffer device
  fb_stat(fbdev, &fb_width, &fb_height, &fb_depth);

  screensize = fb_width * fb_height * fb_depth / 8;
  screensize = f_data.buf_len*FRAMEBUFFERMAX;
  fbmem = fb_mmap(fbdev, screensize);
  printf("%dx%d\n",fb_width,fb_height);

  //初始化libimage
  InitLibImage(fbmem, fb_width, fb_height, &f_data);
  
  if( (gpio_fd=open(DEVICE_GPIO,O_RDWR)) < 0 )
   {
     printf("can not open gpio device");
     return;
   }

  //GPIO8-13--->INPUT,其它--->OUTPUT
  ioctl(gpio_fd,IO_SETINOUT, 0xFFFFC0FF);
  //GPIO7-5 设为低
//  ioctl(gpio_fd,IO_SETSCANVALUE, 0xFFFFFF1F);

  //指示灯
  ioctl(gpio_fd, IO_CLEAR, 2);   //布防
  ioctl(gpio_fd, IO_CLEAR, 3);   //信息 留影
  //ioctl(gpio_fd, IO_CLEAR, 4);   //报警
  ioctl(gpio_fd, IO_PUT, 4);     //报警  

  ioctl(gpio_fd, IO_CLEAR, 5);
  ioctl(gpio_fd, IO_CLEAR, 6);
  ioctl(gpio_fd, IO_CLEAR, 7);

  Local.LcdLightFlag = 1; //LCD背光标志
  Local.LcdLightTime = 0; //时间
  PicStatBuf.KeyPressTime = 0;


  OpenLCD();     //打开LCD
 // getchar();

  //GPIO14：输出，选择按键扫描方式，低电平有效
  //GPIO15：输出，选择防区扫描输入，低电平有效
  ioctl(gpio_fd, IO_PUT, 14);
  ioctl(gpio_fd, IO_PUT, 15);          

  SetPMU();  //PMU 关闭时钟

  Local.MenuIndex = 0;
  Local.MaxIndex = 5;
  Local.MainMenuIndex = Local.MenuIndex;

  Local.CurrFbPage = 0;

    //打开声卡播放
    if(!OpenSnd(AUDIODSP1))
     {
      printf("Open play sound device error!\n");
      return;
     }

  //读配置文件
  ReadCfgFile();

  //外出延时
  LocalCfg.Out_DelayTime = 30;
  //报警延时
  LocalCfg.Alarm_DelayTime = 30;
  if(LocalCfg.Addr[0] == 'S')
   {
    Local.AddrLen = 12;  //地址长度  S 12  B 6 M 8 H 6
   }
  if(LocalCfg.Addr[0] == 'B')
   {
    Local.AddrLen = 6;  //地址长度  S 12  B 6 M 8 H 6    
   }  

  Local.Weather[0] = 1;  //天气预报
  Local.Weather[1] = 20;//0;
  Local.Weather[2] = 27;//0;

  Local.ReportSend = 0;  //设备定时报告状态已发送
  Local.RandReportTime = 1;
  Local.ReportTimeNum  = 0;

  Local.nowvideoframeno = 1;   //当前视频帧编号
  Local.nowaudioframeno = 1;   //当前音频帧编号  

  Local.NoBreak = 0;   //免扰状态 1 免扰  0 正常
  //主机
  Local.isHost = '0';
  Local.HostIP[0] = 0;
  Local.HostIP[1] = 0;
  Local.HostIP[2] = 0;
  Local.HostIP[3] = 0;
  if(LocalCfg.Addr[0] == 'S')
   {
    Local.isHost = LocalCfg.Addr[11];
    memcpy(Local.HostAddr, LocalCfg.Addr, 20);
    Local.HostAddr[11] = '0';
   }
  if(LocalCfg.Addr[0] == 'B')
   {
    Local.isHost = LocalCfg.Addr[5];
    memcpy(Local.HostAddr, LocalCfg.Addr, 20);
    Local.HostAddr[5] = '0';
   }
  Local.DenNum = 0;

	printf("%d.%d.%d.%d\n",LocalCfg.IP_Server[0],LocalCfg.IP_Server[1],LocalCfg.IP_Server[2],LocalCfg.IP_Server[3]);
  //防区状态
  LocalCfg.DefenceStatus = 0;
  LocalCfg.DefenceNum = 1;
  for(i=0; i<32; i++)
   for(j=0; j<10; j++)
    LocalCfg.DefenceInfo[i][j] = 0;

  //初始化短信息结构
  Info[0].MaxNum = 200;   //普通信息
  Info[1].MaxNum = 200;   //公共信息
  Info[2].MaxNum = 100;   //天气预报
  Info[3].MaxNum = 200;   //社区服务
  //初始化信息链表头结点
  for(i=0; i<INFOTYPENUM; i++)
    InfoNode_h[i] = init_infonode();

  //读信息文件
  ReadInfoFile();

  for(i=0; i< INFOTYPENUM; i++)
   {
    Info[i].TotalNum = length_infonode(InfoNode_h[i]);
    Info[i].NoReadedNum = length_infonoreaded(InfoNode_h[i]);
    if(Info[i].NoReadedNum > Info[i].TotalNum)
      Info[i].NoReadedNum = Info[i].TotalNum;
    #ifdef _DEBUG
      printf("Info[i].TotalNum = %d, Info[i].NoReadedNum = %d\n", Info[i].TotalNum, Info[i].NoReadedNum);
    #endif  
    Info[i].CurrentInfoPage = 1;
   }

  //读照片文件
  ReadPictureFile();

  DeltaLen = 9 + sizeof(struct talkdata1);  //数据包有效数据偏移量

  strcpy(UdpPackageHead, "XXXCID");

  Local.Status = 0;  //状态为空闲
  Local.RecPicSize = 1;  //默认视频大小为352*240
  Local.PlayPicSize = 1;  //默认视频大小为352*240

  Local.DefenceDelayFlag = 0;    //布防延时标志
  Local.DefenceDelayTime = 0;    //计数
  Local.AlarmDelayFlag[0] = 0;     //报警延时标志
  Local.AlarmDelayTime[0] = 0;    //计数
  Local.AlarmDelayFlag[1] = 0;     //报警延时标志
  Local.AlarmDelayTime[1] = 0;    //计数

  Local.keypad_show = 0;
  Local.keypad_type = 0;///

  Local.IP_Group[0] = 0; //组播地址
  Local.IP_Group[1] = 0;
  Local.IP_Group[2] = 0;
  Local.IP_Group[3] = 0;

  strcpy(NullAddr, "00000000000000000000");
  Ip_Int=inet_addr("192.168.68.88");
  memcpy(Remote.IP,&Ip_Int,4);
  memcpy(Remote.Addr[0],NullAddr,20);
  memcpy(Remote.Addr[0],"S0001010101",11);      //
  #ifdef _DEBUG
    printf("[zz]-Local.IP[0]=%d, Local.IP[1]=%d, Local.IP[2]=%d, Local.IP[3]=%d\n",
       LocalCfg.IP[0],LocalCfg.IP[1],LocalCfg.IP[2],LocalCfg.IP[3]);
  #endif

    #ifdef _R2RVIDEO
      printf("R2R is Valid\n");
    #endif    

//  getchar();

  //线程运行标志
   Local.Key_Press_Run_Flag = 1;
   Local.Save_File_Run_Flag = 1;
   Local.Multi_Send_Run_Flag = 1;
   Local.Multi_Comm_Send_Run_Flag = 1;

  //系统初始化标志
  	InitSuccFlag = 0;
  	MainPageInit(); 

	TalkWindowInit();
  	SetupWindowInit();
  //系统初始化标志
  InitSuccFlag = 1;

  //打开16点阵汉字库
  openhzk16();
  //打开24点阵汉字库
  openhzk24();

  //COMM
  Comm2fd = OpenComm(2,9600,8,1,'N');
  if(Comm2fd <= 0)
    printf("Comm2 Open error!");

//  Comm3fd = OpenComm(3,9600,8,1,'N');
//  if(Comm3fd <= 0)
//    printf("Comm3 Open error!");

  Comm4fd = OpenComm(4,9600,8,1,'N');
  if(Comm4fd <= 0)
    printf("Comm4 Open error!");

  //检测网络连接  
  m_EthSocket = 0;    
  //UDP
  if(InitUdpSocket(LocalDataPort)==0)
   {
    printf("can't create data socket.\n\r");
    return 0;
   }
  if(InitUdpSocket(LocalVideoPort)==0)
   {
    printf("can't create video socket.\n\r");
    return 0;
   }


//将UFLASH存储缓冲置为无效
  for(i=0; i<SAVEMAX; i++)
    Save_File_Buff[i].isValid = 0;
//FLASH存储线程
  sem_init(&save_file_sem,0,0);
  save_file_flag = 1;
  //创建互斥锁
  pthread_mutex_init (&Local.save_lock, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&save_file_thread,&attr,(void *)save_file_thread_func,NULL);
  pthread_attr_destroy(&attr);
  if ( save_file_thread == 0 ) {
        printf("无法创建FLASH存储线程\n");
        return;
    }

//将UDP发送缓冲置为无效
  for(i=0; i<UDPSENDMAX; i++)
   {
    Multi_Udp_Buff[i].isValid = 0;
    Multi_Udp_Buff[i].SendNum = 0;
    Multi_Udp_Buff[i].DelayTime = 100;
   }
//主动命令数据发送线程：终端主动发送命令，如延时一段没收到回应，则多次发送
//用于UDP和Comm通信
  sem_init(&multi_send_sem,0,0);
  multi_send_flag = 1;
  //创建互斥锁
  pthread_mutex_init (&Local.udp_lock, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&multi_send_thread,&attr,(void *)multi_send_thread_func,NULL);
  pthread_attr_destroy(&attr);
  if ( multi_send_thread == 0 ) {
        printf("无法创建主动命令数据发送线程\n");
        return;
    }

//将COMM发送缓冲置为无效
  for(i=0; i<COMMSENDMAX; i++)
   {
    Multi_Comm_Buff[i].isValid = 0;
    Multi_Comm_Buff[i].SendNum = 0;
   }
//主动命令数据发送线程：终端主动发送命令，如延时一段没收到回应，则多次发送
//用于UDP和Comm通信
  sem_init(&multi_comm_send_sem,0,0);
  multi_comm_send_flag = 1;
  //创建互斥锁
  pthread_mutex_init (&Local.comm_lock, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&multi_comm_send_thread,&attr,(void *)multi_comm_send_thread_func,NULL);
  pthread_attr_destroy(&attr);
  if ( multi_comm_send_thread == 0 ) {
        printf("无法创建串口主动命令数据发送线程\n");
        return;
    }

  //延时清提示信息标志
  PicStatBuf.Flag = 0;
  PicStatBuf.Type = 0;
  PicStatBuf.Time = 0;

  programrun = 1;
  ch=' ';
  printf("Please select: 'q' is exit!\n");

  //运行守护进程  
 system("/tmp/daemon &");

  gettimeofday(&tv2, NULL);
  if(DebugMode == 1)
   {
    printf("tv1.tv_sec=%d, tv1.tv_usec=%d\n", tv1.tv_sec,tv1.tv_usec);
    printf("tv2.tv_sec=%d, tv2.tv_usec=%d\n", tv2.tv_sec,tv2.tv_usec);
   }
  //当前界面为"开机界面"
  Local.PrevWindow = 0;
  Local.CurrentWindow = MainWindow;

  //显示信息提示窗口
  DisplayMainWindow(Local.CurrentWindow);


  //初始化ARP Socket  
  InitArpSocket();

  //加入NS组播组
  AddMultiGroup(m_VideoSocket, NSMULTIADDR);

  Local.OsdOpened = 1;
  CloseOsd();  //关闭OSD
  //发送免费ARP  
 // SendFreeArp();
/*  watchdog_fd = open("/dev/wdt",O_WRONLY);
  if(watchdog_fd == -1)
   {
    printf("watchdog error\n");
   }
  printf("watchdog_fd = %d\n", watchdog_fd);    */
      
  //初始化
  gpio_rcv_flag = 1;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&gpio_rcv_thread,&attr,(void *)gpio_rcv_thread_func,NULL);
  pthread_attr_destroy(&attr);
  if ( gpio_rcv_thread == 0 ) {
        printf("无法创建gpio按键处理线程\n");
        return;
    }          
  TimeReportStatusFunc();//设备定时报告状态

  do
   {
    // 以无限回圈不断等待键盘操作
    ch=getchar();
    if((Local.CurrentWindow == 2)&&(Local.CurrFbPage == 2))
     {
       Local.CurrFbPage = 0;
       fb_setpage(fbdev, Local.CurrFbPage);
     }
    else
      // 等待自键盘输入字元
      switch(ch)
       {                     // 判断输入字元为何
	case 'Q':                     // 判断是否[q]键被按下
		  programrun = 0;     //退出程序
                  break;
        case '0':				
	    case '1':                   	            				
        case '2':				
        case '3':				
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
			      Local.HavePicRecorded_flag = 0;
                  key_press = ch - '0';
                  break;
        case 'a': //0
        case 'b': //1
        case 'c': //2
        case 'd': //3
        case 'e': //4
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':  //14
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':   
  			      Local.HavePicRecorded_flag = 1;
                  key_press = ch ;//- '0';
                  break;
	default:
		 break;
	}
    } while (programrun);
//与守护进程采用共享内存方式通信  正常停止
  c_addr[0] = 'E';
  shmdt(c_addr);   //断开连接共享内存
  system("killall -q daemon");

  //退出NS组播组
  DropNsMultiGroup(m_VideoSocket, NSMULTIADDR);

  CloseOsd();    //关闭OSD

  CloseLCD();    //关闭LCD

  //关闭声卡播放
  CloseSnd(AUDIODSP1);

  //关闭Comm及接收线程
  CloseComm();



  //FLASH存储线程
  save_file_flag = 0;
  usleep(40*1000);
  pthread_cancel(save_file_thread);
  sem_destroy(&save_file_sem);
  pthread_mutex_destroy(&Local.save_lock);  //删除互斥锁  

  //主动命令数据发送线程
  multi_send_flag = 0;
  usleep(40*1000);
  pthread_cancel(multi_send_thread);
  sem_destroy(&multi_send_sem);
  pthread_mutex_destroy(&Local.udp_lock);  //删除互斥锁

  //COMM主动命令数据发送线程
  multi_comm_send_flag = 0;
  usleep(40*1000);
  pthread_cancel(multi_comm_send_thread);
  sem_destroy(&multi_comm_send_sem);
  pthread_mutex_destroy(&Local.comm_lock);  //删除互斥锁

  //gpio
  gpio_rcv_flag = 0;
  usleep(40*1000);
  pthread_cancel(gpio_rcv_thread);
  ioctl(gpio_fd,IO_SETINOUT,0xFFFF3FFF);
  close(gpio_fd);

  //关闭ARP
  CloseArpSocket();
  //关闭UDP及接收线程
  CloseUdpSocket();  

  fb_munmap(fbmem, screensize);
  //close framebuffer device
  fb_close(fbdev);

  //状态提示信息缓冲
  BuffUninit();

  //free链表
  for(i=0; i<INFOTYPENUM; i++)
   {
    free_infonode(InfoNode_h[i]);
    free(InfoNode_h[i]);
   }

  //free链表
  for(i=0; i<2; i++)
   {
    free_infonode(PicNode_h[i]);
    free(PicNode_h[i]);
   }

  free_videonode(TempVideoNode_h);   //视频接收缓冲列表
  free_audionode(TempAudioNode_h);   //音频接收缓冲列表  
  printf("1111\n");
  //按钮释放
  ButtonUnInit();
  printf("2222\n");
  //Gif释放
  GifUnInit();
  printf("3333\n");
  //Edit释放
  EditUnInit();
  printf("4444\n");
  //Image释放
  ImageUnInit();
  //PopupWin释放
  PopupWinUnInit();

  //关闭16点阵汉字库
  closehzk16();
  //关闭24点阵汉字库
  closehzk24();
  
  return (0);  
}
void ChangeBl(int val)
{
	int i;
	pthread_mutex_lock(&Local.comm_lock);
	for(i=0;i<COMMSENDMAX;i++)
	{
		if(Multi_Comm_Buff[i].isValid == 0)
		{
			Multi_Comm_Buff[i].SendNum = 5;
			Multi_Comm_Buff[i].m_Comm = Comm2fd;
			Multi_Comm_Buff[i].buf[0] = 0xfe;
			Multi_Comm_Buff[i].buf[1] = 0x00;
			Multi_Comm_Buff[i].buf[2] = 0x00;
			Multi_Comm_Buff[i].buf[3] = 0x00;
			Multi_Comm_Buff[i].buf[4] = (val&0x0f);
			Multi_Comm_Buff[i].nlength = 5;
			Multi_Comm_Buff[i].isValid = 1;
			sem_post(&multi_comm_send_sem);
			break;
		}
	}
	pthread_mutex_unlock(&Local.comm_lock);

}
//---------------------------------------------------------------------------
void OpenLCD(void)
{
  //LCD上电20ms后，打开LCD输出，10ms后开背光
  //LCD Power
	 ioctl(gpio_fd, IO_CLEAR, 0);
  	usleep((20-10)*1000);
  	fb_display_open(fbdev);
  	usleep((15-10)*1000);
  //LCD背光
//  ioctl(gpio_fd, IO_PUT, 1);  
  	ChangeBl(0);
  	printf("open lcd backlight!\n");

}
//---------------------------------------------------------------------------
void CloseLCD(void)
{
  //LCD上电20ms后，打开LCD输出，10ms后开背光
  //LCD背光
 // ioctl(gpio_fd, IO_CLEAR, 1);
 
	 ChangeBl(0x0a);
	printf("closed lcd backlight!\n");
	usleep((20-10)*1000);
  	fb_display_close(fbdev);
 	 usleep((30-10)*1000);
 	 //LCD Power
  	ioctl(gpio_fd, IO_PUT, 0);
}
//---------------------------------------------------------------------------
/*
 * open framebuffer device.
 * return positive file descriptor if success,
 * else return -1.
 */
int fb_open(char *fb_device)
{
  int  fd;
  if ((fd = open(fb_device, O_RDWR)) < 0) {
    perror(__func__);
    return (-1);
  }
  return (fd);
}
//---------------------------------------------------------------------------
/*
 * get framebuffer's width,height,and depth.
 * return 0 if success, else return -1.
 */
int fb_stat(int fd, int *width, int *height, int *depth)
{
  struct fb_fix_screeninfo fb_finfo;
  struct fb_var_screeninfo fb_vinfo;
  int fbn =0;

  if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_finfo)) {
    perror(__func__);
    return (-1);
  }

  if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_vinfo)) {
    perror(__func__);
    return (-1);
  }

  //init to 0
  if (ioctl(fd,FLCD_SET_FB_NUM,&fbn)<0) {
    printf("Fail to set fb num\n");
    return 0;
  }

  if (ioctl(fd,FLCD_GET_DATA_SEP, &f_data) < 0) {
    printf("LCD Error: can not operate 0x%x\n", FLCD_GET_DATA_SEP);
    return 0;
  }
  fb_uvoffset = f_data.uv_offset;
  #ifdef _DEBUG
    printf("f_data.uv_offset = %d\n", fb_uvoffset);
  #endif
  *width = fb_vinfo.xres;
  *height = fb_vinfo.yres;
  *depth = fb_vinfo.bits_per_pixel;
  return (0);
}
//---------------------------------------------------------------------------
//设置显存页
int fb_setpage(int fd, int fbn)
{
  //init to 0
  if (ioctl(fd,FLCD_SET_FB_NUM,&fbn)<0) {
    printf("Fail to set fb num\n");
    return 0;
  }
}
//---------------------------------------------------------------------------
//打开显示
int fb_display_open(int fd)
{
  if (ioctl(fd, FLCD_OPEN, NULL)) {
    perror(__func__);
    return (-1);
  }
  return (0);
}
//---------------------------------------------------------------------------
//关闭显示
int fb_display_close(int fd)
{
  if (ioctl(fd, FLCD_CLOSE, NULL)) {
    perror(__func__);
    return (-1);
  }
  return (0);
}
//---------------------------------------------------------------------------
/*
 * map shared memory to framebuffer device.
 * return maped memory if success,
 * else return -1, as mmap dose.
 */
void *fb_mmap(int fd, unsigned int screensize)
{
  caddr_t fbmem;
  if ((fbmem = mmap(0, screensize, PROT_READ | PROT_WRITE,
      MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror(__func__);
        return (void *) (-1);
  }
  return (fbmem);
}
//---------------------------------------------------------------------------
/*
 * unmap map memory for framebuffer device.
 */
int fb_munmap(void *start, size_t length)
{
	return (munmap(start, length));
}
//---------------------------------------------------------------------------
/*
 * close framebuffer device
 */
int fb_close(int fd)
{
	return (close(fd));
}
//---------------------------------------------------------------------------
/*
 * display a pixel on the framebuffer device.
 * fbmem is the starting memory of framebuffer,
 * width and height are dimension of framebuffer,
 * x and y are the coordinates to display,
 * color is the pixel's color value.
 * return 0 if success, otherwise return -1.
 */

int fb_pixel(/*unsigned char *fbmem, */int width, int height,
		 int x, int y, unsigned short color, int PageNo)
{
  uint8_t isU;
  uint8_t isChr;

  uint8_t Y;
  uint8_t U;
  uint8_t V;

  if ((x > width) || (y > height))
    return (-1);

  isU=0;
  if( y%2==0 ) isU=1; // this is a U line

  switch(color)
   {
    case cWhite:  //  1
               Y=235;
               U=128;
               V=128;
               break;
    case cYellow: // 2
               Y=210;
               U=16;
               V=146;
               break;
    case cCyan:   //   3
               Y=170;
               U=166;
               V=16;
               break;
    case cGreen:  //  4
               Y=145;
               U=54;
               V=34;
               break;
    case cMagenta://  5
               Y=106;
               U=202;
               V=222;
               break;
    case cRed:    //  6
               Y=81;
               U=90;
               V=240;
               break;
    case cBlue:   //  7
               Y=41;
               U=240;
               V=110;
               break;
    case cBlack:  //  8
               Y=16;
               U=128;
               V=128;
               break;
    default:
               Y=16;
               U=128;
               V=128;
               break;
   }
  *(fbmem + f_data.buf_len*PageNo + y*width + x) = Y;
  return (0);
}
//---------------------------------------------------------------------------
void fb_line(int start_x,int start_y,int end_x,int end_y, unsigned short color, int PageNo)
{
  int i,j;
  for(j=start_y;j<=end_y;j++)
    for(i=start_x;i<=end_x;i++)
     {
      fb_pixel(fb_width, fb_height, i, j, color, PageNo);
     }

}
//---------------------------------------------------------------------------
/* strip string */
void strip(char *p) {
    while (1) {
        if ((*p == '\r') || (*p == '\n'))
            *p = '\0';
        if (*p == '\0')
            break;
        p++;
    }
}
//---------------------------------------------------------------------------
void openhzk16(void)
{
  char hzk16name[80];
  strcpy(hzk16name, sZkPath);
  strcat(hzk16name,"hzk16");
  if((hzk16fp=fopen(hzk16name,"rb"))==NULL){
    	printf("Can't open file \" hzk16 \".");
        return;
    }
}
//---------------------------------------------------------------------------
void closehzk16(void)
{
   fclose(hzk16fp);
}
//---------------------------------------------------------------------------
void outxy16(int x,int y,int wd,int clr,int mx,int my,char s[128],int pass, int PageNo)
{
//  FILE *ccfp,*ascfp;
  unsigned char ccdot[16][2];
  unsigned char ascdot[16];
  int col,byte,bit,mask;
  int mxc,myc;
  int len,cx,cy;
  int ascm,oldclr;
  char exasc[2];
  unsigned long offset;
  unsigned long ascoff;

  for(len=0;len < strlen(s);len +=2){
    if((s[len]&0x80)&&(s[len+1]&0x80)){
      offset=(((unsigned char)s[len]-0xa1)*94+(unsigned char)s[len+1]-0xa1)*32L;
        if(fseek(hzk16fp,offset,SEEK_SET)!=0)
         {
          printf("Seek File \" HZK16\" Error.");
         }
        fread(ccdot,2,16,hzk16fp);

        for(col=0;col<16;col++)
         {
          cx=x+col;
          for(byte=0;byte<2;byte++)
           {
            cy=y+8*byte;
            mask=0x80;
            for(bit=0;bit < 8; bit++)
             {
              if(ccdot[col][byte]&mask)
               for(myc=0;myc<my;myc++)
                for(mxc=0;mxc<mx;mxc++)
                  fb_pixel(fb_width, fb_height,
                           x+8*byte*mx+bit*mx+mxc,y+col*my+myc, clr, PageNo);
               mask =mask>>1;
              }
            }
         }
        if(pass==0)
          x=x+16*mx+wd;
        else
          y=y+16*my+wd;
      }
    else
     {
          {
           ascoff=(long)s[len]*16;
           if(fseek(ascfp,ascoff,SEEK_SET)!=0){
             printf("Seek File \" ASC16\" Error.");
          }
         fread(ascdot,1,16,ascfp);
         for(byte=0;byte < 16;byte++){
           mask=0x80;
           cy=y+byte+2;//*2;
          for(bit=0;bit < 8;bit++){
            if(ascdot[byte]&mask){
              cx=x+bit;//*2;
                fb_pixel(fb_width, fb_height,
                           cx/*+ascm*/,cy, clr, PageNo);
             }
            mask=mask>>1;
           }
       }
        if(pass==0)
          x=x+8+wd;
        else
          y=y+8+wd;
      }
      len--;
    }
   }
//  fclose(ascfp);
//  fclose(ccfp);
}
//---------------------------------------------------------------------------
void openhzk24(void)
{
  char hzk24name[80];
  char hzk24tname[80];
  char asc16name[80];
  strcpy(hzk24name, sZkPath);
  strcat(hzk24name,"hzk24s");
  if((hzk24fp=fopen(hzk24name,"rb"))==NULL){
    	printf("Can't open file \" hzk24s \".");
        return;
    }

  strcpy(hzk24tname, sZkPath);
  strcat(hzk24tname,"hzk24t");
  if((hzk24t=fopen(hzk24tname,"rb"))==NULL){
    	printf("Can't open file \" hzk24t \".");
        return;
    }

  strcpy(asc16name, sZkPath);
  strcat(asc16name,"asc16");
  if((ascfp=fopen(asc16name,"rb"))==NULL){
    	printf("Can't open file \" ASC16 \".");
        return;
    }
}
//---------------------------------------------------------------------------
void closehzk24(void)
{
   fclose(ascfp);
   fclose(hzk24fp);
   fclose(hzk24t);
}
//---------------------------------------------------------------------------
void outxy24(int x,int y,int wd,int clr,int mx,int my,char s[128],int pass, int PageNo)
{
//  FILE *ccfp,*ascfp;
  unsigned char ccdot[24][3];
  unsigned char ascdot[16];
  int col,byte,bit,mask,mxc,myc;
  int len,cx,cy;
  int ascm,oldclr;
  char exasc[2];
  unsigned long offset;
  unsigned long ascoff;

  for(len=0;len < strlen(s);len +=2){
    if((s[len]&0x80)&&(s[len+1]&0x80)){
      if(s[len] <= 0xA3)
       {
        //24点阵字符库
        offset=(((unsigned char)s[len]-0xa1)*94+(unsigned char)s[len+1]-0xa1)*72L;
        if(fseek(hzk24t,offset,SEEK_SET)!=0)
         {
          printf("Seek File \" HZK24T\" Error.");
          return;
         }
        fread(ccdot,3,24,hzk24t);
       }
      else
       {
        //24点阵字库中没有制表符等,故需减一偏移量
        offset=(((unsigned char)s[len]-0xa1 - 15)*94+(unsigned char)s[len+1]-0xa1)*72L;
        if(fseek(hzk24fp,offset,SEEK_SET)!=0)
         {
          printf("Seek File \" HZK24S\" Error.");
          return;
         }
        fread(ccdot,3,24,hzk24fp);
       }
      for(col=0;col<24;col++){
        cx=x+col*mx;
        for(byte=0;byte<3;byte++){
          cy=y+8*byte*my;
          mask=0x80;
          for(bit=0;bit < 8; bit++){
            if(ccdot[col][byte]&mask)
              for(myc=0;myc < my;myc++)
                for(mxc=0;mxc < mx;mxc++)
                  fb_pixel(fb_width, fb_height,
                           cx+mxc,cy+bit*my+myc,clr, PageNo);
                  mask =mask>>1;
           }
         }
       }
      if(pass==0)
        x=x+24*mx+wd;
      else
        y=y+24*my+wd;
     }
    else{
		      /*	if(s[len]&0x80){
				exasc[0]=s[len];
				exasc[1]='\0';
				oldclr=getcolor();
				setcolor(clr);
				outtextxy(x,y,exasc);
				setcolor(oldclr);
				x +=8;
			}
			else   */
         ascoff=(unsigned char)s[len]*16L;
         if(fseek(ascfp,ascoff,SEEK_SET)!=0){
           printf("Seek File \" ASC16\" Error.");
           return;
          }
        fread(ascdot,1,16,ascfp);
        for(byte=0;byte < 16;byte++){
          mask=0x80;
         // cy=y+byte*2-2;
          cy=y+byte*2-2;
          for(bit=0;bit < 8;bit++){
            if(ascdot[byte]&mask){
              cx=x+bit;//*2;
                fb_pixel(fb_width, fb_height,
                           cx/*+ascm*/,cy, clr, PageNo);
                fb_pixel(fb_width, fb_height,
                           cx/*+ascm*/,cy+1, clr, PageNo);
             }
            mask=mask>>1;
           }
         }
        if(pass==0)
          x=x+8+wd;
        else
          y=y+8+wd;
     len--;
    }
   }
//   fclose(ascfp);
//   fclose(ccfp);
}
//---------------------------------------------------------------------------
void save_file_thread_func(void)
{
  int i;
  FILE *read_fd;
  unsigned char tmpchar[30];
  unsigned char readname[80];
  #ifdef _DEBUG
    printf("创建FLASH存储线程\n" );
  #endif

  //系统初始化标志
  InitSuccFlag = 1;
  while(save_file_flag == 1)
   {
    sem_wait(&save_file_sem);
    if(Local.Save_File_Run_Flag == 0)
      Local.Save_File_Run_Flag = 1;
    else
    {
       //锁定互斥锁
       pthread_mutex_lock (&Local.save_lock);
       for(i=0; i<SAVEMAX; i++)
        if(Save_File_Buff[i].isValid == 1)
         {
          switch(Save_File_Buff[i].Type)
           {
            case 1:      //一类信息
                   WriteInfoFile(Save_File_Buff[i].InfoType);
                  // strcpy(readname, info_name);
                   break;
            case 2:      //单个信息
                   //WriteInfoFileLock(Save_File_Buff[i].InfoType, Save_File_Buff[i].InfoNo,
                   //                  Save_File_Buff[i].Info_Node);
                   //strcpy(readname, info_name);
                   break;
            case 4:      //本地设置
                   WriteCfgFile();
                   strcpy(readname, cfg_name);
                   break;
            case 5:      //未接听留照片
            case 6:      //通话留照片
                   WritePicFunc(Save_File_Buff[i].Type - 5);
                   break;
            case 7: 
            case 8:
                   break;
           }

          Save_File_Buff[i].isValid = 0;

          break;
         }
       //打开互斥锁
       pthread_mutex_unlock (&Local.save_lock);
      }
   }
}
//---------------------------------------------------------------------------
/*
void WriteCallListFunc(int cType)
{
	FILE* read_fd;
	unsigned char tmpchar[30];
	unsigned char readname[80];
	int i,j;
	struct InfoContent1 InitInfoCon;
	InfoNode1* tmp_node;

	InitInfoCon.isValid = 1;
	InitInfoCon.isReaded = 0;
	InitInfoCon.isLocked = 0;
	
}
*/
//写照片函数
void WritePicFunc(int cType)  //cType 0 未接听照片  1 通话照片
{
  FILE *read_fd;
  unsigned char tmpchar[30];
  unsigned char readname[80];

  struct InfoContent1 InitPicCon;
  struct InfoContent1 InitInfoCon;
  int i,j;
  InfoNode1 *tmp_node;
  InfoNode1 *tmp_node2;

  char picPath[80] = "/mnt/mtd/picture/";

  i = 0;

  InitPicCon.isValid = 1;
  InitPicCon.isReaded = 0;
  InitPicCon.isLocked = 0;
  
  InitInfoCon.isValid = 1;
  InitInfoCon.isReaded = 0;
  InitInfoCon.isLocked = 0;

  sprintf(InitPicCon.Time, "%04d-%02d-%02d %02d:%02d:%02d\0", Local.recpic_tm_t->tm_year + 1900, Local.recpic_tm_t->tm_mon+1,
            Local.recpic_tm_t->tm_mday, Local.recpic_tm_t->tm_hour, Local.recpic_tm_t->tm_min, Local.recpic_tm_t->tm_sec);
  sprintf(InitInfoCon.Time,"%s",InitPicCon.Time);
  InitPicCon.Length = 0;
  InitPicCon.Type = cType; //照片类型

  InitInfoCon.Length = 20;
  InitInfoCon.Type = cType;

  sprintf(InitInfoCon.Content,"%s\0",CallListAddr);

  //以时间加随机数方式产生照片文件名
  sprintf(InitPicCon.Content, "%s%02d%02d%02d%2d.jpg\0", picPath, Local.recpic_tm_t->tm_hour, Local.recpic_tm_t->tm_min, Local.recpic_tm_t->tm_sec, 1+(int)(100.0*rand()/(RAND_MAX+1.0)));

  if(PicStrc[0].TotalNum >= PicStrc[0].MaxNum)
  //删除未锁定的最后一个，在头部插入一个
   {
    for(j = PicStrc[0].TotalNum; j >= 1; j --)
     {
      tmp_node=locate_infonode(PicNode_h[0], j);
      if(tmp_node->Content.isLocked == 0)
       {
        unlink(tmp_node->Content.Content);  //删除照片文件
        delete_infonode(tmp_node);
        PicStrc[0].TotalNum --;
        break;
       }
     }
   }
  //如信息未到最大数量，则插入到头部
  if(PicStrc[0].TotalNum < PicStrc[0].MaxNum)
   {
    tmp_node=locate_infonode(PicNode_h[0], 1);
    insert_infonode(PicNode_h[0], tmp_node, InitPicCon);
    PicStrc[0].TotalNum ++;
   }

 if(Info[cType+1].TotalNum >= Info[cType+1].MaxNum)
 {
 	for(j=Info[cType+1].TotalNum;j>=1;j--)
 	{
		tmp_node2 = locate_infonode(InfoNode_h[cType+1],1);
		if(tmp_node2->Content.isLocked == 0)
		{
			delete_infonode(tmp_node2);
			Info[cType+1].TotalNum--;
			break;
		}
 	}
 }

 if(Info[cType+1].TotalNum < Info[cType+1].MaxNum)
 {
 	tmp_node2 = locate_infonode(InfoNode_h[cType+1],1);
	insert_infonode(InfoNode_h[cType+1],tmp_node2,InitInfoCon);
	Info[cType+1].TotalNum++;
 }
  
  //libyuv 库函数
 	WriteYuvToJpg(InitPicCon.Content, 80, Local.yuv[0],  CIF_W, CIF_H);

  //更新数量
  	PicStrc[0].TotalNum = length_infonode(PicNode_h[0]);
  	PicStrc[0].NoReadedNum = length_infonoreaded(PicNode_h[0]);
	Info[cType+1].TotalNum = length_infonode(InfoNode_h[cType+1]);
	Info[cType+1].NoReadedNum = length_infonoreaded(InfoNode_h[cType+1]);

  //WritePicIniFile(0);
}
//---------------------------------------------------------------------------
void multi_send_thread_func(void)
{
  int i,j;
  int isAdded;
  int HaveDataSend;
  char buff[7];
  #ifdef _DEBUG
    printf("创建主动命令数据发送线程：\n" );
    printf("multi_send_flag=%d\n",multi_send_flag);
  #endif
  while(multi_send_flag == 1)
   {
    //等待有按键按下的信号
    sem_wait(&multi_send_sem);
    if(Local.Multi_Send_Run_Flag == 0)
      Local.Multi_Send_Run_Flag = 1;
    else
    {
    HaveDataSend = 1;
    while(HaveDataSend)
     {
      for(i=0; i<UDPSENDMAX; i++)
       if(Multi_Udp_Buff[i].isValid == 1)
        {
          #ifdef _DEBUG
           // printf("watch send Multi_Udp_Buff[i].RemoteHost = %s\n",Multi_Udp_Buff[i].RemoteHost);
          #endif  
          if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
           {
            if(Multi_Udp_Buff[i].m_Socket == ARP_Socket)
              ArpSendBuff(); //免费ARP发送
            else
             {
              if((Multi_Udp_Buff[i].SendNum != 0)&&(Multi_Udp_Buff[i].DelayTime > 100))
                usleep((Multi_Udp_Buff[i].DelayTime - 100)*1000);
			  memcpy(buff,Multi_Udp_Buff[i].buf,6);
              //UDP发送
              UdpSendBuff(Multi_Udp_Buff[i].m_Socket, Multi_Udp_Buff[i].RemoteHost,
                          Multi_Udp_Buff[i].buf , Multi_Udp_Buff[i].nlength);
             }
            Multi_Udp_Buff[i].SendNum++;
           }
          if(Multi_Udp_Buff[i].SendNum  >= MAXSENDNUM)
           {
            //锁定互斥锁
            pthread_mutex_lock (&Local.udp_lock);
            if(Multi_Udp_Buff[i].m_Socket == ARP_Socket)
             {
              Multi_Udp_Buff[i].isValid = 0;
              #ifdef _DEBUG
                printf("免费ARP发送完成\n");
              #endif
             }
            else
             switch(Multi_Udp_Buff[i].buf[6])
              {
               case NSORDER:
                           if(Multi_Udp_Buff[i].CurrOrder == 255) //主机向副机解析
                            {
                             Multi_Udp_Buff[i].isValid = 0;
                             #ifdef _DEBUG
                                printf("查找副机失败\n");
                             #endif
                            }
                           else
                            {
                             //若该命令为子网解析，则下一个命令为向服务器解析
                             Multi_Udp_Buff[i].SendNum = 0;
                             //更改UDP端口
                             Multi_Udp_Buff[i].m_Socket = m_DataSocket;
                             sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",LocalCfg.IP_Server[0],
                                     LocalCfg.IP_Server[1],LocalCfg.IP_Server[2],LocalCfg.IP_Server[3]);
                             //命令, 服务器解析
                             Multi_Udp_Buff[i].buf[6] = NSSERVERORDER;
                             #ifdef _DEBUG
                                printf("正在向NS服务器解析地址\n");
                             #endif
                            }
                           break;
               case NSSERVERORDER: //服务器解析

                           Multi_Udp_Buff[i].isValid = 0;
                           #ifdef _DEBUG
                             printf("服务器解析失败\n");
                           #endif
						 if(Local.CurrentWindow == TalkPicWindow)
						   {
						   		Local.Status = 0;
								DisplayMainWindow(0);
						   }
                           //监视窗口 
                           #if 0
                           if(Local.CurrentWindow == 13)
                            {
                             strcpy(Label_Watch.Text, "查找地址失败");
                             ShowLabel(&Label_Watch, REFRESH);
                            }
                           //呼叫中心窗口
                           if(Local.CurrentWindow == 16)
                            {
                             strcpy(Label_CCenter.Text, "查找地址失败");
                             ShowLabel(&Label_CCenter, REFRESH);
                            }
                           //延时清提示信息标志
                           PicStatBuf.Type = Local.CurrentWindow;
                           PicStatBuf.Time = 0;
                           PicStatBuf.Flag = 1;
						   #endif
                           break;
               case VIDEOTALK:    //局域网可视对讲
               case VIDEOTALKTRANS:  //局域网可视对讲中转服务
                           switch(Multi_Udp_Buff[i].buf[8])
                            {
                             case CALL:
									if(Multi_Udp_Buff[i].buf[6] == VIDEOTALK)
									{
										if(Remote.DenNum == 1)
										{
										//若该命令为直通呼叫，则下一个命令为向服务器请求中转
										Multi_Udp_Buff[i].SendNum = 0;
										//更改UDP端口
										Multi_Udp_Buff[i].m_Socket = m_DataSocket;
										sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",LocalCfg.IP_Server[0],
										LocalCfg.IP_Server[1],LocalCfg.IP_Server[2],LocalCfg.IP_Server[3]);
										//命令, 服务器中转
										Multi_Udp_Buff[i].buf[6] = VIDEOTALKTRANS;
#ifdef _DEBUG
										printf("正在向主服务器申请对讲中转\n");
#endif
										}
									}
									else
									{
										Multi_Udp_Buff[i].isValid = 0;
										if(Local.CurrentWindow == TalkPicWindow)
											ShowStatusText(CALLX,CALLY, 3, cWhite, 1, 1, "呼叫失败", 0);
										
										if(Local.CurrentWindow == TalkPicWindow)
										{
											Local.Status = 0;
											DisplayMainWindow(MainWindow);
										}
										
#ifdef _DEBUG
										printf("呼叫失败, %d\n", Multi_Udp_Buff[i].buf[6]);
#endif
									}
									break;
                             case CALLEND:  //通话结束
                                          Multi_Udp_Buff[i].isValid = 0;
                                          Local.OnlineFlag = 0;
                                          Local.CallConfirmFlag = 0; //设置在线标志
                                          //对讲结束，清状态和关闭音视频
                                          TalkEnd_ClearStatus();
                                          break;
                             default: //为其它命令，本次通信结束
                                          Multi_Udp_Buff[i].isValid = 0;
                                          #ifdef _DEBUG
                                            printf("通信失败1, %d\n", Multi_Udp_Buff[i].buf[6]);
                                          #endif
                                          break;
                            }
                           break;
               case VIDEOWATCH:     //局域网监控
               case VIDEOWATCHTRANS:  //局域网监控中转服务
                           switch(Multi_Udp_Buff[i].buf[8])
                            {
                             case CALL:
                                          if(Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)
                                           {
                                            //若该命令为直通呼叫，则下一个命令为向服务器请求中转
                                            Multi_Udp_Buff[i].SendNum = 0;
                                            //更改UDP端口
                                            Multi_Udp_Buff[i].m_Socket = m_DataSocket;
                                            sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",LocalCfg.IP_Server[0],
                                                    LocalCfg.IP_Server[1],LocalCfg.IP_Server[2],LocalCfg.IP_Server[3]);
                                            //命令, 服务器中转
                                            Multi_Udp_Buff[i].buf[6] = VIDEOWATCHTRANS;
                                            #ifdef _DEBUG
                                               printf("正在向主服务器申请监控中转\n");
                                            #endif
                                           }
                                          else
                                           {
                                            Multi_Udp_Buff[i].isValid = 0;
                                          //  Local.Status = 0;
                                            //延时清提示信息标志
                                           // PicStatBuf.Type = Local.CurrentWindow;
                                        //    PicStatBuf.Time = 0;
                                      //      PicStatBuf.Flag = 1;
                                       //     if(Local.CurrentWindow == 13)
                                        //      ShowStatusText(50, 130 , 3, cBlack, 1, 1, "监视失败", 0);
                                            #ifdef _DEBUG
                                              printf("监视失败, %d\n", Multi_Udp_Buff[i].buf[6]);
                                            #endif
                                           }
                                          break;
                             case CALLEND:  //通话结束
                                          Multi_Udp_Buff[i].isValid = 0;
                                          Local.OnlineFlag = 0;
                                          Local.CallConfirmFlag = 0; //设置在线标志

                                          switch(Local.Status)
                                           {
                                            case 3: //本机监视
                                                   StopPlayVideo();
                                                   CloseOsd();

                                                   //延时清提示信息标志
                                                   PicStatBuf.Type = 13;
                                                   PicStatBuf.Time = 0;
                                                   PicStatBuf.Flag = 1;
                                                   break;
                                            case 4: //本机被监视
                                                   Local.Status = 0;  //状态为空闲
                                                   break;
                                           }
                                          break;
                             default: //为其它命令，本次通信结束
                                          Multi_Udp_Buff[i].isValid = 0;
                                          #ifdef _DEBUG
                                            printf("通信失败2, %d\n", Multi_Udp_Buff[i].buf[6]);
                                          #endif
                                          break;
                            }
                           break;
               default: //为其它命令，本次通信结束
                           Multi_Udp_Buff[i].isValid = 0;
                      //     Local.Status = 0;
                           #ifdef _DEBUG
                             printf("通信失败3, %d\n", Multi_Udp_Buff[i].buf[6]);
                           #endif
                           break;
              }
            //打开互斥锁
            pthread_mutex_unlock (&Local.udp_lock);
           }
        }
      //判断数据是否全部发送完，若是，线程终止
      HaveDataSend = 0;
      for(i=0; i<UDPSENDMAX; i++)
       if(Multi_Udp_Buff[i].isValid == 1)
        {
         HaveDataSend = 1;
         break;
        }
      
      usleep(100*1000);
     }
     }
   }
}
//---------------------------------------------------------------------------
void multi_comm_send_thread_func(void)
{
  int i;
  int HaveDataSend;
  #ifdef _DEBUG
    printf("创建COMM主动命令数据发送线程：\n" );
    printf("multi_comm_send_flag=%d\n",multi_comm_send_flag);
  #endif
  while(multi_comm_send_flag == 1)
   {
    sem_wait(&multi_comm_send_sem);
    if(Local.Multi_Comm_Send_Run_Flag == 0)
      Local.Multi_Comm_Send_Run_Flag = 1;
    else
    {
    HaveDataSend = 1;
    while(HaveDataSend)
     {
      for(i=0; i<COMMSENDMAX; i++)
       if(Multi_Comm_Buff[i].isValid == 1)
        {
          if(Multi_Comm_Buff[i].SendNum  < MAXSENDNUM)
           {
            if(Multi_Comm_Buff[i].SendNum > 0)
              usleep(80*1000);
            //COMM发送
            CommSendBuff(Multi_Comm_Buff[i].m_Comm,
                        Multi_Comm_Buff[i].buf , Multi_Comm_Buff[i].nlength);
          //  printf("send buff %d\n", Multi_Comm_Buff[i].buf[3]);
            Multi_Comm_Buff[i].SendNum++;
            break;
           }
        }
      usleep(20*1000);
      if((Multi_Comm_Buff[i].isValid == 1)&&(Multi_Comm_Buff[i].SendNum  >= MAXSENDNUM))
           {
            //锁定互斥锁
            pthread_mutex_lock (&Local.comm_lock);
            switch(Multi_Comm_Buff[i].buf[6])
             {
              default: //为其它命令，本次通信结束
                           Multi_Comm_Buff[i].isValid = 0;
                           #ifdef _DEBUG
                          //   printf("通信失败, %d\n", Multi_Comm_Buff[i].buf[3]);
                           #endif
                           break;
             }
            //打开互斥锁
            pthread_mutex_unlock (&Local.comm_lock);
           }      
      //判断数据是否全部发送完，若是，线程终止
      HaveDataSend = 0;
      for(i=0; i<COMMSENDMAX; i++)
       if(Multi_Comm_Buff[i].isValid == 1)
        {
         HaveDataSend = 1;
         break;
        }
  //    if(HaveDataSend == 0)
  //      LCD_Bak.isFinished = 1;
     }
     }
   }
}
//---------------------------------------------------------------------------
//读配置文件
void ReadCfgFile(void)
{
  FILE *cfg_fd;
  int bytes_write;
  int bytes_read;
  uint32_t Ip_Int;
  int ReadOk;
  DIR *dirp;
  int i,j;
  //查找配置文件目录是否存在
  if((dirp=opendir("/mnt/mtd/config")) == NULL)
   {
     if(mkdir("/mnt/mtd/config", 1) < 0)
       printf("error to mkdir /mnt/mtd/config\n");
   }
  if(dirp != NULL)
   {
    closedir(dirp);
    dirp = NULL;
   }

//  unlink(cfg_name);
  ReadOk = 0;
  while(ReadOk == 0)
   {
    //打开配置文件
  //  if(access(cfg_name, R_OK|W_OK) == NULL)
    if((cfg_fd=fopen(cfg_name,"rb"))==NULL)
     {
      printf("配置文件不存在，创建新文件\n");
     // if((cfg_fd = open(cfg_name, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) == -1)
      if((cfg_fd = fopen(cfg_name, "wb")) == NULL)
        printf("无法创建配置文件\n");
      else
       {
        //地址编码
        memcpy(LocalCfg.Addr,NullAddr,20);
        memcpy(LocalCfg.Addr,"S00010108080",12);
        //网卡地址
        LocalCfg.Mac_Addr[0] = 0x00;
        LocalCfg.Mac_Addr[1] = 0x50;
        LocalCfg.Mac_Addr[2] = 0x2A;
        LocalCfg.Mac_Addr[3] = 0x33;
        LocalCfg.Mac_Addr[4] = 0x44;
        LocalCfg.Mac_Addr[5] = 0x66;
        //IP地址
        Ip_Int=inet_addr("192.168.1.190");
        memcpy(LocalCfg.IP,&Ip_Int,4);
        //子网掩码
        Ip_Int=inet_addr("255.255.255.0");
        memcpy(LocalCfg.IP_Mask,&Ip_Int,4);
        //网关地址
        Ip_Int=inet_addr("192.168.1.1");
        memcpy(LocalCfg.IP_Gate,&Ip_Int,4);
        //NS（名称解析）服务器地址
        Ip_Int=inet_addr("192.168.1.161");
        memcpy(LocalCfg.IP_NS,&Ip_Int,4);
        //主服务器地址
        Ip_Int=inet_addr("192.168.1.161");
        memcpy(LocalCfg.IP_Server,&Ip_Int,4);
        //广播地址
        Ip_Int=inet_addr("192.168.1.255");
        memcpy(LocalCfg.IP_Broadcast,&Ip_Int,4);
        
        //设备定时报告状态时间
        LocalCfg.ReportTime = 10;
        //布防状态
        LocalCfg.DefenceStatus = 0;
        //防区模块个数
        LocalCfg.DefenceNum = 1;
        for(i=0; i<8; i++)
         for(j=0; j<10; j++)
          LocalCfg.DefenceInfo[i][j] = 0;
        //工程密码
        strcpy(LocalCfg.EngineerPass, "123456");
        //进入延时
        LocalCfg.In_DelayTime = 30;
        //外出延时
        LocalCfg.Out_DelayTime = 30;
        //报警延时
        LocalCfg.Alarm_DelayTime = 30;

		LocalCfg.misscall_num = 0;
		LocalCfg.missmessage_num = 0;
		LocalCfg.news_num = 0;

        //触摸屏
        LocalCfg.Ts_X0 = 491;//1901;
        LocalCfg.Ts_Y0 = 499;//2001;
        LocalCfg.Ts_deltaX = -830;//3744;
        LocalCfg.Ts_deltaY = 683;//3555;
        
        bytes_write=fwrite(&LocalCfg, sizeof(LocalCfg), 1, cfg_fd);
        fclose(cfg_fd);
       }
     }
    else
     {
        ReadOk = 1;
      //  bytes_read=read(cfg_fd, Local.Addr, 12);
        bytes_read=fread(&LocalCfg, sizeof(LocalCfg), 1, cfg_fd);
        //防区模块个数
        LocalCfg.DefenceNum = 1;
        //进入延时
        LocalCfg.In_DelayTime = 10;
        //外出延时
        LocalCfg.Out_DelayTime = 10;
        //报警延时
        LocalCfg.Alarm_DelayTime = 10;        

        if(bytes_read != 1)
         {
          printf("读取配置文件失败,以默认方式重建配置文件\n");
          ReadOk = 0;
          fclose(cfg_fd);
          unlink(cfg_name);
         }
        else
         {
          RefreshNetSetup(0); //刷新网络设置
            //广播地址
            for(i=0; i<4; i++)
             {
              if(LocalCfg.IP_Mask[i] != 0)
                LocalCfg.IP_Broadcast[i] = LocalCfg.IP_Mask[i] & LocalCfg.IP[i];
              else
                LocalCfg.IP_Broadcast[i] = 0xFF;
             }
          fclose(cfg_fd);
         }
       }
   }
}
//---------------------------------------------------------------------------
//写本地设置文件
void WriteCfgFile(void)
{
	FILE *cfg_fd;

	if((cfg_fd = fopen(cfg_name, "wb")) == NULL)
		printf("无法创建配置文件\n");
	else
	{
		//重写本地设置文件
		fwrite(&LocalCfg, sizeof(LocalCfg), 1, cfg_fd);
		fclose(cfg_fd);
	}
}

//---------------------------------------------------------------------------
//读信息文件
void ReadInfoFile(void)
{
	int i;
	int j;
	FILE *info_fd;
	//  int info_fd;
	int bytes_write;
	int bytes_read;
	int ReadOk;
	DIR *dirp;
	struct InfoContent1 InitInfoCon;
	//查找配置文件目录是否存在
	if((dirp=opendir("/mnt/mtd/config")) == NULL)
	{
		if(mkdir("/mnt/mtd/config", 1) < 0)
			printf("error to mkdir /mnt/mtd/config\n");
	}
	if(dirp != NULL)
	{
		closedir(dirp);
		dirp = NULL;
	}

	//  unlink(cfg_name);
	ReadOk = 0;
	while(ReadOk == 0)
	{
		//打开信息文件
		if((info_fd=fopen(info_name,"rb"))==NULL)
		{
			printf("信息文件不存在，创建新文件\n");
			if((info_fd = fopen(info_name, "wb")) == NULL)
				printf("无法创建信息文件\n");
			else
			{
				InitInfoCon.isValid = 0;
				InitInfoCon.isReaded = 1;
				InitInfoCon.isLocked = 0;
				strcpy(InitInfoCon.Time, "");
				InitInfoCon.Type = 1;
				InitInfoCon.Sn = 0;
				InitInfoCon.Length = 0;
				strcpy(InitInfoCon.Content, "中华人民共和国");

				for(i = 0; i < INFOTYPENUM; i++)
				{
					InitInfoCon.Type = i + 1;
					for(j = 0; j < Info[i].MaxNum; j++)
						bytes_write=fwrite(&InitInfoCon, sizeof(InitInfoCon), 1, info_fd);
				}
				fclose(info_fd);
			}
		}
		else
		{
			ReadOk = 1;
			for(i = 0; i < INFOTYPENUM; i++)
			{
				InitInfoCon.Type = i + 1;
				for(j = 0; j < Info[i].MaxNum; j++)
				{
					bytes_read=fread(&InitInfoCon, sizeof(InitInfoCon), 1, info_fd);
					if(bytes_read != 1)
					{
						printf("读取信息文件失败,以默认方式重建信息文件\n");
						ReadOk = 0;
						fclose(info_fd);
						unlink(info_name);
						i = INFOTYPENUM;
						break;
					}
					if(ReadOk == 1)
					{
						if(InitInfoCon.isValid == 1)
						{
							creat_infonode(InfoNode_h[i], InitInfoCon);
						}
						else
						{
							fseek(info_fd, sizeof(InitInfoCon)*(Info[i].MaxNum - j - 1), SEEK_CUR);
							break;
						}
					}
				}
			}
			if(ReadOk == 1)
				fclose(info_fd);
		}
	}
}
//---------------------------------------------------------------------------
//读照片文件
void ReadPictureFile(void)
{
	int i;
	int j;
	FILE *pic_fd;
	int bytes_write;
	int bytes_read;
	DIR *dirp;
	struct InfoContent1 InitPicCon;
	//查找配置文件目录是否存在
	if((dirp=opendir("/mnt/mtd/picture")) == NULL)
	{
		if(mkdir("/mnt/mtd/picture", 1) < 0)
			printf("error to mkdir /mnt/mtd/picture\n");
	}
	if(dirp != NULL)
	{
		closedir(dirp);
		dirp = NULL;
	}

	//初始化链表头结点
	for(i=0; i<2; i++)
	if(PicNode_h[i] == NULL)
		PicNode_h[i] = init_infonode();


	//打开呼叫照片配置文件
	if((pic_fd=fopen(picini_name1, "rb")) !=NULL)
	{
		for(i = 0; i < PICNUM; i++)
		{
			bytes_read=fread(&InitPicCon, sizeof(InitPicCon), 1, pic_fd);
			if(bytes_read == 1)
			{
				if(InitPicCon.isValid == 1)
					creat_infonode(PicNode_h[0], InitPicCon);
				else
					break;
			}
			else
				break;
		}
		fclose(pic_fd);
	}

	PicStrc[0].MaxNum = PICNUM;
	PicStrc[0].TotalNum = length_infonode(PicNode_h[0]);
	printf("PicStrc[0].TotalNum = %d\n", PicStrc[0].TotalNum);
	PicStrc[0].CurrentInfoPage = 1;

}
//---------------------------------------------------------------------------
InfoNode1 * init_infonode(void) //初始化单链表的函数
{
  InfoNode1 *h; // *h保存表头结点的指针，*p指向当前结点的前一个结点，*s指向当前结点
  if((h=(InfoNode1 *)malloc(sizeof(InfoNode1)))==NULL) //分配空间并检测
  {
    printf("不能分配内存空间!");
    return NULL;
  }
  h->llink=NULL; //左链域
  h->rlink=NULL; //右链域
  return(h);
}
//---------------------------------------------------------------------------
//函数名称：creat
//功能描述：在链表尾部添加数据
//返回类型：无返回值
//函数参数： h:单链表头指针
int creat_infonode(InfoNode1 *h, struct InfoContent1  TmpContent)
{
    InfoNode1 *t;
    InfoNode1 *p;
    t=h;
  //  t=h->next;
    while(t->rlink!=NULL)    //循环，直到t指向空
      t=t->rlink;   //t指向下一结点
    if(t)
    {
      //尾插法建立链表
       if((p=(InfoNode1 *)malloc(sizeof(InfoNode1)))==NULL) //生成新结点s，并分配内存空间
       {
        printf("不能分配内存空间!\n");
        return 0;
       }
      p->Content = TmpContent;
      p->rlink=NULL;    //p的指针域为空
      p->llink=t;
      t->rlink=p;       //p的next指向这个结点
      t=p;             //t指向这个结点
      return 1;
    }
}
//---------------------------------------------------------------------------
//函数名称：print
//功能描述：遍历单链表
//返回类型：无返回值
//函数参数：h:单链表头指针
void print(InfoNode1 *h)
{
    InfoNode1 *p;
    p=h->rlink;
    while(p)
    {
      //  printf("%c",p->data); //输出p的数值域
        p=p->rlink;            //p指向下一结点
    }
}
//---------------------------------------------------------------------------
//函数名称：length
//功能描述：求单链表长度
//返回类型：无返回值
//函数参数：h:单链表头指针
int length_infonode(InfoNode1 *h)
{
    InfoNode1 *p;
    int i=0;         //记录链表长度
    p=h->rlink;
    while(p!=NULL)    //循环，直到p指向空
    {
        i=i+1;
        p=p->rlink;   //p指向下一结点
     }
    return i;
 //    printf(" %d",i); //输出p所指接点的数据域
}
//---------------------------------------------------------------------------
//函数名称：length
//功能描述：查找未读信息
//返回类型：无返回值
//函数参数：h:单链表头指针
int length_infonoreaded(InfoNode1 *h)
{
    InfoNode1 *p;
    int i=0;         //记录链表长度
    p=h->rlink;
    while(p!=NULL)    //循环，直到p指向空
     {
      if(p->Content.isReaded == 0)
         i=i+1;
       p=p->rlink;   //p指向下一结点
     }
    return i;
 //    printf(" %d",i); //输出p所指接点的数据域
}
//---------------------------------------------------------------------------
//函数名称：insert
//功能描述：插入函数
//返回类型：整型
//函数参数：h:单链表头指针 x:要插入的元素 i：要插入的位置

//假若s,p,q是连续三个结点的指针，若我们要在p前插入一个新结点r，
//则只需把s的右链域指针指向r，r的左链域指针指向s，r的右链域指针指向p，p的左链域指针指向r即可。
int insert_infonode(InfoNode1 *h, InfoNode1 *p, struct InfoContent1  TmpContent)
{
  InfoNode1 *s;
  if((s=(InfoNode1 *)malloc(sizeof(InfoNode1)))==NULL) //生成新结点s，并分配内存空间
   {
    printf("不能分配内存空间!\n");
    return 0;
   }
  s->Content = TmpContent;        //把TmpContent赋值给s的数据域
  //p->llink  s  p
  //不为第一个
  if(p != NULL)
   {
    (p->llink)->rlink = s;
    s->llink = p->llink;
    s->rlink = p;
    p->llink = s;
   }
  else
   {
    h->rlink = s;
    s->llink = h;
    s->rlink = p;
   }

/*  s->rlink=p->rlink;
  p->rlink=s;
  s->llink=p;
  (s->rlink)->llink=s;
         */
  return(1);         //如果成功返回1
}
//---------------------------------------------------------------------------
//函数名称：delete_
//功能描述：删除函数
//返回类型：整型
//函数参数：h:单链表头指针 i:要删除的位置
int delete_infonode(InfoNode1 *p)
{
  //未锁定
  if(p->Content.isLocked == 0)
   {
    //不为最后一个结点
    if(p->rlink != NULL)
     {
      (p->rlink)->llink=p->llink;
      (p->llink)->rlink=p->rlink;
      free(p);
     }
    else
     {
      (p->llink)->rlink=p->rlink;
      free(p);
     }
    return(1);
   }
  return(0);
}
//---------------------------------------------------------------------------
int delete_all_infonode(InfoNode1 *h)
{
  InfoNode1 *p,*q;
  p=h->rlink;        //此时p为首结点
  while(p != NULL)   //找到要删除结点的位置
   {
    if(p->Content.isLocked == 0)
     {
      //不为最后一个结点
      q = p;
      if(p->rlink != NULL)
       {
        (p->rlink)->llink=p->llink;
        (p->llink)->rlink=p->rlink;
       }
      else
        (p->llink)->rlink=p->rlink;
      p = p->rlink;
      free(q);
     }
    else
      p = p->rlink;
   }
}
//---------------------------------------------------------------------------
//函数名称：locate_
//功能描述：定位函数
//返回类型：整型
//函数参数：h:单链表头指针 i:要定位的位置
InfoNode1 * locate_infonode(InfoNode1 *h,int i)
{
  InfoNode1 *p;
  int j;
  p=h->rlink;    //此时p为首结点
  j=1;
  while(p&&j<i)  //找到要定位的位置
   {
    ++j;
    p=p->rlink;  //p指向下一结点
   }
  if(i>0&&j==i)
    return p;
  else
    return NULL;
}
//---------------------------------------------------------------------------
//函数名称：find_
//功能描述：查找函数
//返回类型：整型
//函数参数：h:单链表头指针 x:要查找的值
InfoNode1 * find_infonode(InfoNode1 *h, struct InfoContent1  TmpContent)
{
  InfoNode1 *p;
  p=h->rlink;    //此时p为首结点
  while(p!=NULL&&p->Content.Sn != TmpContent.Sn) //进入循环，直到p为空，或找到x
    p=p->rlink;   //s指向p的下一结点
  if(p!=NULL)
    return p;
  else
    return NULL;
}
//---------------------------------------------------------------------------
int free_infonode(InfoNode1 *h)
{
  InfoNode1 *p,*t;
  int i=0;         //记录链表长度
  p=h->rlink;
  while(p!=NULL)    //循环，直到p指向空
   {
    i=i+1;
    t = p;
    p=p->rlink;   //p指向下一结点
    free(t);
   }
  return i;
}
//---------------------------------------------------------------------------
int             lock=0;
volatile int    item_select = 0, allitem = 3;
volatile int    startd=0;
pid_t           gpid;
//char            exename[20];
char            poolname[20];
int 	fbfd = 0;

#define FOSD_SETPOS    		0x46e1
#define FOSD_SETDIM    		0x46e2
#define FOSD_SETSCAL    	0x46e3
#define FLCD_SET_TRANSPARENT    0x46e4
#define FLCD_SET_STRING    	0x46e5
#define FOSD_ON    		0x46e6
#define FOSD_OFF    		0x46e7


#define red_fg		0x0
#define green_fg	0x4
#define blue_fg		0x8
#define white_fg	0xC
#define tran_bg 	0x0
#define red_bg		0x1
#define green_bg	0x2
#define blue_bg		0x3

struct fosd_data    f_data1;

struct fosd_string
{
	unsigned int Str_row;
	unsigned int display_mode;
	unsigned int fg_color; 
	unsigned int bg_color;
	unsigned char Str_OSD[30];	
};

struct fosd_data
{
	unsigned int HPos;
	unsigned int VPos;
	unsigned int HDim;
	unsigned int VDim;
	unsigned int transparent_level;
	unsigned int HScal;
	unsigned int VScal;
	struct fosd_string Str_Data[5];	
};	
void sig_pwr(void)
{
    unsigned char 	array[][30] = {	"00,00",
    				     	"> PLAY WMA  ",  
    					"> PLAY AAC  ",  
    					"            ", 
    					"AUDIO DEMO  ",
    					"FARADAY !@#$",				    					    					
    					};
    
    if(startd)
    {
        startd=0;
        
    	if (ioctl(fbdev, FOSD_OFF, &f_data1)) {
            printf("DISABLE OSD FAIL.\n");
        }        
    }
    else
    {  
        startd=1;

	f_data1.HPos = 20;
	f_data1.VPos = 30;
	f_data1.HDim = 5;
	f_data1.VDim = 1;//allitem+3;
	f_data1.transparent_level = 3;
	f_data1.HScal = 1;
	f_data1.VScal = 1;

	f_data1.Str_Data[0].Str_row = 0;
	f_data1.Str_Data[0].display_mode = 2;
	f_data1.Str_Data[0].fg_color = green_fg;
	f_data1.Str_Data[0].bg_color = 0;//green_bg;
	memcpy(f_data1.Str_Data[0].Str_OSD, array[0], f_data1.HDim);

    	if (ioctl(fbdev, FOSD_ON, &f_data1)) {
            printf("Enable OSD FAIL.\n");
        }
    	if (ioctl(fbdev, FOSD_SETPOS, &f_data1)) {
            printf("FOSD_SETPOS FAIL.\n");
        }
    	if (ioctl(fbdev, FOSD_SETDIM, &f_data1)) {
            printf("FOSD_SETDIM FAIL.\n");
        }
    	if (ioctl(fbdev, FLCD_SET_TRANSPARENT, &f_data1)) {
            printf("FLCD_SET_TRANSPARENT FAIL.\n");
        }
    	if (ioctl(fbdev, FLCD_SET_STRING, &f_data1)) {
            printf("FLCD_SET_STRING FAIL.\n");
        }  
    	if (ioctl(fbdev, FOSD_SETSCAL, &f_data1)) {
            printf("FOSD_SETSCAL FAIL.\n");
        }                                             
    }        
    item_select = 0;
}
//---------------------------------------------------------------------------
void OpenOsd(void)   //打开OSD
{
  if(Local.OsdOpened == 0)  //OSD打开标志
   {
    f_data1.HPos = 70;
    f_data1.VPos = 134;
    f_data1.HDim = 5;
    f_data1.VDim = 1;//allitem+3;
    f_data1.transparent_level = 3;
    f_data1.HScal = 1;
    f_data1.VScal = 1;

    f_data1.Str_Data[0].Str_row = 0;
    f_data1.Str_Data[0].display_mode = 2;
    f_data1.Str_Data[0].fg_color = green_fg;
    f_data1.Str_Data[0].bg_color = 0;//green_bg;
    memcpy(f_data1.Str_Data[0].Str_OSD, "00,00", f_data1.HDim);
    if (ioctl(fbdev, FOSD_ON, &f_data1))
       printf("Enable OSD FAIL.\n");
    else
      Local.OsdOpened = 1;
   }
}
//---------------------------------------------------------------------------
void CloseOsd(void)  //关闭OSD
{
  if(Local.OsdOpened == 1)  //OSD打开标志
   {
    if (ioctl(fbdev, FOSD_OFF, &f_data1))
      printf("DISABLE OSD FAIL.\n");
    else
      Local.OsdOpened = 0;
   }
}
//---------------------------------------------------------------------------
void ShowOsd(char *Content) //显示OSD内容
{
  if(Local.OsdOpened == 1)  //OSD打开标志
   {
        if(/*(Local.PlayPicSize == 1)&&*/(Local.CurrFbPage == 0))
         {
          f_data1.HPos = 156;
          f_data1.VPos = 134;
         }
        if(/*(Local.PlayPicSize == 2)&&*/(Local.CurrFbPage == 1))
         {
  	  f_data1.HPos = 50;//80;//20;
	  f_data1.VPos = 30;//70;//20;
          if((SCRWIDTH == 800)&&(SCRHEIGHT == 600))
            f_data1.VPos = 94;
         }

	f_data1.HDim = 5;
	f_data1.VDim = 1;
	f_data1.transparent_level = 3;
	f_data1.HScal = 1;
	f_data1.VScal = 1;

	f_data1.Str_Data[0].Str_row = 0;
	f_data1.Str_Data[0].display_mode = 2;
	f_data1.Str_Data[0].fg_color = green_fg;
	f_data1.Str_Data[0].bg_color = 0;//green_bg;
	memcpy(f_data1.Str_Data[0].Str_OSD, Content, f_data1.HDim);

    	if (ioctl(fbdev, FOSD_SETPOS, &f_data1)) {
            printf("FOSD_SETPOS FAIL.\n");
        }
    	if (ioctl(fbdev, FOSD_SETDIM, &f_data1)) {
            printf("FOSD_SETDIM FAIL.\n");
        }
    	if (ioctl(fbdev, FLCD_SET_TRANSPARENT, &f_data1)) {
            printf("FLCD_SET_TRANSPARENT FAIL.\n");
        }
    	if (ioctl(fbdev, FLCD_SET_STRING, &f_data1)) {
            printf("FLCD_SET_STRING FAIL.\n");
        }  
    	if (ioctl(fbdev, FOSD_SETSCAL, &f_data1)) {
            printf("FOSD_SETSCAL FAIL.\n");
        }           
   }
}
//---------------------------------------------------------------------------
//PMU 关闭时钟
void SetPMU(void)
{
  unsigned int PMU1, PMU2;
  PMU1 = 0;
  PMU2 = 0;
  PMU1 |= 0x8000; //15	HS18OFF	R/W	Turns off the clock of the hs18_hclk(for MPCA)
  PMU1 |= 0x4000; //14	HS17OFF	R/W	Turns off the clock of the hs17_hclk(for MPCA)
  PMU1 |= 0x2000; //13	HS14OFF	R/W	Turns off the clock of the hs14_hclk(for MPCA)
  PMU1 |= 0x1000; //12	HS13OFF	R/W	Turns off the clock of the hs13_hclk(for MPCA)
  PMU1 |= 0x0800; //11	HS11OFF	R/W	Turns off the clock of the hs11_hclk(for MPCA)
  PMU1 |= 0x0400; //10	IDEOFF	R/W	Turns off the clock of the IDE controller
  PMU1 |= 0x0200; //9	PCIOFF	R/W	Turns off the clock of the PCI controller
  //0x0100 //8	MACOFF	R/W	Turns off the clock of the MAC controller
  //0x80   //7	DMAOFF	R/W	Turns off the clock of the DMA controller
       //6	-	-	Reserved	-	-
  //0x20   //5	MCPOFF	R/W	Turns off the clock of the MCP controller
  //0x10   //4	LCDOFF	R/W	Turns off the clock of the LCD controller
  //0x08   //3	SDRAMOFF	R/W	Turns off the clock of the SDRAM controller
  //PMU1 |= 0x04;   //2	CAPOFF	R/W	Turns off the clock of the Capture controller
  //PMU1 |= 0x02;   //1	OTGOFF	R/W	Turns off the clock of the USB 2.0 OTG controller
  //0x01   // MEMOFF	R/W	Turns off the clock of the SRAM controller

//  PMU2 |= 0x040000; //18	UART4OFF	R/W	Turns off the clock of the UART 4 module
  PMU2 |= 0x020000; //17	UART3OFF	R/W	Turns off the clock of the UART 3 module
//  PMU2 |= 0x010000; //16	UART2OFF	R/W	Turns off the clock of the UART 2 module
  //0x8000 //15	PS28OFF	R/W	Turns off the clock of the ps28_pclk
  //0x4000 //14	PS27OFF	R/W	Turns off the clock of the ps27_pclk
  //0x2000 //13	PS26OFF	R/W	Turns off the clock of the ps26_pclk
  //0x1000 //12	PS25OFF	R/W	Turns off the clock of the ps25_pclk
  //0x0800 //11	PS24OFF	R/W	Turns off the clock of the ps24_pclk
  //0x0400 //10	PS23OFF	R/W	Turns off the clock of the ps23_pclk
  //0x0200 //9	MCLKOFF	R/W	Turns off the main clock of audio CODEC
  //0x0100 //8	RTCOFF	R/W	Turns off the clock of the RTC module
  //0x80   //7	SSP1OFF	R/W	Turns off the clock of the SSP 1 module
  PMU2 |= 0x40;   //6	SDCOFF	R/W	Turns off the clock of the SDC controller
  //0x20   //5	I2COFF	R/W	Turns off the clock of the I2C controller
  //0x10   //4	TIMEROFF	R/W	Turns off the clock of the TIMER module
  //0x08   //3	UART1OFF	R/W	Turns off the clock of the UART 1 module
  //0x04   //2	INTCOFF	R/W	Turns off the clock of the INTC controller
  //0x02   //1	WDTOFF	R/W	Turns off the clock of the WDT controller
  //0x01   //0	GPIOOFF	R/W	Turns off the clock of the GPIO controller
  //PMU 关闭时钟   关闭IDE  关闭PCI
  ioctl(gpio_fd, CLOSE_PMU1, PMU1); 
  ioctl(gpio_fd, CLOSE_PMU2, PMU2);
}
//---------------------------------------------------------------------------

void SaveToFlash(int savetype)    //向Flash中存储文件
{
	int i;

	pthread_mutex_lock (&Local.save_lock);
	for(i=0; i<SAVEMAX; i++)
	if(Save_File_Buff[i].isValid == 0)
	{
		Save_File_Buff[i].Type = savetype;      //存储本地设置
		Save_File_Buff[i].isValid = 1;
		sem_post(&save_file_sem);     
		break;
	}
	pthread_mutex_unlock (&Local.save_lock);
}
void WriteInfoFile(int InfoType)
{
	FILE *info_fd;
	InfoNode1 *tmp_node;
	int SeekLength;
	int j;
	uint8_t isValid;

	printf("write a info file!\n");
	if((info_fd = fopen(info_name, "rb+")) == NULL)
		printf("无法打开信息文件\n");
	else
	{
		//重写该类信息文件
		SeekLength = 0;
		for(j=0; j<InfoType; j++)
			SeekLength += Info[j].MaxNum;
		fseek(info_fd, sizeof(tmp_node->Content)*SeekLength, SEEK_SET);
		tmp_node=InfoNode_h[InfoType];
		for(j = 0; j < Info[InfoType].TotalNum; j++)
		{
			tmp_node = tmp_node->rlink;
			if(tmp_node != NULL)
				fwrite(&tmp_node->Content, sizeof(tmp_node->Content), 1, info_fd);
		}
		//写无用信息
		isValid = 0;
		for(j = Info[InfoType].TotalNum; j < Info[InfoType].MaxNum; j++)
		{
			fwrite(&isValid, sizeof(isValid), 1, info_fd);
			fseek(info_fd, (sizeof(tmp_node->Content)-1), SEEK_CUR);
		}
		fclose(info_fd);
	}
	printf("end write a info file!\n");
}

