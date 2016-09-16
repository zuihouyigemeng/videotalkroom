#include <stdio.h>
#include   <time.h>
#include <stdlib.h>
#include <semaphore.h>       //sem_t

#define CommonH
#include "common.h"

    //向Flash中存储文件

void DisplaySetupWindow(void);    //显示设置窗口（一级）
void OperateSetupWindow(short wintype, int currwindow);  //设置窗口操作（一级）

void DisplayEngineerSetupWindow(void);    //显示工程设置窗口（二级）
void OperateEngineerSetupWindow(short wintype, int currwindow);  //工程设置操作（二级）


void DisplayModiPassWindow(int passtype);   //显示修改密码窗口（三级、四级） 0 工程密码
void OperateModiPassWindow(short wintype, int currwindow);  //修改密码操作（三级、四级）

void DisplayAddressWindow(void);    //显示房号设置窗口（三级）
void OperateAddressWindow(short wintype, int currwindow);  //房号设置操作（三级）


void DisplayIPWindow(void);    //显示IP地址设置窗口（三级）
void OperateIPWindow(short wintype, int currwindow);  //修改IP地址设置操作（三级）

void DisplayCalibrateWindow(void);    //校准触摸屏窗口（二级）
void OperateCalibrateWindow(short wintype, int currwindow);  //校准触摸屏操作（二级）
//画十字函数   t_Flag 0--清除  1--写
void WriteCross(int xLeft, int yTop, int t_Flag, int PageNo);
int CrossPosX[4]={50, 750, 50, 750};
int CrossPosY[4]={50, 50, 430, 430};
int Calib_X[4];
int Calib_Y[4];

