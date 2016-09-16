#include <inttypes.h>
#include <signal.h>
#include <semaphore.h>       //sem_t
#include <sys/stat.h>
#include <pthread.h>
#include "sndtools.h"
#include "./include/image.h"

#define MainWindow				0
#define TalkMenuWindow			3
#define TalkPicWindow			31
#define TalkAreaMessageWindow 	32
#define TalkLocalMessageWindow	33
#define TalkPushMessageWindow	34
#define TalkCustomMessageWindow	35
#define TalkMisscallWindow		36
#define TalkCalledWindow		37
#define TalkCallWindow			38
#define TalkPhonebookWindow		39
#define TalkBlacklistWindow		40

#define SetupMainWindow			200

#define LanSetWindow			41
#define WlanSetWindow			42
#define RoomSetWindow			43
#define ScreenContrastWindow	44
#define ScreenSavingWindow		45
#define ScreenCalibrateWindow	46
#define VoiceSetWindow			47
#define TimeSetWindow			48
#define PassSetWindow			49
#define LangSetWindow			50
#define UpgradeSDWindow			51
#define UpgradeRemoteWindow		52
#define SystemInfoWindow		53
#define TalkInfoContentWindow   54






#define cfg_name "/mnt/mtd/config/cfg"
#define picinfo_name "/mnt/mtd/config/picinfo"
#define info_name "/mnt/mtd/config/info"
#define picini_name1 "/mnt/mtd/picture/picini1"
#define picini_name2 "/mnt/mtd/picture/picini2"
#define mtdexe_name "/mnt/mtd/sound70"
//#define UdpPackageHead  "XXXCID"
#define FLAGTEXT   "hikdsdkkkkdfdsIMAGE"
#define FLAGTEXT43 "hikdsdkkkkdfdsPIC70"

//#define _ZHUHAIJINZHEN      //珠海进祯  1、呼叫时可开锁  2、通话时开锁2秒后挂断 20081127
#define _DEBUG           //调试模式
//#define _R2RVIDEO        //户户对讲视频
#define CIF_X    128
#define CIF_Y    124
#define MINIDOOR_OPENLOCK_IO          2   //小门口机开锁（高电平有效）
#define MINIDOOR_POWER_IO             3   //小门口机电源（低电平有效）
#define MINIDOOR_AUDIO_IO             4   //小门口机音频切换（低电平有效）

//#define _TESTNSSERVER        //测试服务器解析模式
//#define _TESTTRANS           //测试视频中转模式

#define HARDWAREVER "S-HW VER 1.0"    //硬件版本
#define SOFTWAREVER "S-SW VER 1.0"    //软件版本
#define SERIALNUM "ET20080816"    //产品序列号

#define NSMULTIADDR  "238.9.9.1"  //NS组播地址

#define ZOOMMAXTIME 2000   //放大缩小延迟处理时间
#define TOUCHMAXTIME 300   //触摸屏处理延迟处理时间

#define INTRTIME 50       //线程50ms
#define INTRPERSEC 20       //每秒20次线程
#define BUFFER_SIZE 1024 
#define FRAMEBUFFERMAX  4
#define COMMMAX 1024     //串口缓冲区最大值
#define SCRWIDTH  800
#define SCRHEIGHT  600//600//480
#define REFRESH  1
#define NOREFRESH 0
#define SHOW  0
#define HIDE  1
#define HILIGHT  2
#define IMAGEUP  0
#define IMAGEDOWN  1
#define CIF_W    352
#define CIF_H    240
#define D1_W    720
#define D1_H    480
#define TIPW     320     //提示条宽度
#define TIPH     28      //提示条高度
#define DIRW     322     //提示条宽度
#define DIRH     20      //提示条高度
#define TIPX     320     //提示条X
#define TIPY     24      //提示条Y
#define CLOCKW     75     //时钟宽度
#define CLOCKH     20      //时钟高度
#define CLOCKX     514     //时钟X
#define CLOCKY     30       //时钟Y
#define WEATHERW     43     //天气宽度
#define WEATHERH     20      //天气高度
#define WEATHERX     665     //天气X
#define WEATHERY     30       //天气Y
#define STATEX     0     //状态条图标X
#define STATEY     454     //状态条图标Y

#define MAXCOUNT   6      //防区最大数量

#define CALLCENTERX     150     //呼叫中心X
#define CALLCENTERY     224    //呼叫中心Y

#define R2RX     150             //户户通话X
#define R2RY     330            //户户通话Y

#define WATCHX     150     //监视X
#define WATCHY     224    //监视Y

#define CALLX     150     //呼叫X
#define CALLY     224    //呼叫Y

#define SETUP1X     50     //设置窗口1X
#define SETUP1Y     50    //设置窗口1Y

#define INFOROWLEN   32    //信息每行长度
#define MAXROW  12          //最大行数
#define PAGEPERROW  3          //页行数

