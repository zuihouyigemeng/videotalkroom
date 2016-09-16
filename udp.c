//UDP
#include <stdio.h>
#include   <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <semaphore.h>       //sem_t
#include <dirent.h>

#define CommonH
#include "common.h"

enum BOOL {FALSE = 0,TRUE = !FALSE};
//UDP
int SndBufLen=1024*128;
int RcvBufLen=1024*128;

short UdpRecvFlag;
pthread_t udpdatarcvid;
pthread_t udpvideorcvid;
int InitUdpSocket(short lPort);
void CloseUdpSocket(void);
int UdpSendBuff(int m_Socket, char *RemoteHost, unsigned char *buf,int nlength);
void CreateUdpDataRcvThread(void);
void CreateUdpVideoRcvThread(void);
void UdpDataRcvThread(void);  //UDP数据接收线程函数
void UdpVideoRcvThread(void);  //UDP音视频接收线程函数
void AddMultiGroup(int m_Socket, char *McastAddr);  //加入组播组
void DropMultiGroup(int m_Socket, char *McastAddr);  //退出组播组
void DropNsMultiGroup(int m_Socket, char *McastAddr);  //退出NS组播组
void RefreshNetSetup(int cType); //刷新网络设置  0 -- 未启动  1 -- 已启动

extern sem_t audiorec2playsem;

extern sem_t videorec2playsem;

int AudioMuteFlag;   //静音标志

extern struct _SYNC sync_s;

//接收线程内部处理函数
//报警
void RecvAlarm_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//取消报警
void RecvCancelAlarm_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//信息
void RecvMessage_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//设备定时报告状态
void RecvReportStatus_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//管理中心查询设备状态
void RecvQueryStatus_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//远程布防
void RecvRemoteDefence_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//复位密码
void RecvResetPass_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//写地址设置
void RecvWriteAddress_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//读地址设置
void RecvReadAddress_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//写室内机工程设置
void RecvWriteRoomSetup_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//读室内机工程设置
void RecvReadRoomSetup_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);

//解析
void RecvNSAsk_Func(unsigned char *recv_buf, char *cFromIP, int m_Socket);  //解析请求
void RecvNSReply_Func(unsigned char *recv_buf, char *cFromIP, int m_Socket);//解析应答
//监视
void RecvWatchCall_Func(unsigned char *recv_buf, char *cFromIP);  //监视呼叫
void RecvWatchLineUse_Func(unsigned char *recv_buf, char *cFromIP);  //监视占线应答
void RecvWatchCallAnswer_Func(unsigned char *recv_buf, char *cFromIP);  //监视呼叫应答
void RecvWatchCallConfirm_Func(unsigned char *recv_buf, char *cFromIP);  //通话在线确认
void RecvWatchCallEnd_Func(unsigned char *recv_buf, char *cFromIP);  //监视呼叫结束
void RecvWatchZoomOut_Func(unsigned char *recv_buf, char *cFromIP);  //放大(720*480)
void RecvWatchZoomIn_Func(unsigned char *recv_buf, char *cFromIP);  //缩小(352*240)
void RecvWatchCallUpDown_Func(unsigned char *recv_buf, char *cFromIP, int length);  //监视数据
//对讲
void RecvTalkCall_Func(unsigned char *recv_buf, char *cFromIP);  //对讲呼叫
void ExitGroup(unsigned char *buf);      //向其它被叫方退出组播组命令
void RecvTalkLineUse_Func(unsigned char *recv_buf, char *cFromIP);  //对讲占线应答
void RecvTalkCallAnswer_Func(unsigned char *recv_buf, char *cFromIP);  //对讲呼叫应答
void RecvTalkCallConfirm_Func(unsigned char *recv_buf, char *cFromIP); //对讲在线确认
void RecvTalkCallAsk_Func(unsigned char *recv_buf, char *cFromIP);  //对讲开始通话询问
void RecvTalkCallStart_Func(unsigned char *recv_buf, char *cFromIP);  //对讲开始通话
void RecvTalkCallEnd_Func(unsigned char *recv_buf, char *cFromIP);  //对讲呼叫结束
void RecvTalkRemoteOpenLock_Func(unsigned char *recv_buf, char *cFromIP);  //远程开锁
void RecvTalkZoomOut_Func(unsigned char *recv_buf, char *cFromIP);  //放大(720*480)
void RecvTalkZoomIn_Func(unsigned char *recv_buf, char *cFromIP);  //缩小(352*240)
void RecvTalkCallUpDown_Func(unsigned char *recv_buf, char *cFromIP, int length);  //对讲数据
void TalkEnd_ClearStatus(void); //对讲结束，清状态和关闭音视频
void ForceIFrame_Func(void);  //强制I帧
//主机与副机
void RecvSyncSub_Func(unsigned char *recv_buf, char *cFromIP);  //为主机，定时与副机同步状态
//查找设备
void RecvFindEquip_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);

//信息链表
// *h保存表头结点的指针，*p指向当前结点的前一个结点，*s指向当前结点
extern InfoNode1 *InfoNode_h[INFOTYPENUM];
//短信息数量结构
extern struct Info1 Info[INFOTYPENUM];
//当前信息窗口状态
extern struct InfoStatus1 InfoStatus;

int downbuflen;
char downip[20];
unsigned char *downbuf;
int download_image_flag;
pthread_t download_image_thread;      //下载系统映像线程
void download_image_thread_func(void);
int downloaded_flag[2000]; //已下载标志
int OldPackage;
//下载应用程序
void RecvDownLoadFile_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//下载系统映像
void RecvDownLoadImage_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket);
//---------------------------------------------------------------------------
int InitUdpSocket(short lPort)
{
  struct sockaddr_in s_addr;
  int  nZero=0;
  int  iLen;
  int m_Socket;
  int  nYes;
  int ret;

  /* 创建 socket , 关键在于这个 SOCK_DGRAM */
  if ((m_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
   {
    perror("Create socket error\r\n");
    return 0;
   }
  else
    printf("create socket.\n\r");

  if(m_EthSocket == 0)
    m_EthSocket = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&s_addr, 0, sizeof(struct sockaddr_in));
  /* 设置地址和端口信息 */
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(lPort);
  s_addr.sin_addr.s_addr = INADDR_ANY;//inet_addr(LocalIP);//INADDR_ANY;

  iLen=sizeof(nZero);           //  SO_SNDBUF
  nZero=SndBufLen;       //128K
  setsockopt(m_Socket,SOL_SOCKET,SO_SNDBUF,(char*)&nZero,sizeof((char*)&nZero));
  nZero=RcvBufLen;       //128K
  setsockopt(m_Socket,SOL_SOCKET,SO_RCVBUF,(char*)&nZero,sizeof((char*)&nZero));

  //音视频端口，允许发送和接收广播
  if(lPort == LocalVideoPort)
   {
    nYes = 1;
    if (setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char *)&nYes, sizeof((char *)&nYes))== -1)
     {
      printf("set broadcast error.\n\r");
      return 0;
     }
   }

  /* 绑定地址和端口信息 */
  if ((bind(m_Socket, (struct sockaddr *) &s_addr, sizeof(s_addr))) == -1)
   {
    perror("bind error");
    return 0;
   }
  else
    printf("bind address to socket.\n\r");
  if(lPort == LocalDataPort)
   {
    m_DataSocket = m_Socket;
    //创建UDP接收线程
    CreateUdpDataRcvThread();
   }
  if(lPort == LocalVideoPort)
   {
    m_VideoSocket = m_Socket;
    //创建UDP接收线程
    CreateUdpVideoRcvThread();
   }
  return 1;
}
//---------------------------------------------------------------------------
void CloseUdpSocket(void)
{
  UdpRecvFlag = 0;
  pthread_cancel(udpdatarcvid);
  pthread_cancel(udpvideorcvid);
  close(m_DataSocket);
  close(m_VideoSocket);
}
//---------------------------------------------------------------------------
int UdpSendBuff(int m_Socket, char *RemoteHost, unsigned char *buf,int nlength)
{
  struct sockaddr_in To;
  int nSize;
  To.sin_family=AF_INET;
  if(m_Socket == m_DataSocket)
    To.sin_port=htons(RemoteDataPort);
  if(m_Socket == m_VideoSocket)
    To.sin_port=htons(RemoteVideoPort);
  To.sin_addr.s_addr = inet_addr(RemoteHost);
  nSize=sendto(m_Socket,buf,nlength,0,(struct sockaddr*)&To,sizeof(struct sockaddr));
  return nSize;
}
//---------------------------------------------------------------------------
void CreateUdpDataRcvThread()
{
  int i,ret;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  ret=pthread_create(&udpdatarcvid, &attr, (void *)UdpDataRcvThread, NULL);
  pthread_attr_destroy(&attr);  
  #ifdef _DEBUG
    printf ("Create UDP data pthread!\n");
  #endif
  if(ret!=0){
    printf ("Create data pthread error!\n");
  }
}
//---------------------------------------------------------------------------
void CreateUdpVideoRcvThread()
{
  pthread_attr_t attr;
  int i,ret;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  ret=pthread_create(&udpvideorcvid, &attr, (void *)UdpVideoRcvThread, NULL);
  pthread_attr_destroy(&attr);
  #ifdef _DEBUG
    printf ("Create UDP video pthread!\n");
  #endif
  if(ret!=0){
    printf ("Create video pthread error!\n");
  }
}
//---------------------------------------------------------------------------
void AddMultiGroup(int m_Socket, char *McastAddr)  //加入组播组
{
// Winsock1.0
  struct ip_mreq mcast; // Winsock1.0
  mcast.imr_multiaddr.s_addr = inet_addr(McastAddr);
  mcast.imr_interface.s_addr = INADDR_ANY;
  if( setsockopt(m_Socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast)) == -1)
  {
      printf("set multicast error.\n\r");
      return;
  }
}
//---------------------------------------------------------------------------
void DropMultiGroup(int m_Socket, char *McastAddr)  //退出组播组
{
// Winsock1.0
/*  struct ip_mreq mcast; // Winsock1.0
  char IP_Group[20];
  //查看是否在其它组播组内
  if(Local.IP_Group[0] != 0)
   {
    sprintf(IP_Group, "%d.%d.%d.%d\0",
            Local.IP_Group[0],Local.IP_Group[1],Local.IP_Group[2],Local.IP_Group[3]);
    Local.IP_Group[0] = 0; //组播地址
    Local.IP_Group[1] = 0;
    Local.IP_Group[2] = 0;
    Local.IP_Group[3] = 0;
    //  memset(&mcast, 0, sizeof(struct ip_mreq));
    mcast.imr_multiaddr.s_addr = inet_addr(IP_Group);
    mcast.imr_interface.s_addr = INADDR_ANY;
    if( setsockopt(m_Socket,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcast,sizeof(mcast)) == -1)
     {
      printf("drop multicast error.\n\r");
      return;
     }
   }    */

}
//---------------------------------------------------------------------------
void DropNsMultiGroup(int m_Socket, char *McastAddr)  //退出NS组播组
{
// Winsock1.0
  struct ip_mreq mcast; // Winsock1.0

    //  memset(&mcast, 0, sizeof(struct ip_mreq));
    mcast.imr_multiaddr.s_addr = inet_addr(McastAddr);
    mcast.imr_interface.s_addr = INADDR_ANY;
    if( setsockopt(m_Socket,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcast,sizeof(mcast)) == -1)
     {
      printf("drop multicast error.\n\r");
      return;
     }

}
//---------------------------------------------------------------------------
void RefreshNetSetup(int cType) //刷新网络设置  0 -- 未启动  1 -- 已启动
{

  char SystemOrder[100];
  #if 1
  //锁定互斥锁
  pthread_mutex_lock (&Local.udp_lock);
  //退出NS组播组
  if(cType == 1)
   {
    DropNsMultiGroup(m_VideoSocket, NSMULTIADDR);
   }
  //设置MAC地址
  system("ifconfig eth0 down");
  sprintf(SystemOrder, "ifconfig eth0 hw ether %02X:%02X:%02X:%02X:%02X:%02X\0",
                 LocalCfg.Mac_Addr[0], LocalCfg.Mac_Addr[1], LocalCfg.Mac_Addr[2],
                 LocalCfg.Mac_Addr[3], LocalCfg.Mac_Addr[4], LocalCfg.Mac_Addr[5]);
  system(SystemOrder);
  system("ifconfig eth0 up");
  //设置IP地址和子网掩码
  sprintf(SystemOrder, "ifconfig eth0 %d.%d.%d.%d netmask %d.%d.%d.%d\0",
                 LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3],
                 LocalCfg.IP_Mask[0], LocalCfg.IP_Mask[1], LocalCfg.IP_Mask[2], LocalCfg.IP_Mask[3]);
  system(SystemOrder);
  //设置网关
  sprintf(SystemOrder, "route add default gw %d.%d.%d.%d\0",
                 LocalCfg.IP_Gate[0], LocalCfg.IP_Gate[1], LocalCfg.IP_Gate[2], LocalCfg.IP_Gate[3]);
  system(SystemOrder);
  //加入NS组播组
  if(cType == 1)
   {
    AddMultiGroup(m_VideoSocket, NSMULTIADDR);
   }
  //打开互斥锁
  pthread_mutex_unlock (&Local.udp_lock);
