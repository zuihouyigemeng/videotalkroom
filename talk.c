#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>       //sem_t

#define CommonH
#include "common.h"
extern char sPath[80];
extern char NullAddr[21];   //空字符串

struct InfoStatus1 InfoStatus;

InfoNode1 *CurrInfo_Node; //当前信息结点
//短信息结构
extern struct Info1 Info[INFOTYPENUM];
// *h保存表头结点的指针，*p指向当前结点的前一个结点，*s指向当前结点
extern InfoNode1 *InfoNode_h[INFOTYPENUM];



void DisplayTalkWindow(void);    //显示对讲窗口（一级）
void OperateTalkWindow(short wintype, int currwindow);    //对讲窗口操作（一级）
void DisplayWatchWindow(void);    //监视窗口操作（二级）
void OperateWatchWindow(short wintype, int currwindow);    //监视窗口操作（二级）
void DisplayTalkPicWindow(int type);    //显示图像操作窗口（二级）,有呼叫时自动显示
void OperateTalkPicWindow(short wintype, int currwindow);    //对讲图像操作窗口（二级）

//void ShowVideoWindow(void);    //刷新视频窗口

void Talk_Func(void);       //通话 接听
void CallCenter_Func(void);  //呼叫中心
void CallR2R_Func(void);    //户户对讲
void Watch_Func(void);       //监视
void TalkEnd_Func(void);
void ZoomOut_Func(void); //视频放大
void ZoomIn_Func(void);  //视频缩小
//void WatchEnd_Func(void);
void CallTimeOut_Func(void); //呼叫超时
void OpenLock_Func(void);  //开锁
void TalkWindowInit(void);
void DisplayAuthenticationWindow(void);
void DisplayPhonebookWindow(void);