#define WATCHTIMEOUT  30*(1000/INTRTIME)    //监视最长时间
#define CALLTIMEOUT  25*(1000/INTRTIME)     //呼叫最长时间
#define TALKTIMEOUT  130*(1000/INTRTIME)//30*20     //通话最长时间
#define PREPARETIMEOUT  10*(1000/INTRTIME)     //留影留言预备最长时间
#define RECORDTIMEOUT  30*(1000/INTRTIME)     //留影留言最长时间

//命令 管理中心
#define ALARM         1    //报警
#define CANCELALARM   2    //取消报警
#define SENDMESSAGE   3   //发送信息
#define REPORTSTATUS  4   //设备定时报告状态
#define QUERYSTATUS   5   //管理中心查询设备状态
#define REMOTEDEFENCE   20   //远程布防
#define RESETPASS       30   //复位密码
#define WRITEADDRESS   40   //写地址设置
#define READADDRESS    41   //读地址设置
#define WRITEROOMSETUP     44   //写室内机工程设置
#define READROOMSETUP      45   //读室内机工程设置
#define WRITESETUP     52   //写单元门口机、围墙机设置信息
#define READSETUP      53   //读单元门口机、围墙机设置信息
//对讲
#define VIDEOTALK      150 //局域网可视对讲
#define VIDEOTALKTRANS 151 //局域网可视对讲中转服务
#define VIDEOWATCH     152 //局域网监控
#define VIDEOWATCHTRANS   153 //局域网监控中转服务
#define NSORDER        154 //主机名解析（子网内广播）
#define NSSERVERORDER  155 //主机名解析(NS服务器)
#define FINDEQUIP      170 //查找设备

#define ASK              1     //命令类型 主叫
#define REPLY            2     //命令类型 应答

#define CALL             1     //呼叫
#define LINEUSE          2     //占线
#define QUERYFAIL        3      //通信失败
#define CALLANSWER       4     //呼叫应答
#define CALLSTART        6     //开始通话

#define CALLUP           7     //通话数据1（主叫方->被叫方）
#define CALLDOWN         8     //通话数据2（被叫方->主叫方）
#define CALLCONFIRM      9     //通话在线确认（接收方发送，以便发送方确认在线）
#define REMOTEOPENLOCK   10     //远程开锁
#define FORCEIFRAME      11     //强制I帧请求
#define ZOOMOUT          15     //放大(720*480)
#define ZOOMIN           16     //缩小(352*288)

#define CALLEND          30     //通话结束

#define DOWNLOADFILE  224    //下载应用程序
#define DOWNLOADIMAGE  225    //下载系统映像
#define STARTDOWN  1       //开始下载
#define DOWN       2       //下载
#define DOWNFINISHONE       3  //下载完成一个
#define STOPDOWN       10      //停止下载
#define DOWNFINISHALL       20 //全部完成下载
#define DOWNFAIL         21 //下载失败  设备－》管理中心

#define ERASEFLASH  31    //正在删除Flash
#define WRITEFLASH  32    //正在写Flash
#define CHECKFLASH  33    //正在校验Flash
#define ENDFLASH  34      //完成写Image
#define ERRORFLASH  35      //操作Image失败


#define MAINPICNUM  24      //首页图片数量
#define MAINLABELNUM  2     //首页Label数量
#define DOWNLOAD  220      //下载
#define ASK  1
#define REPLY  2

#define SAVEMAX  50     //FLASH存储缓冲最大值
#define UDPSENDMAX  50  //UDP多次发送缓冲最大值
#define COMMSENDMAX  10  //COMM多次发送缓冲最大值
#define MAXSENDNUM  6  //最大发送次数

//按钮压下时间
#define DELAYTIME  200
//按钮数量
#define InfoButtonMax  16
//短信息
#define INFOTYPENUM 4 //4    //短信息类型
#define INFOMAXITEM  50 //200    //短信息最大条数
#define INFOMAXSIZE  400 //短信息内容最大容量
#define INFONUMPERPAGE 9  //一页显示信息数
//照片
#define PICNUM   20

//视频常量
#define cWhite  1
#define cYellow 2
#define cCyan   3
#define cGreen  4
#define cMagenta  5
#define cRed      6
#define cBlue     7
#define cBlack    8
#define	FB_DEV	"/dev/fb0"

#define VIDEO_PICTURE_QUEUE_SIZE_MAX 20

#define CONFLICTARP  0x8950
#define FLCD_GET_DATA_SEP   0x46db
#define FLCD_GET_DATA       0x46dc
#define FLCD_SET_FB_NUM     0x46dd


#define FLCD_SWITCH_MODE    0x46de
#define FLCD_CLOSE_PANEL    0x46df
#define FLCD_BYPASS    0x46e0
#define FLCD_OPEN	0x46fa
#define FLCD_CLOSE	0x46fb

#define DEVICE_GPIO                "/dev/gpio"
#define IO_PUT                 0
#define IO_CLEAR               3
#define IO_READ         4
#define IO_SETINOUT     5
#define IO_TRIGGERMODE  6
#define IO_EDGE         7
#define IO_SETSCANVALUE     8
#define IO_SETVALUE     9

//20080401 设置PMU，关闭不用的时钟
#define CLOSE_PMU1  0x2255
#define CLOSE_PMU2  0x2256
//20080802 检查Audio play ring stoped
#define CHECK_PLAY_RING  0x2257

