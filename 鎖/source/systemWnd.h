#ifndef _MYSYSTEMWND
#define _MYSYSTEMWND

#include "graphic.h"
#include "escript.h"

enum Map_State{
	map_fadein = 0,
	map_fadeout,
	map_selectin,
	map_selectout,
	map_select,
	map_itemViewin,
	map_itemViewout,
	map_itemView,
	map_itemchangein,
	map_itemchangeout,
};

class MapWnd
{
	ColorBuf		baseCBuf;		
	ColorBuf		backupWnd;
	ColorBuf		mapSelectBuf;
	ColorBuf		mapFloorBuf;
	ColorBuf		floorSelect;
	ColorBuf		floorCBuf[8];
	ColorBuf		floorNameBuf[3];
	ColorBuf		FloorNamePrt;
	ColorBuf		mapAllFloor;
	ColorBuf		RoomNameBase;
	ColorBuf		RoomNameLine;
	ColorBuf		RoomName[5];
	ColorBuf		selectFloorPrt;
	ColorBuf		itemList[2];
	ColorBuf		itemList_s;
	ColorBuf		itemSelect;
	ColorBuf		itemRet;
	ColorBuf		itemInfoPtr;
	ColorBuf		retBtnPrt;
	MyButton		retBtn;
	Map_State		state;			
	exec_mode		bak_mode;
	int				alpha_time;
	float			alpha;
	float			map_alpha;
	short			floorNum;		
	short			lookFloor;		
	short			lastFloor;		
	short			selectFloor;	
	short			tmpfloorNum;	
	short			mainRoom;		
	short			subRoom;		
	short			itemNum;		
	short			oldItemNum;
	short			tmpItemNum;		
	short			itemMax;
	BYTE			btnSelect;
	BYTE			mapSelectCnt;
	RECT			mapSelectRect;
	int				itemTable[40];
	BYTE			fontColor[3];
public:
	MapWnd();
	~MapWnd(){
		int		i;
		baseCBuf.Release();
		backupWnd.Release();
		mapSelectBuf.Release();
		mapFloorBuf.Release();
		floorSelect.Release();
		mapAllFloor.Release();
		retBtnPrt.Release();
		FloorNamePrt.Release();
		RoomNameBase.Release();
		RoomNameLine.Release();
		selectFloorPrt.Release();
		itemList_s.Release();
		itemSelect.Release();
		itemInfoPtr.Release();
		itemRet.Release();
		for(i=0;i<2;i++)itemList[i].Release();
		for(i=0;i<8;i++)floorCBuf[i].Release();
		for(i=0;i<3;i++)floorNameBuf[i].Release();
		for(i=0;i<5;i++)RoomName[i].Release();
	}
	BOOL Exec(void);
	void SelectItem(void);
	void Draw(void);
	void CreateItemList(void);
	void CreateItemInfo(void);
	void SetAlpha(void){
		int		i;
		baseCBuf.alpha = itemList_s.alpha = itemSelect.alpha = itemRet.alpha = retBtnPrt.alpha = alpha;
		FloorNamePrt.alpha = RoomNameBase.alpha = RoomNameLine.alpha = alpha;
		for(i=0;i<5;i++)RoomName[i].alpha = alpha;
		mapSelectBuf.alpha = selectFloorPrt.alpha = alpha *map_alpha;
		itemList[0].alpha = itemList[1].alpha = itemInfoPtr.alpha = 1.0f -mapSelectBuf.alpha;
	}
	int SelectFloor(){
		RECT floorRect = {412,64,412+290,64+220};
		if(!myPtInRect(&floorRect,&sysInf.msPoint))return -1;
		int ret = floorSelect.pBuf[floorSelect.wPitch*(sysInf.msPoint.y-64) +(sysInf.msPoint.x-412)];
		if(ret==selectFloor || ret>=5)return -1;
		return ret;
	}
};

class OptionMode
{
	float			alpha;
	exec_mode		bak_execMode;
	BYTE			state;
	BYTE			meterCnt;
	int				fadeTime;
	BYTE			selectBtnNum;