#endif
}
//---------------------------------------------------------------------------
void UdpDataRcvThread(void)  //UDP接收线程函数
{
  /* 循环接收数据 */
//  int oldframeno=0;
  unsigned char send_b[1520];
  int sendlength;
  char FromIP[20];
  int newframeno;
  int currpackage;
  int i,j;
  int sub;
  short PackIsExist; //数据包已接收标志
  short FrameIsNew;  //数据包是否是新帧的开始
  struct sockaddr_in c_addr;
  socklen_t addr_len;
  int len;
  int tmp;
  unsigned char buff[8096];

  char tmpAddr[21];
  int isAddrOK;
  #ifdef _DEBUG
    printf("This is udp pthread.\n");
  #endif
  UdpRecvFlag = 1;

  addr_len = sizeof(c_addr);
  while (UdpRecvFlag == 1)
   {
    len = recvfrom(m_DataSocket, buff, sizeof(buff) - 1, 0,
     (struct sockaddr *) &c_addr, &addr_len);
    if (len < 0)
     {
      perror("recvfrom");
     }
    buff[len] = '\0';
    strcpy(FromIP, inet_ntoa(c_addr.sin_addr));
    if((buff[0]==UdpPackageHead[0])&&(buff[1]==UdpPackageHead[1])&&(buff[2]==UdpPackageHead[2])
      &&(buff[3]==UdpPackageHead[3])&&(buff[4]==UdpPackageHead[4])&&(buff[5]==UdpPackageHead[5]))
     {
      switch(buff[6])
       {
        case ALARM:   //报警
                if(len == 41)
                 {
                  RecvAlarm_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("报警应答长度异常\n");
                  #endif
                 }
                break;
        case CANCELALARM:   //取消报警
                if(len == 30)
                 {
                  RecvCancelAlarm_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("取消报警应答长度异常\n");
                  #endif
                 }
                break;
        case SENDMESSAGE: //信息
                RecvMessage_Func(buff, FromIP, len, m_DataSocket);
                break;
        case REPORTSTATUS:   //设备定时报告状态
                if(len == 40)
                 {
                  RecvReportStatus_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("设备定时报告状态应答长度异常\n");
                  #endif
                 }
                break;
        case QUERYSTATUS:   //管理中心查询设备状态
                if(len == 40)
                 {
                  RecvQueryStatus_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("管理中心查询设备状态长度异常\n");
                  #endif
                 }
                break;                
        case REMOTEDEFENCE:   //远程布防
                if(len == 29)
                 {
                  RecvRemoteDefence_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("远程布防长度异常\n");
                  #endif
                 }
                break;
        case RESETPASS:   //复位密码
                if(len == 29)
                 {
                  RecvResetPass_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("复位密码长度异常\n");
                  #endif
                 }
                break;
        case WRITEADDRESS:   //写地址设置
                if(len == 72)
                 {
                  RecvWriteAddress_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("写地址设置长度异常\n");
                  #endif
                 }
                break;
        case READADDRESS:   //读地址设置
                if(len == 28)
                 {
                  RecvReadAddress_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("读地址设置长度异常\n");
                  #endif
                 }
                break;
        case WRITEROOMSETUP:   //写室内机工程设置
                break;
        case READROOMSETUP:   //读室内机工程设置
                break;
        //        case NSOrder:   //主机名解析（子网内广播）
        case NSSERVERORDER:  //主机名解析(NS服务器)
                switch(buff[7])
                 {
                  case 1://解析
                         if(len == 56)
                          {
                           RecvNSAsk_Func(buff, FromIP, m_DataSocket);
                          }
                         else
                          {
                           #ifdef _DEBUG
                             printf("解析请求数据长度异常\n");
                           #endif
                          }
                         break;
                  case 2://解析回应
                         if(len >= 57)
                          {
                           RecvNSReply_Func(buff, FromIP, m_DataSocket);
                          }
                         else
                          {
                           #ifdef _DEBUG
                             printf("解析应答数据长度异常\n");
                           #endif
                          }
                         break;
                 }
                break;
        case DOWNLOADFILE:   //下载应用程序
                if(len >= (9 + sizeof(struct downfile1)))
                 {
                  RecvDownLoadFile_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("下载应用程序长度异常\n");
                  #endif
                 }
                break;
        case DOWNLOADIMAGE:   //下载系统映像
                if(len >= (9 + sizeof(struct downfile1)))
                 {
                  RecvDownLoadImage_Func(buff, FromIP, len, m_DataSocket);
                 }
                else
                 {
                  #ifdef _DEBUG
                    printf("下载系统映像长度异常\n");
                  #endif
                 }
                break;
        }
      }
    if(strcmp(buff,"exit")==0)
     {
      printf("recvfrom888888888");
      UdpRecvFlag=0;
     }
   }
}
//---------------------------------------------------------------------------
//报警
void RecvAlarm_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i,j;
  int newlength;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;
  unsigned char AlarmByte;
  unsigned char tmp_p;
  //时钟
  time_t t;

  i = 0;
  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  //地址匹配
  if(isAddrOK == 1)
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    if(recv_buf[7] == REPLY)   //应答
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_DataSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if(Multi_Udp_Buff[i].buf[6] == ALARM)
          if(Multi_Udp_Buff[i].buf[7] == ASK)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;
              AlarmByte = (recv_buf[37] & 0x3F);
              tmp_p = 0x01;
              for(j=0; j<6; j++)
               {
                if((AlarmByte & tmp_p) == tmp_p)
                  LocalCfg.DefenceInfo[j][3] = 2; //报警已接收
                tmp_p = (tmp_p << 1);
               }
              #ifdef _DEBUG
                printf("收到报警状态应答\n");
              #endif
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//---------------------------------------------------------------------------
//取消报警
void RecvCancelAlarm_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i,j;
  int newlength;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;
  //时钟
  time_t t;

  i = 0;
  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  //地址匹配
  if(isAddrOK == 1)
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    if(recv_buf[7] == REPLY)   //应答
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_DataSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if(Multi_Udp_Buff[i].buf[6] == CANCELALARM)
          if(Multi_Udp_Buff[i].buf[7] == ASK)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;

              #ifdef _DEBUG
                printf("收到报警状态应答\n");
              #endif
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//---------------------------------------------------------------------------
//信息
void RecvMessage_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
	struct InfoContent1 InitInfoCon;
	int i,j;
	int newlength;
	int isAddrOK;
	InfoNode1 *tmp_node;
	//时钟
	time_t t;
	struct tm *tm_t;
	unsigned char send_b[1520];
	int sendlength;
	//信息数据结构
	struct infodata1 infodata;

	printf("recv data\n");
	i = 0;
	isAddrOK = 1;

	for(j=8; j<8+Local.AddrLen; j++)
	if(LocalCfg.Addr[j-8] != recv_buf[j])
	{
		isAddrOK = 0;
		break;
	}
	//地址匹配
	if(isAddrOK == 1)
	{
		memcpy(send_b, recv_buf, length);
		send_b[7] = REPLY;    //应答
		sendlength = length;
		UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

		memcpy(&infodata, recv_buf + 8, sizeof(infodata));
		newlength = infodata.Length + 36;

		//只接收普通信息  
		if((length == newlength)&&(infodata.Type == 1))
		{
			InitInfoCon.isValid = 1;
			InitInfoCon.isReaded = 0;
			InitInfoCon.isLocked = 0;
			time(&t);
			tm_t=localtime(&t);
			sprintf(InitInfoCon.Time, "%04d-%02d-%02d %02d:%02d:%02d\0", tm_t->tm_year + 1900, tm_t->tm_mon+1,
				tm_t->tm_mday, tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);
			InitInfoCon.Type = infodata.Type;
			InitInfoCon.Sn = infodata.Sn;
			InitInfoCon.Length = infodata.Length;
			memcpy(InitInfoCon.Content, recv_buf + 36, InitInfoCon.Length);
			InitInfoCon.Content[InitInfoCon.Length] = '\0';
			printf("InitInfoCon.Type = %d\n", InitInfoCon.Type);
	//如信息为0 ，为添加

		if(Info[InitInfoCon.Type-1].TotalNum >= Info[InitInfoCon.Type-1].MaxNum)//删除未锁定的最后一个，在头部插入一个
		{
			for(j = Info[InitInfoCon.Type-1].TotalNum; j >= 1; j --)
			{
				tmp_node=locate_infonode(InfoNode_h[InitInfoCon.Type-1], j);
				if(tmp_node->Content.isLocked == 0)
				{
					delete_infonode(tmp_node);
					Info[InitInfoCon.Type-1].TotalNum --;
					break;
				}
			}
		}

		//如信息未到最大数量，则插入到头部
		if(Info[InitInfoCon.Type-1].TotalNum < Info[InitInfoCon.Type-1].MaxNum)
		{
			tmp_node=locate_infonode(InfoNode_h[InitInfoCon.Type-1], 1);
			insert_infonode(InfoNode_h[InitInfoCon.Type-1], tmp_node, InitInfoCon);
			Info[InitInfoCon.Type-1].TotalNum ++;
		}
		//更新数量
		Info[InitInfoCon.Type-1].TotalNum = length_infonode(InfoNode_h[InitInfoCon.Type-1]);
		Info[InitInfoCon.Type-1].NoReadedNum = length_infonoreaded(InfoNode_h[InitInfoCon.Type-1]);

		//如当前为信息窗口，刷新屏幕
		if(Local.CurrentWindow == TalkAreaMessageWindow)
		{
			printf("需要刷新本界面!\n");
			ShowInfoList(InitInfoCon.Type-1);
			//     ShowInfoNum(InitInfoCon.Type-1);  //显示信息总数，未读信息总数
			//     if((InfoStatus.CurrType == (InitInfoCon.Type - 1))&&(InfoStatus.CurrWin == 0))
			//ShowInfoList(InitInfoCon.Type-1);
		}
		//锁定互斥锁
		pthread_mutex_lock (&Local.save_lock);
		//查找可用存储缓冲并填空
		for(i=0; i<SAVEMAX; i++)
		if(Save_File_Buff[i].isValid == 0)
		{
			Save_File_Buff[i].Type = 1;
			Save_File_Buff[i].InfoType = InitInfoCon.Type-1;
			Save_File_Buff[i].isValid = 1;
			sem_post(&save_file_sem);
			break;
		}

		//打开互斥锁
		pthread_mutex_unlock (&Local.save_lock);
		//  WriteInfoFile(InitInfoCon.Type-1);
		
		if(Local.CurrentWindow == MainWindow)
		{
			if(Info[0].NoReadedNum >= 10)
				DisplayImage(&missmessage_image[9],0);
			else if(Info[0].NoReadedNum > 0)
				DisplayImage(&missmessage_image[Info[0].NoReadedNum -1],0);
			else
				DisplayImage(&missmessage_image[10],0);
		}
	}
	else
	{
#ifdef _DEBUG
	printf("信息数据长度错误或类型不对\n");
#endif
	}
	}
}
//---------------------------------------------------------------------------
//设备定时报告状态
void RecvReportStatus_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
	int i,j;
	int newlength;
	int isAddrOK;
	unsigned char send_b[1520];
	int sendlength;
	//时钟
	time_t t;

	i = 0;
	isAddrOK = 1;
	for(j=8; j<8+Local.AddrLen; j++)
	if(LocalCfg.Addr[j-8] != recv_buf[j])
	{
		isAddrOK = 0;
		break;
	}
	//地址匹配
	if(isAddrOK == 1)
	{
		//锁定互斥锁
		pthread_mutex_lock (&Local.udp_lock);
		if(recv_buf[7] == REPLY)   //应答
		for(i=0; i<UDPSENDMAX; i++)
		if(Multi_Udp_Buff[i].isValid == 1)
		if(Multi_Udp_Buff[i].m_Socket == m_DataSocket)
		if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
		if(Multi_Udp_Buff[i].buf[6] == REPORTSTATUS)
		if(Multi_Udp_Buff[i].buf[7] == ASK)
		if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
		{
			Multi_Udp_Buff[i].isValid = 0;
			if(((recv_buf[29] << 8) + recv_buf[28]) != LocalCfg.ReportTime)
			{
				LocalCfg.ReportTime = (recv_buf[29] << 8) + recv_buf[28];
				SaveToFlash(4);    //向Flash中存储文件
			}
			//校准时钟
			#if 0
			if(((curr_tm_t->tm_year + 1900) != ((recv_buf[30] << 8) + recv_buf[31]))
				||((curr_tm_t->tm_mon+1) != recv_buf[32])
				||(curr_tm_t->tm_mday != recv_buf[33])
				||(curr_tm_t->tm_hour != recv_buf[34])
				||(curr_tm_t->tm_min != recv_buf[35]))
			{
				curr_tm_t->tm_year   = (recv_buf[30] << 8) + recv_buf[31] - 1900;
				curr_tm_t->tm_mon   =   recv_buf[32] - 1;
				curr_tm_t->tm_mday   =   recv_buf[33];
				curr_tm_t->tm_hour   =   recv_buf[34];
				curr_tm_t->tm_min   = recv_buf[35];
				curr_tm_t->tm_sec   =   recv_buf[36];
				t=mktime(curr_tm_t);
				stime((long*)&t);
				sprintf(Label_Clock.Text, "%02d:%02d\0",  curr_tm_t->tm_hour, curr_tm_t->tm_min);
				if(Local.CurrentWindow == 0)
				ShowClock(&Label_Clock, REFRESH);
			}
			#endif
		//天气预报
			if((Local.Weather[0] != recv_buf[37])||
				(Local.Weather[1] != recv_buf[38])||
				(Local.Weather[2] != recv_buf[39]))
			{
				Local.Weather[0] = recv_buf[37];
				Local.Weather[1] = recv_buf[38];
				Local.Weather[2] = recv_buf[39];
				//天气预报
				if(Local.CurrentWindow == MainWindow)
					ShowWeather();
			}
			break;
		}
		//打开互斥锁
		pthread_mutex_unlock (&Local.udp_lock);
	}
}
//---------------------------------------------------------------------------
//管理中心查询设备状态
void RecvQueryStatus_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
	int i,j,k;
	int newlength;
	int isAddrOK;
	unsigned char send_b[1520];
	int sendlength;
	//时钟
	time_t t;

	i = 0;
	isAddrOK = 1;
	for(j=8; j<8+Local.AddrLen; j++)
	if(LocalCfg.Addr[j-8] != recv_buf[j])
	{
		isAddrOK = 0;
		break;
	}
	//地址匹配
	if(isAddrOK == 1)
	{
		//头部
		memcpy(send_b, UdpPackageHead, 6);
		//命令
		send_b[6] = QUERYSTATUS;
		send_b[7] = ASK;    //主叫
		memcpy(send_b + 8, LocalCfg.Addr, 20);
		memcpy(send_b + 28, LocalCfg.Mac_Addr, 6);
		send_b[34] = LocalCfg.DefenceStatus;
		send_b[35] = LocalCfg.DefenceNum;
		for(k=0; k<(LocalCfg.DefenceNum*6); k++)
		memcpy(send_b + 36 + 10*k, LocalCfg.DefenceInfo[k], 10);
		sendlength = 36 + LocalCfg.DefenceNum*6*10;
		UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

		if(((recv_buf[29] << 8) + recv_buf[28]) != LocalCfg.ReportTime)
		{
			LocalCfg.ReportTime = (recv_buf[29] << 8) + recv_buf[28];
			SaveToFlash(4);    //向Flash中存储文件
		}
		#if 0
		//校准时钟
		if(((curr_tm_t->tm_year + 1900) != ((recv_buf[30] << 8) + recv_buf[31]))
		||((curr_tm_t->tm_mon+1) != recv_buf[32])
		||(curr_tm_t->tm_mday != recv_buf[33])
		||(curr_tm_t->tm_hour != recv_buf[34])
		||(curr_tm_t->tm_min != recv_buf[35]))
		//          ||(curr_tm_t->tm_sec != recv_buf[36]))
		{
		curr_tm_t->tm_year   = (recv_buf[30] << 8) + recv_buf[31] - 1900;
		curr_tm_t->tm_mon   =   recv_buf[32] - 1;
		curr_tm_t->tm_mday   =   recv_buf[33];
		curr_tm_t->tm_hour   =   recv_buf[34];
		curr_tm_t->tm_min   = recv_buf[35];
		curr_tm_t->tm_sec   =   recv_buf[36];
		t=mktime(curr_tm_t);
		stime((long*)&t);
		sprintf(Label_Clock.Text, "%02d:%02d\0",  curr_tm_t->tm_hour, curr_tm_t->tm_min);
		ShowClock(&Label_Clock, REFRESH);
		}
		#endif
		//天气预报
		if((Local.Weather[0] != recv_buf[37])||
		(Local.Weather[1] != recv_buf[38])||
		(Local.Weather[2] != recv_buf[39]))
		{
			Local.Weather[0] = recv_buf[37];
			Local.Weather[1] = recv_buf[38];
			Local.Weather[2] = recv_buf[39];
			if(Local.CurrentWindow == 0)
				ShowWeather();
		}
	}

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//远程布防
void RecvRemoteDefence_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i,j;
  int newlength;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  i = 0;
  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  //地址匹配
  if(isAddrOK == 1)
   {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答
    sendlength = length;
    UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
    //远程布防
    LocalCfg.DefenceStatus = recv_buf[28];
  }
}
//---------------------------------------------------------------------------
//复位密码
void RecvResetPass_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i,j;
  int newlength;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  i = 0;
  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  //地址匹配
  if(isAddrOK == 1)
   {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答
    sendlength = length;
    UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
    //复位密码
    switch(recv_buf[28])
     {
      case 1: //复位工程密码
             if(strcmp(LocalCfg.EngineerPass, "1234") != 0)
              {
               strcpy(LocalCfg.EngineerPass, "1234");
               SaveToFlash(4);    //向Flash中存储文件
              }
             break;
     }
  }
}
//---------------------------------------------------------------------------
//写地址设置
void RecvWriteAddress_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i,j;
  int newlength;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  i = 0;
  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  //地址匹配
  if(isAddrOK == 1)
   {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答
    sendlength = length;
    UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

    //写地址设置
    if(recv_buf[28] & 0x01) //地址编码
      memcpy(LocalCfg.Addr, recv_buf + 30, 20);
    if(recv_buf[28] & 0x02) //网卡地址
     {
      memcpy(LocalCfg.Mac_Addr, recv_buf + 50, 6);
     }
    if(recv_buf[28] & 0x04) //IP地址
     {
      memcpy(LocalCfg.IP, recv_buf + 56, 4);
     }
    if(recv_buf[28] & 0x08) //子网掩码
     {
      memcpy(LocalCfg.IP_Mask, recv_buf + 60, 4);
     }
    if(recv_buf[28] & 0x10) //网关地址
     {
      memcpy(LocalCfg.IP_Gate, recv_buf + 64, 4);
     }
    if(recv_buf[28] & 0x20) //服务器地址
      memcpy(LocalCfg.IP_Server, recv_buf + 68, 4);

    SaveToFlash(4);    //向Flash中存储文件

    RefreshNetSetup(1); //刷新网络设置
  }
}
//---------------------------------------------------------------------------
//读地址设置
void RecvReadAddress_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i,j;
  int newlength;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  i = 0;
  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  //地址匹配
  if(isAddrOK == 1)
   {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答

    send_b[28] = 0;
    send_b[29] = 0;

    //地址编码
    memcpy(send_b + 30, LocalCfg.Addr, 20);
    //网卡地址
    memcpy(send_b + 50, LocalCfg.Mac_Addr, 6);
    //IP地址
    memcpy(send_b + 56, LocalCfg.IP, 4);
    //子网掩码
    memcpy(send_b + 60, LocalCfg.IP_Mask, 4);
    //网关地址
    memcpy(send_b + 64, LocalCfg.IP_Gate, 4);
    //服务器地址
    memcpy(send_b + 68, LocalCfg.IP_Server, 4);

    sendlength = 72;
    UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
  }
}
//---------------------------------------------------------------------------
//下载应用程序
void RecvDownLoadFile_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i;
  unsigned char send_b[8096];
  int PackDataLen = 8000;  
  int sendlength;
  FILE *pic_fd;
  DIR *dirp;
  char picname[80];
  char filename[20] = "sound70";
  struct downfile1 DownData;
  char systemtext[50];
  int DownOK;
  memcpy(&DownData, recv_buf + 9, sizeof(struct downfile1));
  if((strcmp(DownData.FlagText, FLAGTEXT) == 0)&&(strcmp(DownData.FileName, filename) == 0))
   {
    switch(recv_buf[8])
     {
      case STARTDOWN:         //开始下载
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

                     printf("开始下载, DownData.Filelen = %d\n", DownData.Filelen);
                     if(downbuf == NULL)
                       downbuf = (unsigned char *)malloc(DownData.Filelen);
                     for(i=0; i<DownData.TotalPackage; i++)
                       downloaded_flag[i] = 0;
                     break;
      case DOWN:              //下载
                     memcpy(downbuf + DownData.CurrPackage * PackDataLen, recv_buf  + 9 + sizeof(struct downfile1), DownData.Datalen);

                     downloaded_flag[DownData.CurrPackage] = 1;
                     /*if(DownData.CurrPackage != 0)
                      if(DownData.CurrPackage != (OldPackage + 1))
                        printf("CurrPackage = %d, package lost %d, length = %d\n", DownData.CurrPackage , OldPackage + 1, length);
                     if(OldPackage != DownData.CurrPackage)
                       OldPackage = DownData.CurrPackage;     */

                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = 9 + sizeof(struct downfile1);
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
                     break;
      case DOWNFINISHONE:         //下载完成一个
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

                     printf("下载完成一个\n");
                     DownOK = 1;
                     for(i=0; i<DownData.TotalPackage; i++)
                       if(downloaded_flag[i] == 0)
                        {
                         printf("丢失数据包，i = %d\n", i);
                         DownOK = 0;
                        }
                     if(DownOK == 1)
                      {
                       sprintf(picname, "/mnt/mtd/%s\0", DownData.FileName);
                       if((pic_fd = fopen(picname, "wb")) == NULL)
                         printf("无法创建应用程序文件\n");
                       else
                        {
                         fwrite(downbuf, DownData.Filelen, 1, pic_fd);
                         fclose(pic_fd);
                        }

                       sprintf(systemtext, "chmod 777 %s\0", picname);
                       system(systemtext);

                       free(downbuf);
                       downbuf = NULL;
                       usleep(200*1000);
                       sync();
                       system("reboot");
                      }
                     else
                      {
                       free(downbuf);
                       downbuf = NULL;
                       SendUdpOne(DOWNLOADIMAGE, DOWNFAIL, 0, cFromIP);  //失败
                      }
                     break;
      case STOPDOWN:             //停止下载
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

                     printf("停止下载\n");
                     if(downbuf != NULL)
                      {
                       free(downbuf);
                       downbuf = NULL;
                      } 
                     break;
      case DOWNFINISHALL:        //全部完成下载
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
                     
                     printf("下载完成\n");
                     break;
     }
   }
}
//---------------------------------------------------------------------------
//下载系统映像
void RecvDownLoadImage_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i;
  pthread_attr_t attr;
  unsigned char send_b[8096];
  int PackDataLen = 8000;  
  int sendlength;
  FILE *pic_fd;
  DIR *dirp;
  char picname[80];
  char filename[20] = "msound70pImage";
  struct downfile1 DownData;
  char systemtext[50];
  int DownOK;
  memcpy(&DownData, recv_buf + 9, sizeof(struct downfile1));
  if((strcmp(DownData.FlagText, FLAGTEXT) == 0)&&(strcmp(DownData.FileName, filename) == 0))
   {
    switch(recv_buf[8])
     {
      case STARTDOWN:         //开始下载
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

                     printf("开始下载, DownData.Filelen = %d\n", DownData.Filelen);
                     if(downbuf == NULL)
                       downbuf = (unsigned char *)malloc(DownData.Filelen);
                     for(i=0; i<DownData.TotalPackage; i++)
                       downloaded_flag[i] = 0;
                     break;
      case DOWN:              //下载
                     memcpy(downbuf + DownData.CurrPackage * PackDataLen, recv_buf  + 9 + sizeof(struct downfile1), DownData.Datalen);

                     downloaded_flag[DownData.CurrPackage] = 1;
                     /*if(DownData.CurrPackage != 0)
                      if(DownData.CurrPackage != (OldPackage + 1))
                        printf("CurrPackage = %d, package lost %d, length = %d\n", DownData.CurrPackage , OldPackage + 1, length);
                     if(OldPackage != DownData.CurrPackage)
                       OldPackage = DownData.CurrPackage;  */
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = 9 + sizeof(struct downfile1);
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
                     break;
      case DOWNFINISHONE:         //下载完成一个
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

                     printf("下载完成一个\n");
                     DownOK = 1;
                     for(i=0; i<DownData.TotalPackage; i++)
                       if(downloaded_flag[i] == 0)
                        {
                         printf("丢失数据包，i = %d\n", i);
                         DownOK = 0;
                        }
                     if(DownOK == 1)
                      {
                       downbuflen = DownData.Filelen;
                       strcpy(downip, cFromIP);
                       download_image_flag = 1;
                       pthread_attr_init(&attr);
                       pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                       pthread_create(&download_image_thread,&attr,(void *)download_image_thread_func, NULL);
                       pthread_attr_destroy(&attr);
                       if(download_image_thread == 0 )
                        {
                          printf("无法系统映像下载线程\n");
                          free(downbuf);
                          downbuf = NULL;
                        }
                      }
                     else
                      {
                       free(downbuf);
                       downbuf = NULL;
                       SendUdpOne(DOWNLOADIMAGE, DOWNFAIL, 0, cFromIP);  //失败
                      }
                     break;
      case STOPDOWN:             //停止下载
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);

                     printf("停止下载\n");
                     if(downbuf != NULL)
                      {
                       free(downbuf);
                       downbuf = NULL;
                      } 
                     break;
      case DOWNFINISHALL:        //全部完成下载
                     memcpy(send_b, recv_buf, length);
                     send_b[7] = REPLY;    //应答
                     sendlength = length;
                     UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
                     
                     printf("下载完成\n");
                     break;
     }
   }
}
//---------------------------------------------------------------------------
//系统映像下载线程
void download_image_thread_func(void)
{
  #ifdef _DEBUG
    printf("创建系统映像下载线程\n" );
  #endif
  flashcp(downbuf, downbuflen, downip);
  free(downbuf);
  downbuf = NULL;

  #ifdef _DEBUG
    printf("结束系统映像下载线程\n" );
  #endif
}
//---------------------------------------------------------------------------
//写室内机工程设置
void RecvWriteRoomSetup_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
}
//---------------------------------------------------------------------------
//读室内机工程设置
void RecvReadRoomSetup_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
}
//---------------------------------------------------------------------------
void UdpVideoRcvThread(void)  //UDP接收线程函数
{
  /* 循环接收数据 */
//  int oldframeno=0;
  unsigned char send_b[1520];
  int sendlength;
  char FromIP[20];
  int newframeno;
  int currpackage;
  int i,j;
  int sub;
  short PackIsExist; //数据包已接收标志
  short FrameIsNew;  //数据包是否是新帧的开始
  struct sockaddr_in c_addr;
  socklen_t addr_len;
  int len;
  int tmp;
  unsigned char buff[8096];

//  char tmpAddr[21];
  int isAddrOK;
  #ifdef _DEBUG
    printf("This is udp video pthread.\n");
  #endif
  UdpRecvFlag = 1;

  addr_len = sizeof(c_addr);
  while (UdpRecvFlag == 1)
   {
    len = recvfrom(m_VideoSocket, buff, sizeof(buff) - 1, 0,
     (struct sockaddr *) &c_addr, &addr_len);
    if (len < 0)
     {
      perror("recvfrom");
     }
    buff[len] = '\0';
    strcpy(FromIP, inet_ntoa(c_addr.sin_addr));
    #ifdef _DEBUG
//      printf("FromIP is %s\n",FromIP);
    #endif
    if((buff[0]==UdpPackageHead[0])&&(buff[1]==UdpPackageHead[1])&&(buff[2]==UdpPackageHead[2])
      &&(buff[3]==UdpPackageHead[3])&&(buff[4]==UdpPackageHead[4])&&(buff[5]==UdpPackageHead[5]))
     {
      if(Local.CurrentWindow == 255)
        return;
      switch(buff[6])
       {
        case NSORDER:   //主机名解析（子网内广播）
    //    case NSSERVERORDER:  //主机名解析(NS服务器)
                switch(buff[7])
                 {
                  case 1://解析
                         if(len == 56)
                          {
                           RecvNSAsk_Func(buff, FromIP, m_VideoSocket);
                          }
                         else
                          {
                           #ifdef _DEBUG
                             printf("解析请求数据长度异常\n");
                           #endif
                          }
                         break;
                  case 2://解析回应
                         if(len >= 57)
                          {
                           RecvNSReply_Func(buff, FromIP, m_VideoSocket);
                          }
                         else
                          {
                           #ifdef _DEBUG
                             printf("解析应答数据长度异常\n");
                           #endif
                          }
                         break;
                 }
                break;
        case VIDEOTALK:    //局域网可视对讲
        case VIDEOTALKTRANS:  //局域网可视对讲中转服务
               switch(buff[8])
                {
                  case CALL:        //对方发起对讲
                            if(len == 62)
                             {
                              if(InitSuccFlag == 1)
                                RecvTalkCall_Func(buff, FromIP);
                              else
                                #ifdef _DEBUG
                                  printf("本机初始化没有完成\n");
                                #endif
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("对讲呼叫数据长度异常\n");
                              #endif
                             }
                            break;
                  case LINEUSE:        //对方正忙
                            if(len == 57)
                             {
                              RecvTalkLineUse_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("占线应答数据长度异常\n");
                              #endif
                             }
                            break;
                  case CALLANSWER:  //呼叫应答
                    //        printf("FromIP is %s\n",FromIP);
                    //        printf("Multi_Udp_Buff[i].RemoteHost is %s\n",Multi_Udp_Buff[i].RemoteHost);
                            if(len == 62)
                             {
                              RecvTalkCallAnswer_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("对讲呼叫应答数据长度异常\n");
                              #endif
                             }
                            break;
                  case CALLSTART:  //被叫方开始通话
                            if(len == 57)
                             {
                              RecvTalkCallStart_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("对讲开始通话数据长度异常\n");
                              #endif
                             }
                            break;
                  case CALLCONFIRM:     //通话在线确认
                            if(len == 61)
                             {
                              RecvTalkCallConfirm_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("对讲通话在线确认数据长度异常\n");
                              #endif
                             }
                            break;
                  case CALLEND:  //通话结束
                            //结束播放视频
                            //如为对方结束，需应答
                            if(len == 57)
                             {
                              RecvTalkCallEnd_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("结束对讲数据长度异常\n");
                              #endif
                             }
                            break;
                  case REMOTEOPENLOCK:    //远程开锁
                            if(len == 57)
                             {
                              RecvTalkRemoteOpenLock_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("远程开锁数据长度异常\n");
                              #endif
                             }
                            break;                            
                  case FORCEIFRAME:    //强制I帧请求
                            Local.ForceIFrame = 1;
                            break;
                  case ZOOMOUT:    //放大(720*480)
                            if(len == 57)
                             {
                              RecvTalkZoomOut_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                 printf("放大(720*480)数据长度异常");
                              #endif
                             }
                            break;
                  case ZOOMIN:    //缩小(352*240)
                            if(len == 57)
                             {
                              RecvTalkZoomIn_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                 printf("缩小(352*240)数据长度异常");
                              #endif
                             }
                            break;
                  case CALLUP: //通话上行
                  case CALLDOWN: //通话下行
                           RecvTalkCallUpDown_Func(buff, FromIP, len);
                           break;
                  }
                 break;
        case VIDEOWATCH:     //局域网监控
        case VIDEOWATCHTRANS:  //局域网监控中转服务
               switch(buff[8])
                {
                  case CALL:        //对方发起监视  室内机不响应监视
                         /*   if(len == 57)
                             {
                              RecvWatchCall_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("监视呼叫数据长度异常\n");
                              #endif
                             }     */
                            break;
                  case LINEUSE:        //对方正忙
                            if(len == 57)
                             {
                              RecvWatchLineUse_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("占线应答数据长度异常\n");
                              #endif
                             }
                            break;
                  case CALLANSWER:  //呼叫应答
                    //        printf("FromIP is %s\n",FromIP);
                    //        printf("Multi_Udp_Buff[i].RemoteHost is %s\n",Multi_Udp_Buff[i].RemoteHost);
                            if(len == 57)
                             {
                              RecvWatchCallAnswer_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("监视呼叫应答数据长度异常\n");
                              #endif
                             }
                            break;
                  case CALLCONFIRM:     //通话在线确认
                            if(len == 61)
                             {
                              RecvWatchCallConfirm_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("监视通话在线确认数据长度异常\n");
                              #endif
                             }
                            break;
                  case CALLEND:  //通话结束
                            //结束播放视频
                            //如为对方结束，需应答
                            if(len == 57)
                             {
                              RecvWatchCallEnd_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                printf("结束监视数据长度异常\n");
                              #endif
                             }
                            break;
                  case FORCEIFRAME:    //强制I帧请求
                            Local.ForceIFrame = 1;
                            break;
                  case ZOOMOUT:    //放大(720*480)
                            if(len == 57)
                             {
                              RecvWatchZoomOut_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                 printf("放大(720*480)数据长度异常");
                              #endif
                             }
                            break;
                  case ZOOMIN:    //缩小(352*240)
                            if(len == 57)
                             {
                              RecvWatchZoomIn_Func(buff, FromIP);
                             }
                            else
                             {
                              #ifdef _DEBUG
                                 printf("缩小(352*240)数据长度异常");
                              #endif
                             }
                            break;
                  case CALLUP: //通话上行
                  case CALLDOWN: //通话下行
                           RecvWatchCallUpDown_Func(buff, FromIP, len);
                           break;
                }
               break;
        case FINDEQUIP:       //查找设备
               if(len == 56)
                {
                 RecvFindEquip_Func(buff, FromIP, len, m_VideoSocket);
                }
               else
                {
                 #ifdef _DEBUG
                   printf("查找设备数据长度异常\n");
                 #endif
                }
               break;
       }
     }
   }
}
//-----------------------------------------------------------------------
//解析请求
void RecvNSAsk_Func(unsigned char *recv_buf, char *cFromIP, int m_Socket)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;
  isAddrOK = 1;
  for(j=8; j<8+Local.AddrLen; j++)
   if(LocalCfg.Addr[j-8] != recv_buf[j])
   {
    isAddrOK = 0;
    break;
   }
  //不是本机所发
  if(isAddrOK == 0)
   {
    isAddrOK = 1;
    for(j=32; j<32+12; j++)
     if(LocalCfg.Addr[j-32] != recv_buf[j])
      {
       isAddrOK = 0;
       break;
      }

//printf("isAddrOK = %d\n", isAddrOK);
  //要求解析的是本机地址
    if(isAddrOK == 1)
     {
      memcpy(send_b, recv_buf, 32);
      send_b[7] = REPLY;    //应答

      if(Local.isHost == '0')  //主机
       {
        send_b[32] = Local.DenNum + 1;   //地址个数

        memcpy(send_b + 33, LocalCfg.Addr, 20);
        memcpy(send_b + 53, LocalCfg.IP, 4);
        for(i=0; i<Local.DenNum; i++)
         {
          memcpy(send_b + 57 + 24*i, Local.DenAddr[i], 20);
          memcpy(send_b + 57 + 20 +24*i, Local.DenIP[i], 4);
         }
        sendlength = 57 + 24*Local.DenNum;
       }
      else                    //副机
       {
        send_b[32] = 1;   //地址个数

        memcpy(send_b + 33, LocalCfg.Addr, 20);
        memcpy(send_b + 53, LocalCfg.IP, 4);
        sendlength = 57;
       }
      UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
     }
  }
}
//-----------------------------------------------------------------------
//解析应答
void RecvNSReply_Func(unsigned char *recv_buf, char *cFromIP, int m_Socket)
{
  int i,j, k;
  int CurrOrder;
  int isAddrOK;

  //暂时关闭本地解析，试验服务器解析
  #ifdef _TESTNSSERVER
    if(m_Socket == m_VideoSocket)
      return;
  #endif
    
  //锁定互斥锁
  pthread_mutex_lock (&Local.udp_lock);

  for(i=0; i<UDPSENDMAX; i++)
   if(Multi_Udp_Buff[i].isValid == 1)
     if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
      if((Multi_Udp_Buff[i].buf[6] == NSORDER)||(Multi_Udp_Buff[i].buf[6] == NSSERVERORDER))
       if((Multi_Udp_Buff[i].buf[7] == ASK)&&(recv_buf[32] > 0))
        {
         //判断要求解析地址是否匹配
         isAddrOK = 1;
         for(j=32; j<32+12; j++)
          if(Multi_Udp_Buff[i].buf[j] != recv_buf[j+1])
           {
            isAddrOK = 0;
            break;
           }
         CurrOrder = Multi_Udp_Buff[i].CurrOrder;
         Multi_Udp_Buff[i].isValid = 0;
         Multi_Udp_Buff[i].SendNum = 0;
         if(isAddrOK == 1)
           break;         
        }
  //打开互斥锁
  pthread_mutex_unlock (&Local.udp_lock);

  if(isAddrOK == 1)
   { //收到正确的解析回应
    if(CurrOrder == 255) //主机向副机解析
     {
      //查看列表中是否已有该副机信息
      isAddrOK = 0;
      for(i=0; i<Local.DenNum; i++)
       {
        isAddrOK = 1;
        for(j=0; j<12; j++)
         if(Local.DenAddr[i][j] != recv_buf[j+33])
          {
           isAddrOK = 0;
           break;
          }
        if(isAddrOK == 1)
          for(j=0; j<4; j++)
           if(Local.DenIP[i][j] != recv_buf[j+53])
            {
             isAddrOK = 0;
             break;
            }
        if(isAddrOK == 1)
          break;
       }
      if(isAddrOK == 0)
       {
        memcpy(Local.DenIP[Local.DenNum], recv_buf + 53, 4);
        memcpy(Local.DenAddr[Local.DenNum], recv_buf + 33, 20);
        Local.DenNum ++;
       }
     }
    else
     {
           Remote.DenNum = recv_buf[32];
           if((Remote.DenNum >= 1)&&(Remote.DenNum <= 10))
            {
              {
               for(j=0; j<Remote.DenNum; j++)
                {
                 Remote.IP[j][0] = recv_buf[53+24*j];
                 Remote.IP[j][1] = recv_buf[54+24*j];
                 Remote.IP[j][2] = recv_buf[55+24*j];
                 Remote.IP[j][3] = recv_buf[56+24*j];
                 Remote.DenIP[0] = Remote.IP[j][0];
                 Remote.DenIP[1] = Remote.IP[j][1];
                 Remote.DenIP[2] = Remote.IP[j][2];
                 Remote.DenIP[3] = Remote.IP[j][3];
                 for(k=0; k<20; k++)
                   Remote.Addr[j][k] = recv_buf[33+24*j+k];
                 Remote.GroupIP[0] = 236;
                 Remote.GroupIP[1] = LocalCfg.IP[1];
                 Remote.GroupIP[2] = LocalCfg.IP[2];
                 Remote.GroupIP[3] = LocalCfg.IP[3];

                 //锁定互斥锁
                 pthread_mutex_lock (&Local.udp_lock);

                 for(i=0; i<UDPSENDMAX; i++)
                  if(Multi_Udp_Buff[i].isValid == 0)
                   {
                    Multi_Udp_Buff[i].SendNum = 0;
                    Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
                    sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",
                         Remote.DenIP[0],Remote.DenIP[1],Remote.DenIP[2],Remote.DenIP[3]);
                    #ifdef _DEBUG
                      printf("Multi_Udp_Buff[i].RemoteHost is %s\n",Multi_Udp_Buff[i].RemoteHost);
                      printf("解析地址成功,正在呼叫\n");
                    #endif
                    //头部
                    memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
                    //命令
                    Multi_Udp_Buff[i].buf[6] = CurrOrder;
                    Multi_Udp_Buff[i].buf[7] = ASK;    //主叫
                    // 子命令
                    Multi_Udp_Buff[i].buf[8] = CALL;

                    memcpy(Multi_Udp_Buff[i].buf+9,LocalCfg.Addr,20);
                    memcpy(Multi_Udp_Buff[i].buf+29,LocalCfg.IP,4);
                    memcpy(Multi_Udp_Buff[i].buf+33,Remote.Addr[j],20);
                    memcpy(Multi_Udp_Buff[i].buf+53,Remote.IP[j],4);

                    if(Remote.DenNum == 1)
                      Multi_Udp_Buff[i].buf[57] = 0; //单播
                    else
                      Multi_Udp_Buff[i].buf[57] = 1; //组播
                    //组播地址
                    Multi_Udp_Buff[i].buf[58] = Remote.GroupIP[0];
                    Multi_Udp_Buff[i].buf[59] = Remote.GroupIP[1];
                    Multi_Udp_Buff[i].buf[60] = Remote.GroupIP[2];
                    Multi_Udp_Buff[i].buf[61] = Remote.GroupIP[3];

                    Multi_Udp_Buff[i].nlength = 62;
                    Multi_Udp_Buff[i].DelayTime = 800;
                    Multi_Udp_Buff[i].isValid = 1;
                    sem_post(&multi_send_sem);
                    break;
                   }
                 //打开互斥锁
                 pthread_mutex_unlock (&Local.udp_lock);
                }
              }
           }
      }
  }
}
//-----------------------------------------------------------------------
//监视呼叫
void RecvWatchCall_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;
  uint32_t Ip_Int;
  //本机状态为空闲
  if(Local.Status == 0)
   {
    memcpy(send_b, recv_buf, 57);
    send_b[7]=ASK;    //主叫
    send_b[8]=CALLANSWER;//监视应答
    sendlength=57;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);

    //获取对方地址
    memcpy(Remote.Addr[0], recv_buf+9, 20);
    memcpy(Remote.IP[0], recv_buf+29, 4);

    Ip_Int=inet_addr(cFromIP);
    memcpy(Remote.DenIP, &Ip_Int,4);
    printf("Remote.DenIP, %d.%d.%d.%d\0",
            Remote.DenIP[0],Remote.DenIP[1],Remote.DenIP[2],Remote.DenIP[3]);
    if((Remote.DenIP[0] == Remote.IP[0][0]) && (Remote.DenIP[1] == Remote.IP[0][1])
      && (Remote.DenIP[2] == Remote.IP[0][2]) &&(Remote.DenIP[3] == Remote.IP[0][3]))
      {
       Remote.isDirect = 0;
       #ifdef _DEBUG
         printf("对方发起直通监视呼叫\n");
       #endif
      }
    else
      {
       Remote.isDirect = 1; 
       #ifdef _DEBUG
         printf("对方发起中转监视呼叫\n");
       #endif
      }

    Local.Status = 4;  //状态为被监视
    //开始录制视频