#define NMAX 512*64  //AUDIOBLK*64  //音频环形缓冲区大小
#define G711NUM  64*512/AUDIOBLK       //音频接收缓冲区个数 未解码   10

#define VIDEOMAX 720*480
#define VNUM  3         //视频采集缓冲区大小
#define VPLAYNUM  10         //视频播放缓冲区大小         6
#define MP4VNUM  20         //视频接收缓冲区个数 未解码   10
#define PACKDATALEN  1200   //数据包大小
#define MAXPACKNUM  100     //帧最大数据包数量

struct TimeStamp1{
    unsigned int OldCurrVideo;     //上一次当前视频时间
    unsigned int CurrVideo;
    unsigned int OldCurrAudio;     //上一次当前音频时间
    unsigned int CurrAudio;
   };
//视频采集缓冲
struct videobuf1
 {
  int iput; // 环形缓冲区的当前放入位置
  int iget; // 缓冲区的当前取出位置
  int n; // 环形缓冲区中的元素总数量
  uint32_t timestamp[VNUM]; //时间戳

  uint32_t frameno[VNUM];   //帧序号
  unsigned char *buffer_y[VNUM];//[VIDEOMAX];
  unsigned char *buffer_u[VNUM];//[VIDEOMAX/4];
  unsigned char *buffer_v[VNUM];//[VIDEOMAX/4];
 };
//视频接收缓冲  未解码
struct tempvideobuf1
 {
//  int iput;                     // 环形缓冲区的当前放入位置
//  int iget;                     // 缓冲区的当前取出位置
//  int n;                        // 环形缓冲区中的元素总数量
  uint32_t timestamp;  //时间戳
  uint32_t frameno;       //帧序号
  short TotalPackage;     //总包数
  uint8_t CurrPackage[MAXPACKNUM]; //当前包   1 已接收  0 未接收
  int Len;                //帧数据长度
  uint8_t isFull;                  //该帧已接收完全
  unsigned char *buffer;//[VIDEOMAX];
  unsigned char frame_flag;             //帧标志 音频帧 I帧 P帧  
 };                            //     [MP4VNUM]
//视频接收缓冲 链表
typedef struct node2{
               struct tempvideobuf1 Content;
               struct node2 *llink, *rlink;
}TempVideoNode1;
//视频播放缓冲
struct videoplaybuf1
 {
  uint8_t isUse;     //该帧已解码未播放,缓冲区不可用
  uint32_t timestamp; //时间戳
  uint32_t frameno;   //帧序号
  unsigned char *buffer;//[VIDEOMAX];
  unsigned char frame_flag;             //帧标志 音频帧 I帧 P帧
 };
//同步播放结构
struct _SYNC
 {
  pthread_cond_t cond;       //同步线程条件变量
  pthread_condattr_t cond_attr;
  pthread_mutex_t lock;      //互斥锁
  pthread_mutex_t audio_rec_lock;//[VPLAYNUM];//音频录制互斥锁
  pthread_mutex_t audio_play_lock;//[VPLAYNUM];//音频播放互斥锁
  pthread_mutex_t video_rec_lock;//[VPLAYNUM];//视频录制互斥锁
  pthread_mutex_t video_play_lock;//[VPLAYNUM];//视频播放互斥锁
  unsigned int count;        //计数
  uint8_t isDecodeVideo;     //视频已解码一帧  解码线程-->同步线程
  uint8_t isPlayVideo;       //视频已播放一帧  播放线程-->同步线程
  uint8_t isDecodeAudio;     //音频已解码一帧  解码线程-->同步线程
  uint8_t isPlayAudio;       //音频已播放一帧  播放线程-->同步线程
 };

//加缓冲锁? 
struct audiobuf1
 {
  int iput; // 环形缓冲区的当前放入位置
  int iget; // 缓冲区的当前取出位置 
  int n; // 环形缓冲区中的元素总数量
  uint32_t timestamp[NMAX/AUDIOBLK]; //时间戳
  uint32_t frameno[NMAX/AUDIOBLK];   //帧序号
  unsigned char buffer[NMAX];
 };

//音频接收缓冲  未解码
struct tempaudiobuf1
 {
  uint32_t timestamp;  //时间戳
  uint32_t frameno;       //帧序号
  short TotalPackage;     //总包数
  uint8_t CurrPackage[MAXPACKNUM]; //当前包   1 已接收  0 未接收
  int Len;                //帧数据长度
  uint8_t isFull;                  //该帧已接收完全
  unsigned char *buffer;//[AUDIOBLK];
  unsigned char frame_flag;             //帧标志 音频帧 I帧 P帧
 };                            //     [MP4VNUM]
//音频接收缓冲 链表
typedef struct node3{
               struct tempaudiobuf1 Content;
               struct node3 *llink, *rlink;
}TempAudioNode1;

//音频播放缓冲
struct audioplaybuf1
 {
  uint8_t isUse;     //该帧已解码未播放,缓冲区不可用
  uint32_t timestamp; //时间戳
  uint32_t frameno;   //帧序号
  unsigned char *buffer;//[VIDEOMAX];
 };