void TalkWindowInit(void)
{
	int i;
	char jpgfilename[80];
	int talk_menu_button_y[5] = {57,132,207,282,357};
	int talk_keynum_button_x[12] ={565,485,565,645,485,565,645,485,565,645,485,645};
	int talk_keynum_button_y[12] = {355,190,190,190,245,245,245,300,300,300,355,355};
	int talk_hotkey_button_x[3] = {229,309,389};
	int talkpic_button_y[5] = {125,194,263,332,125};
	int talk_bg_x[8]={169,169,169,169,169,169,169,169};
	int talk_bg_y[8] = {72,72,72,72,72,72,72,72};
	int talk_banner_x[9] = {178,178,178,178,178,178,178,178,178};
	int talk_banner_y[9] = {127,127,127,127,127,127,127,127,127};
	int talk_comkey_x[19] ={195,316};
	int talk_comkey_y[19]={470,470};
	int talk_message_button_x[5] = {184,286,388,488,592};
	int talk_info_button_y[9] = {163,197,231,265,299,333,367,401,435};

	struct TImageButton info_button[2];
	///talk menu
	sprintf(jpgfilename,"%scall_image1.jpg\0",sPath);
	ImageLoadFromFile(jpgfilename,&talk_main_image);
	talk_main_image.xLeft = 0;
	talk_main_image.yTop = 0;

	
	//呼叫界面菜单键
	for(i=0;i<5;i++)
	{
		sprintf(jpgfilename,"%snetcomm%d_down.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talk_menu_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%snetcomm%d_up.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talk_menu_button[i],IMAGEUP);
		talk_menu_button[i].xLeft = 0;
		talk_menu_button[i].yTop = talk_menu_button_y[i];
	}

	//可视对讲输入界面按钮
	for(i=0;i<12;i++)
	{
		sprintf(jpgfilename,"%snum%02d_down.jpg\0",sPath,i);
		ImageButtonLoadFromFile(jpgfilename,&talk_keynum_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%snum%02d_up.jpg\0",sPath,i);
		ImageButtonLoadFromFile(jpgfilename,&talk_keynum_button[i],IMAGEUP);
		talk_keynum_button[i].xLeft = talk_keynum_button_x[i];
		talk_keynum_button[i].yTop = talk_keynum_button_y[i];
	}

	//可视对讲界面三个快捷按钮
	for(i=0;i<3;i++)
	{
		sprintf(jpgfilename,"%snum%02d_down.jpg\0",sPath,i+12);
		ImageButtonLoadFromFile(jpgfilename,&talk_hotkey_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%snum%02d_up.jpg\0",sPath,i+12);
		ImageButtonLoadFromFile(jpgfilename,&talk_hotkey_button[i],IMAGEUP);
		talk_hotkey_button[i].xLeft = talk_hotkey_button_x[i];
		talk_hotkey_button[i].yTop = 355;
	}

	/// 对讲画面的4 个按钮
	for(i=0;i<5;i++)
	{
		sprintf(jpgfilename,"%stalk%d_down.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talkpic_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%stalk%d_up.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talkpic_button[i],IMAGEUP);
		talkpic_button[i].xLeft = 509;
		talkpic_button[i].yTop = talkpic_button_y[i];
	}

	
	sprintf(jpgfilename,"%sphonebook_edit2.jpg\0",sPath);
	EditLoadFromFile(jpgfilename,&roomaddr_edit);
	roomaddr_edit.CursorX = 10;
	roomaddr_edit.CursorY = 13;
	roomaddr_edit.CursorHeight = 23;
	roomaddr_edit.CursorCorlor = cBlack;
	roomaddr_edit.fWidth = 8;
	roomaddr_edit.Cursor_H = (unsigned char*)malloc(1*roomaddr_edit.CursorHeight*3/2);
	SavePicS_D_Func(roomaddr_edit.CursorX,roomaddr_edit.CursorY,1,roomaddr_edit.CursorHeight,
					roomaddr_edit.Cursor_H,roomaddr_edit.width,roomaddr_edit.height,roomaddr_edit.image);

	//可视呼叫界面的地址框
	talkaddr_edit = roomaddr_edit;
	talkaddr_edit.xLeft = 220;
	talkaddr_edit.yTop = 190;
	talkaddr_edit.Text[0] = '\0';
	talkaddr_edit.BoxLen = 0;
	talkaddr_edit.MaxLen = 10;
	talkaddr_edit.Focus = 0;
	talkaddr_edit.Visible = 0;


	///message
	#if 0
	for(i=0;i<16;i++)
	{
		sprintf(jpgfilename,"%smessage_image%d\n",sPath,i+1);
		ImageLoadFromFile(jpgfilename,&talk_message_image[i]);
		talk_message_image[i].xLeft = 0;
		talk_message_image[i].yTop = 0;
	}
	#endif
	for(i=0;i<19;i++)
	{
		sprintf(jpgfilename,"%scomkey%d.jpg\0",sPath,i+1);
		ImageLoadFromFile(jpgfilename,&talk_comkey[i]);
	}
	
	for(i=0;i<4;i++)
	{
		sprintf(jpgfilename,"%smessage%d_down.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talk_message_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%smessage%d_up.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talk_message_button[i],IMAGEUP);
		talk_message_button[i].xLeft = talk_message_button_x[i];
		talk_message_button[i].yTop = 88;
	}
	sprintf(jpgfilename,"%scomkey19.jpg\0",sPath);
	ImageButtonLoadFromFile(jpgfilename,&talk_message_button[4],IMAGEDOWN);
	sprintf(jpgfilename,"%scomkey19.jpg\0",sPath);
	ImageButtonLoadFromFile(jpgfilename,&talk_message_button[4],IMAGEUP);
	talk_message_button[4].xLeft = 592;
	talk_message_button[4].yTop = 88;
	

	AreaMessageComKey[0] = talk_comkey[0];
	AreaMessageComKey[0].xLeft = 195;
	AreaMessageComKey[0].yTop = 470;

	AreaMessageComKey[1] = talk_comkey[1];
	AreaMessageComKey[1].xLeft = 316;
	AreaMessageComKey[1].yTop = 470;

	AreaMessageComKey[2] = talk_comkey[13];
	AreaMessageComKey[2].xLeft = 659;
	AreaMessageComKey[2].yTop = 291;

	AreaMessageComKey[3] = talk_comkey[9];
	AreaMessageComKey[3].xLeft = 659;
	AreaMessageComKey[3].yTop = 359;

	AreaMessageComKey[4] = talk_comkey[14];
	AreaMessageComKey[4].xLeft = 659;
	AreaMessageComKey[4].yTop = 427;

	for(i=0;i<5;i++)
	{
		LocalMessageComKey[i] = AreaMessageComKey[i];
	}
	LocalMessageComKey[5] = talk_comkey[15];
	LocalMessageComKey[5].xLeft = 659;
	LocalMessageComKey[5].yTop = 223;




	for(i=0;i<5;i++)
	{
		PushMessageComKey[i] = AreaMessageComKey[i];
		CustomMessageComKey[i] = AreaMessageComKey[i];
	}

	MisscallComKey[0] = talk_comkey[0];
	MisscallComKey[0].xLeft = 195;
	MisscallComKey[0].yTop = 470;

	MisscallComKey[1] = talk_comkey[1];
	MisscallComKey[1].xLeft = 316;
	MisscallComKey[1].yTop = 470;

	MisscallComKey[2] = talk_comkey[12];
	MisscallComKey[2].xLeft = 659;
	MisscallComKey[2].yTop = 291;

	MisscallComKey[3] = talk_comkey[9];
	MisscallComKey[3].xLeft = 659;
	MisscallComKey[3].yTop = 359;

	MisscallComKey[4] = talk_comkey[14];
	MisscallComKey[4].xLeft = 659;
	MisscallComKey[4].yTop = 427;

	MisscallComKey[5] = talk_comkey[11];
	MisscallComKey[5].xLeft = 659;
	MisscallComKey[5].yTop = 223;


	for(i=0;i<5;i++)
	{
		CalledComKey[i] = MisscallComKey[i];
		CallComKey[i] = MisscallComKey[i];
	}
	CalledComKey[5] = talk_comkey[10];
	CalledComKey[5].xLeft = 659;
	CalledComKey[5].yTop = 223;
	CallComKey[5] = CalledComKey[5];
	
	for(i=0;i<9;i++)
	{
		sprintf(jpgfilename,"%sbanner%d.jpg\0",sPath,i+1);
		ImageLoadFromFile(jpgfilename,&talk_banner[i]);
		talk_banner[i].xLeft = talk_banner_x[i];
		talk_banner[i].yTop = talk_banner_y[i];
	}


	for(i=0;i<8;i++)
	{
		sprintf(jpgfilename,"%sbg%d.jpg\0",sPath,i+1);
		ImageLoadFromFile(jpgfilename,&talk_bg[i]);
		talk_bg[i].xLeft = talk_bg_x[i];
		talk_bg[i].yTop = talk_bg_y[i];
	}

	for(i=0;i<3;i++)
	{
		sprintf(jpgfilename,"%scallrec%d_down.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talk_callrec_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%scallrec%d_up.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&talk_callrec_button[i],IMAGEUP);
		talk_callrec_button[i].xLeft = talk_message_button_x[i];
		talk_callrec_button[i].yTop = 88;
	}
	talk_callrec_button[3] = talk_message_button[4];

	for(i=0;i<9;i++)
	{
		if(i%2==0)
		{
			sprintf(jpgfilename,"%sinfo1_down.jpg\0",sPath);
			ImageButtonLoadFromFile(jpgfilename,&talk_info_button[i],IMAGEDOWN);
			sprintf(jpgfilename,"%sinfo1_up.jpg\0",sPath);
			ImageButtonLoadFromFile(jpgfilename,&talk_info_button[i],IMAGEUP);
		}
		else
		{
			sprintf(jpgfilename,"%sinfo2_down.jpg\0",sPath);
			ImageButtonLoadFromFile(jpgfilename,&talk_info_button[i],IMAGEDOWN);
			sprintf(jpgfilename,"%sinfo2_up.jpg\0",sPath);
			ImageButtonLoadFromFile(jpgfilename,&talk_info_button[i],IMAGEUP);
		}
		talk_info_button[i].xLeft = 182;
		talk_info_button[i].yTop = talk_info_button_y[i];
	}


	talk_infocontent_comkey[0] = talk_comkey[2];
	talk_infocontent_comkey[0].xLeft = 215;
	talk_infocontent_comkey[0].yTop = 458;

	talk_infocontent_comkey[1] = talk_comkey[3];
	talk_infocontent_comkey[1].xLeft = 348;
	talk_infocontent_comkey[1].yTop = 458;

	talk_infocontent_comkey[2] = talk_comkey[4];
	talk_infocontent_comkey[2].xLeft = 489;
	talk_infocontent_comkey[2].yTop = 458;

	talk_infocontent_comkey[3] = talk_comkey[5];
	talk_infocontent_comkey[3].xLeft = 630;
	talk_infocontent_comkey[3].yTop = 458;


	
}

void ShowInfoList(int infotype)    //信息显示方框（一级）  信息列表
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
	int ascnum;
	xLeft = 120;
	yTop = 36;
	tmp_len = 10;


	InfoStatus.CurrWin = 0;    //信息列表

	switch(infotype)
	{
	case 0: //小区消息
		for(i = 0; i < INFONUMPERPAGE; i++)
		{
			DisplayImageButton(&talk_info_button[i], IMAGEUP, 0);
		}

		if(Info[infotype].TotalNum > 0)
		{
			//总页数
			if((Info[infotype].TotalNum % INFONUMPERPAGE) == 0)
				Info[infotype].TotalInfoPage = Info[infotype].TotalNum /INFONUMPERPAGE;
			else
				Info[infotype].TotalInfoPage = Info[infotype].TotalNum /INFONUMPERPAGE + 1;

		//当前页
			Info[infotype].CurrentInfoPage = InfoStatus.CurrNo /INFONUMPERPAGE + 1;
			if(Info[infotype].CurrentInfoPage < Info[infotype].TotalInfoPage)
				PageTotalNum = INFONUMPERPAGE;
			else
				PageTotalNum = Info[infotype].TotalNum - (Info[infotype].CurrentInfoPage - 1)*INFONUMPERPAGE;

			//当前信息在本页中的位置
			NoInPage = (InfoStatus.CurrNo)%INFONUMPERPAGE;

			DisplayImageButton(&talk_info_button[NoInPage], IMAGEDOWN, 0);

			for(i=0;i<5;i++)
			{
				DisplayImage(&AreaMessageComKey[i],0);
			}
			
			for(i = 0; i < PageTotalNum; i++)
			{
				if(i !=  NoInPage)
					fontcolor = cBlue;
				else
					fontcolor = cWhite;

				tmp_node=locate_infonode(InfoNode_h[infotype], (Info[infotype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);

			//序号
				sprintf(tmp_con, "%02d\0", (Info[infotype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);
				outxy16(talk_info_button[i].xLeft+10, talk_info_button[i].yTop +5, 1,	fontcolor, 1, 1, tmp_con, 0, 0);


				//内容
				if(tmp_node->Content.Length >= tmp_len)
				{
					memcpy(tmp_con, tmp_node->Content.Content, tmp_len);
					tmp_con[tmp_len] = '\0';
					for(j=0; j<tmp_len; j++)
					if(tmp_con[j] == '\r')
						tmp_con[j] = '\0';
					if(tmp_con[8] < 0xA1)
						tmp_con[9] = '\0';

					ascnum = 0;
					for(j=0; j<strlen(tmp_con); j++)
					{
						if(tmp_con[j] < 0xA1)
							ascnum ++;
						else
							j ++;  
					}
					if((ascnum % 2) != 0)
						tmp_con[9] = '\0';                                   
				}
				else
				{
					memcpy(tmp_con, tmp_node->Content.Content, tmp_node->Content.Length);
					tmp_con[tmp_node->Content.Length] = '\0';
					for(j=0; j<tmp_node->Content.Length; j++)
					if(tmp_con[j] == '\r')
						tmp_con[j] = '\0';
				}
				///标题
				outxy16(talk_info_button[i].xLeft+120, talk_info_button[i].yTop+5, 0,fontcolor, 1, 1, tmp_con, 0, 0);
				outxy16(talk_info_button[i].xLeft+240,talk_info_button[i].yTop+5,0,fontcolor,1,1,tmp_node->Content.Time,0,0);
				if(tmp_node->Content.isReaded)
					outxy16(talk_info_button[i].xLeft+420, talk_info_button[i].yTop+5, 1,fontcolor, 1, 1,"是", 0, 0);
				else
					outxy16(talk_info_button[i].xLeft+420, talk_info_button[i].yTop+5, 1,	fontcolor, 1, 1, "否", 0, 0);
			}
		}
		else
		{
			for(i=0;i<5;i++)
			{
				DisplayImage(&AreaMessageComKey[i],0);
			}
		}
		break;

	case 1:///未接电话
		for(i = 0; i < INFONUMPERPAGE; i++)
		{
			DisplayImageButton(&talk_info_button[i], IMAGEUP, 0);
		}

		if(Info[infotype].TotalNum > 0)
		{
			//总页数
			if((Info[infotype].TotalNum % INFONUMPERPAGE) == 0)
				Info[infotype].TotalInfoPage = Info[infotype].TotalNum /INFONUMPERPAGE;
			else
				Info[infotype].TotalInfoPage = Info[infotype].TotalNum /INFONUMPERPAGE + 1;

		//当前页
			Info[infotype].CurrentInfoPage = InfoStatus.CurrNo /INFONUMPERPAGE + 1;
			if(Info[infotype].CurrentInfoPage < Info[infotype].TotalInfoPage)
				PageTotalNum = INFONUMPERPAGE;
			else
				PageTotalNum = Info[infotype].TotalNum - (Info[infotype].CurrentInfoPage - 1)*INFONUMPERPAGE;

			//当前信息在本页中的位置
			NoInPage = (InfoStatus.CurrNo)%INFONUMPERPAGE;

			DisplayImageButton(&talk_info_button[NoInPage], IMAGEDOWN, 0);

			for(i=0;i<6;i++)
			{
				DisplayImage(&MisscallComKey[i],0);
			}
			
			for(i = 0; i < PageTotalNum; i++)
			{
				if(i !=  NoInPage)
					fontcolor = cBlue;
				else
					fontcolor = cWhite;

				tmp_node=locate_infonode(InfoNode_h[infotype], (Info[infotype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);

			//序号
				sprintf(tmp_con, "%02d\0", (Info[infotype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);
				outxy16(talk_info_button[i].xLeft+10, talk_info_button[i].yTop +5, 1,	fontcolor, 1, 1, tmp_con, 0, 0);


				//内容
				if(tmp_node->Content.Length >= tmp_len)
				{
					memcpy(tmp_con, tmp_node->Content.Content, tmp_len);
					tmp_con[tmp_len] = '\0';
					for(j=0; j<tmp_len; j++)
					if(tmp_con[j] == '\r')
						tmp_con[j] = '\0';
					if(tmp_con[8] < 0xA1)
						tmp_con[9] = '\0';

					ascnum = 0;
					for(j=0; j<strlen(tmp_con); j++)
					{
						if(tmp_con[j] < 0xA1)
							ascnum ++;
						else
							j ++;  
					}
					if((ascnum % 2) != 0)
						tmp_con[9] = '\0';                                   
				}
				else
				{
					memcpy(tmp_con, tmp_node->Content.Content, tmp_node->Content.Length);
					tmp_con[tmp_node->Content.Length] = '\0';
					for(j=0; j<tmp_node->Content.Length; j++)
					if(tmp_con[j] == '\r')
						tmp_con[j] = '\0';
				}
				///标题
				outxy16(talk_info_button[i].xLeft+120, talk_info_button[i].yTop+5, 0,fontcolor, 1, 1, tmp_con, 0, 0);
				outxy16(talk_info_button[i].xLeft+240,talk_info_button[i].yTop+5,0,fontcolor,1,1,tmp_node->Content.Time,0,0);
				if(tmp_node->Content.isReaded)
					outxy16(talk_info_button[i].xLeft+420, talk_info_button[i].yTop+5, 1,fontcolor, 1, 1,"是", 0, 0);
				else
					outxy16(talk_info_button[i].xLeft+420, talk_info_button[i].yTop+5, 1,	fontcolor, 1, 1, "否", 0, 0);
			}
		}
		else
		{
			for(i=0;i<5;i++)
			{
				DisplayImage(&MisscallComKey[i],0);
			}
		}
		break;

	case 2:///已接电话
		for(i = 0; i < INFONUMPERPAGE; i++)
		{
			DisplayImageButton(&talk_info_button[i], IMAGEUP, 0);
		}

		if(Info[infotype].TotalNum > 0)
		{
			//总页数
			if((Info[infotype].TotalNum % INFONUMPERPAGE) == 0)
				Info[infotype].TotalInfoPage = Info[infotype].TotalNum /INFONUMPERPAGE;
			else
				Info[infotype].TotalInfoPage = Info[infotype].TotalNum /INFONUMPERPAGE + 1;

		//当前页
			Info[infotype].CurrentInfoPage = InfoStatus.CurrNo /INFONUMPERPAGE + 1;
			if(Info[infotype].CurrentInfoPage < Info[infotype].TotalInfoPage)
				PageTotalNum = INFONUMPERPAGE;
			else
				PageTotalNum = Info[infotype].TotalNum - (Info[infotype].CurrentInfoPage - 1)*INFONUMPERPAGE;

			//当前信息在本页中的位置
			NoInPage = (InfoStatus.CurrNo)%INFONUMPERPAGE;

			DisplayImageButton(&talk_info_button[NoInPage], IMAGEDOWN, 0);

			for(i=0;i<6;i++)
			{
				DisplayImage(&CalledComKey[i],0);
			}
			
			for(i = 0; i < PageTotalNum; i++)
			{
				if(i !=  NoInPage)
					fontcolor = cBlue;
				else
					fontcolor = cWhite;

				tmp_node=locate_infonode(InfoNode_h[infotype], (Info[infotype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);

			//序号
				sprintf(tmp_con, "%02d\0", (Info[infotype].CurrentInfoPage-1)*INFONUMPERPAGE+i+1);
				outxy16(talk_info_button[i].xLeft+10, talk_info_button[i].yTop +5, 1,	fontcolor, 1, 1, tmp_con, 0, 0);


				//内容
				if(tmp_node->Content.Length >= tmp_len)
				{
					memcpy(tmp_con, tmp_node->Content.Content, tmp_len);
					tmp_con[tmp_len] = '\0';
					for(j=0; j<tmp_len; j++)
					if(tmp_con[j] == '\r')
						tmp_con[j] = '\0';
					if(tmp_con[8] < 0xA1)
						tmp_con[9] = '\0';

					ascnum = 0;
					for(j=0; j<strlen(tmp_con); j++)
					{
						if(tmp_con[j] < 0xA1)
							ascnum ++;
						else
							j ++;  
					}
					if((ascnum % 2) != 0)
						tmp_con[9] = '\0';                                   
				}
				else
				{
					memcpy(tmp_con, tmp_node->Content.Content, tmp_node->Content.Length);
					tmp_con[tmp_node->Content.Length] = '\0';
					for(j=0; j<tmp_node->Content.Length; j++)
					if(tmp_con[j] == '\r')
						tmp_con[j] = '\0';
				}
				///标题
				outxy16(talk_info_button[i].xLeft+120, talk_info_button[i].yTop+5, 0,fontcolor, 1, 1, tmp_con, 0, 0);
				outxy16(talk_info_button[i].xLeft+240,talk_info_button[i].yTop+5,0,fontcolor,1,1,tmp_node->Content.Time,0,0);
				if(tmp_node->Content.isReaded)
					outxy16(talk_info_button[i].xLeft+420, talk_info_button[i].yTop+5, 1,fontcolor, 1, 1,"是", 0, 0);
				else
					outxy16(talk_info_button[i].xLeft+420, talk_info_button[i].yTop+5, 1,	fontcolor, 1, 1, "否", 0, 0);
			}
		}
		else
		{
			for(i=0;i<5;i++)
			{
				DisplayImage(&CalledComKey[i],0);
			}
		}
		break;
		
	}
}
void CreateContentRow(char *Content, int nLength, char create_row[6][40])  //产生要显示的行内容
{
  int i,j;
  int row;
  int asciinum;
  row = 0;
  for(i=0; i<MAXROW; i++)
   for(j=0; j<(INFOROWLEN + 8); j++)
    create_row[i][j] = '\0';
  j = 0;
  asciinum = 0;
  for(i=0; i<nLength; i++)
   {
    if((Content[i] == '\r')&&(Content[i+1] == '\n'))
     {
      create_row[row][j] = '\0';
      row ++;
      asciinum = 0;
      j = 0;
      i++;
     }
    else
     {
      if((unsigned char)Content[i] >= 0xA1)
       {
        create_row[row][j] = Content[i];
        i ++;
        j ++;
        create_row[row][j] = Content[i];
        j ++;
       }
      else
       {
          create_row[row][j] = Content[i];
          j ++;
          asciinum ++;
       }
      if(j >= (INFOROWLEN-1))
       {
        if((asciinum % 2) != 0)
         {
          create_row[row][j] = ' ';
         }
        create_row[row][j+1] = '\0';
        row ++;
        asciinum = 0;
        j = 0;
       }
     }
    if(row >= MAXROW)
      break;
   }
}


void ShowInfoContent(int infotype, int infono)    // 信息内容
{
	int xLeft,yTop,yHeight;
	int i;
	InfoNode1 *tmp_node;
	char tmp_con[50];
	int tmp_len;  //预览长度
	int tmp_row;

	char content_row[12][40];


	tmp_len = 20;  

	xLeft = label_talkinfo.xLeft + 9;
	yTop = label_talkinfo.yTop + 89;
	yHeight = 38;
	RestorePicBuf_Func(label_talkinfo.xLeft,label_talkinfo.yTop, 583, 360,label_talkinfo.image, 0);
	if(infono <= Info[infotype].TotalNum - 1)
	{

		tmp_node=locate_infonode(InfoNode_h[infotype], infono + 1);
		//当前信息结点
		CurrInfo_Node = tmp_node;
		tmp_node->Content.isReaded = 1;


		sprintf(tmp_con, "第 %d 条" , infono + 1);
		outxy16(xLeft+10, yTop+0*yHeight, 1, cWhite, 1, 1, tmp_con, 0, 0);

		
		memcpy(tmp_con, tmp_node->Content.Time, tmp_len);
		tmp_con[tmp_len] = '\0';
		outxy16(xLeft +240, yTop+0*yHeight, 1, cWhite, 1, 1, tmp_con, 0, 0);

		outxy16(xLeft +430, yTop+0*yHeight, 1, cWhite, 1, 1, "是", 0, 0);
		
		tmp_len = 32;
		yTop = yTop+1*yHeight;
		CreateContentRow(tmp_node->Content.Content, tmp_node->Content.Length, content_row);

		for(i=0; i< 6 ; i++)
			outxy16(xLeft + 15, yTop+i*yHeight, 2, cWhite, 1, 1,content_row[i], 0, 0);

		pthread_mutex_lock(&Local.save_lock);
			for(i=0;i<SAVEMAX;i++)
			{
				if(Save_File_Buff[i].isValid == 0)
				{
					Save_File_Buff[i].Type = 1;
					Save_File_Buff[i].InfoType = InfoStatus.CurrType;
					Save_File_Buff[i].isValid = 1;
					break;
				}
			}
		pthread_mutex_unlock(&Local.save_lock);
		sem_post(&save_file_sem);
	}

}



void DisplayInfoContent(int infotype, int infono)    //信息显示方框（二级）  信息内容
{

  	char jpgfilename[80];
  	int i;
	
	Local.CurrentWindow = TalkInfoContentWindow;
	DisplayImage(&talk_bg[3],0);
	DisplayImage(&talk_banner[4],0);
	

	for(i=0;i<4;i++)
	{
		DisplayImage(&talk_infocontent_comkey[i],0);
	}

  
  	InfoStatus.CurrWin = 1;    //信息内容

	if(label_talkinfo.image == NULL)
	{
		label_talkinfo.width = 583;
		label_talkinfo.height = 360;
		label_talkinfo.xLeft = 169;
		label_talkinfo.yTop = 72;
		label_talkinfo.image = (unsigned char*)malloc(583*360*3/2);
		SavePicBuf_Func(label_talkinfo.xLeft,label_talkinfo.yTop,583,360,label_talkinfo.image,0);
	}
  	ShowInfoContent(infotype, infono);
}


void OperateInfoContent(int wintype,int currwindow)
{
	InfoNode1* tmp_node;
	int i;

	#if 0
	if((wintype>=0)&&(wintype<4))
	{
		DisplayImageButton(&talk_infocontent_comkey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&talk_infocontent_comkey[wintype],IMAGEUP,0);
	}
	else
	#endif
	if(wintype>50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 0:///上一条
		if(Info[InfoStatus.CurrType].CurrNo > 0)
		{
			Info[InfoStatus.CurrType].CurrNo --;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoContent(InfoStatus.CurrType,InfoStatus.CurrNo);
		}
	
		break;
	case 1:
		if(Info[InfoStatus.CurrType].CurrNo < Info[InfoStatus.CurrType].TotalNum-1)
		{
			Info[InfoStatus.CurrType].CurrNo++;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoContent(InfoStatus.CurrType,InfoStatus.CurrNo);
		}
		break;
	case 2:
		tmp_node = locate_infonode(InfoNode_h[InfoStatus.CurrType],InfoStatus.CurrNo+1);
		if(tmp_node != NULL)
		{
			delete_infonode(tmp_node);
			Info[InfoStatus.CurrType].TotalNum --;
			pthread_mutex_lock(&Local.save_lock);
			for(i=0;i<SAVEMAX;i++)
			{
				if(Save_File_Buff[i].isValid == 0)
				{
					Save_File_Buff[i].Type = 1;
					Save_File_Buff[i].InfoType = InfoStatus.CurrType;
					Save_File_Buff[i].isValid = 1;
					break;
				}
			}
			pthread_mutex_unlock(&Local.save_lock);
			sem_post(&save_file_sem);
			if(InfoStatus.CurrNo > Info[InfoStatus.CurrType].TotalNum-1)
				InfoStatus.CurrNo = Info[InfoStatus.CurrType].TotalNum;
			ShowInfoContent(InfoStatus.CurrType,InfoStatus.CurrNo);
		}
		break;
	case 3:
		InitParam = 1;
		if(InfoStatus.CurrType == 0)
			DisplayAreaMessageWindow();
		break;

	case 43:
		DisplayTalkWindow();
		break;
	case 45:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 46:
		DisplayMisscallWindow();
		break;
	case 47:
		//DisplayPhonebookWindow();
		break;
	case 48:
		//DisplayBlacklistWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
		
	}
}

void DisplayAreaMessageWindow(void)
{
	int i;
	Local.CurrentWindow = TalkAreaMessageWindow;
	DisplayImage(&talk_main_image,0);

	for(i=0;i<5;i++)
	{
		if(i== 1)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	
	DisplayImage(&talk_bg[2],0);
	DisplayImage(&talk_banner[4],0);
	///上一页，下一页
	for(i=0;i<5;i++)
	{
		DisplayImage(&AreaMessageComKey[i],0);
	}

#if 1
	for(i=0;i<5;i++)
	{
		if(i==0)
		{
			DisplayImageButton(&talk_message_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&talk_message_button[i],IMAGEUP,0);
		}
	}
#endif
	////显示信息列表
	if(InitParam == 0)
	{
		InfoStatus.CurrType = 0;
		InfoStatus.CurrWin = 0;
		InfoStatus.CurrNo = 0;
		Info[InfoStatus.CurrType].CurrentInfoPage = 1;
		Info[InfoStatus.CurrType].CurrNo = 0;
	}
	ShowInfoList(0);
	
}

void OperateAreaMessageWindow(int wintype,int currwindow)
{
	int i;
	int TmpInfoNo;

	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype >=0)&&(wintype<5))
	{
		DisplayImageButton(&AreaMessageComKey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&AreaMessageComKey[wintype],IMAGEUP,0);
	}
	else if(wintype >=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 0:///上一页
		if(Info[InfoStatus.CurrType].CurrentInfoPage > 1 )
		{
			Info[InfoStatus.CurrType].CurrentInfoPage--;
			Info[InfoStatus.CurrType].CurrNo = (Info[InfoStatus.CurrType].CurrentInfoPage -1)*INFONUMPERPAGE;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoList(InfoStatus.CurrType);
		}
		break;
	case 1:////下一页
		if(Info[InfoStatus.CurrType].CurrentInfoPage < Info[InfoStatus.CurrType].TotalInfoPage)
		{
			Info[InfoStatus.CurrType].CurrentInfoPage ++;
			Info[InfoStatus.CurrType].CurrNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoList(InfoStatus.CurrType);
		}
		break;
	case 2:////查看
		DisplayInfoContent(InfoStatus.CurrType,InfoStatus.CurrNo);
		break;
	case 3:///删除单条
		CurrInfo_Node = locate_infonode(InfoNode_h[InfoStatus.CurrType],InfoStatus.CurrNo+1);
		if(CurrInfo_Node != NULL)
		{
			delete_infonode(CurrInfo_Node);
			Info[InfoStatus.CurrType].TotalNum--;
			pthread_mutex_lock(&Local.save_lock);
			for(i=0;i<SAVEMAX;i++)
			{
				if(Save_File_Buff[i].isValid == 0)
				{
					Save_File_Buff[i].Type = 1;
					Save_File_Buff[i].InfoType = InfoStatus.CurrType;
					Save_File_Buff[i].isValid = 1;
					break;
				}
			}
			pthread_mutex_unlock(&Local.save_lock);
			sem_post(&save_file_sem);
			if(InfoStatus.CurrNo >= Info[InfoStatus.CurrType].TotalNum -1)
				InfoStatus.CurrNo = Info[InfoStatus.CurrType].TotalNum -1;
			ShowInfoList(InfoStatus.CurrType);
		}
		break;
	case 4:////全部删除
		delete_all_infonode(InfoNode_h[InfoStatus.CurrType]);
		Info[InfoStatus.CurrType].TotalNum = 0;
		Info[InfoStatus.CurrType].CurrNo = 0;
		pthread_mutex_lock(&Local.save_lock);
		for(i=0;i<SAVEMAX;i++)
		{
			if(Save_File_Buff[i].isValid == 0)
			{
				Save_File_Buff[i].Type = 1;
				Save_File_Buff[i].InfoType = InfoStatus.CurrType;
				Save_File_Buff[i].isValid = 1;
				break;
			}
		}
		pthread_mutex_unlock(&Local.save_lock);
		sem_post(&save_file_sem);
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		TmpInfoNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE+wintype-5;
		if(TmpInfoNo < Info[InfoStatus.CurrType].TotalNum)
		{
			Info[InfoStatus.CurrType].CurrNo = TmpInfoNo;
			InfoStatus.CurrNo = TmpInfoNo;
			ShowInfoList(InfoStatus.CurrType);
		}
		break;
	case 15:///AreaMessage
		break;
	case 16:
		DisplayLocalMessageWindow();
		break;
	case 17:
		DisplayPushMessageWindow();
		break;
	case 18:
		DisplayCustomMessageWindow();
		break;
	case 19:
		break;


	case 43:
		printf("test!\n");
		DisplayTalkWindow();
		break;
	case 44:
		//DisplayAreaMessageWindow();
		break;
	case 45:
		DisplayMisscallWindow();
		break;
	case 46:
		//DisplayPhonebookWindow();
		break;
	case 47:
		///DisplayBlacklistWindow();
		break;
	case 48:
		break;
	//// big menu
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
		
	}
		
}

void DisplayLocalMessageWindow(void)
{
	int i;
	Local.CurrentWindow = TalkLocalMessageWindow;

	for(i=0;i<5;i++)
	{
		if(i== 1)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	
	DisplayImage(&talk_bg[2],0);
	DisplayImage(&talk_banner[5],0);
	///上一页，下一页
	for(i=0;i<6;i++)
	{
		DisplayImage(&LocalMessageComKey[i],0);
	}

	for(i=0;i<5;i++)
	{
		if(i==1)
		{
			DisplayImageButton(&talk_message_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&talk_message_button[i],IMAGEUP,0);
		}
	}
}
void OperateLocalMessageWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype>=0)&&(wintype<6))
	{
		DisplayImageButton(&LocalMessageComKey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&LocalMessageComKey[wintype],IMAGEUP,0);
	}
	else if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		printf("press comkey!\n");
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		printf("select which message!\n");
		break;
	case 15:
		DisplayAreaMessageWindow();
		break;
	case 16:
		printf("displaylocalmessagewindow!\n");
		break;
	case 17:
		DisplayPushMessageWindow();
		break;
	case 18:
		DisplayCustomMessageWindow();
		break;
	case 19:
		printf("right button!\n");
		break;

	case 43:
		DisplayTalkWindow();
		break;
	case 44:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 45:
		DisplayMisscallWindow();
		break;
	case 46:
		//DisplayPhonebookWindow();
		break;
	case 47:
		//DisplayBlacklistWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
	}
	
}
void DisplayPushMessageWindow(void)
{
	int i;
	Local.CurrentWindow = TalkPushMessageWindow;
	
	for(i=0;i<5;i++)
	{
		if(i== 1)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	
	DisplayImage(&talk_bg[2],0);
	DisplayImage(&talk_banner[6],0);
	///上一页，下一页
	for(i=0;i<5;i++)
	{
		DisplayImage(&PushMessageComKey[i],0);
	}

	for(i=0;i<5;i++)
	{
		if(i==2)
		{
			DisplayImageButton(&talk_message_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&talk_message_button[i],IMAGEUP,0);
		}
	}
}
void OperatePushMessageWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype>=0)&&(wintype<5))
	{
		DisplayImageButton(&PushMessageComKey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&PushMessageComKey[wintype],IMAGEUP,0);
	}
	else if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	switch(wintype)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		printf("comkey!\n");
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		break;
		
	case 15:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 16:
		DisplayLocalMessageWindow();
		break;
	case 17:
		break;
	case 18:
		DisplayCustomMessageWindow();
		break;
	case 19:
		break;

	case 43:
		DisplayTalkWindow();
		break;
	case 44:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 45:
		DisplayMisscallWindow();
		break;
	case 46:
		//DisplayPhonebookWindow();
		break;
	case 47:
		//DisplayBlacklistWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
	}
}

void DisplayCustomMessageWindow(void)
{
	int i;
	Local.CurrentWindow = TalkCustomMessageWindow;
	for(i=0;i<5;i++)
	{
		if(i== 1)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	
	DisplayImage(&talk_bg[2],0);
	DisplayImage(&talk_banner[7],0);
	///上一页，下一页
	for(i=0;i<5;i++)
	{
		DisplayImage(&CustomMessageComKey[i],0);
	}

	for(i=0;i<5;i++)
	{
		if(i==3)
		{
			DisplayImageButton(&talk_message_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&talk_message_button[i],IMAGEUP,0);
		}
	}
}
void OperateCustomMessageWindow(int wintype,int currwindow)
{
	if((wintype>=0)&&(wintype<5))
	{
		DisplayImageButton(&CustomMessageComKey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&CustomMessageComKey[wintype],IMAGEUP,0);
	}
	else if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		break;
	case 15:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 16:
		DisplayLocalMessageWindow();
		break;
	case 17:
		DisplayPushMessageWindow();
		break;
	case 18:
	case 19:
		break;

	case 43:
		DisplayTalkWindow();
		break;
	case 44:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 45:
		DisplayMisscallWindow();
		break;
	case 46:
		//DisplayPhonebookWindow();
		break;
	case 47:
		//DisplayBlacklistWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
	}
}


void DisplayMisscallWindow(void)
{
	int i;

	Local.CurrentWindow = TalkMisscallWindow;
	for(i=0;i<5;i++)
	{
		if(i== 2)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	DisplayImage(&talk_bg[2],0);
	DisplayImage(&talk_banner[1],0);
	for(i=0;i<6;i++)
	{
		DisplayImage(&MisscallComKey[i],0);
	}

	for(i=0;i<4;i++)
	{
		if(i==0)
		{
			DisplayImageButton(&talk_callrec_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&talk_callrec_button[i],IMAGEUP,0);
		}
	}

//	if(InitParam == 0)
	{
		InfoStatus.CurrType = 1;
		InfoStatus.CurrWin = 0;
		InfoStatus.CurrNo = 0;
		Info[InfoStatus.CurrType].CurrentInfoPage = 1;
		Info[InfoStatus.CurrType].CurrNo = 0;
	}

	ShowInfoList(InfoStatus.CurrType);
	
}

void OperateMisscallWindow(int wintype,int currwindow)
{
	int i;
	int TmpInfoNo;
	
	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype>=0)&&(wintype<6))
	{
		DisplayImageButton(&MisscallComKey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&MisscallComKey[wintype],IMAGEUP,0);
	}
	else if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 0:///上一页
		if(Info[InfoStatus.CurrType].CurrentInfoPage>1)
		{
			Info[InfoStatus.CurrType].CurrentInfoPage--;
			Info[InfoStatus.CurrType].CurrNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoList(InfoStatus.CurrType);
			
		}
		break;
	case 1:///下一页
		if(Info[InfoStatus.CurrType].CurrentInfoPage < Info[InfoStatus.CurrType].TotalInfoPage)
		{
			Info[InfoStatus.CurrType].CurrentInfoPage++;
			Info[InfoStatus.CurrType].CurrNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoList(InfoStatus.CurrType);	
		}
		break;
	case 2:///回拨
		printf("回拨!\n");
		break;
	case 3:///黑名单
		printf("black list!\n");
		break;
	case 4:////删除
		CurrInfo_Node = locate_infonode(InfoNode_h[InfoStatus.CurrType],InfoStatus.CurrNo+1);
		if(CurrInfo_Node != NULL)
		{
			delete_infonode(CurrInfo_Node);
			Info[InfoStatus.CurrType].TotalNum--;
			pthread_mutex_lock(&Local.save_lock);
			for(i=0;i<SAVEMAX;i++)
			{
				if(Save_File_Buff[i].isValid == 0)
				{
					Save_File_Buff[i].Type = 1;
					Save_File_Buff[i].InfoType = InfoStatus.CurrType;
					Save_File_Buff[i].isValid = 1;
					break;
				}
			}
			pthread_mutex_unlock(&Local.save_lock);
			sem_post(&save_file_sem);
			if(InfoStatus.CurrNo >= Info[InfoStatus.CurrType].TotalNum -1)
				InfoStatus.CurrNo = Info[InfoStatus.CurrType].TotalNum -1;
			ShowInfoList(InfoStatus.CurrType);
		}
	case 5:////全部删除
		delete_all_infonode(InfoNode_h[InfoStatus.CurrType]);
		Info[InfoStatus.CurrType].TotalNum = 0;
		Info[InfoStatus.CurrType].CurrNo = 0;
		pthread_mutex_lock(&Local.save_lock);
			for(i=0;i<SAVEMAX;i++)
			{
				if(Save_File_Buff[i].isValid == 0)
				{
					Save_File_Buff[i].Type = 1;
					Save_File_Buff[i].InfoType = InfoStatus.CurrType;
					Save_File_Buff[i].isValid = 1;
					break;
				}
			}
			pthread_mutex_unlock(&Local.save_lock);
			sem_post(&save_file_sem);
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		TmpInfoNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE+wintype-6;
		if(TmpInfoNo < Info[InfoStatus.CurrType].TotalNum)
		{
			Info[InfoStatus.CurrType].CurrNo = TmpInfoNo;
			InfoStatus.CurrNo = TmpInfoNo;
			ShowInfoList(InfoStatus.CurrType);
		}
		break;
	case 15:
		break;
	case 16:
		DisplayCalledWindow();
		break;
	case 17:
		DisplayCallWindow();
		break;
	case 18:
		break;

	case 43:
		DisplayTalkWindow();
		break;
	case 44:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 45:
		DisplayMisscallWindow();
		break;
	case 46:
		//DisplayPhonebookWindow();
		break;
	case 47:
		//DisplayBlacklistWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
	}
}

void DisplayCalledWindow(void)
{
	int i;

	Local.CurrentWindow = TalkCalledWindow;
	for(i=0;i<5;i++)
	{
		if(i== 2)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	DisplayImage(&talk_bg[2],0);
	DisplayImage(&talk_banner[1],0);
	for(i=0;i<6;i++)
	{
		DisplayImage(&CalledComKey[i],0);
	}

	for(i=0;i<4;i++)
	{
		if(i==1)
		{
			DisplayImageButton(&talk_callrec_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&talk_callrec_button[i],IMAGEUP,0);
		}
	}

	{
		InfoStatus.CurrType = 2;
		InfoStatus.CurrWin = 0;
		InfoStatus.CurrNo = 0;
		Info[InfoStatus.CurrType].CurrentInfoPage = 1;
		Info[InfoStatus.CurrType].CurrNo = 0;
	}

	ShowInfoList(InfoStatus.CurrType);
}

void OperateCalledWindow(int wintype,int currwindow)
{
	int i;
	int TmpInfoNo;
	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype>=0)&&(wintype<6))
	{
		DisplayImageButton(&CalledComKey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&CalledComKey[wintype],IMAGEUP,0);
	}
	else if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	switch(wintype)
	{
	case 0:///上一页
		if(Info[InfoStatus.CurrType].CurrentInfoPage>1)
		{
			Info[InfoStatus.CurrType].CurrentInfoPage--;
			Info[InfoStatus.CurrType].CurrNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoList(InfoStatus.CurrType);
			
		}
		break;
	case 1:///下一页
		if(Info[InfoStatus.CurrType].CurrentInfoPage < Info[InfoStatus.CurrType].TotalInfoPage)
		{
			Info[InfoStatus.CurrType].CurrentInfoPage++;
			Info[InfoStatus.CurrType].CurrNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE;
			InfoStatus.CurrNo = Info[InfoStatus.CurrType].CurrNo;
			ShowInfoList(InfoStatus.CurrType);	
		}
		break;
	case 2:///回拨
		printf("回拨!\n");
		break;
	case 3:///黑名单
		printf("black list!\n");
		break;
	case 4:////删除
		CurrInfo_Node = locate_infonode(InfoNode_h[InfoStatus.CurrType],InfoStatus.CurrNo+1);
		if(CurrInfo_Node != NULL)
		{
			delete_infonode(CurrInfo_Node);
			Info[InfoStatus.CurrType].TotalNum--;
			pthread_mutex_lock(&Local.save_lock);
			for(i=0;i<SAVEMAX;i++)
			{
				if(Save_File_Buff[i].isValid == 0)
				{
					Save_File_Buff[i].Type = 1;
					Save_File_Buff[i].InfoType = InfoStatus.CurrType;
					Save_File_Buff[i].isValid = 1;
					break;
				}
			}
			pthread_mutex_unlock(&Local.save_lock);
			sem_post(&save_file_sem);
			if(InfoStatus.CurrNo >= Info[InfoStatus.CurrType].TotalNum -1)
				InfoStatus.CurrNo = Info[InfoStatus.CurrType].TotalNum -1;
			ShowInfoList(InfoStatus.CurrType);
		}
	case 5:////全部删除
		delete_all_infonode(InfoNode_h[InfoStatus.CurrType]);
		Info[InfoStatus.CurrType].TotalNum = 0;
		Info[InfoStatus.CurrType].CurrNo = 0;
		pthread_mutex_lock(&Local.save_lock);
			for(i=0;i<SAVEMAX;i++)
			{
				if(Save_File_Buff[i].isValid == 0)
				{
					Save_File_Buff[i].Type = 1;
					Save_File_Buff[i].InfoType = InfoStatus.CurrType;
					Save_File_Buff[i].isValid = 1;
					break;
				}
			}
			pthread_mutex_unlock(&Local.save_lock);
			sem_post(&save_file_sem);
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		TmpInfoNo = (Info[InfoStatus.CurrType].CurrentInfoPage-1)*INFONUMPERPAGE+wintype-6;
		if(TmpInfoNo < Info[InfoStatus.CurrType].TotalNum)
		{
			Info[InfoStatus.CurrType].CurrNo = TmpInfoNo;
			InfoStatus.CurrNo = TmpInfoNo;
			ShowInfoList(InfoStatus.CurrType);
		}
		break;
	case 15:
		DisplayMisscallWindow();
		break;
	case 16:
		break;
	case 17:
		DisplayCallWindow();
		break;
	case 18:
		break;

	case 43:
		DisplayTalkWindow();
		break;
	case 44:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 45:
		DisplayMisscallWindow();
		break;
	case 46:
		//DisplayPhonebookWindow();
		break;
	case 47:
		//DisplayBlacklistWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
	}
}


void DisplayCallWindow(void)
{
	int i;

	Local.CurrentWindow = TalkCallWindow;

	for(i=0;i<5;i++)
	{
		if(i== 2)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	DisplayImage(&talk_bg[2],0);
	DisplayImage(&talk_banner[1],0);
	for(i=0;i<6;i++)
	{
		DisplayImage(&CallComKey[i],0);
	}

	for(i=0;i<4;i++)
	{
		if(i==2)
		{
			DisplayImageButton(&talk_callrec_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&talk_callrec_button[i],IMAGEUP,0);
		}
	}
}


void OperateCallWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype>=0)&&(wintype<6))
	{
		DisplayImageButton(&CallComKey[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&CallComKey[wintype],IMAGEUP,0);
	}
	
	else if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	switch(wintype)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		break;
	case 15:
		DisplayMisscallWindow();
		break;
	case 16:
		DisplayCalledWindow();
		break;
	case 17:
		break;
	case 18:
		break;

	case 43:
		DisplayTalkWindow();
		break;
	case 44:
		InitParam = 0;
		DisplayAreaMessageWindow();
		break;
	case 45:
		DisplayMisscallWindow();
		break;
	case 46:
		//DisplayPhonebookWindow();
		break;
	case 47:
		//DisplayBlacklistWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-50);
		break;
	}
}

void DisplayPhonebookWindow(void)
{
	
}
void OperatePhonebookWindow(void)
{
	
}

void DisplayBlacklistWindow(void)
{
}
void OperateBlacklistWindow(void)
{
}
//---------------------------------------------------------------------------
void DisplayTalkWindow(void)    //显示对讲窗口（一级）
{
	int i=0;
	Local.CurrentWindow = TalkMenuWindow;

	DisplayImage(&talk_main_image,0);
	printf("talk window!\n");

	talkaddr_edit.xLeft = 220;
	talkaddr_edit.yTop = 190;
	talkaddr_edit.BoxLen = 0;
	talkaddr_edit.Text[0] = '\0';
	CurrBox = 0;

	WriteCursorTalk(&talkaddr_edit,1,1,0);
	for(i=0;i<5;i++)
	{
		if(i== 0)
			DisplayImageButton(&talk_menu_button[i],IMAGEDOWN,0);
		else
			DisplayImageButton(&talk_menu_button[i],IMAGEUP,0);
	}
	
	DisplayEdit(&talkaddr_edit,0);
	WriteCursorTalk(&talkaddr_edit, 1, 1, 0);
}
//---------------------------------------------------------------------------
void OperateTalkWindow(short wintype, int currwindow)    //对讲窗口操作（一级）
{
  char wavFile[80];
  char tmpstring1[5],tmpstring2[3],tmpstring3[5];
  char tmpstring4[5],tmpstring5[3],tmpstring6[5];
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char str[3];
  xLeft = 120;
  yTop = 36;

	if(Local.CurrentWindow != currwindow)
		return;

	if(wintype < 12)
	{
		DisplayImageButton(&talk_keynum_button[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&talk_keynum_button[wintype],IMAGEUP,0);
	}
	else if((wintype >= 12)&&(wintype <= 14))
   	{
		DisplayImageButton(&talk_hotkey_button[wintype-12], IMAGEDOWN, 0);
    	usleep(DELAYTIME*1000);
    	DisplayImageButton(&talk_hotkey_button[wintype-12], IMAGEUP, 0);
   	}
	else if((wintype >=15)&&(wintype <= 19))
	{
		DisplayImageButton(&talk_menu_button[wintype-15], IMAGEDOWN, 0);
    	usleep(DELAYTIME*1000);
    	DisplayImageButton(&talk_menu_button[wintype-15], IMAGEUP, 0);
	}
	else if(wintype >= 30)
	{
		DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
	}

  	//停止光标,并清一下
  	WriteCursorTalk(&talkaddr_edit, 0, 0, 0);
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
		 if(talkaddr_edit.BoxLen < talkaddr_edit.MaxLen)
		 {
		 	str[0] = '0' + wintype;
			str[1] = '\0' ;
			strcat(talkaddr_edit.Text,str);
		 	if(talkaddr_edit.BoxLen < 4)
		 	{
				snprintf(tmpstring4,sizeof(tmpstring4),talkaddr_edit.Text);
				outxy16(230,203,1,cBlack,1,1,tmpstring4,0,0);
				//outxy16(230,203,1,cBlack,1,1,talkaddr_edit.Text,0,0);
				//outxy16(230+talkaddr_edit.BoxLen*talkaddr_edit.fWidth,203,1,cBlack,1,1,str,0,0);
		 	}
			else if((talkaddr_edit.BoxLen >=4)&&(talkaddr_edit.BoxLen <6))
			{
				snprintf(tmpstring4,sizeof(tmpstring4),talkaddr_edit.Text);
				snprintf(tmpstring5,sizeof(tmpstring5),talkaddr_edit.Text+4);
				outxy16(230,203,1,cBlack,1,1,tmpstring4,0,0);
				outxy16(304,203,1,cBlack,1,1,tmpstring5,0,0);
				//outxy16(230,203,1,cBlack,1,1,talkaddr_edit.Text,0,0);
				//outxy16(304+(talkaddr_edit.BoxLen-4)*talkaddr_edit.fWidth,203,1,cBlack,1,1,str,0,0);
				//outxy16(304+(talkaddr_edit.BoxLen-4)*talkaddr_edit.fWidth,203,1,cBlack,1,1,str,0,0);
			}
			else if(talkaddr_edit.BoxLen >=6)
			{
				snprintf(tmpstring4,sizeof(tmpstring4),talkaddr_edit.Text);
				snprintf(tmpstring5,sizeof(tmpstring5),talkaddr_edit.Text+4);
				snprintf(tmpstring6,sizeof(tmpstring6),talkaddr_edit.Text+6);
				outxy16(230,203,1,cBlack,1,1,tmpstring4,0,0);
				outxy16(304,203,1,cBlack,1,1,tmpstring5,0,0);
				outxy16(389,203,1,cBlack,1,1,tmpstring6,0,0);
				//outxy16(389+(talkaddr_edit.BoxLen-6)*talkaddr_edit.fWidth,203,1,cBlack,1,1,str,0,0);
			}
			talkaddr_edit.BoxLen ++;
		 }
         break;
    case 11: //clear
          	DisplayEdit(&talkaddr_edit,0);
			if(talkaddr_edit.BoxLen > 0)
			{
				talkaddr_edit.BoxLen --;
				talkaddr_edit.Text[talkaddr_edit.BoxLen] = '\0';
				if(talkaddr_edit.BoxLen > 6 )
				{
					snprintf(tmpstring1,sizeof(tmpstring1),talkaddr_edit.Text);
					snprintf(tmpstring2,sizeof(tmpstring2),talkaddr_edit.Text+4);
					snprintf(tmpstring3,sizeof(tmpstring3),talkaddr_edit.Text+6);
					outxy16(230,203,1,cBlack,1,1,tmpstring1,0,0);
					outxy16(304,203,1,cBlack,1,1,tmpstring2,0,0);
					outxy16(389,203,1,cBlack,1,1,tmpstring3,0,0);
				}
				else if(talkaddr_edit.BoxLen <4)
				{
					snprintf(tmpstring1,sizeof(tmpstring1),talkaddr_edit.Text);
					outxy16(230,203,1,cBlack,1,1,tmpstring1,0,0);
				}
				else
				{
					snprintf(tmpstring1,sizeof(tmpstring1),talkaddr_edit.Text);
					snprintf(tmpstring2,sizeof(tmpstring2),talkaddr_edit.Text+4);
					outxy16(230,203,1,cBlack,1,1,tmpstring1,0,0);
					outxy16(304,203,1,cBlack,1,1,tmpstring2,0,0);
				}
					
			}
			if(Local.Status != 0)
			{
				TalkEnd_Func();
			}
			break;
   	 case 10: // call 
			if((talkaddr_edit.BoxLen == 4)||(talkaddr_edit.BoxLen == 10))
			{
				DisplayTalkPicWindow(0);
				CallR2R_Func();
			}
			else
			{
				sprintf(wavFile, "%sinputerror.wav\0", wavPath);
				StartPlayWav(wavFile, 0);
			}
			break;
    	case 12: //中心
			if(Local.Status == 0)
			{
				DisplayTalkPicWindow(0);
				CallCenter_Func();
			}
           break;
    	case 13:	///authentication
			//DisplayAuthenticationWindow();
           	break;
   		case 14: ///phone book
           	//DisplayPhonebookWindow();
           	break;

		case 15:
			DisplayTalkWindow();
			break;
		case 16:
			InitParam = 0;
			DisplayAreaMessageWindow();
			//DisplayLocalMessageWindow();
			//DisplayPushMessageWindow();
			//DisplayCustomMessageWindow();
			
			break;
		case 17:
			//DisplayMisscallWindow();
			//DisplayCalledWindow();
			DisplayCallWindow();
			break;
		case 18:
		case 19:
		case 20:
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
  	if(Local.CurrentWindow == 3)
    	WriteCursorTalk(&talkaddr_edit, 1, 1, 0);
}
//---------------------------------------------------------------------------
void CallCenter_Func(void)  //呼叫中心
{
	int i;

	if(Local.Status == 0)
	{
		pthread_mutex_lock (&Local.udp_lock);
		//查找可用发送缓冲并填空
		for(i=0; i<UDPSENDMAX; i++)
		if(Multi_Udp_Buff[i].isValid == 0)
		{
			Multi_Udp_Buff[i].SendNum = 0;
			Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
			Multi_Udp_Buff[i].CurrOrder = VIDEOTALK;
			//  sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",LocalCfg.IP_Broadcast[0],
			//          LocalCfg.IP_Broadcast[1],LocalCfg.IP_Broadcast[2],LocalCfg.IP_Broadcast[3]);
			strcpy(Multi_Udp_Buff[i].RemoteHost, NSMULTIADDR);
			//头部
			memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
			//命令  ,子网广播解析
			Multi_Udp_Buff[i].buf[6] = NSORDER;
			Multi_Udp_Buff[i].buf[7] = ASK;    //主叫

			memcpy(Multi_Udp_Buff[i].buf+8,LocalCfg.Addr,20);
			memcpy(Multi_Udp_Buff[i].buf+28,LocalCfg.IP,4);
			memcpy(Remote.Addr[0], NullAddr, 20);
			memcpy(Remote.Addr[0],"Z00010000000",12);
			printf("%s\n",Remote.Addr);
			memcpy(Multi_Udp_Buff[i].buf+32,Remote.Addr[0],20);
			Remote.IP[0][0] = 0;
			Remote.IP[0][1] = 0;
			Remote.IP[0][2] = 0;
			Remote.IP[0][3] = 0;
			memcpy(Multi_Udp_Buff[i].buf+52,Remote.IP[0],4);

			Multi_Udp_Buff[i].nlength = 56;
			Multi_Udp_Buff[i].DelayTime = 100;
			Multi_Udp_Buff[i].isValid = 1;
#ifdef _DEBUG
			printf("正在解析地址\n");
#endif
		//	strcpy(Label_CCenter.Text, "正在查找地址...");
		//	ShowLabel(&Label_CCenter, NOREFRESH, cBlack);
			sem_post(&multi_send_sem);
			break;
		}
		//打开互斥锁
		pthread_mutex_unlock (&Local.udp_lock);
	}
	else
	{
#ifdef _DEBUG
		printf("本机正忙,无法呼叫\n");
#endif
	}
}
//---------------------------------------------------------------------------
void CallR2R_Func(void)    //户户对讲
{
	int i;
	if(Local.Status == 0)
	{
		//锁定互斥锁
		pthread_mutex_lock (&Local.udp_lock);
		//查找可用发送缓冲并填空
		for(i=0; i<UDPSENDMAX; i++)
		if(Multi_Udp_Buff[i].isValid == 0)
		{
			Multi_Udp_Buff[i].SendNum = 0;
			Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
			Multi_Udp_Buff[i].CurrOrder = VIDEOTALK;
			strcpy(Multi_Udp_Buff[i].RemoteHost, NSMULTIADDR);
			//头部
			memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
			//命令  ,子网广播解析
			Multi_Udp_Buff[i].buf[6] = NSORDER;
			Multi_Udp_Buff[i].buf[7] = ASK;    //主叫

			memcpy(Multi_Udp_Buff[i].buf+8,LocalCfg.Addr,20);
			memcpy(Multi_Udp_Buff[i].buf+28,LocalCfg.IP,4);
			memcpy(Remote.Addr[0], NullAddr, 20);
			if(talkaddr_edit.BoxLen == 4)
			{ //为别墅机
				Remote.Addr[0][0] = 'B';
				memcpy(Remote.Addr[0] + 1,talkaddr_edit.Text, 4);
				Remote.Addr[0][5] = '0';
			}
			else
			{ 	//为普通室内机
				Remote.Addr[0][0] = 'S';
				memcpy(Remote.Addr[0] + 1,talkaddr_edit.Text, 10);
				Remote.Addr[0][11] = '0';
			}

			memcpy(Multi_Udp_Buff[i].buf+32,Remote.Addr[0],20);
			Remote.IP[0][0] = 0;
			Remote.IP[0][1] = 0;
			Remote.IP[0][2] = 0;
			Remote.IP[0][3] = 0;
			memcpy(Multi_Udp_Buff[i].buf+52,Remote.IP[0],4);

			Multi_Udp_Buff[i].nlength = 56;
			Multi_Udp_Buff[i].DelayTime = 100;
			Multi_Udp_Buff[i].isValid = 1;
#ifdef _DEBUG
			printf("正在解析地址\n");
#endif
			ShowStatusText(CALLX, CALLY , 3, cBlack, 1, 1, "正在查找地址...", 0);
			sem_post(&multi_send_sem);
			break;
		}
		//打开互斥锁
		pthread_mutex_unlock (&Local.udp_lock);
	}
	else
	{
#ifdef _DEBUG
		printf("本机正忙,无法呼叫\n");
#endif
	}
}

//---------------------------------------------------------------------------
void DisplayTalkPicWindow(int type)    //显示对讲图像操作窗口（二级）,有呼叫时自动显示
{
	char jpgfilename[80];
	char str[3];

	PicStatBuf.Flag = 0;
	PicStatBuf.Time = 0;

	if(Local.CurrFbPage != 0)
	{
		Local.CurrFbPage = 0;
		fb_setpage(fbdev, Local.CurrFbPage);
	}  

	Local.CurrentWindow = TalkPicWindow;

	if(Local.LcdLightFlag == 0)
	{
		//点亮LCD背光
		OpenLCD();
		Local.LcdLightFlag = 1; //LCD背光标志
		Local.LcdLightTime = 0; //时间
	}  

	Local.ShowRecPic = 0;
	sprintf(jpgfilename,"%stalkpic.jpg\0",sPath);
	DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);
	if(type == 0)
	{
		Local.ShowRecPic = 1;
		DisplayImageButton(&talkpic_button[4],IMAGEUP,0);
	}
	if(label_call.image == NULL)
	{
		label_call.width = TIPW;
		label_call.height = TIPH;
		label_call.xLeft = CALLX;
		label_call.yTop = CALLY;
		label_call.image = (unsigned char*)malloc(TIPW*TIPH*3/2);
		SavePicBuf_Func(CALLX,CALLY,TIPW,TIPH,label_call.image,0);
	}
	
	//显示视频窗口
	//ShowVideoWindow();
	

	//提示条
	/*
	if(Label_Call.image == NULL)
	{
		Label_Call.width = TIPW;
		Label_Call.height = TIPH;
		Label_Call.xLeft = CALLX;
		Label_Call.yTop = CALLY;
		Label_Call.image = (unsigned char *)malloc(TIPW*TIPH*3/2);  //保存一块屏幕缓冲
		SavePicBuf_Func(CALLX, CALLY, TIPW, TIPH, Label_Call.image, 0);
	}
	*/
}
//---------------------------------------------------------------------------
void OperateTalkPicWindow(short wintype, int currwindow)    //对讲图像操作窗口（二级）
{
	int xLeft,yTop;
	int i, j;
	char jpgfilename[80];
	char str[3];
	xLeft = 120;
	yTop = 36;

	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype < 5)
	{
		DisplayImageButton(&talkpic_button[wintype], IMAGEDOWN, 0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&talkpic_button[wintype], IMAGEUP, 0);
	}

	if(Local.Status == 0)
	{
		PicStatBuf.Flag = 0;
		PicStatBuf.Time = 0;
	}
	
	switch(wintype)
	{
	case 0:
		if(Local.ShowRecPic == 0)
		{
			if(Local.Status == 2)
			{
				Talk_Func();
			}
			Local.ShowRecPic = 1;
			DisplayImageButton(&talkpic_button[4],IMAGEUP,0);
		}
		//else if (Local.ShowRecPic == 0)
		//{
		//	printf("留影!\n");
		//	//DisplayImageButton
		//}
		break;
	case 3:  //挂断
		switch(Local.Status)
        		{
		        case 1: //主叫对讲
		        case 2: //被叫对讲
		        case 5: //主叫通话
		        case 6: //被叫通话
		        	TalkEnd_Func();
		            break;
				default:
					break;
        		}
       		 break;
			
    case 2:  //放大
    case 11: //放大, 在被叫通话时
    	if((Local.PlayPicSize != 2)&&(Local.Status == 6))
        	ZoomOut_Func(); //视频放大
        break;
    case 1:  //开锁	   
		if(Local.Status == 6)  //状态为被对讲 
		{
			OpenLock_Func();	
		}
		break;
   case 4:
   		if(Local.ShowRecPic == 1)
   		{
			printf("留影操作!\n");
   		}
   	break;
   }
}
//---------------------------------------------------------------------------
void Talk_Func(void)       //通话 接听
{
	int i;
	if(Local.Status == 2)  //状态为被对讲
	{
		StopPlayWavFile();  //关闭铃音
		usleep(200*1000);  //延时是为了等待声音播放完成，否则会有杂音
		for(i=0; i<UDPSENDMAX; i++)
		if(Multi_Udp_Buff[i].isValid == 0)
		{
			//锁定互斥锁
			pthread_mutex_lock (&Local.udp_lock);
			Multi_Udp_Buff[i].SendNum = 0;
			Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
			sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",Remote.DenIP[0],
			Remote.DenIP[1],Remote.DenIP[2],Remote.DenIP[3]);
			//头部
			memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
			//命令  ,子网广播解析
			if(Remote.isDirect == 1)
			Multi_Udp_Buff[i].buf[6] = VIDEOTALKTRANS;
			else
			Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
			Multi_Udp_Buff[i].CurrOrder = Multi_Udp_Buff[i].buf[6];
			Multi_Udp_Buff[i].buf[7] = ASK;    //主叫
			Multi_Udp_Buff[i].buf[8] = CALLSTART;

			//本机为主叫方
			if((Local.Status == 1)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
			{
				memcpy(Multi_Udp_Buff[i].buf+9,LocalCfg.Addr,20);
				memcpy(Multi_Udp_Buff[i].buf+29,LocalCfg.IP,4);
				memcpy(Multi_Udp_Buff[i].buf+33,Remote.Addr[0],20);
				memcpy(Multi_Udp_Buff[i].buf+53,Remote.IP[0],4);
			}
			//本机为被叫方
			if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
			{
				memcpy(Multi_Udp_Buff[i].buf+9,Remote.Addr[0],20);
				memcpy(Multi_Udp_Buff[i].buf+29,Remote.IP[0],4);
				memcpy(Multi_Udp_Buff[i].buf+33,LocalCfg.Addr,20);
				memcpy(Multi_Udp_Buff[i].buf+53,LocalCfg.IP,4);
			}


			Multi_Udp_Buff[i].nlength = 57;
			Multi_Udp_Buff[i].DelayTime = 100;
			Multi_Udp_Buff[i].isValid = 1;

			//打开互斥锁
			pthread_mutex_unlock (&Local.udp_lock);
			sem_post(&multi_send_sem);
			break;
		}
	}
}
//---------------------------------------------------------------------------
/*
void ShowVideoWindow(void)    //关闭视频窗口
{
  char jpgfilename[80];

  PicStatBuf.Flag = 0;
  PicStatBuf.Time = 0;

  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"videoscreen.jpg");
  DisplayJPG(129, 125, jpgfilename, 1, 352, 240, 0);
  
  //呼叫中心提示条
  if(Label_CCenter.image == NULL)
   {
    Label_CCenter.width = TIPW;
    Label_CCenter.height = TIPH;
    Label_CCenter.xLeft = CALLCENTERX;
    Label_CCenter.yTop = CALLCENTERY;
    Label_CCenter.image = (unsigned char *)malloc(TIPW*TIPH*3/2);  //保存一块屏幕缓冲
    SavePicBuf_Func(CALLCENTERX, CALLCENTERY, TIPW, TIPH, Label_CCenter.image, 0);
   }
}
*/
//---------------------------------------------------------------------------
void TalkEnd_Func(void)
{
	int i;
	if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6)
		||(Local.Status == 7)||(Local.Status == 8)||(Local.Status == 9)||(Local.Status == 10))  //状态为对讲
	{
		//锁定互斥锁
		pthread_mutex_lock (&Local.udp_lock);
		for(i=0; i<UDPSENDMAX; i++)
		if(Multi_Udp_Buff[i].isValid == 0)
		{
			Multi_Udp_Buff[i].SendNum = 0;
			Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
			sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",Remote.DenIP[0],
			Remote.DenIP[1],Remote.DenIP[2],Remote.DenIP[3]);
			//头部
			memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
			//命令  ,子网广播解析
			if(Remote.isDirect == 1)
			Multi_Udp_Buff[i].buf[6] = VIDEOTALKTRANS;
			else
			Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
			Multi_Udp_Buff[i].CurrOrder = Multi_Udp_Buff[i].buf[6];
			Multi_Udp_Buff[i].buf[7] = ASK;    //主叫
			Multi_Udp_Buff[i].buf[8] = CALLEND;    //CALLEND
			//本机为主叫方
			if((Local.Status == 1)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
			{
				memcpy(Multi_Udp_Buff[i].buf+9,LocalCfg.Addr,20);
				memcpy(Multi_Udp_Buff[i].buf+29,LocalCfg.IP,4);
				memcpy(Multi_Udp_Buff[i].buf+33,Remote.Addr[0],20);
				memcpy(Multi_Udp_Buff[i].buf+53,Remote.IP[0],4);
			}
			//本机为被叫方
			if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
			{
				memcpy(Multi_Udp_Buff[i].buf+9,Remote.Addr[0],20);
				memcpy(Multi_Udp_Buff[i].buf+29,Remote.IP[0],4);
				memcpy(Multi_Udp_Buff[i].buf+33,LocalCfg.Addr,20);
				memcpy(Multi_Udp_Buff[i].buf+53,LocalCfg.IP,4);
			}

			Multi_Udp_Buff[i].nlength = 57;
			Multi_Udp_Buff[i].DelayTime = 100;
			Multi_Udp_Buff[i].isValid = 1;
			sem_post(&multi_send_sem);
			break;
		}
		//打开互斥锁
		pthread_mutex_unlock (&Local.udp_lock);
	}
}