//    StartRecVideo(720, 480);
//    StartRecVideo(352, 288);
//    StartRecVideo(CIF_W, CIF_H);
    Local.CallConfirmFlag = 1; //设置在线标志
    Local.Timer1Num = 0;
    Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听
    Local.OnlineNum = 0;     //在线确认序号
    Local.OnlineFlag = 1;
   }
  //本机为忙
  else
   {
    memcpy(send_b, recv_buf, 57);
    send_b[7]=ASK;    //主叫
    send_b[8]=LINEUSE;//占线应答
    sendlength=57;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);
    #ifdef _DEBUG
      printf("对方发起监视呼叫\n");
    #endif
   }
}
//-----------------------------------------------------------------------
//监视占线应答
void RecvWatchLineUse_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;

  //锁定互斥锁
  pthread_mutex_lock (&Local.udp_lock);
  if(recv_buf[7] == ASK)   //应答
   for(i=0; i<UDPSENDMAX; i++)
    if(Multi_Udp_Buff[i].isValid == 1)
     if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
      if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
       if((Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)||(Multi_Udp_Buff[i].buf[6] == VIDEOWATCHTRANS))
        if(Multi_Udp_Buff[i].buf[7] == ASK)
         if(Multi_Udp_Buff[i].buf[8] == CALL)
          if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
           {
            Multi_Udp_Buff[i].isValid = 0;
          //  Local.Status = 0;  //状态设为空闲
            ShowStatusText(CALLX,CALLY, 3, cBlack, 1, 1, "对方占线", 0);
		  DisplayMainWindow(MainWindow);
#if 0
            //延时清提示信息标志
            PicStatBuf.Type = 13;
            PicStatBuf.Time = 0;
            PicStatBuf.Flag = 1;
            #ifdef _DEBUG
              printf("收到监视占线应答\n");
            #endif
#endif
            break;
           }
  //打开互斥锁
  pthread_mutex_unlock (&Local.udp_lock);
}
//-----------------------------------------------------------------------
//监视呼叫应答
void RecvWatchCallAnswer_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;

  //锁定互斥锁
  pthread_mutex_lock (&Local.udp_lock);
  if(recv_buf[7] == ASK)   //应答
   for(i=0; i<UDPSENDMAX; i++)
    if(Multi_Udp_Buff[i].isValid == 1)
     if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
      if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
       if((Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)||(Multi_Udp_Buff[i].buf[6] == VIDEOWATCHTRANS))
        if(Multi_Udp_Buff[i].buf[7] == ASK)
         if(Multi_Udp_Buff[i].buf[8] == CALL)
          if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
           {
            Multi_Udp_Buff[i].isValid = 0;
            //开始播放视频
            StartPlayVideo(CIF_W, CIF_H);
        //    StartPlayVideo(720, 480);

            Local.CallConfirmFlag = 1; //设置在线标志
            Local.Timer1Num = 0;
            Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听
            Local.OnlineNum = 0;     //在线确认序号
            Local.OnlineFlag = 1;

            Local.Status = 3;  //状态为监视
            #ifdef _DEBUG
              printf("收到监视呼叫应答\n");
            #endif
            OpenOsd();   //打开OSD
            //sprintf(Label_W_Time.Text, "%02d:%02d\0", Local.TimeOut/INTRPERSEC/60,
            //       (Local.TimeOut/INTRPERSEC)%60);
            //Label_W_Time.Text[2] = ',';
           // Label_W_Time.Text[5] = '\0';
           // ShowOsd(Label_W_Time.Text);
    //        ShowStatusText(50, 380 , 3, cBlack, 1, 1, "正在监视中", 0);
            break;
           }
  //打开互斥锁
  pthread_mutex_unlock (&Local.udp_lock);
}
//-----------------------------------------------------------------------
//监视在线确认
void RecvWatchCallConfirm_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  //本机被动    被监视
  if((Local.Status == 4)&&(recv_buf[7] == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7]=REPLY;    //应答
    sendlength=61;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);
    Local.CallConfirmFlag = 1;
    #ifdef _DEBUG
  //    printf("收到监视在线确认\n");
    #endif
   }
  else  //本机监视
   if(Local.Status == 3)
   {
    Local.CallConfirmFlag = 1;
    #ifdef _DEBUG
//      printf("收到对方应答本机监视在线确认\n");
    #endif
   }
}
//-----------------------------------------------------------------------
//监视呼叫结束
void RecvWatchCallEnd_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  //本机被动    被监视
  if(((Local.Status == 3)||(Local.Status == 4))&&(recv_buf[7] == ASK))
   {
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //设置在线标志
    memcpy(send_b, recv_buf, 57);
    send_b[7]=REPLY;    //应答
    sendlength=57;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);

    switch(Local.Status)
     {
      case 3: //本机监视
             StopPlayVideo();
             //ShowVideoWindow();    //关闭视频窗口
             CloseOsd();
             break;
      case 4: //本机被监视
             Local.Status = 0;  //状态为空闲
             break;
     }
    CloseOsd();
    if(Local.CurrentWindow == 13)
     {
      ShowStatusText(50, 380 , 3, cBlack, 1, 1, "对方结束监视", 0);
      //延时清提示信息标志
      PicStatBuf.Type = 13;
      PicStatBuf.Time = 0;
      PicStatBuf.Flag = 1;
     }
    #ifdef _DEBUG
      printf("对方结束监视\n");
    #endif
   }
  else  //本机监视
   if(Local.Status == 3)
   {
    Local.OnlineFlag = 0;
    Local.CallConfirmFlag = 0; //设置在线标志
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    //本机主动
    if(recv_buf[7] == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if((Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)||(Multi_Udp_Buff[i].buf[6] == VIDEOWATCHTRANS))
          if(Multi_Udp_Buff[i].buf[7] == ASK)
           if(Multi_Udp_Buff[i].buf[8] == CALLEND)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;
              switch(Local.Status)
               {
                case 3: //本机监视
                       StopPlayVideo();
                       //ShowVideoWindow();    //关闭视频窗口
                       CloseOsd();

                       if(Local.ForceEndWatch == 0)  //有呼叫时，强制关监视
                        {
                         //延时清提示信息标志
                         PicStatBuf.Type = 13;
                         PicStatBuf.Time = 0;
                         PicStatBuf.Flag = 1;
                        }
                       else
                        {
                         DisplayMainWindow(0);
                         Local.Status = 0;  //状态为空闲
                         Local.ForceEndWatch = 0;  //有呼叫时，强制关监视
                        }                       
                       
       //                Local.Status = 0;  //状态为空闲
                       break;
                case 4: //本机被监视
                       Local.Status = 0;  //状态为空闲
                 //      StopRecVideo();
                       break;
               }
              #ifdef _DEBUG
                printf("对方应答本机结束监视\n");
              #endif
              CloseOsd();
              if(Local.CurrentWindow == 13)
               {
                ShowStatusText(50, 380 , 3, cBlack, 1, 1, "对方应答本机结束监视", 0);
                //延时清提示信息标志
                PicStatBuf.Type = 13;
                PicStatBuf.Time = 0;
                PicStatBuf.Flag = 1;
               }
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//-----------------------------------------------------------------------
//放大(720*480)
void RecvWatchZoomOut_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  //本机被动    被监视
  if((Local.Status == 4)&&(recv_buf[7] == ASK))
   {
   }
  else  //本机监视
   if(Local.Status == 3)
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    //本机主动
    if(recv_buf[7] == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if((Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)||(Multi_Udp_Buff[i].buf[6] == VIDEOWATCHTRANS))
          if(Multi_Udp_Buff[i].buf[7] == ASK)
           if(Multi_Udp_Buff[i].buf[8] == ZOOMOUT)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;
              if(Local.PlayPicSize == 1)
               {
                Local.ZoomInOutFlag = 1;   //正在放大缩小中
                if(Local.Status != 0)
                  StopPlayVideo();   //352*240
                //清屏  第二页  视频720*480
                memset(fbmem + f_data.buf_len, 16, f_data.uv_offset);
                memset(fbmem + f_data.buf_len + f_data.uv_offset, 128, f_data.uv_offset/4);
                memset(fbmem + f_data.buf_len + f_data.uv_offset*5/4, 128, f_data.uv_offset/4);
                if(Local.Status != 0)
                  StartPlayVideo(D1_W, D1_H);  //720*480
                Local.ZoomInOutFlag = 0;   //正在放大缩小中
                #ifdef _DEBUG
                  printf("对方应答本机放大图像\n");
                #endif
               }
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//-----------------------------------------------------------------------
//缩小(352*240)
void RecvWatchZoomIn_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  //本机被动    被监视
  if((Local.Status == 4)&&(recv_buf[7] == ASK))
   {
   }
  else  //本机监视
   if(Local.Status == 3)
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    //本机主动
    if(recv_buf[7] == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if((Multi_Udp_Buff[i].buf[6] == VIDEOWATCH)||(Multi_Udp_Buff[i].buf[6] == VIDEOWATCHTRANS))
          if(Multi_Udp_Buff[i].buf[7] == ASK)
           if(Multi_Udp_Buff[i].buf[8] == ZOOMIN)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;
              if(Local.PlayPicSize == 2)
               {
                Local.ZoomInOutFlag = 1;   //正在放大缩小中
                //显示视频窗口
                //ShowVideoWindow();
                if(Local.Status != 0)
                  StopPlayVideo();   //720*480
                if(Local.Status != 0)
                  StartPlayVideo(CIF_W, CIF_H);  //352*240
                Local.ZoomInOutFlag = 0;   //正在放大缩小中
                #ifdef _DEBUG
                  printf("对方应答本机缩小图像\n");
                #endif
               }
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//-----------------------------------------------------------------------
//监视数据
void RecvWatchCallUpDown_Func(unsigned char *recv_buf, char *cFromIP, int length)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;
//  int newframeno;
//  int currpackage;
//  int sub;
  short PackIsExist; //数据包已接收标志
  short FrameIsNew;  //数据包是否是新帧的开始
  int tmp;
  TempVideoNode1 * tmp_videonode;
  int isFull;
  struct talkdata1 talkdata;
  if((Local.Status == 3)||(Local.Status == 4))  //状态为监视
   switch(recv_buf[61])
   {
    case 2://视频  I帧  352*240
    case 3://视频  P帧  352*240
    case 4://视频  I帧  720*480
    case 5://视频  P帧  720*480
           if(((recv_buf[61] == 2)||(recv_buf[61] == 3))&&(Local.PlayPicSize == 2))
             break;
           if(((recv_buf[61] == 4)||(recv_buf[61] == 5))&&(Local.PlayPicSize == 1))
             break;    
                  //帧序号
                  memcpy(&talkdata, recv_buf + 9, sizeof(talkdata));
                  PackIsExist = 0;
                  FrameIsNew = 1;
                  //锁定videoplaybuf[vpbuf_iget].buffer
                  pthread_mutex_lock(&sync_s.video_play_lock);

                  if(temp_video_n >= MP4VNUM)
                   {
                    temp_video_n = MP4VNUM;
                    #ifdef _DEBUG
                      printf("temp_video is full\n");
                    #endif
                   }
                  else
                   {
                    tmp_videonode = find_videonode(TempVideoNode_h, talkdata.Frameno, talkdata.CurrPackage);
                    if(tmp_videonode == NULL)
                      isFull = creat_videonode(TempVideoNode_h, talkdata, recv_buf, length);
                    else
                      isFull = add_videonode(tmp_videonode, talkdata, recv_buf, length);

                    if(isFull == 1)
                     {
                      TimeStamp.OldCurrVideo = TimeStamp.CurrVideo; //上一次当前视频时间
                      TimeStamp.CurrVideo = talkdata.timestamp;

                      temp_video_n = length_videonode(TempVideoNode_h);
                      if(temp_video_n >= 4)
                        sem_post(&videorec2playsem);
                     }
                   }
                 //解锁
                 pthread_mutex_unlock(&sync_s.video_play_lock);
           break;
  }
}
//-----------------------------------------------------------------------
//对讲呼叫
void RecvTalkCall_Func(unsigned char *recv_buf, char *cFromIP)
{
  char wavFile[80];
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;
  uint32_t Ip_Int;
  char str[100];
  char IP_Group[15];

  //本机状态为监视
  if((Local.Status == 3)||(Local.Status == 4))
   {
     	Local.ForceEndWatch = 1;  //有呼叫时，强制关监视
    	 //WatchEnd_Func();
    	 return;
   }

  //本机状态为空闲
  if(Local.Status == 0)
   {
//    gettimeofday(&tv1, NULL);
    //获取对方地址
    memcpy(Remote.Addr[0], recv_buf+9, 20);
    memcpy(Remote.IP[0], recv_buf+29, 4);

    if(recv_buf[57] == 1)
     {
      //查看是否在其它组播组内
      DropMultiGroup(m_VideoSocket, NULL);

      Local.IP_Group[0] = recv_buf[58]; //组播地址
      Local.IP_Group[1] = recv_buf[59];
      Local.IP_Group[2] = recv_buf[60];
      Local.IP_Group[3] = recv_buf[61];
      sprintf(IP_Group, "%d.%d.%d.%d\0",
                     Local.IP_Group[0],Local.IP_Group[1],Local.IP_Group[2],Local.IP_Group[3]);
      AddMultiGroup(m_VideoSocket, IP_Group);
     }

    Ip_Int=inet_addr(cFromIP);
    memcpy(Remote.DenIP, &Ip_Int,4);
    printf("Remote.DenIP, %d.%d.%d.%d\0",
            Remote.DenIP[0],Remote.DenIP[1],Remote.DenIP[2],Remote.DenIP[3]);

    Local.Status = 2;  //状态为被对讲
    //显示对讲窗口
    DisplayTalkPicWindow(1);
    //开始播放视频
    StartPlayVideo(CIF_W, CIF_H);

    Local.RecordPic = 1;   //呼叫留照片
    Local.IFrameCount = 0; //I帧计数
    Local.IFrameNo = 3;    //留第几个I帧

    sprintf(wavFile, "%sring1.wav\0", wavPath);
    StartPlayWav(wavFile, 1);
    
    Local.CallConfirmFlag = 1; //设置在线标志
    Local.Timer1Num = 0;
    Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听
    Local.OnlineNum = 0;     //在线确认序号
    Local.OnlineFlag = 1;

	strcpy(CallListAddr,Remote.Addr[0]);
	CallListAddr[20] = '\0';
    if((Remote.DenIP[0] == Remote.IP[0][0]) && (Remote.DenIP[1] == Remote.IP[0][1])
      && (Remote.DenIP[2] == Remote.IP[0][2]) &&(Remote.DenIP[3] == Remote.IP[0][3]))
      {
       		Remote.isDirect = 0;
       		strcpy(str, Remote.Addr[0]);
       		strcat(str, "  直通对讲呼叫");
       		ShowStatusText(CALLX, CALLY, 3, cBlack, 1, 1, str, 0);
      }
    else
      {
       Remote.isDirect = 1;
       strcpy(str, Remote.Addr[0]);
       strcat(str, "  中转对讲呼叫");
       ShowStatusText(CALLX, CALLY , 3, cBlack, 1, 1, str, 0);
      }
	    //完成处理后再给应答
	    memcpy(send_b, recv_buf, 62);
	    send_b[7]=ASK;    //主叫
	    send_b[8]=CALLANSWER;//对讲应答
	    sendlength=62;
	    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);  
   }
  //本机为忙
  else
   {
    memcpy(send_b, recv_buf, 57);
    send_b[7]=ASK;    //主叫
    send_b[8]=LINEUSE;//占线应答
    sendlength=57;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);
    #ifdef _DEBUG
      printf("对方发起对讲呼叫\n");
    #endif
   }

}
//-----------------------------------------------------------------------
//对讲占线应答
void RecvTalkLineUse_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;

  //锁定互斥锁
  pthread_mutex_lock (&Local.udp_lock);
  if(recv_buf[7] == ASK)   //应答
   for(i=0; i<UDPSENDMAX; i++)
    if(Multi_Udp_Buff[i].isValid == 1)
     if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
      if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
       if((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)||(Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS))
        if(Multi_Udp_Buff[i].buf[7] == ASK)
         if(Multi_Udp_Buff[i].buf[8] == CALL)
          if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
           {
           	 Multi_Udp_Buff[i].isValid = 0;
      		 //     Local.Status = 0;  //设状态为空闲
            	ShowStatusText(CALLX, CALLY , 3, cBlack, 1, 1, "对方占线", 0);

		Local.Status = 0;
		DisplayMainWindow(MainWindow);
#if 0		
           	 //延时清提示信息标志
            	PicStatBuf.Type = 16;
            	PicStatBuf.Time = 0;
            	PicStatBuf.Flag = 1;
            #ifdef _DEBUG
              	printf("收到对讲占线应答\n");
            #endif
#endif
            break;
           }
  //打开互斥锁
  pthread_mutex_unlock (&Local.udp_lock);
}
//-----------------------------------------------------------------------
//对讲呼叫应答
void RecvTalkCallAnswer_Func(unsigned char *recv_buf, char *cFromIP)
{
  char wavFile[80];
  int i,j;
  int isAddrOK;
  uint32_t Ip_Int;
  //锁定互斥锁
  pthread_mutex_lock (&Local.udp_lock); //收到呼叫应答后 比较缓冲区的是否先前发送的对讲呼叫
  if(recv_buf[7] == ASK)   //应答
   for(i=0; i<UDPSENDMAX; i++)
    if(Multi_Udp_Buff[i].isValid == 1)
     if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
      if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
       if((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)||(Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS))
        if(Multi_Udp_Buff[i].buf[7] == ASK)
         if(Multi_Udp_Buff[i].buf[8] == CALL)
          if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0) //先前的呼叫对讲命令
          {
            	Multi_Udp_Buff[i].isValid = 0; //置为无效
            	if((LocalCfg.Addr[0] == 'S')||(LocalCfg.Addr[0] == 'B')||(LocalCfg.Addr[0] == 'Z')
               		||(Remote.DenNum == 1))
             	{
              		Ip_Int=inet_addr(cFromIP);
              		memcpy(Remote.DenIP, &Ip_Int,4); //对方IP或视频服务器IP
             	}
            	else if(Remote.DenNum > 1)
                {
               		Remote.DenIP[0] = Remote.GroupIP[0];
               		Remote.DenIP[1] = Remote.GroupIP[1];
               		Remote.DenIP[2] = Remote.GroupIP[2];
               		Remote.DenIP[3] = Remote.GroupIP[3];
              	}
				
            	//开始录制视频
            	#ifdef _R2RVIDEO           //户户对讲视频
              	StartRecVideo(CIF_W, CIF_H);
            	#endif

            	Local.Status = 1;  			//状态为主叫对讲
            	Local.CallConfirmFlag = 1;  //设置在线标志
            	Local.Timer1Num = 0;
            	Local.TimeOut   = 0;        //监视超时,  通话超时,  呼叫超时，无人接听
            	Local.OnlineNum = 0;        //在线确认序号
            	Local.OnlineFlag = 1;
            	printf("Local.Status = %d\n", Local.Status);

            	sprintf(wavFile, "%sring1.wav\0", wavPath);
            	StartPlayWav(wavFile, 1);

            	#ifdef _DEBUG
              	printf("收到对讲呼叫应答\n");
            	#endif
            	ShowStatusText(CALLX, CALLY , 3, cBlack, 1, 1, "收到对讲呼叫应答", 0);
            	break;
           }
  //打开互斥锁
  pthread_mutex_unlock (&Local.udp_lock);
}
//-----------------------------------------------------------------------
//对讲开始通话  由被叫方发起，主叫方应答
void RecvTalkCallStart_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;
  uint32_t Ip_Int;
  struct timeval tv1,tv2;
  char OsdContent[20];
  //时钟
  time_t t;

  Local.TalkTimeOut = TALKTIMEOUT; //通话最长时间
  //本机为主叫方 应答
  if((Local.Status == 1)&&(recv_buf[7] == ASK))
   {
    memcpy(send_b, recv_buf, 57);
    send_b[7]=REPLY;    //应答
    sendlength=57;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);

    ExitGroup(recv_buf);      //向其它被叫方退出组播组命令

    //获取被叫方地址
    memcpy(Remote.Addr[0], recv_buf+33, 20);
    memcpy(Remote.IP[0], recv_buf+53, 4);
    Remote.DenNum = 1;
        
    Ip_Int=inet_addr(cFromIP);
    memcpy(Remote.DenIP, &Ip_Int,4);

    StopPlayWavFile();  //关闭铃音
    usleep(200*1000);    //延时是为了等待声音播放完成，否则会有杂音
    //打开音频录制、播放，视频播放
    #ifdef _R2RVIDEO           //户户对讲视频
      StartPlayVideo(CIF_W, CIF_H);
    #endif
    StartRecAudio();
    StartPlayAudio();

	DisplayImageButton(&talkpic_button[4],IMAGEUP,0);
	Local.ShowRecPic = 1;
    Local.Status = 5;  //状态为主叫通话
    Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听

    #ifdef _DEBUG
      printf("对方开始通话\n");
    #endif
    ShowStatusText(CALLX, CALLY , 3, cBlack, 1, 1, "开始通话", 0);
    OpenOsd();   //打开OSD
    sprintf(OsdContent, "%02d:%02d\0", Local.TimeOut/INTRPERSEC/60,
           (Local.TimeOut/INTRPERSEC)%60);
    OsdContent[2] = ',';
    OsdContent[5] = '\0';
    ShowOsd(OsdContent);
    time(&t);
    Local.call_tm_t = localtime(&t);
   }
  else  //本机为被叫方 发起
   if(Local.Status == 2)
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    //本机主动
    if(recv_buf[7] == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)||(Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS))
          if(Multi_Udp_Buff[i].buf[7] == ASK)
           if(Multi_Udp_Buff[i].buf[8] == CALLSTART)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              //查看是否在其它组播组内
              DropMultiGroup(m_VideoSocket, NULL);

              Multi_Udp_Buff[i].isValid = 0;

          //    StopPlayWavFile();  //关闭铃音
              
              //打开音频录制、播放，视频录制
              #ifdef _R2RVIDEO           //户户对讲视频
                if((Remote.Addr[0][0] == 'S')||(Remote.Addr[0][0] == 'B')||(Remote.Addr[0][0] == 'Z'))
                 	 StartRecVideo(CIF_W, CIF_H);
              #endif
              StartRecAudio();
              StartPlayAudio();
              Local.Status = 6;  //状态为被叫通话
              Local.TimeOut = 0;       //监视超时,  通话超时,  呼叫超时，无人接听

              //Local.RecordPic = 2;   //通话留照片
              //Local.IFrameCount = 0; //I帧计数
              //Local.IFrameNo = 2;    //留第几个I帧
              
              #ifdef _DEBUG
                printf("对方应答本机开始通话\n");
              #endif
             ShowStatusText(CALLX,CALLY , 3, cBlack, 1, 1, "开始通话", 0);
              OpenOsd();   //打开OSD
              sprintf(OsdContent, "%02d:%02d\0", Local.TimeOut/INTRPERSEC/60,
                     (Local.TimeOut/INTRPERSEC)%60);
              OsdContent[2] = ',';
              OsdContent[5] = '\0';
              ShowOsd(OsdContent);
              time(&t);
              Local.call_tm_t=localtime(&t);

              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//-----------------------------------------------------------------------