//家庭留言缓冲
struct wavbuf1
 {
  int iput; // 环形缓冲区的当前放入位置
  int iget; // 缓冲区的当前取出位置
  int n; // 环形缓冲区中的元素总数量
  unsigned char *buffer;
 };

struct WaveFileHeader
{
 char chRIFF[4];
 uint32_t dwRIFFLen;
 char chWAVE[4];

 char chFMT[4];
 uint32_t dwFMTLen;
 uint16_t wFormatTag;
 uint16_t nChannels;
 uint32_t nSamplesPerSec;
 uint32_t nAvgBytesPerSec;
 uint16_t nBlockAlign;
 uint16_t wBitsPerSample;

 char chFACT[4];
 uint32_t dwFACTLen;

 char chDATA[4];
 uint32_t dwDATALen;
};

/*typedef */struct fcap_frame_buff
{
    unsigned int phyAddr;
    unsigned int mmapAddr;   //length per dma buffer
    unsigned int frame_no;
};
struct Local1{
               int Status;
		 int ShowHotkey;
			   int ShowHotkeyTime;
		int ShowRecPic;
               //状态 0 空闲 1 主叫对讲  2 被叫对讲  3 监视  4 被监视  5 主叫通话
               //6 被叫通话
               //21 小门口机被叫对讲  22 小门口机被叫通话 23 小门口机监视
               int RecordPic;  //留照片  0 不留  1 呼叫留照片  2 通话留照片
               int IFrameCount; //I帧计数
               int IFrameNo;    //留第几个I帧
               unsigned char yuv[2][CIF_W*CIF_H*3/2];
               int HavePicRecorded;  //有照片已录制
               int HavePicRecorded_flag;
               struct tm *recpic_tm_t; //留照片时间               

               struct tm *call_tm_t; //被呼叫时间
               
               int CallConfirmFlag; //在线标志
               int Timer1Num;  //定时器1计数
               int OnlineFlag; //需检查在线确认
               int OnlineNum;  //在线确认序号
               int TimeOut;    //监视超时,  通话超时,  呼叫超时，无人接听
               int TalkTimeOut; //通话最长时间
               int RecPicSize;  //视频大小  1  352*288   2  720*480
               int PlayPicSize;  //视频大小  1  352*288   2  720*480
               pthread_mutex_t save_lock;//互斥锁
               pthread_mutex_t udp_lock;//互斥锁
               pthread_mutex_t comm_lock;//互斥锁
               int PrevWindow;      //上一个窗口编号
               int TmpWindow;       //暂存窗口编号 用于弹出窗口时
               int CurrentWindow;   //当前窗口编号
               int DefenceDelayFlag;    //布防延时标志
               int DefenceDelayTime;   //计数
               int PassLen;            //密码长度
               int AlarmDelayFlag[2];    //报警延时标志
               int AlarmDelayTime[2];   //计数

               int ForceIFrame;    //1 强制I帧
               int CalibratePos;   //校准触摸屏十字位置 0 1 2 3
               int CalibrateSucc;  //校准成功
               int CurrFbPage; //当前Fb页
               unsigned char IP_Group[4];  //组播地址
               unsigned char Weather[3];   //天气预报

               int AddrLen;          //地址长度  S 12  B 6 M 8 H 6                 

               int isHost;           //'0' 主机 '1' 副机 '2' ...
               int ConnToHost;       //与主机连接正常 1 正常 0 不正常
               unsigned char HostIP[4]; //主机IP
               unsigned char HostAddr[21]; //主机Addr
               int DenNum;             //目标数量  副机
               unsigned char DenIP[10][4];    //副机IP
               char DenAddr[10][21];         //副机Addr

               int NetStatus;   //网络状态 1 断开  0 接通
               int OldNetSpeed;  //网络速度                
               int NoBreak;     //免扰状态 1 免扰  0 正常

               int ReportSend;  //设备定时报告状态已发送
               int RandReportTime; //设备定时报告状态随机时间
               int ReportTimeNum;  //计时
                                 //在GPIO线程中查询各线程是否运行
               int Key_Press_Run_Flag;
               int Save_File_Run_Flag;
               int Dispart_Send_Run_Flag;
               int Multi_Send_Run_Flag;
               int Multi_Comm_Send_Run_Flag;

               int MenuIndex;     //当前按钮索引
               int MaxIndex;      //本界面最大索引
               int MainMenuIndex;     //主界面按钮索引

               int OsdOpened;  //OSD打开标志

               int LcdLightFlag; //LCD背光标志
               int LcdLightTime; //时间

	     int newInfoNum;
               int ResetPlayRingFlag;  //复位Audio Play flag

               int nowvideoframeno;   //当前视频帧编号
               int nowaudioframeno;   //当前音频帧编号

               int ForceEndWatch;  //有呼叫时，强制关监视
               int ZoomInOutFlag;  //正在放大缩小中
               uint32_t newzoomtime;
               uint32_t oldzoomtime;
               uint32_t newtouchtime;
               uint32_t oldtouchtime;    //上一次触摸屏处理时间