//---------------------------------------------------------------------------
//呼叫超时
void CallTimeOut_Func(void)
{
	///post signal to write new message
	TalkEnd_Func();
	Local.OnlineFlag = 0;
}
//---------------------------------------------------------------------------
void ZoomOut_Func(void) //视频放大
{
	int i;
	if(Local.Status != 0)   //状态为忙
	for(i=0; i<UDPSENDMAX; i++)
	if(Multi_Udp_Buff[i].isValid == 0)
	{
		//锁定互斥锁
		pthread_mutex_lock (&Local.udp_lock);
		Multi_Udp_Buff[i].SendNum = 0;
		Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
		sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",Remote.IP[0][0],
		Remote.IP[0][1],Remote.IP[0][2],Remote.IP[0][3]);
		//头部
		memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
		//命令  ,子网广播解析
		if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6))
		{
			if(Remote.isDirect == 1)
				Multi_Udp_Buff[i].buf[6] = VIDEOTALKTRANS;
			else
				Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
		}
		if((Local.Status == 3)||(Local.Status == 4))
		{
			if(Remote.isDirect == 1)
				Multi_Udp_Buff[i].buf[6] = VIDEOWATCHTRANS;
			else
				Multi_Udp_Buff[i].buf[6] = VIDEOWATCH;
		}
		Multi_Udp_Buff[i].CurrOrder = Multi_Udp_Buff[i].buf[6];
		Multi_Udp_Buff[i].buf[7] = ASK;    //主叫
		Multi_Udp_Buff[i].buf[8] = ZOOMOUT;

		//本机为主叫方
		if((Local.Status == 1)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
		{
			memcpy(Multi_Udp_Buff[i].buf+9, LocalCfg.Addr, 20);
			memcpy(Multi_Udp_Buff[i].buf+29, LocalCfg.IP, 4);
			memcpy(Multi_Udp_Buff[i].buf+33, Remote.Addr[0], 20);
			memcpy(Multi_Udp_Buff[i].buf+53, Remote.IP[0], 4);
		}
		//本机为被叫方
		if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
		{
			memcpy(Multi_Udp_Buff[i].buf+9, Remote.Addr[0], 20);
			memcpy(Multi_Udp_Buff[i].buf+29, Remote.IP[0], 4);
			memcpy(Multi_Udp_Buff[i].buf+33, LocalCfg.Addr, 20);
			memcpy(Multi_Udp_Buff[i].buf+53, LocalCfg.IP, 4);
		}

		Multi_Udp_Buff[i].nlength = 57;
		Multi_Udp_Buff[i].DelayTime = 100;
		Multi_Udp_Buff[i].isValid = 1;

		//打开互斥锁
		pthread_mutex_unlock (&Local.udp_lock);
		sem_post(&multi_send_sem);
		break;
	}
}
//---------------------------------------------------------------------------
void ZoomIn_Func(void)  //视频缩小
{
	int i;
	if(Local.Status != 0)   //状态为忙
	for(i=0; i<UDPSENDMAX; i++)
	if(Multi_Udp_Buff[i].isValid == 0)
	{
		//锁定互斥锁
		pthread_mutex_lock (&Local.udp_lock);
		Multi_Udp_Buff[i].SendNum = 0;
		Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
		sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",Remote.IP[0][0],
		Remote.IP[0][1],Remote.IP[0][2],Remote.IP[0][3]);
		//头部
		memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
		//命令  ,子网广播解析
		if((Local.Status == 1)||(Local.Status == 2)||(Local.Status == 5)||(Local.Status == 6))
		{
			if(Remote.isDirect == 1)
				Multi_Udp_Buff[i].buf[6] = VIDEOTALKTRANS;
			else
				Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
		}
		if((Local.Status == 3)||(Local.Status == 4))
		{
			if(Remote.isDirect == 1)
				Multi_Udp_Buff[i].buf[6] =VIDEOWATCHTRANS;
			else
				Multi_Udp_Buff[i].buf[6] = VIDEOWATCH;
		}
		Multi_Udp_Buff[i].CurrOrder = Multi_Udp_Buff[i].buf[6];
		Multi_Udp_Buff[i].buf[7] = ASK;    //主叫
		Multi_Udp_Buff[i].buf[8] = ZOOMIN;

		//本机为主叫方
		if((Local.Status == 1)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
		{
			memcpy(Multi_Udp_Buff[i].buf+9, LocalCfg.Addr, 20);
			memcpy(Multi_Udp_Buff[i].buf+29, LocalCfg.IP, 4);
			memcpy(Multi_Udp_Buff[i].buf+33, Remote.Addr[0], 20);
			memcpy(Multi_Udp_Buff[i].buf+53, Remote.IP[0], 4);
		}
		//本机为被叫方
		if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
		{
			memcpy(Multi_Udp_Buff[i].buf+9, Remote.Addr[0], 20);
			memcpy(Multi_Udp_Buff[i].buf+29, Remote.IP[0], 4);
			memcpy(Multi_Udp_Buff[i].buf+33, LocalCfg.Addr, 20);
			memcpy(Multi_Udp_Buff[i].buf+53, LocalCfg.IP, 4);
		}

		Multi_Udp_Buff[i].nlength = 57;
		Multi_Udp_Buff[i].DelayTime = 100;
		Multi_Udp_Buff[i].isValid = 1;

		//打开互斥锁
		pthread_mutex_unlock (&Local.udp_lock);
		sem_post(&multi_send_sem);
		break;
	}
}
//---------------------------------------------------------------------------
void OpenLock_Func(void)  //开锁
{
	int i;
	//锁定互斥锁
	pthread_mutex_lock (&Local.udp_lock);
	for(i=0; i<UDPSENDMAX; i++)
	if(Multi_Udp_Buff[i].isValid == 0)
	{
		Multi_Udp_Buff[i].SendNum = 0;
		Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
		Multi_Udp_Buff[i].CurrOrder = VIDEOTALK;
		sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d\0",Remote.DenIP[0],
		Remote.DenIP[1],Remote.DenIP[2],Remote.DenIP[3]);
		//头部
		memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
		//命令  ,子网广播解析
		Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
		Multi_Udp_Buff[i].buf[7] = ASK;    //主叫
		Multi_Udp_Buff[i].buf[8] = REMOTEOPENLOCK;

		//本机为主叫方
		if((Local.Status == 1)||(Local.Status == 3)||(Local.Status == 5)||(Local.Status == 7)||(Local.Status == 9))
		{
			memcpy(Multi_Udp_Buff[i].buf+9, LocalCfg.Addr, 20);
			memcpy(Multi_Udp_Buff[i].buf+29, LocalCfg.IP, 4);
			memcpy(Multi_Udp_Buff[i].buf+33, Remote.Addr[0], 20);
			memcpy(Multi_Udp_Buff[i].buf+53, Remote.IP[0], 4);
		}
		//本机为被叫方
		if((Local.Status == 2)||(Local.Status == 4)||(Local.Status == 6)||(Local.Status == 8)||(Local.Status == 10))
		{
			memcpy(Multi_Udp_Buff[i].buf+9, Remote.Addr[0], 20);
			memcpy(Multi_Udp_Buff[i].buf+29, Remote.IP[0], 4);
			memcpy(Multi_Udp_Buff[i].buf+33, LocalCfg.Addr, 20);
			memcpy(Multi_Udp_Buff[i].buf+53, LocalCfg.IP, 4);
		}

		Multi_Udp_Buff[i].nlength = 57;
		Multi_Udp_Buff[i].DelayTime = 100;
		Multi_Udp_Buff[i].isValid = 1;

		sem_post(&multi_send_sem);
		break;
	}
	//打开互斥锁
	pthread_mutex_unlock (&Local.udp_lock);
}