	ColorBuf		baseCBuf;
	ColorBuf		retBtnPrt;
	ColorBuf		initBtnPrt;
	ColorBuf		screenBtnPrt;
	ColorBuf		voiceSetBtnPrt;
	ColorBuf		checkBtnPrt;
	ColorBuf		charnamePrt;
	ColorBuf		meterPrt;
	ColorBuf		confirmWnd;
	ColorBuf		yesNoWnd;
	ColorBuf		blackWnd;
	ColorBuf		backupWnd;

	MyButton		retBtn;
	MyButton		initBtn;
	MyButton		voiceSetBtn[4];
	MyButton		checkBtn[16];
	MyButton		yesnoBtn[2];

public:
	MyButton		screenBtn[2];
	OptionMode();
	~OptionMode(){
		baseCBuf.Release();
		retBtnPrt.Release();
		initBtnPrt.Release();
		screenBtnPrt.Release();
		voiceSetBtnPrt.Release();
		checkBtnPrt.Release();
		charnamePrt.Release();
		meterPrt.Release();
		confirmWnd.Release();
		yesNoWnd.Release();
		blackWnd.Release();
		backupWnd.Release();
	}
	void SetAlpha(){
		baseCBuf.alpha = retBtnPrt.alpha = initBtnPrt.alpha = screenBtnPrt.alpha = voiceSetBtnPrt.alpha = checkBtnPrt.alpha = charnamePrt.alpha = meterPrt.alpha = alpha;
	}

	BOOL Exec(void);
	void Draw(void);
};

class MusicModeWnd
{
	int				alpha_time;
	int				btn_time;
	float			alpha;
	exec_mode		bak_mode;
	BYTE			state;
	BYTE			selectNum;
	BYTE			oldMusic;
	BYTE			stopNum;
	BYTE			btnSelect;

	ColorBuf		backupWnd;
	ColorBuf		baseCBuf;
	ColorBuf		musicTitlePrt;
	ColorBuf		musicBtnPrt;
	ColorBuf		musicTitle2;
	ColorBuf		musicInfoPrt[2];
	ColorBuf		musicSelect;
	ColorBuf		retBtnPrt;
	MyButton		musicBtn[21];
	MyButton		retBtn;
public:
	MusicModeWnd();
	~MusicModeWnd(){
		backupWnd.Release();
		baseCBuf.Release();
		musicTitlePrt.Release();
		musicBtnPrt.Release();
		musicTitle2.Release();
		musicSelect.Release();
		retBtnPrt.Release();
		musicInfoPrt[0].Release();
		musicInfoPrt[1].Release();
	}
	BOOL Exec(void);
	void Draw(void);
	void SetAlpha(void){
		baseCBuf.alpha = musicTitlePrt.alpha = musicBtnPrt.alpha = musicTitle2.alpha = 
			musicSelect.alpha = musicInfoPrt[0].alpha = musicInfoPrt[1].alpha = retBtnPrt.alpha = alpha;
	}
};

class TitleWnd
{
	ColorBuf		baseCBuf;		
	ColorBuf		logoCBuf[2];	
	ColorBuf		charCBuf;
	ColorBuf		crCBuf;
	ColorBuf		btnCBuf[7];		
	MyButton		btn[7];
	BYTE			state;			
	short			selectNum;		
	int				timer;
public:
	TitleWnd();
	~TitleWnd(){
		int	i;
		baseCBuf.Release();
		for(i=0;i<2;i++) logoCBuf[i].Release();
		charCBuf.Release();
		crCBuf.Release();
		for(i=0;i<7;i++) btnCBuf[i].Release();
		MessageMenuSet( off , TRUE );
	}
	BOOL Exec(void);
	void Draw(void);
	void SetStartCursor(void);
};


extern TitleWnd		*titleWnd;