               int MiniDoorOpenLockFlag;    //小门口机开锁标志
               int MiniDoorOpenLockTime;    //小门口机开锁延时计数          

		int keypad_type;	//// 0 number keypad   1 character keypad
		int keypad_show;
		int AlarmStatus;
			   
              };

struct LocalCfg1{
               char Addr[20];             //地址编码
               unsigned char Mac_Addr[6]; //网卡地址
               unsigned char IP[4];       //IP地址
               unsigned char IP_Mask[4];  //子网掩码
               unsigned char IP_Gate[4];  //网关地址
               unsigned char IP_NS[4];    //NS（名称解析）服务器地址
               unsigned char IP_Server[4];  //主服务器地址（与NS服务器可为同一个）
               unsigned char IP_Broadcast[4];  //广播地址

               int ReportTime;      //设备定时报告状态时间
               unsigned char DefenceStatus;       //布防状态
               unsigned char DefenceNum;          //防区模块个数
               unsigned char DefenceInfo[32][10]; //防区信息

               char EngineerPass[10];             //工程密码
               
               int In_DelayTime;                //进入延时
               int Out_DelayTime;               //外出延时
               int Alarm_DelayTime;               //报警延时

               int Ts_X0;                   //触摸屏
               int Ts_Y0;
               int Ts_deltaX;
               int Ts_deltaY;

	 int misscall_num;
	   int missmessage_num;
	   int news_num;
};
//串口接收缓冲区
struct commbuf1
 {
  int iput; // 环形缓冲区的当前放入位置
  int iget; // 缓冲区的当前取出位置
  int n; // 环形缓冲区中的元素总数量
  unsigned char buffer[COMMMAX];
 };
//状态提示信息缓冲
//Type
//          11 -- 呼叫中心
//          12 -- 户户通话
//          13 -- 监视
//          16 -- 对讲图像窗口
struct PicStatBuf1{
               int Flag;                      //延时清提示信息标志
               int Type;                      //类型
               int Time;
               int MaxTime;                   //最长时间

               int KeyPressTime;               

               unsigned char *InfoContent;      //信息显示框 , 用于上翻下翻
               unsigned char *InfoContent1;      //信息显示框 , 用于上翻下翻
               unsigned char *InfoButton;       //信息按钮部分
               unsigned char *InfoAllDel;       //信息全部删除按钮

               unsigned char *PicContent;     //相片
               
               unsigned char *InfoNum_N[4];       //信息总信息，未读信息
               unsigned char *InfoNum_H[4];       //信息总信息，未读信息
               unsigned char *Info_Row_N;     //单条信息，正常显示
               unsigned char *Info_Row_H;     //单条信息，高亮显示
              };
struct Remote1{
               int DenNum;             //目标数量  主机+副机
               unsigned char DenIP[4]; //对方IP或视频服务器IP
               unsigned char GroupIP[4]; //GroupIP
               unsigned char IP[10][4];    //对方IP
               int Added[10];                //已加入组
               char Addr[10][21];         //对方Addr
               int isDirect;       //是否直通  0 直通  1 中转
              };

struct Info1{
               int MaxNum;   //最大信息数
               int TotalNum; //信息总数
               int NoReadedNum; //未读信息总数
               int TotalInfoPage;   //总信息页数
               int CurrentInfoPage; //当前信息页
               int CurrNo;    //当前信息序号
               int CurrPlayNo;  //当前播放序号
               int TimeNum;    //计数
              };

//单条信息内容结构体
struct InfoContent1{
               uint8_t isValid;  //有效，未删除标志   1
               uint8_t isReaded; //已读标志    1
               uint8_t isLocked; //锁定标志    1
               char Time[32];    //接收时间    32
               uint8_t Type;     //类型        1    信息类型或事件类型
               uint32_t Sn;      //序号        4
               int Length;       //长度        4
               char Content[INFOMAXSIZE];//内容  400  内容或事件对象
               char Event[20];         //事件
              };                               //内存分配为444
//当前信息窗口状态              
struct InfoStatus1{
                  int CurrType;  //当前信息类型
                  int CurrWin;   //当前信息窗口  0 信息列表  1  信息内容
                  int CurrNo;    //当前信息序号
                 };
//信息链表
typedef struct node{
               struct InfoContent1 Content;
               struct node *llink, *rlink;
}InfoNode1;

//存储文件到FLASH队列 数据结构 由于存储FLASH速度较慢 用线程来操作
struct Save_File_Buff1{
               int isValid; //是否有效
               int Type;    //存储类型 1－一类信息  2－单个信息  3－物业服务  4－本地设置
               int InfoType;   //信息类型
               int InfoNo;     //信息位置
               InfoNode1 *Info_Node; //信息结点
              };

//UDP主动命令数据发送结构
struct Multi_Udp_Buff1{
               int isValid; //是否有效
               int SendNum; //当前发送次数
               int CurrOrder;//当前命令状态,VIDEOTALK VIDEOTALKTRANS VIDEOWATCH VIDEOWATCHTRANS
                             //主要用于需解析时，如单次命令置为0
               int m_Socket;
               char RemoteHost[20];
               unsigned char buf[1500];
               int DelayTime;  //等待时间
               int nlength;
              };
              