//对讲在线确认
void RecvTalkCallConfirm_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  //本机为主叫方
  if(((Local.Status == 1)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
     &&(recv_buf[7] == ASK))
   {
    memcpy(send_b, recv_buf, 61);
    send_b[7]=REPLY;    //应答
    sendlength=61;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);
    Local.CallConfirmFlag = 1;
    #ifdef _DEBUG
   //   printf("收到对讲在线确认\n");
    #endif
   }
  else  //本机为被叫方
   if(((Local.Status == 2)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
      &&(recv_buf[7] == REPLY))
   {
    Local.CallConfirmFlag = 1;
    #ifdef _DEBUG
//      printf("收到对方应答本机对讲在线确认\n");
    #endif
   }
}
//-----------------------------------------------------------------------
//对讲呼叫结束
void RecvTalkCallEnd_Func(unsigned char *recv_buf, char *cFromIP)
{
	int i,j;
	int isAddrOK;
	unsigned char send_b[1520];
	int sendlength;
	//本机被动
	if(((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
	||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))
	&&(recv_buf[7] == ASK))
	{
		Local.OnlineFlag = 0;
		Local.CallConfirmFlag = 0; //设置在线标志

		memcpy(send_b, recv_buf, 57);
		send_b[7]=REPLY;    //应答
		sendlength=57;
		UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);

		ExitGroup(recv_buf);      //向其它被叫方退出组播组命令
		TalkEnd_ClearStatus();
		if(Local.Status == 10)
		{
			if(Local.CurrentWindow == TalkPicWindow)
				ShowStatusText(50, 130 , 3, cBlack, 1, 1, "结束留影", 0);
		}
		else
		{
			if(Local.CurrentWindow == TalkPicWindow)
				ShowStatusText(50, 130 , 3, cBlack, 1, 1, "对方结束对讲", 0);
		}
#ifdef _DEBUG
		printf("对方结束对讲\n");
#endif
	}
	else  //本机主动
	if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
	||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))   
	{

	Local.OnlineFlag = 0;
	Local.CallConfirmFlag = 0; //设置在线标志

	//锁定互斥锁
	pthread_mutex_lock (&Local.udp_lock);
	//本机主动
	if(recv_buf[7] == REPLY)
	for(i=0; i<UDPSENDMAX; i++)
	if(Multi_Udp_Buff[i].isValid == 1)
	if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
	if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
	if((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)||(Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS))
	if(Multi_Udp_Buff[i].buf[7] == ASK)
	if(Multi_Udp_Buff[i].buf[8] == CALLEND)
	if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
	{
		Multi_Udp_Buff[i].isValid = 0;
		TalkEnd_ClearStatus();

		if(Local.CurrentWindow == TalkPicWindow)
		ShowStatusText(50, 130 , 3, cWhite, 1, 1, "本机结束对讲", 0);
#ifdef _DEBUG
		printf("对方应答本机结束对讲\n");
#endif
		break;
	}
	//打开互斥锁
	pthread_mutex_unlock (&Local.udp_lock);
   }
}
//-----------------------------------------------------------------------
//对讲结束，清状态和关闭音视频
void TalkEnd_ClearStatus(void)
{

    StopPlayWavFile();  //关闭铃音

    //查看是否在其它组播组内
    DropMultiGroup(m_VideoSocket, NULL);
    CloseOsd(); //关闭OSD
    switch(Local.Status)
     {
		case 1: //本机主叫
#ifdef _R2RVIDEO           //户户对讲视频
		StopRecVideo();
#endif      
		break;
		case 2: //本机被叫
		StopPlayVideo();

		if(Local.HavePicRecorded == 1)  //有照片已录制
		{
		Local.HavePicRecorded = 0;
		SaveToFlash(5);       //未接听
		}
		break;
		case 5: //本机主叫通话
#ifdef _R2RVIDEO           //户户对讲视频
		StopRecVideo();
#endif
		StopPlayVideo();
		StopRecAudio();
		StopPlayAudio();
		//ShowVideoWindow();
		break;
		case 6: //本机被叫通话
#ifdef _R2RVIDEO           //户户对讲视频
		StopRecVideo();
#endif
		StopPlayVideo();
		StopRecAudio();
		StopPlayAudio();
		//ShowVideoWindow();
		if(Local.HavePicRecorded == 1)  //有照片已录制
		{
			Local.HavePicRecorded = 0;
			SaveToFlash(6);    //通话
		}             
        break;
     }
    //延时清提示信息标志
//    PicStatBuf.Type = Local.CurrentWindow;
	DisplayMainWindow(0);
	Local.Status = 0;
//    PicStatBuf.Time = 0;
//    PicStatBuf.Flag = 1;
}
//-----------------------------------------------------------------------
//远程开锁
void  RecvTalkRemoteOpenLock_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  #ifdef _ZHUHAIJINZHEN      //珠海进祯  呼叫时可开锁
  if((Local.Status == 2)||(Local.Status == 6))  //状态为被对讲 或被叫通话
  #else
  if(Local.Status == 6)  //状态为被对讲
  #endif
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    //本机主动
    if(recv_buf[7] == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)||(Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS))
          if(Multi_Udp_Buff[i].buf[7] == ASK)
           if(Multi_Udp_Buff[i].buf[8] == REMOTEOPENLOCK)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;
              #ifdef _ZHUHAIJINZHEN      //珠海进祯  呼叫时可开锁  通话时开锁2秒后挂断
              if(Local.Status == 6)  //状态为被叫通话
               {
                if((Local.TimeOut + 2*(1000/INTRTIME)) < TALKTIMEOUT)
                  Local.TalkTimeOut = Local.TimeOut + 2*(1000/INTRTIME);
                else
                  Local.TalkTimeOut = TALKTIMEOUT;
               }
              #endif
              #ifdef _DEBUG
                printf("对方应答本机远程开锁\n");
              #endif
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//-----------------------------------------------------------------------
//放大(720*480)
void RecvTalkZoomOut_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  //本机被动
  if(((Local.Status == 5)||(Local.Status == 6))
     &&(recv_buf[7] == ASK))
   {
/*    memcpy(send_b, recv_buf, 57);
    send_b[7]=REPLY;    //应答
    sendlength=57;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);
    StopRecVideo();   //352*240
    StartRecVideo(720, 480);  //720*480
    #ifdef _DEBUG
      printf("对方放大图像\n");
    #endif    */
   }
  else  //本机主动
   if((Local.Status == 5)||(Local.Status == 6))
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    //本机主动
    if(recv_buf[7] == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)||(Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS))
          if(Multi_Udp_Buff[i].buf[7] == ASK)
           if(Multi_Udp_Buff[i].buf[8] == ZOOMOUT)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;
              Local.ZoomInOutFlag = 1;   //正在放大缩小中
              if(Local.PlayPicSize == 1)
               {
                if(Local.Status != 0)
                  StopPlayVideo();   //352*288
                //清屏  第二页  视频720*480
                memset(fbmem + f_data.buf_len, 16, f_data.uv_offset);
                memset(fbmem + f_data.buf_len + f_data.uv_offset, 128, f_data.uv_offset/4);
                memset(fbmem + f_data.buf_len + f_data.uv_offset*5/4, 128, f_data.uv_offset/4);
                if(Local.Status != 0)
                  StartPlayVideo(D1_W, D1_H);  //720*480
                //for(j=0; j<2; j++)
                //  DisplayImageButton(&zoom_button[j], IMAGEUP, 1);
                Local.ZoomInOutFlag = 0;   //正在放大缩小中
                #ifdef _DEBUG
                  printf("对方应答本机放大图像\n");
                #endif
               }
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }
}
//-----------------------------------------------------------------------
//缩小(352*288)
void RecvTalkZoomIn_Func(unsigned char *recv_buf, char *cFromIP)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  int sendlength;

  //本机被动
  if(((Local.Status == 5)||(Local.Status == 6))
     &&(recv_buf[7] == ASK))
   {
/*    memcpy(send_b, recv_buf, 57);
    send_b[7]=REPLY;    //应答
    sendlength=57;
    UdpSendBuff(m_VideoSocket, cFromIP, send_b , sendlength);
    StopRecVideo();   //720*480
    StartRecVideo(CIF_W, CIF_H);  //352*288
    #ifdef _DEBUG
      printf("对方缩小图像\n");
    #endif           */
   }
  else  //本机主动
   if((Local.Status == 5)||(Local.Status == 6))
   {
    //锁定互斥锁
    pthread_mutex_lock (&Local.udp_lock);
    //本机主动
    if(recv_buf[7] == REPLY)
     for(i=0; i<UDPSENDMAX; i++)
      if(Multi_Udp_Buff[i].isValid == 1)
       if(Multi_Udp_Buff[i].m_Socket == m_VideoSocket)
        if(Multi_Udp_Buff[i].SendNum  < MAXSENDNUM)
         if((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)||(Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS))
          if(Multi_Udp_Buff[i].buf[7] == ASK)
           if(Multi_Udp_Buff[i].buf[8] == ZOOMIN)
            if(strcmp(Multi_Udp_Buff[i].RemoteHost, cFromIP) == 0)
             {
              Multi_Udp_Buff[i].isValid = 0;
              Local.ZoomInOutFlag = 1;   //正在放大缩小中
              if(Local.PlayPicSize == 2)
               {
                //显示视频窗口
                //ShowVideoWindow();
                if(Local.Status != 0)
                  StopPlayVideo();   //720*480
                if(Local.Status != 0)
                  StartPlayVideo(CIF_W, CIF_H);  //352*288
                Local.ZoomInOutFlag = 0;   //正在放大缩小中
                #ifdef _DEBUG
                  printf("对方应答本机缩小图像\n");
                #endif
               }
              break;
             }
    //打开互斥锁
    pthread_mutex_unlock (&Local.udp_lock);
   }      
}
//-----------------------------------------------------------------------
//对讲数据
void RecvTalkCallUpDown_Func(unsigned char *recv_buf, char *cFromIP, int length)
{
  int i,j;
  int isAddrOK;
  unsigned char send_b[1520];
  short PackIsExist; //数据包已接收标志
  short FrameIsNew;  //数据包是否是新帧的开始
  TempVideoNode1 * tmp_videonode;
  TempAudioNode1 * tmp_audionode;
  int isFull;
  struct talkdata1 talkdata;
//  struct tempvideobuf1 InitVideoCon;
  if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
     ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))  //状态为对讲
   switch(recv_buf[61])
   {
    case 1://音频
           //帧序号
   //       if(AudioMuteFlag == 0)
            //写留影数据函数, 数据  长度  类型 1--音频  2--视频I  3--视频P
           if((Local.Status == 5)||(Local.Status == 6)||(Local.Status == 10))
            {
             memcpy(&talkdata, recv_buf + 9, sizeof(talkdata));
             if(temp_audio_n >= G711NUM)
              {
               temp_audio_n = G711NUM;
               #ifdef _DEBUG
                 printf("temp_audio is full\n");
               #endif
              }
             else
              {
               tmp_audionode = find_audionode(TempAudioNode_h, talkdata.Frameno, talkdata.CurrPackage);
               if(tmp_audionode == NULL)
                {
                 isFull = creat_audionode(TempAudioNode_h, talkdata, recv_buf, length);
                 PackIsExist = 0;
                }
               else
                 PackIsExist = 1;

             if(PackIsExist == 0)
              {
               TimeStamp.OldCurrAudio = TimeStamp.CurrAudio; //上一次当前音频时间
               TimeStamp.CurrAudio = talkdata.timestamp;

             //  temp_audio_n ++;
               temp_audio_n = length_audionode(TempAudioNode_h);
               if(temp_audio_n >= 6) //VPLAYNUM/2 4个包 128ms
                {
                   sem_post(&audiorec2playsem);
                }
              }
            }
           } 
           break;
    case 2://视频  I帧  352*288
    case 3://视频  P帧  352*288
    case 4://视频  I帧  720*480
    case 5://视频  P帧  720*480
           //  2 被叫对讲  3 监视  5 主叫通话
               //6 被叫通话  8 被叫留影留言预备
               //10 被叫留影留言
           if((Local.Status == 2)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
            {
                  //帧序号
                  memcpy(&talkdata, recv_buf + 9, sizeof(talkdata));
                  PackIsExist = 0;
                  FrameIsNew = 1;

                  if(temp_video_n >= MP4VNUM)
                   {
                    temp_video_n = MP4VNUM;
                    #ifdef _DEBUG
                      printf("temp_video is full\n");
                    #endif
                   }
                  else
                   {
                    //锁定videoplaybuf[vpbuf_iget].buffer
                    pthread_mutex_lock(&sync_s.video_play_lock);
                    tmp_videonode = find_videonode(TempVideoNode_h, talkdata.Frameno, talkdata.CurrPackage);
                    if(tmp_videonode == NULL)
                      isFull = creat_videonode(TempVideoNode_h, talkdata, recv_buf, length);
                    else
                      isFull = add_videonode(tmp_videonode, talkdata, recv_buf, length);

                    if(isFull == 1)
                     {
                           TimeStamp.OldCurrVideo = TimeStamp.CurrVideo; //上一次当前视频时间
                           TimeStamp.CurrVideo = talkdata.timestamp;

                        //   temp_video_n ++;
                           temp_video_n = length_videonode(TempVideoNode_h);
                           if(temp_video_n >= 3)   //3帧 40*3 120
                            {
                               sem_post(&videorec2playsem);
                            }
                    }
                   //解锁
                  pthread_mutex_unlock(&sync_s.video_play_lock);
                 }
             }
    break;
  }
}
//-----------------------------------------------------------------------
void ForceIFrame_Func(void)  //强制I帧
{
  int i;

  for(i=0; i<UDPSENDMAX; i++)
   if(Multi_Udp_Buff[i].isValid == 0)
    {
     //锁定互斥锁
     pthread_mutex_lock (&Local.udp_lock);
     //只发送一次
     Multi_Udp_Buff[i].SendNum = 5;
     Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
     Multi_Udp_Buff[i].CurrOrder = 0;
     sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",Remote.DenIP[0],
             Remote.DenIP[1],Remote.DenIP[2],Remote.DenIP[3]);
     //头部
     memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
     //命令  ,子网广播解析
     if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
       ||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))
         Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
     if((Local.Status == 3)||(Local.Status == 4))
         Multi_Udp_Buff[i].buf[6] = VIDEOWATCH;
     Multi_Udp_Buff[i].buf[7] = ASK;    //主叫
     Multi_Udp_Buff[i].buf[8] = FORCEIFRAME;    //FORCEIFRAME

     memcpy(Multi_Udp_Buff[i].buf+9,LocalCfg.Addr,20);
     memcpy(Multi_Udp_Buff[i].buf+29,LocalCfg.IP,4);
     memcpy(Multi_Udp_Buff[i].buf+33,Remote.Addr[0],20);
     memcpy(Multi_Udp_Buff[i].buf+53,Remote.IP[0],4);

     Multi_Udp_Buff[i].nlength = 57;
     Multi_Udp_Buff[i].DelayTime = 100;
     Multi_Udp_Buff[i].isValid = 1;

     //打开互斥锁
     pthread_mutex_unlock (&Local.udp_lock);

     sem_post(&multi_send_sem);
     break;
    }
}
//-----------------------------------------------------------------------
void ExitGroup(unsigned char *buf)      //向其它被叫方退出组播组命令
{
}
//-----------------------------------------------------------------------
//查找设备
void RecvFindEquip_Func(unsigned char *recv_buf, char *cFromIP, int length, int m_Socket)
{
  int i,j;
  int newlength;
  int isAddrOK;
  //时钟
  time_t t;
  struct tm *tm_t;
  unsigned char send_b[1520];
  int sendlength;
//  char addr[20];
  i = 0;
  isAddrOK = 1;

  for(j=32; j<32+12; j++)
   if(LocalCfg.Addr[j-32] != recv_buf[j])
    {
     isAddrOK = 0;
     break;
    }
  //地址匹配
  if(isAddrOK == 1)
   {
    memcpy(send_b, recv_buf, length);
    send_b[7] = REPLY;    //应答
    sendlength = length;
    UdpSendBuff(m_Socket, cFromIP, send_b , sendlength);
   }
}
//---------------------------------------------------------------------------