enum cgmode_state{
	cgmode_allfadein = 0,
	cgmode_allfadeout,
	cgmode_charSelect,
	cgmode_Thumbnail_fadein,
	cgmode_Thumbnail_fadeout,
	cgmode_Thumbnail_fadein2,
	cgmode_Thumbnail_fadeout2,
	cgmode_Thumbnail,
	cgmode_cgfadein,
	cgmode_cgview,
	cgmode_cgfadeout,
	cgmode_cgcrossfade,
};

class CG_ModeWnd
{
	int				alpha_time;
	float			alpha;
	float			char_alpha_org[2];
	int				selectTime;
	exec_mode		mode;
	exec_mode		bak_mode;
	BYTE			state;
	BYTE			charNum;
	BYTE			prevChar;
	BYTE			selectNum;
	BYTE			offsetNum;
	BYTE			subNum;
	BYTE			selectMax;
	BYTE			pageMax;
	BYTE			btnSelect;
	BYTE			bCgOpen[12];

	ColorBuf		backupWnd;
	ColorBuf		baseCBuf;		
	ColorBuf		mainWnd;		
	ColorBuf		cgWnd;
	ColorBuf		cgWndOld;
	ColorBuf		titlePrt;
	ColorBuf		numParts;
	ColorBuf		sabunNumPrt;
	ColorBuf		charSelectPrt[7];
	ColorBuf		charnamePrt;
	ColorBuf		charSelect;
	ColorBuf		thumbnailPrt[14];
	ColorBuf		retBtnPrt;
	ColorBuf		pagebutton;

	MyButton		btn[2];
	MyButton		retBtn;
public:
	CG_ModeWnd(exec_mode s_mode);
	~CG_ModeWnd(){
		ESC_InitFlag();
		if(lpSoundDS && titleWnd){
			lpSoundDS->ChangeMusic(bgmHandle,2,TRUE,0);
		}
		backupWnd.Release();
		baseCBuf.Release();
		mainWnd.Release();
		cgWnd.Release();
		titlePrt.Release();
		numParts.Release();
		sabunNumPrt.Release();
		charnamePrt.Release();
		charSelect.Release();
		retBtnPrt.Release();
		pagebutton.Release();
		for(int i=0;i<14;i++)thumbnailPrt[i].Release();
		for(i=0;i<7;i++)charSelectPrt[i].Release();
	}
	BOOL Exec(void);
	void CreateThumbnail(void);
	void Draw(void);
	void SetAlpha(void){
		baseCBuf.alpha = mainWnd.alpha = titlePrt.alpha = numParts.alpha = pagebutton.alpha = alpha;
		for(int i=0;i<14;i++) thumbnailPrt[i].alpha = alpha;
	}
	int SelectChar(){
		if(sysInf.msPoint.x<0 || sysInf.msPoint.y<0 || sysInf.msPoint.x>=WIN_SIZEX || sysInf.msPoint.y>=WIN_SIZEY)return 0xff;
		return charSelect.pBuf[charSelect.wPitch*sysInf.msPoint.y +sysInf.msPoint.x];
	}
	exec_mode GetMode(void){return mode; }
};

class RudderWnd
{
	BYTE		state;
	float		alpha;
	SHORT		animeCnt;
	SHORT		animeCnt_bak;
	int			startTime;
	ColorBuf	rudderPrt;
	ColorBuf	infoPrt;

public:
	RudderWnd(){
		ZeroMemory(this,sizeof(RudderWnd));
		rudderPrt.loadLGF(pack_gparts,"darin");
		infoPrt.loadLGF(pack_gparts,"rudderInfo");
		rudderPrt.alpha = 0.5f;
		infoPrt.alpha = 0;
	}
	~RudderWnd(){
		rudderPrt.Release();
		infoPrt.Release();
	}
	BOOL Exec(void);
	void Draw(void);
};

extern OptionMode	*optionWnd;
extern MusicModeWnd *musicWnd;
extern CG_ModeWnd	*cgmodeWnd;
extern RudderWnd	*rudderWnd;
extern MapWnd		*mapWnd;

#endif _MYSYSTEMWND