//COMM主动命令数据发送结构
struct Multi_Comm_Buff1{
               int isValid; //是否有效
               int SendNum; //当前发送次数
               int m_Comm;
               unsigned char buf[1500];
               int nlength;
              };

//通话数据结构
struct talkdata1
  {
   char HostAddr[20];       //主叫方地址
   unsigned char HostIP[4]; //主叫方IP地址
   char AssiAddr[20];       //被叫方地址
   unsigned char AssiIP[4]; //被叫方IP地址
   unsigned int timestamp;  //时间戳
   unsigned short DataType;          //数据类型
   unsigned short Frameno;           //帧序号
   unsigned int Framelen;            //帧数据长度    
   unsigned short TotalPackage;      //总包数
   unsigned short CurrPackage;       //当前包数
   unsigned short Datalen;           //数据长度
   unsigned short PackLen;       //数据包大小
  }__attribute__ ((packed));
//信息数据结构
struct infodata1
  {
   char Addr[20];       //地址编码
   unsigned short Type; //类型
   unsigned int  Sn;         //序号
   unsigned short Length;   //数据长度
  }__attribute__ ((packed));
struct downfile1
  {
   char FlagText[20];     //标志字符串
   char FileName[20];
   unsigned int Filelen;            //文件大小
   unsigned short TotalPackage;      //总包数
   unsigned short CurrPackage;       //当前包数
   unsigned short Datalen;           //数据长度
  }__attribute__ ((packed));

struct PicInfo1{
                int Width;
                int Height;
                int X;
                int Y;
                };
struct LabelInfo1{
                int Width;
                int Height;
                int X;
                int Y;
                };