char keyboard_num_char[26] = {'1','2','3','4','5','6','7','8','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
char keyboard_en_char[26] = {'Q','W','E','R','T','Y','U','I','O','P','A','S','D','F','G','H','J','K','L','Z','X','C','V','B','N','M'};

//void DisplayIporAddr(int wintype);  //显示网络设置或房号设置

void SetupWindowInit(void);
void DisplayLanSetWindow(void);
void DisplayWlanSetWindow(void);
void DisplayRoomnoSetWindow(void);
void DisplayScreenContrastWindow(void);
void DisplayScreenSaveWindow(void);
void DisplayScreenCalibrateWindow(void);
void DisplayVoiceSetWindow(void);
void DisplayTimeSetWindow(void);
void DisplayPasswordSetWindow(void);
void DisplayLangSetWindow(void);
void DisplayUpgradeWindow(void);
void DisplaySysteminfoWindow(void);


void SetupWindowInit(void)
{
	char jpgfilename[80];
	int i;
	int ret;
	int setup_menu_button_y[12] = {57,132,207,282,357,432,533,57,132,207,282,533};
	int setup_screen_menu_button_x[3] = {15,120,223};
	int setup_upgrade_menu_button_x[2] = {15,120};
	int setup_keynum_button_x[15] = {485,485,565,645,485,565,645,485,565,645,565,645,227,309,389};
	int setup_keynum_button_y[15] = {382,217,217,217,272,272,272,327,327,327,382,382,382,382,382};
	int LanSet_edit_y[5] = {116,194,272,350,428};
	int WlanSet_edit_y[8] = {130,215,300,399,130,215,300,399};
	int RoomSet_edit_x[6] = {306,306,306,590,590,590};
	int RoomSet_edit_y[6] = {165,271,377,165,271,377};
	int TimeSet_edit_y[3] = {165,270,375};

	for(i=0;i<12;i++)
	{
		sprintf(jpgfilename,"%ssetup%d_down.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&setup_menu_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%ssetup%d_up.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&setup_menu_button[i],IMAGEUP);
		setup_menu_button[i].xLeft = 0;
		setup_menu_button[i].yTop = setup_menu_button_y[i];
	}

	for(i=0;i<5;i++)
	{
		sprintf(jpgfilename,"%ssetup_comkey%d.jpg\0",sPath,i+1);
		ImageLoadFromFile(jpgfilename,&setup_comkey_image[i]);
	}

	for(i=0;i<12;i++)
	{
		sprintf(jpgfilename,"%ssetup_bg%d.jpg\0",sPath,i+1);
		ImageLoadFromFile(jpgfilename,&setup_bg[i]);
		setup_bg[i].xLeft = 169;
		setup_bg[i].yTop = 72;
	}

	for(i=0;i<3;i++)
	{
		sprintf(jpgfilename,"%sscr%d_down.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&setup_screen_menu_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%sscr%d_up.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&setup_screen_menu_button[i],IMAGEUP);
		setup_screen_menu_button[i].xLeft = 169+setup_screen_menu_button_x[i];
		setup_screen_menu_button[i].yTop = 72+17;
	}

	for(i=0;i<2;i++)
	{
		sprintf(jpgfilename,"%supgrade%d_down.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&setup_upgrade_menu_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%supgrade%d_up.jpg\0",sPath,i+1);
		ImageButtonLoadFromFile(jpgfilename,&setup_upgrade_menu_button[i],IMAGEUP);
		setup_upgrade_menu_button[i].xLeft = 169+setup_upgrade_menu_button_x[i];
		setup_upgrade_menu_button[i].yTop = 72+17;
	}

	//password num
	for(i=0;i<10;i++)
	{
		setup_keynum_button[i] = talk_keynum_button[i];
	}

	for(i=10;i<15;i++)
	{
		sprintf(jpgfilename,"%ssnum%d_down.jpg\0",sPath,i);
		ImageButtonLoadFromFile(jpgfilename,&setup_keynum_button[i],IMAGEDOWN);
		sprintf(jpgfilename,"%ssnum%d_up.jpg\0",sPath,i);
		ImageButtonLoadFromFile(jpgfilename,&setup_keynum_button[i],IMAGEUP);
	}

	for(i=0;i<15;i++)
	{
		setup_keynum_button[i].xLeft = setup_keynum_button_x[i];
		setup_keynum_button[i].yTop = setup_keynum_button_y[i];
	}


	sprintf(jpgfilename,"%ssetup_edit1.jpg\0",sPath);
	EditLoadFromFile(jpgfilename,&setup_edit[0]);
	setup_edit[0].CursorX = 22;
	setup_edit[0].CursorY = 13;
	setup_edit[0].CursorHeight = 12;
	setup_edit[0].CursorCorlor = cBlack;
	setup_edit[0].fWidth = 8;
	setup_edit[0].Cursor_H = (unsigned char*)malloc(1*setup_edit[0].CursorHeight*3/2);
	SavePicS_D_Func(setup_edit[0].CursorX,setup_edit[0].CursorY,1,setup_edit[0].CursorHeight,setup_edit[0].Cursor_H,
					setup_edit[0].width,setup_edit[0].height,setup_edit[0].image);
	
	sprintf(jpgfilename,"%ssetup_edit2.jpg\0",sPath);
	EditLoadFromFile(jpgfilename,&setup_edit[1]);
	setup_edit[1].CursorX = 22;
	setup_edit[1].CursorY = 13;
	setup_edit[1].CursorHeight = 12;
	setup_edit[1].CursorCorlor = cBlack;
	setup_edit[1].fWidth = 8;
	setup_edit[1].Cursor_H = (unsigned char*)malloc(1*setup_edit[1].CursorHeight*3/2);
	SavePicS_D_Func(setup_edit[1].CursorX,setup_edit[1].CursorY,1,setup_edit[1].CursorHeight,setup_edit[1].Cursor_H,
					setup_edit[1].width,setup_edit[1].height,setup_edit[1].image);


	sprintf(jpgfilename,"%ssetup_edit3.jpg\0",sPath);
	EditLoadFromFile(jpgfilename,&setup_edit[2]);
	setup_edit[2].CursorX = 22;
	setup_edit[2].CursorY = 13;
	setup_edit[2].CursorHeight = 12;
	setup_edit[2].CursorCorlor = cBlack;
	setup_edit[2].fWidth = 8;
	setup_edit[2].Cursor_H = (unsigned char*)malloc(1*setup_edit[2].CursorHeight*3/2);
	SavePicS_D_Func(setup_edit[2].CursorX,setup_edit[2].CursorY,1,setup_edit[2].CursorHeight,setup_edit[2].Cursor_H,
					setup_edit[2].width,setup_edit[2].height,setup_edit[2].image);

	sprintf(jpgfilename,"%ssetup_edit4.jpg\0",sPath);
	EditLoadFromFile(jpgfilename,&setup_edit[3]);
	setup_edit[3].CursorX = 15;
	setup_edit[3].CursorY = 13;
	setup_edit[3].CursorHeight = 12;
	setup_edit[3].CursorCorlor = cBlack;
	setup_edit[3].fWidth = 8;
	setup_edit[3].Cursor_H = (unsigned char*)malloc(1*setup_edit[3].CursorHeight*3/2);
	SavePicS_D_Func(setup_edit[3].CursorX,setup_edit[3].CursorY,1,setup_edit[3].CursorHeight,setup_edit[3].Cursor_H,
					setup_edit[3].width,setup_edit[3].height,setup_edit[3].image);


	SetupPass_edit = setup_edit[3];
	SetupPass_edit.xLeft = 220;
	SetupPass_edit.yTop = 217;
	SetupPass_edit.Text[0] = '\0';
	SetupPass_edit.BoxLen = 0;
	SetupPass_edit.MaxLen = 4;
	SetupPass_edit.Focus = 0;
	SetupPass_edit.Visible = 0;

	for(i=0;i<5;i++)
	{
		LanSet_edit[i] = setup_edit[0];
		LanSet_edit[i].xLeft = 418;
		LanSet_edit[i].yTop = LanSet_edit_y[i];
		LanSet_edit[i].Text[0] = '\0';
		LanSet_edit[i].BoxLen = 0;
		LanSet_edit[i].MaxLen = 0;
		LanSet_edit[i].Focus = 0;
		LanSet_edit[i].Visible = 0;
	}

	for(i=0;i<8;i++)
	{
		WlanSet_edit[i] = setup_edit[0];
		WlanSet_edit[i].xLeft = 418;
		WlanSet_edit[i].yTop = WlanSet_edit_y[i];
		WlanSet_edit[i].Text[0] = '\0';
		WlanSet_edit[i].BoxLen = 0;
		WlanSet_edit[i].MaxLen = 0;
		WlanSet_edit[i].Focus = 0;
		WlanSet_edit[i].Visible = 0;
	}


	for(i=0;i<6;i++)
	{
		RoomSet_edit[i] = setup_edit[1];
		RoomSet_edit[i].xLeft = RoomSet_edit_x[i];
		RoomSet_edit[i].yTop = RoomSet_edit_y[i];
		RoomSet_edit[i].Text[0] = '\0';
		RoomSet_edit[i].BoxLen = 0;
		RoomSet_edit[i].MaxLen = 0;
		RoomSet_edit[i].Focus = 0;
		RoomSet_edit[i].Visible = 0;
		
	}

	VoiceSet_edit = setup_edit[2];
	VoiceSet_edit.xLeft = 404;
	VoiceSet_edit.yTop = 421;
	VoiceSet_edit.Text[0] = '\0';
	VoiceSet_edit.BoxLen = 0;
	VoiceSet_edit.MaxLen = 0;
	VoiceSet_edit.Focus = 0;
	VoiceSet_edit.Visible = 0;

	for(i=0;i<3;i++)
	{
		TimeSet_edit[i] = setup_edit[0];
		TimeSet_edit[i].xLeft = 418;
		TimeSet_edit[i].yTop = TimeSet_edit_y[i];
		TimeSet_edit[i].Text[0] = '\0';
		TimeSet_edit[i].BoxLen = 0;
		TimeSet_edit[i].MaxLen = 0;
		TimeSet_edit[i].Focus = 0;
		TimeSet_edit[i].Visible = 0;

		PassSet_edit[i] = setup_edit[0];
		PassSet_edit[i].xLeft = 418;
		PassSet_edit[i].yTop = TimeSet_edit_y[i];
		PassSet_edit[i].Text[0] = '\0';
		PassSet_edit[i].BoxLen = 0;
		PassSet_edit[i].MaxLen = 4;
		PassSet_edit[i].Focus = 0;
		PassSet_edit[i].Visible = 0;
	}

	sprintf(jpgfilename,"%s1.jpg\0",sPath);
	ImageLoadFromFile(jpgfilename,&blank_image);
	blank_image.xLeft = 0;
	blank_image.yTop = 357;

	if(label_keypad.image == NULL)
	{
		label_keypad.width = 480;
		label_keypad.height = 240;
		label_keypad.xLeft = 0;
		label_keypad.yTop = 368;
		label_keypad.image = (unsigned char*)malloc(label_keypad.width*label_keypad.height*3/2);
//		SavePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);
	}
	
}


void ShowKeyPad(int type)
{
	Local.keypad_show = 1;
	Local.keypad_type = type;
	DisplayImage(&keypad_image[Local.keypad_type],0);
}

void HideKeyPad(void)
{
	RestorePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);
	Local.keypad_show = 0;
}
void DisplaySetupMainWindow(void)
{
	char jpgfilename[80];
	
	Local.CurrentWindow = SetupMainWindow;
	sprintf(jpgfilename,"%spass.jpg\0",sPath);
	DisplayJPG(0,0,jpgfilename,1,SCRWIDTH,SCRHEIGHT,0);
	SetupPass_edit.BoxLen = 0;
	SetupPass_edit.Text[0] = '\0';
	DisplayEdit(&SetupPass_edit,0);
	WriteCursor(&SetupPass_edit,1,1,0);
	
}
void OperateSetupMainWindow(int wintype,int CurrentWindow)
{
	char str[10];
	char wavFile[50];
	if(Local.CurrentWindow != CurrentWindow)
		return;
	if(wintype < 15)
	{
		DisplayImageButton(&setup_keynum_button[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&setup_keynum_button[wintype],IMAGEUP,0);
	}
	else if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	WriteCursor(&SetupPass_edit,0,0,0);
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
		if(SetupPass_edit.BoxLen < SetupPass_edit.MaxLen)
		{
			str[0] = '0'+wintype;
			str[1] = '\0';
			strcat(SetupPass_edit.Text,str);
			str[0] = '*';
			str[1] = '\0';
			outxy16(SetupPass_edit.xLeft + SetupPass_edit.CursorX + SetupPass_edit.BoxLen*SetupPass_edit.fWidth,
                     SetupPass_edit.yTop + SetupPass_edit.CursorY - 2, 2, cBlack, 1, 1, str, 0, 0);
             SetupPass_edit.BoxLen ++;
		}
		break;
	case 10: //清除
    case 12: //清除
          SetupPass_edit.Text[0] = '\0';
          SetupPass_edit.BoxLen = 0;
          DisplayEdit(&SetupPass_edit, 0);
          break;
    case 11: //确认
    case 13: //确认
          SetupPass_edit.Text[SetupPass_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
          if(strcmp(SetupPass_edit.Text, LocalCfg.EngineerPass) == 0)
          {
             DisplayLanSetWindow();
          }
          else
          {
              sprintf(wavFile, "%spasserror.wav\0", wavPath);
              StartPlayWav(wavFile, 0);

              //清除输入框
              SetupPass_edit.Text[0] = 0;
              SetupPass_edit.BoxLen = 0;
              DisplayEdit(&SetupPass_edit, 0);
          }
          break;
	case 14:
		DisplayMainWindow(0);
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
//	if(Local.CurrentWindow == CurrentWindow )
	WriteCursor(&SetupPass_edit,1,1,0);
}



void DisplayLanSetWindow(void)
{
	int i;

	Local.CurrentWindow = LanSetWindow;
	DisplayImage(&setup_bg[0],0);
	for(i=0;i<7;i++)
	{
		if(i==0)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
	//DisplayImageButton(&setup_menu_button[10],IMAGEUP,0);
	//display lan set
	//网卡地址
	DisplayEdit(&LanSet_edit[0],0);
	LanSet_edit[0].BoxLen = 0;
	sprintf(LanSet_edit[0].Text,"%02x:%02x:%02x\0",LocalCfg.Mac_Addr[3],LocalCfg.Mac_Addr[4],LocalCfg.Mac_Addr[5]);
	outxy16(LanSet_edit[0].xLeft + LanSet_edit[0].CursorX + LanSet_edit[0].BoxLen*LanSet_edit[0].fWidth,
		LanSet_edit[0].yTop  + LanSet_edit[0].CursorY - 2, 0, cBlack, 1, 1, LanSet_edit[0].Text, 0, 0);
	LanSet_edit[0].MaxLen= 8;
	LanSet_edit[0].BoxLen = 8;

	//IP地址
    DisplayEdit(&LanSet_edit[1], 0);
	LanSet_edit[1].BoxLen = 0;
    sprintf(LanSet_edit[1].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP[0], LocalCfg.IP[1],LocalCfg.IP[2], LocalCfg.IP[3]);
    outxy16(LanSet_edit[1].xLeft + LanSet_edit[1].CursorX + LanSet_edit[1].BoxLen*LanSet_edit[1].fWidth,
		LanSet_edit[1].yTop  + LanSet_edit[1].CursorY - 2, 0, cBlack, 1, 1, LanSet_edit[1].Text, 0, 0);
    LanSet_edit[1].MaxLen = 15;
	LanSet_edit[1].BoxLen = 15;

    //子网掩码
    DisplayEdit(&LanSet_edit[2], 0);	
	LanSet_edit[2].BoxLen = 0;
    sprintf(LanSet_edit[2].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Mask[0], LocalCfg.IP_Mask[1],LocalCfg.IP_Mask[2], LocalCfg.IP_Mask[3]);
    outxy16(LanSet_edit[2].xLeft + LanSet_edit[2].CursorX + LanSet_edit[2].BoxLen*LanSet_edit[2].fWidth,
		LanSet_edit[2].yTop  + LanSet_edit[2].CursorY - 2, 0, cBlack, 1, 1, LanSet_edit[2].Text, 0, 0);
   	LanSet_edit[2].MaxLen = 15;
	LanSet_edit[2].BoxLen = 15;


    //网关
    DisplayEdit(&LanSet_edit[3], 0);
	LanSet_edit[3].BoxLen = 0;
    sprintf(LanSet_edit[3].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Gate[0], LocalCfg.IP_Gate[1],LocalCfg.IP_Gate[2], LocalCfg.IP_Gate[3]);
    outxy16(LanSet_edit[3].xLeft + LanSet_edit[3].CursorX + LanSet_edit[3].BoxLen*LanSet_edit[3].fWidth,
		LanSet_edit[3].yTop  + LanSet_edit[3].CursorY - 2, 0, cBlack, 1, 1, LanSet_edit[3].Text, 0, 0);
    LanSet_edit[3].MaxLen = 15;
	LanSet_edit[3].BoxLen = 15;

	//服务器地址
    DisplayEdit(&LanSet_edit[4], 0);
	LanSet_edit[4].BoxLen = 0;
    sprintf(LanSet_edit[4].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Server[0], LocalCfg.IP_Server[1],LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
	outxy16(LanSet_edit[4].xLeft + LanSet_edit[4].CursorX + LanSet_edit[4].BoxLen*LanSet_edit[4].fWidth,
		LanSet_edit[4].yTop  + LanSet_edit[4].CursorY - 2, 0, cBlack, 1, 1, LanSet_edit[4].Text, 0, 0);
    LanSet_edit[4].MaxLen = 15;
	LanSet_edit[4].BoxLen = 15;

	CurrBox = 0;
//	WriteCursor(&LanSet_edit[0],1,1,0);
	//if(label_keypad.image != NULL)
	//	free(label_keypad.image);

	SavePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);

	Local.keypad_show = 0;
	
}
void OperateLanSetWindow(int wintype,int currwindow)
{
	char str[10];
	int input_ok;
	int hex_data[6];
	char TmpText[5][5];
	char wavFile[100];
	int i,j;
	
	if(Local.CurrentWindow != currwindow)
		return;

	if((wintype>=7)&&(wintype<37))
	{
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEUP,0);
	}
	else if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	WriteCursor(&LanSet_edit[CurrBox],0,0,0);

	switch(wintype)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		CurrBox = wintype;
		ShowKeyPad(1);
		WriteCursor(&LanSet_edit[CurrBox],1,1,0);
		break;
	/*keypad**/
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		switch(CurrBox)
		{
		case 0:
			if(LanSet_edit[CurrBox].BoxLen<LanSet_edit[CurrBox].MaxLen)
			{
				str[0] = keyboard_num_char[wintype-7];
				str[1] = '\0';
				memcpy(LanSet_edit[CurrBox].Text+LanSet_edit[CurrBox].BoxLen,str,1);
				outxy16(LanSet_edit[CurrBox].xLeft+LanSet_edit[CurrBox].CursorX+LanSet_edit[CurrBox].BoxLen*LanSet_edit[CurrBox].fWidth,
					LanSet_edit[CurrBox].yTop+LanSet_edit[CurrBox].CursorY-2,1,cBlack,1,1,str,0,0);
				LanSet_edit[CurrBox].BoxLen++;
				if(LanSet_edit[CurrBox].Text[LanSet_edit[CurrBox].BoxLen]==':')
					LanSet_edit[CurrBox].BoxLen ++;
			}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			if(LanSet_edit[CurrBox].BoxLen<LanSet_edit[CurrBox].MaxLen)
			{
				str[0] = keyboard_num_char[wintype-7];
				str[1] = '\0';
				memcpy(LanSet_edit[CurrBox].Text+LanSet_edit[CurrBox].BoxLen,str,1);
				outxy16(LanSet_edit[CurrBox].xLeft+LanSet_edit[CurrBox].CursorX+LanSet_edit[CurrBox].BoxLen*LanSet_edit[CurrBox].fWidth,
					LanSet_edit[CurrBox].yTop+LanSet_edit[CurrBox].CursorY-2,1,cBlack,1,1,str,0,0);
				LanSet_edit[CurrBox].BoxLen++;
				if(LanSet_edit[CurrBox].Text[LanSet_edit[CurrBox].BoxLen]=='.')
					LanSet_edit[CurrBox].BoxLen ++;
			}
			break;
		}
		break;

	case 33:
	case 34:
	case 35:
		break;
	case 36:
		LanSet_edit[CurrBox].Text[0] = 0;
		LanSet_edit[CurrBox].BoxLen = 0;
		DisplayEdit(&LanSet_edit[CurrBox],0);
		switch(CurrBox)
		{
		case 0:
			strcpy(LanSet_edit[CurrBox].Text,"  :  :  ");
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			strcpy(LanSet_edit[CurrBox].Text,"   .   .   .   ");
			break;
		}
		outxy16(LanSet_edit[CurrBox].xLeft+LanSet_edit[CurrBox].CursorX+LanSet_edit[CurrBox].BoxLen*LanSet_edit[CurrBox].fWidth,
					LanSet_edit[CurrBox].yTop+LanSet_edit[CurrBox].CursorY-2,0,cBlack,1,1,LanSet_edit[CurrBox].Text,0,0);
		break;
	case 37:
		input_ok = 1;
		if(LanSet_edit[0].BoxLen < (LanSet_edit[0].MaxLen - 1))
		{
			usleep(200*1000);
			sprintf(wavFile, "%smodifail.wav\0", wavPath);
			StartPlayWav(wavFile, 0);
			input_ok = 0;
			break;
		}
		if(input_ok == 1)
		for(i=1; i<5; i++)
		if(LanSet_edit[i].BoxLen < (LanSet_edit[i].MaxLen - 2))
		{
			usleep(200*1000);
			sprintf(wavFile, "%smodifail.wav\0", wavPath);
			StartPlayWav(wavFile, 0);

			CurrBox = i;
			input_ok = 0;
			break;
		}

		if(input_ok == 1)
		{
			memcpy(TmpText[0], LanSet_edit[0].Text, 2);
			TmpText[0][2] = '\0';
			memcpy(TmpText[1], LanSet_edit[0].Text + 3, 2);
			TmpText[1][2] = '\0';
			memcpy(TmpText[2], LanSet_edit[0].Text + 6, 2);
			TmpText[2][2] = '\0';

			LocalCfg.Mac_Addr[0] = 0x00;
			LocalCfg.Mac_Addr[1] = 0x50;
           		 LocalCfg.Mac_Addr[2] = 0x2A;

			for(i=0; i<3; i++)
			if(strlen(TmpText[i]) != 2)
			{
				strcpy(str, TmpText[i]);
				strcpy(TmpText[i], "0");
				strcat(TmpText[i], str);
				TmpText[i][2] = '\0';
			}
			for(i=0; i<3; i++)
			{
				sscanf(TmpText[i], "%x", &hex_data[i]);
				LocalCfg.Mac_Addr[i+3] = hex_data[i];
			}

			for(j=1; j<5; j++)
			{
				memcpy(TmpText[0], LanSet_edit[j].Text, 3);
				TmpText[0][3] = '\0';
				memcpy(TmpText[1], LanSet_edit[j].Text + 4, 3);
				TmpText[1][3] = '\0';
				memcpy(TmpText[2], LanSet_edit[j].Text + 8, 3);
				TmpText[2][3] = '\0';
				memcpy(TmpText[3], LanSet_edit[j].Text + 12, 3);
				TmpText[3][3] = '\0';

				switch(j)
				{
				case 1:
					for(i=0; i<4; i++)
					LocalCfg.IP[i] = atoi(TmpText[i]);
					break;
				case 2:
					for(i=0; i<4; i++)
					LocalCfg.IP_Mask[i] = atoi(TmpText[i]);
					break;
				case 3:
					for(i=0; i<4; i++)
					LocalCfg.IP_Gate[i] = atoi(TmpText[i]);
					break;
				case 4:
					for(i=0; i<4; i++)
					LocalCfg.IP_Server[i] = atoi(TmpText[i]);
					break;
				}
             }

			//广播地址
			for(i=0; i<4; i++)
			{
				if(LocalCfg.IP_Mask[i] != 0)
					LocalCfg.IP_Broadcast[i] = LocalCfg.IP_Mask[i] & LocalCfg.IP[i];
				else
					LocalCfg.IP_Broadcast[i] = 0xFF;
			}
			SaveToFlash(4);    //向Flash中存储文件
			RefreshNetSetup(1); //刷新网络设置
			sprintf(wavFile, "%smodisucc.wav\0", wavPath);
			StartPlayWav(wavFile, 0);

			SendFreeArp();
			HideKeyPad();
       	}
    	break;
	case 38:
		HideKeyPad();
		break;
		
	case 42:
		break;
	case 43:
		DisplayWlanSetWindow();
		break;
	case 44:
		DisplayRoomnoSetWindow();
		break;
	case 45:
		DisplayScreenContrastWindow();
		break;
	case 46:
		DisplayVoiceSetWindow();
		break;
	case 47:
		DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
		break;
	/*menu*/
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		Local.keypad_show = 0;
		DisplayMainWindow(wintype-50);
		break;
		
	}
	if(Local.CurrentWindow == currwindow)
		WriteCursor(&LanSet_edit[CurrBox],1,1,0);
}

void DisplayWlanSet(int wintype)
{
	DisplayImage(&setup_bg[wintype+1],0);

	switch(wintype)
	{
	case 0:
		///SSID
		//DisplayEdit(&WlanSet_edit[0],0);
		//sprintf(WlanSet_edit);
		break;
	case 1:
		break;
	}
}
void DisplayWlanSetWindow(void)
{
	int i;
	
	Local.CurrentWindow = WlanSetWindow;
	for(i=0;i<7;i++)
	{
		if(i==1)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
	DisplayWlanSet(0);
	SavePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);
}

void OperateWlanSetWindow(int wintype,int currwindow)
{
	if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 42:
		DisplayLanSetWindow();
		break;
	case 43:
		break;
	case 44:
		DisplayRoomnoSetWindow();
		break;
	case 45:
		DisplayScreenContrastWindow();
		break;
	case 46:
		DisplayVoiceSetWindow();
		break;
	case 47:
		DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
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
void DisplayRoomnoSetWindow(void)
{
	int i;
	
	Local.CurrentWindow = RoomSetWindow;
	DisplayImage(&setup_bg[3],0);
	for(i=0;i<7;i++)
	{
		if(i==2)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
	///
	//幢号
	DisplayEdit(&RoomSet_edit[0], 0);
	RoomSet_edit[0].BoxLen = 0;
	if(LocalCfg.Addr[0] == 'S')
	{
		memcpy(RoomSet_edit[0].Text, LocalCfg.Addr + 1, 4);
		RoomSet_edit[0].Text[4] = '\0';
	}
	outxy16(RoomSet_edit[0].xLeft + RoomSet_edit[0].CursorX + RoomSet_edit[0].BoxLen*RoomSet_edit[0].fWidth,RoomSet_edit[0].yTop  + RoomSet_edit[0].CursorY - 2, 1, cBlack, 1, 1, RoomSet_edit[0].Text, 0, 0);
	RoomSet_edit[0].BoxLen = 4;
	RoomSet_edit[0].MaxLen = 4;
	//单元号
	DisplayEdit(&RoomSet_edit[1], 0);
	RoomSet_edit[1].BoxLen = 0;
	memcpy(RoomSet_edit[1].Text, LocalCfg.Addr + 5, 2);
	RoomSet_edit[1].Text[2] = '\0';
	outxy16(RoomSet_edit[1].xLeft + RoomSet_edit[1].CursorX + RoomSet_edit[1].BoxLen*RoomSet_edit[1].fWidth,RoomSet_edit[1].yTop  + RoomSet_edit[1].CursorY - 2, 1, cBlack, 1, 1, RoomSet_edit[1].Text, 0, 0);
	RoomSet_edit[1].BoxLen = 2;
	RoomSet_edit[1].MaxLen = 2;
    //层号
	DisplayEdit(&RoomSet_edit[2], 0);
	RoomSet_edit[2].BoxLen = 0;
	memcpy(RoomSet_edit[2].Text, LocalCfg.Addr + 7, 2);
	RoomSet_edit[2].Text[2] = '\0';
	outxy16(RoomSet_edit[2].xLeft + RoomSet_edit[2].CursorX + RoomSet_edit[2].BoxLen*RoomSet_edit[2].fWidth,
		RoomSet_edit[2].yTop  + RoomSet_edit[2].CursorY - 2, 1, cBlack, 1, 1, RoomSet_edit[2].Text, 0, 0);
	RoomSet_edit[2].BoxLen = 2;
	RoomSet_edit[2].MaxLen = 2;

	//房号
	DisplayEdit(&RoomSet_edit[3], 0);
	RoomSet_edit[3].BoxLen = 0;
	memcpy(RoomSet_edit[3].Text, LocalCfg.Addr + 9, 2);
	RoomSet_edit[3].Text[2] = '\0';
	outxy16(RoomSet_edit[3].xLeft + RoomSet_edit[3].CursorX + RoomSet_edit[3].BoxLen*RoomSet_edit[3].fWidth,
		RoomSet_edit[3].yTop  + RoomSet_edit[3].CursorY - 2, 1, cBlack, 1, 1, RoomSet_edit[3].Text, 0, 0);
	RoomSet_edit[3].BoxLen = 2;
	RoomSet_edit[3].MaxLen = 2;

	//设备号
	DisplayEdit(&RoomSet_edit[4], 0);
	RoomSet_edit[4].BoxLen = 0;
	memcpy(RoomSet_edit[4].Text, LocalCfg.Addr + 11, 1);
	RoomSet_edit[4].Text[1] = '\0';
	outxy16(RoomSet_edit[4].xLeft + RoomSet_edit[4].CursorX + RoomSet_edit[4].BoxLen*RoomSet_edit[4].fWidth,
		RoomSet_edit[4].yTop  + RoomSet_edit[4].CursorY - 2, 1, cBlack, 1, 1, RoomSet_edit[4].Text, 0, 0);
	RoomSet_edit[4].BoxLen = 1;
	RoomSet_edit[4].MaxLen = 1;

	DisplayEdit(&RoomSet_edit[5], 0);
	RoomSet_edit[5].BoxLen = 0;
	//memcpy(RoomSet_edit[5].Text, LocalCfg.SyncNo, 1);
	RoomSet_edit[5].Text[0] = '1';
	RoomSet_edit[5].Text[1] = '\0';
	outxy16(RoomSet_edit[5].xLeft + RoomSet_edit[5].CursorX + RoomSet_edit[5].BoxLen*RoomSet_edit[5].fWidth,
		RoomSet_edit[5].yTop  + RoomSet_edit[5].CursorY - 2, 1, cBlack, 1, 1, RoomSet_edit[5].Text, 0, 0);
	RoomSet_edit[5].BoxLen = 1;
	RoomSet_edit[5].MaxLen = 1;

	CurrBox = 0;
	Local.keypad_show = 0;
   	SavePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);
}
void OperateRoomnoSetWindow(int wintype,int currwindow)
{
	char str[10];
	char wavFile[100];
	int i;
	if(Local.CurrentWindow != currwindow)
		return;

	if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	else if((wintype>=7)&&(wintype<39))
	{
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEUP,0);
	}
	WriteCursor(&RoomSet_edit[CurrBox],0,0,0);
	switch(wintype)
	{
	///keypad
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		CurrBox = wintype;
		ShowKeyPad(1);
		WriteCursor(&RoomSet_edit[CurrBox],1,1,0);
		break;
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		if(RoomSet_edit[CurrBox].BoxLen < RoomSet_edit[CurrBox].MaxLen)
		{
			str[0] = keyboard_num_char[wintype-7];
			str[1] = '\0';
		  memcpy(RoomSet_edit[CurrBox].Text + RoomSet_edit[CurrBox].BoxLen, str, 1);
         outxy16(RoomSet_edit[CurrBox].xLeft + RoomSet_edit[CurrBox].CursorX + RoomSet_edit[CurrBox].BoxLen*RoomSet_edit[CurrBox].fWidth,
                 RoomSet_edit[CurrBox].yTop  + RoomSet_edit[0].CursorY - 2, 1, cBlack, 1, 1, str, 0, 0);
         RoomSet_edit[CurrBox].BoxLen ++;
		}
		break;
	case 36:
		RoomSet_edit[CurrBox].Text[0] = 0;
		RoomSet_edit[CurrBox].BoxLen = 0;
		DisplayEdit(&RoomSet_edit[CurrBox],0);
		break;
	case 37:
		if((RoomSet_edit[0].BoxLen == 4))//&&(addr_edit[1].BoxLen == 2)&&(addr_edit[2].BoxLen == 2)&&(addr_edit[3].BoxLen == 2))
           {    
            if((RoomSet_edit[1].BoxLen != 0)||(RoomSet_edit[2].BoxLen != 0)||(RoomSet_edit[3].BoxLen != 0))
             {
              for(i = 1; i <= 3; i++)
               {
                switch(RoomSet_edit[i].BoxLen)
                 {
                  case 0:
                         strcpy(RoomSet_edit[i].Text, "00");
                         break;
                  case 1:
                         strcpy(str, RoomSet_edit[i].Text);
                         strcpy(RoomSet_edit[i].Text, "0");
                         strcat(RoomSet_edit[i].Text, str);
                         break;
                  case 2:
                         break;
                 }
                DisplayEdit(&RoomSet_edit[i], 0);
                outxy16(RoomSet_edit[i].xLeft  + RoomSet_edit[i].CursorX,
                          RoomSet_edit[i].yTop  + RoomSet_edit[0].CursorY - 2 , 1, cBlack, 1, 1, RoomSet_edit[i].Text, 0, 0);
                RoomSet_edit[i].BoxLen = 2;
               }
             }
            LocalCfg.Addr[0] == 'S';
            memcpy(LocalCfg.Addr + 1, RoomSet_edit[0].Text, 4);
            memcpy(LocalCfg.Addr + 5, RoomSet_edit[1].Text, 2);
            memcpy(LocalCfg.Addr + 7, RoomSet_edit[2].Text, 2);
            memcpy(LocalCfg.Addr + 9, RoomSet_edit[3].Text, 2);
            LocalCfg.Addr[11] == RoomSet_edit[4].Text[0];

            SaveToFlash(4);    //向Flash中存储文件
            
            //主机
            Local.isHost = '0';

            if(LocalCfg.Addr[0] == 'S')
             {
              Local.isHost = LocalCfg.Addr[11];
             }
            if(LocalCfg.Addr[0] == 'B')
             {
              Local.isHost = LocalCfg.Addr[5];
             }
            	Local.DenNum = 0;

             usleep(200*1000);
             sprintf(wavFile, "%smodisucc.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
			 HideKeyPad();
           }
          else
           {
             usleep(200*1000);
             sprintf(wavFile, "%smodifail.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
            CurrBox = 0;
           } 
		  
          break;
	case 38:
		HideKeyPad();
		break;
	case 42:
		DisplayLanSetWindow();
		break;
	case 43:
		DisplayWlanSetWindow();
		break;
	case 44:
		break;
	case 45:
		DisplayScreenContrastWindow();
		break;
	case 46:
		DisplayVoiceSetWindow();
		break;
	case 47:
		DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
		break;
		
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		Local.keypad_show = 0;
		DisplayMainWindow(wintype-50);

		break;
	}
	if(Local.CurrentWindow == currwindow)
		WriteCursor(&RoomSet_edit[CurrBox],1,1,0);
}
void DisplayScreenContrastWindow(void)
{
	int i;
	
	Local.CurrentWindow = ScreenContrastWindow;
	DisplayImage(&setup_bg[4],0);
	for(i=0;i<7;i++)
	{
		if(i==3)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
}
void OperateScreenContrastWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype>=0)&&(wintype<3))
	{
		DisplayImageButton(&setup_screen_menu_button[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&setup_screen_menu_button[wintype],IMAGEUP,0);
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
		break;
	case 1:
		DisplayScreenSaveWindow();
		break;
	case 2:
		DisplayScreenCalibrateWindow();
		break;

	
	case 42:
		DisplayLanSetWindow();
		break;
	case 43:
		DisplayWlanSetWindow();
		break;
	case 44:
		DisplayRoomnoSetWindow();
		break;
	case 45:
		//DisplayScreenContrastWindow();
		break;
	case 46:
		DisplayVoiceSetWindow();
		break;
	case 47:
		DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
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

void DisplayScreenSaveWindow(void)
{
	int i;
	
	Local.CurrentWindow = ScreenSavingWindow;
	DisplayImage(&setup_bg[5],0);
	for(i=0;i<7;i++)
	{
		if(i==3)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
}

void OperateScreenSaveWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if((wintype>=0)&&(wintype>3))
	{
		DisplayImageButton(&setup_screen_menu_button[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&setup_screen_menu_button[wintype],IMAGEUP,0);
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
		DisplayScreenContrastWindow();
	case 1:
		break;
	case 2:
		DisplayScreenCalibrateWindow();
		break;
	
	case 42:
		DisplayLanSetWindow();
		break;
	case 43:
		DisplayWlanSetWindow();
		break;
	case 44:
		DisplayRoomnoSetWindow();
		break;
	case 45:
		//DisplayScreenContrastWindow();
		break;
	case 46:
		DisplayVoiceSetWindow();
		break;
	case 47:
		DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
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

void DisplayScreenCalibrateWindow(void)
{
	int i;
	
	Local.CurrentWindow = ScreenCalibrateWindow;
	DisplayImage(&setup_bg[6],0);
	for(i=0;i<7;i++)
	{
		if(i==3)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}


}

void OperateScreenCalibrateWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;

	if((wintype>=0)&&(wintype>3))
	{
		DisplayImageButton(&setup_screen_menu_button[wintype],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&setup_screen_menu_button[wintype],IMAGEUP,0);
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
		DisplayScreenContrastWindow();
		break;
	case 1:
		DisplayScreenSaveWindow();
		break;
	case 2:
		//DisplayScreenCalibrateWindow();
		break;
	
	case 42:
		DisplayLanSetWindow();
		break;
	case 43:
		DisplayWlanSetWindow();
		break;
	case 44:
		DisplayRoomnoSetWindow();
		break;
	case 45:
		//DisplayScreenContrastWindow();
		break;
	case 46:
		DisplayVoiceSetWindow();
		break;
	case 47:
		DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
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
void DisplayVoiceSetWindow(void)
{
	int i;
	
	Local.CurrentWindow = VoiceSetWindow;
	DisplayImage(&setup_bg[7],0);
	for(i=0;i<6;i++)
	{
		if(i==4)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
	CurrBox = 0;
	Local.keypad_show = 0;
	SavePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);
}
void OperateVoiceSetWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	switch(wintype)
	{
	case 42:
		DisplayLanSetWindow();
		break;
	case 43:
		DisplayWlanSetWindow();
		break;
	case 44:
		DisplayRoomnoSetWindow();
		break;
	case 45:
		DisplayScreenContrastWindow();
		break;
	case 46:
		//DisplayVoiceSetWindow();
		break;
	case 47:
		DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
		break;
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		DisplayMainWindow(wintype-55);
		break;
	}
}
void DisplayTimeSetWindow(void)
{
	int i;
	time_t t;
	
	Local.CurrentWindow = TimeSetWindow;
	DisplayImage(&setup_bg[8],0);
	for(i=0;i<7;i++)
	{
		if(i==5)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
	
	time(&t);
	curr_tm_t=localtime(&t);

	DisplayEdit(&TimeSet_edit[0],0);
	TimeSet_edit[0].BoxLen = 0;
	sprintf(TimeSet_edit[0].Text,"%04d/%02d\0",curr_tm_t->tm_year+1900,curr_tm_t->tm_mon+1);
	outxy16(TimeSet_edit[0].xLeft + TimeSet_edit[0].CursorX + TimeSet_edit[0].BoxLen*TimeSet_edit[0].fWidth,TimeSet_edit[0].yTop  + TimeSet_edit[0].CursorY - 2, 0, cBlack, 1, 1, TimeSet_edit[0].Text, 0, 0);
	TimeSet_edit[0].MaxLen = 7;
	TimeSet_edit[0].BoxLen = 7;
	
	DisplayEdit(&TimeSet_edit[1],0);
	TimeSet_edit[1].BoxLen = 0;
	sprintf(TimeSet_edit[1].Text,"%02d\0",curr_tm_t->tm_mday);
	outxy16(TimeSet_edit[1].xLeft + TimeSet_edit[1].CursorX + TimeSet_edit[1].BoxLen*TimeSet_edit[1].fWidth,TimeSet_edit[1].yTop  + TimeSet_edit[1].CursorY - 2, 0, cBlack, 1, 1, TimeSet_edit[1].Text, 0, 0);
	TimeSet_edit[1].MaxLen = 2;
	TimeSet_edit[1].BoxLen = 2;

	DisplayEdit(&TimeSet_edit[2],0);
	TimeSet_edit[2].BoxLen = 0;
	sprintf(TimeSet_edit[2].Text,"%02d:%02d\0",curr_tm_t->tm_hour,curr_tm_t->tm_min);
	outxy16(TimeSet_edit[2].xLeft + TimeSet_edit[2].CursorX + TimeSet_edit[2].BoxLen*TimeSet_edit[2].fWidth,TimeSet_edit[2].yTop  + TimeSet_edit[2].CursorY - 2, 0, cBlack, 1, 1, TimeSet_edit[2].Text, 0, 0);
	TimeSet_edit[2].MaxLen = 5;
	TimeSet_edit[2].BoxLen = 5;

	Local.keypad_show = 0;
	SavePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);
}

void OperateTimeSetWindow(int wintype,int currwindow)
{
	char str[10],wavFile[100];
	int i;
	int input_ok;
	char TmpText[5][5];
	time_t t;
	
	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	else if((wintype>=7)&&(wintype<39))
	{
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEUP,0);
	}
	
	WriteCursor(&TimeSet_edit[CurrBox],0,0,0);
	switch(wintype)
	{
	case 0:
	case 1:
	case 2:
		CurrBox = wintype;
		ShowKeyPad(1);
		WriteCursor(&TimeSet_edit[CurrBox],1,1,0);
		break;
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		switch(CurrBox)
		{
		case 0:
			if(TimeSet_edit[0].BoxLen < TimeSet_edit[0].MaxLen)
			{
				str[0] = keyboard_num_char[wintype-7];
				str[1] = '\0';
				memcpy(TimeSet_edit[0].Text+TimeSet_edit[0].BoxLen,str,1);
				outxy16(TimeSet_edit[0].xLeft + TimeSet_edit[0].CursorX + TimeSet_edit[0].BoxLen*TimeSet_edit[0].fWidth,
					TimeSet_edit[0].yTop  + TimeSet_edit[0].CursorY - 2, 1, cBlack, 1, 1, str, 0, 0);
				TimeSet_edit[0].BoxLen++;
				if(TimeSet_edit[0].Text[TimeSet_edit[0].BoxLen] == '/')
					TimeSet_edit[0].BoxLen++;
			}
			break;
		case 1:
			if(TimeSet_edit[1].BoxLen < TimeSet_edit[1].MaxLen)
			{
				str[0] = keyboard_num_char[wintype-7];
				str[1] = '\0';
				memcpy(TimeSet_edit[1].Text+TimeSet_edit[1].BoxLen,str,1);
				outxy16(TimeSet_edit[1].xLeft + TimeSet_edit[1].CursorX + TimeSet_edit[1].BoxLen*TimeSet_edit[1].fWidth,
					TimeSet_edit[1].yTop  + TimeSet_edit[1].CursorY - 2, 1, cBlack, 1, 1,str, 0, 0);
				TimeSet_edit[1].BoxLen ++;
			}
			break;
		case 2:
			if(TimeSet_edit[2].BoxLen < TimeSet_edit[2].MaxLen)
			{
				str[0] = keyboard_num_char[wintype-7];
				str[1] = '\0';
				memcpy(TimeSet_edit[2].Text+TimeSet_edit[2].BoxLen,str,1);
				outxy16(TimeSet_edit[2].xLeft + TimeSet_edit[2].CursorX + TimeSet_edit[2].BoxLen*TimeSet_edit[2].fWidth,
					TimeSet_edit[2].yTop  + TimeSet_edit[2].CursorY - 2, 1, cBlack, 1, 1,str, 0, 0);
				TimeSet_edit[2].BoxLen++;
				if(TimeSet_edit[2].Text[TimeSet_edit[2].BoxLen] == ':')
					TimeSet_edit[2].BoxLen++;
			}
			break;
		}
		break;
	case 36:
		TimeSet_edit[CurrBox].BoxLen = 0;
		TimeSet_edit[CurrBox].Text[0] = '\0';
		DisplayEdit(&TimeSet_edit[CurrBox],0);
		switch(CurrBox)
		{
		case 0:
			strcpy(TimeSet_edit[CurrBox].Text,"    /  ");
			break;
		case 1:
			strcpy(TimeSet_edit[CurrBox].Text,"  ");
			break;
		case 2:
			strcpy(TimeSet_edit[CurrBox].Text,"  :  ");
			break;
		}
		outxy16(TimeSet_edit[CurrBox].xLeft + TimeSet_edit[CurrBox].CursorX + TimeSet_edit[CurrBox].BoxLen*TimeSet_edit[CurrBox].fWidth,
					TimeSet_edit[CurrBox].yTop  + TimeSet_edit[CurrBox].CursorY - 2, 0, cBlack, 1, 1, TimeSet_edit[CurrBox].Text, 0, 0);
		break;
	case 37:
		input_ok = 1;
		for(i=0;i<3;i++)
		{
			if(TimeSet_edit[CurrBox].BoxLen < TimeSet_edit[CurrBox].MaxLen -1)
			{
				usleep(200*1000);
				sprintf(wavFile,"%smodifail.wav\0",wavPath);
				StartPlayWav(wavFile,0);
				input_ok = 0;
				break;
			}
		}
		if(input_ok == 1)
		{
			memcpy(TmpText[0],TimeSet_edit[0].Text,4);
			TmpText[0][4] = '\0';
			memcpy(TmpText[1],TimeSet_edit[0].Text+5,2);
			TmpText[1][2] = '\0';
			memcpy(TmpText[2],TimeSet_edit[1].Text,2);
			TmpText[2][2] = '\0';
			memcpy(TmpText[3],TimeSet_edit[2].Text,2);
			TmpText[3][2] = '\0';
			memcpy(TmpText[4],TimeSet_edit[2].Text+3,2);
			TmpText[4][2] = '\0';

			#if 0
			if((atoi(TmpText[1])>12)||(atoi(TmpText[2])>31)||(atoi(TmpText[3])>24)||(atoi(TmpText[4]>59)))
			{
				usleep(wavFile,"%smodifail.wav\0",wavPath);
				StartPlayWav(wavFile,0);
				
			}
			else
			#endif
			{
				curr_tm_t->tm_year  =  atoi(TmpText[0])-1900;
				curr_tm_t->tm_mon   =  atoi(TmpText[1])-1;
				curr_tm_t->tm_mday  =  atoi(TmpText[2]);
				curr_tm_t->tm_hour  =  atoi(TmpText[3]);
				curr_tm_t->tm_min   =  atoi(TmpText[4]);
				curr_tm_t->tm_sec   =   0;

			
				printf("%d/%d/%d %d:%d\n",curr_tm_t->tm_year+1900,curr_tm_t->tm_mon+1,curr_tm_t->tm_mday,
					curr_tm_t->tm_hour,curr_tm_t->tm_min);
				t= mktime(curr_tm_t);
				stime((long*)&t);
#if 0
				DisplayEdit(&TimeSet_edit[0],0);
				TimeSet_edit[0].BoxLen = 0;
				sprintf(TimeSet_edit[0].Text,"%04d/%02d\0",curr_tm_t->tm_year+1900,curr_tm_t->tm_mon+1);
				outxy16(TimeSet_edit[0].xLeft + TimeSet_edit[0].CursorX + TimeSet_edit[0].BoxLen*TimeSet_edit[0].fWidth,TimeSet_edit[0].yTop  + TimeSet_edit[0].CursorY - 2, 0, cBlack, 1, 1, TimeSet_edit[0].Text, 0, 0);
				TimeSet_edit[0].MaxLen = 7;
				TimeSet_edit[0].BoxLen = 7;
				
				DisplayEdit(&TimeSet_edit[1],0);
				TimeSet_edit[1].BoxLen = 0;
				sprintf(TimeSet_edit[1].Text,"%02d\0",curr_tm_t->tm_mday);
				outxy16(TimeSet_edit[1].xLeft + TimeSet_edit[1].CursorX + TimeSet_edit[1].BoxLen*TimeSet_edit[1].fWidth,TimeSet_edit[1].yTop  + TimeSet_edit[1].CursorY - 2, 0, cBlack, 1, 1, TimeSet_edit[1].Text, 0, 0);
				TimeSet_edit[1].MaxLen = 2;
				TimeSet_edit[0].BoxLen = 2;

				printf("haha!\n");
				DisplayEdit(&TimeSet_edit[2],0);
				TimeSet_edit[2].BoxLen = 0;
				sprintf(TimeSet_edit[2].Text,"%02d:%02d\0",curr_tm_t->tm_hour,curr_tm_t->tm_min);
				outxy16(TimeSet_edit[2].xLeft + TimeSet_edit[2].CursorX + TimeSet_edit[2].BoxLen*TimeSet_edit[2].fWidth,TimeSet_edit[2].yTop  + TimeSet_edit[2].CursorY - 2, 0, cBlack, 1, 1, TimeSet_edit[2].Text, 0, 0);
				TimeSet_edit[2].MaxLen = 5;
				TimeSet_edit[2].BoxLen = 5;
				printf("设置系统时间!\n");
				#endif
			}
			HideKeyPad();
		}
		break;
	case 38:
		HideKeyPad();
		break;
	case 42:
		DisplayLanSetWindow();
		break;
	case 43:
		DisplayWlanSetWindow();
		break;
	case 44:
		DisplayRoomnoSetWindow();
		break;
	case 45:
		DisplayScreenContrastWindow();
		break;
	case 46:
		DisplayVoiceSetWindow();
		break;
	case 47:
		//DisplayTimeSetWindow();
		break;
	case 48:
		DisplayPasswordSetWindow();
		break;

	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		Local.keypad_show = 0;
		DisplayMainWindow(wintype-55);
		break;
	}
	if(Local.CurrentWindow == currwindow)
		WriteCursor(&TimeSet_edit[CurrBox],1,1,0);
}
void DisplayPasswordSetWindow(void)
{
	int i;
	char jpgfilename[100];
	
	Local.CurrentWindow = PassSetWindow;
	DisplayImage(&setup_bg[9],0);
	DisplayImage(&blank_image,0);
	for(i=7;i<12;i++)
	{
		if(i==7)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
	CurrBox = 0;
	for(i=0;i<3;i++)
	{
		PassSet_edit[i].BoxLen = 0;
	}
	Local.keypad_show = 0;
	SavePicBuf_Func(label_keypad.xLeft,label_keypad.yTop,label_keypad.width,label_keypad.height,label_keypad.image,0);
	
}
void OperatePasswordSetWindow(int wintype,int currwindow)
{
	char str[10];
	char wavFile[100];
	int i;
	
	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}
	else if((wintype>=7)&&(wintype<39))
	{
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&keyboard_num[wintype-7],IMAGEUP,0);
	}

	WriteCursor(&PassSet_edit[CurrBox],0,0,0);
	switch(wintype)
	{
	case 0:
	case 1:
	case 2:
		CurrBox = wintype;
		ShowKeyPad(1);
		WriteCursor(&PassSet_edit[CurrBox],1,1,0);
		break;

	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		if(PassSet_edit[CurrBox].BoxLen < PassSet_edit[CurrBox].MaxLen)
		{
			str[0] = keyboard_num_char[wintype-7];
			str[1] = '\0';
			strcat(PassSet_edit[CurrBox].Text,str);
			str[0] = '*';
			str[1] = '\0';
			outxy16(PassSet_edit[CurrBox].xLeft + PassSet_edit[CurrBox].CursorX + PassSet_edit[CurrBox].BoxLen*PassSet_edit[CurrBox].fWidth,
                     PassSet_edit[CurrBox].yTop  + PassSet_edit[CurrBox].CursorY - 2, 3, cBlack, 1, 1, str, 0, 0);
			PassSet_edit[CurrBox].BoxLen++;
		}
		break;
	case 36:///清除
		PassSet_edit[CurrBox].Text[0] = '\0';
		PassSet_edit[CurrBox].BoxLen = 0;
		DisplayEdit(&PassSet_edit[CurrBox],0);
		break;
	case 37:///confirm
		if((strcmp(PassSet_edit[0].Text,LocalCfg.EngineerPass)==0)&&(strcmp(PassSet_edit[1].Text,PassSet_edit[2].Text)==0)&&(PassSet_edit[1].Text[0]!= '\0'))
		{
			strcpy(LocalCfg.EngineerPass,PassSet_edit[1].Text);
			SaveToFlash(4);
			sprintf(wavFile,"%smodisucc.wav\0",wavPath);
			StartPlayWav(wavFile);
		}
		else
		{
			printf("修改失败!\n");
			sprintf(wavFile,"%smodifail.wav\0",wavPath);
			StartPlayWav(wavFile);
		}
		for(i=0;i<3;i++)
		{
			PassSet_edit[i].BoxLen = 0;
			PassSet_edit[i].Text[0] = '\0';
			DisplayEdit(&PassSet_edit[i],0);
		}
		HideKeyPad();
	case 38:
		HideKeyPad();
		break;
	case 42:
		//DisplayPasswordSetWindow();
		break;
	case 43:
		DisplayLangSetWindow();
		break;
	case 44:
		DisplayUpgradeSDWindow();
		break;
	case 45:
		DisplaySysteminfoWindow();
		break;
	case 46:
		DisplayLanSetWindow();
		break;
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
		Local.keypad_show = 0;
		DisplayMainWindow(wintype-50);
		break;
	}
}
void DisplayLangSetWindow(void)
{
	int i;

	Local.CurrentWindow = LangSetWindow;
	DisplayImage(&setup_bg[10],0);
	for(i=7;i<12;i++)
	{
		if(i==8)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}	
}
void OperateLangSetWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 42:
		DisplayPasswordSetWindow();
		break;
	case 43:
		//DisplayLangSetWindow();
		break;
	case 44:
		DisplayUpgradeSDWindow();
		break;
	case 45:
		DisplaySysteminfoWindow();
		break;
	case 46:
		DisplayLanSetWindow();
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
void DisplayUpgradeSDWindow(void)
{
	int i;
	
	Local.CurrentWindow = UpgradeSDWindow;
	DisplayImage(&setup_bg[11],0);
	for(i=7;i<12;i++)
	{
		if(i==9)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}	

	for(i=0;i<2;i++)
	{
		if(i==0)
		{
			DisplayImageButton(&setup_upgrade_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_upgrade_menu_button[i],IMAGEUP,0);
		}
	}
}

void OperateUpgradeSDWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 0:
		//DisplayUpgradeSDWindow();
		break;
	case 1:
		DisplayUpgradeRemoteWindow();
		break;

	case 42:
		DisplayPasswordSetWindow();
		break;
	case 43:
		DisplayLangSetWindow();
		break;
	case 44:
		//DisplayUpgradeSDWindow();
		break;
	case 45:
		DisplaySysteminfoWindow();
		break;
	case 46:
		DisplayLanSetWindow();
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
void DisplayUpgradeRemoteWindow(void)
{
	int i;
	
	Local.CurrentWindow = UpgradeRemoteWindow;
	for(i=7;i<12;i++)
	{
		if(i==9)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}
	DisplayImage(&setup_bg[11],0);

	for(i=0;i<2;i++)
	{
		if(i==1)
		{
			DisplayImageButton(&setup_upgrade_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_upgrade_menu_button[i],IMAGEUP,0);
		}
	}
}
void OperateUpgradeRemoteWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype >= 50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 0:
		DisplayUpgradeSDWindow();
		break;
	case 1:
		//DisplayUpgradeRemoteWindow();
		break;
	case 42:
		DisplayPasswordSetWindow();
		break;
	case 43:
		DisplayLangSetWindow();
		break;
	case 44:
		//DisplayUpgradeSDWindow();
		break;
	case 45:
		DisplaySysteminfoWindow();
		break;
	case 46:
		DisplayLanSetWindow();
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

void ShowVersion(void)
{
	char str[100];
	int xLeft = 200;
	int yTop = 100;
	

	sprintf(str,"硬件版本:      %s\0",HARDWAREVER);
	outxy16(xLeft, yTop+30*1, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"软件版本:      %s\0",SOFTWAREVER);
	outxy16(xLeft, yTop+30*2, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"产品序列号:    %s\0",SERIALNUM);
	outxy16(xLeft, yTop+30*3, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"地址:          %s\0",LocalCfg.Addr);
	outxy16(xLeft, yTop+30*4, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"Mac地址:       %02x:%02x:%02x:%02x:%02x:%02x\0",LocalCfg.Mac_Addr[0],LocalCfg.Mac_Addr[1],LocalCfg.Mac_Addr[2],LocalCfg.Mac_Addr[3],LocalCfg.Mac_Addr[4],LocalCfg.Mac_Addr[5]);
	outxy16(xLeft, yTop+30*5, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"IP地址:        %d.%d.%d.%d\0",LocalCfg.IP[0],LocalCfg.IP[1],LocalCfg.IP[2],LocalCfg.IP[3]);
	outxy16(xLeft, yTop+30*6, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"子网掩码:      %d.%d.%d.%d\0",LocalCfg.IP_Mask[0],LocalCfg.IP_Mask[1],LocalCfg.IP_Mask[2],LocalCfg.IP_Mask[3]);
	outxy16(xLeft, yTop+30*7, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"网关IP地址:    %d.%d.%d.%d\0",LocalCfg.IP_Gate[0],LocalCfg.IP_Gate[1],LocalCfg.IP_Gate[2],LocalCfg.IP_Gate[3]);
	outxy16(xLeft, yTop+30*8, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"服务器IP地址:  %d.%d.%d.%d\0",LocalCfg.IP_Server[0],LocalCfg.IP_Server[1],LocalCfg.IP_Server[2],LocalCfg.IP_Server[3]);
	outxy16(xLeft, yTop+30*9, 3, cWhite, 1, 1, str, 0, 0);
	sprintf(str,"副机数量:      %d\0",Local.DenNum);
	outxy16(xLeft, yTop+30*10, 3, cWhite, 1, 1, str, 0, 0);
	
}
void DisplaySysteminfoWindow(void)
{
	int i;
	
	Local.CurrentWindow = SystemInfoWindow;
	DisplayImage(&talk_bg[1],0);
	for(i=7;i<12;i++)
	{
		if(i==10)
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEDOWN,0);
		}
		else
		{
			DisplayImageButton(&setup_menu_button[i],IMAGEUP,0);
		}
	}	
	ShowVersion();
}
void OperateSysteminfoWindow(int wintype,int currwindow)
{
	if(Local.CurrentWindow != currwindow)
		return;
	if(wintype>=50)
	{
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEDOWN,0);
		usleep(DELAYTIME*1000);
		DisplayImageButton(&bigmenu_button[wintype-50],IMAGEUP,0);
	}

	switch(wintype)
	{
	case 42:
		DisplayPasswordSetWindow();
		break;
	case 43:
		DisplayLangSetWindow();
		break;
	case 44:
		DisplayUpgradeSDWindow();
		break;
	case 45:
		//DisplaySysteminfoWindow();
		break;
	case 46:
		DisplayLanSetWindow();
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
#if 0

//---------------------------------------------------------------------------
void DisplaySetupWindow(void)    //显示设置窗口（一级）
{
  int i;
  char vername[30];
  char jpgfilename[80];
  strcpy(jpgfilename, sPath);
  strcat(jpgfilename,"setup.jpg");
  DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 0);

  for(i=0; i<12; i++)
   {
    num2_button[i].xLeft = NumxLeft4[i];
    num2_button[i].yTop = NumyTop4[i];
    DisplayImageButton(&num2_button[i], IMAGEUP, 0);
   }

  //设置窗口（一级）提示条
  if(Label_Setup1.image == NULL)
   {
    Label_Setup1.width = 42;
    Label_Setup1.height = 48;
    Label_Setup1.xLeft = 176;
    Label_Setup1.yTop = 159;
    Label_Setup1.image = (unsigned char *)malloc(42*48*3/2);  //保存一块屏幕缓冲
    SavePicBuf_Func(176, 159, 42, 48, Label_Setup1.image, 0);

    modi_engi_label = Label_Setup1;

    addr_old_label = Label_Setup1;   //原房号Label
    addr_label = Label_Setup1;   //房号设置窗口Label

    ip_old_label = Label_Setup1;   //原IP地址Label
    ip_label = Label_Setup1;   //IP地址设置窗口Label
   }

  //设置窗口（一级）提示条
  if(Label_Setup2.image == NULL)
   {
    Label_Setup2.width = 42;
    Label_Setup2.height = 48;
    Label_Setup2.xLeft = 411;
    Label_Setup2.yTop = 153;
    Label_Setup2.image = (unsigned char *)malloc(42*48*3/2);  //保存一块屏幕缓冲
    SavePicBuf_Func(411, 153, 42, 48, Label_Setup2.image, 0);
   }

  setup_pass_edit.xLeft = 355;
  setup_pass_edit.yTop = 206;
  setup_pass_edit.BoxLen = 0;
  setup_pass_edit.Text[0] = '\0';
  CurrBox = 0;
  //打开光标
  WriteCursor(&setup_pass_edit, 1, 1, 0);
  DisplayEdit(&setup_pass_edit, 0);

  for(i=0; i<2; i++)
    DisplayImageButton(&setup_pass_button[i], IMAGEUP, 0);

  Local.PrevWindow = Local.CurrentWindow;
  Local.CurrentWindow = 5;
}
//---------------------------------------------------------------------------
void OperateSetupWindow(short wintype, int currwindow)  //设置窗口操作（一级）
{
  char wavFile[80];
  int xLeft,yTop;
  int i, j;
  char jpgfilename[80];
  char str[5];
  int isOk;
  char TmpText[5][5];
  int hex_data[6];
  xLeft = 120;
  yTop = 36;
  printf("wintype = %d\n", wintype);
  if(wintype < 12)
   {
    DisplayImageButton(&num2_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&num2_button[wintype], IMAGEUP, 0);
   }
  if((wintype >= 12)&&(wintype <= 13))
   {
    DisplayImageButton(&setup_pass_button[wintype-12], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&setup_pass_button[wintype-12], IMAGEUP, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }
  //停止光标,并清一下
  WriteCursor(&setup_pass_edit, 0, 0, 0);
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
           if(setup_pass_edit.BoxLen < setup_pass_edit.MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             strcat(setup_pass_edit.Text, str);
             str[0] = '*';
             str[1] = '\0';
             outxy16(setup_pass_edit.xLeft + setup_pass_edit.CursorX + setup_pass_edit.BoxLen*setup_pass_edit.fWidth,
                     setup_pass_edit.yTop + setup_pass_edit.CursorY - 2, 2, cBlack, 1, 1, str, 0, 0);
             setup_pass_edit.BoxLen ++;
            }
           break;
    case 10: //清除
    case 13: //清除
          setup_pass_edit.Text[0] = 0;
          setup_pass_edit.BoxLen = 0;
          DisplayEdit(&setup_pass_edit, 0);
          break;
    case 11: //确认
    case 12: //确认
          printf("setup_pass_edit.BoxLen = %d\n", setup_pass_edit.BoxLen);
          setup_pass_edit.Text[setup_pass_edit.BoxLen] = '\0';
          LocalCfg.EngineerPass[4] = '\0';
          if(strcmp(setup_pass_edit.Text, LocalCfg.EngineerPass) == 0)
           {
             DisplayIPWindow();
           }
          else
             {
              sprintf(wavFile, "%spasserror.wav\0", wavPath);
              StartPlayWav(wavFile, 0);

              //清除输入框
              setup_pass_edit.Text[0] = 0;
              setup_pass_edit.BoxLen = 0;
              DisplayEdit(&setup_pass_edit, 0);
             }
          break;
    case 30:  //首页
    case 31:  //安防
    case 32:  //家电
    case 33:  //对讲
    case 34:  //信息
    //case 35:  //设置
           DisplayMainWindow(wintype - 30);
           break;
   }
  if(Local.CurrentWindow == 5)
    //打开光标
    WriteCursor(&setup_pass_edit, 1, 1, 0);
}
//---------------------------------------------------------------------------
void DisplayIporAddr(int wintype)  //显示网络设置或房号设置
{
  char jpgfilename[80];
  int i;
  int EditxLeft1[5] = {347, 347, 347, 347, 347};
  int EdityTop1[5] = {155, 198, 240, 284, 329};
  int EditMaxLen1[5] = {8, 15, 15, 15, 15};

  int EditxLeft[5] = {347, 347, 347, 347, 347};
  int EdityTop[5] = {155, 198, 240, 284, 329};
  int EditMaxLen[5] = { 4, 2, 2, 2, 1};

  int EditxLeft2[2] = {343, 343};
  int EdityTop2[2] = {214, 256};
  if((Local.CurrentWindow == 151)||(Local.CurrentWindow == 159)||(Local.CurrentWindow == 161)||(Local.CurrentWindow == 156))
   {
    switch(wintype)
     {
      case 0:
             DisplayImage(&ip_image[0], 0);
             DisplayImage(&ip_image[1], 0);
             break;
      case 1:
             DisplayImage(&addr_image[0], 0);
             DisplayImage(&addr_image[1], 0);
             break;
      case 2:
             DisplayImage(&pass_image[0], 0);
             DisplayImage(&pass_image[1], 0);
             break;
     }

  for(i=0; i<5; i++)
   {
    addr_edit[i].xLeft = EditxLeft[i];
    addr_edit[i].yTop = EdityTop[i];
    addr_edit[i].Text[0] = '\0';
    addr_edit[i].BoxLen = 0;         //文本框当前输入长度
    addr_edit[i].MaxLen = EditMaxLen[i];         //文本框最大输入长度
    addr_edit[i].Focus = 0;        //显示输入光标
    addr_edit[i].Visible = 0;
   }

  for(i=0; i<5; i++)
   {
    ip_edit[i].xLeft = EditxLeft1[i];
    ip_edit[i].yTop = EdityTop1[i];
    ip_edit[i].Text[0] = '\0';
    ip_edit[i].BoxLen = 0;         //文本框当前输入长度
    ip_edit[i].MaxLen = EditMaxLen1[i];         //文本框最大输入长度
    ip_edit[i].Focus = 0;        //显示输入光标
    ip_edit[i].Visible = 0;
   }

  for(i=0; i<2; i++)
   {
    modi_engi_edit[i].xLeft = EditxLeft2[i];
    modi_engi_edit[i].yTop = EdityTop2[i];
    modi_engi_edit[i].Text[0] = '\0';
    modi_engi_edit[i].BoxLen = 0;         //文本框当前输入长度
    modi_engi_edit[i].MaxLen = 4;         //文本框最大输入长度
    modi_engi_edit[i].Focus = 0;        //显示输入光标
    modi_engi_edit[i].Visible = 0;
   }

    CurrBox = 0;
    switch(wintype)
     {
      case 0:
            //网卡地址
            DisplayEdit(&ip_edit[0], 0);
            //  strcpy(setup_edit[4].Text, "  :  :  ");
            sprintf(ip_edit[0].Text, "%02X:%02X:%02X\0", LocalCfg.Mac_Addr[3], LocalCfg.Mac_Addr[4], LocalCfg.Mac_Addr[5]);
            outxy16(ip_edit[0].xLeft + ip_edit[0].CursorX + ip_edit[0].BoxLen*ip_edit[0].fWidth,
                    ip_edit[0].yTop  + ip_edit[0].CursorY - 2, 0, cBlack, 1, 1, ip_edit[0].Text, 0, 0);
            ip_edit[0].BoxLen = 8;

            //IP地址
            DisplayEdit(&ip_edit[1], 0);
            //  srcpy(ip_edit.Text, "   .   .   .   ");
            sprintf(ip_edit[1].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP[0], LocalCfg.IP[1],
                    LocalCfg.IP[2], LocalCfg.IP[3]);
            outxy16(ip_edit[1].xLeft + ip_edit[1].CursorX + ip_edit[1].BoxLen*ip_edit[1].fWidth,
                    ip_edit[1].yTop  + ip_edit[0].CursorY - 2, 0, cBlack, 1, 1, ip_edit[1].Text, 0, 0);
            ip_edit[1].BoxLen = 15;

            //子网掩码
            DisplayEdit(&ip_edit[2], 0);
            //  strcpy(ip_edit.Text, "   .   .   .   ");
            sprintf(ip_edit[2].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Mask[0], LocalCfg.IP_Mask[1],
                    LocalCfg.IP_Mask[2], LocalCfg.IP_Mask[3]);
            outxy16(ip_edit[2].xLeft + ip_edit[2].CursorX + ip_edit[2].BoxLen*ip_edit[2].fWidth,
                    ip_edit[2].yTop  + ip_edit[0].CursorY - 2, 0, cBlack, 1, 1, ip_edit[2].Text, 0, 0);
            ip_edit[2].BoxLen = 15;

            //网关
            DisplayEdit(&ip_edit[3], 0);
            //  strcpy(ip_edit.Text, "   .   .   .   ");
            sprintf(ip_edit[3].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Gate[0], LocalCfg.IP_Gate[1],
                    LocalCfg.IP_Gate[2], LocalCfg.IP_Gate[3]);
            outxy16(ip_edit[3].xLeft + ip_edit[3].CursorX + ip_edit[3].BoxLen*ip_edit[3].fWidth,
                    ip_edit[3].yTop  + ip_edit[0].CursorY - 2, 0, cBlack, 1, 1, ip_edit[3].Text, 0, 0);
            ip_edit[3].BoxLen = 15;

            //服务器地址
            DisplayEdit(&ip_edit[4], 0);
            //  strcpy(ip_edit.Text, "   .   .   .   ");
            sprintf(ip_edit[4].Text, "%03d.%03d.%03d.%03d\0", LocalCfg.IP_Server[0], LocalCfg.IP_Server[1],
                    LocalCfg.IP_Server[2], LocalCfg.IP_Server[3]);
            outxy16(ip_edit[4].xLeft + ip_edit[4].CursorX + ip_edit[4].BoxLen*ip_edit[4].fWidth,
                    ip_edit[4].yTop  + ip_edit[0].CursorY - 2, 0, cBlack, 1, 1, ip_edit[4].Text, 0, 0);
            ip_edit[4].BoxLen = 15;
            break;
      case 1:
            //幢号
            DisplayEdit(&addr_edit[0], 0);
            if(LocalCfg.Addr[0] == 'S')
             {
              memcpy(addr_edit[0].Text, LocalCfg.Addr + 1, 4);
              addr_edit[0].Text[4] = '\0';
             }
           outxy16(addr_edit[0].xLeft + addr_edit[0].CursorX + addr_edit[0].BoxLen*addr_edit[0].fWidth,
                   addr_edit[0].yTop  + addr_edit[0].CursorY - 2, 1, cBlack, 1, 1, addr_edit[0].Text, 0, 0);
           addr_edit[0].BoxLen = 4;

           //单元号
           DisplayEdit(&addr_edit[1], 0);
           memcpy(addr_edit[1].Text, LocalCfg.Addr + 5, 2);
           addr_edit[1].Text[2] = '\0';
           outxy16(addr_edit[1].xLeft + addr_edit[1].CursorX + addr_edit[1].BoxLen*addr_edit[1].fWidth,
                   addr_edit[1].yTop  + addr_edit[1].CursorY - 2, 1, cBlack, 1, 1, addr_edit[1].Text, 0, 0);
           addr_edit[1].BoxLen = 2;

           //层号
           DisplayEdit(&addr_edit[2], 0);
           memcpy(addr_edit[2].Text, LocalCfg.Addr + 7, 2);
           addr_edit[2].Text[2] = '\0';
           outxy16(addr_edit[2].xLeft + addr_edit[2].CursorX + addr_edit[2].BoxLen*addr_edit[2].fWidth,
                   addr_edit[2].yTop  + addr_edit[2].CursorY - 2, 1, cBlack, 1, 1, addr_edit[2].Text, 0, 0);
           addr_edit[2].BoxLen = 2;

           //房号
           DisplayEdit(&addr_edit[3], 0);
           memcpy(addr_edit[3].Text, LocalCfg.Addr + 9, 2);
           addr_edit[3].Text[2] = '\0';
           outxy16(addr_edit[3].xLeft + addr_edit[3].CursorX + addr_edit[3].BoxLen*addr_edit[3].fWidth,
                   addr_edit[3].yTop  + addr_edit[3].CursorY - 2, 1, cBlack, 1, 1, addr_edit[3].Text, 0, 0);
           addr_edit[3].BoxLen = 2;

           //设备号
           DisplayEdit(&addr_edit[4], 0);
           memcpy(addr_edit[4].Text, LocalCfg.Addr + 11, 1);
           addr_edit[4].Text[1] = '\0';
           outxy16(addr_edit[4].xLeft + addr_edit[4].CursorX + addr_edit[4].BoxLen*addr_edit[4].fWidth,
                   addr_edit[4].yTop  + addr_edit[4].CursorY - 2, 1, cBlack, 1, 1, addr_edit[4].Text, 0, 0);
           addr_edit[4].BoxLen = 1;
           break;
   case 2:
         DisplayEdit(&modi_engi_edit[0], 0);
         DisplayEdit(&modi_engi_edit[1], 0);
         break;
  }
 } 
}
//---------------------------------------------------------------------------
void DisplayIPWindow(void)    //显示IP地址设置窗口（三级）
{
  int i;
  char jpgfilename[80];
  
  Local.CurrentWindow = 161;
  DisplayIporAddr(0);

  for(i=0; i<3; i++)
   {
    if(i == 0)
      DisplayImageButton(&setup_button[i], IMAGEDOWN, 0);
    else
      DisplayImageButton(&setup_button[i], IMAGEUP, 0);
  }

  for(i=0; i<12; i++)
   {
    num2_button[i].xLeft = NumxLeft4[i];
    num2_button[i].yTop = NumyTop4[i];
    DisplayImageButton(&num2_button[i], IMAGEUP, 0);
   }

  CurrBox = 0;
  //打开光标
  WriteCursor(&ip_edit[0], 1, 1, 0);

}
//---------------------------------------------------------------------------
void OperateIPWindow(short wintype, int currwindow)  //IP地址设置操作（三级）
{
  char wavFile[80];
  int yTop;
  int i,j;
  char jpgfilename[80];
  char str[10];
  int hex_data[6];
  char TmpText[5][5];
  int input_ok;
  yTop = 36;

  if(wintype < 12)
   {
    DisplayImageButton(&num2_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&num2_button[wintype], IMAGEUP, 0);
   }
   
  if(((wintype >= 12) && (wintype <= 14)))
   {
    DisplayImageButton(&setup_button[wintype - 12], IMAGEUP, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&setup_button[wintype - 12], IMAGEDOWN, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }
  //停止光标,并清一下
  WriteCursor(&ip_edit[CurrBox], 0, 0, 0);
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
           switch(CurrBox)
            {
             case 0:
                    if(ip_edit[CurrBox].BoxLen < ip_edit[CurrBox].MaxLen)
                     {
                      if(wintype < 10)
                        str[0] = '0' + wintype;
                      else
                        str[0] = 'A' + wintype - 10;
                      str[1] = '\0';
                      // strcat(ip_edit.Text, str);
                      memcpy(ip_edit[CurrBox].Text + ip_edit[CurrBox].BoxLen, str, 1);
                      outxy16(ip_edit[CurrBox].xLeft + ip_edit[CurrBox].CursorX + ip_edit[CurrBox].BoxLen*ip_edit[CurrBox].fWidth,
                              ip_edit[CurrBox].yTop  + ip_edit[0].CursorY - 2, 1, cBlack, 1, 1, str, 0, 0);
                      ip_edit[CurrBox].BoxLen ++;
                      if(ip_edit[CurrBox].Text[ip_edit[CurrBox].BoxLen] == ':')
                        ip_edit[CurrBox].BoxLen ++;
                     }
                    break;
             case 1:
             case 2:
             case 3:
             case 4:
                    if(ip_edit[CurrBox].BoxLen < ip_edit[CurrBox].MaxLen)
                     {
                      str[0] = '0' + wintype;
                      str[1] = '\0';
                      // strcat(ip_edit.Text, str);
                      memcpy(ip_edit[CurrBox].Text + ip_edit[CurrBox].BoxLen, str, 1);
                      outxy16(ip_edit[CurrBox].xLeft + ip_edit[CurrBox].CursorX + ip_edit[CurrBox].BoxLen*ip_edit[CurrBox].fWidth,
                              ip_edit[CurrBox].yTop  + ip_edit[0].CursorY - 2, 1, cBlack, 1, 1, str, 0, 0);
                      ip_edit[CurrBox].BoxLen ++;
                      if(ip_edit[CurrBox].Text[ip_edit[CurrBox].BoxLen] == '.')
                        ip_edit[CurrBox].BoxLen ++;
                     }
                    break;
            }
           break;
    case 10: //清除        L
          ip_edit[CurrBox].Text[0] = 0;
          ip_edit[CurrBox].BoxLen = 0;
          DisplayEdit(&ip_edit[CurrBox], 0);
           switch(CurrBox)
            {
             case 0:
                    strcpy(ip_edit[CurrBox].Text,  "  :  :  ");
                    break;
             case 1:
             case 2:
             case 3:
             case 4:
                    strcpy(ip_edit[CurrBox].Text, "   .   .   .   ");
                    break;
            }
          outxy16(ip_edit[CurrBox].xLeft + ip_edit[CurrBox].CursorX + ip_edit[CurrBox].BoxLen*ip_edit[CurrBox].fWidth,
                  ip_edit[CurrBox].yTop  + ip_edit[0].CursorY - 2, 0, cBlack, 1, 1, ip_edit[CurrBox].Text, 0, 0);
          break;
    case 11: //确定   K
          input_ok = 1;
          if(ip_edit[0].BoxLen < (ip_edit[0].MaxLen - 1))
           {
             usleep(200*1000);
             sprintf(wavFile, "%smodifail.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
            input_ok = 0;
            break;
           }
          if(input_ok == 1)
           for(i=1; i<5; i++)
            if(ip_edit[i].BoxLen < (ip_edit[i].MaxLen - 2))
             {
              usleep(200*1000);
              sprintf(wavFile, "%smodifail.wav\0", wavPath);
              StartPlayWav(wavFile, 0);
              CurrBox = i;
              input_ok = 0;
              break;
             }

          if(input_ok == 1)
           {
            memcpy(TmpText[0], ip_edit[0].Text, 2);
            TmpText[0][2] = '\0';
            memcpy(TmpText[1], ip_edit[0].Text + 3, 2);
            TmpText[1][2] = '\0';
            memcpy(TmpText[2], ip_edit[0].Text + 6, 2);
            TmpText[2][2] = '\0';

            LocalCfg.Mac_Addr[0] = 0x00;
            LocalCfg.Mac_Addr[1] = 0x50;
            LocalCfg.Mac_Addr[2] = 0x2A;
            for(i=0; i<3; i++)
             if(strlen(TmpText[i]) != 2)
              {
               strcpy(str, TmpText[i]);
               strcpy(TmpText[i], "0");
               strcat(TmpText[i], str);
               TmpText[i][2] = '\0';
              }
            for(i=0; i<3; i++)
             {
              sscanf(TmpText[i], "%x", &hex_data[i]);
              LocalCfg.Mac_Addr[i+3] = hex_data[i];
             }

            for(j=1; j<5; j++)
             {
              memcpy(TmpText[0], ip_edit[j].Text, 3);
              TmpText[0][3] = '\0';
              memcpy(TmpText[1], ip_edit[j].Text + 4, 3);
              TmpText[1][3] = '\0';
              memcpy(TmpText[2], ip_edit[j].Text + 8, 3);
              TmpText[2][3] = '\0';
              memcpy(TmpText[3], ip_edit[j].Text + 12, 3);
              TmpText[3][3] = '\0';

              switch(j)
               {
                case 1:
                       for(i=0; i<4; i++)
                         LocalCfg.IP[i] = atoi(TmpText[i]);
                       break;
                case 2:
                       for(i=0; i<4; i++)
                         LocalCfg.IP_Mask[i] = atoi(TmpText[i]);
                       break;
                case 3:
                       for(i=0; i<4; i++)
                         LocalCfg.IP_Gate[i] = atoi(TmpText[i]);
                       break;
                case 4:
                       for(i=0; i<4; i++)
                         LocalCfg.IP_Server[i] = atoi(TmpText[i]);
                       break;
               }
             }

            //广播地址
            for(i=0; i<4; i++)
             {
              if(LocalCfg.IP_Mask[i] != 0)
                LocalCfg.IP_Broadcast[i] = LocalCfg.IP_Mask[i] & LocalCfg.IP[i];
              else
                LocalCfg.IP_Broadcast[i] = 0xFF;
             }
            printf("eth0 %d.%d.%d.%d \0",
                             LocalCfg.IP[0], LocalCfg.IP[1], LocalCfg.IP[2], LocalCfg.IP[3]);
            SaveToFlash(4);    //向Flash中存储文件

            RefreshNetSetup(1); //刷新网络设置
             sprintf(wavFile, "%smodisucc.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
            SendFreeArp();
           }
          break;
    case 12:
       //   DisplayIPWindow();  //显示网络设置
          break;
    case 13:
          DisplayAddressWindow();  //显示房号设置
          break;
    case 14:
          DisplayModiPassWindow(0);  //修改工程密码窗口
          break;
    case 16:  //IP1
    case 17:  //IP2
    case 18:  //IP3
    case 19:  //IP4
    case 20:  //IP4
           CurrBox = wintype - 16;
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
  if(Local.CurrentWindow == 161)
    //显示光标
    WriteCursor(&ip_edit[CurrBox], 1, 1, 0);
}
//---------------------------------------------------------------------------
void DisplayAddressWindow(void)    //显示房号设置窗口（三级）
{
  int i;
  char jpgfilename[80];

  DisplayIporAddr(1);

  for(i=0; i<3; i++)
   {
    if(i == 1)
      DisplayImageButton(&setup_button[i], IMAGEDOWN, 0);
    else
      DisplayImageButton(&setup_button[i], IMAGEUP, 0);
  }

  for(i=0; i<12; i++)
   {
    num2_button[i].xLeft = NumxLeft4[i];
    num2_button[i].yTop = NumyTop4[i];
    DisplayImageButton(&num2_button[i], IMAGEUP, 0);
   }

  CurrBox = 0;
  //打开光标
  WriteCursor(&addr_edit[0], 1, 1, 0);
  Local.CurrentWindow = 159;
}
//---------------------------------------------------------------------------
void OperateAddressWindow(short wintype, int currwindow)  //房号设置操作（三级）
{
  char wavFile[80];
  int yTop;
  int i;
  char jpgfilename[80];
  char str[10];
  yTop = 36;

  if(wintype < 12)
   {
    DisplayImageButton(&num2_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&num2_button[wintype], IMAGEUP, 0);
   }

  if(((wintype >= 12) && (wintype <= 14)))
   {
    DisplayImageButton(&setup_button[wintype - 12], IMAGEUP, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&setup_button[wintype - 12], IMAGEDOWN, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }   
  if(wintype == 15)
   {
    DisplayImageButton(&setup_button[wintype - 12], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&setup_button[wintype - 12], IMAGEUP, 0);
   }

  //停止光标,并清一下
  WriteCursor(&addr_edit[CurrBox], 0, 0, 0);
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
           if(addr_edit[CurrBox].BoxLen < addr_edit[CurrBox].MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
            // strcat(ip_edit.Text, str);
             memcpy(addr_edit[CurrBox].Text + addr_edit[CurrBox].BoxLen, str, 1);
             outxy16(addr_edit[CurrBox].xLeft + addr_edit[CurrBox].CursorX + addr_edit[CurrBox].BoxLen*addr_edit[CurrBox].fWidth,
                     addr_edit[CurrBox].yTop  + addr_edit[0].CursorY - 2, 1, cBlack, 1, 1, str, 0, 0);
             addr_edit[CurrBox].BoxLen ++;
            }
           break;
    case 10: //清除        L
          addr_edit[CurrBox].Text[0] = 0;
          addr_edit[CurrBox].BoxLen = 0;
          DisplayEdit(&addr_edit[CurrBox], 0);
          break;
    case 11: //确定   K
          if((addr_edit[0].BoxLen == 4))//&&(addr_edit[1].BoxLen == 2)&&(addr_edit[2].BoxLen == 2)&&(addr_edit[3].BoxLen == 2))
           {    
            if((addr_edit[1].BoxLen != 0)||(addr_edit[2].BoxLen != 0)||(addr_edit[3].BoxLen != 0))
             {
              for(i = 1; i <= 3; i++)
               {
                switch(addr_edit[i].BoxLen)
                 {
                  case 0:
                         strcpy(addr_edit[i].Text, "00");
                         break;
                  case 1:
                         strcpy(str, addr_edit[i].Text);
                         strcpy(addr_edit[i].Text, "0");
                         strcat(addr_edit[i].Text, str);
                         break;
                  case 2:
                         break;
                 }
                DisplayEdit(&addr_edit[i], 0);
                outxy16(addr_edit[i].xLeft  + addr_edit[i].CursorX,
                          addr_edit[i].yTop  + addr_edit[0].CursorY - 2 , 1, cBlack, 1, 1, addr_edit[i].Text, 0, 0);
                addr_edit[i].BoxLen = 2;
               }
             }
            LocalCfg.Addr[0] == 'S';
            memcpy(LocalCfg.Addr + 1, addr_edit[0].Text, 4);
            memcpy(LocalCfg.Addr + 5, addr_edit[1].Text, 2);
            memcpy(LocalCfg.Addr + 7, addr_edit[2].Text, 2);
            memcpy(LocalCfg.Addr + 9, addr_edit[3].Text, 2);
            LocalCfg.Addr[11] == addr_edit[4].Text[0];

            SaveToFlash(4);    //向Flash中存储文件
            
            //主机
            Local.isHost = '0';

            if(LocalCfg.Addr[0] == 'S')
             {
              Local.isHost = LocalCfg.Addr[11];
             }
            if(LocalCfg.Addr[0] == 'B')
             {
              Local.isHost = LocalCfg.Addr[5];
             }
            Local.DenNum = 0;

             usleep(200*1000);
             sprintf(wavFile, "%smodisucc.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
           }
          else
           {
             usleep(200*1000);
             sprintf(wavFile, "%smodifail.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
            CurrBox = 0;
           }           
          break;

    case 12:
          DisplayIPWindow();  //显示网络设置
          break;
    case 13:
   //       DisplayAddressWindow();  //显示房号设置
          break;
    case 14:
          DisplayModiPassWindow(0);  //修改工程密码窗口
          break;
    case 16:  //幢号
    case 17:  //单元号
    case 18:  //层号
    case 19:  //房号
    case 20:  //设备号
           CurrBox = wintype - 16;
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
  if(Local.CurrentWindow == 159)
    //显示光标
    WriteCursor(&addr_edit[CurrBox], 1, 1, 0);
}
//---------------------------------------------------------------------------
void DisplayModiPassWindow(int passtype)   //显示修改密码窗口（三级、四级） 0 工程密码 1 使用者密码
{
  int i;

  DisplayIporAddr(2);

  for(i=0; i<3; i++)
   {
    if(i == 2)
      DisplayImageButton(&setup_button[i], IMAGEDOWN, 0);
    else
      DisplayImageButton(&setup_button[i], IMAGEUP, 0);
  }

  for(i=0; i<12; i++)
   {
    num2_button[i].xLeft = NumxLeft4[i];
    num2_button[i].yTop = NumyTop4[i];
    DisplayImageButton(&num2_button[i], IMAGEUP, 0);
   }
    
  CurrBox = 0;
  //打开光标
  WriteCursor(&modi_engi_edit[0], 1, 1, 0);
  Local.CurrentWindow = 156;
  Local.MaxIndex = 2;
  Local.MenuIndex = 0;

  for(i=0; i<2; i++)
    DisplayImageButton(&modi_engi_button[i], IMAGEUP, 0);
}
//---------------------------------------------------------------------------
void OperateModiPassWindow(short wintype, int currwindow)  //修改密码操作（三级、四级）
{
  char wavFile[80];
  int i;
  char jpgfilename[80];
  char str[10];

  if(wintype < 12)
   {
    DisplayImageButton(&num2_button[wintype], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&num2_button[wintype], IMAGEUP, 0);
   }
     
  if(((wintype >= 12) && (wintype <= 14)))
   {
    DisplayImageButton(&setup_button[wintype - 12], IMAGEUP, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&setup_button[wintype - 12], IMAGEDOWN, 0);
   }
  if(wintype >= 30)
   {
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&bigmenu_button[wintype-30], IMAGEUP, 0);
   }
  if(((wintype >= 16) && (wintype <= 17)))
   {
    DisplayImageButton(&modi_engi_button[wintype - 16], IMAGEDOWN, 0);
    usleep(DELAYTIME*1000);
    if(Local.CurrentWindow != currwindow)
      return;
    DisplayImageButton(&modi_engi_button[wintype - 16], IMAGEUP, 0);
   }
  //停止光标,并清一下
  WriteCursor(&modi_engi_edit[CurrBox], 0, 0, 0);
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
           if(modi_engi_edit[CurrBox].BoxLen < modi_engi_edit[CurrBox].MaxLen)
            {
             str[0] = '0' + wintype;
             str[1] = '\0';
             strcat(modi_engi_edit[CurrBox].Text, str);

             str[0] = '*';
             str[1] = '\0';
             outxy16(modi_engi_edit[CurrBox].xLeft + modi_engi_edit[CurrBox].CursorX + modi_engi_edit[CurrBox].BoxLen*modi_engi_edit[CurrBox].fWidth,
                     modi_engi_edit[CurrBox].yTop  + modi_engi_edit[0].CursorY - 2, 3, cBlack, 1, 1, str, 0, 0);
             modi_engi_edit[CurrBox].BoxLen ++;
             if(modi_engi_edit[CurrBox].BoxLen >= modi_engi_edit[CurrBox].MaxLen)
              {
               CurrBox ++;
               if(CurrBox > 1)
                {
                 CurrBox = 1;
                }
              }
            }
           break;
    case 10: //清除        L
    case 17: //清除        L
          modi_engi_edit[CurrBox].Text[0] = 0;
          modi_engi_edit[CurrBox].BoxLen = 0;
          DisplayEdit(&modi_engi_edit[CurrBox], 0);
          break;
    case 11: //确定   K
    case 16:  //确定
          if(strcmp(modi_engi_edit[0].Text, modi_engi_edit[1].Text) == 0)
           {
            strcpy(LocalCfg.EngineerPass, modi_engi_edit[0].Text);
            SaveToFlash(4);    //向Flash中存储文件

             sprintf(wavFile, "%smodisucc.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
           }
          else
           {
             sprintf(wavFile, "%smodifail.wav\0", wavPath);
             StartPlayWav(wavFile, 0);
           }

          //清除输入框
          modi_engi_edit[0].Text[0] = 0;
          modi_engi_edit[0].BoxLen = 0;
          DisplayEdit(&modi_engi_edit[0], 0);
          modi_engi_edit[1].Text[0] = 0;
          modi_engi_edit[1].BoxLen = 0;
          DisplayEdit(&modi_engi_edit[1], 0);
          CurrBox = 0;
          break;
    case 12:
          DisplayIPWindow();  //显示网络设置
          break;
    case 13:
          DisplayAddressWindow();  //显示房号设置
          break;
    case 14:
   //       DisplayModiPassWindow(0);  //修改工程密码窗口
          break;
    case 18:  //密码1
    case 19:  //密码2
           CurrBox = wintype - 18;
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
  if((Local.CurrentWindow == 156)||(Local.CurrentWindow == 192))
    //显示光标
    WriteCursor(&modi_engi_edit[CurrBox], 1, 1, 0);
}
//---------------------------------------------------------------------------
void SaveToFlash(int savetype)    //向Flash中存储文件
{
  int i;
  //写入文件
  //锁定互斥锁
  pthread_mutex_lock (&Local.save_lock);
  printf("to save thread\n");
  //查找可用存储缓冲并填空
  for(i=0; i<SAVEMAX; i++)
   if(Save_File_Buff[i].isValid == 0)
    {
     Save_File_Buff[i].Type = savetype;      //存储本地设置
     Save_File_Buff[i].isValid = 1;
     sem_post(&save_file_sem);     
     break;
    }
  //打开互斥锁
  pthread_mutex_unlock (&Local.save_lock);
}
//---------------------------------------------------------------------------
void DisplayCalibrateWindow(void)    //校准触摸屏窗口（二级）
{
  int i,j;
  char str[10];
  char jpgfilename[80];
//  strcpy(jpgfilename, sPath);
//  strcat(jpgfilename,"white.jpg");
//  DisplayJPG(0, 0, jpgfilename, 1, SCRWIDTH, SCRHEIGHT, 3);
  memset(fbmem + f_data.buf_len*3, 235, f_data.uv_offset);
  memset(fbmem + f_data.buf_len*3 + f_data.uv_offset, 128, f_data.uv_offset/4);
  memset(fbmem + f_data.buf_len*3 + f_data.uv_offset*5/4, 128, f_data.uv_offset/4);

  outxy24(200, 200, 3, cBlack, 1, 1, "请点击十字交叉点", 0, 3);
  Local.CalibratePos = 0;
  if((SCRWIDTH == 800)&&(SCRHEIGHT == 600))
   {
    CrossPosY[2] = 550;
    CrossPosY[3] = 550;
   }

  WriteCross(CrossPosX[Local.CalibratePos], CrossPosY[Local.CalibratePos], 1, 3);
  if(Local.CurrentWindow != 191)
    Local.TmpWindow = Local.CurrentWindow;
  Local.CurrentWindow = 190;
//  Local.CalibrateNum ++;  //校准次数
  //设置FB页面
  if(Local.CurrFbPage != 3)
   {
    Local.CurrFbPage = 3;
    fb_setpage(fbdev, Local.CurrFbPage);
   }
}
//---------------------------------------------------------------------------
void OperateCalibrateWindow(short wintype, int currwindow)  //校准触摸屏操作（二级）
{
  int i;
  int X0,Y0,XA,YA,XB,YB,XC,YC,XD,YD,deltaX,deltaY;
  int Stand_X0, Stand_Y0, Stand_deltaX, Stand_deltaY;
  int RefCalib_X[4], RefCalib_Y[4];
  Stand_X0 = 491;
  Stand_Y0 = 499;
  Stand_deltaX = -830;
  Stand_deltaY = 683;

  WriteCross(CrossPosX[Local.CalibratePos], CrossPosY[Local.CalibratePos], 0, 3);
  Local.CalibratePos ++;
  if(Local.CalibratePos > 3)
   {
    XA = Calib_X[0];
    XB = Calib_X[1];
    XC = Calib_X[2];
    XD = Calib_X[3];
    YA = Calib_Y[0];
    YB = Calib_Y[1];
    YC = Calib_Y[2];
    YD = Calib_Y[3];
    X0=(XA + XB + XC +XD)/4;
    Y0=(YA + YB + YC +YD)/4;
    deltaX = (XB - XA + XD - XC)/2;
 //   deltaX = (XA - XC + XB - XD)/2;
    deltaX = deltaX *SCRWIDTH/(SCRWIDTH - 100);    //由于校准点不在原点
 //   deltaY = (YB - YA + YD - YC)/2;
    deltaY = (YC - YA + YD - YB)/2;
    deltaY = deltaY *SCRHEIGHT/(SCRHEIGHT - 100);
    printf("X0 = %d, Y0 = %d, deltaX = %d, deltaY = %d, 1 = %f,  2 = %f, delta1 = %f, delta2 = %f,\n",
           X0, Y0, deltaX, deltaY, fabs((X0 - Stand_X0)*1.0/(Stand_X0*1.0)),  fabs((Y0 - Stand_Y0)*1.0/(Stand_Y0*1.0)),
           fabs((deltaX - Stand_deltaX)*1.0/(Stand_deltaX*1.0)),  fabs((deltaY - Stand_deltaY)*1.0/(Stand_deltaY*1.0)));
    //如与标准值偏差太大
/*    if((fabs((X0 - Stand_X0)*1.0/(Stand_X0*1.0))<0.2)
       &&(fabs((Y0 - Stand_Y0)*1.0/(Stand_Y0*1.0))<0.2)
       &&(fabs((deltaX - Stand_deltaX)*1.0/(Stand_deltaX*1.0))<0.2)
       &&(fabs((deltaY - Stand_deltaY)*1.0/(Stand_deltaY*1.0))<0.2)) */
       {                                         
        LocalCfg.Ts_X0 = X0;
        LocalCfg.Ts_Y0 = Y0;
        LocalCfg.Ts_deltaX = deltaX;
        LocalCfg.Ts_deltaY = deltaY;

        SaveToFlash(4);    //向Flash中存储文件

        Local.CalibrateSucc = 1; 
             //设置FB页面
             if(Local.CurrFbPage != 0)
              {
               Local.CurrFbPage = 0;
               fb_setpage(fbdev, Local.CurrFbPage);
              }
             Local.CurrentWindow = Local.TmpWindow;
     //        Local.CalibrateNum = 0;
       }
   }
  else
    WriteCross(CrossPosX[Local.CalibratePos], CrossPosY[Local.CalibratePos], 1, 3);
}
//---------------------------------------------------------------------------
//画十字函数   t_Flag 0--清除  1--写
void WriteCross(int xLeft, int yTop, int t_Flag, int PageNo)
{
 if(t_Flag == 1)
  {
   fb_line(xLeft - 20, yTop, xLeft + 20, yTop, cBlack, PageNo);
   fb_line(xLeft - 20, yTop + 1, xLeft + 20, yTop + 1, cBlack, PageNo);
   fb_line(xLeft, yTop - 20, xLeft, yTop + 20, cBlack, PageNo);
   fb_line(xLeft + 1, yTop - 20, xLeft + 1, yTop + 20, cBlack, PageNo);
  }
  else
  {
   fb_line(xLeft - 20, yTop, xLeft + 20, yTop, cWhite, PageNo);
   fb_line(xLeft - 20, yTop + 1, xLeft + 20, yTop + 1, cWhite, PageNo);
   fb_line(xLeft, yTop - 20, xLeft, yTop + 20, cWhite, PageNo);
   fb_line(xLeft + 1, yTop - 20, xLeft + 1, yTop + 20, cWhite, PageNo);
  }
}
//---------------------------------------------------------------------------
#endif