#ifndef CommonH
#define CommonH
  int DebugMode;           //调试模式
  struct flcd_data f_data;
  int             fbdev;
  unsigned char  *fbmem;
  unsigned int    fb_uvoffset;
  unsigned int    fb_width;
  unsigned int    fb_height;
  unsigned int    fb_depth;
  unsigned int    screensize;

  int DeltaLen;  //数据包有效数据偏移量

  struct PicInfo1 PicInfo[MAINPICNUM];      //首页图象
  struct PicInfo1 TmpPicInfo[MAINPICNUM];      //首页图象
  struct LabelInfo1 LabelInfo[MAINLABELNUM]; //首页Label
  struct LabelInfo1 TmpLabelInfo[MAINLABELNUM]; //首页Label
  //首页文件名
  char nPic_Name[MAINPICNUM][20] = {"main.jpg", "logo.jpg", "menu1_up.jpg", "menu2_up.jpg", "menu3_up.jpg", "menu4_up.jpg", "menu5_up.jpg", "menu1_down.jpg",
                                       "menu2_down.jpg", "menu3_down.jpg", "menu4_down.jpg", "menu5_down.jpg", "weather1.jpg", "weather2.jpg", "weather3.jpg", "weather4.jpg", "weather5.jpg",
                                       "state1.jpg", "state2.jpg", "state3.jpg", "state4.jpg", "state5.jpg", "state6.jpg", "state7.jpg"
                                       };

  struct tm *curr_tm_t;
  struct TimeStamp1 TimeStamp;  //接收时间与播放时间，同步用

  int temp_video_n;      //视频接收缓冲个数
  TempVideoNode1 *TempVideoNode_h;    //视频接收缓冲列表
  int temp_audio_n;      //音频接收缓冲个数
  TempAudioNode1 *TempAudioNode_h;    //音频接收缓冲列表

  //系统初始化标志
  int InitSuccFlag;
  //本机状态设置
  struct Local1 Local;
  struct LocalCfg1 LocalCfg;

  //远端地址
  struct Remote1 Remote;
  char NullAddr[21];   //空字符串
  //COMM
  int Comm2fd;  //串口2句柄
  int Comm3fd;  //串口3句柄
  int Comm4fd;  //串口4句柄
  //免费ARP
  int ARP_Socket;
  //检测网络连接
  int m_EthSocket;
  //UDP
  int m_DataSocket;
  int m_VideoSocket;
  int LocalDataPort;   //命令及数据UDP端口
  int LocalVideoPort;  //音视频UDP端口
  int RemoteDataPort;
  int RemoteVideoPort;
  char RemoteHost[20];
  char sPath[80];
  char currpath[80];   //自定义路径  
  char wavPath[80];
  char UdpPackageHead[15];


  char CallListAddr[50];
  //状态提示信息缓冲
  struct PicStatBuf1 PicStatBuf;
  //按键按下效果用一线程处理
  int key_press_flag;
  short key_press;
  //FLASH存储线程
  int save_file_flag;
  pthread_t save_file_thread;
  void save_file_thread_func(void);
  sem_t save_file_sem;
  struct Save_File_Buff1 Save_File_Buff[SAVEMAX]; //FLASH存储缓冲最大值

  //主动命令数据发送线程：终端主动发送命令，如延时一段没收到回应，则多次发送
  //用于UDP和Comm通信
  int multi_send_flag;
  pthread_t multi_send_thread;
  void multi_send_thread_func(void);
  sem_t multi_send_sem;
  struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //10个UDP主动发送缓冲

  //主动命令数据发送线程：终端主动发送命令，如延时一段没收到回应，则多次发送
  //用于UDP和Comm通信
  int multi_comm_send_flag;
  pthread_t multi_comm_send_thread;
  void multi_comm_send_thread_func(void);
  sem_t multi_comm_send_sem;
  struct Multi_Comm_Buff1 Multi_Comm_Buff[COMMSENDMAX]; //10个COMM主动发送缓冲

  //watchdog
  int watchdog_fd;
  int InitParam;

  //gpio 按键
  int gpio_fd;
  int gpio_rcv_flag;
  pthread_t gpio_rcv_thread;
  void gpio_rcv_thread_func(void);

	int CurrBox;
	struct TImage main_image;    //首页背景图像  
	struct TImage main_hotkey_image;
	struct TImage hotkey_image[10];
	struct TImageButton menu_button[5]; //菜单条按钮
	struct TImageButton state_image[6];    //状态条图像
	struct TImage weather_image[12];
	struct TImage misscall_image[20];
	struct TImage missmessage_image[20];
	struct TImage news_image[20];
	struct TLabel label_hotkey;
	struct TImage clock_num_image[10];
	struct TImageButton bigmenu_button[6];
	//对讲界面
	struct TImageButton talk_menu_button[5];
	struct TImageButton talk_keynum_button[12];
	struct TImageButton talk_hotkey_button[3];
	struct TImageButton talkpic_button[10];
	struct TEdit roomaddr_edit;
	struct TEdit talkaddr_edit;
	struct TImage talk_main_image;

	struct TImage talk_message_image[16];
	struct TImageButton talk_message_button[5];
	struct TImage talk_comkey[20];
	struct TImage talk_banner[10];
	struct TImage talk_bg[8];
	struct TImageButton talk_callrec_button[4];
	struct TImageButton talk_info_button[9];
	struct TImage talk_infocontent_comkey[4];
	struct TLabel label_talkinfo;
 
 	struct TImage AreaMessageComKey[5];
	struct TImage LocalMessageComKey[6];
	struct TImage PushMessageComKey[5];
	struct TImage CustomMessageComKey[5];
	struct TImage MisscallComKey[6];
	struct TImage CalledComKey[6];
	struct TImage CallComKey[6];
 
	struct TImageButton setup_menu_button[12];
	struct TImage setup_comkey_image[5];
	struct TImage setup_bg[12];
	struct TImageButton setup_screen_menu_button[3];
	struct TImageButton setup_upgrade_menu_button[2];
	struct TImageButton setup_keynum_button[15];
 
	struct TEdit setup_edit[4];
	struct TEdit SetupPass_edit;
	struct TEdit LanSet_edit[5];
	struct TEdit WlanSet_edit[8];
	struct TEdit RoomSet_edit[6];
	struct TEdit VoiceSet_edit;
	struct TEdit TimeSet_edit[3];
	struct TEdit PassSet_edit[3];
	struct TImage blank_image;

	struct TImage keypad_image[2];
	struct TImageButton keyboard_num[32];
	struct TImageButton keyboard_en[32];
	struct TLabel label_keypad;

	struct TLabel label_call;
 
#else
  extern int DebugMode;           //调试模式
  extern struct flcd_data f_data;
  extern int             fbdev;
  extern unsigned char  *fbmem;
  extern unsigned int    fb_uvoffset;
  extern unsigned int    fb_width;
  extern unsigned int    fb_height;
  extern unsigned int    fb_depth;
  extern unsigned int    screensize;

  extern int DeltaLen;  //数据包有效数据偏移量

  extern struct PicInfo1 PicInfo[MAINPICNUM];      //首页图象
  extern struct PicInfo1 TmpPicInfo[MAINPICNUM];      //首页图象
  extern struct LabelInfo1 LabelInfo[MAINLABELNUM]; //首页Label
  extern struct LabelInfo1 TmpLabelInfo[MAINLABELNUM]; //首页Label
  //首页文件名
  extern char nPic_Name[MAINPICNUM][20];  

  extern struct tm *curr_tm_t;
  extern struct TimeStamp1 TimeStamp;  //接收时间与播放时间，同步用  

  extern int temp_video_n;      //视频接收缓冲个数
  extern TempVideoNode1 *TempVideoNode_h;    //视频接收缓冲列表
  extern int temp_audio_n;      //音频接收缓冲个数
  extern TempAudioNode1 *TempAudioNode_h;    //音频接收缓冲列表

  //系统初始化标志
  extern int InitSuccFlag;  
  //本机状态设置
  extern struct Local1 Local;
  extern struct LocalCfg1 LocalCfg;

  //远端地址
  extern struct Remote1 Remote;
  extern char NullAddr[21];   //空字符串
  //COMM
  extern int Comm2fd;  //串口2句柄
  extern int Comm3fd;  //串口3句柄
  extern int Comm4fd;  //串口4句柄
  //免费ARP
  extern int ARP_Socket;
  //检测网络连接
  int m_EthSocket;   
  //UDP
  extern int m_DataSocket;
  extern int m_VideoSocket;
  extern int LocalDataPort;   //命令及数据UDP端口
  extern int LocalVideoPort;  //音视频UDP端口
  extern int RemoteDataPort;
  extern int RemoteVideoPort;
  extern char RemoteHost[20];
  extern char sPath[80];
  extern char currpath[80];   //自定义路径
  extern char wavPath[80];
 extern char UdpPackageHead[15];
extern  char CallListAddr[50];
  //状态提示信息缓冲
  extern struct PicStatBuf1 PicStatBuf;
  //按键按下效果用一线程处理
  extern int key_press_flag;
  extern short key_press;
  //FLASH存储线程
  extern int save_file_flag;
  extern pthread_t save_file_thread;
  extern void save_file_thread_func(void);
  extern sem_t save_file_sem;
  extern struct Save_File_Buff1 Save_File_Buff[SAVEMAX]; //FLASH存储缓冲最大值

  //主动命令数据发送线程：终端主动发送命令，如延时一段没收到回应，则多次发送
  //用于UDP和Comm通信
  extern int multi_send_flag;
  extern pthread_t multi_send_thread;
  extern void multi_send_thread_func(void);
  extern sem_t multi_send_sem;
  extern struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //10个UDP主动发送缓冲
  //主动命令数据发送线程：终端主动发送命令，如延时一段没收到回应，则多次发送
  //用于UDP和Comm通信
  extern int multi_comm_send_flag;
  extern pthread_t multi_comm_send_thread;
  extern void multi_comm_send_thread_func(void);
  extern sem_t multi_comm_send_sem;
  extern struct Multi_Comm_Buff1 Multi_Comm_Buff[COMMSENDMAX]; //10个COMM主动发送缓冲

  //watchdog
  extern int watchdog_fd;  
 extern  int InitParam;

  //gpio 按键
  	extern int gpio_fd;
  	extern int gpio_rcv_flag;
  	extern pthread_t gpio_rcv_thread;
  	extern void gpio_rcv_thread_func(void);

	extern int CurrBox;
  
  	extern struct TImage main_image;    //首页背景图像  
	extern struct TImage main_hotkey_image;
	extern struct TImage hotkey_image[10];
	extern struct TImageButton menu_button[5]; //菜单条按钮
	extern struct TImageButton state_image[6];    //状态条图像
	extern struct TImage weather_image[12];
	extern struct TImage misscall_image[20];
	extern struct TImage missmessage_image[20];
	extern struct TImage news_image[20];
	extern struct TLabel label_hotkey;
	extern   struct TImage clock_num_image[10];
    	extern struct TImageButton bigmenu_button[6];
	  //对讲界面
	extern   struct TImageButton talk_menu_button[5];
	extern   struct TImageButton talk_keynum_button[12];
	extern   struct TImageButton talk_hotkey_button[3];
	extern   struct TImageButton talkpic_button[10];
	extern   struct TEdit roomaddr_edit;
	extern   struct TEdit talkaddr_edit;
	extern   struct TImage talk_main_image;
	
	extern struct TImage talk_message_image[16];
	extern struct TImageButton talk_message_button[5];
	extern struct TImage talk_comkey[20];
	extern struct TImage talk_banner[10];
	extern struct TImage talk_bg[8];
	extern struct TImageButton talk_callrec_button[4];
	extern struct TImageButton talk_info_button[9];
	extern struct TImage talk_infocontent_comkey[4];

	extern struct TImage AreaMessageComKey[5];
	extern struct TImage LocalMessageComKey[6];
	extern struct TImage PushMessageComKey[5];
	extern struct TImage CustomMessageComKey[5];
	extern struct TImage MisscallComKey[6];
	extern struct TImage CalledComKey[6];
	extern struct TImage CallComKey[6];

	extern struct TImageButton setup_menu_button[12];
	extern struct TImage setup_comkey_image[5];
	extern struct TImage setup_bg[12];
	extern struct TImageButton setup_screen_menu_button[3];
	extern struct TImageButton setup_upgrade_menu_button[2];
	extern struct TImageButton setup_keynum_button[15];

	extern struct TEdit setup_edit[4];
	extern struct TEdit SetupPass_edit;
	extern struct TEdit LanSet_edit[5];
	extern struct TEdit WlanSet_edit[8];
	extern struct TEdit RoomSet_edit[6];
	extern struct TEdit VoiceSet_edit;
	extern struct TEdit TimeSet_edit[3];
	extern struct TEdit PassSet_edit[3];
	extern struct TLabel label_talkinfo;
	extern struct TImage blank_image;

	extern struct TImage keypad_image[2];
	extern struct TImageButton keyboard_num[32];
	extern struct TImageButton keyboard_en[32];
	extern struct TLabel label_keypad;
	extern struct TLabel label_call;
#endif
